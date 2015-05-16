#include "countLeastSignificantZeros.h"
#include "MP3FormatException.h"
#include "MP3GearWheel.h"

using namespace MP3epoc;
using namespace std;

namespace
{
    // Layer constants /////////////////////////////////////////////////////////
    
    const int LayerI    = 0x03;
    const int LayerII   = 0x02;
    const int LayerIII  = 0x01;
    
    // Audio version ID constants //////////////////////////////////////////////
    
    const int MPEG1     = 0x03;
    const int MPEG2     = 0x02;
    const int MPEG2_5   = 0x00;
    
    // Miscellaneous global constants //////////////////////////////////////////
    
    const size_t ID3v1TagSize   = 128;
    const size_t MGIXTagSize    = 128;
    
    const uint32_t PaddingMask = 0x00000200;
    
    const uint32_t AttributeMasks[] = { 0x0100, 0x08, 0x04, 0x03 };
    
    const uint8_t Bitrates[][3][14] =
    {
        { // MPEG2, MPEG2.5
            {   1,  2,  3,  4,  5,  6,  7,  8,  10, 12, 14, 16, 18, 20 },
            {   1,  2,  3,  4,  5,  6,  7,  8,  10, 12, 14, 16, 18, 20 },
            {   4,  6,  7,  8,  10, 12, 14, 16, 18, 20, 22, 24, 28, 32 }
        },
        { // MPEG1
            {   4,  5,  6,  7,  8,  10, 12, 14, 16, 20, 24, 28, 32, 40 },
            {   4,  6,  7,  8,  10, 12, 14, 16, 20, 24, 28, 32, 40, 48 },
            {   4,  8,  12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56 }
        }
    };
    
    const size_t BravaSoftwareIncTagSizes[] = { 8472, 8468, 8272, 8204 };
    
    // Functions ///////////////////////////////////////////////////////////////
    
    int calculateCRC(int size, const uint8_t buffer[]);
    MP3AttributeSet processFrames(
        MP3Stream & stream,
        streamoff startOffset,
        streamoff endOffset,
        MP3AttributeSet attributeSetToApply,
        bool testCRC,
        FrameNumber keyFrameNumber,
        bool keyFrameRequired
        );
    
    int calculateCRC(int size, const uint8_t buffer[])
    {
        int crc = 0xffff; // start with inverted value of 0
        for (int index = 2;;)
        {
            int data = buffer[index];
            for (int bitMask = 1 << 7; bitMask != 0; bitMask >>= 1)
            {
                int hiBit = crc & 0x8000;
                crc <<= 1;
                if (!hiBit != !(data & bitMask))
                crc ^= 0x8005; // generator polynome
            }
            
            if (index != 3)
            {
                ++index;
                if (index == size) break;
            }
            else
            index = 6;
        }
        crc &= 0xffff; // mask the result
        return crc;
    }
    
