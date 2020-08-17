#include <engine/pch.h>

#include <engine/input_helper.h>

namespace cgt
{

InputHelper::InputHelper(InputHelper::InputProcessingMode mode)
    : m_Mode(mode)
{
    m_ScancodeMappings[SDL_SCANCODE_A] = KeyCode::A;
    m_ScancodeMappings[SDL_SCANCODE_B] = KeyCode::B;
    m_ScancodeMappings[SDL_SCANCODE_C] = KeyCode::C;
    m_ScancodeMappings[SDL_SCANCODE_D] = KeyCode::D;
    m_ScancodeMappings[SDL_SCANCODE_E] = KeyCode::E;
    m_ScancodeMappings[SDL_SCANCODE_F] = KeyCode::F;
    m_ScancodeMappings[SDL_SCANCODE_G] = KeyCode::G;
    m_ScancodeMappings[SDL_SCANCODE_H] = KeyCode::H;
    m_ScancodeMappings[SDL_SCANCODE_I] = KeyCode::I;
    m_ScancodeMappings[SDL_SCANCODE_J] = KeyCode::J;
    m_ScancodeMappings[SDL_SCANCODE_K] = KeyCode::K;
    m_ScancodeMappings[SDL_SCANCODE_L] = KeyCode::L;
    m_ScancodeMappings[SDL_SCANCODE_M] = KeyCode::M;
    m_ScancodeMappings[SDL_SCANCODE_N] = KeyCode::N;
    m_ScancodeMappings[SDL_SCANCODE_O] = KeyCode::O;
    m_ScancodeMappings[SDL_SCANCODE_P] = KeyCode::P;
    m_ScancodeMappings[SDL_SCANCODE_Q] = KeyCode::Q;
    m_ScancodeMappings[SDL_SCANCODE_R] = KeyCode::R;
    m_ScancodeMappings[SDL_SCANCODE_S] = KeyCode::S;
    m_ScancodeMappings[SDL_SCANCODE_T] = KeyCode::T;
    m_ScancodeMappings[SDL_SCANCODE_U] = KeyCode::U;
    m_ScancodeMappings[SDL_SCANCODE_V] = KeyCode::V;
    m_ScancodeMappings[SDL_SCANCODE_W] = KeyCode::W;
    m_ScancodeMappings[SDL_SCANCODE_X] = KeyCode::X;
    m_ScancodeMappings[SDL_SCANCODE_Y] = KeyCode::Y;
    m_ScancodeMappings[SDL_SCANCODE_Z] = KeyCode::Z;

    m_ScancodeMappings[SDL_SCANCODE_1] = KeyCode::Number1;
    m_ScancodeMappings[SDL_SCANCODE_2] = KeyCode::Number2;
    m_ScancodeMappings[SDL_SCANCODE_3] = KeyCode::Number3;
    m_ScancodeMappings[SDL_SCANCODE_4] = KeyCode::Number4;
    m_ScancodeMappings[SDL_SCANCODE_5] = KeyCode::Number5;
    m_ScancodeMappings[SDL_SCANCODE_6] = KeyCode::Number6;
    m_ScancodeMappings[SDL_SCANCODE_7] = KeyCode::Number7;
    m_ScancodeMappings[SDL_SCANCODE_8] = KeyCode::Number8;
    m_ScancodeMappings[SDL_SCANCODE_9] = KeyCode::Number9;
    m_ScancodeMappings[SDL_SCANCODE_0] = KeyCode::Number0;

    m_ScancodeMappings[SDL_SCANCODE_RETURN] = KeyCode::Return;
    m_ScancodeMappings[SDL_SCANCODE_ESCAPE] = KeyCode::Escape;
    m_ScancodeMappings[SDL_SCANCODE_BACKSPACE] = KeyCode::Backspace;
    m_ScancodeMappings[SDL_SCANCODE_TAB] = KeyCode::Tab;
    m_ScancodeMappings[SDL_SCANCODE_SPACE] = KeyCode::Space;

    m_ScancodeMappings[SDL_SCANCODE_MINUS] = KeyCode::Minus;
    m_ScancodeMappings[SDL_SCANCODE_EQUALS] = KeyCode::Equals;
    m_ScancodeMappings[SDL_SCANCODE_LEFTBRACKET] = KeyCode::LeftBracket;
    m_ScancodeMappings[SDL_SCANCODE_RIGHTBRACKET] = KeyCode::RightBracket;
    m_ScancodeMappings[SDL_SCANCODE_BACKSLASH] = KeyCode::Backslash;
    m_ScancodeMappings[SDL_SCANCODE_SEMICOLON] = KeyCode::Semicolon;
    m_ScancodeMappings[SDL_SCANCODE_APOSTROPHE] = KeyCode::Apostrophe;
    m_ScancodeMappings[SDL_SCANCODE_GRAVE] = KeyCode::Grave;
    m_ScancodeMappings[SDL_SCANCODE_COMMA] = KeyCode::Comma;
    m_ScancodeMappings[SDL_SCANCODE_PERIOD] = KeyCode::Period;
    m_ScancodeMappings[SDL_SCANCODE_SLASH] = KeyCode::Slash;

    m_ScancodeMappings[SDL_SCANCODE_CAPSLOCK] = KeyCode::Capslock;

    m_ScancodeMappings[SDL_SCANCODE_F1] = KeyCode::F1;
    m_ScancodeMappings[SDL_SCANCODE_F2] = KeyCode::F2;
    m_ScancodeMappings[SDL_SCANCODE_F3] = KeyCode::F3;
    m_ScancodeMappings[SDL_SCANCODE_F4] = KeyCode::F4;
    m_ScancodeMappings[SDL_SCANCODE_F5] = KeyCode::F5;
    m_ScancodeMappings[SDL_SCANCODE_F6] = KeyCode::F6;
    m_ScancodeMappings[SDL_SCANCODE_F7] = KeyCode::F7;
    m_ScancodeMappings[SDL_SCANCODE_F8] = KeyCode::F8;
    m_ScancodeMappings[SDL_SCANCODE_F9] = KeyCode::F9;
    m_ScancodeMappings[SDL_SCANCODE_F10] = KeyCode::F10;
    m_ScancodeMappings[SDL_SCANCODE_F11] = KeyCode::F11;
    m_ScancodeMappings[SDL_SCANCODE_F12] = KeyCode::F12;

    m_ScancodeMappings[SDL_SCANCODE_PRINTSCREEN] = KeyCode::Printscreen;
    m_ScancodeMappings[SDL_SCANCODE_SCROLLLOCK] = KeyCode::Scrolllock;
    m_ScancodeMappings[SDL_SCANCODE_PAUSE] = KeyCode::Pause;
    m_ScancodeMappings[SDL_SCANCODE_INSERT] = KeyCode::Insert;
    m_ScancodeMappings[SDL_SCANCODE_HOME] = KeyCode::Home;
    m_ScancodeMappings[SDL_SCANCODE_PAGEUP] = KeyCode::PageUp;
    m_ScancodeMappings[SDL_SCANCODE_DELETE] = KeyCode::Delete;
    m_ScancodeMappings[SDL_SCANCODE_END] = KeyCode::End;
    m_ScancodeMappings[SDL_SCANCODE_PAGEDOWN] = KeyCode::PageDown;
    m_ScancodeMappings[SDL_SCANCODE_RIGHT] = KeyCode::Right;
    m_ScancodeMappings[SDL_SCANCODE_LEFT] = KeyCode::Left;
    m_ScancodeMappings[SDL_SCANCODE_DOWN] = KeyCode::Down;
    m_ScancodeMappings[SDL_SCANCODE_UP] = KeyCode::Up;

    m_ScancodeMappings[SDL_SCANCODE_KP_DIVIDE] = KeyCode::NumPadDivide;
    m_ScancodeMappings[SDL_SCANCODE_KP_MULTIPLY] = KeyCode::NumPadMultiply;
    m_ScancodeMappings[SDL_SCANCODE_KP_MINUS] = KeyCode::NumPadMinus;
    m_ScancodeMappings[SDL_SCANCODE_KP_PLUS] = KeyCode::NumPadPlus;
    m_ScancodeMappings[SDL_SCANCODE_KP_ENTER] = KeyCode::NumPadEnter;
    m_ScancodeMappings[SDL_SCANCODE_KP_1] = KeyCode::NumPad1;
    m_ScancodeMappings[SDL_SCANCODE_KP_2] = KeyCode::NumPad2;
    m_ScancodeMappings[SDL_SCANCODE_KP_3] = KeyCode::NumPad3;
    m_ScancodeMappings[SDL_SCANCODE_KP_4] = KeyCode::NumPad4;
    m_ScancodeMappings[SDL_SCANCODE_KP_5] = KeyCode::NumPad5;
    m_ScancodeMappings[SDL_SCANCODE_KP_6] = KeyCode::NumPad6;
    m_ScancodeMappings[SDL_SCANCODE_KP_7] = KeyCode::NumPad7;
    m_ScancodeMappings[SDL_SCANCODE_KP_8] = KeyCode::NumPad8;
    m_ScancodeMappings[SDL_SCANCODE_KP_9] = KeyCode::NumPad9;
    m_ScancodeMappings[SDL_SCANCODE_KP_0] = KeyCode::NumPad0;
    m_ScancodeMappings[SDL_SCANCODE_KP_PERIOD] = KeyCode::NumPadPeriod;
    m_ScancodeMappings[SDL_SCANCODE_KP_COMMA] = KeyCode::NumPadComma;

    m_ScancodeMappings[SDL_SCANCODE_LCTRL] = KeyCode::LeftCtrl;
    m_ScancodeMappings[SDL_SCANCODE_LSHIFT] = KeyCode::LeftShift;
    m_ScancodeMappings[SDL_SCANCODE_LALT] = KeyCode::LeftAlt;
    m_ScancodeMappings[SDL_SCANCODE_LGUI] = KeyCode::LeftGui;
    m_ScancodeMappings[SDL_SCANCODE_RCTRL] = KeyCode::RightCtrl;
    m_ScancodeMappings[SDL_SCANCODE_RSHIFT] = KeyCode::RightShift;
    m_ScancodeMappings[SDL_SCANCODE_RALT] = KeyCode::RightAlt;
    m_ScancodeMappings[SDL_SCANCODE_RGUI] = KeyCode::RightGui;
}

std::shared_ptr<InputHelper> InputHelper::Create(InputHelper::InputProcessingMode mode)
{
    return std::shared_ptr<InputHelper>(new InputHelper(mode));
}

IEventListener::EventAction InputHelper::OnEvent(const SDL_Event& event)
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
        return IEventListener::EventAction::Consume;
    }

    return IEventListener::EventAction::Passthrough;
}

