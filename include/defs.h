// Define current OS
#if defined( __WIN32__ ) || defined( _WIN32 )
#   define SKETCHTESTER_OS_WIN32
#else
#   define SKETCHTESTER_OS_LINUX
#endif

/* Find the arch type */
#if defined(__x86_64__) || defined(_M_X64) || defined(__powerpc64__) || defined(__alpha__) || defined(__ia64__) || defined(__s390__) || defined(__s390x__)
#   define SKETCHTESTER_ARCH_64
#else
#   define SKETCHTESTER_ARCH_32
#endif