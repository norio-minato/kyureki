
#ifndef PHP_KYUREKI_H
#define PHP_KYUREKI_H

extern zend_module_entry kyureki_module_entry;
#define phpext_kyureki_ptr &kyureki_module_entry

#define PHP_KYUREKI_VERSION "0.1.0" /* Replace with version number for your extension */

#ifdef PHP_WIN32
#	define PHP_KYUREKI_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_KYUREKI_API __attribute__ ((visibility("default")))
#else
#	define PHP_KYUREKI_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

/*
  	Declare any global variables you may need between the BEGIN
	and END macros here:

ZEND_BEGIN_MODULE_GLOBALS(kyureki)
	zend_long  global_value;
	char *global_string;
ZEND_END_MODULE_GLOBALS(kyureki)
*/

/* Always refer to the globals in your function as KYUREKI_G(variable).
   You are encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/
#define KYUREKI_G(v) ZEND_MODULE_GLOBALS_ACCESSOR(kyureki, v)

#if defined(ZTS) && defined(COMPILE_DL_KYUREKI)
ZEND_TSRMLS_CACHE_EXTERN()
#endif

#endif	/* PHP_KYUREKI_H */

