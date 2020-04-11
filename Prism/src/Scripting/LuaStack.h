#pragma once

#include "LuaBase.h"
#include "LuaUtil.h"

namespace Prism {

	void push(lua_Number n) { lua_pushnumber(L, n); }
	void push(const char* s) { lua_pushstring(L, s); }

	template<typename... Ts>
	void pushargs(Ts... args) {
		(push(L, args), ...);
	}
}