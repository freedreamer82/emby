/**
 * @addtogroup EmbyLibs
 * @{
 *
 * @file EmbyLibs/OutputFormatter.cc
 * @author Massimiliano Pagani
 * @date 2015-09-01
 */

#include <EmbyLibs/OutputFormatter.hh>
#include <EmbyDebug/assert.hh>
#include <EmbyLibs/String.hh>
#include <utility>
#include <algorithm>
#include <cstring>
#include <cctype>

namespace EmbyLibs
{

    namespace
    {
        struct ItoaResult
        {
            size_t length;
            bool isNegative;
        };

        template<typename N>
        size_t itoaImpl( N n, char* buffer )
        {
            // n must be positive
            size_t count = 0;
            char* begin = buffer;

            do
            {
                *buffer++ = '0'+(n % 10);
                n /= 10;
                ++count;
            }
            while( n > 0 );
            *buffer = '\0';

            std::reverse( begin, buffer );

            return count;
        }

        template<typename N>
        typename std::enable_if<std::is_unsigned<N>::value,ItoaResult>::type
        itoa( N n, char* buffer )
        {
            return { itoaImpl( n, buffer ), false };
        }

        template<typename N>
        typename std::enable_if<std::is_signed<N>::value,ItoaResult>::type
        itoa( N n, char* buffer )
        {
            bool isNegative = (n < 0);
            if( isNegative )
            {
                n = -n;
            }
            return { itoaImpl( n, buffer ), isNegative };
        }

        enum LetterCase
        {
            LOWERCASE,
            UPPERCASE
        };

        constexpr char hexDigit( unsigned digit, LetterCase letterCase )
        {
            return digit < 10 ? digit+'0' :
                   letterCase == LOWERCASE ? digit-10+'a' :
                                             digit-10+'A';
        }

        template<typename N>
        size_t itox( N n, char* buffer, LetterCase letterCase )
        {
            static_assert( std::is_unsigned<N>::value, "N must be unsigned" );
            size_t count = 0;
            char* begin = buffer;

            do
            {
                *buffer++ = hexDigit( n % 16, letterCase );
                n /= 16;
                ++count;
            }
            while( n > 0 );
            *buffer = '\0';

            std::reverse( begin, buffer );

            return count;

        }

        struct PrintfState
        {
            public:
                void end( char const* scan, IOutput& out );
                void push( char const* scan, IOutput& out, va_list* args );
            private:
                bool isAlternateForm = false;
                bool isZeroPadded = false;
                bool isLeftAdjusted = false;
                bool isSpaceForPositive = false;
                bool isSignRequired = false;
                char const* begin = nullptr;
                size_t width = 0;
                // int precision = 0;
                LetterCase letterCase = LOWERCASE;

                enum Length
                {
                    DEFAULT,
                    CHAR_OR_SHORT,
                    LONG_OR_LONG_LONG,
                    CHAR,
                    SHORT,
                    LONG,
                    LONG_LONG,
                    LONG_DOUBLE,
                    PTR_DIFF
                };

                Length typeLength = DEFAULT;
                enum State
                {
                    IDLE,
                    SCANNING,
                    PERCENT_FLAG,
                    PERCENT_WIDTH,
                    PERCENT_PRECISION,
                    PERCENT_LENGTH,
                    PERCENT_CONVERSION
                };

                State current = IDLE;

                bool isLengthDefined() const;
                bool processFlag( char c );
                bool processWidth( char c );
                bool processPrecision( char c );
                bool processLength( char c );
                void pushIdle( char const* scan, IOutput& out, va_list* args );
                void pushScanning( char const* scan, IOutput& out, va_list* args );
                void printRepeat( IOutput& out, char c, int repeatCount );
                void printCString( IOutput& out, char const* string );
                void printString( IOutput& out, String const* string );
                void printChar( IOutput& out, char ch );
                void padBuffer( IOutput& out,
                                char const* buffer,
                                size_t length,
                                char sign );
                void printDecimalInt( IOutput& out, va_list* args );
                void printDecimalUint( IOutput& out, va_list* args );
                void printHexUint( IOutput& out, va_list* args );
                void pushPercentFlag( char const* scan, IOutput& out, va_list* args );
                void pushPercentWidth( char const* scan, IOutput& out, va_list* args );
                void pushPercentPrecision( char const* scan, IOutput& out, va_list* args );
                void pushPercentLength( char const* scan, IOutput& out, va_list* args );
                void pushPercentConversion( char const* scan,
                                            IOutput& out,
                                            va_list* args );
        };

