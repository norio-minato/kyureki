
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_kyureki.h"
#include "KyurekiCalc.hpp"

/* If you declare any globals in php_kyureki.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(kyureki)
*/

PHP_FUNCTION(get_kyureki);
PHP_FUNCTION(get_rokuyou);

/* True global resources - no need for thread safety here */
static int le_kyureki;

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("kyureki.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_kyureki_globals, kyureki_globals)
    STD_PHP_INI_ENTRY("kyureki.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_kyureki_globals, kyureki_globals)
PHP_INI_END()
*/
/* }}} */

/* Remove the following function when you have successfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */

/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_kyureki_compiled(string arg)
   Return a string to confirm that the module is compiled in */
PHP_FUNCTION(confirm_kyureki_compiled)
{
	char *arg = NULL;
	size_t arg_len, len;
	zend_string *strg;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &arg, &arg_len) == FAILURE) {
		return;
	}

	strg = strpprintf(0, "Congratulations! You have successfully modified ext/%.78s/config.m4. Module %.78s is now compiled into PHP.", "kyureki", arg);

	RETURN_STR(strg);
}
/* }}} */
/* The previous line is meant for vim and emacs, so it can correctly fold and
   unfold functions in source code. See the corresponding marks just before
   function definition, where the functions purpose is also documented. Please
   follow this convention for the convenience of others editing your code.
*/


/* {{{ php_kyureki_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_kyureki_init_globals(zend_kyureki_globals *kyureki_globals)
{
	kyureki_globals->global_value = 0;
	kyureki_globals->global_string = NULL;
}
*/
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(kyureki)
{
	/* If you have INI entries, uncomment these lines
	REGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(kyureki)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(kyureki)
{
#if defined(COMPILE_DL_KYUREKI) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(kyureki)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(kyureki)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "kyureki support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */

/* {{{ kyureki_functions[]
 *
 * Every user visible function must have an entry in kyureki_functions[].
 */
const zend_function_entry kyureki_functions[] = {
	PHP_FE(get_kyureki,	NULL)		/* For testing, remove later. */
	PHP_FE(get_rokuyou,	NULL)		/* For testing, remove later. */
	PHP_FE(confirm_kyureki_compiled,	NULL)		/* For testing, remove later. */
	PHP_FE_END	/* Must be the last line in kyureki_functions[] */
};
/* }}} */

/* {{{ kyureki_module_entry
 */
zend_module_entry kyureki_module_entry = {
	STANDARD_MODULE_HEADER,
	"kyureki",
	kyureki_functions,
	PHP_MINIT(kyureki),
	PHP_MSHUTDOWN(kyureki),
	PHP_RINIT(kyureki),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(kyureki),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(kyureki),
	PHP_KYUREKI_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_KYUREKI
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(kyureki)
#endif

PHP_FUNCTION(get_kyureki)
{
    long arg_year;
    long arg_month;
    long arg_day;
    int len;
    char *strg;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lll", &arg_year, &arg_month, &arg_day) == FAILURE) {
        return;
    }

    KyurekiCalc *kyureki_calc = new KyurekiCalc();
    const vector<int> kyureki = kyureki_calc->getKyureki(arg_year, arg_month, arg_day);
    delete kyureki_calc;

    // len = spprintf(&strg, 0, "%d:%d:%d:%d", kyureki[0], kyureki[2], kyureki[3], kyureki[1]);
    // RETURN_STRINGL(strg, len, 0);

	array_init(return_value);
	add_index_double(return_value, 0, kyureki[0]);
	add_index_double(return_value, 1, kyureki[2]);
	add_index_double(return_value, 2, kyureki[3]);
	add_index_double(return_value, 3, kyureki[4]);
}

PHP_FUNCTION(get_rokuyou)
{
    long arg_year;
    long arg_month;
    long arg_day;
    int len;
    char *strg;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lll", &arg_year, &arg_month, &arg_day) == FAILURE) {
      return;
    }

    KyurekiCalc *kyureki_calc = new KyurekiCalc();
    int rokuyou = kyureki_calc->getRokuyou(arg_year, arg_month, arg_day);
    delete kyureki_calc;

    RETURN_DOUBLE(rokuyou)
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
