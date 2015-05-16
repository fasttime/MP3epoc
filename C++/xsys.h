#pragma once

#ifdef _WIN32

#define xcerr           wcerr
#define xchar           wchar_t
#define xcmatch         wcmatch
#define xcout           wcout
#define xistringstream  wistringstream
#define xostringstream  wostringstream
#define xmain           wmain
#define xregex          wregex
#define XSTR(str)       XSTR_(str)
#define XSTR_(str)      L ## str
#define xstring         wstring

#else // #ifdef _WIN32

#define xcerr           cerr
#define xchar           char
#define xcmatch         cmatch
#define xcout           cout
#define xistringstream  istringstream
#define xostringstream  ostringstream
#define xmain           main
#define xregex          regex
#define XSTR(str)       str
#define xstring         string

#endif

#if _MSC_VER > 1200
#define DEFAULT_UNREACHABLE default: __assume(0)
#elif defined __clang__ // #if _MSC_VER > 1200
#define DEFAULT_UNREACHABLE default: __builtin_unreachable()
#endif // #if _MSC_VER > 1200
