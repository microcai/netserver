#!/bin/sh
set -x
aclocal
autoheader
cp /usr/share/libtool/config/ltmain.sh ./
automake --foreign --add-missing --copy
autoconf


