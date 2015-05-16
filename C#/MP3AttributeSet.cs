using System;
using System.Collections.Specialized;
using System.ComponentModel;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Text;
using System.Xml.Serialization;

namespace MP3Attr
{
    [Serializable, StructLayout(LayoutKind.Explicit)]
    public struct MP3AttributeSet : IFormattable
    {
        #region Public Fields

        public static readonly MP3AttributeSet Empty;

        [FieldOffset((int)MP3Attribute.Private), NonSerialized]
        public BinaryAttributeInfo Private;

        [FieldOffset((int)MP3Attribute.Copyright), NonSerialized]
        public BinaryAttributeInfo Copyright;

        [FieldOffset((int)MP3Attribute.Original), NonSerialized]
        public BinaryAttributeInfo Original;

        [FieldOffset((int)MP3Attribute.Emphasis), NonSerialized]
        public EmphasisAttributeInfo Emphasis;

        #endregion

        #region Public Types

        [Serializable]
        public struct BinaryAttributeInfo : IMP3AttributeInfo
        {
            public bool IsUnspecified
            {
                get
                { return Status == BinaryAttributeStatus.Unspecified; }
            }

            public BinaryAttributeStatus Status
            {
                get
                {
                    return
                        (BinaryAttributeStatus)(_data & _attributeStatusMask);
                }
                set
                {
                    Type enumType = typeof(BinaryAttributeStatus);
                    int status = (int)value;
                    if (!Enum.IsDefined(enumType, value))
                        throw
                            new InvalidEnumArgumentException(
                            "value",
                            status,
                            enumType
                            );
                    _data = (byte)(_data & ~_attributeStatusMask | status);
                }
            }

            public bool WholeFile
            {
                get
                { return (_data & _attributeWholeFileMask) != 0; }
                set
                {
                    if (value ^ (_data & _attributeWholeFileMask) != 0)
                        _data ^= _attributeWholeFileMask;
                }
            }

            int IMP3AttributeInfo.Status
            {
                get
                { return (int)Status; }
            }

            bool IMP3AttributeInfo.WholeFile
            {
                get
                { return WholeFile; }
            }

            Type IMP3AttributeInfo.GetStatusType()
            { return typeof(BinaryAttributeStatus); }

            private byte _data;
        }

        [Serializable]
        public struct EmphasisAttributeInfo : IMP3AttributeInfo
        {
            public bool IsUnspecified
            {
                get
                { return Status == EmphasisAttributeStatus.Unspecified; }
            }

            public EmphasisAttributeStatus Status
            {
                get
                {
                    return
                        (EmphasisAttributeStatus)(_data & _attributeStatusMask);
                }
                set
                {
                    Type enumType = typeof(EmphasisAttributeStatus);
                    int status = (int)value;
                    if (!Enum.IsDefined(enumType, value))
                        throw
                            new InvalidEnumArgumentException(
                            "value",
                            status,
                            enumType
                            );
                    _data = (byte)(_data & ~_attributeStatusMask | status);
                }
            }

            public bool WholeFile
            {
                get
                { return (_data & _attributeWholeFileMask) != 0; }
                set
                {
                    if (value ^ (_data & _attributeWholeFileMask) != 0)
                        _data ^= _attributeWholeFileMask;
                }
            }

            int IMP3AttributeInfo.Status
            {
                get
                { return (int)Status; }
            }

            bool IMP3AttributeInfo.WholeFile
            {
                get
                { return WholeFile; }
            }

            Type IMP3AttributeInfo.GetStatusType()
            { return typeof(EmphasisAttributeStatus); }

            private byte _data;
        }

        #endregion

        #region Public Properties

        public bool IsUnspecified
        {
            get
            { return (_data & _statusMask) == 0; }
        }

        public MP3AttributeSet Unspecified
        {
            get
            {
                MP3AttributeSet that = Empty;
                that._data = _data & _wholeFileMask;
                return that;
            }
        }

