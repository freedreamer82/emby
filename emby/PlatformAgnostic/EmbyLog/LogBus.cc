/**
 * @addtogroup EmbyLog
 *
 * @author Massimiliano Pagani
 * @version 1.0
 * @date 18/05/2007
 * @file EmbyLog/Log.cc
 *
 * @notes
 * @history
 *
 */

#include <EmbyLog/LogBus.hh>
#include <algorithm>
#include <EmbyDebug/assert.hh>
#include <EmbyLibs/BitMask.hh>
#include <EmbyThreading/ScopedMutex.hh>

using namespace EmbyLog;
using EmbyThreading::ScopedMutex;

LogBus::LogBus()
{
}

LogBus::~LogBus()
{
}

void
LogBus::log( EmbyLog::LogLevel level,
             EmbyLog::LogClass logClass,
             EmbyLibs::String const& message )
{

	if(EmbySystem::isInInterrupt())
	{
		//NO logs from isr allowed!
		EmbyDebug_ASSERT_FAIL();
	}
	else
	{
		ScopedMutex mutex( m_processorMutex );

		for( auto &processor : m_processors )
		{
			LogMask logMask = processor->getLevelMask( );
			if( logMask & logMaskOf( level ))
			{
				processor->log( level, logClass, message );
			}
		}
	}
}

void
LogBus::log( EmbyLog::LogLevel level,
             EmbyLog::LogClass logClass,
             EmbyLibs::String&& message )
{
    log( level, logClass, message );
}

void
LogBus::registerProcessor( LogProcessor* processor )
{
    EmbyDebug_ASSERT( processor != NULL );
    ScopedMutex mutex( m_processorMutex );

    m_processors.push_back( processor );
}

void
LogBus::unregisterProcessor( LogProcessor* processor )
{
    EmbyDebug_ASSERT( processor != NULL );
    ScopedMutex mutex( m_processorMutex );

    auto proc =  std::find(m_processors.begin(),m_processors.end(),processor);
    if(proc != m_processors.end() )
    {
    	m_processors.erase(proc);
    }
}

bool
LogBus::isRegistered( LogProcessor* processor )
{
    ScopedMutex mutex( m_processorMutex );
    return ! ( std::find( m_processors.begin(),m_processors.end(),
               processor ) == m_processors.end() );
}

void
LogBus::flush()
{
    for( auto processor : m_processors )
    {
        processor->flush();
    }
}
