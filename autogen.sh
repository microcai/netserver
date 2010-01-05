#!/bin/sh
set -x
aclocal
autoheader
#cp /usr/share/libtool/config/ltmain.sh ./
automake --add-missing --copy
autoconf


