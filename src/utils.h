#if !defined UTILS_H
#define UTILS_H

#define DEBUG_LEVEL_NOTHING 0
#define DEBUG_LEVEL_ERRORS 1
#define DEBUG_LEVEL_WARNINGS 2
#define DEBUG_LEVEL_INFO 3
#define DEBUG_LEVEL_VERBOSE 4

#if !defined DEBUG_LEVEL
#define DEBUG_LEVEL DEBUG_LEVEL_ERRORS
#endif

#define LOG_ERROR(...) do { \
    if (DEBUG_LEVEL >= DEBUG_LEVEL_ERRORS) { \
        Serial.print("[ERROR] "); \
        Serial.println(__VA_ARGS__); \
    } \
} while (0)

#define LOG_WARNING(...) do { \
    if (DEBUG_LEVEL >= DEBUG_LEVEL_WARNINGS) { \
        Serial.print("[WARNING] "); \
        Serial.println(__VA_ARGS__); \
    } \
} while (0)

#define LOG_INFO(...) do { \
    if (DEBUG_LEVEL >= DEBUG_LEVEL_INFO) { \
        Serial.print("[INFO] "); \
        Serial.println(__VA_ARGS__); \
    } \
} while (0)

#define LOG_VERBOSE(...) do { \
    if (DEBUG_LEVEL >= DEBUG_LEVEL_VERBOSE) { \
        Serial.print("[VERBOSE] "); \
        Serial.println(__VA_ARGS__); \
    } \
} while (0)

#define clamp(num, minV, maxV) (max(min((num), (maxV)), (minV)))

#endif
