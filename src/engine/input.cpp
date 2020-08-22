#include <engine/pch.h>

#include <engine/input.h>

namespace cgt
{

Input::ScancodeMappings GenerateScancodeMappings();

Input::ScancodeMappings Input::s_ScancodeMappings = GenerateScancodeMappings();

Input::Input(Input::InputProcessingMode mode)
    : m_Mode(mode)
{
}

WindowEventControlFlow Input::ProcessWindowEvent(const SDL_Event& event)
{
    bool eventHandled = false;
    switch (event.type)
    {
    case SDL_KEYDOWN:
    {
        auto scancode = event.key.keysym.scancode;
        SetKeyboardKeyState(m_Pressed, scancode, true);
        SetKeyboardKeyState(m_Held, scancode, true);
        eventHandled = true;
        break;
    }
    case SDL_KEYUP:
    {
        auto scancode = event.key.keysym.scancode;
        SetKeyboardKeyState(m_Held, scancode, false);
        SetKeyboardKeyState(m_Released, scancode, true);
        eventHandled = true;
        break;
    }
    case SDL_MOUSEBUTTONDOWN:
    {
        u8 buttonIndex = event.button.button;
        SetMouseButtonState(m_Pressed, buttonIndex, true);
        SetMouseButtonState(m_Held, buttonIndex, true);
        eventHandled = true;
        break;
    }
    case SDL_MOUSEBUTTONUP:
    {
        u8 buttonIndex = event.button.button;
        SetMouseButtonState(m_Held, buttonIndex, false);
        SetMouseButtonState(m_Released, buttonIndex, true);
        eventHandled = true;
        break;
    }
    case SDL_MOUSEWHEEL:
        i32 motion = event.wheel.y;
        m_MouseWheelMotion += motion;
        eventHandled = true;
        break;
    }

    if (eventHandled && m_Mode == InputProcessingMode::Consume)
    {
        return WindowEventControlFlow::ConsumeEvent;
    }

    return WindowEventControlFlow::PassthroughEvent;
}

void Input::NewFrame()
{
    m_Pressed.reset();
    m_Released.reset();
    m_MouseWheelMotion = 0;
}

bool Input::IsKeyPressed(KeyCode keyCode) const
{
    return Get(m_Pressed, keyCode);
}

bool Input::IsKeyReleased(KeyCode keyCode) const
{
    return Get(m_Released, keyCode);
}

bool Input::IsKeyHeld(KeyCode keyCode) const
{
    return Get(m_Held, keyCode);
}

glm::ivec2 Input::GetMousePosition() const
{
    glm::ivec2 position;
    SDL_GetMouseState(&position.x, &position.y);

    return position;
}

i32 Input::GetMouseWheelMotion() const
{
    return m_MouseWheelMotion;
}

bool Input::Get(const Input::KeysBitset& bitset, KeyCode key) const
{
    if (key == KeyCode::Any)
    {
        return bitset.any();
    }

    return bitset.test((usize)key);
}

void Input::SetKeyboardKeyState(KeysBitset& bitset, SDL_Scancode scancode, bool active)
{
    CGT_ASSERT(scancode < s_ScancodeMappings.size());

    const auto keyCode = s_ScancodeMappings[scancode];
    if (keyCode != KeyCode::Any)
    {
        bitset.set((usize)keyCode, active);
    }
}

void Input::SetMouseButtonState(KeysBitset& bitset, u8 buttonIndex, bool active)
{
    auto keyCode = KeyCode::Any;
    switch (buttonIndex)
    {
    case SDL_BUTTON_LEFT:
        keyCode = KeyCode::LeftMouseButton;
        break;
    case SDL_BUTTON_RIGHT:
        keyCode =  KeyCode::RightMouseButton;
        break;
    case SDL_BUTTON_MIDDLE:
        keyCode =  KeyCode::MiddleMouseButton;
        break;
    }

    if (keyCode != KeyCode::Any)
    {
        bitset.set((usize)keyCode, active);
    }
}

void Input::Reset()
{
    m_Pressed.reset();
    m_Held.reset();
    m_Released.reset();
    m_MouseWheelMotion = 0;
}

Input::ScancodeMappings GenerateScancodeMappings()
{
    Input::ScancodeMappings mappings;
    mappings[SDL_SCANCODE_A] = KeyCode::A;
    mappings[SDL_SCANCODE_B] = KeyCode::B;
    mappings[SDL_SCANCODE_C] = KeyCode::C;
    mappings[SDL_SCANCODE_D] = KeyCode::D;
    mappings[SDL_SCANCODE_E] = KeyCode::E;
    mappings[SDL_SCANCODE_F] = KeyCode::F;
    mappings[SDL_SCANCODE_G] = KeyCode::G;
    mappings[SDL_SCANCODE_H] = KeyCode::H;
    mappings[SDL_SCANCODE_I] = KeyCode::I;
    mappings[SDL_SCANCODE_J] = KeyCode::J;
    mappings[SDL_SCANCODE_K] = KeyCode::K;
    mappings[SDL_SCANCODE_L] = KeyCode::L;
    mappings[SDL_SCANCODE_M] = KeyCode::M;
    mappings[SDL_SCANCODE_N] = KeyCode::N;
    mappings[SDL_SCANCODE_O] = KeyCode::O;
    mappings[SDL_SCANCODE_P] = KeyCode::P;
    mappings[SDL_SCANCODE_Q] = KeyCode::Q;
    mappings[SDL_SCANCODE_R] = KeyCode::R;
    mappings[SDL_SCANCODE_S] = KeyCode::S;
    mappings[SDL_SCANCODE_T] = KeyCode::T;
    mappings[SDL_SCANCODE_U] = KeyCode::U;
    mappings[SDL_SCANCODE_V] = KeyCode::V;
    mappings[SDL_SCANCODE_W] = KeyCode::W;
    mappings[SDL_SCANCODE_X] = KeyCode::X;
    mappings[SDL_SCANCODE_Y] = KeyCode::Y;
    mappings[SDL_SCANCODE_Z] = KeyCode::Z;

    mappings[SDL_SCANCODE_1] = KeyCode::Number1;
    mappings[SDL_SCANCODE_2] = KeyCode::Number2;
    mappings[SDL_SCANCODE_3] = KeyCode::Number3;
    mappings[SDL_SCANCODE_4] = KeyCode::Number4;
    mappings[SDL_SCANCODE_5] = KeyCode::Number5;
    mappings[SDL_SCANCODE_6] = KeyCode::Number6;
    mappings[SDL_SCANCODE_7] = KeyCode::Number7;
    mappings[SDL_SCANCODE_8] = KeyCode::Number8;
    mappings[SDL_SCANCODE_9] = KeyCode::Number9;
    mappings[SDL_SCANCODE_0] = KeyCode::Number0;

    mappings[SDL_SCANCODE_RETURN] = KeyCode::Return;
    mappings[SDL_SCANCODE_ESCAPE] = KeyCode::Escape;
    mappings[SDL_SCANCODE_BACKSPACE] = KeyCode::Backspace;
    mappings[SDL_SCANCODE_TAB] = KeyCode::Tab;
    mappings[SDL_SCANCODE_SPACE] = KeyCode::Space;

    mappings[SDL_SCANCODE_MINUS] = KeyCode::Minus;
    mappings[SDL_SCANCODE_EQUALS] = KeyCode::Equals;
    mappings[SDL_SCANCODE_LEFTBRACKET] = KeyCode::LeftBracket;
    mappings[SDL_SCANCODE_RIGHTBRACKET] = KeyCode::RightBracket;
    mappings[SDL_SCANCODE_BACKSLASH] = KeyCode::Backslash;
    mappings[SDL_SCANCODE_SEMICOLON] = KeyCode::Semicolon;
    mappings[SDL_SCANCODE_APOSTROPHE] = KeyCode::Apostrophe;
    mappings[SDL_SCANCODE_GRAVE] = KeyCode::Grave;
    mappings[SDL_SCANCODE_COMMA] = KeyCode::Comma;
    mappings[SDL_SCANCODE_PERIOD] = KeyCode::Period;
    mappings[SDL_SCANCODE_SLASH] = KeyCode::Slash;

    mappings[SDL_SCANCODE_CAPSLOCK] = KeyCode::Capslock;

    mappings[SDL_SCANCODE_F1] = KeyCode::F1;
    mappings[SDL_SCANCODE_F2] = KeyCode::F2;
    mappings[SDL_SCANCODE_F3] = KeyCode::F3;
    mappings[SDL_SCANCODE_F4] = KeyCode::F4;
    mappings[SDL_SCANCODE_F5] = KeyCode::F5;
    mappings[SDL_SCANCODE_F6] = KeyCode::F6;
    mappings[SDL_SCANCODE_F7] = KeyCode::F7;
    mappings[SDL_SCANCODE_F8] = KeyCode::F8;
    mappings[SDL_SCANCODE_F9] = KeyCode::F9;
    mappings[SDL_SCANCODE_F10] = KeyCode::F10;
    mappings[SDL_SCANCODE_F11] = KeyCode::F11;
    mappings[SDL_SCANCODE_F12] = KeyCode::F12;

    mappings[SDL_SCANCODE_PRINTSCREEN] = KeyCode::Printscreen;
    mappings[SDL_SCANCODE_SCROLLLOCK] = KeyCode::Scrolllock;
    mappings[SDL_SCANCODE_PAUSE] = KeyCode::Pause;
    mappings[SDL_SCANCODE_INSERT] = KeyCode::Insert;
    mappings[SDL_SCANCODE_HOME] = KeyCode::Home;
    mappings[SDL_SCANCODE_PAGEUP] = KeyCode::PageUp;
    mappings[SDL_SCANCODE_DELETE] = KeyCode::Delete;
    mappings[SDL_SCANCODE_END] = KeyCode::End;
    mappings[SDL_SCANCODE_PAGEDOWN] = KeyCode::PageDown;
    mappings[SDL_SCANCODE_RIGHT] = KeyCode::Right;
    mappings[SDL_SCANCODE_LEFT] = KeyCode::Left;
    mappings[SDL_SCANCODE_DOWN] = KeyCode::Down;
    mappings[SDL_SCANCODE_UP] = KeyCode::Up;

    mappings[SDL_SCANCODE_KP_DIVIDE] = KeyCode::NumPadDivide;
    mappings[SDL_SCANCODE_KP_MULTIPLY] = KeyCode::NumPadMultiply;
    mappings[SDL_SCANCODE_KP_MINUS] = KeyCode::NumPadMinus;
    mappings[SDL_SCANCODE_KP_PLUS] = KeyCode::NumPadPlus;
    mappings[SDL_SCANCODE_KP_ENTER] = KeyCode::NumPadEnter;
    mappings[SDL_SCANCODE_KP_1] = KeyCode::NumPad1;
    mappings[SDL_SCANCODE_KP_2] = KeyCode::NumPad2;
    mappings[SDL_SCANCODE_KP_3] = KeyCode::NumPad3;
    mappings[SDL_SCANCODE_KP_4] = KeyCode::NumPad4;
    mappings[SDL_SCANCODE_KP_5] = KeyCode::NumPad5;
    mappings[SDL_SCANCODE_KP_6] = KeyCode::NumPad6;
    mappings[SDL_SCANCODE_KP_7] = KeyCode::NumPad7;
    mappings[SDL_SCANCODE_KP_8] = KeyCode::NumPad8;
    mappings[SDL_SCANCODE_KP_9] = KeyCode::NumPad9;
    mappings[SDL_SCANCODE_KP_0] = KeyCode::NumPad0;
    mappings[SDL_SCANCODE_KP_PERIOD] = KeyCode::NumPadPeriod;
    mappings[SDL_SCANCODE_KP_COMMA] = KeyCode::NumPadComma;

    mappings[SDL_SCANCODE_LCTRL] = KeyCode::LeftCtrl;
    mappings[SDL_SCANCODE_LSHIFT] = KeyCode::LeftShift;
    mappings[SDL_SCANCODE_LALT] = KeyCode::LeftAlt;
    mappings[SDL_SCANCODE_LGUI] = KeyCode::LeftGui;
    mappings[SDL_SCANCODE_RCTRL] = KeyCode::RightCtrl;
    mappings[SDL_SCANCODE_RSHIFT] = KeyCode::RightShift;
    mappings[SDL_SCANCODE_RALT] = KeyCode::RightAlt;
    mappings[SDL_SCANCODE_RGUI] = KeyCode::RightGui;

    return mappings;
}

}