        [XmlIgnore]
        public bool WholeFile
        {
            get
            { return (_data & _wholeFileMask) != 0; }
            set
            { _vector[_wholeFileMask] = value; }
        }

        #endregion

        #region Public Indexers

        [IndexerName("AttributeInfo"), XmlIgnore]
        public IMP3AttributeInfo this[MP3Attribute attribute]
        {
            get
            {
                switch (attribute)
                {
                    case MP3Attribute.Private:
                        return Private;
                    case MP3Attribute.Copyright:
                        return Copyright;
                    case MP3Attribute.Original:
                        return Original;
                    case MP3Attribute.Emphasis:
                        return Emphasis;
                }
                throw
                    new InvalidEnumArgumentException(
                    "attribute",
                    (int)attribute,
                    typeof(MP3Attribute)
                    );
            }
        }

        #endregion

        #region Public Methods

        public MP3AttributeSet GetSubset(bool wholeFile)
        {
            BitVector32 vector = _vector;
            MP3AttributeSet that = this;
            for (int shift = 0; shift < 32; shift += 8)
                if (vector[_attributeWholeFileMask << shift] != wholeFile)
                {
                    int mask =
                        (_attributeStatusMask | _attributeWholeFileMask)
                        <<
                        shift;
                    that._vector[mask] = false;
                }
            return that;
        }

        public static bool IsValid(MP3AttributeSet attributeSet)
        {
            foreach (
                MP3Attribute attribute in Enum.GetValues(typeof(MP3Attribute)))
            {
                IMP3AttributeInfo attributeInfo = attributeSet[attribute];
                if (
                    !Enum.IsDefined(
                    attributeInfo.GetStatusType(),
                    attributeInfo.Status))
                    return false;
            }
            return true;
        }

        public bool Matches(MP3AttributeSet attributeSet)
        {
            int thatData = attributeSet._data;

            int xorData = _data ^ thatData;

            int nimpData = xorData & thatData; // = ~_data & thatData

            // The set matches if all of its attributes do.
            for (
                int
                    shiftedAttributeStatusMask = _attributeStatusMask,
                    shiftedAttributeWholeFileMask = _attributeWholeFileMask;
                shiftedAttributeStatusMask != 0;
                shiftedAttributeStatusMask <<= 8,
                shiftedAttributeWholeFileMask <<= 8)
            {
                // An unspecified attribute is always matched.
                if ((thatData & shiftedAttributeStatusMask) == 0) continue;

                // A specified attribute is never matched by an attribute with a
                // different status.
                if ((xorData & shiftedAttributeStatusMask) != 0)
                    return false;

                // A specified whole-file attribute is never matched by a non-
                // whole-file attribute.
                if ((nimpData & shiftedAttributeWholeFileMask) != 0)
                    return false;
            }
            return true;
        }

        public override string ToString()
        { return ToString(null, null); }

        #endregion

        #region Internal Methods

        internal void ClearAttributeWholeFile(MP3Attribute attribute)
        {
            int shift = 8 * (int)attribute;
            _vector[_attributeWholeFileMask << shift] = false;
        }

        internal int InitAttributeStatus(MP3Attribute attribute, int status)
        {
            int shift = 8 * (int)attribute;
            int currentStatus = _data >> shift & _attributeStatusMask;
            if (currentStatus == 0) _vector[status << shift] = true;
            return currentStatus;
        }

        internal void UpdateAttributeStatus(
            MP3Attribute attribute,
            int status,
            bool isKeyFrame)
        {
            int shift = 8 * (int)attribute;
            int currentStatus = _data >> shift & _attributeStatusMask;
            if (currentStatus == 0)
            {
                _vector[status << shift] = true;
            }
            else if (currentStatus != status)
            {
                if (isKeyFrame)
                {
                    int mask =
                        (_attributeStatusMask | _attributeWholeFileMask)
                        <<
                        shift;
                    _vector[mask] = false;
                    _vector[status << shift] = true;
                }
                else
                    _vector[_attributeWholeFileMask << shift] = false;
            }
        }

