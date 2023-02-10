/*
 * SystemError.hh
 *
 *  Created on: 20/mar/2015
 *      Author: mgarzola
 */

#ifndef SYSTEMERROR_HH_
#define SYSTEMERROR_HH_

#include <EmbyLibs/Singleton.hh>
#include <EmbyLibs/Buffer.hh>
#include <EmbySystem/ErrorCode.hh>
#include <EmbyThreading/Mutex.hh>
#include <EmbySystem/SystemErrorHandler.hh>

namespace EmbySystem
{
    class SystemError : public EmbyLibs::Singleton<SystemError>
    {
    public:

        static uint16_t constexpr
        ERROR_BUFFER_SIZE = 20;

        SystemError();

        bool addError(ErrorCode &&error, bool syncHandling = false);

        bool clearAllErrors();

        ErrorCode const *getLastError() const;

        unsigned getErrorCount() const;

        ErrorCode const *getErrorAt(unsigned index) const;

        EmbyLibs::Buffer <ErrorCode, ERROR_BUFFER_SIZE>
        getErrorBufferCopy() const;

        void handleErrors();


        bool setErrorHandler(SystemErrorHandler *handler);

    protected:
        SystemError(SystemError const &);

        SystemError(SystemError &&);

        SystemError &operator=(SystemError const &);

        SystemError &operator=(SystemError &&);


        EmbyThreading::Mutex mutable m_mutex;
        EmbyLibs::Buffer <ErrorCode, ERROR_BUFFER_SIZE> m_buffer;
        SystemErrorHandler *m_handler;
    };
}


#endif /* SYSTEMERROR_HH_ */
