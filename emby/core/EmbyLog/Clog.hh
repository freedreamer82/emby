

#if !defined( EMBY_CLOG_HH )
#define EMBY_CLOG_HH


#include "stdint.h"
#include "stdbool.h"

#ifdef __cplusplus
extern "C"
{
#endif


   /**
	* The level describes the severity of the message to log.
	* @note the odd naming is because LOG_* are preprocessor symbol
	*       defined by system includes.
	*  The level could also be used as flags. LOG_FATAL | LOG_NONE
	*/
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
	typedef enum
	{
		None = 0,
		Fatal = 1,
		Error = 2,
		Warning = 3,
		Info = 4,
		Debug = 5,
		Trace = 6,
		Reserved = 7
	} Level;
#pragma GCC diagnostic pop

	extern bool Clog_hex2ascii(uint8_t const *hex, uint8_t *destString, uint8_t hexLen);
	extern void Clog( Level logClass, char const* fmt, ... );


    #define Clog_debug(...)      Clog(Debug,__VA_ARGS__)
    #define Clog_info(...)	     Clog(Info, __VA_ARGS__)
    #define Clog_warning(...)    Clog(Warning,__VA_ARGS__)
    #define Clog_error(...)      Clog(Error,__VA_ARGS__)
    #define Clog_fatal(...)      Clog(Fatal,__VA_ARGS__)
	#define Clog_trace(...)      Clog(Trace,__VA_ARGS__)


#ifdef __cplusplus
}
#endif

#endif
