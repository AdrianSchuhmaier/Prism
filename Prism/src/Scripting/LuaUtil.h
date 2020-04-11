#pragma once

#include "LuaBase.h"

namespace Prism {

	bool checkLua(uint32_t code) {
		if (code != LUA_OK)
		{
			PR_LOG_WARN(lua_tostring(L, -1));
			return false;
		}
		return true;
	}
}