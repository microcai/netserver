#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([netserver], [1.0], [ microcai@fedoraproject.org ])
AC_CONFIG_SRCDIR([netserver/net.cpp])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE


AC_ARG_WITH([zlib],
  [AS_HELP_STRING([--with-zlib],
    [include classes for streaming compressed data in and out @<:@default=check@:>@])],
  [],[with_zlib=check])

CXXFLAGS="-g3 -O0 " 

# Checks for programs.
AC_PROG_INSTALL
AC_PROG_CC
AC_PROG_CXX

AC_MSG_CHECKING([C++ compiler flags...])
AS_IF([test "x${ac_cv_env_CXXFLAGS_set}" = "x"],[
  AS_IF([test "$GCC" = "yes"],[
    PROTOBUF_OPT_FLAG="-O2"
    CXXFLAGS="${CXXFLAGS} -g"
  ])

  # Protocol Buffers contains several checks that are intended to be used only
  # for debugging and which might hurt performance.  Most users are probably
  # end users who don't want these checks, so add -DNDEBUG by default.
  CXXFLAGS="$CXXFLAGS -DNDEBUG"

  AC_MSG_RESULT([use default: $PROTOBUF_OPT_FLAG $CXXFLAGS])
],[
  AC_MSG_RESULT([use user-supplied: $CXXFLAGS])
])


AC_SUBST(PROTOBUF_OPT_FLAG)

AC_PROG_LIBTOOL

AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h inttypes.h limits.h stdlib.h unistd.h])

# Checks for library functions.
AC_FUNC_MEMCMP
AC_FUNC_STRTOD
AC_CHECK_FUNCS([ftruncate memset mkdir strchr strerror strtol])

# Checks for libraries.
# Check for zlib.
HAVE_ZLIB=0

AC_CHECK_LIB(libz,[main],[

	AC_CHECK_HEADER(zlib.h,[HAVE_ZLIB=1],[HAVE_ZLIB=0])

	],[HAVE_ZLIB=0])

AM_CONDITIONAL([HAVE_ZLIB], [test $HAVE_ZLIB = 1])

AC_CHECK_LIB(boost_thread-mt,[main],[],[ AC_MSG_ERROR(must install boost , 1) ])
AC_CHECK_LIB(boost_system-mt,[main],[],[ AC_MSG_ERROR(must install boost , 1) ])
AC_CHECK_LIB(pthread,pthread_create,[],[ AC_MSG_ERROR(must have pthread support  , 1) ])
AC_CHECK_LIB(ssl,[main],[],[])

# Checks for typedefs, structures, and compiler characteristics.

AC_CHECK_HEADER([google/protobuf/stubs/common.h],[],[have_google_protobuff=no])

AM_CONDITIONAL(EXTRA_GOOGLE_PROTOBUFF,[ test "x$have_google_protobuff" = "xyes"])
AM_CONDITIONAL(USE_EXTERNAL_PROTOC,[0])
# Checks for header files.

AC_CHECK_HEADER([pthread.h],[AC_DEFINE(HAVE_PTHREAD,1,[defined on linuxz])])

# Checks for library functions.
ACX_PTHREAD

AC_DEFINE(USE_SYNC,1,[use sync])
AC_DEFINE(SOCKET_SSL,1,[use openssl])

AC_CONFIG_FILES([
	Makefile
	protobuf/Makefile	
	netserver/Makefile	
	])

AC_OUTPUT
