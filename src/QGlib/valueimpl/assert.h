#if !defined(VALUE_ASSERT)
# if !defined(QGST_NO_VALUE_CHECKS)
#  if !defined(QGST_NO_FATAL_VALUE_CHECKS)
#   define VALUE_ASSERT(condition) \
        do { \
            if (!(condition)) { \
                qFatal("ASSERT: %s\n", #condition); \
            } \
        } while(0)
#  else
#   define VALUE_ASSERT(condition) \
        do { \
            if (!(condition)) { \
                qWarning("ASSERT: %s\n", #condition); \
            } \
        } while(0)
#  endif
# else
#  define VALUE_ASSERT(condition) \
        do { \
            if (0 && (condition)) {}  \
        } while(0)
# endif
#endif
