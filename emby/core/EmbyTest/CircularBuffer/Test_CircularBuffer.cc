#define CIRCULAR_BUFFER_ASSERT_INVARIANT

#include <EmbyLibs/CircularBuffer.hh>
#include <map>
#include <vector>
#include <iostream>
#include <cassert>
#include <utility>
#include <string>
#include <algorithm>

using EmbyLibs::CircularBuffer;

static const int SIZE_FOR_INT        = 5;
static const int SIZE_FOR_INSTRUMENT = 3;

#define ARRAY_SIZE(A) (sizeof(A) / sizeof(A[0]))

extern void EmbyMachine::abort()
{
        std::cout << "ERROR: ASSERT BUT WE SHOULD NOT!!" << std::endl;
        exit(1);
}


class InstrumentLogger
{
  public:
    enum Operation
    {
        CONSTRUCT,
        DESTROY,
        COPY_CONSTRUCT,
        MOVE_CONSTRUCT,
        ASSIGN,
        ASSIGN_MOVE
    };

    void log(int value, Operation operation)
    {
        auto ptr = m_logger.find(value);
        if (ptr == m_logger.end())
        {
            bool ok;

            std::tie(ptr, ok) = m_logger.insert(std::make_pair(value, Log()));
            assert(ok);
        }
        ptr->second.push_back(operation);
    }

    bool checkLog(int value, Operation* sequence, size_t sequenceLength)
    {
        auto log = m_logger[value];
#if 0
            for( auto i: log )
            {
                std::cout << i << " ";
            }
            std::cout << "\n";
#endif
        return sequenceLength == log.size() && std::equal(log.begin(), log.end(), sequence);
    }

    void clearLog(int value)
    {
        m_logger[value].clear();
    }

    void clearAll()
    {
        m_logger.clear();
    }

  private:
    typedef std::vector<Operation> Log;
    std::map<int, Log>             m_logger;
};

class Instrument
{
  public:
    explicit Instrument(int value, InstrumentLogger& logger)
        : m_value(value)
        , m_logger(logger)
    {
        m_logger.log(m_value, InstrumentLogger::CONSTRUCT);
    }


    ~Instrument()
    {
        m_logger.log(m_value, InstrumentLogger::DESTROY);
    }

    Instrument(Instrument const& copy)
        : m_value(copy.m_value)
        , m_logger(copy.m_logger)
    {
        m_logger.log(m_value, InstrumentLogger::COPY_CONSTRUCT);
    }

    Instrument(Instrument&& value)
        : m_value(value.m_value)
        , m_logger(value.m_logger)
    {
        m_logger.log(m_value, InstrumentLogger::MOVE_CONSTRUCT);
    }

    Instrument& operator=(Instrument const&)
    {
        m_logger.log(m_value, InstrumentLogger::ASSIGN);
        return *this;
    }

    Instrument& operator=(Instrument&& value)
    {
        m_value = value.m_value;
        m_logger.log(m_value, InstrumentLogger::ASSIGN_MOVE);
        return *this;
    }

    int getValue() const
    {
        return m_value;
    }

  private:
    int               m_value;
    InstrumentLogger& m_logger;
};

typedef EmbyLibs::CircularBuffer<int, SIZE_FOR_INT>               IntFifo;
typedef EmbyLibs::CircularBuffer<Instrument, SIZE_FOR_INSTRUMENT> InstrumentedFifo;

void clearTest(IntFifo& intFifo)
{
    assert(intFifo.getSize() == 0);
    assert(intFifo.getCapacity() == SIZE_FOR_INT);
    assert(intFifo.isEmpty());
    assert(!intFifo.isFull());

}

void testCopyCtor(IntFifo const& intFifo)
{
    assert(intFifo.getSize() != 0);
    IntFifo copy(intFifo);

    assert(intFifo.getSize() == copy.getSize());
    assert(intFifo.getCapacity() == copy.getCapacity());
    //    assert( std::equal( intFifo.begin(), intFifo.end(), copy.begin() ));
}

void testFullBufferPolicyWithAssert(IntFifo& intFifo)
{
    //    EmbyTest::Assertion::hasAsserted = false;
    //    try
    //    {
    //        intFifo.push( 42 );
    //        assert( false );
    //    }
    //    catch( EmbyTest::Assertion& )
    //    {
    //    }
    //    catch( ... )
    //    {
    //        assert( false );
    //    }
    //    assert( EmbyTest::Assertion::hasAsserted );
    //    EmbyTest::Assertion::hasAsserted = false;
}

