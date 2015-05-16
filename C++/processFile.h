#pragma once

#include "MP3GearWheel.h"

enum class ProcessFileResult
{
    Unprocessed,
    Modified,
    Unmodified,
};

ProcessFileResult
    processFile(
    const std::xstring & filePath,
    MP3epoc::MP3GearWheel & gearWheel,
    MP3epoc::MP3AttributeSet attributeSetToView,
    xchar formatSpec
    );
