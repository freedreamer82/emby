//
// Created by marco on 01/02/23.
//

#include "StdLogProcessor.hh"
#include <iostream>


void StdLogProcessor::onLog(EmbyLog::LogLevel level, EmbyLog::LogClass logClass, const EmbyLibs::String& message)
{
    LogMask mask = getLevelMask();
    //@todo check here the levels...
    auto m = message;
     std::string s(m.c_str());
    std::cout << EmbyLog::levelToString(level)  << ": "   << s << std::endl;
}

void StdLogProcessor::waitLogEnd()
{

}