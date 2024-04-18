#ifndef __HASH_H__
#define __HASH_H__
#include "../Globals.h"
namespace TWL {
	typedef uint32_t Hash_t;

	STATIC ALWAYS_INLINE CONSTEXPR size_t StrLen(const char* szString) {
		return *szString ? 1 + StrLen(szString + 1) : 0;
	}

	STATIC ALWAYS_INLINE CONSTEXPR size_t StrLen(const wchar_t* szString) {
		return *szString ? 1 + StrLen(szString + 1) : 0;
	}

	STATIC NO_INLINE CONSTEXPR Hash_t FNV1A(const char* szString, size_t uLen, Hash_t uHash = 2166136261) {
		for (size_t i = 0; i < uLen; ++i) {
			uHash ^= szString[i];
			uHash *= 16777619;
		}
		return uHash;
	}

	STATIC NO_INLINE CONSTEXPR Hash_t FNV1A(const wchar_t* szString, size_t uLen, Hash_t uHash = 2166136261) {
		for (size_t i = 0; i < uLen; ++i) {
			uHash ^= szString[i];
			uHash *= 16777619;
		}
		return uHash;
	}

	STATIC ALWAYS_INLINE CONSTEXPR Hash_t FNV1A(const std::string_view strString) {
		return FNV1A(strString.data(), strString.size());
	}

	STATIC ALWAYS_INLINE CONSTEXPR Hash_t FNV1A(const std::wstring_view strString) {
		return FNV1A(strString.data(), strString.size());
	}
}
#endif // !__HASH_H__
