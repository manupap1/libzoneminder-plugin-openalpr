AC_PREREQ([2.60])
AC_INIT([libzm_plugin_openalpr],[1.0.0],,)
AC_CONFIG_AUX_DIR([.])

AM_INIT_AUTOMAKE([foreign])

AC_ENABLE_SHARED
AC_DISABLE_STATIC
LT_PREREQ([2.4.2])
LT_INIT([dlopen])

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([src/openalpr_plugin.h])
AC_CONFIG_HEADERS([config.h])

AC_SUBST([AM_CXXFLAGS], [-D__STDC_CONSTANT_MACROS])
AC_SUBST([VERSION])

AC_CANONICAL_HOST

# Check for which host we are on and setup a few things
# specifically based on the host
case $host_os in
    darwin* )
        # Do something specific for mac
        HOST_OS='darwin'
        ;;
    linux*)
        # Do something specific for linux
        HOST_OS='linux'
        ;;
    *BSD*)
        # Do something specific for BSD
        HOST_OS='BSD'
        ;;
    *)
        #Default Case
        AC_MSG_ERROR([Your platform is not currently supported])
        ;;
esac
AC_SUBST([HOST_OS])

LIB_ARCH=lib
AC_ARG_WITH([libarch],
    [  --with-libarch=<lib>       architecture library path to use, default lib],
    [LIB_ARCH=$with_libarch],
    [AC_MSG_WARN([You can call configure with the --with-libarch option.
    This tells configure where to find architecture specific libraries.
    The default of 'lib' is usually ok but 64 bit machines may require lib64.
    e.g. --with-libarch=lib or --with-libarch=lib64])]
)
AC_SUBST([LIB_ARCH])

ENABLE_DEBUG=yes
AC_ARG_ENABLE(debug,
    [  --enable-debug=<yes|no>    enable or disable debug, default enabled],
    [ENABLE_DEBUG=$enable_debug],
    [AC_MSG_WARN([You can call configure with the --enable-debug=<yes|no> or --disable-debug option.
    This tells configure whether to compile ZoneMinder with debug included. Although debug is included
    by default it is not output unless explicitly switched on elsewhere. These checks may induce a
    small penalty on performance and if you are after squeezing the maximum possible performance out
    of ZoneMinder you may use this switch to prevent debug from being compiled in.
    e.g. --enable-debug=yes or --disable-debug])]
)
if test "$ENABLE_DEBUG" != "yes"; then
  AC_DEFINE([ZM_DBG_OFF],[1],[Whether debug is switched off and compiled out])
fi

# Compiler
AC_LANG_CPLUSPLUS

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_TYPE_UID_T
AC_C_INLINE
AC_TYPE_MODE_T
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_STRUCT_TM
AC_TYPE_SIGNAL

AC_CHECK_TYPES(siginfo_t,,,[#include <signal.h>])
AC_CHECK_TYPES(ucontext_t,,,[#include <signal.h>])

# Checks for library functions.
AC_PROG_GCC_TRADITIONAL
AC_FUNC_MALLOC
AC_FUNC_MMAP
AC_FUNC_SELECT_ARGTYPES
AC_FUNC_STAT
AC_FUNC_STRFTIME
AC_FUNC_STRTOD
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([gethostbyname gethostname gettimeofday memmove memset mkdir munmap posix_memalign putenv select sendfile socket sqrt strcasecmp strchr strcspn strerror strncasecmp strrchr strspn strstr strtol strtoull])
AC_CHECK_FUNCS([syscall sleep usleep ioctl ioctlsocket sigaction])

# Checks for libraries.
PKG_CHECK_MODULES([LIBZMPLUGINS],
    [libzmplugins >= 1.28],
    [def_pkglibdir=$($PKG_CONFIG --variable=pkglibdir libzmplugins)
     AS_IF([test "x$def_pkglibdir" = "x"],
         [AC_MSG_ERROR([Unable to find plugin library directory])],
         [AC_SUBST([pkglibdir], [$def_pkglibdir])])
     def_sysconfdir=$($PKG_CONFIG --variable=sysconfdir libzmplugins)
     AS_IF([test "x$def_sysconfdir" = "x"],
         [AC_MSG_ERROR([Unable to find plugin configuration directory])],
         [AC_SUBST([sysconfdir], [$def_sysconfdir])])
     def_webdir=$($PKG_CONFIG --variable=webdir libzmplugins)
     AS_IF([test "x$def_webdir" = "x"],
         [AC_MSG_ERROR([Unable to find plugin web directory])],
         [AC_SUBST([webdir], [$def_webdir]/$PACKAGE)])],
    [AC_MSG_ERROR([${LIBZMPLUGINS_PKG_ERRORS}])]
)
AC_CHECK_LIB([zmplugins],[main],,[AC_MSG_ERROR([libzmplugins is required for plugin compilation])])
PKG_CHECK_MODULES([OPENCV],[opencv >= 2.3.1],,[AC_MSG_ERROR([${OPENCV_PKG_ERRORS}])])
AC_CHECK_LIB([opencv_core],[main],,[AC_MSG_ERROR([libopencv_core is required for plugin compilation])])
AC_CHECK_LIB([opencv_imgproc],[main],,[AC_MSG_ERROR([libopencv_imgproc is required for plugin compilation])])
AC_CHECK_LIB([opencv_highgui],[main],,[AC_MSG_ERROR([libopencv_highgui is required for plugin compilation])])
AC_CHECK_LIB([opencv_objdetect],[main],,[AC_MSG_ERROR([libopencv_objdetect is required for plugin compilation])])
AC_CHECK_LIB([boost_program_options],[main],,[AC_MSG_ERROR([libboost_program_options is required for plugin compilation])])

# Checks for header files.
AC_FUNC_ALLOCA
AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h limits.h memory.h stddef.h stdlib.h string.h strings.h sys/param.h sys/time.h syslog.h unistd.h values.h])
AC_CHECK_HEADERS([netdb.h netinet/in.h arpa/inet.h sys/ioctl.h sys/socket.h sys/un.h glob.h sys/sendfile.h])
AC_CHECK_HEADERS(execinfo.h,,,)
AC_CHECK_HEADERS(ucontext.h,,,)
AC_CHECK_HEADERS(sys/syscall.h,,,)
AC_CHECK_HEADERS(pthread.h,,,)
AC_LANG_PUSH([C])
AC_LANG_POP([C])

AC_CHECK_HEADERS(opencv2/core/core.hpp,,AC_MSG_ERROR(plugins require opencv core headers - check that development package is installed),)
AC_CHECK_HEADERS(opencv2/imgproc/imgproc.hpp,,AC_MSG_ERROR(plugins require opencv imgproc headers - check that development package is installed),)
AC_CHECK_HEADERS(opencv2/highgui/highgui.hpp,,AC_MSG_ERROR(plugins require opencv highgui headers - check that development package is installed),)
AC_CHECK_HEADERS(opencv2/objdetect/objdetect.hpp,,AC_MSG_ERROR(plugins require opencv objdetect headers - check that development package is installed),)
AC_CHECK_HEADERS(boost/program_options.hpp,,AC_MSG_ERROR(plugins require boost program_options headers - check that development package is installed),)

AC_CHECK_DECLS(backtrace,,,[#include <execinfo.h>])
AC_CHECK_DECLS(backtrace_symbols,,,[#include <execinfo.h>])

AC_SUBST(LDFLAGS)

AC_CONFIG_FILES([Makefile src/Makefile data/Makefile])

AC_OUTPUT
