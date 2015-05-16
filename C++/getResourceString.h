#pragma once

#include "messages.h"
#include "xsys.h"

#include <string>

std::xstring getResourceString(RESID id, ...);
std::xstring quotePath(const std::xstring & path);
