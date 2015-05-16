#pragma once

#include "FrameNumber.h"
#include "messages.h"
#include "xsys.h"

#include <ios>
#include <stdexcept>

namespace MP3epoc
{
    class MP3GenericException;

    class MP3GenericException: public std::runtime_error
    {
        friend class MP3FormatException;
        friend class MP3KeyFrameNotFoundException;
    public:
        explicit MP3GenericException(const std::xstring & filePath);
        MP3GenericException(
            const std::xstring & filePath,
            const std::xstring & message
            );
        virtual const std::xstring & getFilePath() const;
        virtual const std::xstring & getMessage() const;
    private:
        std::xstring filePath;
        std::xstring message;

        template <typename... Args>
        MP3GenericException(
            RESID errorFormatId,
            const std::xstring & filePath,
            Args &&... moreData
            );
    };

    class MP3FormatException: public MP3GenericException
    {
        friend class MP3DataUnknownException;
        friend class MP3FrameException;
    public:
        MP3FormatException(
            const std::xstring & filePath,
            std::streamoff offset
            );
        MP3FormatException(
            const std::xstring & filePath,
            std::streamoff offset,
            const std::xstring & message
            );
        virtual std::streamoff getOffset() const;
    private:
        std::streamoff offset;

        template <typename... Args>
        MP3FormatException(
            RESID errorFormatId,
            const std::xstring & filePath,
            std::streamoff offset,
            Args &&... moreData
            );
    };
    
    class MP3DataUnknownException: public MP3FormatException
    {
    public:
        MP3DataUnknownException(
            const std::xstring & filePath,
            std::streamoff offset
            );
        MP3DataUnknownException(
            const std::xstring & filePath,
            std::streamoff offset,
            const std::xstring & message
            );
    };

    class MP3FileInvalidException: public MP3FormatException
    {
    public:
        MP3FileInvalidException(
            const std::xstring & filePath,
            std::streamoff offset
            );
        MP3FileInvalidException(
            const std::xstring & filePath,
            std::streamoff offset,
            const std::xstring & message
            );
    };

    class MP3FirstFrameNotFoundException: public MP3FormatException
    {
    public:
        MP3FirstFrameNotFoundException(
            const std::xstring & filePath,
            std::streamoff offset
            );
        MP3FirstFrameNotFoundException(
            const std::xstring & filePath,
            std::streamoff offset,
            const std::xstring & message
            );
    };

    class MP3FrameException: public MP3FormatException
    {
        friend class MP3FrameCRCTestException;
        friend class MP3FrameCRCUnknownException;
        friend class MP3FrameSizeUnknownException;
    public:
        MP3FrameException(
            const std::xstring & filePath,
            std::streamoff offset,
            FrameNumber frameNumber
            );
        MP3FrameException(
            const std::xstring & filePath,
            std::streamoff offset,
            FrameNumber frameNumber,
            const std::xstring & message
            );
        virtual FrameNumber getFrameNumber() const;
    private:
        FrameNumber frameNumber;

        MP3FrameException(
            RESID errorFormatId,
            const std::xstring & filePath,
            std::streamoff offset,
            FrameNumber frameNumber
            );
    };

    class MP3FrameCRCTestException: public MP3FrameException
    {
    public:
        MP3FrameCRCTestException(
            const std::xstring & filePath,
            std::streamoff offset,
            FrameNumber frameNumber
            );
        MP3FrameCRCTestException(
            const std::xstring & filePath,
            std::streamoff offset,
            FrameNumber frameNumber,
            const std::xstring & message
            );
    };

    class MP3FrameCRCUnknownException: public MP3FrameException
    {
    public:
        MP3FrameCRCUnknownException(
            const std::xstring & filePath,
            std::streamoff offset,
            FrameNumber frameNumber
            );
        MP3FrameCRCUnknownException(
            const std::xstring & filePath,
            std::streamoff offset,
            FrameNumber frameNumber,
            const std::xstring & message
            );
    };

    class MP3FrameSizeUnknownException: public MP3FrameException
    {
    public:
        MP3FrameSizeUnknownException(
            const std::xstring & filePath,
            std::streamoff offset,
            FrameNumber frameNumber
            );
        MP3FrameSizeUnknownException(
            const std::xstring & filePath,
            std::streamoff offset,
            FrameNumber frameNumber,
            const std::xstring & message
            );
    };

    class MP3KeyFrameNotFoundException: public MP3GenericException
    {
    public:
        explicit MP3KeyFrameNotFoundException(const std::xstring & filePath);
        MP3KeyFrameNotFoundException(
            const std::xstring & filePath,
            const std::xstring & message
            );
    };
}
