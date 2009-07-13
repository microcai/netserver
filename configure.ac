#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([FULL-PACKAGE-NAME], [VERSION], [BUG-REPORT-ADDRESS])
AC_CONFIG_SRCDIR([netserver/net.cpp])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_INSTALL
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.


AC_CHECK_LIB(boost_thread-mt,[main],[],[ AC_MSG_ERROR(must install boost , 1) ])
AC_CHECK_LIB(boost_system-mt,[main],[],[ AC_MSG_ERROR(must install boost , 1) ])
AC_CHECK_LIB(pthread,pthread_create,[],[ AC_MSG_ERROR(must have pthread support  , 1) ])

# Checks for typedefs, structures, and compiler characteristics.



# Checks for header files.
# Checks for library functions.
AC_CONFIG_FILES([Makefile  netserver/Makefile ])

AC_OUTPUT
