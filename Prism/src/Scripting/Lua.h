#pragma once

#include "Util/Log/Log.h"

namespace Prism {

	// test structure
	struct player {
		std::string name;
		uint32_t level;
	};

#define OCtype_(c, x) std::pair<std::string, x c::*>
#define OCtype(c, x1, x2) OCtype_(c, x1) ## , ## OCtype_(c, x2)


	class Lua {
	public:
		Lua();
		~Lua();

		bool Execute(const std::string& script);
		bool ExecuteFile(const std::string& file);

		template<class T, typename... X>
		T LoadObject(const std::string& name, std::tuple<X...> configs);

		// test method
		player GetPlayer();

		bool Push(const std::string& name);
		void Pop(const std::string& name);
		uint32_t GetInt(const std::string& name);


	private:
		void* m_Instance = nullptr; // lua_State*

		//TODO: let get return variant

		template<typename T>
		T Get(const std::string& name);

		template<>
		std::string Get(const std::string& name) { return "abc"; };

		template<>
		uint32_t Get(const std::string& name) { return GetInt(name); };
	};

	template<class T, typename... X>
	inline T Lua::LoadObject(const std::string& name, std::tuple<X...> configs)
	{
		T result;

		if (!Push(name)) return result;

		std::apply([&](auto&&... args) {((result.*args.second = Get<uint32_t>(args.first)), ...);}, configs);
		//for (const auto& config : configs)
		//{
		//	const auto m = config.second;
		//	result.*m = Get<uint32_t>(config.first);
		//}
		Pop(name);
		return result;
	}
}