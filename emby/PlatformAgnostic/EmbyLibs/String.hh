#if !defined(EMBY_STRINGS_HH)
#define EMBY_STRINGS_HH

#include <algorithm>
#include <EmbyLibs/StringPool.hh>

/* strstr example */
#include <stdio.h>
#include <string.h>
#include <iterator>

namespace EmbyLibs
{
    class String
    {

    public:
        enum class FullStringBufferPolicy : uint8_t
        {
            NullPointer = 0,
            Assert,
            CropString
        };
    private :
        inline static FullStringBufferPolicy s_policy = FullStringBufferPolicy::NullPointer;

    public:
        static  FullStringBufferPolicy getBufferFullPolicy( )
        {
            return  s_policy;
        }

        static void  setBufferFullPolicy(FullStringBufferPolicy policy)
        {
            String::s_policy = policy;
        }

        typedef StringPoolBase<char>   Pool;
        typedef std::char_traits<char> traits_type;
        //            typedef typename Traits::char_type value_type;
        typedef StringPoolBase<char>                  allocator_type;
        typedef typename Pool::size_type              size_type;
        typedef typename Pool::difference_type        difference_type;
        typedef typename Pool::reference              reference;
        typedef typename Pool::const_reference        const_reference;
        typedef typename Pool::pointer                pointer;
        typedef typename Pool::const_pointer          const_pointer;
        typedef char*                                 iterator;
        typedef char const*                           const_iterator;
        typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
        typedef std::reverse_iterator<iterator>       reverse_iterator;


    private:



        enum class AllocType : uint8_t
        {
            STATIC,
            DYNAMIC
        };

        uint16_t  m_length;
        AllocType m_allocType;

        struct Dynamic
        {
            char*  m_ptr;
            size_t m_capacity;
        };

        static size_type const STATIC_CAPACITY = sizeof(Dynamic) / sizeof(char);
        union
        {
            Dynamic m_dynamic;
            char    m_storage[STATIC_CAPACITY];
        } m_impl;

    public:
        /// Value returned by various member functions when they fail.
        static size_type const npos = static_cast<size_type>(-1);

        /**
         * @brief  Default constructor creates an empty string.
         */
        String();

        /**
         * @brief  Construct string with copy of value of @a text.
         * @param  text  Source string.
         */
        String(char const* text);

        /**
         * @brief  Construct string with copy of value of @a text with a specific len.
         * @param  text  Source string.
         */
        String(char const* text, size_type len);

        /**
         *  @brief  Construct string with copy of value of @a other.
         *  @param  other Source string.
         */
        String(String const& other);

        /**
         *  @brief  Construct string as copy of a substring.
         *  @param  text Source string.
         *  @param  index Index of first character to copy from.
         *  @param  count Number of characters to copy (default
         *                remainder).
         */
        String(String const& text, size_type index, size_type count = npos);

        /**
         *  @brief  Construct string as multiple characters.
         *  @param  count Number of characters.
         *  @param  c  character to use.
         */
        String(size_type count, char c);

        /**
         *  @brief  Move construct string.
         *  @param  other Source string.
         *
         *  The newly-created string contains the exact contents of @a
         *  other.
         *
         *  @a other is a valid, but unspecified string.
         */
        String(String&& other) noexcept;

        /**
         *  @brief  Construct string from an initializer %list.
         *  @param  init  std::initializer_list of characters.
         */
        String(std::initializer_list<char> init);

        /**
         *  @brief  Construct string as copy of a range.
         *  @param  begin Start of range.
         *  @param  end  End of range.
         */

        template <class InputIterator>
        String(InputIterator begin, InputIterator last);

        /**
         * @brief Destroy the string instance
         */
        ~String() noexcept;

        /**
         * Swaps the content of this string with the other string.
         *
         * @param other the string to swap with.
         */
        void swap(String& other) noexcept;

        /**
         * @brief Assign the value of @a source to this string.
         * @param source source string.
         * @return a reference to this.
         */
        String& operator=(String const& source);

        String& operator=(char const* text);

        String& operator=(char character);

        String& operator=(String&& source);

        String& operator=(std::initializer_list<char> init);

        iterator       begin() noexcept;
        iterator       end() noexcept;
        const_iterator begin() const noexcept;
        const_iterator end() const noexcept;
        const_iterator cbegin() const noexcept;
        const_iterator cend() const noexcept;

