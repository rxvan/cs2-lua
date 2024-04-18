#include "../Lua.h"
namespace TWL::LUA {
	STATIC NO_INLINE int DETAIL::LuaPanic(lua_State* pLuaState) {
		lua_getglobal(pLuaState, "debug");
		lua_getfield(pLuaState, -1, "traceback");
		lua_call(pLuaState, 0, 1);
		const char* pError = lua_tostring(pLuaState, -1);
		lua_pop(pLuaState, 2);
		printf("Lua Panic: %s\n", pError);
		return 0;
	}

	STATIC NO_INLINE int DETAIL::LuaException(lua_State* pLuaState, sol::optional<const std::exception&> oEx, sol::string_view strExceptionInfo) {
		return printf("Lua Exception: %s\n", oEx.value_or(std::exception(strExceptionInfo.data())).what());
	}

	STATIC NO_INLINE const char* DETAIL::GetCurrentFile(sol::this_state State) {
		lua_Debug Debug;
		lua_getstack(State.lua_state(), 1, &Debug);
		lua_getinfo(State.lua_state(), "S", &Debug);
		return std::filesystem::path(Debug.source).filename().string().substr(1).c_str(); // Skip the @
	}

	/*
	* LJLIB_PUSH("tostring")
LJLIB_CF(print)
{
  ptrdiff_t i, nargs = L->top - L->base;
  cTValue *tv = lj_tab_getstr(tabref(L->env), strV(lj_lib_upvalue(L, 1)));
  int shortcut;
  if (tv && !tvisnil(tv)) {
    copyTV(L, L->top++, tv);
  } else {
    setstrV(L, L->top++, strV(lj_lib_upvalue(L, 1)));
    lua_gettable(L, LUA_GLOBALSINDEX);
    tv = L->top-1;
  }
  shortcut = (tvisfunc(tv) && funcV(tv)->c.ffid == FF_tostring)
              && !gcrefu(basemt_it(G(L), LJ_TNUMX));
  for (i = 0; i < nargs; i++) {
    cTValue *o = &L->base[i];
    const char *str;
    size_t size;
    MSize len;
    if (shortcut && (str = lj_strfmt_wstrnum(L, o, &len)) != NULL) {
      size = len;
    } else {
      copyTV(L, L->top+1, o);
      copyTV(L, L->top, L->top-1);
      L->top += 2;
      lua_call(L, 1, 1);
      str = lua_tolstring(L, -1, &size);
      if (!str)
	lj_err_caller(L, LJ_ERR_PRTOSTR);
      L->top--;
    }
    if (i)
      putchar('\t');
    fwrite(str, 1, size, stdout);
  }
  putchar('\n');
  return 0;
}
	*/

	STATIC NO_INLINE int DETAIL::LuaPrint(lua_State* pLuaState) {
		int iNArgs = lua_gettop(pLuaState);
		lua_getglobal(pLuaState, "tostring");
		for (int i = 1; i <= iNArgs; i++) {
			const char* pStr;
			size_t iSize;
			lua_pushvalue(pLuaState, -1);
			lua_pushvalue(pLuaState, i);
			lua_call(pLuaState, 1, 1);
			pStr = lua_tolstring(pLuaState, -1, &iSize);
			if (!pStr)
				return luaL_error(pLuaState, "tostring must return a string to print");
			if (i > 1)
				putchar('\t');
			fwrite(pStr, sizeof(char), iSize, stdout);
			lua_pop(pLuaState, 1);
		}
		putchar('\n');
		return 0;
	}
}