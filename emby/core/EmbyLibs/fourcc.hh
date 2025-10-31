
#if !defined( EmbyUtils_FOURCC_HH )
#define EmbyUtils_FOURCC_HH


#include <cstdint>

namespace EmbyLibs
{

    /**
     * constexpr function for creating a FourCC code.
     *
     */
    constexpr uint32_t fourcc( char const p[5] )
    {
        return (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
    }

}  // end of namespace EmbyLibs

#endif
///@}
