#include <engine/pch.h>

#include <engine/imgui_helper.h>
#include <engine/window.h>
#include <render_core/i_render_context.h>
#include <render_core/i_camera.h>

namespace cgt
{
ImGuiHelper::ImGuiHelper(Window& window, render::IRenderContext& render)
{
    ImGui::CreateContext();

    // TODO: is this the best way to do this?
#ifdef WIN32
    ImGui_ImplSDL2_InitForD3D(window.GetSDLWindow());
#else
#error "Platform not supported!"
#endif

    render.ImGuiBindingsInit();
    render.Im3dBindingsInit();
}

void ImGuiHelper::RenderIm3dText(const render::ICamera& camera)
{
    //
    // NOTE: what follows is more or less a literal translation of the text rendering function from Im3d examples
    //

    BeginInvisibleFullscreenWindow();

    ImDrawList* imDrawList = ImGui::GetWindowDrawList();
    const glm::mat4 viewProj = camera.GetViewProjection();
    for (u32 i = 0; i < Im3d::GetTextDrawListCount(); ++i)
    {
        const Im3d::TextDrawList& textDrawList = Im3d::GetTextDrawLists()[i];

        for (u32 j = 0; j < textDrawList.m_textDataCount; ++j)
        {
            const Im3d::TextData& textData = textDrawList.m_textData[j];
            if (textData.m_positionSize.w == 0.0f || textData.m_color.getA() == 0.0f)
            {
                continue;
            }

            // Project world -> screen space.
            glm::vec4 clip = viewProj * glm::vec4(
                textData.m_positionSize.x,
                textData.m_positionSize.y,
                textData.m_positionSize.z,
                1.0f);
            glm::vec2 screen = glm::vec2(clip.x / clip.w, clip.y / clip.w);

            // Cull text which falls offscreen. Note that this doesn't take into account text size but works well enough in practice.
            if (clip.w < 0.0f || screen.x >= 1.0f || screen.y >= 1.0f)
            {
                continue;
            }

            // Pixel coordinates for the ImGuiWindow ImGui.
            screen = screen * glm::vec2(0.5f) + glm::vec2(0.5f);
            screen.y = 1.0f - screen.y; // screen space origin is reversed by the projection.
            auto imguiWindowSize = ImGui::GetWindowSize();
            screen = screen * glm::vec2(imguiWindowSize.x, imguiWindowSize.y);

            // All text data is stored in a single buffer; each textData instance has an offset into this buffer.
            const char* text = textDrawList.m_textBuffer + textData.m_textBufferOffset;

            // Calculate the final text size in pixels to apply alignment flags correctly.
            ImGui::SetWindowFontScale(textData.m_positionSize.w);
            // NB no CalcTextSize API which takes a font/size directly...
            auto imguiTextSize = ImGui::CalcTextSize(text, text + textData.m_textLength);
            glm::vec2 textSize = glm::vec2(imguiTextSize.x, imguiTextSize.y);
            ImGui::SetWindowFontScale(1.0f);

            // Generate a pixel offset based on text flags.
            glm::vec2 textOffset = glm::vec2(-textSize.x * 0.5f, -textSize.y * 0.5f); // default to center
            if ((textData.m_flags & Im3d::TextFlags_AlignLeft) != 0)
            {
                textOffset.x = -textSize.x;
            }
            else if ((textData.m_flags & Im3d::TextFlags_AlignRight) != 0)
            {
                textOffset.x = 0.0f;
            }

            if ((textData.m_flags & Im3d::TextFlags_AlignTop) != 0)
            {
                textOffset.y = -textSize.y;
            }
            else if ((textData.m_flags & Im3d::TextFlags_AlignBottom) != 0)
            {
                textOffset.y = 0.0f;
            }

            // Add text to the window draw list.
            screen = screen + textOffset;
            imDrawList->AddText(
                nullptr,
                textData.m_positionSize.w * ImGui::GetFontSize(),
                ImVec2(screen.x, screen.y),
                textData.m_color.getABGR(),
                text,
                text + textData.m_textLength);
        }
    }

    EndInvisibleFullscreenWindow();
}

void ImGuiHelper::Shutdown(render::IRenderContext& render)
{
    render.Im3dBindingsShutdown();
    render.ImGuiBindingsShutdown();
}

void ImGuiHelper::NewFrame(Window& window, render::IRenderContext& render, const render::ICamera& camera, float deltaTime)
{
    m_WindowDimensions = window.GetDimensions();

    render.ImGuiBindingsNewFrame();
    ImGui_ImplSDL2_NewFrame(window.GetSDLWindow());
    ImGui::NewFrame();

    Im3d::AppData& ad = Im3d::GetAppData();
    ad.m_deltaTime = deltaTime;
    ad.m_viewportSize = Im3d::Vec2(m_WindowDimensions.x, m_WindowDimensions.y);
    ad.m_viewOrigin = camera.GetPosition(); // for VR use the head position
    ad.m_viewDirection = camera.GetForwardDirection();
    ad.m_worldUp = camera.GetUpDirection(); // used internally for generating orthonormal bases
    ad.m_projOrtho = camera.IsOrthographic();

    // m_projScaleY controls how gizmos are scaled in world space to maintain a constant screen height
    // TODO: support perspective projection?
    CGT_ASSERT_ALWAYS_MSG(camera.IsOrthographic(), "Perspective projection is not supported!");
    ad.m_projScaleY = 2.0f / camera.GetProjection()[1][1]; // use far plane height for an ortho projection

    // World space cursor ray from mouse position; for VR this might be the position/orientation of the HMD or a tracked controller.
    int mouseX, mouseY;
    auto mouseState = SDL_GetMouseState(&mouseX, &mouseY);
    glm::vec2 cursorPos(mouseX, mouseY);
    cursorPos = (cursorPos / glm::vec2(ad.m_viewportSize.x, ad.m_viewportSize.y)) * 2.0f - 1.0f;
    cursorPos.y = -cursorPos.y; // window origin is top-left, ndc is bottom-left
    glm::vec3 rayOrigin, rayDirection;

    rayOrigin.x = cursorPos.x / camera.GetProjection()[0][0];
    rayOrigin.y = cursorPos.y / camera.GetProjection()[1][1];
    rayOrigin.z = 0.0f;
    rayOrigin = camera.GetView() * glm::vec4(rayOrigin, 1.0f);
    rayDirection = camera.GetView() * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);

