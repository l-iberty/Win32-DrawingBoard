#ifndef _PLUGIN_LOADER_H_
#define _PLUGIN_LOADER_H_

#include <Windows.h>
#include <io.h>
#include <cassert>
#include <string>
#include <vector>

class PluginLoader {
  public:
    PluginLoader(const char *path);
    ~PluginLoader();

    PluginLoader(const PluginLoader &) = delete;
    PluginLoader& operator=(const PluginLoader &) = delete;

    const std::vector<HMODULE>& GetModules() const {
        return m_hModules;
    }

  private:
    std::vector<HMODULE> m_hModules;
};

PluginLoader::PluginLoader(const char *dir) {
    int hFile;
    struct _finddata_t fileinfo;

    if ((hFile = _findfirst(dir, &fileinfo)) != -1) {
        do {
            if (!(fileinfo.attrib & _A_SUBDIR)) {
                std::string path = dir;
                path.resize(path.size() - 1); // `dir' looks like "path\\to\\plugins\\*", we need to remove the last character '*'.
                path.append(fileinfo.name);
                if (path.find(".dll") != std::string::npos ||
                    path.find(".DLL") != std::string::npos) {
                    HMODULE hMod = ::LoadLibraryA(path.c_str());
                    if (hMod) {
                        m_hModules.push_back(hMod);
                    }
                }
            }
        } while (_findnext(hFile, &fileinfo) == 0);
    }
}

PluginLoader::~PluginLoader() {
    for (HMODULE hMod : m_hModules) {
        ::FreeLibrary(hMod);
    }
}

#endif // _PLUGIN_LOADER_H_