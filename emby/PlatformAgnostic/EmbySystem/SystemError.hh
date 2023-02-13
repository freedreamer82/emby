/*
 * SystemError.hh
 *
 *  Created on: 20/mar/2015
 *      Author: mgarzola
 */

#ifndef SYSTEMERROR_HH_
#define SYSTEMERROR_HH_

#include <stdint.h>
#include <EmbyLibs/Singleton.hh>
#include <EmbyLibs/Buffer.hh>
#include <EmbySystem/ErrorCode.hh>
#include <EmbyLibs/Callback.hh>
#include <EmbyThreading/Mutex.hh>


#ifndef EMBY_CFG_SYSTEM_ERROR_BUFFER_SIZE
#define   EMBY_CFG_SYSTEM_ERROR_BUFFER_SIZE  5
#endif

namespace EmbySystem
{
    class SystemError : public EmbyLibs::Singleton<SystemError>
    {
    public:

        enum class Status : uint8_t
        {
            NoError,
            BufferFull
        };

        //Return true if Error Consumed otherwise false and will be pushed in the buffer
        typedef EmbyLibs::Callback<bool, ErrorCode *, Status> SystemErrorCallback;

        SystemError();

        bool addError(ErrorCode &error);

        bool clearAllErrors();

        ErrorCode const *getLastError() const;

        unsigned getErrorCount() const;

        ErrorCode const *getErrorAt(unsigned index) const;

        EmbyLibs::Buffer<ErrorCode, EMBY_CFG_SYSTEM_ERROR_BUFFER_SIZE>
        getErrorBufferCopy() const;

        void handleErrors();


        bool setErrorCallback(SystemErrorCallback &cb);

    protected:
        SystemError(SystemError const &);

        SystemError(SystemError &&);

        SystemError &operator=(SystemError const &);

        SystemError &operator=(SystemError &&);


        EmbyThreading::Mutex mutable m_mutex;
        EmbyLibs::Buffer<ErrorCode, EMBY_CFG_SYSTEM_ERROR_BUFFER_SIZE> m_buffer;
        SystemErrorCallback m_cb;
    };
}


#endif /* SYSTEMERROR_HH_ */
