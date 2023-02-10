

#if !defined( EMBYEXEC_MESSAGEDICTIONARY_HH )
#define EMBYEXEC_MESSAGEDICTIONARY_HH

#include <EmbyExec/Message.hh>

namespace EmbyExec
{
    static constexpr Message::Type MessageId_INVALID =           0;

    static constexpr Message::Type
    MessageId_FIRST_EXTENDED = Message::TYPE_EXTENDED_BASE;

}  // end of namespace EmbyExec

#endif
///@}


