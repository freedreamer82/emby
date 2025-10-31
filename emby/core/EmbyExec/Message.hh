
#if !defined( EMBYEXEC_MESSAGE_HH )
#define EMBYEXEC_MESSAGE_HH

#include <cstdint>
#include <array>

namespace EmbyExec
{

    /** 
     * Base message class.
     *
     * Messages have a type that cannot be changed during the message life
     * cycle. Also the message is designed to be created and not changed until
     * its destruction.
     *
     * In order to avoid to deal with complex memory management, a Message can
     * be only moved rather than copied. The producer moves the message into
     * the scheduler, the scheduler shows the message to all recipients (via
     * const reference) and eventually the message is destroyed.
     *
     * If a client needs to keep the message data, the client has to perform
     * his own copy. The message provides a way to clone itself,
     *
     * Messages come in two flavors - fixed size and extended. If your data
     * fits in 7 bytes then you may use the fixed size. Otherwise you have to
     * undergo the pain of extended messages.
     * An extended message is basically a pointer to a single copy of user data.
     * User must provide function to destroy and copy the extended data.
     *
     * Mind - NO COPY CONSTRUCTOR nor ASSIGNMENT OPERATOR are provided and
     * that's by design.
     *
     * For your convenience use the MessageBase template to inherit from.
     */

    class Message
    {
        public:
            /**
             * Type is used to identify message. Based on this value the
             * message is managed differently.
             */
            typedef uint8_t Type;



            /**
             * Builds a message of the given type.
             *
             * @param type the type of the message.
             */
            explicit Message( Type type );
            ~Message();

            /**
             * Move operations.
             * @{
             */
            Message( Message&& other );
            Message& operator=( Message&& other );
            /**
             * @}
             */

            /**
             * Returns the type of the message.
             *
             * @return message type.
             */
            Type getType() const;

            /**
             * Creates a duplicate of the message. Note that this is returned
             * via move, so the only instance is the returned value.
             */
            Message clone() const;

            /**
             * Upper range of message types is reserved for extended messages.
             *
             * This is the first extended message.
             */
            static Type constexpr TYPE_EXTENDED_BASE = 0b11100000;

            /*
             *
             * Invalid Message
             * **/
            static Type constexpr TYPE_INVALID  = 0xFF;

            /**
             * User supplied functions for extended messages. The user will
             * provide a table with two function pointers that define specific
             * extended message destructor and cloner.
             * @{
             */
            typedef void (DestructorFn)( Message* message );
            typedef Message (CloneFn)( Message const& message );
            struct ExtendedFunctions
            {
                DestructorFn* destructor;
                CloneFn* clone;
            };
            /**
             * @}
             */

            /**
             * Before using an extended message you need to register it via
             * this function call. In this way cloning and destroying
             * operations are properly performed.
             *
             * @param type the type of the exteneded message you are
             *             registering.
             * @param functions pointer to the custom function table.
             */
            static void
            registerExtendedFunctions( Type type,
                                       ExtendedFunctions const* functions );

            /**
             * Performs a downcast to determine whether the message is of the
             * given type or not.
             *
             * @param message the message to test.
             * @return a pointer to the message if it is of type T, otherwise
             *         nullptr.
             */
            template<typename T>
            static T const* cast( Message const& message );

            template<typename T>
            static T* cast( Message& message );

            template<typename T>
            T const* as() const;
        protected:

            enum class ByteIndex :uint8_t
			{
				INDEX_0 = 1,
				INDEX_1,
				INDEX_2,
				INDEX_3,
				INDEX_4,
				INDEX_5,
				INDEX_6
			};

            /**
             * Long (32bits) value accessors.
             * @note that in exteneded messages this space is used to store the
             *       extended data pointer, therefore it is not available.
             *
             * @{
             */
            uint32_t getLong() const;
            void setLong( uint32_t value );
            /**
             * @}
             */

            /**
             * Short (16 bits) value accessors.
             *
             * @{
             */
            uint16_t getShort() const;
            void setShort( uint16_t value );
            /**
             * @}
             */

            /**
             * Byte (8 bits) value accessors. Not that two byte shares the same
             * space used by the Short. They are named short-low and short-high.
             *
             * @{
             */
            uint8_t getByteAt(ByteIndex index) const;
            uint8_t getByte() const;
            void setByte( uint8_t value );
            void setByteAt( uint8_t value , ByteIndex index);

            uint8_t getShortLow() const;
            uint8_t getShortHigh() const;

            void setShortLow( uint8_t value );
            void setShortHigh( uint8_t value );

            void setShort1( uint16_t value );
            void setShort2( uint16_t value );
            uint16_t getShort1() const;
            uint16_t getShort2() const;

            /**
             * @}
             */

            /**
             * Extended data pointer accessors. This pointer is conveniently
             * cast into what you need. Do not abuse.
             *
             * @note the pointer shares the same memory space of the long value
             *       (see #getLong() and #setLong()). This means you cannot
             *       access the long value if you are using an extended
             *       message.
             *
             * @{
             */
            template<typename T> T*
            getPointer() const;

