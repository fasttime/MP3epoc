#include "Finally.h"
#include "PathProcessor.h"

#if defined(_WIN32)

#include "Windows API.h"

#include <algorithm>

using namespace std;

namespace
{
    bool exists(const wchar_t * filePath);
    string getSystemErrorMessage(DWORD error);
    wstring
        joinFilePath(
        const wchar_t * dirNameBegin,
        const wchar_t * dirNameEnd,
        const wchar_t * fileName
        );
    DECLSPEC_NORETURN void throwFailure(DWORD error);

    bool exists(const wchar_t * filePath)
    {
        HANDLE hFile =
            CreateFileW(
            filePath,
            0,
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
            NULL,
            OPEN_EXISTING,
            0,
            NULL
            );
        if (hFile == INVALID_HANDLE_VALUE) return false;
        CloseHandle(hFile);
        return true;
    }

    string getSystemErrorMessage(DWORD error)
    {
        string message;
        {
            char * buffer = NULL;
            FormatMessageA(
                FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                NULL,
                error,
                0,
                reinterpret_cast<LPSTR>(&buffer),
                0,
                NULL
                );
            Finally fin(
                [&buffer]
                {
                    if (buffer != NULL) LocalFree(buffer);
                }
                );
            if (buffer != NULL) message = buffer;
        }
        return message;
    }

    wstring
        joinFilePath(
        const wchar_t * dirNameBegin,
        const wchar_t * dirNameEnd,
        const wchar_t * fileName)
    {
        wstring filePath;
        {
            size_t fileNameLength = wcslen(fileName);
            filePath.reserve(dirNameEnd - dirNameBegin + fileNameLength);
            filePath
                .assign(dirNameBegin, dirNameEnd)
                .append(fileName, fileName + fileNameLength);
        }
        return filePath;
    }

    void throwFailure(DWORD error)
    {
        generic_errno internalErrno;
        switch (error)
        {
        case ERROR_FILE_NOT_FOUND:
        case ERROR_PATH_NOT_FOUND:
            internalErrno = generic_errno::no_such_file_or_directory;
            break;
        case ERROR_ACCESS_DENIED:
            internalErrno = generic_errno::permission_denied;
            break;
        case ERROR_INVALID_NAME:
            // The path contains invalid characters like "<", ">" or a double
            // quotation mark, or it contains a colon followed by a directory
            // separator and not preceeded by a drive letter.
            internalErrno = generic_errno::invalid_argument;
            break;
        default:
            internalErrno = generic_errno::io_error;
            break;
        }
        throw
            ios_base::failure(
            getSystemErrorMessage(error),
            make_error_code(internalErrno)
            );
    }
}

vector<xstring> findFilePaths(const xchar * path, bool & wildcardsUsed)
{
    vector<wstring> filePaths;
    {
        DWORD error;
        const wchar_t * dirNameEnd;
        {
            WIN32_FIND_DATAW findFileData;
            HANDLE hFindFile =
                FindFirstFileExW(
                path,
                FindExInfoBasic,
                &findFileData,
                FindExSearchNameMatch,
                NULL,
                FIND_FIRST_EX_LARGE_FETCH
                );
            Finally fin(
                [&hFindFile]
                {
                    if (hFindFile != INVALID_HANDLE_VALUE) FindClose(hFindFile);
                }
                );
            if (hFindFile == INVALID_HANDLE_VALUE)
            {
                DWORD error = GetLastError();
                throwFailure(error);
            }
            dirNameEnd = getFileName(path);
            do
                if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
                {
                    wstring filePath =
                        joinFilePath(path, dirNameEnd, findFileData.cFileName);
                    if (exists(filePath.c_str()))
                        filePaths.push_back(move(filePath));
                }
            while (FindNextFileW(hFindFile, &findFileData));
            error = GetLastError();
        }
        if (error != ERROR_NO_MORE_FILES) throwFailure(error);
        if (filePaths.empty()) throwFailure(ERROR_FILE_NOT_FOUND);
        filePaths.shrink_to_fit();
        sort(filePaths.begin(), filePaths.end());
        wildcardsUsed = wcspbrk(dirNameEnd, L"*?") != NULL;
    }
    return filePaths;
}

const xchar * getFileName(const xchar * path)
{
    const wchar_t * fileName = path;
    for (const wchar_t * pch = path;;)
    {
        int ch = *pch;
        if (ch == L'\0') break;
        ++pch;
        if (ch == L'/' || ch == L'\\') fileName = pch;
    }
    // Handle cases where the drive spec (i.e. "C:") is directly followed by a
    // file name without a directory separator. UNC paths must have a directory
    // separator after the drive spec, so no special processing is required.
    if (fileName == path && path[0] != L'\0' && path[1] == L':')
        fileName = path + 2;
    return fileName;
}

bool isDirectory(const xchar * path)
{
    DWORD attributes = GetFileAttributesW(path);
    return
        attributes != INVALID_FILE_ATTRIBUTES &&
        attributes & FILE_ATTRIBUTE_DIRECTORY;
}

#elif defined(__APPLE__) // #if defined(_WIN32)

#include <glob.h>
#include <ios>
#include <sys/stat.h>
#include <unistd.h>

using namespace std;

vector<xstring> findFilePaths(const xchar * path, bool & wildcardsUsed)
{
    vector<xstring> filePaths;
    {
        glob_t globT;
        int globResult;
        int globErrno;
        {
            globResult =
                glob(
                path,
                GLOB_ERR | GLOB_NOESCAPE | GLOB_TILDE,
                NULL,
                &globT
                );
            Finally fin(
                [&globT]
                {
                    globfree(&globT);
                }
                );
            globErrno = errno;
            if (!globResult)
            {
                size_t count = globT.gl_pathc;
                for (size_t i = 0; i < count; ++i)
                {
                    const char * filePath = globT.gl_pathv[i];
                    struct stat st;
                    if (stat(filePath, &st) == 0 && S_ISREG(st.st_mode))
                        filePaths.emplace_back(filePath);
                }
            }
        }
        switch (globResult)
        {
        case GLOB_NOSPACE:
            throw bad_alloc();
        case GLOB_ABORTED:
            throw
                ios_base::failure(
                strerror(globErrno),
                make_error_code(errc::io_error)
                );
        }
        if (filePaths.empty())
            throw
            ios_base::failure(
            strerror(globErrno),
            make_error_code(errc::no_such_file_or_directory)
            );
        filePaths.shrink_to_fit();
        wildcardsUsed = globT.gl_flags & GLOB_MAGCHAR;
    }
    return filePaths;
}

const xchar * getFileName(const xchar * path)
{
    const char * fileName = path;
    for (const char * pch = path;;)
    {
        int ch = *pch;
        if (ch == '\0') break;
        ++pch;
        if (ch == '/') fileName = pch;
    }
    return fileName;
}

bool isDirectory(const xchar * path)
{
    struct stat st;
    return !stat(path, &st) && st.st_mode & S_IFDIR;
}

#endif // #if defined(_WIN32)
