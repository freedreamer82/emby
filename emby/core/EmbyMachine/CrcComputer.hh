#if !defined( EMBYMACHINE_CRC_COMPUTER_HH )
#define EMBYMACHINE_CRC_COMPUTER_HH

#include <cstdint>

namespace EmbyMachine
{
    namespace CrcComputer
    {
        /**
         * Compute the Crc for a given set defined via iterators. Note that
         * poly and seed parameters define the size of the CRC you are going
         * to compute.
         *
         * @param poly the polynom for computing the CRC
         * @param seed the initial seed for CRC computation.
         * @param begin the iterator to the first item to compute CRC of.
         * @param end the iterator to one past the last item of the range on
         *            which you want to compute the CRC.
         *
         * @return the computed CRC,
         */

        uint16_t
        crc16compute( uint16_t poly,
                      uint16_t seed,
                      uint8_t const* data,
                      uint32_t length );

    }
}

#endif

///@}
