#pragma once

#include "getResourceString.h"
#include "PathProcessor.h"

#include <functional>
#include <ios>

namespace
{
    template <typename T>
    int findAllFilePaths(
        std::function<void(const std::xstring &)> logError,
        const T & paths,
        std::vector<const std::xstring> & filePaths)
    {
        int result = 0;

        for (const std::xstring & path: paths)
        {
            RESID errorFormatId;

            // If any of the specified paths is a directory, provide a useful
            // error description.
            if (isDirectory(path.c_str()))
                errorFormatId = MSG_PATH_IS_DIR;
            else
            {
                try
                {
                    bool wildcardsUsed;
                    std::vector<std::xstring> newFilePaths =
                        findFilePaths(path.c_str(), wildcardsUsed);
                    if (result >= 0 && wildcardsUsed) ++result;
                    filePaths.reserve(filePaths.size() + newFilePaths.size());
                    for (std::xstring & filePath: newFilePaths)
                        filePaths.push_back(move(filePath));
                    continue;
                }
                catch (const std::ios_base::failure & e)
                {
                    switch (e.code().value())
                    {
                    case EINVAL:
                        errorFormatId = MSG_BAD_PATH;
                        break;
                    case EACCES:
                        errorFormatId = MSG_ACCESS_DENIED;
                        break;
                    default:
                        errorFormatId = MSG_PATH_NOT_FOUND;
                        break;
                    }
                }
            }
            result = -1;
            std::xstring error =
                getResourceString(errorFormatId, quotePath(path).c_str());
            logError(error);
        }
        return result;
    }
}
