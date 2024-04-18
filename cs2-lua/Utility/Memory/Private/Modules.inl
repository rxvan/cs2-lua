#include "../Memory.h"
namespace TWL::MEM {
	STATIC NO_INLINE bool GetModuleEntry(const Hash_t uModuleHash, PLDR_DATA_TABLE_ENTRY* pEntry) {
		PPEB pPeb = reinterpret_cast<PPEB>(NtCurrentTeb()->ProcessEnvironmentBlock);
		if (pPeb == nullptr || pPeb->Ldr == nullptr || pPeb->Ldr->Initialized == 0)
			return false;

		PLIST_ENTRY pCurrentEntry = pPeb->Ldr->InMemoryOrderModuleList.Flink;
		while (pCurrentEntry != &pPeb->Ldr->InMemoryOrderModuleList) {
			PLDR_DATA_TABLE_ENTRY pCurrent = CONTAINING_RECORD(pCurrentEntry, LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);
			if (FNV1A(pCurrent->BaseDllName.Buffer, pCurrent->BaseDllName.Length / sizeof(wchar_t)) == uModuleHash) {
				*pEntry = pCurrent;
				return true;
			}
			pCurrentEntry = pCurrent->InMemoryOrderLinks.Flink;
		}

		return false;
	}

	STATIC NO_INLINE bool GetModuleExport(const PLDR_DATA_TABLE_ENTRY pEntry, const Hash_t uExportHash, uintptr_t* pExport) {
		if (pEntry == nullptr || pEntry->DllBase == nullptr)
			return false;

		PIMAGE_DOS_HEADER pDosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(pEntry->DllBase);
		PIMAGE_NT_HEADERS pNtHeaders = reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<uintptr_t>(pDosHeader) + pDosHeader->e_lfanew);
		PIMAGE_EXPORT_DIRECTORY pExportDirectory = reinterpret_cast<PIMAGE_EXPORT_DIRECTORY>(reinterpret_cast<uintptr_t>(pDosHeader) + pNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);

		PDWORD pdwFunctions = reinterpret_cast<PDWORD>(reinterpret_cast<uintptr_t>(pDosHeader) + pExportDirectory->AddressOfFunctions);
		PDWORD pdwNames = reinterpret_cast<PDWORD>(reinterpret_cast<uintptr_t>(pDosHeader) + pExportDirectory->AddressOfNames);
		PWORD pwOrdinals = reinterpret_cast<PWORD>(reinterpret_cast<uintptr_t>(pDosHeader) + pExportDirectory->AddressOfNameOrdinals);

		for (DWORD i = 0; i < pExportDirectory->NumberOfNames; ++i) {
			if (FNV1A(reinterpret_cast<char*>(reinterpret_cast<uintptr_t>(pDosHeader) + pdwNames[i])) == uExportHash) {
				*pExport = reinterpret_cast<uintptr_t>(pDosHeader) + pdwFunctions[pwOrdinals[i]];
				return true;
			}
		}

		return false;
	}
}