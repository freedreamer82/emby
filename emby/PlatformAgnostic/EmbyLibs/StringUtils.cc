#include <EmbyLibs/StringUtils.hh>
#include <EmbyLibs/OutputFormatter.hh>
#include <cmath>

namespace EmbyLibs
{
    static const char *TRUE_STR = "true";
    static const char *FALSE_STR = "false";

    // reverses a string 'str' of length 'len'
    static void reverse(char *str, int len)
    {
        int i = 0, j = len - 1, temp;
        while (i < j)
        {
            temp = str[i];
            str[i] = str[j];
            str[j] = temp;
            i++;
            j--;
        }
    }


    String to_string(int val)
    {
        String text;
        bool isNegative = (val < 0);
        val = isNegative ? -val : val;
        do
        {
            int c = val % 10;
            val /= 10;
            text.push_back('0' + c);
        } while (val != 0);
        if (isNegative)
        {
            text.push_back('-');
        }
        std::reverse(text.begin(), text.end());
        return text;
    }

    String to_string_hex(int val)
    {
        String text;
        bool isNegative = (val < 0);
        val = isNegative ? -val : val;
        do
        {
            int c = val % 16;
            val /= 16;
            text.push_back((c < 10) ? '0' + c : 'A' + c - 10);
        } while (val != 0);
        if (isNegative)
        {
            text.push_back('-');
        }
        std::reverse(text.begin(), text.end());
        return text;
    }

    String to_bool_str(bool val)
    {
        return get_bool_str(val);
    }

    const char *get_bool_str(bool val)
    {
        return val ? TRUE_STR : FALSE_STR;
    }

    void ltrim(String &str)
    {
        String::size_type notwhite = str.find_first_not_of(" \t\n\r");
        str.erase(0, notwhite);
    }

    void rtrim(String &str)
    {
        String::size_type notwhite = str.find_last_not_of(" \t\n\r");
        str.erase(notwhite + 1);
    }

    void trim(String &str)
    {
        ltrim(str);
        rtrim(str);
    }

    bool
    doesStringBeginWith(String const &str, String const &probe)
    {
        return str.compare(0, probe.size(), probe) == 0;
    }

    bool
    doesStringEndWith(String const &str, String const &probe)
    {
        if (str.size() < probe.size())
        {
            return false;
        }
        return str.compare(str.size() - probe.size(),
                           probe.size(),
                           probe) == 0;
    }


    void
    tokenizeFillVector(String const &input,
                       std::vector <String> &array,
                       char const *delims)
    {
        struct is
        {
            static bool delimiter(char probe, char const *delim)
            {
                for (char const *scan = delim; *scan != '\0'; ++scan)
                {
                    if (*scan == probe)
                    {
                        return true;
                    }
                }
                return false;
            }
        };

        String::size_type first = 0;
        for (String::size_type i = 0; i < input.size(); ++i)
        {
            while (i < input.size() && is::delimiter(input[i], delims))
            {
                ++i;
            }
            first = i;
            while (i < input.size() && !is::delimiter(input[i], delims))
            {
                ++i;
            }
            if (first != i)
            {
                array.push_back(input.substr(first, i - first));
            }
        }

    }

    String &
    sprintf(String &out, char const *fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        EmbyLibs::sprintf(out, fmt, args);
        va_end(args);
        return out;
    }

    namespace
    {
        class StringAdapter : public IOutput
        {
        public:
            explicit StringAdapter(String &string) : m_string{string}
            {
            }

            ~StringAdapter() = default;

            void transmit(char const *buffer, size_t length) override
            {
                m_string.append(buffer, length);
            }

        private:
            String &m_string;
        };
    }

    String &
    sprintf(String &out, char const *fmt, va_list args)
    {
        out.clear();
        StringAdapter adapter(out);
        vprintf(adapter, fmt, args);
        return out;
    }

    bool
    isValidforBase(char n, int base)
    {
        if (n < '0')
        {
            return false;
        }
        if (n <= '9')
        {
            return n - '0' < base;
        }
        n = toupper(n);
        if (n < 'A')
        {
            return false;
        }
        return n - 'A' + 10 < base;
    }

    int
    charToDigit(char n, int base)
    {
        if (n <= '9')
        {
            return n - '0';
        }
        n = toupper(n);
        return n - 'A' + 10;
    }

    int
    atoi(String const &value, int base)
    {
        int retval = 0;

        bool isNegative = false;

        for (auto n: value)
        {
            if (n == '-')
            {
                isNegative = true;
                continue;
            }
            else if (n == '+')
            {
                continue;
            }

            if (!isValidforBase(n, base))
            {
                break;
            }
            int digit = charToDigit(n, base);

            retval = retval * base + digit;
        }
        return retval * (isNegative ? -1 : 1);
    }

    size_t
    copy(String const &value, char *dst, size_t size)
    {
        EmbyDebug_ASSERT_CHECK_NULL_PTR(dst);

        if (size == 0)
        {
            return 0;
        }

        size = std::min(size - 1, value.length());

        for (size_t i = 0; i < size; ++i)
        {
            *dst++ = value[i];
        }

        *dst = '\0';

        return size;
    }

