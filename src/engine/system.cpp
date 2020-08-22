#include <engine/pch.h>

#include <engine/system.h>

namespace cgt
{

PanicDialogResponse ShowPanicDialog(const char* message, const char* title, bool recoverable)
{
    SDL_MessageBoxButtonData buttons[] =
    {
            { SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 0, "Debug Break" },
            { SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, -1, "Quit" },
            { 0, 1, "Continue" }
    };
    SDL_MessageBoxData data {};
    data.buttons = buttons;
    data.numbuttons = recoverable ? CGT_ARRAY_LENGTH(buttons) : CGT_ARRAY_LENGTH(buttons) - 1;
    data.flags = SDL_MESSAGEBOX_ERROR;
    data.title = title;
    data.message = message;

    i32 pressedButton = -1;
    i32 result = SDL_ShowMessageBox(&data, &pressedButton);
    if (result == -1 || pressedButton == 0)
    {
        return PanicDialogResponse::DebugBreak;
    }
    else if (pressedButton == 1)
    {
        return PanicDialogResponse::Continue;
    }

    return PanicDialogResponse::Quit;
}

}