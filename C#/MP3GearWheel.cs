using System;
using System.Collections.Specialized;
using System.IO;
using System.Runtime.CompilerServices;

namespace MP3Attr
{
    internal struct MP3FrameHeader
    {
        #region Public Constructors

        public MP3FrameHeader(byte[] buffer)
        {
            int data =
                buffer[0] << 24 | buffer[1] << 16 | buffer[2] << 8 | buffer[3];
            _vector = new BitVector32(data);
        }

        #endregion

        #region Public Properties

        public int FrameSize
        {
            // Always validate a frame header using IsValid before accessing
            // this property.
            get
            {
                BitVector32 vector = _vector;

                int id = vector[IdSection];
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

                int layer = vector[LayerSection];
                if (layer == 0) return 0;

                int bitrate = vector[BitrateSection];
                if (bitrate == 0) return 0;

                int samplingRateBase;
                switch (vector[SamplingRateSection])
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
                    _bitrates[id & 0x01, layer - 1, bitrate - 1] * 320;

                int realSamplingRateDiv25 =
                    samplingRateBase << samplingRateShift;

                int paddingSize;
                if (!vector[_paddingMask]) // no padding
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
        }

        public int ProtectedSize
        {
            // Always validate a frame header using IsValid before accessing
            // this property.
            get
            {
                BitVector32 vector = _vector;

                if (vector[ProtectionSection] != 0) return -1;

                int layer = vector[LayerSection];
                if (layer == LayerI)
                {
                    int channelMode = vector[ChannelModeSection];
                    switch (channelMode)
                    {
                        case 0x03: // Mono
                            return 22;
                        case 0x01: // Joint Stereo
                            return 24 + 2 * vector[ModeExtensionSection];
                        default:
                            return 38;
                    }
                }
                else if (layer == LayerIII)
                {
                    int id = vector[IdSection];
                    if (id == 0x01) return 0;

                    int channelMode = vector[ChannelModeSection];

                    if (id == MPEG1 && channelMode != 0x03) // Not Mono
                        return 38;
                    else if (id != MPEG1 && channelMode == 0x03) // Mono
                        return 15;
                    else
                        return 23;
                }
                return 0;
            }
        }

        #endregion

        #region Public Methods

        public bool ApplyAttributes(
            MP3AttributeSet attributeSetToApply,
            bool isKeyFrame,
            ref MP3AttributeSet attributeSetToUpdate)
        {
            bool hasChanged = false;

            for (
                MP3Attribute attribute = 0;
                attribute < (MP3Attribute)4;
                ++attribute)
            {
                int currentStatus = this[attribute];

                if (isKeyFrame || attributeSetToUpdate[attribute].WholeFile)
                {
                    attributeSetToUpdate.UpdateAttributeStatus(
                        attribute,
                        currentStatus,
                        isKeyFrame
                        );
                }

                if (isKeyFrame || attributeSetToApply[attribute].WholeFile)
                {
                    int newStatus = attributeSetToApply[attribute].Status;
                    if (newStatus != 0)
                    {
                        if (currentStatus != newStatus)
                        {
                            this[attribute] = newStatus;
                            hasChanged = true;
                        }
                    }
                }
            }

            return hasChanged;
        }

        public static bool IsValid(MP3FrameHeader header)
        {
            BitVector32 vector = header._vector;
            return vector.Data >> 21 == -1 && vector[BitrateSection] != 0x0f;
        }

        public void ToByteArray(byte[] array)
        {
            int data = _vector.Data;
            for (int index = 3; index >= 0; --index)
            {
                array[index] = (byte)data;
                data >>= 8;
            }
        }

        #endregion

        #region Private Fields

        // Layer constants /////////////////////////////////////////////////////

        private const int LayerI    = 0x03;
        private const int LayerII   = 0x02;
        private const int LayerIII  = 0x01;

        // Audio version ID constants //////////////////////////////////////////

        private const int MPEG1     = 0x03;
        private const int MPEG2     = 0x02;
        private const int MPEG2_5   = 0x00;

        // Bit vector sections /////////////////////////////////////////////////

        private static readonly BitVector32.Section[]   AttributeSections;
        private static readonly BitVector32.Section     BitrateSection;
        private static readonly BitVector32.Section     ChannelModeSection;
        private static readonly BitVector32.Section     IdSection;
        private static readonly BitVector32.Section     LayerSection;
        private static readonly BitVector32.Section     ModeExtensionSection;
        private static readonly BitVector32.Section     ProtectionSection;
        private static readonly BitVector32.Section     SamplingRateSection;

        // Miscellaneous immutable fields //////////////////////////////////////

        private const int _paddingMask = 0x00000200;

        private static readonly byte[,,] _bitrates =
            {
                { // MPEG2, MPEG2.5
                    {
                        1,  2,  3,  4,  5,  6,  7,
                        8,  10, 12, 14, 16, 18, 20
                    },
                    {
                        1,  2,  3,  4,  5,  6,  7,
                        8,  10, 12, 14, 16, 18, 20
                    },
                    {
                        4,  6,  7,  8,  10, 12, 14,
                        16, 18, 20, 22, 24, 28, 32
                    }
                },
                { // MPEG1
                    {
                        4,  5,  6,  7,  8,  10, 12,
                        14, 16, 20, 24, 28, 32, 40
                    },
                    {
                        4,  6,  7,  8,  10, 12, 14,
                        16, 20, 24, 28, 32, 40, 48
                    },
                    {
                        4,  8,  12, 16, 20, 24, 28,
                        32, 36, 40, 44, 48, 52, 56
                    }
                }
            };

        // Instance fields /////////////////////////////////////////////////////

        private BitVector32 _vector;

        #endregion

        #region Private Indexers

        [IndexerName("Status")]
        public int this[MP3Attribute attribute]
        {
            get
            {
                BitVector32.Section section = AttributeSections[(int)attribute];

                int status = _vector[section] + 1;
                return status;
            }
            set
            {
                BitVector32.Section section = AttributeSections[(int)attribute];

                int status = value - 1;
                _vector[section] = status;
            }
        }

        #endregion

        #region Static Constructor

        static MP3FrameHeader()
        {
            AttributeSections = new BitVector32.Section[4];

            BitVector32.Section section;
            section = AttributeSections[(int)MP3Attribute.Emphasis] =
                BitVector32.CreateSection(0x0003);
            section = AttributeSections[(int)MP3Attribute.Original] =
                BitVector32.CreateSection(0x0001, section);
            section = AttributeSections[(int)MP3Attribute.Copyright] =
                BitVector32.CreateSection(0x0001, section);
            section = ModeExtensionSection =
                BitVector32.CreateSection(0x0003, section);
            section = ChannelModeSection =
                BitVector32.CreateSection(0x0003, section);
            section = AttributeSections[(int)MP3Attribute.Private] =
                BitVector32.CreateSection(0x0001, section);
            section =
                BitVector32.CreateSection(0x0001, section);
            section = SamplingRateSection =
                BitVector32.CreateSection(0x0002, section);
            section = BitrateSection =
                BitVector32.CreateSection(0x000e, section);
            section = ProtectionSection =
                BitVector32.CreateSection(0x0001, section);
            section = LayerSection =
                BitVector32.CreateSection(0x0003, section);
            IdSection =
                BitVector32.CreateSection(0x0003, section);
        }

        #endregion
    }