        reverse_iterator       rbegin() noexcept;
        reverse_iterator       rend() noexcept;
        const_reverse_iterator rbegin() const noexcept;
        const_reverse_iterator rend() const noexcept;
        const_reverse_iterator crbegin() const noexcept;
        const_reverse_iterator crend() const noexcept;

        size_type size() const noexcept;

        size_type length() const noexcept;

        size_type max_size() const noexcept;

        void resize(size_type newSize, char fillchar);
        void resize(size_type newSize);

        void shrink_to_fit();

        size_type capacity() const noexcept;

        // return new capacity allocated , -1 otherwise
        int reserve(size_type newCapacity = 0);

        void clear() noexcept;

        bool empty() const noexcept;

        const_reference operator[](size_type index) const;
        reference       operator[](size_type index);
        const_reference at(size_type index) const;
        reference       at(size_type index);

        reference       front();
        const_reference front() const;
        reference       back();
        const_reference back() const;

        /**
         *  @brief  Append a string to this string.
         *  @param text The string to append.
         *  @return  Reference to this string.
         */
        String& operator+=(String const& text);

        /**
         *  @brief  Append a C string.
         *  @param text The C string to append.
         *  @return  Reference to this string.
         */
        String& operator+=(char const* text);

        /**
         *  @brief  Append a character.
         *  @param c  The character to append.
         *  @return  Reference to this string.
         */
        String& operator+=(char c);

        /**
         *  @brief  Append an std::initializer_list of characters.
         *  @param initList The std::initializer_list of characters to be
         *                  appended.
         *  @return  Reference to this string.
         */
        String& operator+=(std::initializer_list<char> initList);

        /**
         *  @brief  Append a string to this string.
         *  @param text The string to append.
         *  @return  0 if appended completely , -1 if error or > 0 if count the char appended
         */
        int append(String const& text) noexcept;

        /**
         *  @brief  Append a substring.
         *  @param text The string to append.
         *  @param index Index of the first character of str to append.
         *  @param count The number of characters to append.
         *  @return  0 if appended completely , -1 if error or > 0 if count the char appended
         *  @throw  std::out_of_range if @a position is not a valid index.
         *
         *  This function appends @a count characters from @a text
         *  starting at @a position to this string.  If @a count is is larger
         *  than the number of available characters in @a text, the
         *  remainder of @a text is appended.
         */
        int append(String const& text, size_type index, size_type count);

        /**
         *  @brief Append a C substring.
         *  @param text The C string to append.
         *  @param count The number of characters to append.
         *  @return  0 if appended completely , -1 if error or > 0 if count the char appended
         */
        int append(char const* text, size_type count);

        /**
         *  @brief  Append a C string.
         *  @param text The C string to append.
         *  @return  0 if appended completely , -1 if error or > 0 if count the char appended
         */
        int append(char const* text);

        /**
         *  @brief  Append multiple characters.
         *  @param count The number of characters to append.
         *  @param character The character to use.
         *  @return  Reference to this string.
         *
         *  Appends count copies of character to this string.
         */
        String& append(size_type count, char character);

        /**
         *  @brief  Append an std::initializer_list of characters.
         *  @param initList The std::initializer_list of characters to append.
         *  @return  Reference to this string.
         */
        String& append(std::initializer_list<char> initList);

        /**
         *  @brief  Append a range of characters.
         *  @param first  Iterator referencing the first character to append.
         *  @param last  Iterator marking the end of the range.
         *  @return  Reference to this string.
         *
         *  Appends characters in the range [first,last) to this
         *  string.
         */
        template <class InputIterator>
        String& append(InputIterator first, InputIterator last);

        /**
         *  @brief  Append a single character.
         *  @param character  character to append.
         *   @return  true if pushed
         */
        bool push_back(char character);

        /**
         *  @brief  Set value to contents of another string.
         *  @param  text  Source string to use.
         *  @return  Reference to this string.
         */
        String& assign(String const& text);

        /**
         *  @brief  Set value to contents of another string.
         *  @param  text Source string to use.
         *  @return  Reference to this string.
         *
         *  This function sets this string to the exact contents of @a text.
         *  @a text is a valid, but unspecified string.
         */
        String& assign(String&& text);