            template<typename T> void
            setPointer( T* pointer );
            /**
             * @}
             */
        private:
            static constexpr unsigned DATASIZE = 2*sizeof( uint32_t* );
            Message( Message const& );
            Message& operator=( Message const& );
            Message( uint8_t const data[DATASIZE] );

            bool isExtended() const;
            void destroy();
            char* getDataRef();
            char const* getDataRef() const;


      //      uint8_t m_data[DATASIZE] __attribute__((__may_alias__));
            std::array<uint8_t,DATASIZE> 	m_data;

    };

    template<Message::Type T>
    class MessageClassifier
    {
        public:
             static inline bool isClassOf( Message const& message )
             {
                 return message.getType() == T;
             }
    };

    template<Message::Type T>
    class MessageBase : public Message,
                        public MessageClassifier<T>
    {
        public:
             MessageBase() : Message( T ) {}
    };

    // :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    // :::::::::::::::::::::::::::::::::::::::::::::::::::: inline methods :::
    // :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    inline char* Message::getDataRef()
    {
        return reinterpret_cast<char*>( m_data.data() );
    }

    inline char const* Message::getDataRef() const
    {
        return reinterpret_cast<char const*>(m_data.data());
    }

    inline Message::Message( uint8_t type ) : m_data{ type }
    {
    }

    inline uint8_t Message::getType() const
    {
        return m_data[0];
    }

    inline uint32_t Message::getLong() const
    {
        char const* data __attribute__((__may_alias__)) = getDataRef();
        return reinterpret_cast<uint32_t const*>(data)[1];
    }

    inline void Message::setLong( uint32_t value )
    {
        char* data __attribute__((__may_alias__)) = getDataRef();
        reinterpret_cast<uint32_t*>(data)[1] = value;
    }

    inline uint16_t Message::getShort() const
    {
        char const* data __attribute__((__may_alias__)) = getDataRef();
        return reinterpret_cast<uint16_t const*>(data)[1];
    }

    inline uint16_t Message::getShort1() const
    {
        char const* data __attribute__((__may_alias__)) = getDataRef();
        return reinterpret_cast<uint16_t const*>(data)[2];
    }

    inline uint16_t Message::getShort2() const
    {
        char const* data __attribute__((__may_alias__)) = getDataRef();
        return reinterpret_cast<uint16_t const*>(data)[3];
    }

    inline uint8_t Message::getShortLow() const
    {
        return m_data[2];
    }

    inline uint8_t Message::getShortHigh() const
    {
        return m_data[3];
    }

    inline void Message::setShort( uint16_t value )
    {
        char* data __attribute__((__may_alias__)) = getDataRef();
        reinterpret_cast<uint16_t*>(data)[1] = value;
    }

    inline void Message::setShort1( uint16_t value )
    {
        char* data __attribute__((__may_alias__)) = getDataRef();
        reinterpret_cast<uint16_t*>(data)[2] = value;
    }

    inline void Message::setShort2( uint16_t value )
    {
        char* data __attribute__((__may_alias__)) = getDataRef();
        reinterpret_cast<uint16_t*>(data)[3] = value;
    }

    inline void Message::setShortLow( uint8_t value )
    {
        m_data[2] = value;
    }

    inline void Message::setShortHigh( uint8_t value )
    {
        m_data[3] = value;
    }

    inline uint8_t Message::getByte() const
    {
        return m_data[1];
    }

    inline uint8_t Message::getByteAt( ByteIndex index ) const
    {
        return m_data[static_cast<uint8_t>(index)];
    }

    inline void Message::setByteAt( uint8_t value , ByteIndex index )
	{
	    m_data[static_cast<uint8_t>(index)] = value;
	}

    inline void Message::setByte( uint8_t value )
    {
        m_data[1] = value;
    }

    template<typename T> inline T*
    Message::getPointer() const
    {
        char const* data __attribute__((__may_alias__)) = getDataRef();
        return reinterpret_cast<T* const*>(data)[1];
    }

    template<typename T> inline void
    Message::setPointer( T* pointer )
    {
        char* data __attribute__((__may_alias__)) = getDataRef();
        reinterpret_cast<T**>(data)[1] = pointer;
    }

    template<typename T>
    T const* Message::cast( Message const& message )
    {
        return T::isClassOf( message ) ?
                    reinterpret_cast<T const*>( &message ) :
                    nullptr;
    }

    template<typename T>
    T* Message::cast( Message& message )
    {
        return T::isClassOf( message ) ?
                    reinterpret_cast<T*>( &message ) :
                    nullptr;
    }

    template<typename T>
    inline T const* Message::as() const
    {
        return cast<T>( *this );
    }



}  // end of namespace EmbyExec

#endif  // EMBYEXEC_MESSAGE_HH
///@}


