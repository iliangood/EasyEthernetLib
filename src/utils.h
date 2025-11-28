#if !defined UTILS_H
#define UTILS_H

#include <Arduino.h>
#include <Arduino_DebugUtils.h>

#if !defined DEBUG_ERROR
#define DEBUG_ERROR(...)      ((void)0)
#endif

#if !defined DEBUG_WARNING
#define DEBUG_WARNING(...)    ((void)0)
#endif

#if !defined DEBUG_INFO
#define DEBUG_INFO(...)       ((void)0)
#endif

#if !defined DEBUG_DEBUG
#define DEBUG_DEBUG(...)      ((void)0)
#endif

#if !defined DEBUG_VERBOSE
#define DEBUG_VERBOSE(...)    ((void)0)
#endif

#define clamp(num, minV, maxV) (max(min((num), (maxV)), (minV)))

#define IS_TRIVIALLY_COPYABLE(T) __has_trivial_copy(T)

#endif