        /**
         *  @brief  Set value to a substring of a string.
         *  @param text The string to use.
         *  @param index Index of the first character of str.
         *  @param count  Number of characters to use.
         *  @return  Reference to this string.
         *  @throw  std::out_of_range if @a pos is not a valid index.
         *
         *  This function sets this string to the substring of @a text
         *  consisting of @a count characters at @a position.  If @a count is
         *  is larger than the number of available characters in @a
         *  text, the remainder of @a text is used.
         */
        String& assign(String const& text, size_type index, size_type count);

        /**
         *  @brief  Set value to a C substring.
         *  @param text The C string to use.
         *  @param count Number of characters to use.
         *  @return  Reference to this string.
         *
         *  This function sets the value of this string to the first @a count
         *  characters of @a text.  If @a count is is larger than the number of
         *  available characters in @a text, the remainder of @a text is used.
         */
        String& assign(char const* text, size_type count);

        /**
         *  @brief  Set value to contents of a C string.
         *  @param text The C string to use.
         *  @return  Reference to this string.
         *
         *  This function sets the value of this string to the value of @a text.
         *  The data is copied, so there is no dependence on @a text once the
         *  function returns.
         */
        String& assign(char const* text);

        /**
         *  @brief  Set value to multiple characters.
         *  @param count  Length of the resulting string.
         *  @param character The character to use.
         *  @return  Reference to this string.
         *
         *  This function sets the value of this string to @a count copies of
         *  character @a character.
         */
        String& assign(size_type count, char character);

        /**
         *  @brief  Set value to a range of characters.
         *  @param first  Iterator referencing the first character to append.
         *  @param last  Iterator marking the end of the range.
         *  @return  Reference to this string.
         *
         *  Sets value of string to characters in the range [first,last ).
         */
        template <class InputIterator>
        String& assign(InputIterator first, InputIterator last);

        /**
         *  @brief  Set value to an std::initializer_list of characters.
         *  @param initList The std::initializer_list of characters to assign.
         *  @return  Reference to this string.
         */
        String& assign(std::initializer_list<char> initList);

        /**
         *  @brief  Insert multiple characters.
         *  @param position Iterator referencing location in string to insert
         *                  at.
         *  @param count  Number of characters to insert
         *  @param filler The character to insert.
         *  @throw  std::length_error  If new length exceeds @c max_size().
         *
         *  Inserts @a count copies of character @a filler starting at the
         *  position referenced by iterator @a position.  If adding
         *  characters causes the length to exceed max_size(),
         *  length_error is thrown.  The value of the string doesn't
         *  change if an error is thrown.
         */
        void insert(iterator position, size_type count, char filler);

        /**
         *  @brief  Insert a range of characters.
         *  @param position Iterator referencing location in string to insert
         *                  at.
         *  @param begin  Start of range.
         *  @param end  End of range.
         *  @throw  std::length_error  If new length exceeds @c max_size().
         *
         *  Inserts characters in range [begin,end ).  If adding
         *  characters causes the length to exceed max_size(),
         *  length_error is thrown.  The value of the string doesn't
         *  change if an error is thrown.
         */
        template <class InputIterator>
        void insert(iterator position, InputIterator begin, InputIterator end);

        /**
         *  @brief  Insert an std::initializer_list of characters.
         *  @param position  Iterator referencing location in string to insert
         *                   at.
         *  @param initList The std::initializer_list of characters to insert.
         *  @throw  std::length_error  If new length exceeds @c max_size().
         */
        void insert(iterator position, std::initializer_list<char> initList);

        /**
         *  @brief  Insert value of a string.
         *  @param position Iterator referencing location in string to insert
         *                  at.
         *  @param text The string to insert.
         *  @return  Reference to this string.
         *  @throw  std::length_error  If new length exceeds @c max_size().
         *
         *  Inserts value of @a text starting at @a position.  If adding
         *  characters causes the length to exceed max_size(),
         *  length_error is thrown.  The value of the string doesn't
         *  change if an error is thrown.
         */
        String& insert(size_type position, String const& text);

        /**
         *  @brief  Insert a substring.
         *  @param position1 Iterator referencing location in string to
         *                   insert at.
         *  @param text  The string to insert.
         *  @param position2 Start of characters in str to insert.
         *  @param count  Number of characters to insert.
         *  @return  Reference to this string.
         *  @throw  std::length_error  If new length exceeds @c max_size().
         *  @throw  std::out_of_range  If @a pos1 > size() or
         *  @a position2 > @a str.size().
         *
         *  Starting at @a position1, insert @a count character of @a text
         *  beginning with @a position2.  If adding characters causes the
         *  length to exceed max_size(), length_error is thrown.  If @a
         *  position1 is beyond the end of this string or @a position2 is
         *  beyond the end of @a text, out_of_range is thrown.  The
         *  value of the string doesn't change if an error is thrown.
         */
        String&
        insert(size_type position1, String const& text, size_type position2, size_type count);

