/// For Concurrency and Time-Related Functions

#ifndef TIME_HPP_INCLUDED
#define TIME_HPP_INCLUDED

#include "General.hpp"

#include <sstream>
#include <fstream>
#include <vector>
#include <set>

#include <chrono>
#include <atomic>
#include <thread>
#include <mutex>
#include <ctime>

std::mutex m1,m2;

using namespace std::literals;
using sys_clock = std::chrono::system_clock;
using system_point = std::chrono::time_point<sys_clock>;

inline auto current_time()
{
    return sys_clock::now();
}

template <typename Clock, typename Duration>
std::ostream& operator<<(std::ostream& os, const std::chrono::time_point<Clock, Duration>& timep)
{
    auto converted_timep = Clock::to_time_t(timep);
    os<<std::put_time(std::localtime(&converted_timep),"%d/%m/%Y %H:%M");
    return os;
}

template <typename Clock, typename Duration>
std::istream& operator>>(std::istream& is, std::chrono::time_point<Clock, Duration>& timep)
{
    std::string data; getline(is>>std::ws,data); tm tinfo = {};
    std::stringstream(data)>>std::get_time(&tinfo,"%d/%m/%Y %H:%M");
    timep = sys_clock::from_time_t(mktime(&tinfo)); return is;
}

template <typename Clock, typename Duration>
auto operator++(std::chrono::time_point<Clock,Duration>& tp)
{
    tp = Clock::from_time_t(Clock::to_time_t(tp)+86400); return tp;
}

template <typename Clock, typename Duration>
auto operator++(std::chrono::time_point<Clock,Duration>& tp,int)
{
    auto tp2 = tp; ++tp; return tp2;
}

class Alarm
{
    std::vector<system_point> times; int n; bool start;
public:
    Alarm()
    {
        n=-500; start=false;
        std::ifstream fin; fin.open("AlarmList.cfg");
        system_point event; auto spt = current_time();
        while(fin>>event) { if(event>=spt) { times.push_back(event); } }
        fin.close();
    }
    ~Alarm() { this->refresh(); Program::terminate(); }
    std::string ring()
    {
        std::stringstream ss;
        if(times.size()>0) { ss<<times[0]; return ss.str(); }
        return "";
    }
    std::vector<system_point>::size_type count() const noexcept
    {
        return times.size();
    }
    void refresh() noexcept
    {
        std::set<system_point> mys(times.begin(),times.end()); times.clear();
        auto spt = current_time(); for(const auto& time:mys) if(time>=spt) times.push_back(time);
        std::ofstream fout; fout.open("AlarmList.cfg",std::ios::trunc);
        for(const auto& time:times) fout<<time<<"\n";
        fout.close();
    }
    void add(std::string str) noexcept
    {
        if(regex_match(str,std::regex("(\\d{2}/\\d{2}/\\d{4} )?((([0-1]\\d)|[2][0-3]):[0-5]\\d)")))
        {
            system_point point; std::stringstream ss;
            if(str.find('/')!=std::string::npos) { ss.str(str); ss>>point; }
            else
            {
                ss<<current_time(); str = ss.str().substr(0,11) + str;
                ss.str(str); ss>>point; if(point<=current_time()) ++point;
            }
            if(point<=current_time()) std::cout<<"Error : Cannot set an alarm in the past.\n";
            else { times.push_back(point); } this->refresh();
        }
        else std::cout<<"Error : Time format is Invalid. Try Again\n";
    }
    void remove(std::vector<system_point>::size_type idx) noexcept
    {
        times.erase(times.begin()+idx-1);
    }
    bool check()
    {
        this->refresh(); auto spt = current_time();
        if(times.size()>0&&times[0]-spt<1s) { return true; }
        else { return false; }
    }
    void print()
    {
        std::cout<<"Currently "<<this->count()<<" Alarm"<<(this->count()!=1?"s ":" ")
                 <<(this->count()!=1?"are":"is")<<" set as of now.\n\n";
        for(const auto& tp:times) { std::cout<<tp<<"\n"; }
        if(this->count()>0) std::cout<<"\n";
    }
};

// Global Alarm Clock
Alarm Clock;

#endif // TIME_HPP_INCLUDED
