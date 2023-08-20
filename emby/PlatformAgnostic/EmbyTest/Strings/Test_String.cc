#/*
    g++ -Wall -Wextra -g -std=gnu++1y -DEMBY_BUILD_x86
   -I../PlatformSpecific/X86_Unix/EmbySystem/  -I../PlatformAgnostic/EmbySystem
   -I../PlatformSpecific/X86_Unix/EmbyThreading -I../PlatformAgnostic/EmbyCcLibs
   -I. -g -std=gnu++1y EmbyTest/CcLibs_String.cc  ../PlatformSpecific/X86_Unix/EmbyThreading/Mutex.cc
    ../PlatformAgnostic/EmbyCcLibs/String.cc
    ../PlatformSpecific/X86_Unix/EmbySystem/System.cc -o test_string
# */

#include <EmbyLibs/String.hh>
#include <EmbyLibs/StringUtils.hh>
#include <cassert>
#include <iostream>
#include <EmbyMachine/EmbyMachine.hh>

bool g_isInAssert = false;

bool g_shouldAssert = false;
extern void EmbyMachine::abort()
{
    g_isInAssert = true;
    std::cout << "ASSERT!!" << std::endl;
    if (g_shouldAssert)
    {
    }
    else
    {

        std::cout << "ERROR: ASSERT BUT WE SHOULD NOT!!" << std::endl;
        exit(1);
    }
}

using namespace EmbyLibs;
typedef StringPool::size_type size_type;

static std::pair<size_type, size_type> getPoolSizes()
{
    char *ptr;
    size_type shortSize;
    size_type longSize;

    std::tie(ptr, shortSize) = StringPool::allocate(1);
    StringPool::free(ptr, shortSize);

    std::tie(ptr, longSize) = StringPool::allocate(shortSize + 1);
    StringPool::free(ptr, longSize);

    return std::make_pair(shortSize, longSize);
}

void testStaticDynamicCtors(size_type shortSize, size_type longSize)
{
    char const test1[] = "hello";
    String compact(test1);
    assert(compact.size() == sizeof(test1) - 1);
    assert(compact.length() == compact.size());
    assert(std::equal(compact.begin(), compact.end(), test1));
    assert(StringPool::getUsedMem() == 0);

    size_type compactSize = compact.capacity();
    assert(compact.capacity() >= sizeof(test1) - 1);
    assert(compactSize < shortSize);

    char const test2[] = "0123456789012345";
    assert(sizeof(test2) - 1 == compactSize);
    String yetCompact(test2);
    assert(yetCompact.size() == sizeof(test2) - 1);
    assert(std::equal(yetCompact.begin(), yetCompact.end(), test2));
    assert(StringPool::getUsedMem() == 0);

    char const test3[] = "01234567890123456";
    assert(sizeof(test3) - 1 < shortSize);
    String shortString(test3);
    assert(shortString.size() == sizeof(test3) - 1);
    assert(std::equal(shortString.begin(), shortString.end(), test3));
    assert(StringPool::getUsedMem() == shortSize);
    assert(shortString.capacity() == shortSize);

    char const test4[] = "01234567890123456789012345678901";
    assert(sizeof(test4) - 1 == shortSize);
    String anotherShort(test4);
    assert(anotherShort.size() == sizeof(test4) - 1);
    assert(anotherShort.capacity() == shortSize);
    assert(std::equal(anotherShort.begin(), anotherShort.end(), test4));
    assert(StringPool::getUsedMem() == 2 * shortSize);

    char const test5[] = "012345678901234567890123456789012";
    assert(sizeof(test5) - 1 > shortSize);
    String longString(test5);
    assert(longString.size() == sizeof(test5) - 1);
    assert(longString.capacity() == longSize);
    assert(std::equal(longString.begin(), longString.end(), test5));
    assert(StringPool::getUsedMem() == 2 * shortSize + longSize);
}

