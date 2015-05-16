#pragma once

#include "xsys.h"

#define XQUOTE(token)       XQUOTE_(token)
#define XQUOTE_(token)      XSTR(#token)

#define APP_MAJOR_VERSION   1
#define APP_MINOR_VERSION   3
#define APP_REVISION        0
#define APP_NAME            XSTR("MP3epoc")
#define APP_COMPANY         XSTR("fasttime.org")
#define APP_COPYRIGHT       XSTR("Copyright 2005-2014 Francesco Trotta")

#if APP_REVISION

#define APP_VERSION         XQUOTE(APP_MAJOR_VERSION) XSTR(".") \
                            XQUOTE(APP_MINOR_VERSION) XSTR(".") \
                            XQUOTE(APP_REVISION)

#else // #if APP_REVISION

#define APP_VERSION         XQUOTE(APP_MAJOR_VERSION) XSTR(".") \
                            XQUOTE(APP_MINOR_VERSION)

#endif // #if APP_REVISION
