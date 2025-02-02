#include "Lua.h"

#include "LuaUtil.h"
#include "LuaBase.h"

namespace Prism {

#define L (lua_State*)m_Instance

	Lua::Lua()
	{
		m_Instance = (void*)luaL_newstate();

		lua_gc(L, LUA_GCSTOP, 0);
		luaL_openlibs(L);
		lua_gc(L, LUA_GCRESTART, 0);
	}

	Lua::~Lua()
	{
		lua_close(L);
	}

	bool Lua::Execute(const std::string& script)
	{
		if (checkLua(L, luaL_dostring(L, script.c_str())))
			return true;
		else
			return false;
	}

	bool Lua::ExecuteFile(const std::string& file)
	{
		if (checkLua(L, luaL_dofile(L, file.c_str())))
			return true;
		else
			return false;
	}


	player Lua::GetPlayer()
	{
		player player{};
		lua_getglobal(L, "player");
		if (lua_istable(L, -1))
		{
			lua_pushstring(L, "name");
			lua_gettable(L, -2);
			player.name = lua_tostring(L, -1);
			lua_pop(L, 1);

			lua_pushstring(L, "level");
			lua_gettable(L, -2);
			player.level = static_cast<uint32_t>(lua_tointeger(L, -1));
			lua_pop(L, 1);
		}
		return player;
	}

	bool Lua::Push(const std::string& name)
	{
		lua_getglobal(L, name.c_str());
		return lua_istable(L, -1);
	}

	void Lua::Pop(const std::string& name)
	{
		lua_pop(L, 1);
	}

	uint32_t Lua::GetInt(const std::string& name)
	{
		uint32_t result = 100;
		lua_pushstring(L, name.c_str());
		lua_gettable(L, -2);
		result = static_cast<uint32_t>(lua_tointeger(L, -1));
		lua_pop(L, 1);
		return result;
	}
}