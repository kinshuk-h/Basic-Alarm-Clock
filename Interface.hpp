#ifndef INTERFACE_HPP_INCLUDED
#define INTERFACE_HPP_INCLUDED

#include <windows.h>

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
};

bool Program::inited = false;
bool Program::running = false;
SHELLEXECUTEINFO Program::rSEI = {0};

#endif // INTERFACE_HPP_INCLUDED
