#include <EmbyExec/Message.hh>
#include <EmbyDebug/assert.hh>
#include <limits>
#include <algorithm>
#include <EmbyExec/MessageDictionary.hh>
#include <array>

namespace EmbyExec
{
    namespace
    {
        static constexpr unsigned TABLE_SIZE =
                std::numeric_limits<Message::Type>::max() +1u - Message::TYPE_EXTENDED_BASE;

        Message::ExtendedFunctions const*
        g_extededFunctions[ TABLE_SIZE ];

        inline bool isTypeExtended( Message::Type type )
        {
            return (type & Message::TYPE_EXTENDED_BASE) == Message::TYPE_EXTENDED_BASE;

        }
    }

    inline bool Message::isExtended() const
    {
        return isTypeExtended( getType() );
    }

    inline void Message::destroy()
    {
        if( isExtended() && getPointer<void*>() != nullptr &&
        	g_extededFunctions[ getType() - TYPE_EXTENDED_BASE ]->destructor != nullptr )
        {

            g_extededFunctions[ getType() - TYPE_EXTENDED_BASE ]->destructor( this );
        }
    }

    Message::~Message()
    {
        destroy();
    }

    Message::Message( Message&& other )
    {
        std::copy( std::begin( other.m_data ),
                   std::end( other.m_data ),
                   std::begin( m_data ) );
        if( isExtended() )
        {
            other.setPointer( static_cast<void*>( nullptr ));
        }
    }

    /*
    Message::Message( uint8_t const data[] ) :
    {
    }
*/

    Message&
    Message::operator=( Message&& other )
    {
        destroy();
        std::copy( std::begin( other.m_data ),
                   std::end( other.m_data ),
                   std::begin( m_data ) );
        if( isExtended() )
        {
            other.setPointer( static_cast<void*>(nullptr) );
        }
        return *this;
    }

    Message
    Message::clone() const
    {
        if( isExtended() &&
        	g_extededFunctions[ getType() - TYPE_EXTENDED_BASE ]->clone != nullptr	)
        {
            return g_extededFunctions[ getType() - TYPE_EXTENDED_BASE ]->clone( *this );
        }
        else
        {
            Message copy( getType() );
            std::copy( std::begin( m_data ),
                       std::end( m_data ),
                       std::begin( copy.m_data ) );
            return copy;
        }
    }
    
    void
    Message::registerExtendedFunctions( Type type, ExtendedFunctions const* functions )
    {
        EmbyDebug_ASSERT( isTypeExtended( type ));
        auto index = type - TYPE_EXTENDED_BASE;
        g_extededFunctions[ index ] = functions;
    }
}
