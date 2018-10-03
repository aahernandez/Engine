#include "ThirdParty/Lua/Wrappers/LuaBinding.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/DeveloperConsole.hpp"
#include "Engine/Core/EngineCommon.hpp"

void WriteToDevConsole(const char* str)
{
	g_theDevConsole->ConsolePrintf(str);
}

static int luaWriteToDevConsole(lua_State* L) 
{
	const char* str = lua_tostring(L, 1);
	WriteToDevConsole(str);
	return 0;
}

LuaBinding::LuaBinding(const std::string& filename)
{
	m_luaState = luaL_newstate();
	luaL_openlibs(m_luaState);
	if (luaL_loadfile(m_luaState, filename.c_str())) 
	{
		DebuggerPrintf("Error: script not loaded (%s)", filename.c_str());
		m_luaState = 0;
	}
	BindFunction("Write", luaWriteToDevConsole);
}

LuaBinding::LuaBinding(lua_State* newState)
{
	m_luaState = newState;
}

LuaBinding::~LuaBinding()
{
}

void LuaBinding::BindFunctionsToLua(std::string name, const luaL_reg* list, const int listSize)
{
	lua_createtable(m_luaState, 0, listSize);

	for (int listIndex = 0; listIndex < listSize; listIndex++)
	{
		lua_pushcfunction(m_luaState, list[listIndex].func);
		lua_setfield(m_luaState, -2, list[listIndex].name);
	}
	lua_setfield(m_luaState, LUA_GLOBALSINDEX, name.c_str());
}

void LuaBinding::PushBool(bool boolValue)
{
	PushInteger((int)boolValue);
// 	lua_pushboolean(m_luaState, (int)boolValue);
}

bool LuaBinding::CheckBool(int index)
{
	//not tested
	return lua_toboolean(m_luaState, index) != 0;
}

void LuaBinding::PushFloat(float number)
{
	lua_pushnumber(m_luaState, number);
}

float LuaBinding::CheckFloat(int index)
{
	return (float)luaL_checknumber(m_luaState, index);
}

void LuaBinding::PushInteger(int number)
{
	lua_pushinteger(m_luaState, number);
}

int LuaBinding::CheckInteger(int index)
{
	return (int)luaL_checkinteger(m_luaState, index);
}

void LuaBinding::PushString(std::string string)
{
	lua_pushstring(m_luaState, string.c_str());
}

const char* LuaBinding::CheckString(int index)
{
	return luaL_checkstring(m_luaState, index);
}

void LuaBinding::PushUserType(void* userData)
{
	lua_pushlightuserdata(m_luaState, userData);
}

void* LuaBinding::CheckUserType(int index, std::string name)
{
	void* udata = lua_touserdata(m_luaState, index);
	if (udata == 0)
	{
		luaL_typerror(m_luaState, index, name.c_str());
	}
	return udata;
}

int LuaBinding::GetStackSize()
{
	return lua_gettop(m_luaState);
}

void LuaBinding::PopOffStack(int numToPop)
{
	lua_pop(m_luaState, numToPop);
}

void LuaBinding::ClearStack()
{
	int stackSize = GetStackSize();
	PopOffStack(stackSize);
}

void LuaBinding::RunScript()
{
	lua_pcall(m_luaState, 0, 0, 0);
}

void LuaBinding::PushFunctionOntoStack(std::string functionName)
{
	lua_getglobal(m_luaState, functionName.c_str());
}

void LuaBinding::RunFunction(int numArguements, int numResults, int errorCode /*= 0*/)
{
	lua_pcall(m_luaState, numArguements, numResults, errorCode);
}

void LuaBinding::BindFunction(std::string functionName, lua_CFunction luaFunction)
{
	lua_pushcfunction(m_luaState, luaFunction);
	lua_setglobal(m_luaState, functionName.c_str());
}
