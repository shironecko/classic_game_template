#pragma once

#include <engine/window.h>

namespace cgt
{

enum class KeyCode
{
    Any, /* special case, amount to any button */

    A,
    B,
    C,
    D,
    E,
    F,
    G,
    H,
    I,
    J,
    K,
    L,
    M,
    N,
    O,
    P,
    Q,
    R,
    S,
    T,
    U,
    V,
    W,
    X,
    Y,
    Z,

    Number1,
    Number2,
    Number3,
    Number4,
    Number5,
    Number6,
    Number7,
    Number8,
    Number9,
    Number0,

    Return,
    Escape,
    Backspace,
    Tab,
    Space,

    Minus,
    Equals,
    LeftBracket,
    RightBracket,
    Backslash,
    Semicolon,
    Apostrophe,
    Grave,
    Comma,
    Period,
    Slash,

    Capslock,

    F1,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    F11,
    F12,

    Printscreen,
    Scrolllock,
    Pause,
    Insert, /* insert on PC, help on some Mac keyboards */
    Home,
    PageUp,
    Delete,
    End,
    PageDown,
    Right,
    Left,
    Down,
    Up,

    Numlock, /* num lock on PC, clear on Mac keyboards */
    NumPadDivide,
    NumPadMultiply,
    NumPadMinus,
    NumPadPlus,
    NumPadEnter,
    NumPad1,
    NumPad2,
    NumPad3,
    NumPad4,
    NumPad5,
    NumPad6,
    NumPad7,
    NumPad8,
    NumPad9,
    NumPad0,
    NumPadPeriod,
    NumPadEquals,
    NumPadComma,

    LeftCtrl,
    LeftShift,
    LeftAlt, /* alt, option */
    LeftGui, /* windows, command (apple), meta */
    RightCtrl,
    RightShift,
    RightAlt, /* alt gr, option */
    RightGui, /* windows, command (apple), meta */

    /*
     * Mouse Buttons
     */

    LeftMouseButton,
    RightMouseButton,
    MiddleMouseButton,

    MouseWheelUp,
    MouseWheelDown,

    Count
};

class Input final
{
public:
    static void Initialize();

    enum class InputProcessingMode
    {
        Passthrough,
        Consume,
    };

    Input(InputProcessingMode mode = InputProcessingMode::Consume);

    WindowEventControlFlow ProcessWindowEvent(const SDL_Event& event);

    void NewFrame();

    bool IsKeyPressed(KeyCode keyCode) const;
    bool IsKeyReleased(KeyCode keyCode) const;
    bool IsKeyHeld(KeyCode keyCode) const;

    glm::ivec2 GetMousePosition() const;

    void Reset();

private:
    typedef std::array<KeyCode, SDL_Scancode::SDL_NUM_SCANCODES> ScancodeMappings;
    typedef std::bitset<(usize)KeyCode::Count> KeysBitset;

    bool Get(const KeysBitset& bitset, KeyCode key) const;
    void Set(KeysBitset& bitset, SDL_Scancode scancode, bool active);

    KeyCode SDLMouseButtonIndexToKeyCode(u8 buttonIndex) const;

    InputProcessingMode m_Mode;

    KeysBitset m_Pressed;
    KeysBitset m_Released;
    KeysBitset m_Held;

    static ScancodeMappings s_ScancodeMappings;
};

}