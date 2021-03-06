using MP3Attr.App;
using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace MP3Attr.UI
{
    internal static class ConsoleApplication
    {
        private delegate string GetBadOptionDelegate();

        private delegate int ParseAttrSpecDelegate(
            string arg,
            BinaryAttributeStatus status,
            out string errorFormatName,
            out char badChar
            );

        private delegate int ParseOptDelegate(
            string arg,
            GetBadOptionDelegate getBadOption
            );

        private enum ProcessFileResult
        {
            Unprocessed,
            Modified,
            Unmodified,
        }

        private static readonly char OptionPrefix;
        private static readonly string HelpMyMusicDirName;
        private static readonly string HelpNetworkDrivePrefix;

        static ConsoleApplication()
        {
            if (Path.DirectorySeparatorChar != '/')
            {
                OptionPrefix = '/';
                HelpMyMusicDirName = "My Music";
                HelpNetworkDrivePrefix = @"\\";
            }
            else
            {
                OptionPrefix = ':';
                HelpMyMusicDirName = "~/Music";
                HelpNetworkDrivePrefix = "/Volumes/";
            }
        }

        private static int ConsoleBufferWidth
        {
            get
            {
                try
                {
                    bool dummy = Console.CursorVisible;
                }
                catch
                {
                    return -1;
                }
                return Console.BufferWidth;
            }
        }

        private static int FindAllFilePaths(
            Action<string> logError,
            IList<string> paths,
            List<string> filePaths)
        {
            int result = 0;

            foreach (string path in paths)
            {
                string errorFormatName;

                // If any of the specified paths is a directory, provide a
                // useful error description.
                if (Commons.IsDirectory(path))
                    errorFormatName = "PathIsDir";
                else
                {
                    try
                    {
                        bool wildcardsUsed;
                        string[] newFilePaths =
                            Commons.FindFilePaths(path, out wildcardsUsed);
                        if (result >= 0 && wildcardsUsed) ++result;
                        filePaths.AddRange(newFilePaths);
                        continue;
                    }
                    catch (ArgumentException)
                    {
                        errorFormatName = "BadPath";
                    }
                    catch (PathTooLongException)
                    {
                        errorFormatName = "PathTooLong";
                    }
                    catch (UnauthorizedAccessException)
                    {
                        errorFormatName = "AccessDenied";
                    }
                    catch (FileNotFoundException)
                    {
                        errorFormatName = "PathNotFound";
                    }
                }
                result = -1;
                string error =
                    String.Format(
                    Resources.GetString(errorFormatName),
                    Resources.QuotePath(path)
                    );
                logError(error);
            }
            return result;
        }

        private static string FormatResourceWithOptionPrefix(string name)
        {
            return String.Format(Resources.GetString(name), OptionPrefix);
        }

        private static void Main(string[] args)
        {
            SetUpOutputEncoding();

            MP3AttributeSet attributeSet = MP3AttributeSet.Empty;
            char formatSpec = '\0';
            bool optionF = false;

            string error = null;

            string errorName = null;

            IList<string> paths = new List<string>();

            ParseAttrSpecDelegate
                ParseAttrSpec = (
                string arg,
                BinaryAttributeStatus status,
                out string errorFormatName,
                out char badChar
                ) =>
                {
                    int argLen = arg.Length;
                    for (int index = 1; index < argLen; ++index)
                    {
                        char currentChar = char.ToUpperInvariant(arg[index]);

                        MP3Attribute attribute;

                        switch (currentChar)
                        {
                            case 'P':
                                attribute = MP3Attribute.Private;
                                break;
                            case 'C':
                                attribute = MP3Attribute.Copyright;
                                break;
                            case 'O':
                                attribute = MP3Attribute.Original;
                                break;
                            default:
                                errorFormatName = "BadAttribute";
                                badChar = currentChar;
                                return 0;
                        }
                        if (
                            attributeSet.InitAttributeStatus(
                            attribute,
                            (int)status)
                            !=
                            0)
                        {
                            errorFormatName = "DoubleAttribute";
                            badChar = currentChar;
                            return -1;
                        }
                    }
                    errorFormatName = null;
                    badChar = '\0';
                    return 1;
                };

            ParseOptDelegate ParseOpt =
                (string arg, GetBadOptionDelegate getBadOption) =>
                {
                    int argLen = arg.Length;
                    char secondChar = char.ToUpperInvariant(arg[1]);
                    if (secondChar == 'E')
                    {
                        if (argLen != 3) goto bad_option;

                        EmphasisAttributeStatus emphasisStatus;
                        switch (char.ToUpperInvariant(arg[2]))
                        {
                            case '0':
                                emphasisStatus =
                                    EmphasisAttributeStatus.None;
                                break;
                            case '1':
                                emphasisStatus =
                                    EmphasisAttributeStatus.e50_15_µs;
                                break;
                            case '2':
                                emphasisStatus =
                                    EmphasisAttributeStatus.CCITT_j_17;
                                break;
                            case 'X':
                                emphasisStatus =
                                    EmphasisAttributeStatus.Invalid;
                                break;
                            default:
                                goto bad_option;
                        }
                        if (
                            attributeSet.InitAttributeStatus(
                            MP3Attribute.Emphasis,
                            (int)emphasisStatus)
                            ==
                            0)
                            return 1;
                    }
                    else
                    {
                        if (argLen != 2) goto bad_option;

                        switch (secondChar)
                        {
                            case 'L':
                            case 'S':
                                if (formatSpec != '\0') break;
                                formatSpec = secondChar;
                                return 1;
                            case 'W':
                                if (attributeSet.WholeFile) break;
                                attributeSet.WholeFile = true;
                                return 1;
                            case 'F':
                                if (optionF) break;
                                optionF = true;
                                return 1;
                            case '?':
                                if (args.Length != 1) break;
                                WriteHelp();
                                return 0;
                            default:
                                goto bad_option;
                        }
                    }
                    error = Resources.GetString("SyntaxError");
                    return -1;

                bad_option:
                    error = getBadOption();
                    return -1;
                };

            foreach (string arg in args)
            {
                // I guess none of the args may be a null reference.

                int argLen = arg.Length;

                // As a general rule we can state that no argument other than a
                // path may be one character long.

                if (argLen >= 2)
                {
                    int firstChar = arg[0];

                    switch (firstChar)
                    {
                        case '+':
                            {
                                string errorFormatName;
                                char badChar;
                                int result =
                                    ParseAttrSpec(
                                    arg,
                                    BinaryAttributeStatus.Set,
                                    out errorFormatName,
                                    out badChar
                                    );
                                if (result > 0) continue;
                                error =
                                    String.Format(
                                    Resources.GetString(errorFormatName),
                                    badChar
                                    );
                            }
                            goto error;
                        case '-':
                            {
                                string errorFormatName;
                                char badChar;
                                int result =
                                    ParseAttrSpec(
                                    arg,
                                    BinaryAttributeStatus.NotSet,
                                    out errorFormatName,
                                    out badChar
                                    );
                                if (result > 0) continue;
                                if (result < 0)
                                {
                                    error =
                                        String.Format(
                                        Resources.GetString(errorFormatName),
                                        badChar
                                        );
                                    goto error;
                                }
                            }
                            {
                                int result =
                                    ParseOpt(
                                    arg,
                                    () =>
                                    {
                                        return
                                            Resources.GetString(
                                            "BadOptionOrAttribute"
                                            );
                                    }
                                    );
                                if (result > 0) continue;
                                if (result < 0) goto error;
                            }
                            return;
                        default:
                            if (firstChar == OptionPrefix)
                            {
                                int result =
                                    ParseOpt(
                                    arg,
                                    () =>
                                    {
                                        return
                                            FormatResourceWithOptionPrefix(
                                            "BadOption"
                                            );
                                    }
                                    );
                                if (result > 0) continue;
                                if (result < 0) goto error;
                                return;
                            }
                            // Must parse arg as a file name.
                            break;
                    }
                }

                // Path found.
                paths.Add(arg);
            }

            if (paths.Count == 0)
            {
                errorName = "NoFile";
                goto error_name;
            }

            {
                bool anyReadingOption =
                    formatSpec != '\0' || attributeSet.WholeFile || optionF;

                if (
                    !anyReadingOption &&
                    attributeSet.Emphasis.Status ==
                    EmphasisAttributeStatus.Invalid)
                {
                    error = FormatResourceWithOptionPrefix("OptExInWritingOp");
                    goto error;
                }

                List<string> filePaths = new List<string>();
                int findFilePathsResult =
                    FindAllFilePaths(WriteError, paths, filePaths);
                if (findFilePathsResult < 0) return;

                {
                    // Process files.

                    MP3AttributeSet attributeSetToApply;

                    if (anyReadingOption || attributeSet.IsUnspecified)
                    {
                        attributeSetToApply = attributeSet.Unspecified;
                        // If neither of the options /L or /S and no attribute
                        // specification is provided, then MP3epoc lists the
                        // attributes of the specified files as if /L was
                        // specified in the command line.
                        if (formatSpec == '\0') formatSpec = 'L';
                    }
                    else
                    {
                        attributeSetToApply = attributeSet;
                        attributeSetToApply.WholeFile = true;
                        formatSpec = '\0';
                    }

                    int processedFileCount = 0;
                    int modifiedFileCount = 0;

                    MP3GearWheel gearWheel =
                        new MP3GearWheel(attributeSetToApply);
                    if (!optionF) gearWheel.KeyFrameNumber = 2;

                    foreach (string filePath in filePaths)
                    {
                        ProcessFileResult processFileResult =
                            ProcessFile(
                            filePath,
                            gearWheel,
                            attributeSet,
                            formatSpec
                            );
                        if (processFileResult != ProcessFileResult.Unprocessed)
                        {
                            ++processedFileCount;
                            if (processFileResult == ProcessFileResult.Modified)
                                ++modifiedFileCount;
                        }
                    }

                    if (!attributeSetToApply.IsUnspecified)
                        WriteSummary(
                            findFilePathsResult > 0,
                            processedFileCount,
                            modifiedFileCount
                            );
                    }
                }

            return;

        error_name:
            error = Resources.GetString(errorName);

    error:
            WriteError(error);
        }

        private static ProcessFileResult ProcessFile(
            string filePath,
            MP3GearWheel gearWheel,
            MP3AttributeSet attributeSetToView,
            char formatSpec)
        {
            MP3AttributeSet attributeSetBefore;
            try
            {
                // The main difference between invoking gearWheel.ReadAttributes
                // and invoking gearWheel.ApplyAttributes without any attributes
                // to apply is that gearWheel.ReadAttributes throws
                // MP3KeyFrameNotFoundException if the file processed has no key
                // frame, while gearWheel.ApplyAttributes succeeds normally,
                // possibly returning an unspecified or partially unspecified
                // set (which cannot happen if the key frame is found).
                if (gearWheel.AttributeSetToApply.IsUnspecified)
                    attributeSetBefore = gearWheel.ReadAttributes(filePath);
                else
                    attributeSetBefore = gearWheel.ApplyAttributes(filePath);
            }
            catch (Exception e)
            {
                // Errors in file processing are printed to the standard output
                // stream rather than the standard error output stream in order
                // to keep all information in one listing when the output is
                // redirected.
                Console.WriteLine(
                    "{0}: {1}",
                    Resources.GetString("ERROR"),
                    e.Message
                    );
                return ProcessFileResult.Unprocessed;
            }

            if (
                formatSpec != '\0' &&
                attributeSetBefore.Matches(attributeSetToView))
            {
                String format = "{0:" + formatSpec + "}    {1}";
                Console.WriteLine(
                    format,
                    attributeSetBefore,
                    Path.GetFileName(filePath)
                    );
            }
            return
                attributeSetBefore.Matches(gearWheel.AttributeSetToApply) ?
                ProcessFileResult.Unmodified :
                ProcessFileResult.Modified;
        }

        private static void SetUpOutputEncoding()
        {
            Encoding encoding = null;
            if (Console.IsOutputRedirected)
            {
                StreamWriter writer =
                    new StreamWriter(
                        Console.OpenStandardOutput(),
                        encoding = new UTF8Encoding(false)
                        );
                writer.AutoFlush = true;
                Console.SetOut(writer);
            }
            if (Console.IsErrorRedirected)
            {
                StreamWriter writer =
                    new StreamWriter(
                        Console.OpenStandardError(),
                        encoding != null ? encoding : new UTF8Encoding(false)
                        );
                writer.AutoFlush = true;
                Console.SetError(writer);
            }
        }

        private static void WriteError(string error)
        {
            Console.Error.WriteLine(error);
        }

        private static void WriteHelp()
        {
            string help =
                String.Format(
                Resources.GetString("Help"),
                OptionPrefix,
                HelpMyMusicDirName,
                Path.DirectorySeparatorChar,
                HelpNetworkDrivePrefix
                );
            string str =
                Environment.NewLine + AssemblyInfo.Name + ' ' +
                AssemblyInfo.Version + " - " + AssemblyInfo.Copyright + " - " +
                AssemblyInfo.Company + Environment.NewLine + help;
            {
                int width = ConsoleBufferWidth;
                if (width > 40) str = Commons.ShrinkTextWidth(str, width - 1);
            }
            Console.Write(str);
        }

        private static void WriteSummary(
            bool wildcardsUsed,
            int processedFileCount,
            int modifiedFileCount)
        {
            string summary;

            if (wildcardsUsed)
            {
                if (processedFileCount == 0)
                    summary = Resources.GetString("FilesProcessed0");
                else
                {
                    string processedFileString;
                    string modifiedFileString;

                    if (processedFileCount == 1)
                        processedFileString =
                            Resources.GetString("FilesProcessed1");
                    else
                        processedFileString =
                            String.Format(
                            Resources.GetString("FilesProcessedMany"),
                            processedFileCount
                            );

                    if (modifiedFileCount == 0)
                        modifiedFileString =
                            Resources.GetString("FilesChanged0");
                    else if (modifiedFileCount == 1)
                        modifiedFileString =
                            Resources.GetString("FilesChanged1");
                    else
                        modifiedFileString =
                            String.Format(
                            Resources.GetString("FilesChangedMany"),
                            modifiedFileCount
                            );

                    summary =
                        String.Format(
                        "{0}, {1}",
                        processedFileString,
                        modifiedFileString
                        );
                }
            }
            else
            {
                // An error message should have been printed already; since
                // exactly one file was specified, no summary is required.
                if (processedFileCount == 0) return;

                summary =
                    Resources.GetString(
                    modifiedFileCount != 0 ? "FileChanged" : "FileNotChanged"
                    );
            }

            Console.WriteLine("{0}.", summary);
        }
    }
}
