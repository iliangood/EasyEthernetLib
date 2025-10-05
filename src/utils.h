#if !defined UTILS_H
#define UTILS_H

#include <Arduino.h>
#include <Arduino_DebugUtils.h>


#define clamp(num, minV, maxV) (max(min((num), (maxV)), (minV)))


#define IS_TRIVIALLY_COPYABLE(T) __has_trivial_copy(T)

#endif
