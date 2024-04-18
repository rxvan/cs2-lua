#ifndef __GLOBALS_H__
#define __GLOBALS_H__
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#define NOMINMAX
#include <Windows.h>
#include <winternl.h>
#include <iostream>
#include <string>
#include <string_view>

#include <mutex>

#include <filesystem>

#ifndef _MAPPED_
#include <memory> // only use smart pointers if not mapped, because CRT will handle the initialization there but not when mapped
#endif

#define MOONJIT_VERSION LUAJIT_VERSION
#include <Sol/sol.hpp> // includes lua.hpp

#ifdef RegisterClass
#undef RegisterClass
#endif

#ifdef __clang__ // If the compiler is clang
#define ALWAYS_INLINE __attribute__((always_inline))	// Tell the compiler to always inline this function
#define NO_INLINE __attribute__((noinline))				// Tell the compiler to never inline this function
#define NO_RETURN __attribute__((noreturn))				// Tell the compiler that this function will never return
#define NO_VTABLE __attribute__((novtable))				// Tell the compiler to not generate a vtable for this class
#define NO_EXCEPTION __attribute__((nothrow))			// Tell the compiler that this function will never throw an exception
#define NO_ALIAS __attribute__((noalias))				// Tell the compiler that this function will only access the memory it is given, not global or thread-local-storage
#define NO_SANITIZE __attribute__((no_sanitize("undefined")))	// Tell the compiler to not sanitize this function for undefined behavior
#define NO_OPTIMIZE __attribute__((optimize("O0")))		// Tell the compiler to not optimize this function
#define OPTIIMIZE __attribute__((optimize("O3")))		// Tell the compiler to optimize this function
#elif defined(_MSC_VER) // If the compiler is MSVC
#define ALWAYS_INLINE __forceinline
#define NO_INLINE __declspec(noinline)
#define NO_RETURN __declspec(noreturn)
#define NO_VTABLE __declspec(novtable)
#define NO_EXCEPTION __declspec(nothrow)
#define NO_ALIAS __declspec(noalias)
#define NO_SANITIZE __declspec(no_sanitize_address)
#define NO_OPTIMIZE
#define OPTIMIZE
#else // If the compiler is not clang or MSVC
#error "Unsupported compiler breh"
#endif // __clang__
#define NO_INLINE_NO_OPTIMIZE NO_INLINE NO_OPTIMIZE
#define ALWAYS_INLINE_NO_OPTIMIZE ALWAYS_INLINE NO_OPTIMIZE
#define NO_INLINE_OPTIMIZE NO_INLINE OPTIMIZE
#define ALWAYS_INLINE_OPTIMIZE ALWAYS_INLINE OPTIMIZE
#define STATIC static // The static keyword is used in C and C++ to declare variables and functions that are local to a file or a function. When a variable or function is declared as static, it tells the compiler that the variable or function is only accessible within the file or function it is declared in.
#define EXTERN extern // The extern keyword is used in C and C++ to declare a variable or function that is defined in another file or module. When a variable or function is declared as extern, it tells the compiler that the variable or function is defined in another module (outside the current file) and can be used in the current file.
#define CONST const // const is a type qualifier that tells the compiler that the value of the variable will not change. It can be applied to any data type including primitive data types, user-defined data types, and pointers.
#define VOLATILE volatile // volatile is a type qualifier that prevents the compiler from optimizing away the variable. It tells the compiler that the value of the variable can change at any time--without any action being taken by the code the compiler finds nearby.
#define MUTABLE mutable // mutable is a storage class specifier that can be applied to member variables of a class. It allows a member variable to be modified by a const member function, e.g. a member function that is declared as const.
#define CONSTEXPR constexpr	// Tell the compiler that this function can be evaluated at compile time
#define CONSTEVAL consteval // Tell the compiler that this function can be evaluated at compile time

namespace TWL {
	typedef unsigned long long QWORD, * PQWORD;
	typedef unsigned long ULONG, * PULONG;
	typedef void* PVOID, * LPVOID;
	typedef wchar_t WCHAR, * PWCHAR;
	typedef WCHAR* PWSTR;
	typedef unsigned short USHORT, * PUSHORT;
	typedef unsigned char BYTE, * PBYTE;

	typedef struct _UNICODE_STRING {
		USHORT Length;
		USHORT MaximumLength;
		PWSTR Buffer;
	} UNICODE_STRING, * PUNICODE_STRING;

	enum _LDR_DLL_LOAD_REASON {
		LoadReasonStaticDependency = 0,
		LoadReasonStaticForwarderDependency = 1,
		LoadReasonDynamicForwarderDependency = 2,
		LoadReasonDelayloadDependency = 3,
		LoadReasonDynamicLoad = 4,
		LoadReasonAsImageLoad = 5,
		LoadReasonAsDataLoad = 6,
		LoadReasonUnknown = -1
	};

