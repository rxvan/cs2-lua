#include "Utility/Lua/Lua.h"

template <typename Class_t, typename... Args_t>
#ifdef _MAPPED_
Class_t*
#else
std::unique_ptr<Class_t>
#endif
MakeNew(Args_t&&... args) {
#ifdef _MAPPED_
	return new Class_t(std::forward<Args_t>(args)...);
#else
	return std::make_unique<Class_t>(std::forward<Args_t>(args)...);
#endif
}

namespace TWL {
	DWORD WINAPI MainThread(LPVOID lpParam) {
		FILE* pFile{ nullptr };
		if (AllocConsole() && freopen_s(&pFile, "CONOUT$", "w", stdout) == 0) {
			SetConsoleTitleA("Counter-Strike: 2 (Lua)");
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED);
			printf("CS:2 (Lua)\n\n");
		}
		else {
			MessageBoxA(NULL, "Failed to allocate console", "Error", MB_OK | MB_ICONERROR);
			return 0;
		}

		g_pLuaManager = MakeNew<LUA::CLuaManager>();
		if (!g_pLuaManager->Init()) {
			MessageBoxA(NULL, "Failed to initialize Lua Manager", "Error", MB_OK | MB_ICONERROR);
			return 0;
		}

		{
			g_pLuaManager->RegisterClass<LDR_DATA_TABLE_ENTRY>("LDR_DATA_TABLE_ENTRY", [&](sol::usertype<LDR_DATA_TABLE_ENTRY> UserType) -> void {
				UserType["InLoadOrderLinks"] = &LDR_DATA_TABLE_ENTRY::InLoadOrderLinks;
				UserType["InMemoryOrderLinks"] = &LDR_DATA_TABLE_ENTRY::InMemoryOrderLinks;
				UserType["InInitializationOrderLinks"] = &LDR_DATA_TABLE_ENTRY::InInitializationOrderLinks;
				UserType["DllBase"] = &LDR_DATA_TABLE_ENTRY::DllBase;
				UserType["EntryPoint"] = &LDR_DATA_TABLE_ENTRY::EntryPoint;
				UserType["SizeOfImage"] = &LDR_DATA_TABLE_ENTRY::SizeOfImage;
				UserType["FullDllName"] = &LDR_DATA_TABLE_ENTRY::FullDllName;
				UserType["BaseDllName"] = &LDR_DATA_TABLE_ENTRY::BaseDllName;
				UserType["Flags"] = &LDR_DATA_TABLE_ENTRY::Flags;
				UserType["ObsoleteLoadCount"] = &LDR_DATA_TABLE_ENTRY::ObsoleteLoadCount;
				UserType["TlsIndex"] = &LDR_DATA_TABLE_ENTRY::TlsIndex;
				UserType["HashLinks"] = &LDR_DATA_TABLE_ENTRY::HashLinks;
				UserType["TimeDateStamp"] = &LDR_DATA_TABLE_ENTRY::TimeDateStamp;
				UserType.set_function("IsValid", &LDR_DATA_TABLE_ENTRY::IsValid);
				}
			);

			g_pLuaManager->RegisterClass<PEB_LDR_DATA>("PEB_LDR_DATA", [&](sol::usertype<PEB_LDR_DATA> UserType) -> void {
				UserType["Length"] = &PEB_LDR_DATA::Length;
				UserType["Initialized"] = &PEB_LDR_DATA::Initialized;
				UserType["SsHandle"] = &PEB_LDR_DATA::SsHandle;
				UserType["InLoadOrderModuleList"] = &PEB_LDR_DATA::InLoadOrderModuleList;
				UserType["InMemoryOrderModuleList"] = &PEB_LDR_DATA::InMemoryOrderModuleList;
				UserType["InInitializationOrderModuleList"] = &PEB_LDR_DATA::InInitializationOrderModuleList;
				UserType["EntryInProgress"] = &PEB_LDR_DATA::EntryInProgress;
				UserType["ShutdownInProgress"] = &PEB_LDR_DATA::ShutdownInProgress;
				UserType["ShutdownThreadId"] = &PEB_LDR_DATA::ShutdownThreadId;
				}
			);

			g_pLuaManager->RegisterClass<PEB>("PEB", [&](sol::usertype<PEB> UserType) -> void {
				UserType["BeingDebugged"] = &PEB::BeingDebugged;
				UserType["NtGlobalFlag"] = &PEB::NtGlobalFlag;
				UserType["Ldr"] = &PEB::Ldr;
				UserType["ProcessParameters"] = &PEB::ProcessParameters;
				}
			);

			g_pLuaManager->RegisterClass<TEB>("TEB", [&](sol::usertype<TEB> UserType) -> void {
				UserType["ProcessEnvironmentBlock"] = &TEB::ProcessEnvironmentBlock;
				UserType["TlsExpansionSlots"] = &TEB::TlsExpansionSlots;
				}
			);
		}