    MP3AttributeSet processFrames(
        MP3Stream & stream,
        streamoff startOffset,
        streamoff endOffset,
        MP3AttributeSet attributeSetToApply,
        bool testCRC,
        FrameNumber keyFrameNumber,
        bool keyFrameRequired)
    {
        uint8_t * buffer = stream.buffer;
        const xstring & filePath = stream.getPath();
        
        MP3AttributeSet attributeSetToUpdate =
            attributeSetToApply.getUnspecified();
        streamoff offset = startOffset;
        FrameNumber frameNumber = 1;
        for (;; ++frameNumber)
        {
            if (!stream.readBuffer(offset, 4)) break;
            
            MP3FrameHeader header = MP3FrameHeader(buffer);
            if (!MP3FrameHeader::isValid(header)) break;
            
            size_t size = header.getFrameSize();
            if (size == 0)
            throw MP3FrameSizeUnknownException(filePath, offset, frameNumber);
            
            int protectedSize = 0;
            
            // If a CRC exists and can be calculated, check it.
            if (testCRC)
            {
                protectedSize = stream.readProtectedData(header);
                if (protectedSize > 0)
                {
                    int crc = calculateCRC(protectedSize, stream.buffer);
                    if (crc != (buffer[4] << 8 | buffer[5]))
                    MP3FrameCRCTestException(filePath, offset, frameNumber);
                }
            }
            
            if (
                header.applyAttributes(
                attributeSetToApply,
                frameNumber == keyFrameNumber,
                attributeSetToUpdate))
            {
                // May still have to read the protected data.
                if (!testCRC) protectedSize = stream.readProtectedData(header);
                
                if (protectedSize == 0)
                    throw
                    MP3FrameCRCUnknownException(filePath, offset, frameNumber);
                
                header.toByteArray(buffer);
                
                {
                    size_t count;
                    if (protectedSize < 0) // have no CRC
                    count = 4;
                    else // have CRC
                    {
                        int crc = calculateCRC(protectedSize, stream.buffer);
                        buffer[4] = static_cast<uint8_t>(crc >> 8);
                        buffer[5] = static_cast<uint8_t>(crc);
                        count = 6;
                    }
                    stream.writeBuffer(offset, count);
                }
            }
            
            if (
                frameNumber == keyFrameNumber &&
                !attributeSetToApply.isWholeFile())
                return attributeSetToUpdate;
            
            offset += size;
        }
        if (offset < endOffset) throw MP3DataUnknownException(filePath, offset);
        
        // If no key frame exists, only whole file attributes are meaningful to
        // be returned.
        if (frameNumber <= keyFrameNumber)
        {
            if (keyFrameRequired) throw MP3KeyFrameNotFoundException(filePath);
            attributeSetToUpdate = attributeSetToUpdate.getSubset(true);
        }
        return attributeSetToUpdate;
    }
}

namespace MP3epoc
{
    // MP3FrameHeader //////////////////////////////////////////////////////////

    MP3FrameHeader::MP3FrameHeader()
    { }

    MP3FrameHeader::MP3FrameHeader(const uint8_t buffer[4]):
        data(buffer[0] << 24 | buffer[1] << 16 | buffer[2] << 8 | buffer[3])
    { }

    bool MP3FrameHeader::applyAttributes(
        MP3AttributeSet attributeSetToApply,
        bool isKeyFrame,
        MP3AttributeSet & attributeSetToUpdate)
    {
        bool hasChanged = false;

        for (
            MP3Attribute attribute = MP3Attribute::First;
            attribute <= MP3Attribute::Last;
            ++attribute)
        {
            int currentStatus = getStatus(attribute);

            if (isKeyFrame || attributeSetToUpdate[attribute].isWholeFile())
            {
                attributeSetToUpdate.updateAttributeStatus(
                    attribute,
                    currentStatus,
                    isKeyFrame
                    );
            }

            if (isKeyFrame || attributeSetToApply[attribute].isWholeFile())
            {
                int newStatus = attributeSetToApply[attribute].getStatus();
                if (newStatus != 0)
                {
                    if (currentStatus != newStatus)
                    {
                        setStatus(attribute, newStatus);
                        hasChanged = true;
                    }
                }
            }
        }

        return hasChanged;
    }

    // Always validate a frame header using isValid before calling this method.
    size_t MP3FrameHeader::getFrameSize() const
    {
        int id = this->id;
        int samplingRateShift;
        switch (id)
        {
            case MPEG1:
                samplingRateShift = 2;
                break;
            case MPEG2:
                samplingRateShift = 1;
                break;
            case MPEG2_5:
                samplingRateShift = 0;
                break;
            default:
                return 0;
        }

        int layer = this->layer;
        if (layer == 0) return 0;

        int bitrate = this->bitrate;
        if (bitrate == 0) return 0;

        int samplingRateBase;
        switch (this->samplingRate)
        {
            case 0:
                samplingRateBase = 441;
                break;
            case 1:
                samplingRateBase = 480;
                break;
            case 2:
                samplingRateBase = 320;
                break;
            default:
                return 0;
        }

        // Frame size can be calculated

        int samplesPerFrameDiv8;
        if (layer == LayerI)
            samplesPerFrameDiv8 = 48;
        else if (layer == LayerII || id == MPEG1)
            samplesPerFrameDiv8 = 144;
        else // Layer III and MPEG2, MPEG2.5
            samplesPerFrameDiv8 = 72;

        int realBitrateDiv25 =
            Bitrates[id & 0x01][layer - 1][bitrate - 1] * 320;

        int realSamplingRateDiv25 = samplingRateBase << samplingRateShift;

        int paddingSize;
        if (!(data & PaddingMask)) // no padding
            paddingSize = 0;
        else if (layer == LayerI)
            paddingSize = 4;
        else // Layer II, Layer III
            paddingSize = 1;

        return
            samplesPerFrameDiv8 * realBitrateDiv25
            /
            realSamplingRateDiv25
            +
            paddingSize;
    }

