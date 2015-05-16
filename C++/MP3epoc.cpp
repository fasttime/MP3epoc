#include "findAllFilePaths.h"
#include "getStdOutBufferWidth.h"
#include "processFile.h"
#include "setUpOutputEncoding.h"
#include "shrinkTextWidth.h"
#include "toUpperASCII.h"
#include "version.h"

#include <iostream>
#include <sstream>

#ifdef _WIN32

#define OPTION_PREFIX '/'

#else

#define OPTION_PREFIX ':'

#endif

using namespace MP3epoc;
using namespace std;

namespace
{
    int getConsoleBufferWidth();
    void subMain(int argc, xchar * argv[]);
    void writeError(const xstring & error);
    void writeHelp();
    void
        writeSummary(
        bool wildcardsUsed,
        int processedFileCount,
        int modifiedFileCount
        );

    int getConsoleBufferWidth()
    {
        return getStdOutBufferWidth();
    }

    void subMain(int argc, xchar * argv[])
    {
        setUpOutputEncoding();

        MP3AttributeSet attributeSet;
        xchar formatSpec = XSTR('\0');
        bool optionF = false;

        xstring error;

        RESID errorId;

        vector<const xstring> paths;

        auto
            parseAttrSpec =
            [&attributeSet] (
            const xstring & arg,
            BinaryAttributeStatus status,
            RESID & errorFormatId,
            xchar & badChar)
            {
                auto argLen = arg.length();
                for (unsigned int index = 1; index < argLen; ++index)
                {
                    xchar currentChar = toUpperASCII(arg[index]);

                    MP3Attribute attribute;

                    switch (currentChar)
                    {
                        case XSTR('P'):
                            attribute = MP3Attribute::Private;
                            break;
                        case XSTR('C'):
                            attribute = MP3Attribute::Copyright;
                            break;
                        case XSTR('O'):
                            attribute = MP3Attribute::Original;
                            break;
                        default:
                            errorFormatId = MSG_BAD_ATTRIBUTE;
                            badChar = currentChar;
                            return 0;
                    }
                    if (
                        attributeSet.initAttributeStatus(
                        attribute,
                        static_cast<int>(status))
                        != 0)
                    {
                        errorFormatId = MSG_DOUBLE_ATTRIBUTE;
                        badChar = currentChar;
                        return -1;
                    }
                }
                return 1;
            };

        auto
            parseOpt =
            [&attributeSet, &errorId, &formatSpec, &optionF, argc]
            (const xstring & arg, RESID badOptionErrorId)
            {
                auto argLen = arg.length();
                xchar secondChar = toUpperASCII(arg[1]);
                if (secondChar == XSTR('E'))
                {
                    if (argLen != 3) goto bad_option;

                    EmphasisAttributeStatus emphasisStatus;
                    switch (toUpperASCII(arg[2]))
                    {
                    case XSTR('0'):
                        emphasisStatus = EmphasisAttributeStatus::None;
                        break;
                    case XSTR('1'):
                        emphasisStatus = EmphasisAttributeStatus::e50_15_µs;
                        break;
                    case XSTR('2'):
                        emphasisStatus = EmphasisAttributeStatus::CCITT_j_17;
                        break;
                    case XSTR('X'):
                        emphasisStatus = EmphasisAttributeStatus::Invalid;
                        break;
                    default:
                        goto bad_option;
                    }
                    if (
                        attributeSet.initAttributeStatus(
                        MP3Attribute::Emphasis,
                        static_cast<int>(emphasisStatus))
                        ==
                        0)
                        return 1;
                }
                else
                {
                    if (argLen != 2) goto bad_option;

                    switch (secondChar)
                    {
                    case XSTR('L'):
                    case XSTR('S'):
                        if (formatSpec != XSTR('\0')) break;
                        formatSpec = secondChar;
                        return 1;
                    case XSTR('W'):
                        if (attributeSet.isWholeFile()) break;
                        attributeSet.setWholeFile(true);
                        return 1;
                    case XSTR('F'):
                        if (optionF) break;
                        optionF = true;
                        return 1;
                    case XSTR('?'):
                        if (argc != 1) break;
                        writeHelp();
                        return 0;
                    default:
                        goto bad_option;
                    }
                }
                errorId = MSG_SYNTAX_ERROR;
                return -1;

            bad_option:
                errorId = badOptionErrorId;
                return -1;
            };

        for (int i = 0; i < argc; ++i)
        {
            xstring arg(argv[i]);

            // I guess none of the args may be a null reference.

            auto argLen = arg.length();

            // As a general rule we can state that no argument other than a path
            // may be one character long.

            if (argLen >= 2)
            {
                int firstChar = arg[0];

                switch (firstChar)
                {
                case XSTR('+'):
                    {
                        RESID errorFormatId;
                        xchar badChar;
                        int result =
                            parseAttrSpec(
                            arg,
                            BinaryAttributeStatus::Set,
                            errorFormatId,
                            badChar
                            );
                        if (result > 0) continue;
                        error = getResourceString(errorFormatId, badChar);
                    }
                    goto error;
                case XSTR('-'):
                    {
                        RESID errorFormatId;
                        xchar badChar;
                        int result =
                            parseAttrSpec(
                            arg,
                            BinaryAttributeStatus::NotSet,
                            errorFormatId,
                            badChar
                            );
                        if (result > 0) continue;
                        if (result < 0)
                        {
                            error = getResourceString(errorFormatId, badChar);
                            goto error;
                        }
                    }
                    {
                        int result = parseOpt(arg, MSG_BAD_OPTION_OR_ATTRIBUTE);
                        if (result > 0) continue;
                        if (result < 0) goto error_id;
                    }
                    return;
                case XSTR(OPTION_PREFIX):
                    {
                        int result = parseOpt(arg, MSG_BAD_OPTION);
                        if (result > 0) continue;
                        if (result < 0) goto error_id;
                    }
                    return;
                default:
                    // Must parse arg as a file name.
                    break;
                }
            }

            // Path found.
            paths.push_back(move(arg));
        }

        if (paths.empty())
        {
            errorId = MSG_NO_FILE;
            goto error_id;
        }

        {
            bool anyReadingOption =
                formatSpec != XSTR('\0') ||
                attributeSet.isWholeFile() ||
                optionF;

            if (
                !anyReadingOption &&
                attributeSet.emphasis_().getStatus() ==
                EmphasisAttributeStatus::Invalid)
            {
                errorId = MSG_OPT_EX_IN_WRITING_OP;
                goto error_id;
            }

            vector<const xstring> filePaths;
            int findFilePathsResult =
                findAllFilePaths(writeError, paths, filePaths);
            if (findFilePathsResult < 0) return;

            {
                // Process files.

                MP3AttributeSet attributeSetToApply;

                if (anyReadingOption || attributeSet.isUnspecified())
                {
                    attributeSetToApply = attributeSet.getUnspecified();
                    // If neither of the options /L or /S and no attribute
                    // specification is provided, then MP3epoc lists the
                    // attributes of the specified files as if /L was specified
                    // in the command line.
                    if (formatSpec == XSTR('\0')) formatSpec = XSTR('L');
                }
                else
                {
                    attributeSetToApply = attributeSet;
                    attributeSetToApply.setWholeFile(true);
                    formatSpec = XSTR('\0');
                }

                int processedFileCount = 0;
                int modifiedFileCount = 0;

                MP3GearWheel gearWheel(attributeSetToApply);
                if (!optionF) gearWheel.setKeyFrameNumber(2);

                for (const xstring & filePath: filePaths)
                {
                    ProcessFileResult processFileResult =
                        processFile(
                        filePath,
                        gearWheel,
                        attributeSet,
                        formatSpec
                        );
                    switch (processFileResult)
                    {
                    case ProcessFileResult::Modified:
                        ++modifiedFileCount;
                        // fall through
                    case ProcessFileResult::Unmodified:
                        ++processedFileCount;
                        break;
                    case ProcessFileResult::Unprocessed:
                        break;
                    }
                }

                if (!attributeSetToApply.isUnspecified())
                    writeSummary(
                        findFilePathsResult > 0,
                        processedFileCount,
                        modifiedFileCount
                        );
            }
        }

        return;

    error_id:
        error = getResourceString(errorId);

    error:
        writeError(error);
    }
    
