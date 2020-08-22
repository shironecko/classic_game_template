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
    Continue,
};

PanicDialogResponse ShowPanicDialog(const char* message, const char* title, bool recoverable);

#define CGT_PANIC_EX(recoverable, fmtStr, ...) \
do {                                                                                                                \
    std::string CGT_PANIC_userMsg = fmt::format(fmtStr, ##__VA_ARGS__);                                             \
    std::string CGT_PANIC_fullMsg = fmt::format("File: {}\nLine: {}\n{}", __FILE__, __LINE__, CGT_PANIC_userMsg);   \
    auto CGT_PANIC_userResponse = ::cgt::ShowPanicDialog(CGT_PANIC_fullMsg.c_str(), (recoverable) ? "Recoverable Panic!" : "Panic!", (recoverable)); \
    if (CGT_PANIC_userResponse == ::cgt::PanicDialogResponse::DebugBreak)                                           \
    {                                                                                                               \
        SDL_TriggerBreakpoint();                                                                                    \
    }                                                                                                               \
    else if (CGT_PANIC_userResponse == ::cgt::PanicDialogResponse::Quit)                                            \
    {                                                                                                               \
        exit(-1);                                                                                                   \
    }                                                                                                               \
} while (false);

#define CGT_PANIC(fmtStr, ...) CGT_PANIC_EX(false, (fmtStr), ##__VA_ARGS__)
#define CGT_PANIC_RECOVERABLE(fmtStr, ...) CGT_PANIC_EX(true, (fmtStr), ##__VA_ARGS__)

#define CGT_ASSERT_ALWAYS_EX(recoverable, condition, fmtStr, ...) \
do {                                                                  \
    if (!(condition))                                                                            \
    {                                                                                               \
        std::string CGT_ASSERT_ALWAYS_userMsg = fmt::format((fmtStr), ##__VA_ARGS__);                 \
        CGT_PANIC_EX((recoverable), "Assertion failed: {}\n{}", #condition, CGT_ASSERT_ALWAYS_userMsg); \
    }                                                                 \
} while (false);

#define CGT_ASSERT_ALWAYS_MSG(condition, fmtStr, ...) CGT_ASSERT_ALWAYS_EX(false, (condition), (fmtStr), ##__VA_ARGS__)

#define CGT_ASSERT_ALWAYS(condition) CGT_ASSERT_ALWAYS_MSG((condition), "")

#if defined(DEBUG) || defined(_DEBUG)
#define CGT_DEBUG_ONLY(stmt) (stmt)
#define CGT_ASSERT(condition) CGT_ASSERT_ALWAYS((condition))
#define CGT_ASSERT_MSG(condition, fmtStr, ...) CGT_ASSERT_ALWAYS_MSG((condition), (fmtStr), ##__VA_ARGS__)
#define CGT_ASSERT_RECOVERABLE(condition) CGT_ASSERT_ALWAYS_EX(true, (condition), "")
#define CGT_ASSERT_RECOVERABLE_MSG(condition, fmtStr, ...) CGT_ASSERT_ALWAYS_EX(true, (condition), (fmtStr), ##__VA_ARGS__)
#else
#define CGT_DEBUG_ONLY(stmt)
#define CGT_ASSERT(condition)
#define CGT_ASSERT_MSG(condition, fmtStr, ...)
#define CGT_ASSERT_RECOVERABLE(condition)
#define CGT_ASSERT_RECOVERABLE_MSG(condition, fmtStr, ...)
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