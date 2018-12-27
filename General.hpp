#ifndef GENERAL_HPP_INCLUDED
#define GENERAL_HPP_INCLUDED

#include <algorithm>
#include <cxxabi.h>
#include <typeinfo>
#include <iostream>
#include <iomanip>
#include <string>
#include <locale>
#include <regex>

// For Testing Only : Returns correct form of the name of a type.
char* demangle(const char* s)
{
    char* res; int status;
    res = abi::__cxa_demangle(s,0,0,&status);
    return res;
}

// Returns the string in upper case.
std::string to_upper(std::string s)
{
    std::transform(s.begin(),s.end(),s.begin(),::toupper); return s;
}

#endif // GENERAL_HPP_INCLUDED
