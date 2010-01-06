## Process this file with autoconf to produce configure.
## In general, the safest way to proceed is to run ./autogen.sh

AC_PREREQ(2.59)
AC_INIT([netserver], [1.0], [ microcai@fedoraproject.org ])
AC_CONFIG_SRCDIR([netserver/net.cpp])
AC_CONFIG_HEADERS([config.h])

# autoconf's default CXXFLAGS are usually "-g -O2".  These aren't necessarily
# the best choice for libprotobuf.
AS_IF([test "x${ac_cv_env_CFLAGS_set}" = "x"],
      [CFLAGS=""])
AS_IF([test "x${ac_cv_env_CXXFLAGS_set}" = "x"],
      [CXXFLAGS=""])

AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE

AC_ARG_WITH([zlib],
  [AS_HELP_STRING([--with-zlib],
    [include classes for streaming compressed data in and out @<:@default=check@:>@])],
  [],[with_zlib=check])

AC_ARG_WITH([protoc],
  [AS_HELP_STRING([--with-protoc=COMMAND],
    [use the given protoc command instead of building a new one when building tests (useful for cross-compiling)])],
  [],[with_protoc=no])

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_LANG([C++])
AC_PROG_RANLIB

AM_CONDITIONAL(GCC, test "$GCC" = yes)   # let the Makefile know if we're gcc

# test_util.cc takes forever to compile with GCC and optimization turned on.
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

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h inttypes.h limits.h stdlib.h unistd.h])

# Checks for library functions.
AC_FUNC_MEMCMP
AC_FUNC_STRTOD
AC_CHECK_FUNCS([ftruncate memset mkdir strchr strerror strtol])

# Check for zlib.
HAVE_ZLIB=0


AS_IF([test "$with_zlib" != no],
		[
  			AC_MSG_CHECKING([zlib version])
  			
  			AC_COMPILE_IFELSE(
  						[[
  						#include <zlib.h>
  						#if !defined(ZLIB_VERNUM) || (ZLIB_VERNUM < 0x1204)
  							# error zlib version too old
  						#endif
  						]],
						[
						    AC_MSG_RESULT([ok (1.2.0.4 or later)])
						    AC_SEARCH_LIBS([zlibVersion], [z],
						    		 [
						    		 	AC_DEFINE([HAVE_ZLIB],[1],[Enable classes using zlib compression.])
						    		 	HAVE_ZLIB=1
						    		 ],
						    		 [
						    		   AS_IF([test "$with_zlib" != check],
						    		   	[AC_MSG_FAILURE([--with-zlib was given, but no working zlib library was found])]) 
						    		 ])
						],
						[
							AS_IF([test "$with_zlib" = check],
							 [AC_MSG_RESULT([headers missing or too old (requires 1.2.0.4)])] ,
							 [AC_MSG_FAILURE([--with-zlib was given, but zlib headers were not present or were too old (requires 1.2.0.4)])])				
						
						])		
		])
		
AM_CONDITIONAL([HAVE_ZLIB], [test $HAVE_ZLIB = 1])

AS_IF([test "$with_protoc" != "no"], [
  PROTOC=$with_protoc
  AS_IF([test "$with_protoc" == "yes"], [
    # No argument given.  Use system protoc.
    PROTOC=protoc
  ])
  AS_IF([echo "$PROTOC" | grep -q '^@<:@^/@:>@.*/'], [
    # Does not start with a slash, but contains a slash.  So, it's a relative
    # path (as opposed to an absolute path or an executable in $PATH).
    # Since it will actually be executed from the src directory, prefix with
    # the current directory.  We also insert $ac_top_build_prefix in case this
    # is a nested package and --with-protoc was actually given on the outer
    # package's configure script.
    PROTOC=`pwd`/${ac_top_build_prefix}$PROTOC
  ])
  AC_SUBST([PROTOC])
])
AM_CONDITIONAL([USE_EXTERNAL_PROTOC], [test "$with_protoc" != "no"])

# HACK:  Make gtest's configure script pick up our copy of CFLAGS and CXXFLAGS,
#   since the flags added by ACX_CHECK_SUNCC must be used when compiling gtest
#   too.
export CFLAGS
export CXXFLAGS

CXXFLAGS="-g3 -O0 " 

AC_CHECK_LIB(boost_thread-mt,[main],[],[ AC_MSG_ERROR(must install boost , 1) ])
AC_CHECK_LIB(boost_system-mt,[main],[],[ AC_MSG_ERROR(must install boost , 1) ])
AC_CHECK_LIB(pthread,pthread_create,[],[ AC_MSG_ERROR(must have pthread support  , 1) ])
AC_CHECK_LIB(ssl,[main])

# Checks for typedefs, structures, and compiler characteristics.

AC_CHECK_HEADER([google/protobuf/stubs/common.h],[],[have_google_protobuff=no])

AM_CONDITIONAL(EXTRA_GOOGLE_PROTOBUFF,[ test "x$have_google_protobuff" = "xyes"])
AM_CONDITIONAL(USE_EXTERNAL_PROTOC,[false])
# Checks for header files.

AC_CHECK_HEADER([openssl/bio.h],[AC_DEFINE(SOCKET_SSL,1,[use openssl])])
AC_CHECK_HEADER([pthread.h],[AC_DEFINE(HAVE_PTHREAD,1,[defined on Linux])])

AC_DEFINE(USE_SYNC,1,[use sync])

srcdir=`dirname $0`

AS_IF( test "$srcdir" != "$PWD" , [  
	cp ${srcdir}/netserver/server.pem ${PWD}/netserver/
	cp ${srcdir}/netserver/dh512.pem ${PWD}/netserver/ 
  ]  )
  
AC_CONFIG_FILES([
	Makefile
	protobuf/Makefile	
	netserver/Makefile	
	])

AC_OUTPUT
