//
//  Values are 32 bit values laid out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +---+-+-+-----------------------+-------------------------------+
//  |Sev|C|R|     Facility          |               Code            |
//  +---+-+-+-----------------------+-------------------------------+
//
//  where
//
//      Sev - is the severity code
//
//          00 - Success
//          01 - Informational
//          10 - Warning
//          11 - Error
//
//      C - is the Customer code flag
//
//      R - is a reserved bit
//
//      Facility - is the facility code
//
//      Code - is the facility's status code
//
//
// Define the facility codes
//
#define FACILITY_REQUEST                 0xEFF
#define FACILITY_QUESTION                0xDFF
#define FACILITY_APP                     0xFFF


//
// Define the severity codes
//
#define STATUS_SEVERITY_WARNING          0x2
#define STATUS_SEVERITY_SUCCESS          0x0
#define STATUS_SEVERITY_INFORMATIONAL    0x1
#define STATUS_SEVERITY_ERROR            0x3


//
// MessageId: MSG_ACCESS_DENIED
//
// MessageText:
//
// The path %1 cannot be accessed.
//
#define MSG_ACCESS_DENIED                ((DWORD)0xEFFF0001L)

//
// MessageId: MSG_BAD_ATTRIBUTE
//
// MessageText:
//
// The attribute specification "%1!c!" is not valid.
//
#define MSG_BAD_ATTRIBUTE                ((DWORD)0xEFFF0002L)

//
// MessageId: MSG_BAD_OPTION
//
// MessageText:
//
// One of the parameters begins with "/", but is not a valid option.
//
#define MSG_BAD_OPTION                   ((DWORD)0xEFFF0003L)

//
// MessageId: MSG_BAD_OPTION_OR_ATTRIBUTE
//
// MessageText:
//
// One of the parameters begins with "-", but is not a valid option or attribute specification.
//
#define MSG_BAD_OPTION_OR_ATTRIBUTE      ((DWORD)0xEFFF0004L)

//
// MessageId: MSG_BAD_PATH
//
// MessageText:
//
// The format of the path %1 is incorrect.
//
#define MSG_BAD_PATH                     ((DWORD)0xEFFF0005L)

//
// MessageId: MSG_DOUBLE_ATTRIBUTE
//
// MessageText:
//
// The attribute specification "%1!c!" was repeated.
//
#define MSG_DOUBLE_ATTRIBUTE             ((DWORD)0xEFFF0006L)

//
// MessageId: MSG_ERROR
//
// MessageText:
//
// ERROR
//
#define MSG_ERROR                        ((DWORD)0xEFFF0007L)

//
// MessageId: MSG_FILE_CHANGED
//
// MessageText:
//
// The file has been modified
//
#define MSG_FILE_CHANGED                 ((DWORD)0x2FFF0008L)

//
// MessageId: MSG_FILE_NOT_CHANGED
//
// MessageText:
//
// No changes needed
//
#define MSG_FILE_NOT_CHANGED             ((DWORD)0x2FFF0009L)

//
// MessageId: MSG_FILES_CHANGED_0
//
// MessageText:
//
// no changes needed
//
#define MSG_FILES_CHANGED_0              ((DWORD)0x2FFF000AL)

//
// MessageId: MSG_FILES_CHANGED_1
//
// MessageText:
//
// 1 modified
//
#define MSG_FILES_CHANGED_1              ((DWORD)0x2FFF000BL)

//
// MessageId: MSG_FILES_CHANGED_MANY
//
// MessageText:
//
// %1!i! modified
//
#define MSG_FILES_CHANGED_MANY           ((DWORD)0x2FFF000CL)

//
// MessageId: MSG_FILES_PROCESSED_0
//
// MessageText:
//
// No files processed
//
#define MSG_FILES_PROCESSED_0            ((DWORD)0x2FFF000DL)

//
// MessageId: MSG_FILES_PROCESSED_1
//
// MessageText:
//
// 1 file processed
//
#define MSG_FILES_PROCESSED_1            ((DWORD)0x2FFF000EL)

//
// MessageId: MSG_FILES_PROCESSED_MANY
//
// MessageText:
//
// %1!i! files processed
//
#define MSG_FILES_PROCESSED_MANY         ((DWORD)0x2FFF000FL)

