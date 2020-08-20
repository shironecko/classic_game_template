#pragma once

#include <cstdint>
#include <memory>
#include <random>
#include <string>
#include <vector>
#include <unordered_map>
#include <filesystem>
#include <fstream>
#include <bitset>

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

namespace cgt
{

#define CGT_ARRAY_LENGTH(staticArray) SDL_arraysize(staticArray)

enum class PanicDialogResponse
{
    DebugBreak,
    Quit,
};

PanicDialogResponse ShowPanicDialog(const char* message, const char* title = "Panic!");

#define CGT_PANIC(fmtStr, ...)                                                                                      \
do {                                                                                                                \
    std::string CGT_PANIC_userMsg = fmt::format(fmtStr, ##__VA_ARGS__);                                             \
    std::string CGT_PANIC_fullMsg = fmt::format("File: {}\nLine: {}\n{}", __FILE__, __LINE__, CGT_PANIC_userMsg);   \
    auto CGT_PANIC_userResponse = ::cgt::ShowPanicDialog(CGT_PANIC_fullMsg.c_str());                   \
    if (CGT_PANIC_userResponse == ::cgt::PanicDialogResponse::DebugBreak)                                           \
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
#define CGT_ASSERT_MSG(condition, fmtStr, ...) CGT_ASSERT_ALWAYS_MSG((condition), (fmtStr), ##__VA_ARGS__)
#else
#define CGT_DEBUG_ONLY(stmt)
#define CGT_ASSERT(condition)
#define CGT_ASSERT_MSG(condition, fmtStr, ...)
#endif

class NonCopyable
{
protected:
    constexpr NonCopyable() = default;

    ~NonCopyable() = default;

public:
    NonCopyable(const NonCopyable&) = delete;

    NonCopyable& operator=(const NonCopyable&) = delete;
};

}