    int MP3FrameHeader::getProtectedSize() const
    {
        if (protection) return -1;

        int layer = this->layer;
        if (layer == LayerI)
        {
            switch (channelMode)
            {
                case 0x03: // Mono
                    return 22;
                case 0x01: // Joint Stereo
                    return 24 + 2 * modeExtension;
                default:
                    return 38;
            }
        }
        else if (layer == LayerIII)
        {
            int id = this->id;
            if (id == 0x01) return 0;

            int channelMode = this->channelMode;

            if (id == MPEG1 && channelMode != 0x03) // Not Mono
                return 38;
            else if (id != MPEG1 && channelMode == 0x03) // Mono
                return 15;
            else
                return 23;
        }
        return 0;
    }

    int MP3FrameHeader::getStatus(MP3Attribute attribute) const
    {
        uint32_t mask = AttributeMasks[static_cast<int>(attribute)];
        return ((data & mask) >> countLeastSignificantZeros(mask)) + 1;
    }

    bool MP3FrameHeader::isValid(MP3FrameHeader header)
    {
        return
            static_cast<int32_t>(header.data) >> 21 == -1 &&
            header.bitrate != 0x0f;
    }

    void MP3FrameHeader::setStatus(MP3Attribute attribute, int status)
    {
        uint32_t mask = AttributeMasks[static_cast<int>(attribute)];
        data =
            (data & ~mask) | (status - 1) << countLeastSignificantZeros(mask);
    }

    void MP3FrameHeader::toByteArray(uint8_t array[4]) const
    {
        uint32_t data = this->data;
        for (int index = 3; index >= 0; --index)
        {
            array[index] = static_cast<uint8_t>(data);
            data >>= 8;
        }
    }

    // MP3GearWheel ////////////////////////////////////////////////////////////

    MP3GearWheel::MP3GearWheel(): MP3GearWheel(false)
    { }

    MP3GearWheel::MP3GearWheel(MP3AttributeSet attributeSetToApply):
        MP3GearWheel(attributeSetToApply, false)
    { }

    MP3GearWheel::MP3GearWheel(bool skipTest):
        keyFrameNumber(1), skipTest(skipTest)
    { }
    
    MP3GearWheel::MP3GearWheel(
        MP3AttributeSet attributeSetToApply,
        bool skipTest):
        MP3GearWheel(skipTest)
    {
        setAttributeSetToApply(attributeSetToApply);
    }

    MP3AttributeSet MP3GearWheel::applyAttributes(const xstring & filePath)
    {
        return applyAttributes(filePath, attributeSetToApply, false);
    }

    MP3AttributeSet
        MP3GearWheel::applyAttributes(
        const xstring & filePath,
        MP3AttributeSet attributeSetToApply,
        bool keyFrameRequired)
    {
        try
        {
            return
                internalApplyAttributes(
                filePath,
                attributeSetToApply,
                keyFrameRequired
                );
        }
        catch (const MP3GenericException &)
        {
            throw;
        }
        catch (const exception &)
        {
            throw MP3GenericException(filePath);
        }
    }

    MP3AttributeSet MP3GearWheel::getAttributeSetToApply() const
    {
        return attributeSetToApply;
    }

