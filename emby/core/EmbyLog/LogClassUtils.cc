
#include <EmbyLog/LogClassUtils.hh>
#include <EmbyLibs/fourcc.hh>

namespace EmbyLog
{
	EmbyLibs::String LogClass_toString( LogClass logClass )
	{
		EmbyLibs::String string;
		uint32_t fourCC = (uint32_t) logClass;
		string.push_back( ((fourCC >> 24) & 0xff));
		string.push_back( (fourCC >> 16) & 0xff );
		string.push_back( (fourCC >> 8) & 0xff);
		string.push_back(  fourCC & 0xff );
		return string;
	}


	uint32_t LogCLass_fromNameToNumberId(char* p)
	{
		 return (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
	}

}
