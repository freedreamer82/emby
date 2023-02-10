#include <EmbySystem/System.hh>
#include <EmbyMachine/EmbyMachine.hh>


#if !defined( EMBYDEBUG_ASSERT_HH )
#define EMBYDEBUG_ASSERT_HH

#ifndef EmbyDebug_ASSSERT_FAIL_ACTION
    #define EmbyDebug_ASSSERT_FAIL_ACTION(F, L, T) \
        do                                         \
        {                                          \
            EmbyMachine::abort();                  \
        } while (false)
#endif

    #define EmbyDebug_ASSERT(X)                                        \
        do                                                             \
        {                                                              \
            if (!(X))                                                  \
            {                                                          \
                EmbyDebug_ASSSERT_FAIL_ACTION(__FILE__, __LINE__, #X); \
            }                                                          \
        } while (false)

    #define EmbyDebug_ASSERT_FAIL() EmbyDebug_ASSERT(false)

    #define EmbyDebug_ASSERT_CHECK_NULL_PTR(P) EmbyDebug_ASSERT((P) != nullptr)

    #define EmbyDebug_STATIC_ASSERT(X) static_assert((X))
#endif    /* EMBYDEBUG_ASSERT_HH */

