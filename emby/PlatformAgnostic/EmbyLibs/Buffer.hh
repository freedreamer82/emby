
#if !defined(EmbyUtils_BUFFER_HH)
#define EmbyUtils_BUFFER_HH

#include <stddef.h>
#include "EmbyDebug/assert.hh"
#include "EmbySystem/System.hh"
#include <algorithm>

// Enable this macro if you want to disable critical section in puclic method of Buffer.
// #define DISABLE_BUFFER_CRTICAL_SECTIONS

#if defined(EMBY_CFG_DISABLE_BUFFER_CRITICAL_SECTIONS)
#define SCOPED_CRITICAL_SECTION()
#else
#define SCOPED_CRITICAL_SECTION() EmbySystem::CriticalSection cs
#endif

namespace EmbyLibs
{
    /**
     * Buffer is a fixed size vector that can be filled progressively but never
     * extends.
     *
     * You may access elements in any order.
     *
     */

    template<typename T, int N>
    class Buffer
    {
    public:
        typedef T value_type;
        typedef T &reference;
        typedef const T &const_reference;
        typedef T *iterator;
        typedef const T *const_iterator;
        typedef std::reverse_iterator <iterator> reverse_iterator;
        typedef std::reverse_iterator <const_iterator> const_reverse_iterator;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;

        /**
         * Default constructor. Constructs an empty buffer.
         */
        Buffer()
                : m_firstFree(0)
        {
        }

        ~Buffer()
        {
            for (size_type i = 0; i < m_firstFree; ++i)
            {
                itemAt(i)->~T();
            }
        }

        /**
         * Copy constructor. This copies from a buffer with the same size
         * whose base type is copy constructable to T.
         *
         * @param copy the buffer to copy.
         */
        template<typename U>
        Buffer(Buffer<U, N> const &copy)
                : m_firstFree(copy.m_firstFree)
        {
            for (size_type i = 0; i < m_firstFree; ++i)
            {
                *itemAt(i) = *(copy.itemAt(i));
            }
        }

        Buffer(Buffer const &copy)
                : m_firstFree(copy.m_firstFree)
        {
            for (size_type i = 0; i < m_firstFree; ++i)
            {
                *itemAt(i) = *(copy.itemAt(i));
            }
        }

        Buffer(Buffer &&from)
                : m_firstFree{from.m_firstFree}
        {
            for (size_type i = 0; i < m_firstFree; ++i)
            {
                *itemAt(i) = std::move(*from.itemAt(i));
            }
            from.m_firstFree = 0;
        }

        /**
         * Constructs and initializes the buffer with the content of
         * another container.
         *
         * @tparam Iter an iterator. Dereferencing this iterator should
         *              yield type T.
         * @param begin iterator to container begin.
         * @param end iterator to container end.
         */
        template<typename Iter>
        Buffer(Iter begin, Iter end)
                : m_firstFree{0}
        {
            while (begin != end)
            {
                push_back(*begin);
                ++begin;
            }
        }

        /**
         * Constructs and initializes the buffer with the content of
         * another container.
         *
         * @tparam Iter an iterator. Dereferencing this iterator should
         *              yield type T.
         * @param begin iterator to container begin.
         * @param count the number of elements to copy.
         */
        template<typename Iter>
        Buffer(Iter begin, size_t count)
                : m_firstFree{0}
        {
            for (size_t i = 0; i != count; ++i)
            {
                push_back(*begin);
                ++begin;
            }
        }

        /**
         * Assignment operator.
         * @note passing the argument by copy it is not an error.
         */
        Buffer &operator=(Buffer copy)
        {
            swap(copy);
            return *this;
        }

        /**
         * Swaps the content of this buffer with the content of another
         * one.
         *
         * @param other the other buffer.
         * @exception none
         */
        void swap(Buffer &other) throw()
        {
            SCOPED_CRITICAL_SECTION();
            std::swap(m_firstFree, other.m_firstFree);
            std::swap(m_store, other.m_store);

        }

        // iterators
        iterator begin()
        {
            SCOPED_CRITICAL_SECTION();
            iterator it = itemAt(0);

            return it;
        }

        const_iterator begin() const
        {
            SCOPED_CRITICAL_SECTION();
            const_iterator it = itemAt(0);

            return it;
        }