    FrameNumber MP3GearWheel::getKeyFrameNumber() const
    {
        return keyFrameNumber;
    }

    MP3AttributeSet
        MP3GearWheel::internalApplyAttributes(
        const xstring & filePath,
        MP3AttributeSet attributeSetToApply,
        bool keyFrameRequired)
    {
        // First of all, let's clear the nonframed data field.
        nonFramedDataField = NonFramedDataFlags::None;

        ios_base::openmode access =
            attributeSetToApply.isUnspecified() ?
            ios_base::in | ios_base::binary :
            ios_base::in | ios_base::out | ios_base::binary;
        MP3Stream stream(filePath, access);

        // Look for ID3v2 tag //////////////////////////////////////////////////

        streamoff startOffset, endOffset;

        startOffset = stream.getID3v2TagSize();
        if (startOffset != 0)
        {
            // Set flag first.
            nonFramedDataField |= NonFramedDataFlags::ID3v2Tag;
            if (startOffset > stream.getSize())
                throw MP3FirstFrameNotFoundException(filePath, 0);
        }

        {
            // The following code assumes that startOffset is still set to the
            // length of the ID3v2 tag, or 0.
            NonFramedDataFlags flags = stream.findTrailingData(startOffset);
            if (flags != NonFramedDataFlags::None) nonFramedDataField |= flags;
        }
        stream.clear();
        endOffset = stream.tellg();

        // Detect nonframed data before first frame ////////////////////////////

        streamoff newStart = stream.resync(startOffset);
        if (newStart < 0) throw MP3FileInvalidException(filePath, startOffset);
        if (newStart != startOffset)
        {
            nonFramedDataField |= NonFramedDataFlags::DataBeforeFirstFrame;
            startOffset = newStart;
        }

        // Process frames //////////////////////////////////////////////////////

        FrameNumber keyFrameNumber = this->keyFrameNumber;

        bool testCRC;
        if (!skipTest && !attributeSetToApply.isUnspecified())
        {
            MP3AttributeSet attributeSetBefore =
                processFrames(
                stream,
                startOffset,
                endOffset,
                attributeSetToApply.getUnspecified(),
                true,
                keyFrameNumber,
                keyFrameRequired
                );

            // If no changes are required:
            if (attributeSetBefore.matches(attributeSetToApply))
                return attributeSetBefore;

            testCRC = false;
        }
        else
            testCRC = true;
        return
            processFrames(
            stream,
            startOffset,
            endOffset,
            attributeSetToApply,
            testCRC,
            keyFrameNumber,
            keyFrameRequired
            );
    }

    bool MP3GearWheel::isSkipTest() const
    {
        return skipTest;
    }

    MP3AttributeSet MP3GearWheel::readAttributes(const xstring & filePath)
    {
        return readAttributes(filePath, attributeSetToApply.isWholeFile());
    }

    MP3AttributeSet
        MP3GearWheel::readAttributes(const xstring & filePath, bool wholeFile)
    {
        MP3AttributeSet attributeSetToApply;
        attributeSetToApply.setWholeFile(wholeFile);
        return applyAttributes(filePath, attributeSetToApply, true);
    }

    void
        MP3GearWheel::setAttributeSetToApply(
        MP3AttributeSet attributeSetToApply)
    {
        if (!MP3AttributeSet::isValid(attributeSetToApply))
            throw invalid_argument("MP3AttributeSet invalid");
        this->attributeSetToApply = attributeSetToApply;
    }

    void MP3GearWheel::setKeyFrameNumber(FrameNumber keyFrameNumber)
    {
        if (keyFrameNumber <= 0)
            throw invalid_argument("Frame number must be > 0");
        this->keyFrameNumber = keyFrameNumber;
    }

    void MP3GearWheel::setSkipTest(bool skipTest)
    {
        this->skipTest = skipTest;
    }

    // MP3Stream ///////////////////////////////////////////////////////////////

    MP3Stream::MP3Stream(const xstring & path, openmode access):
        fstream(path, access), path(path), size(calculateSize())
    { }

