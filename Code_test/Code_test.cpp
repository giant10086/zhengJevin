// Code_test.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <Windows.h>
#include "stdio.h"

using namespace std;


void SimulateUselessKeyBoardClick()
{
	INPUT inp;
	inp.type = INPUT_KEYBOARD;
	inp.ki.wScan = 0;
	inp.ki.time = 0;
	inp.ki.dwExtraInfo = 0;
	inp.ki.wVk = 0xff;

	inp.ki.dwFlags = 0;
	SendInput(1, &inp, sizeof(INPUT));

	inp.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &inp, sizeof(INPUT));
	return;
}

#if 0
int main()
{
	int flag = 0;
	while (true)
	{
		LASTINPUTINFO lii;
		lii.cbSize = sizeof(LASTINPUTINFO);
		SimulateUselessKeyBoardClick();
		if (GetLastInputInfo(&lii))
		{
			unsigned long ulTickTimes = GetTickCount() - lii.dwTime;
			//LOG_DEBUG("GetTickCount %lu - lii.dwTime %lu = ulTickTimes %lu", GetTickCount(), lii.dwTime, ulTickTimes);
			cout << "time:" << ulTickTimes << endl;
			if (ulTickTimes >= 3 * 1000)
			{
				if (flag)
					continue;
#if 0
				INPUT m_InPut[2] = { 0 };
				//鼠标消息，需将type置为INPUT_MOUSE，如果是键盘消息,将type置为INPUT_KEYBOARD。
				m_InPut[0].type = m_InPut[1].type = INPUT_MOUSE;
				//将type置为鼠标消息后，其INPUT结构中的mi结构是可以使用的，hi、ki结构不可使用
				m_InPut[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
				m_InPut[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;
				SendInput(sizeof(m_InPut) / sizeof(m_InPut[0]), m_InPut, sizeof(INPUT));
#endif
				printf("hello\n");
				flag = 1;
			}
			else
			{
				flag = 0;
			}
		}
		Sleep(500);
	}
}
#else

#include <winerror.h>
#include <Windows.h>
#include <iostream>
#include <wbemidl.h>
#include <rpc.h>

#pragma comment(lib, "wbemuuid.lib")

int main() {
    HRESULT hres;
    IWbemLocator* pLoc = nullptr;
    IWbemServices* pSvc = nullptr;

    // 初始化COM库
    hres = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(hres)) {
        std::cerr << "Failed to initialize COM library. Error code: " << hres << std::endl;
        return 1;
    }

    // 创建WMI定位器
    hres = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID*)&pLoc);
    if (FAILED(hres)) {
        std::cerr << "Failed to create WMI locator. Error code: " << hres << std::endl;
        CoUninitialize();
        return 1;
    }

    // 连接到WMI服务
    hres = pLoc->ConnectServer((BSTR)L"ROOT\\WMI", nullptr, nullptr, 0, 0, 0, 0, &pSvc);
    if (FAILED(hres)) {
        std::cerr << "Failed to connect to WMI service. Error code: " << hres << std::endl;
        pLoc->Release();
        CoUninitialize();
        return 1;
    }

    // 设置安全级别
    hres = CoSetProxyBlanket(pSvc, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, nullptr, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE);
    if (FAILED(hres)) {
        std::cerr << "Failed to set proxy blanket. Error code: " << hres << std::endl;
        pSvc->Release();
        pLoc->Release();
        CoUninitialize();
        return 1;
    }

    // 查询显示器的EDID信息
    IEnumWbemClassObject* pEnumerator = nullptr;
    hres = pSvc->ExecQuery((BSTR)L"WQL", (BSTR)L"SELECT * FROM WmiMonitorID", WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, nullptr, &pEnumerator);
    if (FAILED(hres)) {
        std::cerr << "Failed to execute WMI query. Error code: " << hres << std::endl;
        pSvc->Release();
        pLoc->Release();
        CoUninitialize();
        return 1;
    }

    // 遍历查询结果
    IWbemClassObject* pclsObj = nullptr;
    ULONG uReturn = 0;
    while (pEnumerator) {
        hres = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);
        if (uReturn == 0) {
            break;
        }

        VARIANT vtProperty;
        hres = pclsObj->Get(L"EDIDData", 0, &vtProperty, 0, 0);
        if (SUCCEEDED(hres) && vtProperty.vt == (VT_UI1 | VT_ARRAY)) {
            // 解析EDID数据以获取刷新率等信息
            // 此处需要解析复杂的二进制数据
            // 可以使用EDID解析库或自定义解析逻辑来处理EDID数据
            std::cout << "EDID Data: (Parsing EDID data required)" << std::endl;
        }
        else
        {
            std::cout << "EDID Data: ERROR!" << std::endl;
        }

        VariantClear(&vtProperty);
        pclsObj->Release();
    }

    // 释放资源
    pEnumerator->Release();
    pSvc->Release();
    pLoc->Release();
    CoUninitialize();

    return 0;
}


#endif