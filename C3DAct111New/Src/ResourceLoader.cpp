#include "ResourceLoader.h"
#include <unordered_map>

namespace {
    std::unordered_map<std::string, char*> files;
};

void ResourceLoader::Reset()
{
    for (auto itr = files.begin(); itr != files.end();)
    {
        delete  itr->second;
        itr = files.erase(itr);
    }
    files.clear();
}

char* ResourceLoader::Load(std::string filename)
{
    char* p = files.at(filename);
    if (p == nullptr)
        return nullptr;
    return p;
}