		{
			g_pLuaManager->RegisterTable("utl", [&](sol::table UserTable) -> void {
				UserTable["get_module_entry"].set_function([&](sol::string_view strModuleName) -> PLDR_DATA_TABLE_ENTRY {
					PLDR_DATA_TABLE_ENTRY pLdrDataTableEntry = { 0 };
					if (!MEM::GetModuleEntry(strModuleName, &pLdrDataTableEntry))
						return nullptr;
					return pLdrDataTableEntry;
					}
				);
				UserTable["get_module_export"].set_function([&](PLDR_DATA_TABLE_ENTRY pLdrDataTableEntry, sol::string_view strExportName) -> unsigned __int64 {
					unsigned __int64 uExport{ 0x0 };
					if (!MEM::GetModuleExport(pLdrDataTableEntry, strExportName, &uExport))
						return 0x0;
					return uExport;
					}
				);
				UserTable["get_vfunc"].set_function([&](uintptr_t uAddr, int nIndex) -> uintptr_t {
					// convert uAddr to vtable
					if (uAddr <= 0x0)
						return 0x0;

					uintptr_t* pVTable = *reinterpret_cast<uintptr_t**>(uAddr);
					// get number of functions in vtable
					int nFunctions = 0;
					while (pVTable[nFunctions])
						++nFunctions;

					// check if nIndex is valid
					if (nIndex < 0 || nIndex >= nFunctions)
						return 0x0;

					// return the function address
					return pVTable[nIndex];
					}
				);
				UserTable["get_object_ptr"].set_function([&](sol::object UserObject) -> uintptr_t {
					return reinterpret_cast<uintptr_t>(UserObject.pointer());
					}
				);
				}
			);
		}

		try {
			// set the require path to G::GetPath()
			std::filesystem::path path{};
			if (G::GetPath(path)) {
				// we need to double escape the backslashes
				std::string strPath{ path.string() };
				for (size_t i = 0; i < strPath.size(); ++i) {
					if (strPath[i] == '\\') {
						strPath.insert(i, "\\");
						++i;
					}
				}

				g_pLuaManager->RunString(std::format("package.path = package.path .. '{0}\\\\?.lua;{0}\\\\?\\\\init.lua;{0}\\\\?.dll'", strPath.data()).c_str());

				g_pLuaManager->RunFile("autorun.lua");
			}
		}
		catch (const sol::error& Error) {
			printf("Error: %s\n", Error.what());
		}

		if (g_pLuaManager) {
#if _MAPPED_ 
			delete g_pLuaManager;
#else
			TWL::g_pLuaManager.reset();
#endif
			g_pLuaManager = nullptr;
		}

		FreeConsole();
		if (_fcloseall() != 0) {
			fclose(pFile);
			fclose(stdin);
			fclose(stdout);
			fclose(stderr);
		}

		FreeLibraryAndExitThread(static_cast<HMODULE>(lpParam), 0);
		return 0;
	}
}

// DllEntryPoint
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
	if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
		DisableThreadLibraryCalls(hModule);

		if (HANDLE hThread = CreateThread(NULL, NULL, TWL::MainThread, hModule, NULL, NULL); hThread)
			CloseHandle(hThread);
		else
			return FALSE;
	}
	else if (ul_reason_for_call == DLL_PROCESS_DETACH) {
		if (TWL::g_pLuaManager) {
#if _MAPPED_
			delete TWL::g_pLuaManager;
#else
			TWL::g_pLuaManager.reset();
#endif
		}
		}

	return TRUE;
	}
