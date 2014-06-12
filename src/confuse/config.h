/* config.h for confuse */

#define PACKAGE "confuse"
#define PACKAGE_BUGREPORT "confuse-devel@nongnu.org"
#define PACKAGE_NAME "libConfuse"
#define PACKAGE_STRING "libConfuse 2.5"
#define PACKAGE_TARNAME "confuse"
#define PACKAGE_VERSION "2.5"
#define VERSION "2.5"

// Linux
#if defined(__linux__) || defined(__EMSCRIPTEN__)
	#define HAVE_DCGETTEXT 1
	#define HAVE_DLFCN_H 1
	#define HAVE_GETTEXT 1
	#define HAVE_INTTYPES_H 1
	#define HAVE_MEMORY_H 1
	#define HAVE_STDINT_H 1
	#define HAVE_STDLIB_H 1
	#define HAVE_STRCASECMP 1
	#define HAVE_STRDUP 1
	#define HAVE_STRINGS_H 1
	#define HAVE_STRING_H 1
	#define HAVE_STRNDUP 1
	#define HAVE_SYS_STAT_H 1
	#define HAVE_SYS_TYPES_H 1
	#define HAVE_UNISTD_H 1
	#define YYTEXT_POINTER 1
#endif

// Windows
#ifdef _WIN32
	#define HAVE_MEMORY_H 1
	#define HAVE_STDINT_H 1
	#define HAVE_STDLIB_H 1
	#define HAVE__STRDUP 1
	#define HAVE_STRING_H 1
	#undef HAVE_SYS_STAT_H
	#define HAVE_SYS_TYPES_H 1
	#define STDC_HEADERS 1
	#define HAVE__FILENO 1
	#define HAVE__ISATTY 1
#endif



