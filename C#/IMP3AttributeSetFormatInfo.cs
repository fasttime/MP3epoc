using System;

namespace MP3Attr
{
    public interface IMP3AttributeSetFormatInfo
    {
        #region Public Properties

        string InvalidSymbol { get; }
        char WholeFileChar { get; }

        #endregion

        #region Public Methods

        char GetAttributeChar(MP3Attribute attribute);
        char GetCompactFormatChar(IMP3AttributeInfo attributeInfo);

        #endregion
    }

    public class DefaultMP3AttributeSetFormatInfo : IMP3AttributeSetFormatInfo
    {
        #region Public Properties

        public string InvalidSymbol
        {
            get
            {
                // While the IMP3AttributeSetFormatInfo interface does not
                // require culture invariance, having this accessor return a
                // localized string would make round-trip parsing difficult.
                return "Invalid";
            }
        }

        public char WholeFileChar
        {
            get
            { return '*'; }
        }

        #endregion

        #region Public Methods

        public char GetAttributeChar(MP3Attribute attribute)
        {
            switch (attribute)
            {
                case MP3Attribute.Private:
                    return 'P';
                case MP3Attribute.Copyright:
                    return 'C';
                case MP3Attribute.Original:
                    return 'O';
                case MP3Attribute.Emphasis:
                    return 'E';
                default:
                    throw new ArgumentOutOfRangeException("attribute");
            }
        }

        public char GetCompactFormatChar(IMP3AttributeInfo attributeInfo)
        {
            int status = attributeInfo.Status;
            if (status == 0) return ' ';

            Type statusType = attributeInfo.GetStatusType();
            char formatChar;
            if (statusType == typeof(BinaryAttributeStatus))
            {
                switch ((BinaryAttributeStatus)status)
                {
                    case BinaryAttributeStatus.Unspecified:
                        return '.';
                    case BinaryAttributeStatus.NotSet:
                        formatChar = 'n';
                        break;
                    case BinaryAttributeStatus.Set:
                        formatChar = 'y';
                        break;
                    default:
                        goto argument_exception;
                }
            }
            else if (statusType == typeof(EmphasisAttributeStatus))
            {
                switch ((EmphasisAttributeStatus)status)
                {
                    case EmphasisAttributeStatus.Unspecified:
                        return '.';
                    case EmphasisAttributeStatus.None:
                        formatChar = 'n';
                        break;
                    case EmphasisAttributeStatus.e50_15_µs:
                        formatChar = 'm';
                        break;
                    case EmphasisAttributeStatus.CCITT_j_17:
                        // c may be confused with copyright, use j instead.
                        formatChar = 'j';
                        break;
                    case EmphasisAttributeStatus.Invalid:
                        formatChar = 'x';
                        break;
                    default:
                        goto argument_exception;
                }
            }
            else
                goto argument_exception;
            return
                attributeInfo.WholeFile ?
                char.ToUpperInvariant(formatChar) :
                formatChar;

            argument_exception:
            throw new ArgumentException("attributeInfo");
        }

        #endregion
    }
}