        inline bool PrintfState::isLengthDefined() const
        {
            return typeLength != CHAR_OR_SHORT &&
                   typeLength != LONG_OR_LONG_LONG;
        }

        bool PrintfState::processFlag( char c )
        {
            switch( c )
            {
                case '#':
                    isAlternateForm = true;
                    break;
                case '0':
                    isZeroPadded = true;
                    break;
                case '-':
                    isLeftAdjusted = true;
                    break;
                case ' ':
                    isSpaceForPositive = true;
                    break;
                case '+':
                    isSignRequired = true;
                    break;
                default:
                    return false;
            }
            return true;
        }

        bool PrintfState::processWidth( char c )
        {
            if( isdigit(c) )
            {
                width = width*10 + c-'0';
                return true;
            }
            return false;
        }
/*
        bool PrintfState::processPrecision( char c )
        {
            if( isdigit( c ))
            {
                precision = precision*10 + c-'0';
                return true;
            }
            return false;
        }
*/
        bool PrintfState::processLength( char c )
        {
            if( typeLength == DEFAULT )
            {
                switch( c )
                {
                    case 'h':
                        typeLength = CHAR_OR_SHORT;
                        break;
                    case 'l':
                        typeLength = LONG_OR_LONG_LONG;
                        break;
                    case 'L':
                        typeLength = LONG_DOUBLE;
                        break;
                    case 't':
                        typeLength = PTR_DIFF;
                        break;
                    default:
                        return false;

                }
                return true;
            }
            else if( typeLength == CHAR_OR_SHORT )
            {
                if( c == 'h' )
                {
                    typeLength = CHAR;
                    return true;
                }
                else
                {
                    typeLength = SHORT;
                    return false;
                }
            }
            else if( typeLength == LONG_OR_LONG_LONG )
            {
                if( c == 'l' )
                {
                    typeLength = LONG_LONG;
                    return true;
                }
                else
                {
                    typeLength = LONG;
                    return false;
                }
            }
            return false;
        }

        void PrintfState::pushIdle( char const* scan, IOutput& out, va_list* args )
        {
            (void)out;
            (void)args;
            switch( *scan )
            {
                case '%':
                    current = PERCENT_FLAG;
                    break;
                default:
                    begin = scan;
                    current = SCANNING;
                    break;
            }
        }

        void PrintfState::pushScanning( char const* scan, IOutput& out, va_list* args )
        {
            (void)args;
            switch( *scan )
            {
                case '%':
                    out.transmit( begin, scan-begin );
                    current = PERCENT_FLAG;
                    break;
                default:
                    break;
            }
        }

        void PrintfState::printRepeat( IOutput& out, char c, int repeatCount )
        {
            while( repeatCount > 0 )
            {
                out.transmit( &c, 1 );
                --repeatCount;
            }
        }

        void PrintfState::printString( IOutput& out, String const* string )
        {
            if( width != 0 && width > string->length() )
            {
                int spaceCount = width - string->length();
                if( !isLeftAdjusted )
                {
                    printRepeat( out, ' ', spaceCount );
                }
                out.transmit( string->data(), string->length() );
                if( isLeftAdjusted )
                {
                    printRepeat( out, ' ', spaceCount );
                }
            }
            else
            {
                out.transmit( string->data(), string->length() );
            }
        }

        void PrintfState::printCString( IOutput& out, char const* string )
        {
            size_t length = std::strlen( string );
            if( width != 0 && width > length )
            {
                int spaceCount = width - length;
                if( !isLeftAdjusted )
                {
                    printRepeat( out, ' ', spaceCount );
                }
                out.transmit( string, length );
                if( isLeftAdjusted )
                {
                    printRepeat( out, ' ', spaceCount );
                }
            }
            else
            {
                out.transmit( string, length );
            }
        }

