/**
 * @addtogroup EmbyMachine
 * @{
 * @file CrcComputer.cc
 * @author Emby Team
 * @version 1.0
 * @date 11/05/2015
 *
 */

#include <EmbyMachine/CrcComputer.hh>

namespace EmbyMachine
{
    namespace CrcComputer
    {

		uint16_t
		crc16compute( uint16_t poly,
					  uint16_t seed,
					  uint8_t const* data,
					  uint32_t length )
		{

			uint16_t crc = seed;
			while(length--)
			{
				crc = crc ^ ((uint16_t)(*data++) << 8);
				int i;
				for (i=0; i<8; i++)
				{
					if (crc & 0x8000)
						crc = (crc << 1) ^ poly;
					else
						crc <<= 1;
				};
			};
			return crc;
		}

    }
}  // end of namespace EmbyMachine
///@}