void testFullBufferPolicyWithDiscard(IntFifo& intFifo)
{
    auto ok = intFifo.push(42);
    assert(ok);
}

void testFullBufferPolicyWithThrow(IntFifo& intFifo)
{
    bool hasThrown = false;

    auto ok = intFifo.push(42);

    assert(ok);
}

void testFullBufferPolicyWithPop(IntFifo& intFifo)
{
    auto prevSize = intFifo.getSize();
    assert(prevSize >= 2);
    assert(intFifo.isFull());

    bool ok = intFifo.push(23);
    assert(!ok);

    auto copy = intFifo;

      ok = intFifo.push(42);
    assert(!ok);
    int  back = 0;
      ok   = intFifo.back(back);
    assert(ok);
    intFifo.pop();
    ok = intFifo.push(42);
    assert(ok);
    ok   = intFifo.back(back);
    assert(ok);

    int front = 0;
    ok        = intFifo.front(front);
    assert(ok);

    assert(intFifo.getSize() == prevSize);

    copy.pop();

    int cback;
    ok = copy.back(back);
    assert(ok);

    int cfront;
    ok = copy.front(front);
    assert(ok);

//    assert(cfront == front);

    intFifo.clear();
    clearTest(intFifo);

    intFifo.push(42);
    intFifo.push(41);
    auto value = intFifo.popMove();
    assert(   value == 42 );
    assert(intFifo.getSize() == 1);
    assert(!intFifo.isEmpty());
    auto v2 = intFifo.popMove();
    assert(   v2 == 41 );
    assert(intFifo.getSize() == 0);
    assert(intFifo.isEmpty());

    intFifo.clear();
    clearTest(intFifo);

    for (int i = 0; i < SIZE_FOR_INT; ++i)
    {
        intFifo.push(i);
    }
    for (int i = 0; i < SIZE_FOR_INT; ++i)
    {
        auto t = intFifo.popMove();
    }

    ok = intFifo.pop();
    assert(!ok);
    assert(intFifo.isEmpty());
}

void fullTest(IntFifo& intFifo)
{
    assert(intFifo.isFull());
    assert(!intFifo.isEmpty());
    assert(intFifo.getSize() == SIZE_FOR_INT);
}

void intFifoTest()
{
    IntFifo intFifo;
    clearTest(intFifo);

    intFifo.clear();
    clearTest(intFifo);

    int  front,back,popped;
    bool ok = intFifo.push(11);
    assert(ok);
    assert(intFifo.getSize() == 1);
    assert(!intFifo.isEmpty());
    assert(!intFifo.isFull());


    ok = intFifo.front(front);
    ok = intFifo.back(back);
    popped = intFifo.popMove();
    assert(ok && (back == front) && (back == popped));
    assert(front == 11);

    intFifo.clear();
    clearTest(intFifo);

    intFifo.push(11);
    intFifo.pop();
    assert(intFifo.isEmpty());
    clearTest(intFifo);

    for (int i = 0; i < 13; ++i)
    {
        intFifo.push(i  );
        int value;
        ok = intFifo.front(value);
        assert(ok && value == i );
        intFifo.pop();
    }
    assert(intFifo.getSize() == 0);
     intFifo.clear();
     intFifo.clear();
    clearTest(intFifo);

    for (int i = 0; i < SIZE_FOR_INT; ++i)
    {
        std::cout << i << std::endl;
        intFifo.push(i);
    }
    for (auto i  : intFifo)
    {
        std::cout << "value:" << i << std::endl;
    }
    fullTest(intFifo);

    intFifo.front(front);
    assert(front == 0);

    testCopyCtor(intFifo);

    testFullBufferPolicyWithAssert(intFifo);

    for (int i = SIZE_FOR_INT; i < SIZE_FOR_INT + 13; ++i)
    {
        int value;
        intFifo.front(value);
        assert(value == i - SIZE_FOR_INT);
        intFifo.pop();
        intFifo.push(i);
        fullTest(intFifo);
    }

    testFullBufferPolicyWithPop(intFifo);

    intFifo.clear();
    clearTest(intFifo);
}

 void objectFifoTest()
{
     InstrumentLogger logger;
     InstrumentedFifo fifo;
     Instrument       value(1, logger);

     fifo.push(value);
     InstrumentLogger::Operation pushOp[] = {
         InstrumentLogger::CONSTRUCT,      // constructed
         InstrumentLogger::COPY_CONSTRUCT, // Fifo item copy
     };

     assert(logger.checkLog(1, pushOp, ARRAY_SIZE(pushOp)));
     logger.clearLog(1);

     InstrumentLogger::Operation popOp[] = {
         InstrumentLogger::DESTROY // Fifo item destruction
     };

     fifo.pop( );
     assert(logger.checkLog(1, popOp, ARRAY_SIZE(popOp)));

     logger.clearLog(1);

     fifo.push(std::move(value));
     Instrument newValue(2, logger);
     newValue = fifo.popMove( );

     assert(newValue.getValue() == 1);

     InstrumentLogger::Operation popMoveOp[] = {InstrumentLogger::MOVE_CONSTRUCT,
                                                InstrumentLogger::MOVE_CONSTRUCT,
                                                InstrumentLogger::DESTROY,
                                                InstrumentLogger::ASSIGN_MOVE,
                                                InstrumentLogger::DESTROY};
     assert(fifo.isEmpty());
     assert(logger.checkLog(1, popMoveOp, ARRAY_SIZE(popMoveOp)));
 }