    void writeError(const xstring & error)
    {
        xcerr << error << endl;
    }

    void writeHelp()
    {
        xstring str;
        {
            {
                xostringstream ostream;
                xstring help = getResourceString(MSG_HELP);
                ostream <<
                    XSTR('\n') << APP_NAME << XSTR(' ') << APP_VERSION <<
                    XSTR(" - ") << APP_COPYRIGHT << XSTR(" - ") <<
                    APP_COMPANY << XSTR('\n') << help;
                str = ostream.str();
            }
            {
                int conWidth = getConsoleBufferWidth();
                if (conWidth > 40)
                    str = shrinkTextWidth(str.c_str(), conWidth - 1);
            }
        }
        xcout << str << flush;
    }

    void
        writeSummary(
        bool wildcardsUsed,
        int processedFileCount,
        int modifiedFileCount)
    {
        xstring summary;

        if (wildcardsUsed)
        {
            if (processedFileCount == 0)
                summary = getResourceString(MSG_FILES_PROCESSED_0);
            else
            {
                xstring processedFileString;
                xstring modifiedFileString;

                if (processedFileCount == 1)
                    processedFileString =
                    getResourceString(MSG_FILES_PROCESSED_1);
                else
                    processedFileString =
                    getResourceString(
                    MSG_FILES_PROCESSED_MANY,
                    processedFileCount
                    );

                if (modifiedFileCount == 0)
                    modifiedFileString = getResourceString(MSG_FILES_CHANGED_0);
                else if (modifiedFileCount == 1)
                    modifiedFileString = getResourceString(MSG_FILES_CHANGED_1);
                else
                    modifiedFileString =
                        getResourceString(
                        MSG_FILES_CHANGED_MANY,
                        modifiedFileCount
                        );

                xostringstream stream;
                stream <<
                    processedFileString << XSTR(", ") << modifiedFileString;
                summary = stream.str();
            }
        }
        else
        {
            // An error message should have been printed already; since exactly
            // one file was specified, no summary is required.
            if (processedFileCount == 0) return;

            summary =
                getResourceString(
                modifiedFileCount != 0 ? MSG_FILE_CHANGED : MSG_FILE_NOT_CHANGED
                );
        }

        xcout << summary << XSTR('.') << endl;
    }
}

int xmain(int argc, xchar * argv[])
{
    subMain(argc - 1, argv + 1);
    return 0;
}
