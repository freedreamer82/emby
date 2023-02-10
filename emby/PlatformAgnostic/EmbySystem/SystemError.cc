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

    SystemError::SystemError() : m_mutex(), m_buffer(), m_handler(nullptr)
    {
    }

    bool
	SystemError:: addError(ErrorCode&& error , bool syncHandling )
	{
		bool retval = false;

		ScopedMutex mutex(m_mutex);
		if( !m_buffer.full() )
		{
			m_buffer.push_back( std::move( error ));
			if (syncHandling)
			{
				handleErrors();
			}
			retval = true;
		}
		return retval;
	}

	bool SystemError::setErrorHandler( SystemErrorHandler* handler )
	{
		bool retval = false;
		ScopedMutex mutex(m_mutex);
		if( handler != nullptr )
		{
			m_handler = handler ;
			retval = true;
		}
		return retval;
	}

	void SystemError::handleErrors()
	{
		ScopedMutex mutex(m_mutex);
		if(m_handler)
		{
			m_handler->doWork();
		}
	}


	bool
	SystemError::clearAllErrors()
	{
		bool retval = false;
		ScopedMutex mutex(m_mutex);

		if(!m_buffer.empty())
		{
			m_buffer.clear();
			retval = true;
		}
		return retval;
	}

	ErrorCode const*
	SystemError::getLastError() const
	{
		ScopedMutex mutex(m_mutex);
		if( m_buffer.empty() )
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

    ErrorCode const*
    SystemError::getErrorAt( unsigned index ) const
    {
        ScopedMutex mutex(m_mutex);
        if( index < m_buffer.size() )
        {
            return &m_buffer[index];
        }
        else
        {
            return nullptr;
        }
    }

    EmbyLibs::Buffer<ErrorCode,SystemError::ERROR_BUFFER_SIZE>
    SystemError::getErrorBufferCopy() const
    {
        ScopedMutex mutex(m_mutex);

        EmbyLibs::Buffer<ErrorCode,ERROR_BUFFER_SIZE> copy( m_buffer );
        return copy;
    }

}  // namespace EmbySystem