//
// void iteratorCheck(CircularBuffer<int, 3> const& buffer, int low, int high)
//{
//     int n = low;
//     for (auto i = buffer.begin(); i != buffer.end(); ++i)
//     {
//         assert(*i == n);
//         ++n;
//     }
//     assert(n == high + 1);
// }
//
// void iteratorTest_base()
//{
//     CircularBuffer<int, 3> buffer;
//     buffer.push(1);
//     buffer.push(2);
//     iteratorCheck(buffer, 1, 2);
// }
//
// void iteratorTest_circular()
//{
//     CircularBuffer<int, 3> buffer;
//     buffer.push(1);
//     buffer.push(2);
//     buffer.push(3);
//
//     iteratorCheck(buffer, 1, 3);
//
//     buffer.pop();
//     buffer.push(4);
//
//     iteratorCheck(buffer, 2, 4);
//
//     buffer.pop();
//     buffer.push(5);
//
//     iteratorCheck(buffer, 3, 5);
//
//     buffer.pop();
//     buffer.push(6);
//
//     iteratorCheck(buffer, 4, 6);
//
//     buffer.pop();
//     iteratorCheck(buffer, 5, 6);
//
//     buffer.pop();
//     iteratorCheck(buffer, 6, 6);
//
//     buffer.pop();
//     assert(buffer.begin() == buffer.end());
// }
//
// void iteratorTest_isFull()
//{
//     std::string x =
//         "\r\n+WIND:21:WiFi Scanning\r\n\r\n+WIND:35:WiFi Scan Complete (0x0)\r\n\r\n+WIND:21:WiFi
//         " "Scanning\r\n\r\n+WIND:35:WiFi Scan Complete (0x0)\r\n\r\n+WIND:21:WiFi "
//         "Scanning\r\n\r\n+WIND:35:WiFi Scan Complete (0x0)\r\n\r\n+WIND:21:";
//     CircularBuffer<int, 0x100> buffer;
//
//     for (auto c : x)
//     {
//         buffer.push(c);
//     }
//
//     auto i = std::find(buffer.begin(), buffer.end(), '\n');
//     assert(i != buffer.end());
//     auto check = buffer.begin();
//     ++check;
//     assert(i == check);
//
//     auto n = std::count_if(buffer.begin(), buffer.end(), [](char c) { return c == '\n'; });
//     assert(n == 13);
//
//     int count = 0;
//     for (auto c : x)
//     {
//         if (c == '\n')
//         {
//             ++count;
//         }
//     }
//     assert(count == 13);
// }
//
// void iteratorTest_oddCase()
//{
//     CircularBuffer<uint8_t, 0x100> buffer;
// }

void crashBoomTest()
{
    int constexpr const SIZE = 16;
    EmbyLibs::CircularBuffer<int, SIZE> buffer;

    for (int i = 0; i < SIZE; ++i)
    {
        buffer.push(i);
    }
    for (int i = 0; i < 3 * SIZE; ++i)
    {
        buffer.pop();
        buffer.push(i);
    }

    buffer.clear();
    for (int i = 0; i < 3 * SIZE; ++i)
    {
        buffer.push(i);
        buffer.pop();
    }
}

int main()
{
    std::cout << "Starting test..." << std::endl;
    intFifoTest();
//    objectFifoTest();
    crashBoomTest();
    std::cout << "TEST OK" << std::endl;
    return 0;
}

///@}
