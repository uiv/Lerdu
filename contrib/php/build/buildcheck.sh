#! /bin/sh
#  +----------------------------------------------------------------------+
#  | PHP Version 5                                                        |
#  +----------------------------------------------------------------------+
#  | Copyright (c) 1997-2007 The PHP Group                                |
#  +----------------------------------------------------------------------+
#  | This source file is subject to version 3.01 of the PHP license,      |
#  | that is bundled with this package in the file LICENSE, and is        |
#  | available through the world-wide-web at the following url:           |
#  | http://www.php.net/license/3_01.txt                                  |
#  | If you did not receive a copy of the PHP license and are unable to   |
#  | obtain it through the world-wide-web, please send a note to          |
#  | license@php.net so we can mail you a copy immediately.               |
#  +----------------------------------------------------------------------+
#  | Authors: Stig Bakken <ssb@php.net>                                   |
#  |          Sascha Schumann <sascha@schumann.cx>                        |
#  +----------------------------------------------------------------------+
#
# $Id: buildcheck.sh,v 1.37.2.2.2.1 2007-01-01 19:32:10 iliaa Exp $ 
#

echo "buildconf: checking installation..."

stamp=$1

# Allow the autoconf executable to be overridden by $PHP_AUTOCONF.
if test -z "$PHP_AUTOCONF"; then
  PHP_AUTOCONF='autoconf'
fi

# autoconf 2.13 or newer
ac_version=`$PHP_AUTOCONF --version 2>/dev/null|head -n 1|sed -e 's/^[^0-9]*//' -e 's/[a-z]* *$//'`
if test -z "$ac_version"; then
echo "buildconf: autoconf not found."
echo "           You need autoconf version 2.13 or newer installed"
echo "           to build PHP from SVN."
exit 1
fi
IFS=.; set $ac_version; IFS=' '
if test "$1" = "2" -a "$2" -lt "13" || test "$1" -lt "2"; then
echo "buildconf: autoconf version $ac_version found."
echo "           You need autoconf version 2.13 or newer installed"
echo "           to build PHP from SVN."
exit 1
fi

if test "$1" = "2" -a "$2" -gt "59"; then
  echo "buildconf: You need autoconf 2.59 or lower to build this version of PHP."
  echo "           You are currently trying to use $ac_version"
  echo "           Most distros have separate autoconf 2.13 or 2.59 packages."
  echo "           On Debian/Ubuntu both autoconf2.13 and autoconf2.59 packages exist."
  echo "           Install autoconf2.13 and set the PHP_AUTOCONF env var to "
  echo "           autoconf2.13 and try again."
  exit 1
else
  echo "buildconf: autoconf version $ac_version (ok)"
fi

if test "$1" = "2" -a "$2" -ge "50"; then
  ./vcsclean
  stamp=
fi

test -n "$stamp" && touch $stamp

exit 0
