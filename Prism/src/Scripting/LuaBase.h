#pragma once

/**
 * This file shall be included where raw lua functions are used
 * Must be kept away from Lua.h to not expose the link dependency
 */

extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}