	struct _RTL_BALANCED_NODE
	{
		union
		{
			struct _RTL_BALANCED_NODE* Children[2];                             //0x0
			struct
			{
				struct _RTL_BALANCED_NODE* Left;                                //0x0
				struct _RTL_BALANCED_NODE* Right;                               //0x4
			};
		};
		union
		{
			struct
			{
				UCHAR Red : 1;                                                    //0x8
				UCHAR Balance : 2;                                                //0x8
			};
			ULONG ParentValue;                                                  //0x8
		};
	};

	typedef struct _PEB_LDR_DATA
	{
		ULONG Length;                                                           //0x0
		UCHAR Initialized;                                                      //0x4
		VOID* SsHandle;                                                         //0x8
		struct _LIST_ENTRY InLoadOrderModuleList;                               //0xc
		struct _LIST_ENTRY InMemoryOrderModuleList;                             //0x14
		struct _LIST_ENTRY InInitializationOrderModuleList;                     //0x1c
		VOID* EntryInProgress;                                                  //0x24
		UCHAR ShutdownInProgress;                                               //0x28
		VOID* ShutdownThreadId;                                                 //0x2c
	} PEB_LDR_DATA, * PPEB_LDR_DATA;

	typedef struct _PEB
	{
		UCHAR InheritedAddressSpace;                                            //0x0
		UCHAR ReadImageFileExecOptions;                                         //0x1
		UCHAR BeingDebugged;                                                    //0x2
		union
		{
			UCHAR BitField;                                                     //0x3
			struct
			{
				UCHAR ImageUsesLargePages : 1;                                    //0x3
				UCHAR IsProtectedProcess : 1;                                     //0x3
				UCHAR IsImageDynamicallyRelocated : 1;                            //0x3
				UCHAR SkipPatchingUser32Forwarders : 1;                           //0x3
				UCHAR IsPackagedProcess : 1;                                      //0x3
				UCHAR IsAppContainer : 1;                                         //0x3
				UCHAR IsProtectedProcessLight : 1;                                //0x3
				UCHAR IsLongPathAwareProcess : 1;                                 //0x3
			};
		};
		VOID* Mutant;                                                           //0x4
		VOID* ImageBaseAddress;                                                 //0x8
		struct _PEB_LDR_DATA* Ldr;                                              //0xc
		struct _RTL_USER_PROCESS_PARAMETERS* ProcessParameters;                 //0x10
		VOID* SubSystemData;                                                    //0x14
		VOID* ProcessHeap;                                                      //0x18
		struct _RTL_CRITICAL_SECTION* FastPebLock;                              //0x1c
		union _SLIST_HEADER* volatile AtlThunkSListPtr;                         //0x20
		VOID* IFEOKey;                                                          //0x24
		union
		{
			ULONG CrossProcessFlags;                                            //0x28
			struct
			{
				ULONG ProcessInJob : 1;                                           //0x28
				ULONG ProcessInitializing : 1;                                    //0x28
				ULONG ProcessUsingVEH : 1;                                        //0x28
				ULONG ProcessUsingVCH : 1;                                        //0x28
				ULONG ProcessUsingFTH : 1;                                        //0x28
				ULONG ProcessPreviouslyThrottled : 1;                             //0x28
				ULONG ProcessCurrentlyThrottled : 1;                              //0x28
				ULONG ProcessImagesHotPatched : 1;                                //0x28
				ULONG ReservedBits0 : 24;                                         //0x28
			};
		};
		union
		{
			VOID* KernelCallbackTable;                                          //0x2c
			VOID* UserSharedInfoPtr;                                            //0x2c
		};
		ULONG SystemReserved;                                                   //0x30
		union _SLIST_HEADER* volatile AtlThunkSListPtr32;                       //0x34
		VOID* ApiSetMap;                                                        //0x38
		ULONG TlsExpansionCounter;                                              //0x3c
		VOID* TlsBitmap;                                                        //0x40
		ULONG TlsBitmapBits[2];                                                 //0x44
		VOID* ReadOnlySharedMemoryBase;                                         //0x4c
		VOID* SharedData;                                                       //0x50
		VOID** ReadOnlyStaticServerData;                                        //0x54
		VOID* AnsiCodePageData;                                                 //0x58
		VOID* OemCodePageData;                                                  //0x5c
		VOID* UnicodeCaseTableData;                                             //0x60
		ULONG NumberOfProcessors;                                               //0x64
		ULONG NtGlobalFlag;                                                     //0x68
		union _LARGE_INTEGER CriticalSectionTimeout;                            //0x70
		ULONG HeapSegmentReserve;                                               //0x78
		ULONG HeapSegmentCommit;                                                //0x7c
		ULONG HeapDeCommitTotalFreeThreshold;                                   //0x80
		ULONG HeapDeCommitFreeBlockThreshold;                                   //0x84
		ULONG NumberOfHeaps;                                                    //0x88
		ULONG MaximumNumberOfHeaps;                                             //0x8c
		VOID** ProcessHeaps;                                                    //0x90
		VOID* GdiSharedHandleTable;                                             //0x94
		VOID* ProcessStarterHelper;                                             //0x98
		ULONG GdiDCAttributeList;                                               //0x9c
		struct _RTL_CRITICAL_SECTION* LoaderLock;                               //0xa0
		ULONG OSMajorVersion;                                                   //0xa4
		ULONG OSMinorVersion;                                                   //0xa8
		USHORT OSBuildNumber;                                                   //0xac
		USHORT OSCSDVersion;                                                    //0xae
		ULONG OSPlatformId;                                                     //0xb0
		ULONG ImageSubsystem;                                                   //0xb4
		ULONG ImageSubsystemMajorVersion;                                       //0xb8
		ULONG ImageSubsystemMinorVersion;                                       //0xbc
		ULONG ActiveProcessAffinityMask;                                        //0xc0
		ULONG GdiHandleBuffer[34];                                              //0xc4
		VOID(*PostProcessInitRoutine)();                                       //0x14c
		VOID* TlsExpansionBitmap;                                               //0x150
		ULONG TlsExpansionBitmapBits[32];                                       //0x154
		ULONG SessionId;                                                        //0x1d4
		union _ULARGE_INTEGER AppCompatFlags;                                   //0x1d8
		union _ULARGE_INTEGER AppCompatFlagsUser;                               //0x1e0
		VOID* pShimData;                                                        //0x1e8
		VOID* AppCompatInfo;                                                    //0x1ec
		struct _UNICODE_STRING CSDVersion;                                      //0x1f0
		struct _ACTIVATION_CONTEXT_DATA* ActivationContextData;                 //0x1f8
		struct _ASSEMBLY_STORAGE_MAP* ProcessAssemblyStorageMap;                //0x1fc
		struct _ACTIVATION_CONTEXT_DATA* SystemDefaultActivationContextData;    //0x200
		struct _ASSEMBLY_STORAGE_MAP* SystemAssemblyStorageMap;                 //0x204
		ULONG MinimumStackCommit;                                               //0x208
		VOID* SparePointers[4];                                                 //0x20c
		ULONG SpareUlongs[5];                                                   //0x21c
		VOID* WerRegistrationData;                                              //0x230
		VOID* WerShipAssertPtr;                                                 //0x234
		VOID* pUnused;                                                          //0x238
		VOID* pImageHeaderHash;                                                 //0x23c
		union
		{
			ULONG TracingFlags;                                                 //0x240
			struct
			{
				ULONG HeapTracingEnabled : 1;                                     //0x240
				ULONG CritSecTracingEnabled : 1;                                  //0x240
				ULONG LibLoaderTracingEnabled : 1;                                //0x240
				ULONG SpareTracingBits : 29;                                      //0x240
			};
		};
		ULONGLONG CsrServerReadOnlySharedMemoryBase;                            //0x248
		ULONG TppWorkerpListLock;                                               //0x250
		struct _LIST_ENTRY TppWorkerpList;                                      //0x254
		VOID* WaitOnAddressHashTable[128];                                      //0x25c
		VOID* TelemetryCoverageHeader;                                          //0x45c
		ULONG CloudFileFlags;                                                   //0x460
		ULONG CloudFileDiagFlags;                                               //0x464
		CHAR PlaceholderCompatibilityMode;                                      //0x468
		CHAR PlaceholderCompatibilityModeReserved[7];                           //0x469
		struct _LEAP_SECOND_DATA* LeapSecondData;                               //0x470
		union
		{
			ULONG LeapSecondFlags;                                              //0x474
			struct
			{
				ULONG SixtySecondEnabled : 1;                                     //0x474
				ULONG Reserved : 31;                                              //0x474
			};
		};
		ULONG NtGlobalFlag2;                                                    //0x478
	} PEB, * PPEB;

