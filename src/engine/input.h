#pragma once

#include <engine/window.h>
#include <engine/key_codes.h>

namespace cgt
{

class Input final
{
public:
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

    i32 GetMouseWheelMotion() const;
    glm::ivec2 GetMousePosition() const;

    void Reset();

private:
    typedef std::array<KeyCode, SDL_Scancode::SDL_NUM_SCANCODES> ScancodeMappings;
    typedef std::bitset<(usize)KeyCode::Count> KeysBitset;

    friend ScancodeMappings GenerateScancodeMappings();

    bool Get(const KeysBitset& bitset, KeyCode key) const;
    void SetKeyboardKeyState(KeysBitset& bitset, SDL_Scancode scancode, bool active);
    void SetMouseButtonState(KeysBitset& bitset, u8 buttonIndex, bool active);

    InputProcessingMode m_Mode;

    KeysBitset m_Pressed;
    KeysBitset m_Released;
    KeysBitset m_Held;

    i32 m_MouseWheelMotion = 0;

    static ScancodeMappings s_ScancodeMappings;
};

}