    bool
        MP3Stream::bufferContains(const wchar_t signature[], size_t start) const
    {
        for (size_t index = 0;; ++index)
        {
            wchar_t sign = signature[index];
            wchar_t item = buffer[start + index];

            switch (sign)
            {
            case L'\0':
                return true;
            case L'\ue000':
                if (item < L'0' || item > L'9') return false;
                break;
            case L'\ue001':
                break;
            case L'\ue002':
                if (item == 0xff) return false;
                break;
            case L'\ue003':
                if (item > 0x7f) return false;
                break;
            default:
                if (sign != item) return false;
                break;
            }
        }
    }

    streamsize MP3Stream::calculateSize()
    {
        exceptions(failbit | badbit);
        streamsize size = static_cast<streamsize>(seekg(0, end).tellg());
        seekg(0);
        return size;
    }

    NonFramedDataFlags MP3Stream::findTrailingData(streamoff minStartOffset)
    {
        size_t size;
        NonFramedDataFlags flags;
        if ((size = getBravaSoftwareIncTagSize(minStartOffset, false)) != 0)
            flags = NonFramedDataFlags::BravaSoftwareIncTag;
        else if ((size = getLyrics3TagSize(minStartOffset, false)) != 0)
            flags = NonFramedDataFlags::Lyrics3Tag;
        else if ((size = getApeTagSize(minStartOffset, false)) != 0)
            flags = NonFramedDataFlags::ApeTag;
        else
        {
            size = 0;
            flags = NonFramedDataFlags::None;
        }

        if (hasID3v1Tag(minStartOffset))
        {
            // Although some MP3s have a Lyrics3 tag preceding the APE tag,
            // Winamp only recognizes the last tag before the ID3v1 tag.
            size_t size2;
            NonFramedDataFlags flags2;
            if ((size2 = getBravaSoftwareIncTagSize(minStartOffset, true)) != 0)
                flags2 =
                NonFramedDataFlags::ID3v1Tag |
                NonFramedDataFlags::BravaSoftwareIncTag;
            else if ((size2 = getLyrics3TagSize(minStartOffset, true)) != 0)
                flags2 =
                NonFramedDataFlags::ID3v1Tag | NonFramedDataFlags::Lyrics3Tag;
            else if ((size2 = getApeTagSize(minStartOffset, true)) != 0)
                flags2 =
                NonFramedDataFlags::ID3v1Tag | NonFramedDataFlags::ApeTag;
            else if (hasMGIXTag(minStartOffset))
            {
                size2   = MGIXTagSize;
                flags2  =
                    NonFramedDataFlags::ID3v1Tag | NonFramedDataFlags::MGIXTag;
            }
            else
            {
                size2   = 0;
                flags2  = NonFramedDataFlags::ID3v1Tag;
            }
            size2 += ID3v1TagSize;

            if (size2 > size)
            {
                size    = size2;
                flags   = flags2;
            }
        }
        clear();
        // assuming that the last call to exceptions only set badbit
        seekg(-static_cast<off_type>(size), end);
        return flags;
    }