        inline void PrintfState::printChar( IOutput& out, char ch )
        {
            out.transmit( &ch, 1 );
        }

        void PrintfState::padBuffer( IOutput& out,
                                     char const* buffer,
                                     size_t length,
                                     char sign )
        {
            if( sign == '\0' && isSpaceForPositive )
            {
                sign = ' ';
            }
            size_t lengthWithSign = length + (sign != '\0');
            if( width > lengthWithSign )
            {
                if( !isLeftAdjusted )
                {
                    if( isZeroPadded )
                    {
                        if( sign != '\0' )
                        {
                            out.transmit( &sign, 1 );
                        }
                        printRepeat( out, '0', width-lengthWithSign );
                    }
                    else
                    {
                        printRepeat( out, ' ', width-lengthWithSign );
                        if( sign != '\0' )
                        {
                            out.transmit( &sign, 1 );
                        }
                    }
                }
                else
                {
                    if( sign != '\0' )
                    {
                        out.transmit( &sign, 1 );
                    }
                }
                out.transmit( buffer, length );
                if( isLeftAdjusted )
                {
                    printRepeat( out, ' ', width-lengthWithSign );
                }
            }
            else
            {
                if( sign != '\0' )
                {
                    out.transmit( &sign, 1 );
                }
                out.transmit( buffer, length );
            }
        }

        void PrintfState::printDecimalInt( IOutput& out, va_list* args )
        {
            char buffer[32];    // this holds more than what is needed
                                // for uint64_t
            ItoaResult result;
            switch( typeLength )
            {
                case SHORT:
                    // fall through
                case CHAR:
                    // fall through
                case DEFAULT:
                    result = itoa( va_arg( *args, int ), buffer );
                    break;
                case LONG:
                    result = itoa( va_arg( *args, long ), buffer );
                    break;
                case LONG_LONG:
                    result = itoa( va_arg( *args, long long ), buffer );
                    break;
                default:
                    result.length = 0;
                    result.isNegative = false;
            }
            char sign = '\0';

            if( result.isNegative )
            {
                sign = '-';
            }
            else if( isSignRequired )
            {
                sign = '+';
            }
            padBuffer( out, buffer, result.length, sign );
        }

        void PrintfState::printDecimalUint( IOutput& out, va_list* args )
        {
            char buffer[32];    // this holds more than what is needed
                                // for uint64_t
            ItoaResult result;
            switch( typeLength )
            {
                case SHORT:
                    // fall through
                case CHAR:
                    // fall through
                case DEFAULT:
                    result = itoa( va_arg( *args, unsigned int ), buffer );
                    break;
                case LONG:
                    result = itoa( va_arg( *args, unsigned long ), buffer );
                    break;
                case LONG_LONG:
                    result = itoa( va_arg( *args, unsigned long long ), buffer );
                    break;
                default:
                    result.length = 0;
                    result.isNegative = false;
            }
            char sign = '\0';

            if( isSignRequired )
            {
                sign = '+';
            }

            padBuffer( out, buffer, result.length, sign );
        }

        void PrintfState::printHexUint( IOutput& out, va_list* args )
        {
            char buffer[32];    // this holds more than what is needed
                                // for uint64_t
            size_t length;
            switch( typeLength )
            {
                case SHORT:
                    // fall through
                case CHAR:
                    // fall through
                case DEFAULT:
                    length = itox( va_arg( *args, unsigned int ),
                                   buffer,
                                   letterCase );
                    break;
                case LONG:
                    length = itox( va_arg( *args, unsigned long ),
                                   buffer,
                                   letterCase );
                    break;
                case LONG_LONG:
                    length = itox( va_arg( *args, unsigned long long ),
                                   buffer,
                                   letterCase );
                    break;
                default:
                    length = 0;
            }
            padBuffer( out, buffer, length, '\0' );
        }

