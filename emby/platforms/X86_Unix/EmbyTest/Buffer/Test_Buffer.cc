#include <EmbyLibs/Buffer.hh>
#include <cassert>
#include <iostream>

using namespace EmbyLibs;
extern void EmbyMachine::abort()
{
    std::cout << "ERROR: ASSERT BUT WE SHOULD NOT!!" << std::endl;
    exit(1);
}



int main()
{
    std::cout << "init.." << std::endl;


    int const          SIZE = 3;
    Buffer<char, SIZE> b;

    std::cout << "starting test" << std::endl;

    bool               ok = false;

    assert(b.empty());
    assert(b.size() == 0);
    assert(b.capacity() == SIZE);
    assert(b.max_size() == SIZE);

    bool isOutOfRange = false;

    //this must be fail with assert...accesising 0 that is not available
//    auto value = b.at(0);

    // clearing an empty container
    b.clear();
    assert(b.empty());
    assert(b.size() == 0);
    assert(b.capacity() == SIZE);
    assert(b.max_size() == SIZE);

    b.push_back('M');
    assert(!b.empty());
    assert(b.size() == 1);
    assert(b.capacity() == SIZE);
    assert(b.max_size() == SIZE);
    assert(b.at(0) == 'M');
    assert(b[0] == 'M');

    b[0] = 'J';
    assert(b[0] == 'J');
    assert(b.at(0) == 'J');
    assert(b.front() == 'J');
    assert(b.back() == 'J');
    assert(b.size() == 1);
    assert(b.capacity() == SIZE);
    assert(b.max_size() == SIZE);

    b.push_back('M');
    assert(!b.empty());
    assert(b.size() == 2);
    assert(b.capacity() == SIZE);
    assert(b.max_size() == SIZE);
    assert(b[0] == 'J');
    assert(b[1] == 'M');
    assert(b.at(1) == 'M');
    assert(b.front() == 'J');
    assert(b.back() == 'M');

    // clearing a container with 2 elements
    b.clear();
    assert(b.empty());

    b.push_back('J');
    b.push_back('M');

    b.push_back('A');
    assert(b[2] == 'A');
    assert(b[1] == 'M');
    assert(b[0] == 'J');
    assert(b.front() == 'J');
    assert(b.back() == 'A');
    assert(b.size() == 3);
    assert(!b.empty());


    ok = b.push_back('X');
    assert(!ok);
    assert(b.full());

    assert(b[2] == 'A');
    assert(b[1] == 'M');
    assert(b[0] == 'J');
    assert(b.front() == 'J');
    assert(b.back() == 'A');
    assert(b.size() == 3);
    assert(!b.empty());

    b.clear();
    assert(b.empty());

    std::cout <<  std::endl <<"TEST OK!!" << std::endl;

    return 0;
}

///@}