        /**
         *  @brief  Insert a C substring.
         *  @param position  Iterator referencing location in string to
         *                   insert at.
         *  @param text  The C string to insert.
         *  @param count  The number of characters to insert.
         *  @return  Reference to this string.
         *  @throw  std::length_error  If new length exceeds @c max_size().
         *  @throw  std::out_of_range  If @a position is beyond the end of
         *                             this string.
         *
         *  Inserts the first @a count characters of @a text starting at @a
         *  position.  If adding characters causes the length to exceed
         *  max_size(), length_error is thrown.  If @a position is beyond
         *  end(), out_of_range is thrown.  The value of the string
         *  doesn't change if an error is thrown.
         */
        String& insert(size_type position, char const* text, size_type count);

        /**
         *  @brief  Insert a C string.
         *  @param position  Iterator referencing location in string to
         *                   insert at.
         *  @param text  The C string to insert.
         *  @return  Reference to this string.
         *  @throw  std::length_error  If new length exceeds @c max_size().
         *  @throw  std::out_of_range  If @a pos is beyond the end of this
         *  string.
         *
         *  Inserts the first @a n characters of @a text starting at @a
         *  position.  If adding characters causes the length to exceed
         *  max_size(), length_error is thrown. If @a position is beyond
         *  end(), out_of_range is  thrown. The value of the string doesn't
         *  change if an error is  thrown.
         */
        String& insert(size_type position, char const* text);

        /**
         *  @brief  Insert multiple characters.
         *  @param position  Index in string to insert at.
         *  @param count  Number of characters to insert
         *  @param fillchar The character to insert.
         *  @return  Reference to this string.
         *  @throw  std::length_error  If new length exceeds @c max_size().
         *  @throw  std::out_of_range  If @a position is beyond the end of
         *                             this string.
         *
         *  Inserts @a count copies of character @a fillchar starting at
         *  index @a position.  If adding characters causes the length to
         *  exceed max_size(), length_error is thrown.  If @a position >
         *  length(), out_of_range is thrown.  The value of the string
         *  doesn't change if an error is thrown.
         */
        String& insert(size_type position, size_type count, char fillchar);

        /**
         *  @brief  Insert one character.
         *  @param position Iterator referencing position in string to
         *                  insert at.
         *  @param character The character to insert.
         *  @return  Iterator referencing newly inserted char.
         *  @throw  std::length_error  If new length exceeds @c
         *                             max_size().
         *
         *  Inserts character @a character at position referenced by @a
         *  position. If adding character causes the length to exceed
         *  max_size(), length_error is thrown. If @a position is beyond
         *  end of string, out_of_range is thrown. The value of the string
         *  doesn't  change if an error is thrown.
         */
        iterator insert(iterator position, char character);

        /**
         *  @brief  Remove characters.
         *  @param position  Index of first character to remove ( default
         *                   0 ).
         *  @param count  Number of characters to remove ( default
         *                remainder ).
         *  @return  Reference to this string.
         *  @throw  std::out_of_range  If @a pos is beyond the end of this
         *  string.
         *
         *  Removes @a count characters from this string starting at @a
         *  position.  The length of the string is reduced by @a count.  If
         *  there are < @a count characters to remove, the remainder of
         *  the string is truncated.  If @a position is beyond end of
         *  string, out_of_range is thrown.  The value of the string
         *  doesn't change if an error is thrown.
         */
        String& erase(size_type position = 0, size_type count = npos);

        /**
         *  @brief  Remove one character.
         *  @param positionition  Iterator referencing the character to
         *                        remove.
         *  @return  iterator referencing same location after removal.
         *
         *  Removes the character at @a position from this string. The
         *  value of the string doesn't change if an error is thrown.
         */
        iterator erase(iterator position);

        /**
         *  @brief  Remove a range of characters.
         *  @param begin Iterator referencing the first character to
         *               remove.
         *  @param end Iterator referencing the end of the range.
         *  @return  Iterator referencing location of first after removal.
         *
         *  Removes the characters in the range [begin,end) from this
         *  string. The value of the string doesn't change if an error is
         *  thrown.
         */
        iterator erase(iterator begin, iterator end);

