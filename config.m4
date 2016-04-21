dnl $Id$
dnl config.m4 for extension kyureki

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(kyureki, for kyureki support,
dnl Make sure that the comment is aligned:
dnl [  --with-kyureki             Include kyureki support])

dnl Otherwise use enable:

PHP_ARG_ENABLE(kyureki7, whether to enable kyureki7 support,
Make sure that the comment is aligned:
[  --enable-kyureki7           Enable kyureki7 support])

if test "$PHP_KYUREKI" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-kyureki -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/kyureki.h"  # you most likely want to change this
  dnl if test -r $PHP_KYUREKI/$SEARCH_FOR; then # path given as parameter
  dnl   KYUREKI_DIR=$PHP_KYUREKI
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for kyureki files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       KYUREKI_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$KYUREKI_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the kyureki distribution])
  dnl fi

  dnl # --with-kyureki -> add include path
  dnl PHP_ADD_INCLUDE($KYUREKI_DIR/include)

  dnl # --with-kyureki -> check for lib and symbol presence
  dnl LIBNAME=kyureki # you may want to change this
  dnl LIBSYMBOL=kyureki # you most likely want to change this

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $KYUREKI_DIR/$PHP_LIBDIR, KYUREKI_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_KYUREKILIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong kyureki lib version or lib not found])
  dnl ],[
  dnl   -L$KYUREKI_DIR/$PHP_LIBDIR -lm
  dnl ])
  dnl
  dnl PHP_SUBST(KYUREKI_SHARED_LIBADD)

  dnl PHP_NEW_EXTENSION(kyureki, kyureki.c, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
  PHP_NEW_EXTENSION(kyureki, KyurekiCalc.cpp kyureki.c, $ext_shared)
fi
