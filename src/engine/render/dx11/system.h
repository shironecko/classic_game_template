#pragma once

#include <engine/system.h>

#define CGT_CHECK_HRESULT(hresult, msgStr, ...)                                                         \
while(!SUCCEEDED(hresult))                                                                              \
{                                                                                                       \
    _com_error CGT_CHECK_HRESULT_comError(hresult);                                                     \
    std::string CGT_CHECK_HRESULT_userMsg = fmt::format(msgStr, ##__VA_ARGS__);                         \
    CGT_PANIC("HRESULT error: {}\n{}", CGT_CHECK_HRESULT_comError.ErrorMessage(), CGT_CHECK_HRESULT_userMsg.c_str());  \
    break;                                                                                              \
}