        /**
         *  @brief  Remove the last character.
         *
         *  The string must be non-empty.
         */
        void pop_back();

        /**
         *  @brief  Replace characters with value from another string.
         *  @param position  Index of first character to replace.
         *  @param count  Number of characters to be replaced.
         *  @param text  String to insert.
         *  @return  Reference to this string.
         *  @throw  std::out_of_range  If @a pos is beyond the end of this
         *  string.
         *  @throw  std::length_error  If new length exceeds @c max_size().
         *
         *  Removes the characters in the range [position,position+count )
         *  from this string.  In place, the value of @a text is inserted.
         *  If @a position is beyond end of string, out_of_range is
         *  thrown. If the length of the result exceeds max_size(),
         *  length_error is thrown.  The value of the string doesn't
         *  change if an error is thrown.
         */
        String& replace(size_type position, size_type count, String const& text);

        /**
         *  @brief  Replace characters with value from another string.
         *  @param position1  Index of first character to replace.
         *  @param count1  Number of characters to be replaced.
         *  @param text String to insert.
         *  @param position2  Index of first character of str to use.
         *  @param count2  Number of characters from str to use.
         *  @return  Reference to this string.
         *  @throw  std::out_of_range  If @a position1 > size() or @a
         *                             position2 > text.size().
         *  @throw  std::length_error  If new length exceeds @c max_size().
         *
         *  Removes the characters in the range [position1,position1 + n )
         *  from this  string.  In place, the value of @a text is
         *  inserted.  If @a position is  beyond end of string,
         *  out_of_range is thrown.  If the length of the  result exceeds
         *  max_size(), length_error is thrown.
         *  The value of the  string doesn't change if an error is thrown.
         */
        String& replace(size_type     position1,
                        size_type     count1,
                        String const& text,
                        size_type     position2,
                        size_type     count2);

        /**
         *  @brief  Replace characters with value of a C substring.
         *  @param position  Index of first character to replace.
         *  @param count1  Number of characters to be replaced.
         *  @param text C string to insert.
         *  @param count2  Number of characters from @a s to use.
         *  @return  Reference to this string.
         *  @throw  std::out_of_range  If @a pos1 > size().
         *  @throw  std::length_error  If new length exceeds @c max_size().
         *
         *  Removes the characters in the range [position,position +
         *  count1) from this string.  In place, the first @a count2
         *  characters of @a text are inserted, or all of @a text if @a
         *  count2 is too large. If  @a position is beyond end of string,
         *  out_of_range is thrown. If  the length of result exceeds
         *  max_size(), length_error is thrown.  The value of the string
         *  doesn't change if an error  is thrown.
         */
        String& replace(size_type position, size_type count1, char const* text, size_type count2);

        /**
         *  @brief  Replace characters with value of a C string.
         *  @param position  Index of first character to replace.
         *  @param count1  Number of characters to be replaced.
         *  @param text C string to insert.
         *  @return  Reference to this string.
         *  @throw  std::out_of_range  If @a pos > size().
         *  @throw  std::length_error  If new length exceeds @c max_size().
         *
         *  Removes the characters in the range [position,position +
         *  count1) from this string.  In place, the characters of @a text
         *  are inserted.  If @a position is beyond end of string,
         *  out_of_range is thrown.  If the length of result exceeds
         *  max_size(), length_error is thrown.  The value of the string
         *  doesn't change if an error is thrown.
         */
        String& replace(size_type position, size_type count1, char const* text);

        /**
         *  @brief  Replace characters with multiple characters.
         *  @param position  Index of first character to replace.
         *  @param count1  Number of characters to be replaced.
         *  @param count2  Number of characters to insert.
         *  @param fillchar character to insert.
         *  @return  Reference to this string.
         *  @throw  std::out_of_range  If @a position > size().
         *  @throw  std::length_error  If new length exceeds @c max_size().
         *
         *  Removes the characters in the range [pos,pos + n1 ) from this
         *  string. In place, @a count2 copies of @a fillchar are inserted.
         *  If @a position is beyond end of string, out_of_range is thrown.
         *  If the length of result exceeds max_size(), length_error is
         *  thrown.  The value of the string doesn't change if an error
         *  is thrown.
         */
        String& replace(size_type position, size_type count1, size_type count2, char fillchar);

