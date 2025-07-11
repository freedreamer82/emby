
#ifndef EMBYLIBS_CIRCULARBUFFER_HH
#define EMBYLIBS_CIRCULARBUFFER_HH

#include <stdexcept>
#include <EmbyDebug/assert.hh>
#include <iterator>

// Enable this macro if you want to disable critical section in puclic method of Circular Buffer.
// #define EMBY_CFG_DISABLE_CIRCULAR_BUFFER_CRITICAL_SECTIONS

#if defined(EMBY_CFG_DISABLE_CIRCULAR_BUFFER_CRITICAL_SECTIONS)
#define SCOPED_CRITICAL_SECTION()
#else
#define SCOPED_CRITICAL_SECTION() if (!EmbySystem::isInInterrupt()) EmbySystem::CriticalSection cs
#endif

namespace EmbyLibs
{

    
    /*Forward declaration for template*/
    template <typename T, int N>
    class CircularBufferIterator;

    typedef uint32_t CircularBufferCounterT;

    /**
     * Fixed size queue component.
     *
     * This template implements a fixed size circular buffer which sports a
     * FIFO access for generic items.
     */
    template <typename T, int N>
    class CircularBuffer
    {
    public:
        /*implementation is below*/
        typedef const CircularBufferIterator<T, N> const_iterator;



        /**
         * Constructs an empty buffer.
         */
        CircularBuffer()
                : m_first(0)
                , m_last(0)
                , m_size(0)
                , m_policy(FullBufferPolicy::Discard)
        {
        }

        friend CircularBufferIterator<T, N>;
        /**
         * Standard copy constructor.
         *
         * @param orig the buffer to copy from.
         */

        CircularBuffer(CircularBuffer<T, N> const& copy)
                : m_first(copy.m_first)
                , m_last(copy.m_last)
                , m_size(copy.m_size)
                , m_policy(copy.m_policy)
        {
            int scan = m_first;
            for (int i = 0; i != m_size; ++i)
            {
                auto destPtr = itemPtr(scan);
                auto srcPtr  = copy.itemPtr(scan);
                *destPtr     = *srcPtr;
                scan         = nextIndex(scan);
            }
            checkInvariant();
        }

        /**
         * Assignment operator.
         *
         * @param rhs the buffer assigned.
         * @return a reference to *this.
         */
        CircularBuffer& operator=(CircularBuffer const& rhs)
        {
            SCOPED_CRITICAL_SECTION();

            checkInvariant();
            if (this != &rhs)
            {
                CircularBuffer<T, N> tmp = rhs; // copy constructor
                swap(tmp);
            }
            checkInvariant();
            return *this;
        }

        // iterators
        const_iterator cbegin() const
        {
            SCOPED_CRITICAL_SECTION();
            checkInvariant();
            return CircularBufferIterator<T, N>::begin(*this);
        }

        const_iterator end() const
        {
            SCOPED_CRITICAL_SECTION();
            checkInvariant();
            return CircularBufferIterator<T, N>::end(*this);
        }
        const_iterator begin() const
        {
            SCOPED_CRITICAL_SECTION();
            checkInvariant();
            return CircularBufferIterator<T, N>::begin(*this);
        }

        const_iterator cend() const
        {
            SCOPED_CRITICAL_SECTION();
            checkInvariant();
            return CircularBufferIterator<T, N>::end(*this);
        }



        /**
         * Swaps the content of two buffers.
         *
         * @note complexity is O(N).
         *
         * @param other the other object.
         */
        void swap(CircularBuffer<T, N>& other)
        {
            SCOPED_CRITICAL_SECTION();

            checkInvariant();
            std::swap(m_first, other.m_first);
            std::swap(m_last, other.m_last);
            std::swap(m_size, other.m_size);
            std::swap(m_policy, other.m_policy);
            std::swap(m_memory, other.m_memory);
            checkInvariant();
        }