    [Serializable]
    public class MP3GearWheel
    {
        #region Public Constructors

        public MP3GearWheel()
        { _keyFrameNumber = 1; }

        public MP3GearWheel(MP3AttributeSet attributeSetToApply)
        {
            if (!MP3AttributeSet.IsValid(attributeSetToApply))
                throw new ArgumentException(null, "attributeSetToApply");
            _attributeSetToApply = attributeSetToApply;
            _keyFrameNumber = 1;
        }

        public MP3GearWheel(bool skipTest)
        {
            _skipTest = skipTest;
            _keyFrameNumber = 1;
        }

        public MP3GearWheel(
            MP3AttributeSet attributeSetToApply,
            bool skipTest)
            : this(attributeSetToApply)
        { _skipTest = skipTest; }

        #endregion

        #region Public Properties

        public virtual MP3AttributeSet AttributeSetToApply
        {
            get
            { return _attributeSetToApply; }
            set
            {
                if (!MP3AttributeSet.IsValid(value))
                    throw new ArgumentException(null, "value");
                _attributeSetToApply = value;
            }
        }

        public virtual long KeyFrameNumber
        {
            get
            { return _keyFrameNumber; }
            set
            {
                if (value <= 0) throw new ArgumentException(null, "value");

                _keyFrameNumber = value;
            }
        }