    ad.m_cursorRayOrigin = rayOrigin;
    ad.m_cursorRayDirection = rayDirection;

    // Set cull frustum planes. This is only required if IM3D_CULL_GIZMOS or IM3D_CULL_PRIMTIIVES is enable via
    // im3d_config.h, or if any of the IsVisible() functions are called.
    ad.setCullFrustum(camera.GetViewProjection(), true);

    // Fill the key state array; using GetAsyncKeyState here but this could equally well be done via the window proc.
    // All key states have an equivalent (and more descriptive) 'Action_' enum.
    ad.m_keyDown[Im3d::Mouse_Left/*Im3d::Action_Select*/] = (mouseState & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0;

    auto keyboard = SDL_GetKeyboardState(nullptr);
    // The following key states control which gizmo to use for the generic Gizmo() function. Here using the left ctrl
    // key as an additional predicate.
    bool ctrlDown = keyboard[SDL_SCANCODE_LCTRL];
    ad.m_keyDown[Im3d::Key_L/*Action_GizmoLocal*/] = ctrlDown && keyboard[SDL_SCANCODE_L];
    ad.m_keyDown[Im3d::Key_T/*Action_GizmoTranslation*/] = ctrlDown && keyboard[SDL_SCANCODE_T];
    ad.m_keyDown[Im3d::Key_R/*Action_GizmoRotation*/] = ctrlDown && keyboard[SDL_SCANCODE_R];
    ad.m_keyDown[Im3d::Key_S/*Action_GizmoScale*/] = ctrlDown && keyboard[SDL_SCANCODE_S];

    // Enable gizmo snapping by setting the translation/rotation/scale increments to be > 0
    ad.m_snapTranslation = ctrlDown ? 0.1f : 0.0f;
    ad.m_snapRotation = ctrlDown ? glm::radians(30.0f) : 0.0f;
    ad.m_snapScale = ctrlDown ? 0.5f : 0.0f;

    Im3d::NewFrame();
}

void ImGuiHelper::RenderUi(render::IRenderContext& render, const render::ICamera& camera, glm::uvec2 windowDimensions)
{
    render.Im3dBindingsRender(camera, windowDimensions);
    RenderIm3dText(camera);

    ImGui::Render();
    render.ImGuiBindingsRender(ImGui::GetDrawData());
}

void ImGuiHelper::BeginInvisibleFullscreenWindow()
{
    auto& io = ImGui::GetIO();
    ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32_BLACK_TRANS);
    ImGui::SetNextWindowPos({ 0.0f, 0.0f });
    ImGui::SetNextWindowSize(io.DisplaySize);
    ImGui::Begin(
        "Invisible",
        nullptr,
        0 | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
        ImGuiWindowFlags_NoBringToFrontOnFocus);
}

void ImGuiHelper::EndInvisibleFullscreenWindow()
{
    ImGui::End();
    ImGui::PopStyleColor(1);
}

IEventListener::EventAction ImGuiHelper::OnEvent(const SDL_Event& event)
{
    const bool isKeyboardEvent =
        event.type == SDL_KEYDOWN
        || event.type == SDL_KEYUP
        || event.type == SDL_TEXTINPUT;

    const bool isMouseEvent =
        event.type == SDL_MOUSEBUTTONDOWN
        || event.type == SDL_MOUSEBUTTONUP
        || event.type == SDL_MOUSEWHEEL;

    auto& io = ImGui::GetIO();
    const bool passMouseToImgui = io.WantCaptureMouse && isMouseEvent;
    const bool passKeyboardToImgui = io.WantCaptureKeyboard && isKeyboardEvent;

    const bool imguiConsumedEvent = ImGui_ImplSDL2_ProcessEvent(&event);

    return (passMouseToImgui || passKeyboardToImgui) && imguiConsumedEvent
       ? IEventListener::EventAction::Consume
       : IEventListener::EventAction::Passthrough;
}

}
