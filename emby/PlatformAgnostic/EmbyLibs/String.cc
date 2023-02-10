
#include <EmbyLibs/String.hh>
static char constexpr STRING_TERMINATOR = '\0';

namespace EmbyLibs
{
    String::String(size_type requiredLength)
            : m_length(requiredLength)
            , m_allocType(getAllocTypeForLength(requiredLength))
    {
        if (m_allocType == AllocType::DYNAMIC)
        {
            if( requiredLength <= Pool::max_size() )
            {
                m_length =  requiredLength ;
            }
            else
            {
                if (String::getBufferFullPolicy() ==  FullStringBufferPolicy::Assert)
                {
                    EmbyDebug_ASSERT_FAIL();
                }
                else if   (String::getBufferFullPolicy() ==  FullStringBufferPolicy::CropString)
                {
                    m_length =  Pool::max_size();
                    //go ahead clipping the string...
                }
                else if   (String::getBufferFullPolicy() ==  FullStringBufferPolicy::NullPointer)
                {
                    m_length = requiredLength;
                }
            }
            auto p   = Pool::allocate(m_length);
            m_impl.m_dynamic.m_ptr      = p.first;
            m_impl.m_dynamic.m_capacity = p.second;
        }
    }

    char* String::getDataPtr()
    {
        if (m_allocType == AllocType::STATIC)
        {
            return m_impl.m_storage;
        }
        else
        {
            return m_impl.m_dynamic.m_ptr;
        }
    }

    const char* String::c_str()
    {
        if (push_back(STRING_TERMINATOR))
        {
            m_length--;
            return getDataPtr();
        }
        else
        {
            return nullptr;
        }
    }

    auto String::getDataPtr() const -> char const*
    {
        if (m_allocType == AllocType::STATIC)
        {
            return m_impl.m_storage;
        }
        else
        {
            return m_impl.m_dynamic.m_ptr;
        }
    }

    auto String::getPointerToIndex(size_type index) -> char*
    {
        return getDataPtr() + index;
    }

    auto String::getPointerToIndex(size_type index) const -> char const*
    {
        return getDataPtr() + index;
    }

    String::String()
            : m_length(0)
            , m_allocType(AllocType::STATIC)
    {
    }

    auto String::getLegacyEnd(char const* scan) -> char const*
    {
        while (*scan != static_cast<char>(0))
        {
            ++scan;
        }
        return scan;
    }

    auto String::getLegacyLength(char const* scan) -> size_type
    {
        return getLegacyEnd(scan) - scan;
    }

    String::String(char const* text)
            : String(getLegacyLength(text))
    {
        if (getDataPtr())
        {
            std::copy(text, text + m_length, getDataPtr());
        }
    }

    String::String(char const* text, size_type len)
            : String(len)
    {
        if (getDataPtr())
        {
            std::copy(text, text + m_length, getDataPtr());
        }
    }

    String::String(String const& other)
            : m_length(other.m_length)
            , m_allocType(other.m_allocType)
    {
        char*       pointer;
        char const* source;
        if (m_allocType == AllocType::STATIC)
        {
            pointer = m_impl.m_storage;
            source  = other.m_impl.m_storage;
        }
        else
        {
            size_type neededCapacity;
            //@todo modified allocating minor buffer , is it correct or it's better to have a
            // null???
            m_length = (m_length <= Pool::max_size()) ? m_length : Pool::max_size();

            //            std::tie(pointer, neededCapacity) = Pool::allocate(m_length);
            auto p                      = Pool::allocate(m_length);
            pointer                     = p.first;
            neededCapacity              = p.second;
            m_impl.m_dynamic.m_ptr      = pointer;
            m_impl.m_dynamic.m_capacity = neededCapacity;
            source                      = other.m_impl.m_dynamic.m_ptr;
        }
        if (source && pointer)
            std::copy(source, source + m_length, pointer);
    }

    String::String(String const& text, size_type index, size_type count)
            : String(count == npos ? text.length() - index : count)
    {
        if (index > text.length())
        {
            EmbyDebug_ASSERT_FAIL();
            //        throw std::out_of_range("String");
        }
        auto src = text.getPointerToIndex(index);
        auto dst = getDataPtr();
        std::copy(src, src + m_length, dst);
    }

