#include "getResourceString.h"
#include "MP3FormatException.h"

#include <sstream>

using namespace std;

namespace
{
    xstring formatOffset(streamoff offset);
    
    template <typename... Args>
    xstring
        makeMessage(
        RESID errorFormatId,
        const xstring & filePath,
        Args &&... moreData
        );

    xstring formatOffset(streamoff offset)
    {
        xostringstream ostream;
        ostream <<
            offset << hex << uppercase << XSTR(" (0x") << offset << XSTR(")");
        return ostream.str();
    }
    
    template <typename... Args>
    xstring
        makeMessage(
        RESID errorFormatId,
        const xstring & filePath,
        Args &&... moreData)
    {
        xstring quotedFilePath = quotePath(filePath);
        xstring message =
            getResourceString(
            errorFormatId,
            forward<Args>(moreData)...,
            quotedFilePath.c_str()
            );
        return message;
    }
}

namespace MP3epoc
{
    // MP3DataUnknownException

    MP3DataUnknownException::MP3DataUnknownException(
        const xstring & filePath,
        streamoff offset):
        MP3FormatException(MSG_MP3_DATA_UNKNOWN_EXCEPTION, filePath, offset)
    { }

    MP3DataUnknownException::MP3DataUnknownException(
        const xstring & filePath,
        streamoff offset,
        const xstring & message):
        MP3FormatException(filePath, offset, message)
    { }

    // MP3FileInvalidException

    MP3FileInvalidException::MP3FileInvalidException(
        const xstring & filePath,
        streamoff offset):
        MP3FormatException(
        filePath,
        offset,
        makeMessage(MSG_MP3_FILE_INVALID_EXCEPTION, filePath))
    { }
    
    MP3FileInvalidException::MP3FileInvalidException(
        const xstring & filePath,
        streamoff offset,
        const xstring & message):
        MP3FormatException(filePath, offset, message)
    { }

    // MP3FirstFrameNotFoundException

    MP3FirstFrameNotFoundException::MP3FirstFrameNotFoundException(
        const xstring & filePath,
        streamoff offset):
        MP3FormatException(
        filePath,
        offset,
        makeMessage(MSG_MP3_FIRST_FRAME_NOT_FOUND_EXCEPTION, filePath))
    { }
    
    MP3FirstFrameNotFoundException::MP3FirstFrameNotFoundException(
        const xstring & filePath,
        streamoff offset,
        const xstring & message):
        MP3FormatException(filePath, offset, message)
    { }

    // MP3FormatException

    MP3FormatException::MP3FormatException(
        const xstring & filePath,
        streamoff offset):
        MP3FormatException(MSG_MP3_FORMAT_EXCEPTION, filePath, offset)
    { }

    MP3FormatException::MP3FormatException(
        const xstring & filePath,
        streamoff offset,
        const xstring & message):
        MP3GenericException(filePath, message),
        offset(offset)
    { }

    template <typename... Args>
    MP3FormatException::MP3FormatException(
        RESID errorFormatId,
        const xstring & filePath,
        streamoff offset,
        Args &&... moreData):
        MP3GenericException(
        errorFormatId,
        filePath,
        forward<Args>(moreData)...,
        formatOffset(offset).c_str()),
        offset(offset)
    { }

    streamoff MP3FormatException::getOffset() const
    {
        return offset;
    }

    // MP3FrameCRCTestException

    MP3FrameCRCTestException::MP3FrameCRCTestException(
        const xstring & filePath,
        streamoff offset,
        FrameNumber frameNumber):
        MP3FrameException(
        MSG_MP3_FRAME_CRC_TEST_EXCEPTION,
        filePath,
        offset,
        frameNumber)
    { }

    MP3FrameCRCTestException::MP3FrameCRCTestException(
        const xstring & filePath,
        streamoff offset,
        FrameNumber frameNumber,
        const xstring & message):
        MP3FrameException(filePath, offset, frameNumber, message)
    { }

    // MP3FrameCRCUnknownException

    MP3FrameCRCUnknownException::MP3FrameCRCUnknownException(
        const xstring & filePath,
        streamoff offset,
        FrameNumber frameNumber):
        MP3FrameException(
        MSG_MP3_FRAME_CRC_UNKNOWN_EXCEPTION,
        filePath,
        offset,
        frameNumber)
    { }

    MP3FrameCRCUnknownException::MP3FrameCRCUnknownException(
        const xstring & filePath,
        streamoff offset,
        FrameNumber frameNumber,
        const xstring & message):
        MP3FrameException(filePath, offset, frameNumber, message)
    { }

    // MP3FrameException

    MP3FrameException::MP3FrameException(
        const xstring & filePath,
        streamoff offset,
        FrameNumber frameNumber):
        MP3FormatException(
        MSG_MP3_FRAME_EXCEPTION,
        filePath,
        offset,
        frameNumber),
        frameNumber(frameNumber)
    { }

    MP3FrameException::MP3FrameException(
        const xstring & filePath,
        streamoff offset,
        FrameNumber frameNumber,
        const xstring & message):
        MP3FormatException(filePath, offset, message),
        frameNumber(frameNumber)
    { }

    MP3FrameException::MP3FrameException(
        RESID errorFormatId,
        const xstring & filePath,
        streamoff offset,
        FrameNumber frameNumber):
        MP3FormatException(
        errorFormatId,
        filePath,
        offset,
        frameNumber),
        frameNumber(frameNumber)
    { }

    FrameNumber MP3FrameException::getFrameNumber() const
    {
        return frameNumber;
    }

    // MP3FrameSizeUnknownException

    MP3FrameSizeUnknownException::MP3FrameSizeUnknownException(
        const xstring & filePath,
        streamoff offset,
        FrameNumber frameNumber):
        MP3FrameException(
        MSG_MP3_FRAME_SIZE_UNKNOWN_EXCEPTION,
        filePath,
        offset,
        frameNumber)
    { }

    MP3FrameSizeUnknownException::MP3FrameSizeUnknownException(
        const xstring & filePath,
        streamoff offset,
        FrameNumber frameNumber,
        const xstring & message):
        MP3FrameException(filePath, offset, frameNumber, message)
    { }

    // MP3GenericException

    MP3GenericException::MP3GenericException(const xstring & filePath):
        MP3GenericException(MSG_MP3_GENERIC_EXCEPTION, filePath)
    { }

    MP3GenericException::MP3GenericException(
        const xstring & filePath,
        const xstring & message):
        runtime_error("Can't process MP3 file"),
        filePath(filePath),
        message(message)
    { }

    template <typename... Args>
    MP3GenericException::MP3GenericException(
        RESID errorFormatId,
        const xstring & filePath,
        Args &&... moreData):
        MP3GenericException(
        filePath,
        makeMessage(errorFormatId, filePath, forward<Args>(moreData)...))
    { }

    const xstring & MP3GenericException::getFilePath() const
    {
        return filePath;
    }

    const xstring & MP3GenericException::getMessage() const
    {
        return message;
    }

    // MP3KeyFrameNotFoundException

    MP3KeyFrameNotFoundException::MP3KeyFrameNotFoundException(
        const xstring & filePath):
        MP3GenericException(MSG_MP3_KEY_FRAME_NOT_FOUND_EXCEPTION, filePath)
    { }
    
    MP3KeyFrameNotFoundException::MP3KeyFrameNotFoundException(
        const xstring & filePath,
        const xstring & message):
        MP3GenericException(filePath, message)
    { }
}
