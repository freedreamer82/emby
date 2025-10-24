
#if !defined(Emby_EmbyCCLIBS_STRINGPOOL_HH)
#define Emby_EmbyCCLIBS_STRINGPOOL_HH

#include <EmbyLibs/SimpleObjectPool.hh>
#include <EmbyLibs/Singleton.hh>
#include <cstddef>
#include <EmbyThreading/Mutex.hh>
#include <EmbyThreading/ScopedMutex.hh>
#include <EmbyDebug/assert.hh>

namespace EmbyLibs
{
    /**
     *
     * This class manages the string pools.
     *
     */

    template<class Char>
    class StringPoolBase : public Singleton<StringPoolBase<Char>>
    {
    public:
        StringPoolBase();

        ~StringPoolBase();

        typedef size_t size_type;
        typedef std::ptrdiff_t difference_type;
        typedef Char &reference;
        typedef Char const &const_reference;
        typedef Char *pointer;
        typedef Char const *const_pointer;

        static std::pair<Char *, size_type> allocate(size_type size);

        static void free(Char *memory, size_type capacity)

        noexcept;

        static size_type max_size();

        /**
         * Returns the number of bytes used in the pools.
         *
         * @return the number of the used bytes.
         */
        static size_type getUsedMem()

        noexcept;

        static size_type getMaxMem()

        noexcept;

#ifndef SHORT_STRING_LENGTH
#define SHORT_STRING_LENGTH  (2 * (sizeof(char *) + sizeof(size_t))) // 16
#endif

#ifndef  EMBY_CFG_STRING_MEDIUM_LENGTH
#define  EMBY_CFG_STRING_MEDIUM_LENGTH  64       //64
#endif

#ifndef  EMBY_CFG_STRING_LONG_LENGTH
#define  EMBY_CFG_STRING_LONG_LENGTH   (8 * SHORT_STRING_LENGTH)  //128
#endif

#ifndef EMBY_CFG_STRING_SHORT_POOL_SIZE
#define   EMBY_CFG_STRING_SHORT_POOL_SIZE  60
#endif

#ifndef EMBY_CFG_STRING_MEDIUM_POOL_SIZE
#define   EMBY_CFG_STRING_MEDIUM_POOL_SIZE  15
#endif

#ifndef EMBY_CFG_STRING_LONG_POOL_SIZE
#define   EMBY_CFG_STRING_LONG_POOL_SIZE  10
#endif

    private:
        StringPoolBase(StringPoolBase const &copy);

        StringPoolBase &operator=(StringPoolBase const &copy);

        std::pair<Char *, size_type> allocateImpl(size_type size);

        void freeImpl(Char *memory, size_type capacity)

        noexcept;

        size_type getUsedMemImpl();

        size_type getMaxMemImpl();

        EmbyThreading::Mutex m_mutex;


        typedef Char ShortString[SHORT_STRING_LENGTH];
        typedef Char MediumString[EMBY_CFG_STRING_MEDIUM_LENGTH];
        typedef Char LongString[EMBY_CFG_STRING_LONG_LENGTH];

        SimpleObjectPool<ShortString, EMBY_CFG_STRING_SHORT_POOL_SIZE, OOOP_RETURN_NULL>
                m_shortStrings;
        SimpleObjectPool<MediumString, EMBY_CFG_STRING_MEDIUM_POOL_SIZE, OOOP_RETURN_NULL>
                m_mediumStrings;
        SimpleObjectPool<LongString, EMBY_CFG_STRING_LONG_POOL_SIZE, OOOP_RETURN_NULL>
                m_longStrings;
    };

    // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    // ::::::::::::::::::::::::::::::::::::::::::::::::::: Implementation :::
    // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

#define NO_INLINE

    template<class Char>
    StringPoolBase<Char>::StringPoolBase()
    {
    }

    template<class Char>
    NO_INLINE StringPoolBase<Char>::~StringPoolBase()
    {
    }

