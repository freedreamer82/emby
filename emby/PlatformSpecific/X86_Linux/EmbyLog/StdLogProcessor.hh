//
// Created by marco on 01/02/23.
//

#ifndef DVC_EVO_STDLOGPROCESSOR_HH
#define DVC_EVO_STDLOGPROCESSOR_HH

#include <EmbyLog/LogMessage.hh>
#include <EmbyLog/LogProcessor.hh>
#include <EmbyLog/LogLevel.hh>

using namespace EmbyLog;

class StdLogProcessor : public LogProcessor
{
  public:
    StdLogProcessor(EmbyLog::LogMask levelMask) : LogProcessor(levelMask) {};

    virtual ~StdLogProcessor(){};

  protected:
  private:
    virtual void onLog(LogLevel level, LogClass logClass, EmbyLibs::String const& message);

    virtual void waitLogEnd();
};

#endif // DVC_EVO_STDLOGPROCESSOR_HH
