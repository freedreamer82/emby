/*
 * Flags.hh
 *
 *  Created on: 27/apr/2016
 *      Author: mgarzola
 */

#ifndef Flags_HH_
#define Flags_HH_

#include <EmbyThreading/Flags_Impl.hh>
#include <EmbyDebug/assert.hh>
#include <unistd.h>


namespace EmbyThreading
{
	class Flags
	{
		public:
			Flags( );
			~Flags( );

			uint32_t
			wait(uint32_t maskEventToWaitfor , int32_t timeoutMs = -1);

			/*Could also be used in ISR*/
			bool
			post( uint32_t eventMask );

//			uint32_t
//			getFlags()
//			{
//				return m_flags;
//			}

		private:
			uint32_t 	m_flags;
			FlagsImpl   m_impl;
	};
}


#endif /* Flags_HH_ */