void testFind()
{
#define COMMON_PREFIX "112231"
#define UNIQUE_TAIL "abc"

    String text(COMMON_PREFIX UNIQUE_TAIL);
    auto pos = text.find_first_not_of("123");
    assert(pos == sizeof(COMMON_PREFIX) - 1);

    pos = text.find_first_not_of(" ");
    assert(pos == 0);
    pos = text.find_first_not_of("123", 3);
    assert(pos == sizeof(COMMON_PREFIX) - 1);

    pos = text.find_first_not_of("123abc");
    assert(pos == String::npos);
#undef COMMON_PREFIX
#undef UNIQUE_TAIL

    String dummy = "abcde0100ghif1100";
    auto i = dummy.find_last_not_of("01");
    assert(i != String::npos);
    assert(dummy.at(i) == 'f');
    dummy = "0110101";
    i = dummy.find_last_not_of("01");
    assert(i == String::npos);
}

void testEqual()
{
    String a("abc");
    assert(a == a);
    String b("abcd");
    assert(!(a == b));
    String c("ab");
    assert(!(a == c));

    assert(a == "abc");
    assert(!(a == "abcd"));
    assert(!(a == "ab"));
}

void testSubstr()
{
#define FIRST_HALF "01234"
#define CENTER "56"
#define ENDING "789"

    String text(FIRST_HALF CENTER ENDING);
    auto all = text.substr();
    assert(text == all);
    auto firstHalf = text.substr(0, sizeof(FIRST_HALF) - 1);
    assert(firstHalf == FIRST_HALF);
    auto center = text.substr(sizeof(FIRST_HALF) - 1, sizeof(CENTER) - 1);
    assert(center == CENTER);
    auto center_end = text.substr(sizeof(FIRST_HALF) - 1, String::npos);
    assert(center_end == CENTER ENDING);
    auto end = text.substr(sizeof(FIRST_HALF) - 1 + sizeof(CENTER) - 1, sizeof(ENDING) - 1);
    assert(end == ENDING);
#undef ENDING
#undef CENTER
#undef FIRST_HALF
}

void testEmpty()
{
    String dummy;
    assert(dummy.empty());
    dummy = "adslkjsadlklwqjewqjlk";
    dummy.clear();
    assert(dummy.empty());
}

void testAccessors()
{
    String dummy = "0123456";
    for (size_t i = 0; i < dummy.size(); ++i)
    {
        assert(dummy.at(i) == static_cast<char>(i + '0'));
        assert(dummy[i] == static_cast<char>(i + '0'));
    }

    dummy.at(3) = 13;
    assert(dummy.at(3) == 13);
    dummy[4] = 14;
    assert(dummy[4] == 14);

    //    dummy.at(128);
    //    assert( false );

    // testing long string
    dummy = "abcdefghijklmnopqrstuvwxyz0123456789";
    assert(dummy.at(dummy.size() - 1) == '9');
}

void testErase()
{
#define FIRST "abc"
#define MIDDLE "defgh"
#define END "ijklm"

    size_t const FIRST_LEN = sizeof(FIRST) - 1;
    size_t const MIDDLE_LEN = sizeof(MIDDLE) - 1;
    size_t const END_LEN = sizeof(END) - 1;

    String dummy = FIRST MIDDLE END;
    auto &x = dummy.erase(size_t(0));
    assert(&x == &dummy);
    assert(dummy.empty());
    dummy = FIRST MIDDLE END;
    dummy.erase(0, FIRST_LEN);
    assert(dummy == MIDDLE END);
    dummy = FIRST MIDDLE END;
    dummy.erase(FIRST_LEN, MIDDLE_LEN);
    assert(dummy == FIRST END);
    dummy = FIRST MIDDLE END;
    dummy.erase(FIRST_LEN + MIDDLE_LEN, END_LEN);
    assert(dummy == FIRST MIDDLE);

    dummy = FIRST MIDDLE END;
    auto b = dummy.begin() + FIRST_LEN;
    auto e = b + MIDDLE_LEN;
    auto n = dummy.erase(b, e);
    assert(n == b);
    assert(dummy == FIRST END);

#undef END
#undef MIDDLE
#undef FIRST

    String foo = "012345";
    auto i = foo.erase(foo.begin() + 2);
    assert(*i == '3');
    assert(foo == "01345");
}
const char *TEST_258Len =
    "11123451234567123456789abc12345678absaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaadasdasdasfss3222222222222"
    "2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222"
    "222222222222222222222222fassssssc123456789abc123456789abc123456789abc1";
const char *TEST2_256Len =
    "123451234567123456789abc12345678absaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaadasdasdasfss322222222222222"
    "2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222"
    "2222222222222222222222fassssssc123456789abc123456789abc123456789abc1";
