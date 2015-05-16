#pragma once

#if defined(_WIN32)

#define RESID unsigned long

typedef unsigned long DWORD;

#include "winres\\messages.h"

#elif defined(__APPLE__) // #if defined(_WIN32)

#include <CoreFoundation/CFString.h>

#define RESID CFStringRef

#include "osxres/messages.h"

#endif // #if defined(_WIN32)
