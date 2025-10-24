/*
 * ErrorCode.hh
 *
 *  Created on: 20/mar/2015
 *      Author: mgarzola
 */

#ifndef ERRORCODE_HH_
#define ERRORCODE_HH_

#include <EmbyLibs/String.hh>

namespace EmbySystem
{
    class ErrorCode
    {
    public:
        typedef int ErrorTypeCode;
        static constexpr int ErrorTypeCode_NONE = -1;

        ErrorCode(ErrorTypeCode code,
                  EmbyTime::Millis time,
                  EmbyLibs::String description = "");

        ErrorCode();

        ErrorTypeCode getCode() const;

        EmbyLibs::String const &getDescription() const;

        EmbyTime::Millis getTime() const;

    private:
        EmbyLibs::String m_description;
        EmbyTime::Millis m_time;
        ErrorTypeCode m_code;
    };

// :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
// :::::::::::::::::::::::::::::::::::::::::::::::::::::::: inline methods :::
// :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    inline ErrorCode::ErrorTypeCode ErrorCode::getCode() const
    {
        return m_code;
    }

    inline EmbyLibs::String const &ErrorCode::getDescription() const
    {
        return m_description;
    }

    inline EmbyTime::Millis ErrorCode::getTime() const
    {
        return m_time;
    }

}

#endif /* ERRORCODE_HH_ */
