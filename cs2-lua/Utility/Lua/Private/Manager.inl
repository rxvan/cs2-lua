#include "../Lua.h"
namespace TWL::LUA {
	NO_INLINE bool CLuaManager::Init() {
		if (m_pState)
			return true;

		static const auto MakeState = []() -> State_t {
#if _MAPPED_
			return new sol::state(DETAIL::LuaPanic);
#else
			return std::make_unique<sol::state>(DETAIL::LuaPanic);
#endif
			};
		try {
			m_pState = MakeState();
			if (!m_pState)
				return false;
		}
		catch (const std::exception& e) {
			std::printf("Failed to initialize Lua Manager: %s\n", e.what());
			return false;
		}

		// open all libraries including jit
		{
			m_pState->open_libraries(sol::lib::base, sol::lib::package, sol::lib::coroutine, sol::lib::string, sol::lib::os, sol::lib::math, sol::lib::table, sol::lib::debug, sol::lib::bit32, sol::lib::io, sol::lib::jit, sol::lib::ffi, sol::lib::utf8);
			m_pState->set_panic(DETAIL::LuaPanic);
			m_pState->set_exception_handler(DETAIL::LuaException);
			m_pState->set_function("print", DETAIL::LuaPrint);
		}

		// return true if everything is fine
		return true;
	}

	NO_INLINE void CLuaManager::Shutdown() {
		if (m_pState) {
			static const auto FreeState = [](State_t& pState) {
#if _MAPPED_
				delete pState;
#else
				pState.reset();
#endif
				pState = nullptr;
				};
			FreeState(m_pState);
		}
	}

	ALWAYS_INLINE void CLuaManager::RegisterTable(const std::string_view strClassName, std::function<void(sol::table)> fCallback) {
		if (!m_pState)
			return;

		fCallback(m_pState->create_named_table(strClassName.data()));
	}

	template<typename T>
	ALWAYS_INLINE void CLuaManager::RegisterClass(const std::string_view strClassName, std::function<void(sol::usertype<T>)> fCallback) {
		if (!m_pState)
			return;
		fCallback(m_pState->new_usertype<T>(strClassName.data()));
	}

	NO_INLINE std::optional<sol::protected_function_result> CLuaManager::RunFile(const std::string_view strFileName) {
		std::filesystem::path fsPath{};
		if (!G::GetPath(fsPath))
			return std::nullopt;

		fsPath /= strFileName;
		if (!std::filesystem::exists(fsPath))
			return std::nullopt;

		return m_pState->safe_script_file(fsPath.string());
	}

	NO_INLINE std::optional<sol::protected_function_result> CLuaManager::RunString(const std::string_view strString){
		return m_pState->safe_script(strString.data());
	}
}
