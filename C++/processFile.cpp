#include "getResourceString.h"
#include "MP3FormatException.h"
#include "PathProcessor.h"
#include "processFile.h"

#include <iostream>

using namespace MP3epoc;
using namespace std;

ProcessFileResult
    processFile(
    const xstring & filePath,
    MP3GearWheel & gearWheel,
    MP3AttributeSet attributeSetToView,
    xchar formatSpec)
{
    MP3AttributeSet attributeSetBefore;
    try
    {
        // The main difference between invoking gearWheel.ReadAttributes and
        // invoking gearWheel.ApplyAttributes without any attributes to apply is
        // that gearWheel.ReadAttributes throws MP3KeyFrameNotFoundException if
        // the file processed has no key frame, while gearWheel.ApplyAttributes
        // succeeds normally, possibly returning an unspecified or partially
        // unspecified set (which cannot happen if the key frame is found).
        if (gearWheel.getAttributeSetToApply().isUnspecified())
            attributeSetBefore = gearWheel.readAttributes(filePath);
        else
            attributeSetBefore = gearWheel.applyAttributes(filePath);
    }
    catch (const MP3GenericException & e)
    {
        // Errors in file processing are printed to the standard output stream
        // rather than the standard error output stream in order to keep all
        // information in one listing when the output is redirected.
        xcout <<
            getResourceString(MSG_ERROR) << XSTR(": ") << e.getMessage() <<
            endl;
        return ProcessFileResult::Unprocessed;
    }

    if (
        formatSpec != XSTR('\0') &&
        attributeSetBefore.matches(attributeSetToView))
    {
        bool useCompactFormat = formatSpec == XSTR('S');
        xcout <<
            attributeSetBefore.toString(useCompactFormat) << XSTR("    ") <<
            getFileName(filePath.c_str()) << endl;
    }
    return
        attributeSetBefore.matches(gearWheel.getAttributeSetToApply()) ?
        ProcessFileResult::Unmodified :
        ProcessFileResult::Modified;
}