const char *TEST3_255Len =
    "23451234567123456789abc12345678absaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaadasdasdasfss3222222222222222"
    "2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222"
    "222222222222222222222fassssssc123456789abc123456789abc123456789abc1";

void testAppend()
{
    int v = 0;
    String dummy("a");
    dummy.append(dummy);
    assert(dummy == "aa");
    String empty;
    empty.append(empty);
    assert(empty.empty());

    String a = "abc";
    String b = "def";
    a.append(b);
    assert(a == "abcdef");

    // trying to break the barrier.
#define SIXTEEN "0123456789012345"
    a = SIXTEEN;
    b = a;
    a.append(b);
    assert(a == SIXTEEN SIXTEEN);
#undef SIXTEEN

    String c = "abc";
    String d;
    char const *buffer = "defghi";

    v = c.append(buffer, 4);
    assert(c == "abcdefg" && v == 0);

    v = d.append(buffer, 2);
    assert(d == "de" && v == 0);

    String dummylong(TEST3_255Len);
    v = dummylong.append("a");
    assert(v == 0);
    v = dummylong.append("a");
    assert(v < 0);
    auto cs = dummylong.c_str();
    assert(cs == nullptr);

    String dummy2long(TEST3_255Len);
    v = dummylong.append("abfbfbf");
    assert(v < 0);

    String dummy3long(TEST3_255Len);
    String ss("a");
    v = dummy3long.append(ss);
    assert(v == 0);
}

void testCstring()
{
    const char *TEST = "11111111111111121dasfSDFzsdfSDFasdfSDFsDfzsdfzs";
    String dummy(TEST);
    const char *c = dummy.c_str();
    int diff = strcmp(c, TEST);
    assert(diff == 0);
    assert(strlen(dummy.c_str()) == strlen(TEST));

    // 256 len that is MAX , when push a terminator if fails...it depends on the policy used default
    // is Nullptr
    String dummy2(TEST2_256Len);
    const char *c2 = dummy2.c_str();
    assert(c2 == nullptr);

    // 258 len
    String dummy3(TEST_258Len);
    // ha nullptr as buffer...every operation can lead to an assert
    dummy3.push_back('da');
    assert(dummy3.data() == nullptr);
}

void testCstringAssert()
{
    String::setBufferFullPolicy(String::FullStringBufferPolicy::Assert);

    assert(g_isInAssert == false);
    // must assert
    g_shouldAssert = true;
    std::cout << "This test should ASSERT!" << std::endl;
    String dummy3(TEST_258Len);
    assert(g_isInAssert == true);
    std::cout << "This test should ASSERT!" << std::endl;
    g_isInAssert = false;
    const char *c3 = dummy3.c_str();
    assert(c3 == nullptr);
    std::cout << "End Asssert Test" << std::endl;
    g_isInAssert = false;
    g_shouldAssert = false;
}

void testStringUtils()
{
    String a("abcdefg");
    String head("abcd");
    String tail("efg");
    String faketail("ffg");
    bool ok = false;
    ok = doesStringBeginWith(a, head);
    assert(ok);
    ok = doesStringEndWith(a, tail);
    assert(ok);
    ok = doesStringEndWith(a, faketail);
    assert(!ok);
}

void testCompare()
{
    String str1("green apple");
    String str2("red apple");
    String str3("apple");


    if (str1.compare(6, 5, String(("apple"))) == 0)
    {
        std::cout << str1.c_str() << " starting from 6 char and for next 5 char is an apple" << std::endl;
    }
    else
    {
        assert(false);
    }

}

int main()
{
    std::cout << "Start String Test..." << std::endl;
    // tune pool.
    size_type shortSize;
    size_type longSize;

    std::tie(shortSize, longSize) = getPoolSizes();
    assert(shortSize < longSize);

    testStaticDynamicCtors(shortSize, longSize);
    assert(StringPool::getUsedMem() == 0);

    testEqual();
    testFind();
    testSubstr();

    testEmpty();
    testAccessors();
    testErase();
    testAppend();
    testCstring();
    testCstringAssert();

    testStringUtils();

    testCompare();

    std::cout << "OK" << std::endl;

    return 0;
}

///@}
