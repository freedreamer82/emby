
#if !defined( EmbyUtils_CHARBUFFER_HH )
#define EmbyUtils_CHARBUFFER_HH

#include <EmbyLibs/Buffer.hh>

namespace EmbyLibs
{

    /**
     * This is a buffer of characters.
     * 
     *
     */

    template< size_t N >
        using CharBuffer = Buffer<char,N>;

}  // end of namespace EmbyLibs

#endif
///@}


