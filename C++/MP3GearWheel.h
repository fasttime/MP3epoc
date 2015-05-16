#pragma once

#include "FrameNumber.h"
#include "MP3AttributeSet.h"

#include <cstdint>
#include <fstream>

namespace MP3epoc
{
    enum NonFramedDataFlags
    {
        ID3v2Tag                = 0x01,
        DataBeforeFirstFrame    = 0x02,
        ID3v1Tag                = 0x08,
        BravaSoftwareIncTag     = 0x10,
        Lyrics3Tag              = 0x20,
        ApeTag                  = 0x40,
        MGIXTag                 = 0x80,

        None                    = 0,

        LeadingData             =
        ID3v2Tag | DataBeforeFirstFrame,

        TrailingData            =
        ID3v1Tag | BravaSoftwareIncTag | Lyrics3Tag | ApeTag,
    };

    NonFramedDataFlags
        operator ^ (NonFramedDataFlags value1, NonFramedDataFlags value2);

    NonFramedDataFlags
        operator & (NonFramedDataFlags value1, NonFramedDataFlags value2);

    NonFramedDataFlags
        operator | (NonFramedDataFlags value1, NonFramedDataFlags value2);

    NonFramedDataFlags &
        operator ^= (NonFramedDataFlags & lvalue, NonFramedDataFlags rvalue);

    NonFramedDataFlags &
        operator &= (NonFramedDataFlags & lvalue, NonFramedDataFlags rvalue);

    NonFramedDataFlags &
        operator |= (NonFramedDataFlags & lvalue, NonFramedDataFlags rvalue);

    class MP3FrameHeader
    {
    public:
        MP3FrameHeader();
        MP3FrameHeader(const uint8_t buffer[4]);
        bool applyAttributes(
            MP3AttributeSet attributeSetToApply,
            bool isKeyFrame,
            MP3AttributeSet & attributeSetToUpdate
            );
        size_t getFrameSize() const;
        int getProtectedSize() const;
        static bool isValid(MP3FrameHeader header);
        void toByteArray(uint8_t array[4]) const;
    private:

#pragma warning (push)
#pragma warning (disable: 4201)

        union
        {
            uint32_t data;
            struct
            {
                unsigned emphasisField:     2;
                unsigned originalField:     1;
                unsigned copyrightField:    1;
                unsigned modeExtension:     2;
                unsigned channelMode:       2;
                unsigned privateField:      1;
                unsigned:                   1;
                unsigned samplingRate:      2;
                unsigned bitrate:           4;
                unsigned protection:        1;
                unsigned layer:             2;
                unsigned id:                2;
            };
        };

#pragma warning (pop)

        int getStatus(MP3Attribute attribute) const;
        void setStatus(MP3Attribute attribute, int status);
    };

    class MP3Stream: public std::fstream
    {
    public:
        uint8_t buffer[48];
        MP3Stream(const std::xstring & path, openmode access);
        NonFramedDataFlags findTrailingData(streamoff minStartOffset);
        size_t getApeTagSize(streamoff minStartOffset, bool hasID3v1Tag);
        size_t getBravaSoftwareIncTagSize(
            streamoff minStartOffset,
            bool hasID3v1Tag
            );
        size_t getID3v2TagSize();
        size_t getLyrics3TagSize(streamoff minStartOffset, bool hasID3v1Tag);
        const std::xstring & getPath() const;
        std::streamsize getSize() const;
        bool hasID3v1Tag(streamoff minStartOffset);
        bool hasMGIXTag(streamoff minStartOffset);
        bool readBuffer(streamoff offset, size_t count);
        int readProtectedData(MP3FrameHeader header);
        streamoff resync(streamoff offset);
        void writeBuffer(streamoff offset, size_t count);
    private:
        const std::xstring path;
        const std::streamsize size;
        bool bufferContains(const wchar_t signature[], size_t start) const;
        std::streamsize calculateSize();
        bool read(uint8_t * dest, size_t count);
        void write(const uint8_t * src, size_t count);
    };

    class MP3GearWheel
    {
    public:
        MP3GearWheel();
        virtual ~MP3GearWheel() = default;
        explicit MP3GearWheel(MP3AttributeSet attributeSetToApply);
        explicit MP3GearWheel(bool skipTest);
        MP3GearWheel(MP3AttributeSet attributeSetToApply, bool skipTest);
        MP3AttributeSet applyAttributes(const std::xstring & filePath);
        MP3AttributeSet getAttributeSetToApply() const;
        FrameNumber getKeyFrameNumber() const;
        bool isSkipTest() const;
        MP3AttributeSet readAttributes(const std::xstring & filePath);
        MP3AttributeSet
            readAttributes(const std::xstring & filePath, bool wholeFile);
        void setAttributeSetToApply(MP3AttributeSet attributeSet);
        void setKeyFrameNumber(FrameNumber keyFrameNumber);
        void setSkipTest(bool skipTest);
    protected:
        NonFramedDataFlags nonFramedDataField;
        virtual MP3AttributeSet
            internalApplyAttributes(
            const std::xstring & filePath,
            MP3AttributeSet attributeSetToApply,
            bool keyFrameRequired
            );
    private:
        MP3AttributeSet attributeSetToApply;
        FrameNumber keyFrameNumber;
        bool skipTest;
        MP3AttributeSet
            applyAttributes(
            const std::xstring & filePath,
            MP3AttributeSet attributeSetToApply,
            bool keyFrameRequired
            );
    };
}
