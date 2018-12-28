#include "Interface.hpp"
#include "Time.hpp"

using namespace std;
using namespace chrono;

bool its_time;
bool done;

// Keeps checking for alarm hit, parallel to the program.
void try_alarm()
{
    m1.lock();
    while(true)
    {
        if(not its_time && Clock.check())
        {
            Program::init();
            Program::run();
            Program::end();
            its_time = true;
        }
        else if(its_time) { its_time=false; Program::end(); }
    }
    m1.unlock();
}

void code()
{
    string s,path; size_t vol,pos;
    INPUT_RECORD rec; DWORD ev; bool print = false;
    auto spt = current_time();
    while(true)
    {
        if(current_time()-spt>2s) { spt=current_time(); GOTO(0,4); std::cout<<spt; GOTO(0,13); }
        if(!its_time)
        {
            if(!print)
            {
                system("cls");
                cout<<"Basic Alarm Clock\n-----------------\n\n";
                cout<<"System time : \n"<<spt<<"\n\n";
                cout<<"Enter an Option : \n\n";
                cout<<"1) Add a new alarm\n";
                cout<<"2) View existing alarms\n";
                cout<<"3) Modify and delete existing alarms\n";
                cout<<"4) Alter settings : Wake up media, Volume, etc.\n\n";
                print = true;
            }
            GetNumberOfConsoleInputEvents(in,&ev);
            if(ev>0&&!its_time)
            {
                ReadConsoleInput(in,&rec,1,&ev);
                if(rec.EventType==KEY_EVENT)
                {
                    if(rec.Event.KeyEvent.bKeyDown)
                    {
                        switch(rec.Event.KeyEvent.uChar.AsciiChar)
                        {
                        case '1':
                            cout<<"Enter time in format (HH:MM) or (DD/MM/YYYY HH:MM) : \n";
                            getline(cin,s); Clock.add(s); print = false; break;
                        case '2':
                            Clock.print(); print = false; Sleep(2000);
                            break;
                        case '3':
                            print = false; Clock.print();
                            if(Clock.count()>0)
                            {
                                cout<<"Action Command Line for Basic Alarm Clock : \n\n";
                                cout<<"Enter the desired command to perform the required operation : \n\n";
                                cout<<"1) MODIFY <AIDX> <NEW_TIME> \n"
                                      "   [ Modifies Alarm at Index AIDX-1, replacing with NEW_TIME ] \n";
                                cout<<"2) DELETE <AIDX> \n"
                                      "   [ Deletes Alarm at Index AIDX-1 ] \n";
                                cout<<"3) EXIT \n"
                                      "   [ Exits from the utility ] \n";
                                while(true)
                                {
                                    cout<<"\n>> "; getline(cin,s); s=to_upper(s);
                                    if(s=="EXIT"||s.empty()) break;
                                    else if(s.substr(0,6)=="MODIFY")
                                    {
                                        pos = s.find(' ');
                                        Clock.remove(stoi(s.substr(pos+1,s.size()-s.find(' ',pos+1))));
                                        s = s.substr(s.find(' ',pos+1)+1);
                                        if(not s.empty()) Clock.add(s);
                                    }
                                    else if(s.substr(0,6)=="DELETE") { Clock.remove(stoi(s.substr(s.find(' ')+1))); }
                                    else cout<<"Error : Invalid Command\n";
                                }
                            }
                            else Sleep(2000);
                            break;
                        case '4':
                            print = false; std::cout<<"Type in the new path followed by the new volume (b/w 0-1000) : \n";
                            std::cin>>path>>vol; Program::reload(path,vol%1000); std::cout<<"\n"; break;
                        }
                    }
                    if(not print) cout<<"\n";
                }
            }
        }
    }
}

int main(int argc, char** argv)
{
    try
    {
        //BindToRegistry(argv[0]);
        done=false;
        thread t1(try_alarm);
        thread t2(code);
        t1.join();
        t2.join();
    }
    catch(exception& e)
    {
        cerr<<"Error : "<<e.what()<<"\n";
    }
}
