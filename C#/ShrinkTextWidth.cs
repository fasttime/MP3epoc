using System;
using System.IO;
using System.Text;
using System.Text.RegularExpressions;

namespace MP3Attr.UI
{
    public static partial class Commons
    {
        public static string ShrinkTextWidth(string text, int width)
        {
            StringBuilder stringBuilder = new StringBuilder();
            Regex regex = new Regex("(\\s*)(\\S.*?)\\s*$");
            using (StringReader reader = new StringReader(text))
            {
                string line;
                while ((line = reader.ReadLine()) != null)
                {
                    Match match = regex.Match(line);
                    string prefix = match.Groups[1].ToString();
                    string postfix = match.Groups[2].ToString();
                    int partWidth = width - prefix.Length;
                    int start = 0;
                    do
                    {
                        string part =
                            FindNextPart(postfix, partWidth, ref start);
                        stringBuilder
                            .Append(prefix)
                            .Append(part)
                            .Append(Environment.NewLine);
                    }
                    while (start >= 0);
                }
            }
            return stringBuilder.ToString();
        }

        private static string FindNextPart(
            string postfix,
            int width,
            ref int start)
        {
            int partStart = start;
            int count = 0;
            int partEnd = 0;
            char prevChar = '\0';
            int postfixEnd = postfix.Length;

            start = -1;
            for (int index = partStart;; ++index, ++count)
            {
                if (index >= postfixEnd)
                {
                    if (prevChar != ' ') partEnd = index;
                    start = -1;
                    break;
                }
                char ch = postfix[index];
                if (prevChar == '\u2010' || prevChar != ' ' && ch == ' ')
                    partEnd = index;
                if (count >= width)
                {
                    // Got part end and new part start after it, can break.
                    if (start >= partEnd) break;

                    // Part end got or not, looking for new part start.
                }
                if (prevChar == '\u2010' || prevChar == ' ' && ch != ' ')
                {
                    start = index;
                    if (count >= width) break;
                }
                prevChar = ch;
            }
            return postfix.Substring(partStart, partEnd - partStart);
        }
    }
}