    bool
    hex2Byte(char const *asciiString, uint8_t *result)
    {
        *result = 0;
        for (int i = 0; i < 2; i++)
        {
            if (!isxdigit(asciiString[i]))
            {
                return false;
            }
            uint8_t c = toupper(asciiString[i]);
            if (c < 'A')
            {
                c -= '0';
            }
            else
            {
                c -= 'A' - 10;
            }
            *result = (*result << 4) | c;
        }
        return true;
    }

    void
    ascii2Hex(char const *asciiString, char *hexString, int asciiStrLen)
    {

        for (int i = 0, index = 0; i < asciiStrLen; i = i + 2, index++)
        {
            if (!EmbyLibs::hex2Byte((asciiString + i), (uint8_t * )(hexString + index)))
            {
                break;
            }
        }
    }

    char *strtok(char *str, const char *delim)
    {
        static char *nxt; /* static variable used to advance the string to replace delimiters */
        static int size;  /* static variable used to count until the end of the string        */

        /* IMPORTANT: any advance to 'nxt' must be followed by a diminution of 'size', and vice verce */

        int i; /* counter of delimiter(s) in string */

        /* initialize the string when strtok22 is first calles and supplied with a valid string */
        if (str != NULL)
        {
            nxt = str;
            size = strlen(str);
        }

            /* if we havn't reached the end of the string, any other call to strtok22 with NULL will come here */
        else if (size > 0)
        {
            nxt++;      /* last run left nxt on a null terminator, so we have to advance it */
            size--;     /* any advancement must follow diminution of size                   */
            str = nxt;  /* string now points to the first char after the last delimiter     */
        }

            /* if we reached the end of string, return NULL pointer */
        else
        {
            str = NULL;
        }

        /* nxt is used to advance the string until a delimiter or a series of delimiters are encountered;
         * it then stops on the last delimiter which has turned to NULL terminator
         */
        while (*nxt)
        {
            i = strspn(nxt, delim);
            while (i > 1)        /* turns delimiters to NULL terminator (except the last one) */
            {
                *nxt = '\0';
                nxt++;
                size--;
                i--;
            }                   /* in the last delimiter we have to do something a      */
            if (1 == i)          /* bit different we have to actually take nxt backwards */
            {                   /* one step, to break the while(*nxt) loop              */
                *nxt = '\0';
                if (size > 1)    /* if the delimiter is last char, don't do next lines   */
                {
                    nxt--;
                    size++;     /* nxt is diminished so size increases                    */
                }
            }
            nxt++;          /* if no delimiter is found, advance nxt                  */
            size--;         /* in case of the last delimiter in a series, we took nxt */
        }                   /* a step back, so now moving it a step forward means     */
        /* it rests on a NULL terminator                          */
        return str;
    }


    void toUpper(String &str)
    {
        std::transform(str.begin(), str.end(), str.begin(), ::toupper);
    }

    void toLower(String &str)
    {
        std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    }

    static int intToStr(int x, char str[], int d)
    {
        int i = 0;
        do
        {
            str[i++] = (x % 10) + '0';
            x = x / 10;
        } while (x);

        // If number of digits required is more, then
        // add 0s at the beginning
        while (i < d)
            str[i++] = '0';

        reverse(str, i);
        str[i] = '\0';
        return i;
    }


    // Converts a floating point number to string.
    String float2String(float n, int afterpoint)
    {
        bool isNeg = false;

        if (n < 0)
        {
            n *= -1;
            isNeg = true;
        }

        char res[20];
        // Extract integer part
        int ipart = (int) n;

        // Extract floating part
        float fpart = n - (float) ipart;

        // convert integer part to string
        int i = intToStr(ipart, res, 0);

        // check for display option after point
        if (afterpoint != 0)
        {
            res[i] = '.';  // add dot

            // Get the value of fraction part upto given no.
            // of points after dot. The third parameter is needed
            // to handle cases like 233.007
            fpart = fpart * std::pow(10, afterpoint);

            intToStr((int) fpart, res + i + 1, afterpoint);
        }

        if (isNeg)
        {
            String ret("-");
            ret.append(res);
            return ret;
        }
        else
        {
            String ret(res);
            return ret;
        }
    }


    // Converts a floating point number to string.
    void float2String(float n, char *res, int afterpoint)
    {
        // Extract integer part
        int ipart = (int) n;

        // Extract floating part
        float fpart = n - (float) ipart;

        // convert integer part to string
        int i = intToStr(ipart, res, 0);

        // check for display option after point
        if (afterpoint != 0)
        {
            res[i] = '.';  // add dot

            // Get the value of fraction part upto given no.
            // of points after dot. The third parameter is needed
            // to handle cases like 233.007
            fpart = fpart * pow(10, afterpoint);

            intToStr((int) fpart, res + i + 1, afterpoint);
        }
    }