        /**
         * Inserts a new object in the queue.
         *
         * @param object the object to insert in the queue.
         */
        bool push(T const& object)
        {
            SCOPED_CRITICAL_SECTION();

            checkInvariant();
            if (full() && !handleFullBuffer())
            {
                return false;
            }

            if (m_size != N)
            {
                auto ptr = itemPtr(m_last);
                new (ptr) T(object);
                m_last = nextIndex(m_last);
                m_size += 1;
            }
            return true;
        }

        /**
         * Retrieves the frontmost element in the buffer.
         *
         * @return true if buffer non empty.
         *
         * @note it is an error to attempt to extract an item from an
         *       empty container.
         */
        bool front(T& data) const
        {
            SCOPED_CRITICAL_SECTION();
            checkInvariant();
            if (empty())
            {
                return false;
            }
            T const* item = itemPtr(m_first);
            data          = *item;
            return true;
        }

        /**
         * Retrieves the last element in the buffer.
         *
         * @return true if buffer non empty.
         *
         * @note it is an error to attempt to extract an item from an
         *       empty container.
         */
        bool back(T& data)
        {
            SCOPED_CRITICAL_SECTION();
            checkInvariant();
            if (empty())
            {
                return false;
            }
            T const* item = itemPtr(prevIndex(m_last));
            data          = *item;
            return true;
        }

        /**
         * Removes the frontmost element from the buffer.
         */
        bool pop()
        {
            SCOPED_CRITICAL_SECTION();
            checkInvariant();
            if (empty())
            {
                return false;
            }
            auto ptr = itemPtr(m_first);
            ptr->~T();
            m_first = nextIndex(m_first);
            m_size -= 1;
            checkInvariant();
            return true;
        }

        /**
         * Removes the frontmost element from the buffer and returns it via
         * a move semantic.
         *
         * @return the removed element.
         */
        T popMove()
        {
            SCOPED_CRITICAL_SECTION();
            checkInvariant();
            EmbyDebug_ASSERT(!empty());
            T* ptr = itemPtr(m_first);
            T  item(std::move(*ptr));
            ptr->~T();
            m_first = nextIndex(m_first);
            m_size -= 1;
            checkInvariant();
            return item; // this is a move
        }

        /**
         * Check if the buffer is full.
         *
         * @retval true the buffer is full.
         * @retval false the buffer is not full.
         */
        bool isFull() const
        {
            SCOPED_CRITICAL_SECTION();
            return full();
        }

        /**
         * Checks if the buffer is empty.
         *
         * @retval true the buffer is empty.
         * @retval false the buffer contains at least one element.
         */
        bool isEmpty() const
        {
            SCOPED_CRITICAL_SECTION();
            return empty();
        }

        /**
         * Returns the number of items in the buffer.
         *
         * @return how many items are in the buffer.
         */
        CircularBufferCounterT getSize() const
        {
            SCOPED_CRITICAL_SECTION();
            return size();
        }

        /**
         * Return the capacity of the buffer.
         *
         * @return the buffer capacity (i.e. how many items the buffer can
         *         hold).
         */
        CircularBufferCounterT getCapacity() const
        {
            SCOPED_CRITICAL_SECTION();
            return capacity();
        }

        /**
         * Empties the buffer.
         *
         * After a clear(), isEmpty() returns true.
         */
        void clear()
        {
            SCOPED_CRITICAL_SECTION();
            checkInvariant();
            int scan = m_first;
            for (int i = 0; i != m_size; ++i)
            {
                auto ptr = itemPtr(scan);
                ptr->~T();
                scan = nextIndex(scan);
            }
            m_first = m_last;
            m_size  = 0;
            checkInvariant();
        }

        /**
         * Selection for full buffer policy. This enumeration and the method
         * #setFullBufferPolicy() determine what happens when you push a new
         * item in a full buffer.Default is FBP_DISCARD
         */
        enum class FullBufferPolicy : uint8_t
        {
            Discard, ///@< silently discard the pushed item
            Assert,  ///@< causes an assertion.
            Pop      ///@< pop before pushing
        };

        /**
         * Sets a policy for full buffer.
         *
         * @param policy what the component does when you push a new item in
         *               an already full buffer.
         */
        void setFullBufferPolicy(FullBufferPolicy policy)
        {
            m_policy = policy;
        }



