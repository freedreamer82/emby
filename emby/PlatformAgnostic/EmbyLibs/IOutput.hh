/**
 * @addtogroup EmbyLibs
 * @{
 * @file EmbyLibs/IOutput.hh
 * @date 2015-09-01
 * @author Massimiliano Pagani
 */

#if !defined( EMBYCONSOLE_IOUTPUT_HH )
#define EMBYCONSOLE_IOUTPUT_HH

#include <cstdlib>

namespace EmbyLibs
{
    class IOutput
	{
        public:
            virtual ~IOutput() = default;
            /**
             * This method has to be implemented so that the buffer content be
             * transmitted.
             *
             * Not fail is expected. Failing transmission may go unnoticed and
             * has to be assessed by other means.
             */
            virtual void transmit( char const* buffer, size_t length ) = 0;
        private:
	};
}

#endif

/**
 * @}
 */