        const_iterator cbegin() const
        {
            SCOPED_CRITICAL_SECTION();
            const_iterator it = itemAt(0);

            return it;
        }

        iterator end()
        {
            SCOPED_CRITICAL_SECTION();
            iterator it = itemAt(m_firstFree);

            return it;
        }

        const_iterator end() const
        {
            SCOPED_CRITICAL_SECTION();
            const_iterator it = itemAt(m_firstFree);
            return it;
        }

        const_iterator cend() const
        {
            SCOPED_CRITICAL_SECTION();
            const_iterator it = itemAt(m_firstFree);
            return it;
        }

        reverse_iterator rbegin()
        {
            return reverse_iterator(end());
        }

        const_reverse_iterator rbegin() const
        {
            return reverse_iterator(end());
        }

        reverse_iterator rend()
        {
            return reverse_iterator(begin());
        }

        const_reverse_iterator rend() const
        {
            return reverse_iterator(begin());
        }

        size_type size() const
        {
            SCOPED_CRITICAL_SECTION();
            size_type size = m_firstFree;

            return size;
        }

        size_type max_size() const
        {
            return N;
        }

        size_type capacity() const
        {
            return N;
        }

        bool full() const
        {
            SCOPED_CRITICAL_SECTION();
            auto ok = m_firstFree == N;

            return ok;
        }

        bool empty() const
        {
            SCOPED_CRITICAL_SECTION();
            auto ok = m_firstFree == 0;
            return ok;
        }

        bool push_back(T const &item)
        {
            SCOPED_CRITICAL_SECTION();
            auto ok = false;
            if (m_firstFree >= N)
            {
                ok = false;
            }
            else
            {
                new(itemAt(m_firstFree)) T(item);
                ++m_firstFree;
                ok = true;
            }

            return ok;
        }

        bool pop_back()
        {
            SCOPED_CRITICAL_SECTION();
            bool retval = false;
            if (m_firstFree == 0)
            {
                retval = false;
            }
            else
            {
                itemAt(m_firstFree)->~T();
                --m_firstFree;
                retval = true;
            }

            return retval;
        }

        void clear()
        {
            SCOPED_CRITICAL_SECTION();
            for (size_t i = 0; i < m_firstFree; ++i)
            {
                itemAt(i)->~T();
            }
            m_firstFree = 0;

        }

        // element access:
        reference operator[](size_type n)
        {
            SCOPED_CRITICAL_SECTION();
            return *itemAt(n);

        }

        const_reference operator[](size_type n) const
        {
            SCOPED_CRITICAL_SECTION();
            return *itemAt(n);

        }

        reference front()
        {
            SCOPED_CRITICAL_SECTION();
            return *itemAt(0);
        }

        const_reference front() const
        {
            SCOPED_CRITICAL_SECTION();
            return *itemAt(0);
        }

        reference back()
        {
            SCOPED_CRITICAL_SECTION();
            return *itemAt(m_firstFree - 1);
        }

        const_reference back() const
        {
            SCOPED_CRITICAL_SECTION();
            const_reference elem = *itemAt(m_firstFree - 1);

            return elem;
        }

        const_reference at(size_type n) const
        {
            SCOPED_CRITICAL_SECTION();
            checkIndex(n);
            const_reference elem = *itemAt(n);

            return elem;
        }

        reference at(size_type n)
        {
            SCOPED_CRITICAL_SECTION();
            checkIndex(n);
            reference elem = *itemAt(n);

            return elem;
        }

        T *data()
        {
            SCOPED_CRITICAL_SECTION();
            T *elem = itemAt(0);

            return elem;
        }

        T const *data() const
        {
            SCOPED_CRITICAL_SECTION();
            T const *elem = itemAt(0);

            return elem;
        }

    private:
        uint8_t m_store[sizeof(T[N])];
        size_type m_firstFree;

        T const *itemAt(size_type index) const
        {
            T const *value = reinterpret_cast<T const *>(m_store) + index;
            return value;
        }

        T *itemAt(size_type index)
        {
            T *value = reinterpret_cast<T *>(m_store) + index;
            return value;
        }

        void checkIndex(size_type index)
        {
            if (index >= m_firstFree)
            {
                /// @todo get rid of std::exception offspring
                EmbyDebug_ASSERT_FAIL();
            }
        }

    };

} // namespace EmbyLibs
#endif
///@}
