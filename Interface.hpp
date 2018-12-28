#ifndef INTERFACE_HPP_INCLUDED
#define INTERFACE_HPP_INCLUDED

#include <iostream>
#include <windows.h>
#include <string>

HANDLE in  = GetStdHandle(STD_INPUT_HANDLE),
       out = GetStdHandle(STD_OUTPUT_HANDLE);

void GOTO(SHORT X, SHORT Y)
{ COORD CPos = {X,Y}; SetConsoleCursorPosition(out,CPos); }

// Binds the program to the local machine's registry, enabling launch of the code at startup.
void BindToRegistry(char* path)
{
    HKEY hkey = nullptr;
    RegCreateKey(HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",&hkey);
    RegSetValueExA(hkey, "Alarm Clock", 0, REG_SZ , (BYTE*)path, sizeof(char)*(lstrlen(path)+1));
    RegCloseKey(hkey);
}

int TimedMessageBox(HWND hWnd, LPCSTR sText, LPCSTR sCaption, UINT uType, DWORD dwMilliseconds)
{
    typedef int(__stdcall *MSGBOXWAPI)(IN HWND hWnd, IN LPCSTR lpText,
                IN LPCSTR lpCaption, IN UINT uType, IN WORD wLanguageId, IN DWORD dwMilliseconds);
    int iResult; HMODULE hUser32 = LoadLibraryA("user32.dll");
    if (hUser32)
    {
        auto MessageBoxTimeoutA = (MSGBOXWAPI)GetProcAddress(hUser32, "MessageBoxTimeoutA");
        iResult = MessageBoxTimeoutA(hWnd, sText, sCaption, uType, 0, dwMilliseconds);
        FreeLibrary(hUser32);
    }
    else iResult = MessageBox(hWnd, sText, sCaption, uType);
    return iResult;
}

class Program
{
    static bool inited,running;
    static SHELLEXECUTEINFO rSEI;
public:
    static void init()
    {
        if(not inited)
        {
            rSEI.cbSize = sizeof( rSEI ); rSEI.lpVerb = "open";
            rSEI.lpFile = "bin\\Song Player.exe";
            rSEI.lpParameters = "a.mp3 1000";
            rSEI.lpDirectory = "";
            rSEI.nShow = SW_HIDE; rSEI.fMask = SEE_MASK_NOCLOSEPROCESS;
        }
    }
    static void run()
    {
        if(not running)
        {
            ShellExecuteEx( &rSEI );
            WaitForSingleObject(rSEI.hProcess,60000);
            running=true;
        }
    }
    static void end()
    {
        if(running) { TerminateProcess(rSEI.hProcess,0); running=false; }
    }
    static void terminate()
    {
        TerminateProcess(rSEI.hProcess,0);
    }
    static void reload(const std::string& path, size_t volume)
    {
        std::cout<<"New Song : "<<path<<"\n";
        rSEI.lpParameters = (path+std::to_string(volume)).c_str();
        rSEI.nShow = SW_SHOWNORMAL;
        ShellExecuteEx(&rSEI);
        WaitForSingleObject(rSEI.hProcess,60000);
        TerminateProcess(rSEI.hProcess,0);
        rSEI.nShow = SW_HIDE;
    }
};

bool Program::inited = false;
bool Program::running = false;
SHELLEXECUTEINFO Program::rSEI = {0};

#endif // INTERFACE_HPP_INCLUDED
