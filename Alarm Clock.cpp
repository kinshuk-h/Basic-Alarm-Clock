#include "Interface.hpp"
#include "Time.hpp"

using namespace std;
using namespace chrono;
using namespace std::literals;

bool its_time;

// Keeps checking for alarm hit, parallel to the program.
void try_alarm()
{
    m1.lock();
    if(Clock.check(current_time()))
    {
        its_time = true;
        system("cls");
        cout<<"BOOM!\n";
        SHELLEXECUTEINFO rSEI ={0}; rSEI.cbSize = sizeof( rSEI ); rSEI.lpVerb = "open";
        rSEI.lpFile = "";
        rSEI.lpParameters = "";
        rSEI.lpDirectory = "";
        rSEI.nShow = SW_HIDE; rSEI.fMask = SEE_MASK_NOCLOSEPROCESS;
        ShellExecuteEx( &rSEI );
        // Play the alarm for 120 seconds.
        WaitForSingleObject(rSEI.hProcess,120000);
        CloseHandle(rSEI.hProcess);
    }
    else its_time = false;
    m1.unlock();
}

int main(int argc, char** argv)
{
    //BindToRegistry(argv[0]);

    //thread t1(try_alarm);
    //t1.join();

    char buf[100]; auto old_count = Clock.count(); string s;
    INPUT_RECORD rec; DWORD ev; time_t raw; bool print = false;

    //std::cout<<demangle(typeid(decltype(tp)).name())<<"\n";
    //std::cout<<demangle(typeid(decltype(system_clock::now())).name())<<"\n";
    //long long dur = duration_cast<seconds>(tp-system_clock::now()).count();
    //if(dur<0) cout<<"Time already Elapsed!\n";
    //else if(dur==0) cout<<"Trring!\n";
    //else cout<<"Gotta wait now\n";
    //if(tp<=system_clock::now()) cout<<"Error : Cannot set an alarm in the past.\n";

    while(true)
    {
        if(!its_time)
        {
            if(!print)
            {
                cout<<"Basic Alarm Clock\n-----------------\n\n";
                cout<<"Enter an Option : \n";
                cout<<"1) Add a new alarm\n";
                cout<<"2) View, modify and delete existing alarms\n";
                cout<<"3) View current system time\n";
                cout<<"4) Alter settings : Wake up media, Volume, Permissions, etc.\n\n";
                //cout<<"Alarm Count : "<<Clock.count()<<"\n";
                print = true; if(Clock.count()!=old_count) { Clock.refresh(); old_count = Clock.count(); }
            }
            GetNumberOfConsoleInputEvents(in,&ev);
            if(ev>0&&!its_time)
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
                            cout<<"Enter time in format (HH:MM) or (DD/MM/YYYY HH:MM) : \n";
                            getline(cin,s); Clock.add(s); break;
                        case '2':
                            Clock.print(); cout<<"Modify Command Line for Basic Alarm Clock : \n";
                            cout<<"\nInstructions : \n\n";
                            cout<<"Enter the desired command to perform the required operation : \n";
                            cout<<"1) MODIFY [ALARM_INDEX] - Modifies Alarm at Index ALARM_INDEX-1. \n";
                            cout<<"2) DELETE [ALARM_INDEX] - Deletes Alarm at Index ALARM_INDEX-1. \n";
                            cout<<"3) EXIT - Exits from the utility. \n";
                            while(true)
                            {
                                cout<<"\n>> "; getline(cin,s); to_upper(s);
                                if(s=="EXIT") break;
                                else if(s.substr(0,6)=="MODIFY") {  }
                                else if(s.substr(0,6)=="DELETE") { Clock.remove(stoi(s.substr(s.find(' ')+1))); }
                                else cout<<"Error : Invalid Command\n";
                            }
                            break;
                        case '3':
                            cout<<"System time : \n";
                            raw = system_clock::to_time_t(system_clock::now());
                            strftime(buf,100,"%d/%m/%Y %H:%M",localtime(&raw));
                            cout<<buf<<"\n"; break;
                        }
                    }
                    cout<<"\n";
                }
            }
        }
    }
}
