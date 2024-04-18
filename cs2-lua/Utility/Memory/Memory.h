#ifndef __MEMORY_H__
#define __MEMORY_H__
#include "../Hash.h"

namespace TWL::MEM {
	// MODULE ENTRY
	STATIC NO_INLINE bool GetModuleEntry(const Hash_t, PLDR_DATA_TABLE_ENTRY*);
	STATIC ALWAYS_INLINE bool GetModuleEntry(const std::string_view strModule, PLDR_DATA_TABLE_ENTRY* pEntry) {
		return GetModuleEntry(FNV1A(strModule), pEntry);
	}

	// MODULE EXPORT
	STATIC NO_INLINE bool GetModuleExport(const PLDR_DATA_TABLE_ENTRY pEntry, const Hash_t uExportHash, uintptr_t* pExport);
	STATIC ALWAYS_INLINE bool GetModuleExport(const PLDR_DATA_TABLE_ENTRY pEntry, const std::string_view strExport, uintptr_t* pExport) {
		return GetModuleExport(pEntry, FNV1A(strExport), pExport);
	}
}
#include "Private/Memory.inl"
#include "Private/Modules.inl"
#endif // !__MEMORY_H__
