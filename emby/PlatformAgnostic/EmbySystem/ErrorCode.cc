/**
 * @addtogroup EmbySystem
 * @{
 * @file EmbySystem/ErrorCode.cc
 * @author Massimiliano Pagani
 * @date 20/mar/2015
 */

#include <EmbySystem/ErrorCode.hh>

namespace EmbySystem
{

    ErrorCode::ErrorCode( ErrorTypeCode code,
                          EmbyTime::Millis time,
                          EmbyLibs::String description ) :
               m_description(description),
               m_time(time),
               m_code(code)
    {
    }

    ErrorCode::ErrorCode( ):
               m_description(""),
               m_time(0),
               m_code(ErrorTypeCode_NONE)
    {
    }


}

///@}

