#pragma once

#include "xsys.h"

#include <string>
#include <vector>

std::vector<std::xstring>
    findFilePaths(const xchar * path, bool & wildcardsUsed);
const xchar * getFileName(const xchar * path);
bool isDirectory(const xchar * path);
