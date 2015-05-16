#define _CRT_SECURE_NO_WARNINGS
#define CATCH_CONFIG_RUNNER

#pragma warning (push)
#pragma warning (disable: 4512)

#include "catch.hpp"

#pragma warning (pop)

#include "countLeastSignificantZeros.h"
#include "Finally.h"
#include "findAllFilePaths.h"
#include "MP3FormatException.h"
#include "processFile.h"
#include "shrinkTextWidth.h"

#include <functional>
#include <exception>
#include <regex>
#include <sys/stat.h>

#ifdef __APPLE__

#include <CoreFoundation/CoreFoundation.h>

#endif // #ifdef __APPLE__

using namespace MP3epoc;
using namespace std;

#if defined(_WIN32)

#define HYPHEN          L"\x2010"
#define AUML            L"\xe4"
#define OUML            L"\xf6"
#define UUML            L"\xfc"

#define DIR_SEPARATOR   L"\\"

#elif defined(__APPLE__) // #if defined(_WIN32)

#define HYPHEN          "\xe2\x80\x90"
#define AUML            "\xc3\xa4"
#define OUML            "\xc3\xb6"
#define UUML            "\xc3\xbc"

#define DIR_SEPARATOR   "/"

#endif // #if defined(_WIN32)

void createDir(const xstring & path);
xstring getTempDir();

#if defined(_WIN32)

void createDir(const xstring & path)
{
    CreateDirectoryW(path.c_str(), NULL);
}

xstring getTempDir()
{
    WCHAR buffer[MAX_PATH];
    GetTempPathW(MAX_PATH, buffer);
    return wstring(buffer).append(_wtmpnam(nullptr));
}

#elif defined(__APPLE__) // #if defined(_WIN32)

