#if !defined( EMBYLIBS_STRINGUTILS_HH )
#define EMBYLIBS_STRINGUTILS_HH

#include <EmbyLibs/String.hh>
#include <cstdarg>
#include <vector>

namespace EmbyLibs
{
    enum class AtParserType : uint8_t
    {
        Command,
        Value
    };

    typedef struct AtParserValue
    {
        EmbyLibs::String string;
        AtParserType type;
    } AtValue;

    char *cltrim(char *str);

    bool parseAtcommand(const char *cmd, std::vector <AtParserValue> &result, const char *startCmd = "AT+",
                        const char *token = ",", size_t maxArg = 10);


    String to_string(int val);

    String to_string_hex(int val);

    String to_bool_str(bool val);

    const char *get_bool_str(bool val);

    void ltrim(String &str);

    void rtrim(String &str);

    void trim(String &str);

    bool
    doesStringBeginWith(String const &str, String const &probe);

    bool
    doesStringEndWith(String const &str, String const &probe);

    bool
    hex2Byte(char const *asciiString, uint8_t *result);

    void ascii2Hex(char const *asciiString, char *hexString, int asciiStrLen);

    void
    tokenizeFillVector(String const &input,
                       std::vector <String> &array,
                       char const *delims = " \t");

    String &
    sprintf(String &out, char const *fmt, ...);

    String &
    sprintf(String &out, char const *fmt, va_list args);

    int
    atoi(String const &value, int base = 10);

    /**
     * Copy the content of a String to a C string.
     *
     * @param value the source LightString.
     * @param dst a pointer to the C string where the string content will be
     *            stored, nul terminator included.
     * @param size the size of the space available (nul terminator included) for
     *             storing the string content.
     * @return the number of characters copied (not including the terminator)
     */
    size_t
    copy(String const &value, char *dst, size_t size);

    char *strtok(char *str, const char *delim);

    void toUpper(String &str);

    void toLower(String &str);

    String float2String(float n, int afterpoint = 4);

    int float2String(float n, char *res, int afterpoint = 4);

    bool ip4NumberToString(uint32_t ipno, EmbyLibs::String *ipstring);

    bool ip4StringtoNumber(EmbyLibs::String const &ipstring, uint32_t *ipno);


}  // end of namespace EmbyLibs

#endif
///@}