        public virtual NonFramedDataFlags NonFramedDataFlags
        {
            get
            { return NonFramedDataField; }
        }

        public virtual bool SkipTest
        {
            get
            { return _skipTest; }
            set
            { _skipTest = value; }
        }

        #endregion

        #region Public Methods

        public virtual MP3AttributeSet ApplyAttributes(string filePath)
        { return ApplyAttributes(filePath, _attributeSetToApply, false); }

        public virtual MP3AttributeSet ReadAttributes(string filePath)
        { return ReadAttributes(filePath, _attributeSetToApply.WholeFile); }

        public virtual MP3AttributeSet ReadAttributes(
            string filePath,
            bool wholeFile)
        {
            MP3AttributeSet attributeSetToApply = MP3AttributeSet.Empty;
            attributeSetToApply.WholeFile = wholeFile;
            return ApplyAttributes(filePath, attributeSetToApply, true);
        }

        #endregion

        #region Protected Fields

        protected NonFramedDataFlags NonFramedDataField;

        #endregion

        #region Protected Methods

        protected virtual MP3AttributeSet InternalApplyAttributes(
            string filePath,
            MP3AttributeSet attributeSetToApply,
            bool keyFrameRequired)
        {
            // Although the File.Open method also throws an
            // ArgumentNullException when a null reference is used for the path
            // argument, we'd raher have the ParamName property set to
            // "filePath", in line with the parameter naming used in the public
            // methods of this class.
            if ((object)filePath == null)
                throw new ArgumentNullException("filePath");

            // First of all, let's clear the nonframed data field.
            NonFramedDataField = NonFramedDataFlags.None;

            FileAccess access =
                attributeSetToApply.IsUnspecified ?
                FileAccess.Read :
                FileAccess.ReadWrite;
            using (MP3Stream stream = new MP3Stream(filePath, access))
            {
                // Look for ID3v2 tag //////////////////////////////////////////

                long startOffset, endOffset;

                startOffset = stream.GetID3v2TagSize();
                if (startOffset != 0)
                {
                    // Set flag first.
                    NonFramedDataField |= NonFramedDataFlags.ID3v2Tag;
                    if (startOffset > stream.Length)
                        throw
                            new MP3FirstFrameNotFoundException(
                            filePath,
                            0
                            );
                }

                {
                    // The following code assumes that startOffset is still set
                    // to the length of the ID3v2 tag, or 0.
                    NonFramedDataFlags flags =
                        stream.FindTrailingData(startOffset);
                    if (flags != NonFramedDataFlags.None)
                        NonFramedDataField |= flags;
                }
                endOffset = stream.Position;

                // Detect nonframed data before first frame ////////////////////

                long newStart = stream.Resync(startOffset);
                if (newStart < 0)
                    throw
                        new MP3FileInvalidException(
                        filePath,
                        startOffset
                        );
                if (newStart != startOffset)
                {
                    NonFramedDataField |=
                        NonFramedDataFlags.DataBeforeFirstFrame;
                    startOffset = newStart;
                }

                // Process frames //////////////////////////////////////////////

                long keyFrameNumber = _keyFrameNumber;

                bool testCRC;
                if (!_skipTest && !attributeSetToApply.IsUnspecified)
                {
                    MP3AttributeSet attributeSetBefore =
                        ProcessFrames(
                        stream,
                        startOffset,
                        endOffset,
                        attributeSetToApply.Unspecified,
                        true,
                        keyFrameNumber,
                        keyFrameRequired
                        );

                    // If no changes are required:
                    if (attributeSetBefore.Matches(attributeSetToApply))
                        return attributeSetBefore;

                    testCRC = false;
                }
                else
                    testCRC = true;
                return
                    ProcessFrames(
                    stream,
                    startOffset,
                    endOffset,
                    attributeSetToApply,
                    testCRC,
                    keyFrameNumber,
                    keyFrameRequired
                    );
            }
        }

