#include <Windows.h>
#include <iphlpapi.h>
#include <iostream>

#pragma comment(lib, "iphlpapi.lib")

int main() {
    FIXED_INFO fi;
    ULONG ulOutBufLen = sizeof(FIXED_INFO);

    if (GetNetworkParams(&fi, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
        FIXED_INFO* pFixedInfo = (FIXED_INFO*)malloc(ulOutBufLen);
        if (pFixedInfo) {
            if (GetNetworkParams(pFixedInfo, &ulOutBufLen) == NO_ERROR) {
                std::cout << "DNS Servers:\n";
                IP_ADDR_STRING* pDnsServer = &(pFixedInfo->DnsServerList);
                while (pDnsServer) {
                    std::cout << pDnsServer->IpAddress.String << "\n";
                    pDnsServer = pDnsServer->Next;
                }
            }
            free(pFixedInfo);
        }
    }

    return 0;
}
