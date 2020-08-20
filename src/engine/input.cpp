#include <engine/pch.h>

#include <engine/input.h>

namespace cgt
{

Input::ScancodeMappings Input::s_ScancodeMappings;

void Input::Initialize()
{
    s_ScancodeMappings[SDL_SCANCODE_A] = KeyCode::A;
    s_ScancodeMappings[SDL_SCANCODE_B] = KeyCode::B;
    s_ScancodeMappings[SDL_SCANCODE_C] = KeyCode::C;
    s_ScancodeMappings[SDL_SCANCODE_D] = KeyCode::D;
    s_ScancodeMappings[SDL_SCANCODE_E] = KeyCode::E;
    s_ScancodeMappings[SDL_SCANCODE_F] = KeyCode::F;
    s_ScancodeMappings[SDL_SCANCODE_G] = KeyCode::G;
    s_ScancodeMappings[SDL_SCANCODE_H] = KeyCode::H;
    s_ScancodeMappings[SDL_SCANCODE_I] = KeyCode::I;
    s_ScancodeMappings[SDL_SCANCODE_J] = KeyCode::J;
    s_ScancodeMappings[SDL_SCANCODE_K] = KeyCode::K;
    s_ScancodeMappings[SDL_SCANCODE_L] = KeyCode::L;
    s_ScancodeMappings[SDL_SCANCODE_M] = KeyCode::M;
    s_ScancodeMappings[SDL_SCANCODE_N] = KeyCode::N;
    s_ScancodeMappings[SDL_SCANCODE_O] = KeyCode::O;
    s_ScancodeMappings[SDL_SCANCODE_P] = KeyCode::P;
    s_ScancodeMappings[SDL_SCANCODE_Q] = KeyCode::Q;
    s_ScancodeMappings[SDL_SCANCODE_R] = KeyCode::R;
    s_ScancodeMappings[SDL_SCANCODE_S] = KeyCode::S;
    s_ScancodeMappings[SDL_SCANCODE_T] = KeyCode::T;
    s_ScancodeMappings[SDL_SCANCODE_U] = KeyCode::U;
    s_ScancodeMappings[SDL_SCANCODE_V] = KeyCode::V;
    s_ScancodeMappings[SDL_SCANCODE_W] = KeyCode::W;
    s_ScancodeMappings[SDL_SCANCODE_X] = KeyCode::X;
    s_ScancodeMappings[SDL_SCANCODE_Y] = KeyCode::Y;
    s_ScancodeMappings[SDL_SCANCODE_Z] = KeyCode::Z;

    s_ScancodeMappings[SDL_SCANCODE_1] = KeyCode::Number1;
    s_ScancodeMappings[SDL_SCANCODE_2] = KeyCode::Number2;
    s_ScancodeMappings[SDL_SCANCODE_3] = KeyCode::Number3;
    s_ScancodeMappings[SDL_SCANCODE_4] = KeyCode::Number4;
    s_ScancodeMappings[SDL_SCANCODE_5] = KeyCode::Number5;
    s_ScancodeMappings[SDL_SCANCODE_6] = KeyCode::Number6;
    s_ScancodeMappings[SDL_SCANCODE_7] = KeyCode::Number7;
    s_ScancodeMappings[SDL_SCANCODE_8] = KeyCode::Number8;
    s_ScancodeMappings[SDL_SCANCODE_9] = KeyCode::Number9;
    s_ScancodeMappings[SDL_SCANCODE_0] = KeyCode::Number0;

    s_ScancodeMappings[SDL_SCANCODE_RETURN] = KeyCode::Return;
    s_ScancodeMappings[SDL_SCANCODE_ESCAPE] = KeyCode::Escape;
    s_ScancodeMappings[SDL_SCANCODE_BACKSPACE] = KeyCode::Backspace;
    s_ScancodeMappings[SDL_SCANCODE_TAB] = KeyCode::Tab;
    s_ScancodeMappings[SDL_SCANCODE_SPACE] = KeyCode::Space;

    s_ScancodeMappings[SDL_SCANCODE_MINUS] = KeyCode::Minus;
    s_ScancodeMappings[SDL_SCANCODE_EQUALS] = KeyCode::Equals;
    s_ScancodeMappings[SDL_SCANCODE_LEFTBRACKET] = KeyCode::LeftBracket;
    s_ScancodeMappings[SDL_SCANCODE_RIGHTBRACKET] = KeyCode::RightBracket;
    s_ScancodeMappings[SDL_SCANCODE_BACKSLASH] = KeyCode::Backslash;
    s_ScancodeMappings[SDL_SCANCODE_SEMICOLON] = KeyCode::Semicolon;
    s_ScancodeMappings[SDL_SCANCODE_APOSTROPHE] = KeyCode::Apostrophe;
    s_ScancodeMappings[SDL_SCANCODE_GRAVE] = KeyCode::Grave;
    s_ScancodeMappings[SDL_SCANCODE_COMMA] = KeyCode::Comma;
    s_ScancodeMappings[SDL_SCANCODE_PERIOD] = KeyCode::Period;
    s_ScancodeMappings[SDL_SCANCODE_SLASH] = KeyCode::Slash;

    s_ScancodeMappings[SDL_SCANCODE_CAPSLOCK] = KeyCode::Capslock;

    s_ScancodeMappings[SDL_SCANCODE_F1] = KeyCode::F1;
    s_ScancodeMappings[SDL_SCANCODE_F2] = KeyCode::F2;
    s_ScancodeMappings[SDL_SCANCODE_F3] = KeyCode::F3;
    s_ScancodeMappings[SDL_SCANCODE_F4] = KeyCode::F4;
    s_ScancodeMappings[SDL_SCANCODE_F5] = KeyCode::F5;
    s_ScancodeMappings[SDL_SCANCODE_F6] = KeyCode::F6;
    s_ScancodeMappings[SDL_SCANCODE_F7] = KeyCode::F7;
    s_ScancodeMappings[SDL_SCANCODE_F8] = KeyCode::F8;
    s_ScancodeMappings[SDL_SCANCODE_F9] = KeyCode::F9;
    s_ScancodeMappings[SDL_SCANCODE_F10] = KeyCode::F10;
    s_ScancodeMappings[SDL_SCANCODE_F11] = KeyCode::F11;
    s_ScancodeMappings[SDL_SCANCODE_F12] = KeyCode::F12;

    s_ScancodeMappings[SDL_SCANCODE_PRINTSCREEN] = KeyCode::Printscreen;
    s_ScancodeMappings[SDL_SCANCODE_SCROLLLOCK] = KeyCode::Scrolllock;
    s_ScancodeMappings[SDL_SCANCODE_PAUSE] = KeyCode::Pause;
    s_ScancodeMappings[SDL_SCANCODE_INSERT] = KeyCode::Insert;
    s_ScancodeMappings[SDL_SCANCODE_HOME] = KeyCode::Home;
    s_ScancodeMappings[SDL_SCANCODE_PAGEUP] = KeyCode::PageUp;
    s_ScancodeMappings[SDL_SCANCODE_DELETE] = KeyCode::Delete;
    s_ScancodeMappings[SDL_SCANCODE_END] = KeyCode::End;
    s_ScancodeMappings[SDL_SCANCODE_PAGEDOWN] = KeyCode::PageDown;
    s_ScancodeMappings[SDL_SCANCODE_RIGHT] = KeyCode::Right;
    s_ScancodeMappings[SDL_SCANCODE_LEFT] = KeyCode::Left;
    s_ScancodeMappings[SDL_SCANCODE_DOWN] = KeyCode::Down;
    s_ScancodeMappings[SDL_SCANCODE_UP] = KeyCode::Up;

    s_ScancodeMappings[SDL_SCANCODE_KP_DIVIDE] = KeyCode::NumPadDivide;
    s_ScancodeMappings[SDL_SCANCODE_KP_MULTIPLY] = KeyCode::NumPadMultiply;
    s_ScancodeMappings[SDL_SCANCODE_KP_MINUS] = KeyCode::NumPadMinus;
    s_ScancodeMappings[SDL_SCANCODE_KP_PLUS] = KeyCode::NumPadPlus;
    s_ScancodeMappings[SDL_SCANCODE_KP_ENTER] = KeyCode::NumPadEnter;
    s_ScancodeMappings[SDL_SCANCODE_KP_1] = KeyCode::NumPad1;
    s_ScancodeMappings[SDL_SCANCODE_KP_2] = KeyCode::NumPad2;
    s_ScancodeMappings[SDL_SCANCODE_KP_3] = KeyCode::NumPad3;
    s_ScancodeMappings[SDL_SCANCODE_KP_4] = KeyCode::NumPad4;
    s_ScancodeMappings[SDL_SCANCODE_KP_5] = KeyCode::NumPad5;
    s_ScancodeMappings[SDL_SCANCODE_KP_6] = KeyCode::NumPad6;
    s_ScancodeMappings[SDL_SCANCODE_KP_7] = KeyCode::NumPad7;
    s_ScancodeMappings[SDL_SCANCODE_KP_8] = KeyCode::NumPad8;
    s_ScancodeMappings[SDL_SCANCODE_KP_9] = KeyCode::NumPad9;
    s_ScancodeMappings[SDL_SCANCODE_KP_0] = KeyCode::NumPad0;
    s_ScancodeMappings[SDL_SCANCODE_KP_PERIOD] = KeyCode::NumPadPeriod;
    s_ScancodeMappings[SDL_SCANCODE_KP_COMMA] = KeyCode::NumPadComma;

    s_ScancodeMappings[SDL_SCANCODE_LCTRL] = KeyCode::LeftCtrl;
    s_ScancodeMappings[SDL_SCANCODE_LSHIFT] = KeyCode::LeftShift;
    s_ScancodeMappings[SDL_SCANCODE_LALT] = KeyCode::LeftAlt;
    s_ScancodeMappings[SDL_SCANCODE_LGUI] = KeyCode::LeftGui;
    s_ScancodeMappings[SDL_SCANCODE_RCTRL] = KeyCode::RightCtrl;
    s_ScancodeMappings[SDL_SCANCODE_RSHIFT] = KeyCode::RightShift;
    s_ScancodeMappings[SDL_SCANCODE_RALT] = KeyCode::RightAlt;
    s_ScancodeMappings[SDL_SCANCODE_RGUI] = KeyCode::RightGui;
}

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
        Set(m_Pressed, scancode, true);
        Set(m_Held, scancode, true);
        eventHandled = true;
        break;
    }
    case SDL_KEYUP:
    {
        auto scancode = event.key.keysym.scancode;
        Set(m_Held, scancode, false);
        Set(m_Released, scancode, true);
        eventHandled = true;
        break;
    }
    case SDL_MOUSEBUTTONDOWN:
    {
        auto keyCode = SDLMouseButtonIndexToKeyCode(event.button.button);
        m_Pressed.set((usize)keyCode, true);
        m_Held.set((usize)keyCode, true);
        eventHandled = true;
        break;
    }
    case SDL_MOUSEBUTTONUP:
    {
        auto keyCode = SDLMouseButtonIndexToKeyCode(event.button.button);
        m_Held.set((usize)keyCode, false);
        m_Released.set((usize)keyCode, true);
        eventHandled = true;
        break;
    }
    case SDL_MOUSEWHEEL:
        auto yDirection = event.wheel.y;
        auto keyCode = yDirection > 0 ? KeyCode::MouseWheelUp : KeyCode::MouseWheelDown;
        m_Pressed.set((usize)keyCode, true);
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

bool Input::Get(const Input::KeysBitset& bitset, KeyCode key) const
{
    if (key == KeyCode::Any)
    {
        return bitset.any();
    }

    return bitset.test((usize)key);
}

void Input::Set(KeysBitset& bitset, SDL_Scancode scancode, bool active)
{
    const auto keyCode = s_ScancodeMappings[scancode];
    if (keyCode != KeyCode::Any)
    {
        bitset.set((usize)keyCode, active);
    }
}

KeyCode Input::SDLMouseButtonIndexToKeyCode(u8 buttonIndex) const
{
    switch (buttonIndex)
    {
    case SDL_BUTTON_LEFT: return KeyCode::LeftMouseButton;
    case SDL_BUTTON_RIGHT: return KeyCode::RightMouseButton;
    case SDL_BUTTON_MIDDLE: return KeyCode::MiddleMouseButton;
    default: CGT_PANIC("Unknown SDL mouse button index: %u", (u32)buttonIndex); return KeyCode::Any;
    }
}

void Input::Reset()
{
    m_Pressed.reset();
    m_Held.reset();
    m_Released.reset();
}

}