    template<class Char>
    NO_INLINE std::pair<Char *, typename StringPoolBase<Char>::size_type>
    StringPoolBase<Char>::allocateImpl(size_type size)
    {
        EmbyThreading::ScopedMutex lock(m_mutex);
        Char *ptr = nullptr;
        size_type capacity;

        if (size <= SHORT_STRING_LENGTH)
        {
            ptr = reinterpret_cast<Char *>(m_shortStrings.allocate());
            capacity = SHORT_STRING_LENGTH;
        }
        else if (size <= EMBY_CFG_STRING_MEDIUM_LENGTH)
        {
            ptr = reinterpret_cast<Char *>(m_mediumStrings.allocate());
            capacity = EMBY_CFG_STRING_MEDIUM_LENGTH;
        }
        else if (size <= EMBY_CFG_STRING_LONG_LENGTH)
        {
            ptr = reinterpret_cast<Char *>(m_longStrings.allocate());
            capacity = EMBY_CFG_STRING_LONG_LENGTH;
        }

        if (ptr == nullptr)
        {
            ptr = nullptr; // just to breakpoint
            capacity = 0;
        }
        return std::make_pair(ptr, capacity);
    }

    template<class Char>
    inline void StringPoolBase<Char>::freeImpl(Char *memory, size_type capacity) noexcept
    {
        EmbyThreading::ScopedMutex lock(m_mutex);
        if (capacity <= SHORT_STRING_LENGTH)
        {
            m_shortStrings.free(reinterpret_cast<Char(*)[SHORT_STRING_LENGTH]>(memory));
        }
        else if (capacity <=  EMBY_CFG_STRING_MEDIUM_LENGTH)
        {
        m_mediumStrings.free(reinterpret_cast<Char(*)[ EMBY_CFG_STRING_MEDIUM_LENGTH]>(memory));
        }
        else if (capacity <=  EMBY_CFG_STRING_LONG_LENGTH)
        {
        m_longStrings.free(reinterpret_cast<Char(*)[ EMBY_CFG_STRING_LONG_LENGTH]>(memory));
        }
        else
        {
            EmbyDebug_ASSERT_FAIL();
        }
    }

template<class Char>
size_t StringPoolBase<Char>::getUsedMemImpl()
{
    EmbyThreading::ScopedMutex lock(m_mutex);
    return m_shortStrings.getUsedCount() * SHORT_STRING_LENGTH +
           m_mediumStrings.getUsedCount() * EMBY_CFG_STRING_MEDIUM_LENGTH +
           m_longStrings.getUsedCount() * EMBY_CFG_STRING_LONG_LENGTH;
}

template<class Char>
size_t StringPoolBase<Char>::getMaxMemImpl()
{
    EmbyThreading::ScopedMutex lock(m_mutex);
    return m_shortStrings.getCapacity() * SHORT_STRING_LENGTH +
           m_mediumStrings.getCapacity() * EMBY_CFG_STRING_MEDIUM_LENGTH +
           m_longStrings.getCapacity() * EMBY_CFG_STRING_LONG_LENGTH;
}


template<class Char>
inline std::pair<Char *, typename StringPoolBase<Char>::size_type>
StringPoolBase<Char>::allocate(size_type size)
{
    return Singleton<StringPoolBase<Char>>::get().allocateImpl(size);
}

template<class Char>
inline void StringPoolBase<Char>::free(Char *memory, size_type capacity)

noexcept
{
    Singleton<StringPoolBase < Char>>::get().freeImpl(memory, capacity);
}

template<class Char>
inline typename StringPoolBase<Char>::size_type StringPoolBase<Char>::max_size()
{
    return EMBY_CFG_STRING_LONG_LENGTH;
}

template<class Char>
inline auto StringPoolBase<Char>::getMaxMem()

noexcept -> size_type
{
    return Singleton<StringPoolBase < Char>>::get().getMaxMemImpl();
}

template<class Char>
inline auto StringPoolBase<Char>::getUsedMem()

noexcept -> size_type
{
    return Singleton<StringPoolBase < Char>>::get().getUsedMemImpl();
}

typedef StringPoolBase<char> StringPool;

} // end of namespace EmbyLibs

#undef NO_INLINE

#endif
///@}
