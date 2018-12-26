/// For Concurrency and Time-Related Functions

#ifndef TIME_HPP_INCLUDED
#define TIME_HPP_INCLUDED

#include "General.hpp"

#include <sstream>
#include <fstream>
#include <vector>
#include <set>

#include <chrono>
#include <thread>
#include <mutex>
#include <ctime>

std::mutex m1;

using sys_clock = std::chrono::system_clock;
using system_point = std::chrono::time_point<sys_clock>;

inline auto current_time()
{
    return sys_clock::now();
}

class Alarm
{
    std::vector<system_point> times;
public:
    Alarm()
    {
        std::ifstream fin; fin.open("AlarmList.cfg");
        std::string s; std::stringstream ss; tm timeinfo;
        system_point event;
        while(getline(fin,s))
        {
            time_t raw; time(&raw); timeinfo = *localtime(&raw);
            ss.str(s); ss>>std::get_time(&timeinfo,"%d/%m/%Y %H:%M");
            event = sys_clock::from_time_t(mktime(&timeinfo));
            if(event>=current_time()) times.push_back(event);
        }
        fin.close();
    }
    ~Alarm() { this->refresh(); }
    std::vector<system_point>::size_type count() const noexcept
    {
        return times.size();
    }
    void refresh() noexcept
    {
        std::set<system_point> mys(times.begin(),times.end());
        times.assign(mys.begin(),mys.end());
        char s[100]; time_t raw; std::ofstream fout; fout.open("AlarmList.cfg",std::ios::trunc);
        for(size_t i=0;i<times.size();i++)
        {
            raw = sys_clock::to_time_t(times[i]);
            strftime(s,100,"%d/%m/%Y %H:%M",localtime(&raw));
            fout<<s<<"\n";
        }
        fout.close();
    }
    void add(std::string str) noexcept
    {
        if(regex_match(str,std::regex("(\\d{2}/\\d{2}/\\d{4} )?((([0-1]\\d)|[2][0-3]):[0-5]\\d)")))
        {
            time_t raw; tm timeinfo; std::stringstream ss; ss.str(str); time(&raw); timeinfo = *localtime(&raw);
            if(str.find('/')!=std::string::npos) ss>>std::get_time(&timeinfo,"%d/%m/%Y %H:%M");
            else
            {
                ss>>std::get_time(&timeinfo,"%H:%M");
                if(sys_clock::from_time_t(mktime(&timeinfo))<=current_time()) timeinfo.tm_mday++;
            }
            system_point point = sys_clock::from_time_t(mktime(&timeinfo));
            if(point<=current_time()) std::cout<<"Error : Cannot set an alarm in the past.\n";
            else times.push_back(point);
        }
        else std::cout<<"Error : Time format is Invalid. Try Again\n";
    }
    void remove(std::vector<system_point>::size_type idx) noexcept
    {
        times.erase(times.begin()+idx-1,times.begin()+idx);
    }
    bool check(system_point value)
    {
        auto it = std::find_if(times.begin(),times.end(),
                               [](system_point pt)
                               {
                                   return (std::chrono::duration_cast<std::chrono::seconds>
                                           (pt-current_time()).count())>60;
                               });
        times.erase(times.begin(),it-1);
        return false;
        //auto dur = duration_cast<seconds>(tp-system_clock::now()).count();
    }
    void print()
    {
        std::cout<<"Currently "<<this->count()<<" Alarm"<<(this->count()>1?"s ":" ")
                 <<(this->count()>1?"are":"is")<<" set as of now.\n\n";
        char buf[100]; time_t raw;
        for(const auto& tp:times)
        {
            raw = sys_clock::to_time_t(tp);
            strftime(buf,100,"%d/%m/%Y %H:%M",localtime(&raw));
            std::cout<<buf<<"\n";
        }
        std::cout<<"\n";
    }
};

// Global Alarm Clock
Alarm Clock;

#endif // TIME_HPP_INCLUDED