    private:
        uint8_t                m_memory[N * sizeof(T)];
        CircularBufferCounterT m_first;
        CircularBufferCounterT m_last;
        CircularBufferCounterT m_size;
        FullBufferPolicy       m_policy;

        bool empty() const
        {
            checkInvariant();
            return m_size == 0;
        }

        bool full() const
        {
            checkInvariant();
            return m_size == N;
        }

        CircularBufferCounterT capacity() const
        {
            checkInvariant();
            return N;
        }

        CircularBufferCounterT size() const
        {
            checkInvariant();
            return m_size;
        }

        T* itemPtr(int n)
        {
            return reinterpret_cast<T*>(m_memory) + n;
        }

        T const* itemPtr(int n) const

        {
            return reinterpret_cast<T const*>(m_memory) + n;
        }

        bool handleFullBuffer()
        {
            switch (m_policy)
            {
                case FullBufferPolicy::Assert:
                    EmbyDebug_ASSERT_FAIL();
                    break;

                case FullBufferPolicy::Pop:
                    pop();
                    return true;
                    break;

                case FullBufferPolicy::Discard:
                    return false;

                default:
                    EmbyDebug_ASSERT_FAIL();
                    break;
            }
            return false;
        }

        static int constexpr nextIndex(int i)
        {
            return ++i >= N ? 0 : i;
        }

        static int constexpr prevIndex(int i)
        {
            return --i < 0 ? N - 1 : i;
        }
        void checkInvariant() const
        {
            if (m_last == m_first)
            {
                EmbyDebug_ASSERT(m_size == 0 || m_size == N);
            }
            else if (m_last > m_first)
            {
                EmbyDebug_ASSERT(m_last - m_first == m_size);
            }
            else
            {
                EmbyDebug_ASSERT(N + m_last - m_first == m_size);
            }
        }
    };


    // :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    // :::::::::::::::::::::::::::::::::::::::::::::::::::::::::: iterator :::
    // :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    /*CircularBufferIterator const iterator*/
    template<typename T,int N>
    class CircularBufferIterator : public std::iterator<std::input_iterator_tag,T>
    {
    public:
        static CircularBufferIterator<T,N> begin( CircularBuffer<T,N> const& buffer ){
            return CircularBufferIterator<T,N>( buffer, buffer.m_first );
        }
        static CircularBufferIterator<T,N> end( CircularBuffer<T,N> const& buffer )
        {
            return CircularBufferIterator<T,N>(buffer, buffer.m_first+buffer.m_size );
        }

        T const & operator*() const;

        CircularBufferIterator<T,N>& operator++();

        bool operator==( CircularBufferIterator<T,N> const& obj) const;

        bool operator!=( CircularBufferIterator<T,N> const& obj) const;

    private:
        CircularBufferIterator( CircularBuffer<T,N> const& buffer, int index);

        const CircularBuffer<T,N>& m_buffer;
        int m_index;
    };



    template<typename T,int N> inline
    CircularBufferIterator<T,N>::CircularBufferIterator( const CircularBuffer<T,N> & buffer,
                                                         int index ) :
            m_buffer{buffer},
            m_index{ index }
    {
    }

    template<typename T,int N> inline T const &
    CircularBufferIterator<T,N>::operator*() const
    {
        int index = (m_index >= N) ? m_index-N : m_index;
        return *m_buffer.itemPtr( index );
    }

    template<typename T,int N> inline CircularBufferIterator<T,N>&
    CircularBufferIterator<T,N>::operator++()
    {
        if( m_index < m_buffer.m_first+m_buffer.m_size )
        {
            ++m_index;
        }
        return *this;
    }

    template<typename T,int N> inline bool
    CircularBufferIterator<T,N>::operator==( CircularBufferIterator<T,N> const& obj) const
    {
        return obj.m_index == m_index;
    }

    template<typename T,int N> inline bool
    CircularBufferIterator<T,N>::operator!=( CircularBufferIterator<T,N> const& obj) const
    {
        return  obj.m_index != m_index;
    }


} // namespace EmbyLibs

#endif /* EMBYLIBS_CIRCULARBUFFER_HH */