        /**
         *  @brief  Replace range of characters with string.
         *  @param begin  Iterator referencing start of range to replace.
         *  @param end Iterator referencing end of range to replace.
         *  @param text  String value to insert.
         *  @return  Reference to this string.
         *  @throw  std::length_error  If new length exceeds @c max_size().
         *
         *  Removes the characters in the range [begin,end).  In place,
         *  the value of @a text is inserted.  If the length of result
         *  exceeds max_size(), length_error is thrown.  The value of
         *  the string doesn't change if an error is thrown.
         */
        String& replace(iterator begin, iterator end, String const& text);

        /**
         *  @brief  Replace range of characters with C substring.
         *  @param begin Iterator referencing start of range to replace.
         *  @param end Iterator referencing end of range to replace.
         *  @param text  C string value to insert.
         *  @param count  Number of characters from s to insert.
         *  @return  Reference to this string.
         *  @throw  std::length_error  If new length exceeds @c max_size().
         *
         *  Removes the characters in the range [begin,end).  In place,
         *  the first @a count characters of @a text are inserted.  If the
         *  length of result exceeds max_size(), length_error is thrown.
         *  The value of the string doesn't change if an error is
         *  thrown.
         */
        String& replace(iterator begin, iterator end, char const* text, size_type count);

        /**
         *  @brief  Replace range of characters with C string.
         *  @param begin Iterator referencing start of range to replace.
         *  @param end Iterator referencing end of range to replace.
         *  @param text C string value to insert.
         *  @return  Reference to this string.
         *  @throw  std::length_error  If new length exceeds @c max_size().
         *
         *  Removes the characters in the range [begin,end).  In place,
         *  the characters of @a text are inserted.  If the length of
         *  result exceeds max_size(), length_error is thrown.  The
         *  value of the string doesn't change if an error is thrown.
         */
        String& replace(iterator begin, iterator end, char const* text);

        /**
         *  @brief  Replace range of characters with multiple characters
         *  @param begin  Iterator referencing start of range to replace.
         *  @param end  Iterator referencing end of range to replace.
         *  @param count  Number of characters to insert.
         *  @param fillchar  character to insert.
         *  @return  Reference to this string.
         *  @throw  std::length_error  If new length exceeds @c max_size().
         *
         *  Removes the characters in the range [begin,end ).  In place,
         *  @a count copies of @a fillchar are inserted.  If the length of
         *  result exceeds max_size(), length_error is thrown.  The
         *  value of the string doesn't change if an error is thrown.
         */
        String& replace(iterator begin, iterator end, size_type count, char fillchar);

        /**
         *  @brief  Replace range of characters with range.
         *  @param removeBegin  Iterator referencing start of range to
         *                      replace.
         *  @param removeEnd  Iterator referencing end of range to replace.
         *  @param insertBegin Iterator referencing start of range to
         *                     insert.
         *  @param insertEnd  Iterator referencing end of range to insert.
         *  @return  Reference to this string.
         *  @throw  std::length_error  If new length exceeds @c max_size().
         *
         *  Removes the characters in the range [removeBegin,removeEnd ).
         *  In place, characters in the range [insertBegin,insertEnd ) are
         *  inserted.  If the length of result exceeds max_size(),
         *  length_error is thrown.
         *  The value of the string doesn't change if an error is
         *  thrown.
         */
        template <class InputIterator>
        String& replace(iterator      removeBegin,
                        iterator      removeEnd,
                        InputIterator insertBegin,
                        InputIterator insertEnd);

        /**
         *  @brief  Replace range of characters with std::initializer_list.
         *  @param removeBegin  Iterator referencing start of range to
         *                      replace.
         *  @param removeEnd  Iterator referencing end of range to replace.
         *  @param initList  The std::initializer_list of characters to
         *                   insert.
         *  @return  Reference to this string.
         *  @throw  std::length_error  If new length exceeds @c max_size().
         *
         *  Removes the characters in the range [removeBegin,removeEnd ).
         *  In place, characters in the range [removeBegin,removeEnd) are
         *  inserted.  If the length of result exceeds max_size(),
         *  length_error is thrown. The value of the string doesn't change
         *  if an error is thrown.
         */
        String&
        replace(iterator removeBegin, iterator removeEnd, std::initializer_list<char> initList);

        int compare(String const& str) const noexcept;
        int compare(size_type pos, size_type len, String const& str) const;
        int compare(size_type     pos,
                    size_type     len,
                    String const& str,
                    size_type     subpos,
                    size_type     sublen = npos) const;
        int compare(char const* s) const;
        int compare(size_type pos, size_type len, char const* s) const;
        int compare(size_type pos, size_type len, char const* s, size_type n) const;

