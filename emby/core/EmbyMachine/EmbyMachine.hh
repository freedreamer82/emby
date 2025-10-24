

#if !defined( EMBY_EMBYMACHINE_HH )
#define EMBY_EMBYMACHINE_HH

#include <cstdlib>
#include <EmbyDebug/assert.hh>
#include <limits>



namespace EmbyMachine
{

    /*Must be implemented for you machine!*/

    /**
     * Abort the program execution... On an embedded system this causes a warm
     * reset.
     * */
    void
    abort();

    /**
     * Init of the Machine...depends on...the machine :)
     * */
    void
    init();


}

//#include <EmbyMachine/Machine.hh>

#endif



