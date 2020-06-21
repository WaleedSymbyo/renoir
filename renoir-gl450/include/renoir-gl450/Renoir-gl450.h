#pragma once

#include "renoir-gl450/Exports.h"
#include "renoir/Renoir.h"

// use this if you actually link to the library
extern "C" RENOIR_GL450_EXPORT Renoir*
renoir_api();

// use this if you use RAD api
extern "C" RENOIR_GL450_EXPORT void*
rad_api(void* api, bool reload);
