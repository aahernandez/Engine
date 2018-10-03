#pragma once

#include <string>
#include <vector>

#ifndef __LUA_INC_H__
#define __LUA_INC_H__
#pragma comment( lib, "ThirdParty/Lua/Source/lua5.1.lib" )

#include "ThirdParty/Lua/Source/lua.hpp"

#endif

struct lua_State;

typedef int (*lua_CFunction) (lua_State *L);

class LuaBinding 
{
private:
	int m_level;

public:
	LuaBinding(const std::string& filename);
	LuaBinding(lua_State* newState);
	~LuaBinding();

	lua_State* m_luaState;

	void BindFunctionsToLua(std::string name, const luaL_reg* list, const int listSize);
	void PushBool(bool boolValue);
	bool CheckBool(int index);
	void PushFloat(float number);
	float CheckFloat(int index);
	void PushInteger(int number);
	int CheckInteger(int index);
	void PushString(std::string string);
	const char* CheckString(int index);
	void PushUserType(void* userData);
	void* CheckUserType(int index, std::string name);

	int GetStackSize();
	void PopOffStack(int numToPop);
	void ClearStack();
	
	void RunScript();
	void PushFunctionOntoStack(std::string functionName);
	void RunFunction(int numArguements, int numResults, int errorCode = 0);

	void BindFunction(std::string functionName, lua_CFunction luaFunction);
};