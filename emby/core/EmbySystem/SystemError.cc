/*
 * SystemError.cc
 *
 *  Created on: 20/mar/2015
 *      Author: mgarzola
 */

#include <EmbySystem/SystemError.hh>
#include <EmbyThreading/ScopedMutex.hh>


namespace EmbySystem
{
    using EmbyThreading::ScopedMutex;

    SystemError::SystemError() : m_mutex(), m_buffer()
    {
    }

    bool SystemError::addError(ErrorCode &error)
    {
        bool retval = false;

        ScopedMutex mutex(m_mutex);
        auto status = SystemError::Status::NoError;
        if (!m_buffer.full())
        {
            auto isConsumed = false;
            if (m_cb.isAttached())
            {
                isConsumed = m_cb(&error, status);
            }

            if (!isConsumed)
            {
                //NOT consumed by CB!
                m_buffer.push_back(std::move(error));
            }

            retval = true;
        }
        else
        {
            status = SystemError::Status::BufferFull;
            if (m_cb.isAttached())
            {
                //buffer full not checking value...we cannot push in the buffer
                m_cb(&error, status);
            }
            retval = true;
        }
        return retval;
    }

    bool SystemError::setErrorCallback(SystemErrorCallback &cb)
    {
        bool retval = false;
        ScopedMutex mutex(m_mutex);
        m_cb = cb;
        return retval;
    }


    bool
    SystemError::clearAllErrors()
    {
        bool retval = false;
        ScopedMutex mutex(m_mutex);

        if (!m_buffer.empty())
        {
            m_buffer.clear();
            retval = true;
        }
        return retval;
    }

    ErrorCode const *
    SystemError::getLastError() const
    {
        ScopedMutex mutex(m_mutex);
        if (m_buffer.empty())
        {
            return nullptr;
        }
        else
        {
            return &m_buffer.back();
        }
    }

    unsigned
    SystemError::getErrorCount() const
    {
        ScopedMutex mutex(m_mutex);
        return m_buffer.size();
    }

    ErrorCode const *
    SystemError::getErrorAt(unsigned index) const
    {
        ScopedMutex mutex(m_mutex);
        if (index < m_buffer.size())
        {
            return &m_buffer[index];
        }
        else
        {
            return nullptr;
        }
    }

    EmbyLibs::Buffer <ErrorCode, EMBY_CFG_SYSTEM_ERROR_BUFFER_SIZE>
    SystemError::getErrorBufferCopy() const
    {
        ScopedMutex mutex(m_mutex);

        EmbyLibs::Buffer <ErrorCode, EMBY_CFG_SYSTEM_ERROR_BUFFER_SIZE> copy(m_buffer);
        return copy;
    }

}  // namespace EmbySystem