    String::String(size_type count, char c)
            : String(count)
    {
        auto pointer = getDataPtr();
        std::fill(pointer, pointer + m_length, c);
    }

    String::String(String&& other) noexcept
    : m_length(other.m_length)
    , m_allocType(other.m_allocType)
{
    if (m_allocType == AllocType::STATIC)
{
    std::copy(
            other.m_impl.m_storage, other.m_impl.m_storage + other.m_length, m_impl.m_storage);
}
else
{
m_impl.m_dynamic.m_ptr      = other.m_impl.m_dynamic.m_ptr;
m_impl.m_dynamic.m_capacity = other.m_impl.m_dynamic.m_capacity;
other.m_allocType           = AllocType::STATIC;
}
other.m_length = 0;
}

String::String(std::initializer_list<char> init)
        : String(init.size())
{
    std::copy_n(init.begin(), m_length, getDataPtr());
}

template <class InputIterator>
String::String(InputIterator scan, InputIterator last)
        : String(0u)
{
    while (scan != last)
    {
        push_back(*scan);
        ++scan;
    }
}

String::~String() noexcept
{
// if the allocation is static, then nothing needs to be done
if (m_allocType == AllocType::DYNAMIC && m_impl.m_dynamic.m_ptr)
{
Pool::free(m_impl.m_dynamic.m_ptr, m_impl.m_dynamic.m_capacity);
}
}

void String::swap(String& other) noexcept
{
std::swap(m_length, other.m_length);
std::swap(m_allocType, other.m_allocType);
std::swap(m_impl, other.m_impl);
}

// void swap( EmbyLibs::String const& lhs,
//         EmbyLibs::String const& rhs ) noexcept
// {
//     lhs.swap( rhs );
// }

EmbyLibs::String& String::operator=(EmbyLibs::String const& source)
{
    String tmp(source);
    swap(tmp);
    return *this;
}

String& String::operator=(char const* text)
{
    String tmp(text);
    swap(tmp);
    return *this;
}

String& String::operator=(char character)
{
    String tmp(1, character);
    swap(tmp);
    return *this;
}

String& String::operator=(String&& source)
{
    if (&source != this)
    {
        this->~String();
        new (this) String(source);
    }
    return *this;
}

String& String::operator=(std::initializer_list<char> init)
{
    String tmp(init);
    swap(tmp);
    return *this;
}

auto String::capacity() const noexcept -> size_type
{
if (m_allocType == AllocType::DYNAMIC)
{
return m_impl.m_dynamic.m_capacity;
}
else
{
return STATIC_CAPACITY;
}
}

int String::reserve(size_type newCapacity)
{
    if (newCapacity <= capacity())
    {
        return newCapacity;
    }
    // building a new string and then swapping it with this is the easiest
    // way to deal with exception safety.
    String tmp(newCapacity);
    tmp.m_length = m_length;
    if (tmp.m_allocType == AllocType::DYNAMIC && tmp.getDataPtr() && getDataPtr())
    {
        // if allocator has space continues..otherwise ciao ciao...
        // begin()/end() are simply pointer to the ends of the range
        std::copy(begin(), end(), tmp.getDataPtr());
        swap(tmp);
        return newCapacity;
    }

    return -1;
}

// template<typename char,typename Traits, typename Pool>
// int
// String::compare( String const& str ) const noexcept
//{
//    // use std::mismatch
//    return 0;
//}

namespace
{
    template <typename T, typename U>
    int signCompare(T left, U right)
    {
        if (left < right)
        {
            return -1;
        }
        else if (left == right)
        {
            return 0;
        }
        else
        {
            return +1;
        }
    }
} // namespace

int String::compare(size_type pos, size_type len, String const& str) const
{
    if (pos > m_length)
    {
        return -1;
        EmbyDebug_ASSERT_FAIL();
        //        throw std::out_of_range("String::compare");
    }
    if (pos + len > m_length)
    {
        len = m_length - pos;
    }
    size_type const otherSize  = str.size();
    size_type const compareLen = std::min(len, otherSize);
    int result = traits_type::compare(getDataPtr() + pos, str.getDataPtr(), compareLen);
    if (result == 0)
    {
        result = signCompare(len, otherSize);
    }

    return result;
}

// template<typename char,typename Traits, typename Pool>
// int
// String::compare( char const* s) const
//{
//    // use std::mismatch
//    return 0;
//}

typename String::size_type String::find(char const* s, size_type pos) const
{
    size_type __ret      = -1;
    auto      substr_len = strlen(s);

    char const* text = getDataPtr();
    if (text && (substr_len < m_length) && (pos < m_length))
    {
        auto data = &text[pos];
        auto pos  = strstr(data, s);
        if (pos)
        {
            __ret = (pos - data);
        }
    }
    return __ret;
}

typename String::size_type String::find(String const& str, size_type pos) const noexcept
{
int n = m_length;
int m = pos;
for (int i = 0; i <= n - m; ++i)
{
int j = 0;
while (j < m && getDataPtr()[i + j] == str.at(j))
{
++j;
}
if (j == m)
{ // match found
return i;
}
}
return -1;
}

auto String::find_first_not_of(char const* s, size_type pos) const -> size_type
{
    for (auto i = getPointerToIndex(pos); i != end(); ++i)
    {
        bool found = false;
        for (auto probe = s; *probe != '\0'; ++probe)
        {
            if (*i == *probe)
            {
                found = true;
                break;
            }
        }
        if (!found)
        {
            return pos;
        }
        ++pos;
    }
    return npos;
}

typename String::size_type String::find_last_not_of(char const* s, size_type pos) const
{
    auto predicate = [s](char ch) -> bool
    {
        for (char const* scan = s; *scan != '\0'; ++scan)
        {
            if (*scan == ch)
            {
                return true;
            }
        }
        return false;
    };

    size_type   scan = m_length;
    char const* text = getDataPtr();
    if (scan != 0)
    {
        scan -= 1;
        if (scan > pos)
        {
            scan = pos;
        }
        do
        {
            if (!predicate(text[scan]))
            {
                return scan;
            }
        } while (scan-- != 0);
    }
    return npos;
}

String String::substr(size_t pos, size_t len) const
{
    return String(*this, pos, len);
}

bool operator==(char const* lhs, EmbyLibs::String const& rhs)
{
    return rhs == lhs;
}

bool operator==(EmbyLibs::String const& lhs, EmbyLibs::String const& rhs) noexcept
{
return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

bool operator==(EmbyLibs::String const& lhs, char const* rhs)
{
    auto scan = rhs;

    for (auto ch : lhs)
    {
        if (*scan == '\0')
        {
            return false;
        }
        if (*scan != ch)
        {
            return false;
        }
        ++scan;
    }
    return *scan == '\0';
}

int String::append(String const& text) noexcept
{
size_type cropped    = 0;
size_type baseLength = m_length;
size_type count      = text.size();
size_type neededSize = size() + count;
if (reserve(neededSize) < 0)
{
return -1;
}

if (capacity() < neededSize)
{
count      = capacity() - baseLength;
neededSize = capacity();
cropped    = neededSize - capacity();
}

std::copy(text.getDataPtr(), text.getPointerToIndex(count), getPointerToIndex(baseLength));
m_length = neededSize;

return cropped > 0 ? cropped : 0;
//   return *this;
}
//
// String&
// String::append( String const& text ) noexcept
//{
//
//    size_type baseLength = m_length;
//    size_type count = text.size();
//    size_type neededSize = size() + count;
//    reserve( neededSize );
//    if( capacity() < neededSize )
//    {
//        count = capacity() - baseLength;
//        neededSize = capacity();
//    }
//
//    std::copy( text.getDataPtr(), text.getPointerToIndex( count ),
//               getPointerToIndex( baseLength ));
//    m_length = neededSize;
//    return *this;
//}

int String::append(char const* text, size_type count)
{
    size_type cropped    = 0;
    size_type neededSize = size() + count;
    reserve(neededSize);
    if (reserve(neededSize) < 0)
    {
        return -1;
    }

    if (capacity() < neededSize)
    {
        count   = capacity() - m_length;
        cropped = neededSize - capacity();
    }

    std::copy(text, text + count, end());
    m_length += count;
    // return *this;
    return cropped > 0 ? cropped : 0;
}

int String::append(char const* text)
{
    size_type count = getLegacyLength(text);
    return append(text, count);
}

String& String::erase(size_type position, size_type count)
{
    if (count == npos)
    {
        count = m_length - position;
    }
    else if (position + count > m_length)
    {
        count    = m_length - position;
        m_length = position;
        return *this;
    }
    char*       dst = getPointerToIndex(position);
    char const* src = getPointerToIndex(position + count);
    std::copy(src, const_cast<char const*>(getPointerToIndex(m_length)), dst);
    m_length -= count;
    return *this;
}

typename String::iterator String::erase(iterator position)
{
    if (position != end())
    {
        std::copy(position + 1, end(), position);
        m_length -= 1;
    }
    return position;
}

typename String::iterator String::erase(iterator eraseBegin, iterator eraseEnd)
{
    if (eraseBegin == end())
    {
        return end();
    }
    std::copy(eraseEnd, end(), eraseBegin);
    m_length -= (eraseEnd - eraseBegin);
    return eraseBegin;
}

bool String::push_back(char ch)
{
    auto neededSize = size() + 1;
    if (neededSize > capacity())
    {
        if (reserve(neededSize) < 0)
        {
            return false;
        }
    }
    if (neededSize <= capacity())
    {
        *getPointerToIndex(m_length) = ch;
        ++m_length;
    }
    return true;
}

void String::resize(size_type newSize)
{
    // resize( newSize, char() );
}

void String::clear() noexcept
{
m_length = 0;
}

bool String::empty() const noexcept
{
return m_length == 0;
}

typename String::const_reference String::operator[](size_type index) const
{
    return getDataPtr()[index];
}

typename String::reference String::operator[](size_type index)
{
    return getDataPtr()[index];
}

String::const_reference String::at(size_type index) const
{
    if (index >= m_length)
    {
        EmbyDebug_ASSERT_FAIL();
        //     throw std::out_of_range( "String::at" );
    }
    return operator[](index);
}

typename String::reference String::at(size_type index)
{
    if (index >= m_length)
    {
        EmbyDebug_ASSERT_FAIL();
    }
    return operator[](index);
}

auto String::begin() noexcept -> iterator
{
return getDataPtr();
}

auto String::end() noexcept -> iterator
{
return getDataPtr() + m_length;
}

auto String::begin() const noexcept -> const_iterator
{
return getDataPtr();
}

auto String::end() const noexcept -> const_iterator
{
return getDataPtr() + m_length;
}

auto String::cbegin() const noexcept -> const_iterator
{
return getDataPtr();
}

auto String::cend() const noexcept -> const_iterator
{
return getDataPtr() + m_length;
}

auto String::rbegin() noexcept -> reverse_iterator
{
return reverse_iterator(end());
}

auto String::rend() noexcept -> reverse_iterator
{
return reverse_iterator(begin());
}

// String::const_reverse_iterator
// String::rbegin() const noexcept
//{
//    return reverse_iterator( end() );
//}
//
// auto
// String::rend() const noexcept -> const_reverse_iterator
//{
//    return reverse_iterator( begin());
//}
//
// auto
// String::crbegin() const noexcept -> const_reverse_iterator
//{
//    return reverse_iterator( cend() );
//}
//
// auto
// String::crend() const noexcept -> const_reverse_iterator
//{
//    return reverse_iterator( cbegin() );
//}

auto String::size() const noexcept -> size_type
{
return m_length;
}

auto String::length() const noexcept -> size_type
{
return size();
}

auto String::max_size() const noexcept -> size_type
{
return Pool::max_size();
}

bool operator!=(EmbyLibs::String const& lhs, EmbyLibs::String const& rhs) noexcept
{
return !(lhs == rhs);
}

bool operator!=(char const* lhs, EmbyLibs::String const& rhs)
{
    return !(rhs == lhs);
}

bool operator!=(EmbyLibs::String const& lhs, char const* rhs)
{
    return !(lhs == rhs);
}

} // namespace EmbyLibs
