#ifndef __LUA_H__
#define __LUA_H__
#include "../../Globals.h"
#include "../Memory/Memory.h"
#include "../Hash.h"

#ifdef _MAPPED_
typedef sol::state* State_t;
#else
typedef std::unique_ptr<sol::state> State_t;
#endif

namespace TWL {
	namespace LUA {
		namespace DETAIL {
			STATIC NO_INLINE int LuaPanic(lua_State*);
			STATIC NO_INLINE int LuaException(lua_State*, sol::optional<const std::exception&>, sol::string_view);
			STATIC NO_INLINE const char* GetCurrentFile(sol::this_state);

			// function to replace the default "print" function in lua
			STATIC NO_INLINE int LuaPrint(lua_State*);
		}

		class CLuaManager {
		private:
			State_t m_pState{ nullptr };
		public:
			CLuaManager() = default;
			~CLuaManager() = default;

			NO_INLINE bool Init();
			NO_INLINE void Shutdown();

			ALWAYS_INLINE void RegisterTable(const std::string_view, std::function<void(sol::table)>); // push a class then add any members to it (i.e. functions, variables, etc)
			template <typename T>
			ALWAYS_INLINE void RegisterClass(const std::string_view, std::function<void(sol::usertype<T>)>); // push a class then add any members to it (i.e. functions, variables, etc)

			NO_INLINE std::optional<sol::protected_function_result> RunFile(const std::string_view);
			NO_INLINE std::optional<sol::protected_function_result> RunString(const std::string_view);
		};
	}

#ifdef _MAPPED_
	STATIC LUA::CLuaManager*
#else
	STATIC std::unique_ptr<LUA::CLuaManager>
#endif
		g_pLuaManager{ nullptr };
}
#include "Private/Detail.inl"
#include "Private/Manager.inl"
#endif // !__LUA_H__
