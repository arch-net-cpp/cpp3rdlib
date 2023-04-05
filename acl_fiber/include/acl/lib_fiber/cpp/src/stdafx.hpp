#pragma once

#if defined(__linux__) || defined(MINGW)
#include <sys/select.h>
#include <strings.h>
#endif

#include "lib_acl.h"
#include "acl_cpp/lib_acl.hpp"
#include "fiber/libfiber.h"

#define FIBER_DEBUG_MIN		71
#define FIBER_DEBUG_KEEPER	(FIBER_DEBUG_MIN + 1)