        #endregion

        #region Private Fields

        private const byte _attributeStatusMask = 0x0f;
        private const byte _attributeWholeFileMask = 0x10;
        private const int _statusMask = 0x0f0f0f0f;
        private const int _wholeFileMask = 0x10101010;

        [FieldOffset(0)]
        private int _data;
        [FieldOffset(0), NonSerialized]
        private BitVector32 _vector;

        #endregion

        #region IFormattable Members

        public string ToString(string format, IFormatProvider formatProvider)
        {
            // Parse format argument to useCompactFormat.

            bool useCompactFormat;
            if ((object)format == null || format.Length == 0)
                useCompactFormat = false;
            else
            {
                switch (char.ToUpperInvariant(format[0]))
                {
                    case 'C': // = compact
                    case 'S': // for command line switch /S
                        useCompactFormat = true;
                        break;
                    case 'E': // = extended
                    case 'G': // required general format
                    case 'L': // for command line switch /L
                        useCompactFormat = false;
                        break;
                    default:
                        throw new FormatException();
                }
            }

            // Parse formatProvider argument to formatInfo.

            IMP3AttributeSetFormatInfo formatInfo;
            if (
                (object)formatProvider == null
                ||
                (formatInfo =
                (IMP3AttributeSetFormatInfo)
                formatProvider.GetFormat(typeof(IMP3AttributeSetFormatInfo)))
                ==
                null)
                formatInfo = new DefaultMP3AttributeSetFormatInfo();

            // Cache the current instance.

            MP3AttributeSet attributeSet = this;

            // See if the current instance is invalid.

            if (!IsValid(attributeSet)) return formatInfo.InvalidSymbol;

            // Creation of the output string begins.

            StringBuilder builder = new StringBuilder();
            foreach (
                MP3Attribute attribute in Enum.GetValues(typeof(MP3Attribute)))
            {
                IMP3AttributeInfo attributeInfo = attributeSet[attribute];
                int attributeStatus = attributeInfo.Status;

                if (!useCompactFormat)
                {
                    char[] parts;
                    char attributeChar = formatInfo.GetAttributeChar(attribute);
                    char wholeFileChar =
                        attributeInfo.WholeFile ?
                        formatInfo.WholeFileChar :
                        ' ';
                    if (attribute != MP3Attribute.Emphasis)
                    {
                        char sign;
                        switch ((BinaryAttributeStatus)attributeStatus)
                        {
                            default: // BinaryAttributeStatus.Unspecified
                                sign = '=';
                                break;
                            case BinaryAttributeStatus.NotSet:
                                sign = '-';
                                break;
                            case BinaryAttributeStatus.Set:
                                sign = '+';
                                break;
                        }
                        parts =
                            new char[]
                            { sign, attributeChar, wholeFileChar, ' ' };
                    }
                    else
                    {
                        char predicate;
                        switch ((EmphasisAttributeStatus)attributeStatus)
                        {
                            default: // EmphasisAttributeStatus.Unspecified
                                predicate = '.';
                                break;
                            case EmphasisAttributeStatus.None:
                                predicate = '0';
                                break;
                            case EmphasisAttributeStatus.e50_15_µs:
                                predicate = '1';
                                break;
                            case EmphasisAttributeStatus.CCITT_j_17:
                                predicate = '2';
                                break;
                            case EmphasisAttributeStatus.Invalid:
                                predicate = 'x';
                                break;
                        }
                        parts =
                            new char[]
                            { attributeChar, predicate, wholeFileChar };
                    }
                    builder.Append(parts);
                }
                else
                    builder.Append(
                        formatInfo.GetCompactFormatChar(attributeInfo)
                        );
            }
            return builder.ToString();
        }

        #endregion
    }
}
