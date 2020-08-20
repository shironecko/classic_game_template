#include <engine/pch.h>

#include <engine/system.h>

namespace cgt
{

PanicDialogResponse ShowPanicDialog(const char* message, const char* title)
{
    SDL_MessageBoxButtonData buttons[] =
    {
            { SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 0, "Debug Break" },
            { SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, -1, "Quit" }
    };
    SDL_MessageBoxData data {};
    data.buttons = buttons;
    data.numbuttons = CGT_ARRAY_LENGTH(buttons);
    data.flags = SDL_MESSAGEBOX_ERROR;
    data.title = title;
    data.message = message;

    i32 pressedButton = -1;
    i32 result = SDL_ShowMessageBox(&data, &pressedButton);
    if (result == -1 || pressedButton == 0)
    {
        return PanicDialogResponse::DebugBreak;
    }
    else
    {
        return PanicDialogResponse::Quit;
    }
}

}