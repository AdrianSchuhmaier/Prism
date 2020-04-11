#pragma once

#include <variant>

namespace Prism {
	enum class LuaType {
		Nil, Boolean, Number, String, Function, Userdata, Thread, Table
	};

	struct LuaNil { LuaType type() const { return LuaType::Nil; } };
	struct LuaBoolean { LuaType type() const { return LuaType::Boolean; } };
	struct LuaNumber { LuaType type() const { return LuaType::Number; } };
	struct LuaString { LuaType type() const { return LuaType::String; } };
	struct LuaFunction { LuaType type() const { return LuaType::Function; } };
	struct LuaUserdata { LuaType type() const { return LuaType::Userdata; } };
	struct LuaThread { LuaType type() const { return LuaType::Thread; } };
	struct LuaTable { LuaType type() const { return LuaType::Table; } };

	using LuaValue = std::variant<LuaNil, LuaBoolean, LuaNumber, LuaString, LuaFunction, LuaUserdata, LuaThread, LuaTable>;

	LuaType getType(const LuaValue& v)
	{
		return std::visit([](auto x) { return x.type(); }, v);
	}
}