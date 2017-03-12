
#include "xdSound.hpp"
#include "xdEngine/Platform/Platform.hpp"
#include <cstdio>

#ifdef WINDOWS
    bool WINAPI DllMain(_In_ void* _DllHandle, _In_ unsigned long _Reason, _In_opt_ void* _Reserved)
    {
        switch (_Reason)
        {
        case DLL_PROCESS_ATTACH:
            printf("DllMain, DLL_PROCESS_ATTACH\n");
            break;
        case DLL_THREAD_ATTACH:
            printf("DllMain, DLL_THREAD_ATTACH\n");
            break;
        case DLL_THREAD_DETACH:
            printf("DllMain, DLL_THREAD_DETACH\n");
            break;
        case DLL_PROCESS_DETACH:
            printf("DllMain, DLL_PROCESS_DETACH\n");
            break;
        default:
            printf("DllMain, ????\n");
            break;
        }
        return true;
    }
#elif LINUX
    int main(int argc, char* argv[])
    {

        return 0;
    }
#endif