void InputHelper::NewFrame()
{
    m_Pressed.reset();
    m_Released.reset();
}

bool InputHelper::IsKeyPressed(KeyCode keyCode) const
{
    return Get(m_Pressed, keyCode);
}

bool InputHelper::IsKeyReleased(KeyCode keyCode) const
{
    return Get(m_Released, keyCode);
}

bool InputHelper::IsKeyHeld(KeyCode keyCode) const
{
    return Get(m_Held, keyCode);
}

glm::ivec2 InputHelper::GetMousePosition() const
{
    glm::ivec2 position;
    SDL_GetMouseState(&position.x, &position.y);

    return position;
}

bool InputHelper::Get(const InputHelper::KeysBitset& bitset, KeyCode key) const
{
    if (key == KeyCode::Any)
    {
        return bitset.any();
    }

    return bitset.test((usize)key);
}

void InputHelper::Set(KeysBitset& bitset, SDL_Scancode scancode, bool active)
{
    const auto keyCode = m_ScancodeMappings[scancode];
    if (keyCode != KeyCode::Any)
    {
        bitset.set((usize)keyCode, active);
    }
}

KeyCode InputHelper::SDLMouseButtonIndexToKeyCode(u8 buttonIndex) const
{
    switch (buttonIndex)
    {
    case SDL_BUTTON_LEFT: return KeyCode::LeftMouseButton;
    case SDL_BUTTON_RIGHT: return KeyCode::RightMouseButton;
    case SDL_BUTTON_MIDDLE: return KeyCode::MiddleMouseButton;
    default: CGT_PANIC("Unknown SDL mouse button index: %u", (u32)buttonIndex); return KeyCode::Any;
    }
}

void InputHelper::Reset()
{
    m_Pressed.reset();
    m_Held.reset();
    m_Released.reset();
}

}