#pragma once
#include <string>
#include <vector>

typedef struct ComPortInfo
{
    std::string name;
    std::string value;
}ComPortInfo;

class CComPortInfoHandle
{
public:
    static std::vector<ComPortInfo> GetConPortInfo();
    static bool GenerateComPortManInIConfig(std::string, std::vector<ComPortInfo>&);
};

