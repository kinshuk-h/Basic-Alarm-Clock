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

#endif // INTERFACE_HPP_INCLUDED
