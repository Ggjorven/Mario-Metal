#pragma once

#include <Nano/Nano.hpp>

namespace Obsidian
{

    ////////////////////////////////////////////////////////////////////////////////////
    // Logging & Asserting macros
    ////////////////////////////////////////////////////////////////////////////////////
    #ifndef MM_CONFIG_DIST
        #define MM_LOG_TRACE(fmt, ...)       ::Nano::Log::PrintLvl<::Nano::Log::Level::Trace>(fmt __VA_OPT__(,) __VA_ARGS__)
        #define MM_LOG_INFO(fmt, ...)        ::Nano::Log::PrintLvl<::Nano::Log::Level::Info>(fmt __VA_OPT__(,) __VA_ARGS__)
        #define MM_LOG_WARN(fmt, ...)        ::Nano::Log::PrintLvl<::Nano::Log::Level::Warn>(fmt __VA_OPT__(,) __VA_ARGS__)
        #define MM_LOG_ERROR(fmt, ...)       ::Nano::Log::PrintLvl<::Nano::Log::Level::Error>(fmt __VA_OPT__(,) __VA_ARGS__)
        #define MM_LOG_FATAL(fmt, ...)       ::Nano::Log::PrintLvl<::Nano::Log::Level::Fatal>(fmt __VA_OPT__(,) __VA_ARGS__)

        #define MM_ASSERT(x, fmt, ...)          \
                    do                          \
                    {                           \
                        if (!(x))               \
                        {                       \
                            MM_LOG_FATAL("Assertion failed: ({0}), {1}.", #x, ::Nano::Text::Format(fmt __VA_OPT__(,) __VA_ARGS__)); \
                            NANO_DEBUG_BREAK(); \
                        }                       \
                    } while (false)

        #define MM_VERIFY(x, fmt, ...)          \
                    do                          \
                    {                           \
                        if (!(x))               \
                        {                       \
                            MM_LOG_FATAL("Verify failed: ({0}), {1}.", #x, ::Nano::Text::Format(fmt __VA_OPT__(,) __VA_ARGS__)); \
                        }                       \
                    } while (false)

        #define MM_UNREACHABLE() MM_ASSERT(false, "Unreachable")

    #else
        #define MM_LOG_TRACE(fmt, ...) 
        #define MM_LOG_INFO(fmt, ...) 
        #define MM_LOG_WARN(fmt, ...) 
        #define MM_LOG_ERROR(fmt, ...) 
        #define MM_LOG_FATAL(fmt, ...) 

        #define MM_ASSERT(x, fmt, ...)
        #define MM_VERIFY(x, fmt, ...)
        #define MM_UNREACHABLE()
    #endif

}