        #endregion

        #region Private Fields

        private MP3AttributeSet _attributeSetToApply;
        private long _keyFrameNumber;
        private bool _skipTest;

        #endregion

        #region Private Methods

        private MP3AttributeSet ApplyAttributes(
            string filePath,
            MP3AttributeSet attributeSetToApply,
            bool keyFrameRequired)
        {
            try
            {
                return
                    InternalApplyAttributes(
                    filePath,
                    attributeSetToApply,
                    keyFrameRequired
                    );
            }
            catch (MP3GenericException)
            {
                throw;
            }
            catch (Exception)
            {
                throw new MP3GenericException(filePath);
            }
        }

        private static int CalculateCRC(int size, byte[] buffer)
        {
            int crc = 0xffff; // start with inverted value of 0
            for (int index = 2;;)
            {
                int data = buffer[index];
                for (int bitMask = 1 << 7; bitMask != 0; bitMask >>= 1)
                {
                    int hiBit = crc & 0x8000;
                    crc <<= 1;
                    if (hiBit == 0 ^ (data & bitMask) == 0)
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

        private static MP3AttributeSet ProcessFrames(
            MP3Stream stream,
            long startOffset,
            long endOffset,
            MP3AttributeSet attributeSetToApply,
            bool testCRC,
            long keyFrameNumber,
            bool keyFrameRequired)
        {
            byte[] buffer = stream.Buffer;
            string filePath = stream.Name;

            MP3AttributeSet attributeSetToUpdate =
                attributeSetToApply.Unspecified;
            long offset = startOffset;
            long frameNumber = 1;
            for (;; ++frameNumber)
            {
                if (!stream.ReadBuffer(offset, 4)) break;

                MP3FrameHeader header = new MP3FrameHeader(buffer);
                if (!MP3FrameHeader.IsValid(header)) break;

                int size = header.FrameSize;
                if (size == 0)
                    throw
                        new MP3FrameSizeUnknownException(
                        filePath,
                        offset,
                        frameNumber
                        );

                int protectedSize = 0;

                // If a CRC exists and can be calculated, check it.
                if (testCRC)
                {
                    protectedSize = stream.ReadProtectedData(header);
                    if (protectedSize > 0)
                    {
                        int crc = CalculateCRC(protectedSize, stream.Buffer);
                        if (crc != (buffer[4] << 8 | buffer[5]))
                            throw
                                new MP3FrameCRCTestException(
                                filePath,
                                offset,
                                frameNumber
                                );
                    }
                }

                if (
                    header.ApplyAttributes(
                    attributeSetToApply,
                    frameNumber == keyFrameNumber,
                    ref attributeSetToUpdate))
                {
                    // May still have to read the protected data.
                    if (!testCRC)
                        protectedSize = stream.ReadProtectedData(header);

                    if (protectedSize == 0)
                        throw
                            new MP3FrameCRCUnknownException(
                            filePath,
                            offset,
                            frameNumber
                            );

                    header.ToByteArray(buffer);

                    {
                        int count;
                        if (protectedSize < 0) // have no CRC
                            count = 4;
                        else // have CRC
                        {
                            int crc =
                                CalculateCRC(protectedSize, stream.Buffer);
                            buffer[4] = (byte)(crc >> 8);
                            buffer[5] = (byte)crc;
                            count = 6;
                        }
                        stream.WriteBuffer(offset, count);
                    }
                }

                if (frameNumber == keyFrameNumber)
                {
                    if (!attributeSetToApply.WholeFile)
                        return attributeSetToUpdate;
                }

                offset += size;
            }
            if (offset < endOffset)
                throw new MP3DataUnknownException(filePath, offset);

            // If no key frame exists, only whole file attributes are meaningful
            // to be returned.
            if (frameNumber <= keyFrameNumber)
            {
                if (keyFrameRequired)
                    throw new MP3KeyFrameNotFoundException(filePath);
                attributeSetToUpdate = attributeSetToUpdate.GetSubset(true);
            }
            return attributeSetToUpdate;
        }

        #endregion
    }

    internal class MP3Stream: FileStream
    {
        #region Public Fields

        // Increase the buffer size as needed.
        public readonly byte[] Buffer = new byte[48];

        #endregion

        #region Public Constructors

        public MP3Stream(string path, FileAccess access)
            : base(path, FileMode.Open, access)
        { }

        #endregion

        #region Public Methods

        /// <remarks>
        /// <para>
        /// Position is set to the begin of the nonframed trailing data, or to
        /// the end of the file if no trailing data is found.</para>
        /// <para>
        /// This method does not throw any exception if some aspect of the
        /// trailing data is invalid, and it cannot be used to validate the
        /// file.</para>
        /// </remarks>
        public NonFramedDataFlags FindTrailingData(long minStartOffset)
        {
            int size;
            NonFramedDataFlags flags;
            if ((size = GetBravaSoftwareIncTagSize(minStartOffset, false)) != 0)
                flags = NonFramedDataFlags.BravaSoftwareIncTag;
            else if ((size = GetLyrics3TagSize(minStartOffset, false)) != 0)
                flags = NonFramedDataFlags.Lyrics3Tag;
            else if ((size = GetApeTagSize(minStartOffset, false)) != 0)
                flags = NonFramedDataFlags.ApeTag;
            else
            {
                size = 0;
                flags = NonFramedDataFlags.None;
            }

            if (HasID3v1Tag(minStartOffset))
            {
                // Although some MP3s have a Lyrics3 tag preceding the APE tag,
                // Winamp only recognizes the last tag before the ID3v1 tag.
                int size2;
                NonFramedDataFlags flags2;
                if (
                    (size2 = GetBravaSoftwareIncTagSize(minStartOffset, true))
                    != 0)
                    flags2 =
                        NonFramedDataFlags.ID3v1Tag |
                        NonFramedDataFlags.BravaSoftwareIncTag;
                else if (
                    (size2 = GetLyrics3TagSize(minStartOffset, true))
                    != 0)
                    flags2 =
                        NonFramedDataFlags.ID3v1Tag |
                        NonFramedDataFlags.Lyrics3Tag;
                else if (
                    (size2 = GetApeTagSize(minStartOffset, true))
                    != 0)
                    flags2 =
                        NonFramedDataFlags.ID3v1Tag |
                        NonFramedDataFlags.ApeTag;
                else if (HasMGIXTag(minStartOffset))
                {
                    size2   = MGIXTagSize;
                    flags2  =
                        NonFramedDataFlags.ID3v1Tag |
                        NonFramedDataFlags.MGIXTag;
                }
                else
                {
                    size2   = 0;
                    flags2  = NonFramedDataFlags.ID3v1Tag;
                }
                size2 += ID3v1TagSize;

                if (size2 > size)
                {
                    size = size2;
                    flags = flags2;
                }
            }
            Position = Length - size;
            return flags;
        }

        /// <remarks>
        /// Position is left undefined.
        /// </remarks>
        public int GetApeTagSize(long minStartOffset, bool hasID3v1Tag)
        {
            string signature =
                "APETAGEX" +
                "\ue001\ue001\0\0\ue001\ue001\0\0" +
                "\ue001\ue001\ue001\ue001\ue001\ue001\ue001\ue001" +
                "\0\0\0\0\0\0\0\0";

            long iD3v1TagStartOffset = Length;
            if (hasID3v1Tag) iD3v1TagStartOffset -= ID3v1TagSize;
            long footerStartOffset = iD3v1TagStartOffset - 32;
            if (
                footerStartOffset >= minStartOffset &&
                ReadBuffer(footerStartOffset, 32) &&
                BufferContains(signature, 0))
            {
                byte[] buffer = Buffer;

                int version = buffer[8] | buffer[9] << 8;
                // In APEv2 bit 29 of the flags field is 0 for the footer and 1
                // for the header.
                if (
                    version == 1000 ||
                    version == 2000 && (buffer[23] & 0x20) == 0)
                {
                    int size = buffer[12] | buffer[13] << 8;
                    // The spec says: "Size should be normally in the range of 1
                    // KByte, NEVER more than 8 KByte.". Is that the size of the
                    // whole tag or just the size without the header as coded in
                    // the Header/Footer?
                    if (size <= 0x2000)
                    {
                        if (version == 1000 || (buffer[23] & 0x80) == 0)
                        { // have no header
                            long tagStartOffset = footerStartOffset - size + 32;
                            if (tagStartOffset >= minStartOffset) return size;
                        }
                        else
                        { // have header
                            long headerStartOffset = footerStartOffset - size;
                            if (
                                headerStartOffset >= minStartOffset &&
                                ReadBuffer(footerStartOffset, 32) &&
                                BufferContains(signature, 0))
                                return size + 32;
                        }
                    }
                }
            }
            return 0;
        }
        
        /// <remarks>
        /// Position is left undefined.
        /// </remarks>
        public int GetBravaSoftwareIncTagSize(
            long minStartOffset,
            bool hasID3v1Tag)
        {
            long _ID3v1TagStartOffset = Length;
            if (hasID3v1Tag) _ID3v1TagStartOffset -= ID3v1TagSize;
            long footerStartOffset = _ID3v1TagStartOffset - 48;
            if (
                footerStartOffset >= minStartOffset &&
                ReadBuffer(footerStartOffset, 48) &&
                BufferContains(
                "Brava Software Inc.             \ue000.\ue000\ue000" +
                "            ",
                0))
                foreach (int size in BravaSoftwareIncTagSizes)
                {
                    long headerStartOffset = _ID3v1TagStartOffset - size;
                    if (
                        headerStartOffset >= minStartOffset &&
                        ReadBuffer(headerStartOffset + 4, 16) &&
                        BufferContains("\0\0\0\0\0\0\0\018273645", 0))
                        return size;
                }
            return 0;
        }

        /// <remarks>
        /// Position is left undefined.
        /// </remarks>
        public int GetID3v2TagSize()
        {
            if (
                ReadBuffer(0, 10) &&
                BufferContains(
                "ID3\ue002\ue002\ue001\ue003\ue003\ue003\ue003",
                0))
            {
                byte[] buffer = Buffer;

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

        /// <remarks>
        /// Position is left undefined.
        /// </remarks>
        public int GetLyrics3TagSize(long minStartOffset, bool hasID3v1Tag)
        {
            long footerStartOffset = Length - (6 + 9);
            if (hasID3v1Tag) footerStartOffset -= ID3v1TagSize;
            if (
                footerStartOffset >= minStartOffset &&
                ReadBuffer(footerStartOffset, 6 + 9))
            {
                int size, minSize;

                if ( // Lyrics3v2
                    BufferContains(
                    "\ue000\ue000\ue000\ue000\ue000\ue000LYRICS200",
                    0))
                {
                    byte[] buffer = Buffer;

                    size = 0;
                    for (int index = 0; index < 6; ++index)
                        size = size * 10 + buffer[index] - '0';
                    minSize = size;
                }
                else if (BufferContains("LYRICSEND", 6)) // Lyrics3v1
                {
                    size = 5100;
                    minSize = 11;
                }
                else
                    return 0;

                {
                    int maxSize = (int)(footerStartOffset - minStartOffset);
                    if (size > maxSize) size = maxSize;
                }
                for (; size >= minSize; --size)
                    if (
                        ReadBuffer(footerStartOffset - size, 11) &&
                        BufferContains("LYRICSBEGIN", 0))
                        return size + (6 + 9);
            }
            return 0;
        }

        /// <remarks>
        /// Position is left undefined.
        /// </remarks>
        public bool HasID3v1Tag(long minStartOffset)
        {
            long startOffset = Length - ID3v1TagSize;
            return
                startOffset >= minStartOffset &&
                ReadBuffer(startOffset, 3) &&
                BufferContains("TAG", 0);
        }

        /// <remarks>
        /// Position is left undefined.
        /// </remarks>
        public bool HasMGIXTag(long minStartOffset)
        {
            long startOffset = Length - (ID3v1TagSize + MGIXTagSize);
            return
                startOffset >= minStartOffset &&
                ReadBuffer(startOffset, 4) &&
                BufferContains("MGIX", 0);
        }

        public bool ReadBuffer(long offset, int count)
        {
            Position = offset;
            return Read(Buffer, 0, count) == count;
        }

        /// <summary>
        /// Reads the protected data into the field <c>Buffer</c> at byte offset
        /// 4.
        /// </summary>
        /// <param name="header">
        /// The frame header determining the size of the protected data.
        /// </param>
        /// <returns>
        /// The protected data size. If the frame has no protected data, the
        /// return value is -1. If the protected data size cannot be determined
        /// or if the protected data cannot be read, the return value is 0.
        /// </returns>
        public int ReadProtectedData(MP3FrameHeader header)
        {
            // Still have to calculate the protected size and load protected
            // data.
            int protectedSize = header.ProtectedSize;
            if (protectedSize > 0)
            {
                int count = protectedSize - 4;
                if (Read(Buffer, 4, count) != count) return 0;
            }
            return protectedSize;
        }

        /// <summary>
        /// Tries to locate a frame starting from a specified offset.
        /// </summary>
        /// <returns>
        /// The start offset of the next MP3 frame, being not less than the
        /// offset specified; or -1 if no frame is found.
        /// </returns>
        /// <remarks>
        /// This function offers a very low error tolerance, in line with the
        /// correctness requirements of MP3epoc.
        /// Position is left undefined.
        /// </remarks>
        public long Resync(long offset)
        {
            byte[] buffer = Buffer;

            long currentOffset;
            MP3FrameHeader header;
            for (currentOffset = offset;; ++currentOffset)
            {
                // End of file? Fail!
                if (!ReadBuffer(currentOffset, 4)) return -1;

                // Valid frame header found? Go to next step.
                header = new MP3FrameHeader(buffer);
                if (MP3FrameHeader.IsValid(header)) break;

                // Nothing found yet, so loop once more.
            }

            long nextOffset = currentOffset;
            for (int index = 0; index < 3; ++index)
            {
                // Frame size unknown means failure!
                int frameSize = header.FrameSize;
                if (frameSize == 0) return -1;

                // No CRC test is performed here.

                // If the file ends before the next header can be read, succeed.
                // This behavior is by design, 1-frame MP3 files are validated.
                unchecked
                { nextOffset += frameSize; }  // may overflow
                if (nextOffset < 0) break;
                if (!ReadBuffer(nextOffset, 4)) break;

                // If no valid frame header is found at the expexcted position,
                // fail.
                header = new MP3FrameHeader(buffer);
                if (!MP3FrameHeader.IsValid(header)) return -1;
            }
            return currentOffset;
        }

        public void WriteBuffer(long offset, int count)
        {
            Position = offset;
            Write(Buffer, 0, count);
        }

        #endregion

        #region Private Fields

        private const int ID3v1TagSize = 128;
        private const int MGIXTagSize  = 128;

        private static readonly int[] BravaSoftwareIncTagSizes =
            { 8472, 8468, 8272, 8204 };

        #endregion

        #region Private Methods

        private bool BufferContains(string signature, int start)
        {
            int size = signature.Length;
            byte[] buffer = Buffer;

            for (int index = 0; index < size; ++index)
            {
                char sign = signature[index];
                char item = (char)buffer[start + index];

                switch (sign)
                {
                    case '\ue000':
                        if (item < '0' || item > '9') return false;
                        break;
                    case '\ue001':
                        break;
                    case '\ue002':
                        if (item == 0xff) return false;
                        break;
                    case '\ue003':
                        if (item > 0x7f) return false;
                        break;
                    default:
                        if (sign != item) return false;
                        break;
                }
            }
            return true;
        }

        #endregion
    }

    [Flags, Serializable]
    public enum NonFramedDataFlags
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
    }
}