    char *cltrim(char *str)
    {
        size_t len = 0;
        char *frontp = str;
        char *endp = NULL;

        if (str == NULL)
        { return NULL; }
        if (str[0] == '\0')
        { return str; }

        len = strlen(str);
        endp = str + len;

        /* Move the front and back pointers to address the first non-whitespace
         * characters from each end.
         */
        while (isspace((unsigned char) *frontp))
        { ++frontp; }
        if (endp != frontp)
        {
            while (isspace((unsigned char) *(--endp)) && endp != frontp)
            {}
        }

        if (str + len - 1 != endp)
            *(endp + 1) = '\0';
        else if (frontp != str && endp == frontp)
            *str = '\0';

        /* Shift the string so that it starts at str so that if it's dynamically
         * allocated, we can still free it on the returned pointer.  Note the reuse
         * of endp to mean the front of the string buffer now.
         */
        endp = str;
        if (frontp != str)
        {
            while (*frontp)
            { *endp++ = *frontp++; }
            *endp = '\0';
        }


        return str;
    }

    bool parseAtcommand(const char *cmd, std::vector <EmbyLibs::AtParserValue> &result,
                        const char *startCmd, const char *token, size_t maxArg)
    {
        if (cmd != nullptr && token && startCmd)
        {

            const size_t MAX_CMD_ARGS = maxArg;

            auto *startAt = strstr(cmd, startCmd);
            auto *endOfcmd = strchr(cmd, '=');
            int newArgsNum = 0;

            if (startAt != nullptr)
            {
                //is at command like
                uint16_t len = (endOfcmd == nullptr) ? strlen(cmd) : endOfcmd - startAt;
                EmbyLibs::String cmd(startAt, len);
                trim(cmd);
                AtValue v{.string = cmd, .type = EmbyLibs::AtParserType::Command};
                result.push_back(v);
            }
            else
            {
                return false;
            }

            if (endOfcmd != nullptr)
            {

                endOfcmd = endOfcmd + 1; //remove '=' char
                //we have a possible set command ie. AT+TEST = 1,1
                auto *startArg = cltrim((char*)endOfcmd);
                char *tokenPtr = strstr(startArg, token);
                newArgsNum++;

                while (tokenPtr && startArg && newArgsNum < MAX_CMD_ARGS)
                {

                    auto argLen = (tokenPtr) - startArg;
                    EmbyLibs::String arg(startArg, argLen);
                    trim(arg);
                    AtValue v{.string = arg, .type = EmbyLibs::AtParserType::Value};
                    result.push_back(v);

                    startArg = cltrim(tokenPtr + strlen(token));
                    tokenPtr = strstr(startArg, token);
                    newArgsNum++;
                }

                if (!tokenPtr)
                {
                    //last arg
                    auto lastArgLen = strlen(startArg);
                    EmbyLibs::String lastArg(startArg, lastArgLen);
                    trim(lastArg);
                    AtValue v{.string = lastArg, .type = EmbyLibs::AtParserType::Value};
                    result.push_back(v);
                }
            }

            return true;
        }
        else
        {
            return false;
        };

    }


    bool
    ip4NumberToString(uint32_t ipno, EmbyLibs::String *ipstring)
    {
        return (ipstring->append(EmbyLibs::to_string((ipno >> 24))) == 0 &&
                ipstring->append(".") == 0 &&
                ipstring->append(EmbyLibs::to_string((((ipno >> 16) & 0xff)))) == 0 &&
                ipstring->append(".") == 0 &&
                ipstring->append(EmbyLibs::to_string(((ipno >> 8) & 0xff))) == 0 &&
                ipstring->append(".") == 0 &&
                ipstring->append(EmbyLibs::to_string((ipno & 0xff))) == 0
        );

    }

    bool ip4StringtoNumber(EmbyLibs::String const &ipstring, uint32_t *ipno)
    {
        uint32_t num = 0;
        std::vector <EmbyLibs::String> octets;
        EmbyLibs::tokenizeFillVector(ipstring, octets, ".");

        if (octets.size() == 4)
        {
            for (auto &s: octets)
            {
                auto no = atoi(s);
                num = (num << 8) + no;
            }

            *ipno = num;
            return true;
        }

        return false;

    }

    // Converts a floating point number to string.
    void ftoa(float n, char *res, int afterpoint)
    {
        // Extract integer part
        int ipart = (int) n;

        // Extract floating part
        float fpart = n - (float) ipart;

        // convert integer part to string
        int i = intToStr(ipart, res, 0);

        // check for display option after point
        if (afterpoint != 0)
        {
            res[i] = '.';  // add dot

            // Get the value of fraction part upto given no.
            // of points after dot. The third parameter is needed
            // to handle cases like 233.007
            fpart = fpart * pow(10, afterpoint);

            intToStr((int) fpart, res + i + 1, afterpoint);
        }
    }

}  // end of namespace EmbyLibs
///@}