//
// MessageId: MSG_HELP
//
// MessageText:
//
// 
// Shows or modifies the attributes of MP3 files.
// 
// Usage:
//   MP3EPOC [/L|/S] [/W] [/F] [+P|-P] [+C|-C] [+O|-O] [/E0|/E1|/E2|/Ex] files
// 
//   /L        Show attributes in extended format.
//   /S        Show attributes in compact format.
//   /W        Read whole files, not only the key frames.
//   /F        First frame is key frame (older Winamp versions).
//   +         Set an attribute or show files with an attribute set.
//   -         Clear an attribute or show files with an attribute not set.
//   P         Private attribute.
//   C         Copyright attribute.
//   O         Original attribute.
//   /E0       Remove emphasis or show files with no emphasis.
//   /E1       Set or show files with emphasis at 50/15 microsec.
//   /E2       Set or show files with emphasis at CCITT j. 17.
//   /Ex       Show files with invalid emphasis.
//   files     One or more files to process (wildcards are allowed).
// 
// Attribute specs are introduced by the sign + or - and may specify different attributes at the same time like in +PO or -OCP; the /E0, /E1, /E2 and /Ex options are also considered attribute specs, but they cannot be combined.
// 
// If the command line contains any attribute specs without any of the options /L, /S, /W or /F, MP3epoc writes the new attribute settings in every MP3 file specified.
// If the command line contains any attribute specs along with any of the options /L, /S, /W or /F, MP3epoc shows the MP3 files matching the specified attributes without modifying them.
// If the command line doesn't contain any attribute specs, MP3epoc shows the attributes of the MP3 files without modifying them: in this case, attributes are always displayed in extended format, unless the /S option is explicitly specified.
// Some examples:
// 
//   > MP3EPOC *.MP3
//   Shows in extended format the attributes of all MP3 files in the current directory.
// 
//   > MP3EPOC -C +O "Madonna - Material Girl.mp3"
//   Clears the copyright attribute and sets the original attribute in all frames of the file "Madonna - Material Girl.mp3".
// 
//   > MP3EPOC /S /W /E1 "My Music\*Mozart*.MP3"
//   Shows in compact format the attributes of all MP3 files in the folder "My Music" containing the word "Mozart" in the name and having the emphasis set at 50/15 microsec in all frames.
//   
//   > MP3EPOC -PCO /E0 "\\My-Host\Shared MP3s\*"
//   Clears the private, copyright and original attributes and removes the emphasis from all frames of all files in the network folder "Shared MP3s" on "My-Host".
// 
// Attributes representation in extended format follows the general scheme:
// 
//   ÒP* ÒC* ÒO* E?*
// 
// The letters P, C, O and E stand for the private, copyright, original and emphasis attributes respectively. The sign + before a letter indicates that the corresponding attribute is set. The sign - before a letter indicates that the attribute is not set.
// The emphasis setting is indicated by the character after the E: 0 = no emphasis, 1 = 50/15 microsec, 2 = CCITT j. 17, x = invalid emphasis.
// If using the /W option, an asterisk after an attribute means that the settings of the attribute are the same in all frames of the file.
// 
// The compact format shows the same information as the extended format in less space, it is just somewhat more complicated to read. The compact format uses one letter for each attribute in the sequence: private, copyright, original, emphasis. The different letters have the following meaning:
// 
//   j = emphasis at CCITT j. 17
//   m = emphasis at 50/15 microsec
//   n = attribute not set or no emphasis
//   x = invalid emphasis
//   y = attribute set
// 
// A capital letter means that the settings of the attribute are the same in all frames of the file.
// 
// Altough attribute settings are generally identical for all frames of an MP3 file, this is not always true.
// The settings Winamp shows in the file info dialog for MP3 files are those found in the second frame of the file: this is the default key frame, the one from which MP3epoc normally extracts its informations to show the main settings of attributes or to exclude specific files from the search when the /W option is omitted. Some older Winamp versions and other applications consider rather the first frame as the file's key frame. The same behavior can be achieved with MP3epoc by specifying the /F option.
//
#define MSG_HELP                         ((DWORD)0x6FFF0010L)