        void PrintfState::pushPercentConversion( char const* scan,
                                                 IOutput& out,
                                                 va_list* args )
        {
            switch( *scan )
            {
                case 'd':
                case 'i':
                    printDecimalInt( out, args );
                    break;

                case 'u':
                    printDecimalUint( out, args );
                    break;

                case 'o':
                case 'x':
                    letterCase = LOWERCASE;
                    printHexUint( out, args );
                    break;
                case 'X':
                    letterCase = UPPERCASE;
                    printHexUint( out, args );
                    break;
                case 'e':
                case 'E':
                case 'f':
                case 'F':
                case 'g':
                case 'G':
                case 'a':
                case 'A':
                    break;
                case 'c':
                    printChar( out, va_arg( *args, int ));
                    break;
                case 's':
                    // "long" length not supported.
                    printCString( out, va_arg( *args, const char* ) );
                    break;
                case 'S':
                	printString( out, va_arg( *args, String const* ));
                	break;
                case 'p':
                case 'n':
                    break;
                case '%':
                    // fall through
                default:
                    out.transmit( scan, 1 );
                    break;
            }
            current = IDLE;
        }

        void PrintfState::pushPercentLength( char const* scan,
                                             IOutput& out,
                                             va_list* args )
        {
            if( processLength( *scan ))
            {
                if( isLengthDefined() )
                {
                    current = PERCENT_CONVERSION;
                }
                else
                {
                    current = PERCENT_LENGTH;
                }
            }
            else
            {
                pushPercentConversion( scan, out, args );
            }
        }

        void PrintfState::pushPercentPrecision( char const* scan,
                                                IOutput& out,
                                                va_list* args )
        {
            if( isdigit( *scan ))
            {
//                precision = precision*10 + *scan - '0';
            }
            else
            {
                pushPercentLength( scan, out, args );
            }
        }


        void PrintfState::pushPercentWidth( char const* scan,
                                            IOutput& out,
                                            va_list* args )
        {
            if( processWidth( *scan ))
            {
                current = PERCENT_WIDTH;
            }
            else if( *scan == '.' )
            {
                current = PERCENT_PRECISION;
            }
            else
            {
                pushPercentLength( scan, out, args );
            }
        }

        void PrintfState::pushPercentFlag( char const* scan,
                                           IOutput& out,
                                           va_list* args )
        {
            if( processFlag(*scan ) )
            {
                current = PERCENT_FLAG;
            }
            else
            {
                pushPercentWidth( scan, out, args );
            }
        }

        void PrintfState::push( char const* scan, IOutput& out, va_list* args )
        {
            switch( current )
            {
                case IDLE:
                    pushIdle( scan, out, args );
                    break;
                case SCANNING:
                    pushScanning( scan, out, args );
                    break;
                case PERCENT_FLAG:
                    pushPercentFlag( scan, out, args );
                    break;
                case PERCENT_WIDTH:
                    pushPercentWidth( scan, out, args );
                    break;
                case PERCENT_PRECISION:
                    pushPercentPrecision( scan, out, args );
                    break;
                case PERCENT_LENGTH:
                    pushPercentLength( scan, out, args );
                    break;
                case PERCENT_CONVERSION:
                    pushPercentConversion( scan, out, args );
                    break;
                default:
                    EmbyDebug_ASSERT_FAIL();
                    break;
            }
        }

        inline void PrintfState::end( char const* scan, IOutput& out )
        {
            if( current == SCANNING )
            {
                out.transmit( begin, scan-begin );
            }
        }
    }

    void printf( IOutput& out, char const* fmt, ... )
    {
        va_list args;

        va_start( args, fmt );
        vprintf( out, fmt, args );
        va_end( args );
    }

    void vprintf( IOutput& out, char const* scan, va_list args )
    {
        PrintfState state;
#if defined( va_copy )
        va_list vaCopy;
        va_copy( vaCopy, args );
#endif
        while( *scan != '\0' )
        {
#if defined( va_copy )
            state.push( scan, out, &vaCopy );
#else
            state.push( scan, out, &args );
#endif
            ++scan;
        }
#if defined( va_copy )
        va_end( vaCopy );
#endif
        state.end( scan, out );
    }

}


/**
 * @}
 */