    size_t MP3Stream::getApeTagSize(streamoff minStartOffset, bool hasID3v1Tag)
    {
        const wchar_t * signature =
            L"APETAGEX"
            L"\ue001\ue001\0\0\ue001\ue001\0\0"
            L"\ue001\ue001\ue001\ue001\ue001\ue001\ue001\ue001"
            L"\0\0\0\0\0\0\0\0";

        streamoff _ID3v1TagStartOffset = size;
        if (hasID3v1Tag) _ID3v1TagStartOffset -= ID3v1TagSize;
        streamoff footerStartOffset = _ID3v1TagStartOffset - 32;
        if (
            footerStartOffset >= minStartOffset &&
            readBuffer(footerStartOffset, 32) &&
            bufferContains(signature, 0))
        {
            int version = buffer[8] | buffer[9] << 8;
            // In APEv2 bit 29 of the flags field is 0 for the footer and 1 for
            // the header.
            if (
                version == 1000 ||
                (version == 2000 && (buffer[23] & 0x20) == 0))
            {
                int size = buffer[12] | buffer[13] << 8;
                // The spec says: "Size should be normally in the range of 1
                // KByte, NEVER more than 8 KByte.". Is that the size of the
                // whole tag or just the size without the header as coded in the
                // Header/Footer?
                if (size <= 0x2000)
                {
                    if (version == 1000 || (buffer[23] & 0x80) == 0)
                    { // have no header
                        streamoff tagStartOffset =
                            footerStartOffset - size + 32;
                        if (tagStartOffset >= minStartOffset) return size;
                    }
                    else
                    { // have header
                        streamoff headerStartOffset = footerStartOffset - size;
                        if (
                            headerStartOffset >= minStartOffset &&
                            readBuffer(footerStartOffset, 32) &&
                            bufferContains(signature, 0))
                            return size + 32;
                    }
                }
            }
        }
        return 0;
    }

    size_t MP3Stream::getBravaSoftwareIncTagSize(
        streamoff minStartOffset,
        bool hasID3v1Tag)
    {
        streamoff _ID3v1TagStartOffset = size;
        if (hasID3v1Tag) _ID3v1TagStartOffset -= ID3v1TagSize;
        streamoff footerStartOffset = _ID3v1TagStartOffset - 48;
        if (
            footerStartOffset >= minStartOffset &&
            readBuffer(footerStartOffset, 48) &&
            bufferContains(
            L"Brava Software Inc.             \ue000.\ue000\ue000            ",
            0))
            for (size_t size: BravaSoftwareIncTagSizes)
            {
                streamoff headerStartOffset = _ID3v1TagStartOffset - size;
                if (
                    headerStartOffset >= minStartOffset &&
                    readBuffer(headerStartOffset + 4, 16) &&
                    bufferContains(L"\0\0\0\0\0\0\0\0" L"18273645", 0))
                    return size;
            }
        return 0;
    }

    size_t MP3Stream::getID3v2TagSize()
    {
        if (
            readBuffer(0, 10) &&
            bufferContains(L"ID3\ue002\ue002\ue001\ue003\ue003\ue003\ue003", 0))
        {
            return
                10 + (
                buffer[6] << 21 |
                buffer[7] << 14 |
                buffer[8] << 7  |
                buffer[9]
                );
        }
        return 0;
    }

    size_t MP3Stream::getLyrics3TagSize(
        streamoff minStartOffset,
        bool hasID3v1Tag)
    {
        streamoff footerStartOffset = size - (6 + 9);
        if (hasID3v1Tag) footerStartOffset -= ID3v1TagSize;
        if (
            footerStartOffset >= minStartOffset &&
            readBuffer(footerStartOffset, 6 + 9))
        {
            size_t size, minSize;

            if ( // Lyrics3v2
                bufferContains(
                L"\ue000\ue000\ue000\ue000\ue000\ue000LYRICS200",
                0))
            {
                size = 0;
                for (int index = 0; index < 6; ++index)
                    size = size * 10 + buffer[index] - L'0';
                minSize = size;
            }
            else if (bufferContains(L"LYRICSEND", 6)) // Lyrics3v1
            {
                size = 5100;
                minSize = 11;
            }
            else
                return 0;

            {
                size_t maxSize =
                    static_cast<size_t>(footerStartOffset - minStartOffset);
                if (size > maxSize) size = maxSize;
            }
            for (; size >= minSize; --size)
                if (
                    readBuffer(footerStartOffset - size, 11) &&
                    bufferContains(L"LYRICSBEGIN", 0))
                    return size + (6 + 9);
        }
        return 0;
    }

    const xstring & MP3Stream::getPath() const
    {
        return path;
    }

    streamsize MP3Stream::getSize() const
    {
        return size;
    }

    bool MP3Stream::hasID3v1Tag(streamoff minStartOffset)
    {
        streamoff startOffset = size - ID3v1TagSize;
        return
            startOffset >= minStartOffset &&
            readBuffer(startOffset, 3) &&
            bufferContains(L"TAG", 0);
    }