//
// MessageId: MSG_MP3_DATA_UNKNOWN_EXCEPTION
//
// MessageText:
//
// The file %2 contains unknown data at offset %1.
//
#define MSG_MP3_DATA_UNKNOWN_EXCEPTION   ((DWORD)0xEFFF0011L)

//
// MessageId: MSG_MP3_FILE_INVALID_EXCEPTION
//
// MessageText:
//
// %1 is not an MP3 file.
//
#define MSG_MP3_FILE_INVALID_EXCEPTION   ((DWORD)0xEFFF0012L)

//
// MessageId: MSG_MP3_FIRST_FRAME_NOT_FOUND_EXCEPTION
//
// MessageText:
//
// Either the size of the file %1 or the information in the ID3v2 tag is wrong.
//
#define MSG_MP3_FIRST_FRAME_NOT_FOUND_EXCEPTION ((DWORD)0xEFFF0013L)

//
// MessageId: MSG_MP3_FORMAT_EXCEPTION
//
// MessageText:
//
// An error occurred while processing file %2 at offset %1.
//
#define MSG_MP3_FORMAT_EXCEPTION         ((DWORD)0xEFFF0014L)

//
// MessageId: MSG_MP3_FRAME_CRC_TEST_EXCEPTION
//
// MessageText:
//
// Frame %1!I64i! in file %3 at offset %2 is corrupt and did not pass the CRC test.
//
#define MSG_MP3_FRAME_CRC_TEST_EXCEPTION ((DWORD)0xEFFF0015L)

//
// MessageId: MSG_MP3_FRAME_CRC_UNKNOWN_EXCEPTION
//
// MessageText:
//
// The CRC of frame %1!I64i! in file %3 at offset %2 cannot be recalculated.
//
#define MSG_MP3_FRAME_CRC_UNKNOWN_EXCEPTION ((DWORD)0xEFFF0016L)

//
// MessageId: MSG_MP3_FRAME_EXCEPTION
//
// MessageText:
//
// An error occurred while processing frame %1!I64i! in file %3 at offset %2.
//
#define MSG_MP3_FRAME_EXCEPTION          ((DWORD)0xEFFF0017L)

//
// MessageId: MSG_MP3_FRAME_SIZE_UNKNOWN_EXCEPTION
//
// MessageText:
//
// The size of frame %1!I64i! in file %3 at offset %2 cannot be determined.
//
#define MSG_MP3_FRAME_SIZE_UNKNOWN_EXCEPTION ((DWORD)0xEFFF0018L)

//
// MessageId: MSG_MP3_GENERIC_EXCEPTION
//
// MessageText:
//
// An error occurred while processing file %1.
//
#define MSG_MP3_GENERIC_EXCEPTION        ((DWORD)0xEFFF0019L)

//
// MessageId: MSG_MP3_KEY_FRAME_NOT_FOUND_EXCEPTION
//
// MessageText:
//
// The file %1 has no key frame.
//
#define MSG_MP3_KEY_FRAME_NOT_FOUND_EXCEPTION ((DWORD)0xEFFF001AL)

//
// MessageId: MSG_NO_FILE
//
// MessageText:
//
// No file was specified.
//
#define MSG_NO_FILE                      ((DWORD)0xEFFF001BL)

//
// MessageId: MSG_OPT_EX_IN_WRITING_OP
//
// MessageText:
//
// The /Ex option can only be used together with any of the options /L, /S, /W or /F.
//
#define MSG_OPT_EX_IN_WRITING_OP         ((DWORD)0xEFFF001CL)

//
// MessageId: MSG_PATH_IS_DIR
//
// MessageText:
//
// The path %1 denotes a directory.
//
#define MSG_PATH_IS_DIR                  ((DWORD)0xEFFF001DL)

//
// MessageId: MSG_PATH_NOT_FOUND
//
// MessageText:
//
// The path %1 was not found.
//
#define MSG_PATH_NOT_FOUND               ((DWORD)0xEFFF001EL)

//
// MessageId: MSG_SYNTAX_ERROR
//
// MessageText:
//
// The syntax of the command is incorrect.
//
#define MSG_SYNTAX_ERROR                 ((DWORD)0xEFFF001FL)