        size_type find(String const& str, size_type pos = 0) const noexcept;
        size_type find(char const* str, size_type pos, size_type count) const;
        size_type find(char const* str, size_type pos = 0) const;
        size_type find(char ch, size_type pos) const noexcept;

        size_type rfind(String const& str, size_type pos = npos) const noexcept;
        size_type rfind(char const* s, size_type pos, size_type count) const;
        size_type rfind(char const* s, size_type pos = npos) const;
        size_type rfind(char ch, size_type pos = npos) const noexcept;

        size_type find_first_of(String const& str, size_type pos = 0) const noexcept;
        size_type find_first_of(char const* s, size_type pos, size_type count) const;
        size_type find_first_of(char const* s, size_type pos = 0) const;
        size_type find_first_of(char ch, size_type pos = 0) const noexcept;

        size_type find_first_not_of(String const& str, size_type pos = 0) const noexcept;
        size_type find_first_not_of(char const* s, size_type pos, size_type count) const;
        size_type find_first_not_of(char const* s, size_type pos = 0) const;
        size_type find_first_not_of(char ch, size_type pos = 0) const noexcept;

        size_type find_last_of(String const& str, size_type pos = npos) const noexcept;
        size_type find_last_of(char const* s, size_type pos, size_type count) const;
        size_type find_last_of(char const* s, size_type pos = npos) const;
        size_type find_last_of(char ch, size_type pos = npos) const noexcept;

        size_type find_last_not_of(String const& str, size_type pos = npos) const noexcept;
        size_type find_last_not_of(char const* s, size_type pos, size_type count) const;
        size_type find_last_not_of(char const* s, size_type pos = npos) const;
        size_type find_last_not_of(char ch, size_type pos = npos) const noexcept;

        String substr(size_t pos = 0, size_t len = npos) const;

        /*nternal Buffer ptr -> THIS IS NOT NULL TERMINATED by default , use c_str in the case you want to push a
         * terminator char*/
        const char* data() const noexcept
        {
            return getDataPtr();
        }

        /* use legacy string. It pushes a terminator into internal buffer without increasing length.
         *  USE CAREFULLY! return nullptr if error
         * */
        const char* c_str();

    private:
        static char const* getLegacyEnd(char const* scan);
        static size_type   getLegacyLength(char const* scan);
        // this has been implemented here because g++ 4.8.0 seems to have
        // some issues with noexcept in this case.
        static AllocType constexpr getAllocTypeForLength(size_type length) noexcept
        {
            {
                return (length > STATIC_CAPACITY) ? AllocType::DYNAMIC : AllocType::STATIC;
            }
        }
        explicit String(size_type length);
        char*       getDataPtr();
        char const* getDataPtr() const;
        char*       getPointerToIndex(size_type index);
        char const* getPointerToIndex(size_type index) const;
    };

    /**
     * Swaps the content of a string with another string.
     *
     * @param lhs one string
     * @param rhs the other string.
     *
     * @throw nothing.
     */

    void swap(String const& lhs, String const& rhs) noexcept;

    bool operator==(String const& lhs, String const& rhs) noexcept;

    bool operator==(char const* lhs, String const& rhs);

    bool operator==(String const& lhs, char const* rhs);

    bool operator!=(const String& lhs, const String& rhs) noexcept;

    bool operator!=(char const* lhs, const String& rhs);

    bool operator!=(const String& lhs, char const* rhs);

    bool operator<(const String& lhs, const String& rhs) noexcept;

    bool operator<(char const* lhs, const String& rhs);

    bool operator<(const String& lhs, char const* rhs);

    bool operator<=(String const& lhs, String const& rhs) noexcept;

    bool operator<=(const char* lhs, String const& rhs);

    bool operator<=(String const& lhs, const char* rhs);

    bool operator>(String const& lhs, String const& rhs) noexcept;

    bool operator>(const char* lhs, String const& rhs);

    bool operator>(String const& lhs, const char* rhs);

    bool operator>=(String const& lhs, String const& rhs) noexcept;

    bool operator>=(const char* lhs, String const& rhs);

    bool operator>=(String const& lhs, const char* rhs);

} // namespace EmbyLibs

#endif // EmbyLibs_LIGHTSTRING_HH
///@}