void createDir(const xstring & path)
{
    mkdir(path.c_str(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
}

xstring getTempDir()
{
    char tempDir[] = "/tmp/MP3epoc_XXXXXX";
    mktemp(tempDir);
    return string(tempDir);
};

#endif // #if defined(_WIN32)

const xstring tempDir = getTempDir();

int main (int argc, char * const argv[])
{
    createDir(tempDir);

    int returnCode = Catch::Session().run(argc, argv);

#ifdef _MSC_VER

    _getwch();

#endif // #ifdef _MSC_VER

    return returnCode;
}

xstring getRawString(RESID id);

#if defined(_WIN32)

#define xsregex_iterator wsregex_iterator

wstring fixNewlines(const wchar_t * text);

xstring getRawString(RESID id)
{
    wchar_t * buffer = NULL;
    DWORD result =
        FormatMessageW(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_HMODULE |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        id,
        0,
        reinterpret_cast<LPWSTR>(&buffer),
        0,
        NULL
        );
    Finally fin(
        [&buffer]
        {
            if (buffer != NULL) LocalFree(buffer);
        }
        );
    if (result > 0) return fixNewlines(buffer);
    return wstring();
}

#elif defined(__APPLE__) // #if defined(_WIN32)

#define xsregex_iterator sregex_iterator

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

xstring getRawString(RESID id)
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
    xstring result = CFStringToUTF8String(format);
    return result;
}

#endif // #if defined(_WIN32)

template <typename T>
void doReplace(
    xostringstream & ostream,
    xsregex_iterator & iterator,
    typename remove_reference<T>::type & arg,
    size_t & end)
{
    if (iterator == xsregex_iterator())
        throw invalid_argument("no more tokens to replace");
    xstring prefix = iterator->prefix();
    ostream << prefix << arg;
    end = iterator->position() + iterator->length();
    ++iterator;
}

template <typename... Args>
xstring getExpectedMessage(RESID id, Args... args)
{
    xstring text = getRawString(id);
    xregex token(XSTR("%\\d(!\\w+!|\\$\\w+)?"));
    auto iterator = xsregex_iterator(text.begin(), text.end(), token);
    xostringstream ostream;
    size_t end = 0;
    [] (initializer_list<int>) { }
    ({ (doReplace<Args>(ostream, iterator, args, end), 0)... });
    if (iterator != xsregex_iterator())
        throw invalid_argument("not enough replacements");
    ostream << text.substr(end);
    xstring message = ostream.str();
    return message;
}

////////////////////////////////////////////////////////////////////////////////
// shrinkTextWidth

inline void testShrinkTextWidth(
    const xchar * text,
    int width,
    const xchar * expectedResult)
{
    REQUIRE(shrinkTextWidth(text, width) == expectedResult);
}

TEST_CASE("shrinkTextWidth", "[shrinkTextWidth]")
{
    testShrinkTextWidth(XSTR(""), 80, XSTR(""));
    testShrinkTextWidth(XSTR("Hello World"), 1, XSTR("Hello\nWorld\n"));
    testShrinkTextWidth(XSTR("Hello    World"), 1, XSTR("Hello\nWorld\n"));
    testShrinkTextWidth(XSTR("Hello World"), 11, XSTR("Hello World\n"));
    testShrinkTextWidth(
        XSTR("List\n  1\n  2\n"),
        12,
        XSTR("List\n  1\n  2\n")
        );
    testShrinkTextWidth(
        XSTR("  ABC ABC DEF DEF"),
        10,
        XSTR("  ABC ABC\n  DEF DEF\n")
        );
    testShrinkTextWidth(
        XSTR("1234567890 ABC DEF GHI"),
        10,
        XSTR("1234567890\nABC DEF\nGHI\n")
        );
    testShrinkTextWidth(
        XSTR("ABC") HYPHEN XSTR("DEF"),
        4,
        XSTR("ABC") HYPHEN XSTR("\nDEF\n")
        );
    testShrinkTextWidth(
        AUML XSTR(" ") OUML XSTR(" ") UUML,
        3,
        AUML XSTR(" ") OUML XSTR("\n") UUML XSTR("\n")
        );
}

////////////////////////////////////////////////////////////////////////////////
// isDirectory

TEST_CASE("PathProcessor/isDirectory", "[isDirectory]")
{
    REQUIRE(isDirectory(XSTR(".")));
    REQUIRE(!isDirectory(XSTR(">")));
}

////////////////////////////////////////////////////////////////////////////////
// findFilePaths

static void testFindFilePaths(function<void()> testProc);

void testFindFilePaths(function<void()> testProc)
{
    try
    {
        testProc();
    }
    catch (ios_base::failure & e)
    {
        error_code code = e.code();
        REQUIRE(code == errc::no_such_file_or_directory);
        return;
    }
    FAIL("Expected exception not thrown");
}

TEST_CASE("PathProcessor/findFilePaths", "[findFilePaths]")
{

#if defined(_WIN32)

    function<void(const wstring &, const wstring &, bool)> createSymlink =
        [] (const wstring & oldPath, const wstring & newPath, bool isDir)
    {
        CreateSymbolicLinkW(
            newPath.c_str(),
            oldPath.c_str(),
            isDir ? SYMBOLIC_LINK_FLAG_DIRECTORY : 0
            );
    };

#elif defined(__APPLE__) // #if defined(_WIN32)

    function<void(const string &, const string &, bool)> createSymlink =
        [] (const string & oldPath, const string & newPath, bool)
    {
        symlink(oldPath.c_str(), newPath.c_str());
    };

#endif // #if defined(_WIN32)

    xstring subdirPath = xstring(tempDir).append(DIR_SEPARATOR XSTR("a"));
    createDir(subdirPath);
    testFindFilePaths(
        []
        {
            xstring pattern = xstring(tempDir).append(DIR_SEPARATOR XSTR("*"));
            bool wildcardsUsed;
            findFilePaths(pattern.c_str(), wildcardsUsed);
        }
        );
    xstring filePath = xstring(tempDir).append(DIR_SEPARATOR XSTR("b"));
    ofstream(filePath.c_str()).close();
    {
        xstring pattern = xstring(tempDir).append(DIR_SEPARATOR XSTR("*"));
        bool wildcardsUsed;
        auto result = findFilePaths(pattern.c_str(), wildcardsUsed);
        REQUIRE(result.size() == 1);
        REQUIRE(wildcardsUsed);
    }
    xstring newFilePath = xstring(tempDir).append(DIR_SEPARATOR XSTR("B1"));
    createSymlink(filePath, newFilePath, false);
    {
        xstring pattern = xstring(tempDir).append(DIR_SEPARATOR XSTR("*"));
        bool wildcardsUsed;
        auto result = findFilePaths(pattern.c_str(), wildcardsUsed);
        REQUIRE(result.size() == 2);
        REQUIRE(result[0] == newFilePath);
        REQUIRE(result[1] == filePath);
        REQUIRE(wildcardsUsed);
    }
    xstring newSubdirPath = xstring(tempDir).append(DIR_SEPARATOR XSTR("c"));
    createSymlink(subdirPath.c_str(), newSubdirPath.c_str(), true);
    {
        xstring pattern = xstring(tempDir).append(DIR_SEPARATOR XSTR("*"));
        bool wildcardsUsed;
        auto result = findFilePaths(pattern.c_str(), wildcardsUsed);
        REQUIRE(result.size() == 2);
        REQUIRE(wildcardsUsed);
    }
    testFindFilePaths(
        []
        {
            bool wildcardsUsed;
            xstring pattern =
                xstring(tempDir).append(DIR_SEPARATOR XSTR("abc.xyz"));
            findFilePaths(pattern.c_str(), wildcardsUsed);
        }
        );
    testFindFilePaths(
        []
        {
            bool wildcardsUsed;
            xstring pattern =
                xstring(tempDir).append(DIR_SEPARATOR XSTR("*.xyz"));
            findFilePaths(pattern.c_str(), wildcardsUsed);
        }
        );
    {
        xstring pattern = filePath;
        bool wildcardsUsed;
        auto result = findFilePaths(pattern.c_str(), wildcardsUsed);
        REQUIRE(result.size() == 1);
        REQUIRE(!wildcardsUsed);
    }

#ifdef _WIN32

    SetCurrentDirectoryW(tempDir.c_str());
    {
        xstring pattern = tempDir.substr(0, 2).append(DIR_SEPARATOR XSTR("*"));
        bool wildcardsUsed;
        auto result = findFilePaths(pattern.c_str(), wildcardsUsed);
        REQUIRE(result.size() == 2);
        REQUIRE(wildcardsUsed);
    }

#endif

}

////////////////////////////////////////////////////////////////////////////////
// findAllFilePaths

TEST_CASE("findAllFilePaths", "[findAllFilePaths]")
{
    vector<const xstring> loggedErrors;
    auto logError =
        [&loggedErrors]
        (const xstring & error)
        {
            loggedErrors.push_back(error);
        };
    vector<const xstring> paths { tempDir };
    vector<const xstring> filePaths;
    
    int result = findAllFilePaths(logError, paths, filePaths);
    
    REQUIRE(result == -1);
    REQUIRE(filePaths.size() == 0);
    REQUIRE(loggedErrors.size() == 1);
    xstring quotedPath = quotePath(tempDir);
    xstring expectedMessage = getExpectedMessage(MSG_PATH_IS_DIR, quotedPath);
    REQUIRE(loggedErrors[0] == expectedMessage);
}

////////////////////////////////////////////////////////////////////////////////
// countLeastSignificantZeros

TEST_CASE("countLeastSignificantZeros", "[countLeastSignificantZeros]")
{
    REQUIRE(countLeastSignificantZeros(1) == 0);
    REQUIRE(countLeastSignificantZeros(2) == 1);
    REQUIRE(countLeastSignificantZeros(0x80000000) == 31);
}

////////////////////////////////////////////////////////////////////////////////
// fixNewlines

#ifdef _WIN32

TEST_CASE("fixNewlines", "[fixNewlines]")
{
    REQUIRE(fixNewlines(L"") == L"");
    REQUIRE(fixNewlines(L"123") == L"123");
    REQUIRE(fixNewlines(L"123\n") == L"123\n");
    REQUIRE(fixNewlines(L"a\r\nb\r\n") == L"a\nb\n");
    REQUIRE(fixNewlines(L"a\r\n\n\rb") == L"a\n\n\nb");
}

#endif // #ifdef _WIN32

////////////////////////////////////////////////////////////////////////////////
// processFile

#ifdef _WIN32

#define xstreambuf wstreambuf
#define xstringbuf wstringbuf

#else // #ifdef _WIN32

#define xstreambuf streambuf
#define xstringbuf stringbuf

#endif // #ifdef _WIN32

TEST_CASE("processFile", "[processFile]")
{
    xstringbuf newWriter;
    xstreambuf * oldWriter = xcout.rdbuf();
    xcout.rdbuf(&newWriter);
    MP3GearWheel gearWheel;
    MP3AttributeSet attributeSetToView;
    
    ProcessFileResult result =
        processFile(XSTR(""), gearWheel, attributeSetToView, XSTR('\0'));

    xcout.rdbuf(oldWriter);
    
    REQUIRE(result == ProcessFileResult::Unprocessed);
    xstring actual = newWriter.str();
    
    xostringstream ostream;
    ostream <<
        getExpectedMessage(MSG_ERROR) <<
        ": " <<
        getExpectedMessage(MSG_MP3_GENERIC_EXCEPTION, "\"\"") <<
        "\n";
    xstring expected = ostream.str();
    REQUIRE(actual == expected);
}

////////////////////////////////////////////////////////////////////////////////
// MP3FrameException

TEST_CASE(
    "MP3FormatException/MP3FileInvalidException",
    "[MP3FileInvalidException]")
{
    MP3FileInvalidException e(XSTR("FILEPATH"), 1234);
    REQUIRE(e.getFilePath() == XSTR("FILEPATH"));
    REQUIRE(e.getOffset() == 1234);
    auto actual = e.getMessage();
    auto expected =
        getExpectedMessage(MSG_MP3_FILE_INVALID_EXCEPTION, "\"FILEPATH\"");
    REQUIRE(actual == expected);
}

TEST_CASE(
    "MP3FormatException/MP3FirstFrameNotFoundException",
    "[MP3FirstFrameNotFoundException]")
{
    MP3FirstFrameNotFoundException e(XSTR("FILEPATH"), 1234);
    REQUIRE(e.getFilePath() == XSTR("FILEPATH"));
    REQUIRE(e.getOffset() == 1234);
    auto actual = e.getMessage();
    auto expected =
        getExpectedMessage(
        MSG_MP3_FIRST_FRAME_NOT_FOUND_EXCEPTION,
        "\"FILEPATH\""
        );
    REQUIRE(actual == expected);
}

TEST_CASE("MP3FormatException/MP3FormatException", "[MP3FormatException]")
{
    MP3FormatException e(XSTR("FILEPATH"), 1234);
    REQUIRE(e.getFilePath() == XSTR("FILEPATH"));
    REQUIRE(e.getOffset() == 1234);
    auto actual = e.getMessage();
    auto expected =
        getExpectedMessage(
        MSG_MP3_FORMAT_EXCEPTION,
        "\"FILEPATH\"",
        "1234 (0x4D2)"
        );
    REQUIRE(actual == expected);
}

TEST_CASE(
    "MP3FormatException/MP3FrameCRCTestException",
    "[MP3FrameCRCTestException]")
{
    MP3FrameCRCTestException e(XSTR("FILEPATH"), 1234, 10);
    REQUIRE(e.getFilePath() == XSTR("FILEPATH"));
    REQUIRE(e.getOffset() == 1234);
    REQUIRE(e.getFrameNumber() == 10);
    auto actual = e.getMessage();
    auto expected =
        getExpectedMessage(
        MSG_MP3_FRAME_CRC_TEST_EXCEPTION,
        10,
        "\"FILEPATH\"",
        "1234 (0x4D2)"
        );
    REQUIRE(actual == expected);
}

TEST_CASE(
    "MP3FormatException/MP3FrameCRCUnknownException",
    "[MP3FrameCRCUnknownException]")
{
    MP3FrameCRCUnknownException e(XSTR("FILEPATH"), 1234, 10);
    REQUIRE(e.getFilePath() == XSTR("FILEPATH"));
    REQUIRE(e.getOffset() == 1234);
    REQUIRE(e.getFrameNumber() == 10);
    auto actual = e.getMessage();
    auto expected =
        getExpectedMessage(
        MSG_MP3_FRAME_CRC_UNKNOWN_EXCEPTION,
        10,
        "\"FILEPATH\"",
        "1234 (0x4D2)"
        );
    REQUIRE(actual == expected);
}

TEST_CASE("MP3FormatException/MP3FrameException", "[MP3FrameException]")
{
    MP3FrameException e(XSTR("FILEPATH"), 1234, 10);
    REQUIRE(e.getFilePath() == XSTR("FILEPATH"));
    REQUIRE(e.getOffset() == 1234);
    REQUIRE(e.getFrameNumber() == 10);
    auto actual = e.getMessage();
    auto expected =
        getExpectedMessage(
        MSG_MP3_FRAME_EXCEPTION,
        10,
        "\"FILEPATH\"",
        "1234 (0x4D2)"
        );
    REQUIRE(actual == expected);
}

TEST_CASE(
    "MP3FormatException/MP3FrameSizeUnknownException",
    "[MP3FrameSizeUnknownException]")
{
    MP3FrameSizeUnknownException e(XSTR("FILEPATH"), 1234, 10);
    REQUIRE(e.getFilePath() == XSTR("FILEPATH"));
    REQUIRE(e.getOffset() == 1234);
    REQUIRE(e.getFrameNumber() == 10);
    auto actual = e.getMessage();
    auto expected =
        getExpectedMessage(
        MSG_MP3_FRAME_SIZE_UNKNOWN_EXCEPTION,
        10,
        "\"FILEPATH\"",
        "1234 (0x4D2)"
        );
    REQUIRE(actual == expected);
}

TEST_CASE("MP3FormatException/MP3GenericException", "[MP3GenericException]")
{
    MP3GenericException e(XSTR("FILEPATH"));
    REQUIRE(e.getFilePath() == XSTR("FILEPATH"));
    auto actual = e.getMessage();
    auto expected =
        getExpectedMessage(MSG_MP3_GENERIC_EXCEPTION, "\"FILEPATH\"");
    REQUIRE(actual == expected);
}

TEST_CASE(
    "MP3FormatException/MP3KeyFrameNotFoundException",
    "[MP3KeyFrameNotFoundException]")
{
    MP3KeyFrameNotFoundException e(XSTR("FILEPATH"));
    REQUIRE(e.getFilePath() == XSTR("FILEPATH"));
    auto actual = e.getMessage();
    auto expected =
        getExpectedMessage(
        MSG_MP3_KEY_FRAME_NOT_FOUND_EXCEPTION,
        "\"FILEPATH\""
        );
    REQUIRE(actual == expected);
}
