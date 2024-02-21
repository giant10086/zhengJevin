#include <iostream>
#include <vector>
#include <string>
#include <Windows.h>
#include <cstdio>
#include "ComPortInfoHandle.h"
#include <UserEnv.h>
#include <tchar.h>
#include <TlHelp32.h>
#pragma comment(lib, "Userenv.lib")

using namespace std;

HANDLE RunProcessByUser(const TCHAR* ptszProcessPath, TCHAR* ptszParameter, LONG* plExitCode, BOOL bBlock)
{
	DWORD dwSessionId;
	HANDLE hTokenThis = NULL;
	HANDLE hThisProcess = NULL;
	HANDLE hTokenDup = NULL;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	DWORD dwCreationFlag = NORMAL_PRIORITY_CLASS | CREATE_UNICODE_ENVIRONMENT | CREATE_NEW_CONSOLE; //CREATE_NEW_CONSOLE
	LPVOID pEnv = NULL;
	TCHAR tszDeskTop[20];

	if (!ptszProcessPath)
	{
		printf("ptszProcessPath is NULL !");
		return NULL;
	}

	if (ptszProcessPath[0] == '\0')
	{
		printf("ptszProcessPath is NULL !");
		return NULL;
	}

	hThisProcess = GetCurrentProcess();
	dwSessionId = WTSGetActiveConsoleSessionId();

	if (!hThisProcess)
	{
		printf("GetCurrentProcess failed, ret[%d]!", GetLastError());
		return NULL;
	}

	OpenProcessToken(hThisProcess, TOKEN_ALL_ACCESS, &hTokenThis);

	// 复制一个进程令牌，目的是为了修改session id属性，以便在其它session中创建进程
	if (!DuplicateTokenEx(hTokenThis, MAXIMUM_ALLOWED, NULL, SecurityIdentification, TokenPrimary, &hTokenDup))
	{
		printf("DuplicateTokenEx failed, ret[%d]!", GetLastError());
		return NULL;
	}

	SetTokenInformation(hTokenDup, TokenSessionId, &dwSessionId, sizeof(DWORD)); // 把session id设置到备份的令牌中

	ZeroMemory(&si, sizeof(STARTUPINFO));
	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
	_tcscpy_s(tszDeskTop, sizeof(tszDeskTop) / sizeof(tszDeskTop[0]), _T("Default"));

	si.cb = sizeof(STARTUPINFO);
	si.lpDesktop = tszDeskTop;
	si.wShowWindow = SW_SHOW;
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

	//#include <UserEnv.h>
	CreateEnvironmentBlock(&pEnv, hTokenDup, FALSE); // 创建环境块
	if (!CreateProcessAsUser(hTokenDup, ptszProcessPath, ptszParameter, NULL, NULL, FALSE, dwCreationFlag, pEnv, NULL, &si, &pi))
	{
		printf("CreateProcessAsUser failed, ret[%d]!", GetLastError());
		return NULL;
	}

	if (bBlock)
	{
		WaitForSingleObject(pi.hProcess, INFINITE);
	}

	GetExitCodeProcess(pi.hProcess, (DWORD*)plExitCode);

	if (hTokenDup)
	{
		CloseHandle(hTokenDup);
	}
	if (hTokenThis)
	{
		CloseHandle(hTokenThis);
	}
	if (pi.hThread != INVALID_HANDLE_VALUE)
	{
		CloseHandle(pi.hThread);
	}
	if (pi.hProcess != INVALID_HANDLE_VALUE)
	{
		if (bBlock) {
			CloseHandle(pi.hProcess);
			return NULL;
		}
		else {
			return pi.hProcess;
		}
	}

	return NULL;
}

int main()
{
    vector<ComPortInfo> vComList = CComPortInfoHandle::GetConPortInfo();
    //CComPortInfoHandle::GenerateComPortManInIConfig("C:\\Users\\zjw\\Desktop\\ComPortMan\\ComPortMan.INI", vComList);
#if 1
	LONG exitCode = 0;
	RunProcessByUser(L"C:\\Program Files\\EasyTool\\client\\devcon.exe", (TCHAR*)L"disable = ports", &exitCode, TRUE);

	Sleep(3000);
	RunProcessByUser(L"C:\\Program Files\\EasyTool\\client\\devcon.exe", (TCHAR*)L"enable = ports", &exitCode, TRUE);
#else
	if (std::system("\"C:\\Program Files\\EasyTool\\client\\devcon.exe\" disable =ports"))
		printf("disable ports failed!");

	Sleep(3000);
	if (std::system("\"C:\\Program Files\\EasyTool\\client\\devcon.exe\" enable =ports"))
		printf("enable ports failed!");
#endif


	if (std::system("sc create ComportMan binPath=\"C:\\Program Files\\EasyTool\\client\\ComPortMan.exe\""))
		printf("create ComportMan server failed!");

	std::system("sc stop ComportMan");
	if (std::system("sc start ComportMan"))
		printf("start ComportMan server failed!");
	if (std::system("sc stop ComportMan"))
		printf("stop server failed!");
	if (system("sc delete ComportMan"))
		printf("stop server failed!");
#if 0
    if (std::system("sc create ComportMan binPath=\"C:\\Users\\zjw\\Desktop\\ComPortMan\\ComPortMan.exe\""))
        cout << "create ComportMan server failed!" << std::endl;
    if (std::system("sc start ComportMan"))
        cout << "start ComportMan server failed!" << std::endl;
    if (std::system("sc stop ComportMan"))
        cout << "stop server failed!" << std::endl;
    if (system("sc delete ComportMan"))
        cout << "stop server failed!" << std::endl;
#endif
    getchar();
    return 0;
}

