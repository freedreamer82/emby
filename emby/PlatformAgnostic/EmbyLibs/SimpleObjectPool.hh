
#ifndef EMBYLIBS_SIMPLEOBJECTPOOL_HH
#define EMBYLIBS_SIMPLEOBJECTPOOL_HH

#include "EmbyDebug/assert.hh"
#include <algorithm>
#include <new>
#include <memory>

namespace EmbyLibs
{
    enum OutOfObjectsPolicy
    {
        OOOP_RETURN_NULL,
        OOOP_FAIL_ASSERT,
    };

    template <typename T, int N, OutOfObjectsPolicy P>
    class SimpleObjectPool
    {
    public:
        SimpleObjectPool()
                : m_firstFree(m_pool)
        {
            for (int i = 0; i < N - 1; ++i)
            {
                m_pool[i].nextFree = m_pool + i + 1;
            }
            m_pool[N - 1].nextFree = nullptr;
        }

        SimpleObjectPool(SimpleObjectPool const&) = delete;
        SimpleObjectPool& operator=(SimpleObjectPool const&) = delete;

        ~SimpleObjectPool()
        {

        }

        size_t getUsedCount() const
        {
            size_t           freeCount = 0;
            PoolEntry const* scan      = m_firstFree;
            while (scan != nullptr)
            {
                freeCount += 1;
                scan = scan->nextFree;
            }
            return N - freeCount;
        }

        size_t getCapacity() const
        {
            return N;
        }

        T* allocate()
        {
            auto candidate = m_firstFree;
            if (candidate == nullptr)
            {
                // out of memory
                return outOfObjects();
            }
            m_firstFree = candidate->nextFree;
            return &candidate->object;
        }

        void free(T* object)
        {
            if (object == nullptr)
            {
                return;
            }
            auto entry      = reinterpret_cast<PoolEntry*>(object);
            entry->nextFree = m_firstFree;
            m_firstFree     = entry;
        }

    private:
        union PoolEntry {
            PoolEntry* nextFree;
            T          object;
        };
        PoolEntry  m_pool[N];
        PoolEntry* m_firstFree;

        T* outOfObjects()
        {
            switch (P)
            {
                case OOOP_FAIL_ASSERT:
                    EmbyDebug_ASSERT_FAIL();
                    break;

                case OOOP_RETURN_NULL:
                    return nullptr;

                default:
                    EmbyDebug_ASSERT_FAIL();
                    break;
            }
            // the following line should never be executed.
            return nullptr;
        }
    };
} // namespace EmbyCcLibs

#endif /* EMBYLIBS_SIMPLEOBJECTPOOL_HH */

///@}
