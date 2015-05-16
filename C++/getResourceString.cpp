#include "Finally.h"
#include "getResourceString.h"

#if defined(_WIN32)

#include "Windows API.h"

#include <regex>
#include <sstream>

using namespace std;

wstring fixNewlines(const wchar_t * text);

namespace
{
    wregex fixNewlinesPattern(L"\r\n?");
}

wstring fixNewlines(const wchar_t * text)
{
    wstring result(regex_replace(text, fixNewlinesPattern, L"\n"));
    return result;
}

xstring getResourceString(RESID id, ...)
{
    va_list args;
    va_start(args, id);
    {
        wchar_t * buffer = NULL;
        DWORD result =
            FormatMessageW(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_HMODULE,
            NULL,
            id,
            0,
            reinterpret_cast<LPWSTR>(&buffer),
            0,
            &args
            );
        Finally fin(
            [&buffer]
            {
                if (buffer != NULL) LocalFree(buffer);
            }
            );
        va_end(args);
        if (result > 0) return fixNewlines(buffer);
    }
    return wstring();
}

#elif defined(__APPLE__) // #if defined(_WIN32)

#include <CoreFoundation/CoreFoundation.h>

using namespace std;

inline xstring CFStringToUTF8String(CFStringRef cfstr)
{
    if (cfstr)
    {
        CFIndex length = CFStringGetLength(cfstr);
        CFIndex maxSize =
            CFStringGetMaximumSizeForEncoding(
            length,
            kCFStringEncodingUTF8
            );
        char * buffer = static_cast<char *>(alloca(maxSize));
        if (
            CFStringGetCString(
            cfstr,
            buffer,
            maxSize,
            kCFStringEncodingUTF8))
        {
            xstring result(buffer);
            return result;
        }
    }
    return xstring();
}

xstring getResourceString(RESID id, ...)
{
    class AutoCFStringRef
    {
    public:
        AutoCFStringRef(CFStringRef str): str(str) { }
        
        ~AutoCFStringRef()
        {
            if (str != NULL)
            {
                CFRelease(str);
                str = NULL;
            }
        }
        
        operator CFStringRef ()
        {
            return str;
        }
        
    private:
        CFStringRef str;
    };
    
    AutoCFStringRef format = CFCopyLocalizedString(id, NULL);
    va_list args;
    va_start(args, id);
    AutoCFStringRef value =
        CFStringCreateWithFormatAndArguments(NULL, NULL, format, args);
    format.~AutoCFStringRef();
    va_end(args);
    xstring result = CFStringToUTF8String(value);
    return result;
}

#endif // #if defined(_WIN32)

xstring quotePath(const xstring & path)
{
    xstring quotedPath;
    quotedPath.reserve(path.length() + 2);
    return quotedPath.append(XSTR("\"")).append(path).append(XSTR("\""));
}
