#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <filesystem>
#include <fstream>

#define CGT_PANIC(fmtStr, ...)                                                                                      \
do {                                                                                                                \
    std::string CGT_PANIC_userMsg = fmt::format(fmtStr, ##__VA_ARGS__);                                             \
    std::string CGT_PANIC_fullMsg = fmt::format("File: {}\nLine: {}\n{}", __FILE__, __LINE__, CGT_PANIC_userMsg);   \
    SDL_MessageBoxButtonData CGT_PANIC_buttons[] =                                                                  \
    {                                                                                                               \
            { SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 0, "Debug Break" },                                          \
            { SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, -1, "Quit" }                                                 \
    };                                                                                                              \
    SDL_MessageBoxData CGT_PANIC_data {};                                                                           \
    CGT_PANIC_data.buttons = CGT_PANIC_buttons;                                                                     \
    CGT_PANIC_data.numbuttons = 2;                                                                                  \
    CGT_PANIC_data.flags = SDL_MESSAGEBOX_ERROR;                                                                    \
    CGT_PANIC_data.title = "Panic!";                                                                                \
    CGT_PANIC_data.message = CGT_PANIC_fullMsg.c_str();                                                             \
                                                                                                                    \
    int CGT_PANIC_pressedButton = -1;                                                                               \
    int CGT_PANIC_result = SDL_ShowMessageBox(&CGT_PANIC_data, &CGT_PANIC_pressedButton);                           \
    if (CGT_PANIC_result == -1 || CGT_PANIC_pressedButton == 0)                                                     \
    {                                                                                                               \
        SDL_TriggerBreakpoint();                                                                                    \
    }                                                                                                               \
    else                                                                                                            \
    {                                                                                                               \
        exit(-1);                                                                                                   \
    }                                                                                                               \
} while (false);

#define CGT_ASSERT_ALWAYS_MSG(condition, fmtStr, ...)                                           \
while (!(condition))                                                                            \
{                                                                                               \
    std::string CGT_ASSERT_ALWAYS_userMsg = fmt::format(fmtStr, ##__VA_ARGS__);                 \
    CGT_PANIC("Assertion failed: {}\n{}", #condition, CGT_ASSERT_ALWAYS_userMsg);               \
}

#define CGT_ASSERT_ALWAYS(condition) CGT_ASSERT_ALWAYS_MSG((condition), "Assertion failed!")

#if defined(DEBUG) || defined(_DEBUG)
#define CGT_DEBUG_ONLY(stmt) (stmt)
#define CGT_ASSERT(condition) CGT_ASSERT_ALWAYS((condition))
#define CGT_ASSERT_MSG(condition, fmtStr, ...) CGT_ASSERT_ALWAYS((condition), fmtStr, ##__VA_ARGS__)
#else
#define CGT_DEBUG_ONLY(stmt)
#define CGT_ASSERT(condition)
#define CGT_ASSERT_MSG(condition, fmtStr, ...)
#endif

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef size_t usize;
typedef uintptr_t uptr;
typedef intptr_t ptrdiff;

class NonCopyable
{
protected:
    constexpr NonCopyable() = default;

    ~NonCopyable() = default;

public:
    NonCopyable(const NonCopyable&) = delete;

    NonCopyable& operator=(const NonCopyable&) = delete;
};