    bool MP3Stream::hasMGIXTag(streamoff minStartOffset)
    {
        streamoff startOffset = size - (ID3v1TagSize + MGIXTagSize);
        return
            startOffset >= minStartOffset &&
            readBuffer(startOffset, 4) &&
            bufferContains(L"MGIX", 0);
    }

    bool MP3Stream::read(uint8_t * dest, size_t count)
    {
        return
            static_cast<bool>
            (fstream::read(reinterpret_cast<char *>(dest), count));
    }

    bool MP3Stream::readBuffer(streamoff offset, size_t count)
    {
        clear();
        exceptions(badbit);
        seekg(offset);
        return read(buffer, count);
    }

    int MP3Stream::readProtectedData(MP3FrameHeader header)
    {
        // Still have to calculate the protected size and load protected data.
        int protectedSize = header.getProtectedSize();
        if (protectedSize > 0 && !read(buffer + 4, protectedSize - 4))
            return 0;
        return protectedSize;
    }

    streamoff MP3Stream::resync(streamoff offset)
    {
        streamoff currentOffset;
        MP3FrameHeader header;
        for (currentOffset = offset;; ++currentOffset)
        {
            // End of file? Fail!
            if (!readBuffer(currentOffset, 4)) return -1;

            // Valid frame header found? Go to next step.
            header = MP3FrameHeader(buffer);
            if (MP3FrameHeader::isValid(header)) break;

            // Nothing found yet, so loop once more.
        }

        streamoff nextOffset = currentOffset;
        for (int index = 0; index < 3; ++index)
        {
            // Frame size unknown means failure!
            size_t frameSize = header.getFrameSize();
            if (frameSize == 0) return -1;

            // No CRC test is performed here.

            // If the file ends before the next header can be read, succeed.
            // This behavior is by design, 1-frame MP3 files are validated.
            nextOffset += frameSize;  // may overflow
            if (nextOffset < 0) break;
            if (!readBuffer(nextOffset, 4)) break;

            // If no valid frame header is found at the expexcted position,
            // fail.
            header = MP3FrameHeader(buffer);
            if (!MP3FrameHeader::isValid(header)) return -1;
        }
        return currentOffset;
    }

    void MP3Stream::write(const uint8_t * src, size_t count)
    {
        fstream::write(reinterpret_cast<const char *>(src), count);
    }

    void MP3Stream::writeBuffer(streamoff offset, size_t count)
    {
        clear();
        exceptions(failbit | badbit);
        seekp(offset);
        write(buffer, count);
    }

    // NonFramedDataFlags //////////////////////////////////////////////////////

    NonFramedDataFlags
        operator ^ (NonFramedDataFlags value1, NonFramedDataFlags value2)
    {
        return
            static_cast<NonFramedDataFlags>(static_cast<int>(value1) ^ value2);
    }

    NonFramedDataFlags
        operator & (NonFramedDataFlags value1, NonFramedDataFlags value2)
    {
        return
            static_cast<NonFramedDataFlags>(static_cast<int>(value1) & value2);
    }

    NonFramedDataFlags
        operator | (NonFramedDataFlags value1, NonFramedDataFlags value2)
    {
        return
            static_cast<NonFramedDataFlags>(static_cast<int>(value1) | value2);
    }

    NonFramedDataFlags &
        operator ^= (NonFramedDataFlags & lvalue, NonFramedDataFlags rvalue)
    {
        lvalue = lvalue ^ rvalue;
        return lvalue;
    }

    NonFramedDataFlags &
        operator &= (NonFramedDataFlags & lvalue, NonFramedDataFlags rvalue)
    {
        lvalue = lvalue & rvalue;
        return lvalue;
    }

    NonFramedDataFlags &
        operator |= (NonFramedDataFlags & lvalue, NonFramedDataFlags rvalue)
    {
        lvalue = lvalue | rvalue;
        return lvalue;
    }
}
