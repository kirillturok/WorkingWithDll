#include "pch.h"
#include <string>
#include <iostream>
#include <vector>

#define TARGET_STRING "old string" 
#define REPLACE_STRING "NEW STRING!!!"

#define MAX_STRING_LENGTH 256

using namespace std;


#ifdef __cplusplus    
extern "C" {
#endif
    __declspec(dllexport) bool  __cdecl replaceString(string targetString, string replaceString) {
        if (replaceString.length() > MAX_STRING_LENGTH - 1) return false;
        SYSTEM_INFO si;
        GetSystemInfo(&si);
    
        MEMORY_BASIC_INFORMATION mi;
        for (LPVOID lpAddress = si.lpMinimumApplicationAddress;
            lpAddress <= si.lpMaximumApplicationAddress;
            lpAddress = (LPVOID)((SIZE_T)lpAddress + mi.RegionSize)) {

            if (!VirtualQuery(lpAddress, &mi, sizeof(mi))) break;
            if (!(mi.Protect & PAGE_READWRITE) || (mi.Protect & PAGE_WRITECOPY)) continue;

            vector<char> buffer(mi.RegionSize);
            SIZE_T bytes;
            ReadProcessMemory(GetCurrentProcess(), lpAddress, &buffer[0], mi.RegionSize, &bytes);

            for (int i = 0; i < buffer.size(); i++) {
                if ((char*)lpAddress + i == targetString.c_str()) continue;
                if (strcmp(targetString.c_str(), &buffer[i]) == 0) {
                    WriteProcessMemory(GetCurrentProcess(), 
                        (LPVOID)((char*)lpAddress + i), 
                        replaceString.c_str(), replaceString.length() + 1, &bytes);
                }
            }
        }
    }
#ifdef __cplusplus
}
#endif


BOOL APIENTRY DllMain(HMODULE hModule,DWORD  ul_reason_for_call,LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        replaceString(TARGET_STRING, REPLACE_STRING);
        break;
    case DLL_THREAD_ATTACH:
        replaceString(TARGET_STRING, REPLACE_STRING);
        break;
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}


