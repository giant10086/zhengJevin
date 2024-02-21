#include "ComPortInfoHandle.h"
#include <Windows.h>
#include <iostream>
#include <vector>

#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383

using namespace std;

vector<ComPortInfo> CComPortInfoHandle::GetConPortInfo()
{
    HKEY hKey;
    vector<ComPortInfo> vComList;
    DWORD cKeyNum;
    int ret = RegOpenKeyExA(HKEY_LOCAL_MACHINE, "HARDWARE\\DEVICEMAP\\SERIALCOMM", 0, KEY_READ, &hKey);
    if (ret == ERROR_SUCCESS) {
        int iRet = RegQueryInfoKey(
            hKey,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            &cKeyNum,
            NULL,
            NULL,
            NULL,
            NULL);

        if (cKeyNum > 0 && ERROR_SUCCESS == iRet)
        {
            CHAR* pszDSName = NULL;
            pszDSName = (char*)malloc(MAX_VALUE_NAME);
            if (pszDSName)
            {
                memset(pszDSName, 0, MAX_VALUE_NAME);

                for (int index = 0; index < (int)cKeyNum; ++index)
                {
                    CHAR subKeyName[MAX_KEY_LENGTH];
                    memset(subKeyName, 0, MAX_KEY_LENGTH);
                    DWORD nBuffLen = MAX_KEY_LENGTH;//·ÀÖ¹ ERROR_MORE_DATA 234L ´íÎó
                    DWORD subKeyNameSize = sizeof(subKeyName) / sizeof(subKeyName[0]);
                    ret = RegEnumValueA(hKey, index, subKeyName, &subKeyNameSize, NULL, NULL, (LPBYTE)pszDSName, &nBuffLen);
                    if (ERROR_SUCCESS == ret)
                    {
                        ComPortInfo stInfo;
                        stInfo.name = subKeyName;
                        stInfo.value = pszDSName;
                        vComList.push_back(stInfo);
                    }
                    else
                    {
                        cout << "EnumValue failed ret:" << ret << endl;
                    }
                }
                free(pszDSName);
            }
            else
                cout << "malloc failed" << endl;

        }

        RegCloseKey(hKey);
    }
    else
    {
        std::cerr << "Failed to open registry key." << ret << std::endl;
    }

    return vComList;
}

bool CComPortInfoHandle::GenerateComPortManInIConfig(std::string filePath, std::vector<ComPortInfo>& vComList)
{
    if (!vComList.size())
    {
        return false;
    }

    WritePrivateProfileStringA("Settings", "WriteLogFile", "1", filePath.c_str());
    WritePrivateProfileStringA("Settings", "LogFile", "C:\\Users\\zjw\\Desktop\\ComPortMan\\ComPortMan.log", filePath.c_str());
    WritePrivateProfileStringA("Settings", "LogLevel", "1", filePath.c_str());
    for (size_t i = 0; i < vComList.size(); ++i)
    {
        string NAME = "ComPorts";
        NAME += to_string(i + 1);
        std::cout << "Subkey name: " << vComList[i].name << std::endl;
        cout << "strDSName:" << vComList[i].value << std::endl;
        cout << "Name:" << NAME << std::endl;

        WritePrivateProfileStringA(NAME.c_str(), "KernelName", vComList[i].name.c_str(), filePath.c_str());
        WritePrivateProfileStringA(NAME.c_str(), "PortName", vComList[i].value.c_str(), filePath.c_str());
    }

    return true;
}

