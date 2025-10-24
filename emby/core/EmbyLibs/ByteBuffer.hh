
#if !defined( EmbyUtils_BYTEBUFFER_HH )
#define EmbyUtils_BYTEBUFFER_HH

#include <EmbyLibs/Buffer.hh>

namespace EmbyLibs
{

    /**
     * This is a buffer of bytes.
     */

    template<size_t N>
    using ByteBuffer = Buffer<uint8_t, N>;

}  // end of namespace EmbyLibs

#endif
///@}


