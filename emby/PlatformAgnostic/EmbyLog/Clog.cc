
#include <cstdarg>
#include "EmbyLog/LogClass.hh"
#include "EmbyLog/Log.hh"

EmbyLog_MODULE_LOG_CLASS("Clog")

extern "C"
{
	#include "Clog.hh"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
	void Clog( Level Level , char const* fmt, ... )
	{
#pragma GCC diagnostic pop
		va_list args;
		va_start( args, fmt );
		EmbyLog::logVPrintf( static_cast<EmbyLog::LogLevel>( Level ),
		                     EmbyLog::g_logClass,
		                     fmt,
		                     args );
		va_end( args );
	}

	bool
	Clog_hex2ascii(uint8_t const *hex, uint8_t *destString, uint8_t hexLen)
	{
		uint8_t chars[16] = { '0','1','2','3','4','5','6','7',
							  '8','9','A','B','C','D','E','F' };

		for (int i=0; i<hexLen; i++)
		{
			destString[ 1 + i*2 ] = chars[ hex[ i ] & 0x0F ];
			destString[ 0 + i*2 ] = chars[ hex[ i ] >> 4 ];
		}
		return true;
	}
}
