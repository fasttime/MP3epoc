using System;
using System.IO;
using System.Text.RegularExpressions;

namespace MP3Attr.UI
{
    public static partial class Commons
    {
        /// <exception cref="ArgumentNullException"/>
        /// <exception cref="ArgumentException"/>
        /// <exception cref="PathTooLongException"/>
        /// <exception cref="UnauthorizedAccessException"/>
        /// <exception cref="FileNotFoundException"/>
        public static string[] FindFilePaths(
            string path,
            out bool wildcardsUsed)
        {
            string[] filePaths = null;
            wildcardsUsed = false;

            // Extract directory name from path and validate.

            // Path.GetDirectoryName doesn't handle directory separators
            // consistently. For instance, in @"Dir \\" and @".\\", the double
            // backslash is replaced with a single path separator, but in
            // @". \\", it is not.

            string fixedPath;
            {
                string directorySeparator =
                    String.Empty + Path.DirectorySeparatorChar;
                string escapedDirectorySeparators =
                    Regex.Escape(
                    directorySeparator + Path.AltDirectorySeparatorChar
                    );
                // throws ArgumentNullException
                fixedPath =
                    new Regex(
                        @"(?:^ *|[ " + escapedDirectorySeparators + @"]*)[" +
                        escapedDirectorySeparators + @"]")
                        .Replace(path, directorySeparator);
            }

            // throws ArgumentException, PathTooLongException
            string dirName = Path.GetDirectoryName(fixedPath);

            // dirName will be null, for example, if fixedPath only consists of
            // a drive specification like @"C:" or @"C:\", or a network computer
            // name without a shared folder, like @"\\VBOXSVR\*.*", and it will
            // be an empty string if fixedPath contains no path separator and no
            // drive specification. Both cases are handled in the next function
            // call.

            string searchPattern = GetFileName(fixedPath);

            // throws UnauthorizedAccessException
            filePaths = SafeGetFiles(dirName, searchPattern);

            if (filePaths == null) throw new FileNotFoundException();

            if (searchPattern.IndexOfAny(new char[] { '*', '?' }) >= 0)
                wildcardsUsed = true;
            return filePaths;
        }

        public static bool IsDirectory(string path)
        {
            return Directory.Exists(path);
        }

        private static string GetFileName(string path)
        {
            int prevIndex =
                path.LastIndexOfAny(
                new char[]
                {
                    Path.DirectorySeparatorChar,
                    Path.AltDirectorySeparatorChar
                }
                );

            // True on Windows, false on UNIX...
            if (Path.VolumeSeparatorChar != Path.DirectorySeparatorChar)
            {
                if (prevIndex < 0)
                {
                    int length = path.Length;
                    for (int index = 0; index < length; ++index)
                        if (path[index] != ' ')
                        {
                            int specIndex = index + 1;
                            if (
                                length > specIndex + 1 &&
                                path[specIndex] == Path.VolumeSeparatorChar)
                                prevIndex = specIndex;
                            break;
                        }
                }
            }
            return path.Substring(prevIndex + 1);
        }

        private static string[] SafeGetFiles(
            string dirName,
            string searchPattern)
        {
            if ((object)dirName == null) return null;

            string[] filePaths;
            try
            {
                // If searchPattern is an empty string, then Directory.GetFiles
                // always succeeds and returns an empty array, even if dirName
                // is incorrect in some other way. Although subdirectories may
                // exist that match the search pattern, only files are
                // considered here.
                filePaths =
                    Directory.GetFiles(
                    dirName.Length > 0 ? dirName : ".",
                    searchPattern
                    );
            }
            catch (ArgumentException)
            {
                // An ArgumentException is caught, for example, if dirName
                // contains wildcards or is "http:", or if dirName consists of
                // one single non-A-to-Z character followed by ":" or by ":\".
                return null;
            }
            catch (IOException)
            {
                // DirectoryNotFoundException is caught if dirName does not
                // exist.
                // If dirName is a file, an IOException is caught here.
                // An IOException is also caught when dirName denotes a reserved
                // device name followed by a colon like "nul:" or "con:".
                return null;
            }
            catch (NotSupportedException)
            {
                // This is caught when dirName consists of two or more
                // non-wildcard characters terminated by a colon.
                return null;
            }

            switch (filePaths.Length)
            {
                case 0:
                    // filePaths will be a zero-length array if dirName is ok
                    // but no file exists that matches searchPattern, or if
                    // searchPattern is an empty string even when dirName is
                    // faulty in some other way.
                    return null;
                case 1:
                    // Buggy, buggy .NET. If the file name part of searchPattern
                    // specifies a reserved device name like in "nul.*" or
                    // "aux.txt", then Directory.GetFiles will retrieve the path
                    // of a non-existing file with that name in the specified
                    // directory. A test with File.Exists is not the smartest
                    // solution, but it works.
                    if (!File.Exists(filePaths[0])) return null;
                    break;
                default:
                    Array.Sort(filePaths, StringComparer.Ordinal);
                    break;
            }
            return filePaths;
        }
    }
}
