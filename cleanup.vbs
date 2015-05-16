sub DeleteFile(pathName)
    on error resume next
    fso.DeleteFile pathName
end sub

sub DeleteFolder(pathName)
    on error resume next
    fso.DeleteFolder pathName
end sub

sub RecursiveClean(folder)
    fileName = folder.Path & "\.DS_Store"
    DeleteFile fileName
    for each subFolder in folder.SubFolders
        RecursiveClean subFolder
    next
end sub

set fso = CreateObject("Scripting.FileSystemObject")
scriptFullName = WScript.ScriptFullName
set objFile = fso.GetFile(scriptFullName)
folder = fso.GetParentFolderName(objFile)
set shell = CreateObject("WScript.Shell")
shell.CurrentDirectory = folder

fileNames = Array( _
    "C#\*.csproj.user", _
    "C++\*.aps", _
    "C++\*.vcxproj.user", _
    "MP3epoc.sdf", _
    "MP3epoc.*.suo", _
    "Unit Tests C#\*.csproj.user", _
    "Unit Tests C++\*.aps", _
    "Unit Tests C++\*.vcxproj.user")

for each fileName in fileNames
    DeleteFile fileName
next

folderNames = Array( _
    "MP3epoc.xcodeproj\project.xcworkspace", _
    "MP3epoc.xcodeproj\xcuserdata", _
    "TestResults", _
    "Visual Studio Build")

for each folderName in folderNames
    DeleteFolder folderName
next

RecursiveClean fso.GetFolder(".")
