#include <iostream>
#include <sstream>
#include <WS2tcpip.h>
#include <WinSock2.h> // 包含必要的Windows套接字API头文件
#include <iphlpapi.h>
#include <iomanip>
#include <map>
#pragma comment(lib, "iphlpapi.lib") // 链接到IP帮助程序库
#pragma comment(lib, "ws2_32.lib") // 链接到WinSock2库

using namespace std;

int main() {
    ULONG bufLen = 15000; // 初始化缓冲区大小为足够大的值，以便容纳所有适配器信息
    PIP_ADAPTER_ADDRESSES pAddresses = (PIP_ADAPTER_ADDRESSES)malloc(bufLen);
    DWORD ret;

    // 调用GetAdaptersAddresses函数，获取所有网络接口的信息
    ret = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_SKIP_FRIENDLY_NAME, NULL, pAddresses, &bufLen);

    if (ret == ERROR_BUFFER_OVERFLOW) {
        free(pAddresses);
        pAddresses = (PIP_ADAPTER_ADDRESSES)malloc(bufLen); // 如果缓冲区太小，重新分配内存
        ret = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_SKIP_FRIENDLY_NAME, NULL, pAddresses, &bufLen);
    }
    if (ret != ERROR_SUCCESS) {
        std::cerr << "GetAdaptersAddresses failed with error: " << ret << std::endl;
        free(pAddresses);
        return 1;
    }

    if (pAddresses->OperStatus == IfOperStatusUp) {
        // 遍历所有网络适配器信息
        int count = 0;
        for (PIP_ADAPTER_ADDRESSES pCurrAddresses = pAddresses; pCurrAddresses != NULL; pCurrAddresses = pCurrAddresses->Next) {

            count++;
            std::wcout << L"count: " << count << std::endl;

            // 输出适配器名称
            std::string strAdapterName(pCurrAddresses->AdapterName);
            std::cout << "A Adapter Name: " << strAdapterName << std::endl;
            //std::wcout << L"Adapter Name: " << pCurrAddresses->AdapterName << std::endl;

            // 构造并输出MAC地址
            std::wstringstream macStream;
            for (UINT i = 0; i < pCurrAddresses->PhysicalAddressLength; i++) {
                macStream << std::hex << std::uppercase << std::setfill(L'0') << std::setw(2) << static_cast<int>(pCurrAddresses->PhysicalAddress[i]);
                if (i < pCurrAddresses->PhysicalAddressLength - 1)
                    macStream << L"-";
            }
            //std::wcout << L"MAC Address: " << macStream.str() << std::endl;
            std::wstring wideStr(macStream.str());
            const wchar_t* wideCStr = wideStr.c_str();
            
            size_t len = wcslen(wideCStr) + 1;
            // 分配足够的缓冲区来存储转换后的窄字符
            size_t bufferSize = 0;
            wcstombs_s(&bufferSize, nullptr, 0, wideCStr, 0);
            char* narrowBuffer = new char[bufferSize];
            // 使用 wcstombs_s 将宽字符转换为窄字符
            wcstombs_s(&len, narrowBuffer, bufferSize, wideCStr, _TRUNCATE);

            std::string narrowStr(narrowBuffer);
            delete[] narrowBuffer;

            std::cout << "A MAC Address: " << narrowStr.c_str() << std::endl;
            // 输出友好名称（即传输名称）
            //std::wcout << L"Friendly Name: " << pCurrAddresses->FriendlyName << std::endl << std::endl;

            std::string strShellString("shell:::{208D2C60-3AEA-1069-A2D7-08002B30309D}\\::{7007ACC7-3202-11D1-AAD2-00805FC1270E}\\::");
            strShellString.append(strAdapterName);
            ShellExecuteA(NULL, "open", "explorer.exe", strShellString.c_str(), NULL, SW_SHOW);
        }
    }

    free(pAddresses);

    map<string, string> test;

    test["hello"] = "world";
    cout << test.count("hello")<<endl;
    return 0;
}