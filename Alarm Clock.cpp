#include <algorithm>
#include <windows.h>
#include <cxxabi.h>
#include <typeinfo>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <string>
#include <chrono>
#include <vector>
#include <ctime>

using namespace std;
using namespace chrono;
using namespace std::literals;

class Alarm
{
    vector<time_point<system_clock>> times;
public:
    Alarm()
    {
        ifstream fin; fin.open("AlarmList.cfg");
        string s; tm t; stringstream ss;
        while(getline(fin,s))
        {
            time_t raw; time(&raw); t = *gmtime(&raw);
            ss.str(s); ss>>get_time(&t,"%d/%m/%Y %H:%M:%S");
            times.push_back(system_clock::from_time_t(mktime(&t)));
        }
        fin.close();
    }
    vector<time_point<system_clock>>::size_type count() const noexcept
    {
        return times.size();
    }
    void refresh()
    {
        char s[100]; time_t raw; ofstream fout; fout.open("AlarmList.cfg",ios::trunc);
        for(size_t i=0;i<times.size();i++)
        {
            raw = system_clock::to_time_t(times[i]);
            strftime(s,100,"%d/%m/%Y %H:%M:%S",localtime(&raw));
            fout<<s<<"\n";
        }
        fout.close();
    }
    void add(time_point<system_clock> point)
    {
        times.push_back(point);
    }
    bool check(time_point<system_clock> value)
    {
        return false;
    }
};

// For Testing Only : Returns correct form of the name of a type.
char* demangle(string s)
{
    char* res; int status;
    res = abi::__cxa_demangle(s.c_str(),0,0,&status);
    return res;
}

// Binds the program to the local machine's registry, enabling launch of the code at startup.
void BindToRegistry(char* path)
{
    HKEY hkey = nullptr;
    RegCreateKey(HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",&hkey);
    RegSetValueExA(hkey, "Alarm Clock", 0, REG_SZ , (BYTE*)path, sizeof(char)*(lstrlen(path)+1));
    RegCloseKey(hkey);
}

int main(int argc, char** argv)
{
    //BindToRegistry(argv[0]);

    Alarm Clock; char buf[100];

    HANDLE in = GetStdHandle(STD_INPUT_HANDLE);
    INPUT_RECORD rec; DWORD ev; tm timeinfo; time_t raw;
    time_point<system_clock> event; stringstream ss; string s;

    //std::cout<<demangle(typeid(decltype(tp)).name())<<"\n";
    //std::cout<<demangle(typeid(decltype(system_clock::now())).name())<<"\n";
    //long long dur = duration_cast<seconds>(tp-system_clock::now()).count();
    //if(dur<0) cout<<"Time already Elapsed!\n";
    //else if(dur==0) cout<<"Trring!\n";
    //else cout<<"Gotta wait now\n";
    //if(tp<=system_clock::now()) cout<<"Error : Cannot set an alarm in the past.\n";

    bool print = false;
    while(true)
    {
        if(!print)
        {
            cout<<"Basic Alarm Clock\n-----------------\n\n";
            cout<<"Enter an Option : \n";
            cout<<"1) Add a new alarm\n";
            cout<<"2) View, modify and delete existing alarms\n";
            cout<<"3) View current system time\n";
            cout<<"4) Alter settings : Wake up media, Volume, Permissions, etc.\n\n";
            cout<<"Currently "<<Clock.count()<<" Alarm(s) are set as of now.\n\n";
            print = true; Clock.refresh();
        }
        GetNumberOfConsoleInputEvents(in,&ev);
        if(ev>0)
        {
            ReadConsoleInput(in,&rec,1,&ev);
            if(rec.EventType==KEY_EVENT)
            {
                if(rec.Event.KeyEvent.bKeyDown)
                {
                    print = false;
                    switch(rec.Event.KeyEvent.uChar.AsciiChar)
                    {
                    case '1':
                        cout<<"Enter time in the format \"DD/MM/YYYY HH:MM:SS\" : \n";
                        getline(cin,s); ss.str(s); time(&raw); timeinfo = *gmtime(&raw);
                        ss>>get_time(&timeinfo,"%d/%m/%Y %H:%M:%S");
                        event = system_clock::from_time_t(mktime(&timeinfo));
                        Clock.add(event); break;
                    case '2':
                        break;
                    case '3':
                        cout<<"System time : \n";
                        raw = system_clock::to_time_t(system_clock::now());
                        strftime(buf,100,"%d/%m/%Y %H:%M:%S",localtime(&raw));
                        cout<<buf<<"\n"; break;
                    }
                }
                cout<<"\n";
            }
        }
    }
}