	typedef struct _LDR_DATA_TABLE_ENTRY
	{
		struct _LIST_ENTRY InLoadOrderLinks;                                    //0x0
		struct _LIST_ENTRY InMemoryOrderLinks;                                  //0x8
		struct _LIST_ENTRY InInitializationOrderLinks;                          //0x10
		VOID* DllBase;                                                          //0x18
		VOID* EntryPoint;                                                       //0x1c
		ULONG SizeOfImage;                                                      //0x20
		struct _UNICODE_STRING FullDllName;                                     //0x24
		struct _UNICODE_STRING BaseDllName;                                     //0x2c
		union
		{
			UCHAR FlagGroup[4];                                                 //0x34
			ULONG Flags;                                                        //0x34
			struct
			{
				ULONG PackagedBinary : 1;                                         //0x34
				ULONG MarkedForRemoval : 1;                                       //0x34
				ULONG ImageDll : 1;                                               //0x34
				ULONG LoadNotificationsSent : 1;                                  //0x34
				ULONG TelemetryEntryProcessed : 1;                                //0x34
				ULONG ProcessStaticImport : 1;                                    //0x34
				ULONG InLegacyLists : 1;                                          //0x34
				ULONG InIndexes : 1;                                              //0x34
				ULONG ShimDll : 1;                                                //0x34
				ULONG InExceptionTable : 1;                                       //0x34
				ULONG ReservedFlags1 : 2;                                         //0x34
				ULONG LoadInProgress : 1;                                         //0x34
				ULONG LoadConfigProcessed : 1;                                    //0x34
				ULONG EntryProcessed : 1;                                         //0x34
				ULONG ProtectDelayLoad : 1;                                       //0x34
				ULONG ReservedFlags3 : 2;                                         //0x34
				ULONG DontCallForThreads : 1;                                     //0x34
				ULONG ProcessAttachCalled : 1;                                    //0x34
				ULONG ProcessAttachFailed : 1;                                    //0x34
				ULONG CorDeferredValidate : 1;                                    //0x34
				ULONG CorImage : 1;                                               //0x34
				ULONG DontRelocate : 1;                                           //0x34
				ULONG CorILOnly : 1;                                              //0x34
				ULONG ChpeImage : 1;                                              //0x34
				ULONG ReservedFlags5 : 2;                                         //0x34
				ULONG Redirected : 1;                                             //0x34
				ULONG ReservedFlags6 : 2;                                         //0x34
				ULONG CompatDatabaseProcessed : 1;                                //0x34
			};
		};
		USHORT ObsoleteLoadCount;                                               //0x38
		USHORT TlsIndex;                                                        //0x3a
		struct _LIST_ENTRY HashLinks;                                           //0x3c
		ULONG TimeDateStamp;                                                    //0x44
		struct _ACTIVATION_CONTEXT* EntryPointActivationContext;                //0x48
		VOID* Lock;                                                             //0x4c
		struct _LDR_DDAG_NODE* DdagNode;                                        //0x50
		struct _LIST_ENTRY NodeModuleLink;                                      //0x54
		struct _LDRP_LOAD_CONTEXT* LoadContext;                                 //0x5c
		VOID* ParentDllBase;                                                    //0x60
		VOID* SwitchBackContext;                                                //0x64
		struct _RTL_BALANCED_NODE BaseAddressIndexNode;                         //0x68
		struct _RTL_BALANCED_NODE MappingInfoIndexNode;                         //0x74
		ULONG OriginalBase;                                                     //0x80
		union _LARGE_INTEGER LoadTime;                                          //0x88
		ULONG BaseNameHashValue;                                                //0x90
		enum _LDR_DLL_LOAD_REASON LoadReason;                                   //0x94
		ULONG ImplicitPathOptions;                                              //0x98
		ULONG ReferenceCount;                                                   //0x9c
		ULONG DependentLoadFlags;                                               //0xa0
		UCHAR SigningLevel;                                                     //0xa4

		bool IsValid() const {
			return DllBase != nullptr;
		}
	} LDR_DATA_TABLE_ENTRY, * PLDR_DATA_TABLE_ENTRY;

	typedef struct _TEB {
		PVOID Reserved1[12];
		PPEB ProcessEnvironmentBlock;
		PVOID Reserved2[399];
		BYTE Reserved3[1952];
		PVOID TlsSlots[64];
		BYTE Reserved4[8];
		PVOID Reserved5[26];
		PVOID ReservedForOle;
		PVOID Reserved6[4];
		PVOID TlsExpansionSlots;
	} TEB, * PTEB;

	typedef struct _RTL_USER_PROCESS_PARAMETERS {
		BYTE Reserved1[16];
		PVOID Reserved2[10];
		UNICODE_STRING ImagePathName;
		UNICODE_STRING CommandLine;
	} RTL_USER_PROCESS_PARAMETERS, * PRTL_USER_PROCESS_PARAMETERS;

	namespace G {
		ALWAYS_INLINE bool GetPath(std::filesystem::path& fsPath) {
			const auto& fsCurrentPath = std::filesystem::current_path();
			fsPath = fsCurrentPath / "config" / "cs2-lua";
			return std::filesystem::create_directories(fsPath) || std::filesystem::exists(fsPath);
		}
	}
}
#endif // !__GLOBALS_H__
