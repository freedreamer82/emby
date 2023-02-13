
#include "JsonReader.hh"
namespace EmbyLibs
{
// By default we use single quote in query strings so it's a lot easier
// to type in code i.e.  "{'key'" instead of "{\"key\""
//
#ifdef JREAD_DOUBLE_QUOTE_IN_QUERY
#define QUERY_QUOTE '\"'
#else
#define QUERY_QUOTE '\''
#endif


//------------------------------------------------------
// Internal Functions

    const char emptyString[1] = {0};

//=======================================================

    JsonReader &JsonReader::get()
    {
        static JsonReader jread;
        return jread;
    }

    const char *JsonReader::jReadSkipWhitespace(const char *sp)
    {
        while ((*sp != '\0') && (*sp <= ' '))
            sp++;
        return sp;
    };

// Find start of a token
// - returns pointer to start of next token or element
//   returns type via tokType
//
    const char *JsonReader::jReadFindTok(const char *sp, JsonReader::dataType *tokType)
    {
        char c;
        sp = jReadSkipWhitespace(sp);
        c = *sp;
        if (c == '\0')
            *tokType = JsonReader::dataType::Eol;
        else if ((c == '"') || (c == QUERY_QUOTE))
            *tokType = JsonReader::dataType::String;
        else if ((c >= '0') && (c <= '9'))
            *tokType = JsonReader::dataType::Number;
        else if (c == '-')
            *tokType = JsonReader::dataType::Number;
        else if (c == '{')
            *tokType = JsonReader::dataType::Object;
        else if (c == '[')
            *tokType = JsonReader::dataType::Array;
        else if (c == '}')
            *tokType = JsonReader::dataType::EndObject;
        else if (c == ']')
            *tokType = JsonReader::dataType::EndArray;
        else if ((c == 't') || (c == 'f'))
            *tokType = JsonReader::dataType::Bool;
        else if (c == 'n')
            *tokType = JsonReader::dataType::Null;
        else if (c == ':')
            *tokType = JsonReader::dataType::Colon;
        else if (c == ',')
            *tokType = JsonReader::dataType::Comma;
        else if (c == '*')
            *tokType = JsonReader::dataType::QueryParams;
        else
            *tokType = JsonReader::dataType::Error;
        return sp;
    };

// jReadGetString
// - assumes next element is "string" which may include "\" sequences
// - returns pointer to -------------^
// - pElem contains result ( JsonReader::dataType::String, length, pointer to string)
// - pass quote = '"' for Json, quote = '\'' for query scanning
//
// returns: pointer into pJson after the string (char after the " terminator)
//			pElem contains pointer and length of string (or
// dataType=JsonReader::dataType::Error)
//
    const char *
    JsonReader::jReadGetString(const char *pJson, struct JsonReader::jReadElement *pElem, char quote)
    {
        short skipch;
        pElem->dataType = JsonReader::dataType::Error;
        pElem->elements = 1;
        pElem->bytelen = 0;
        pJson = jReadSkipWhitespace(pJson);
        if (*pJson == quote)
        {
            pJson++;
            pElem->pValue = pJson; // -> start of actual string
            pElem->bytelen = 0;
            skipch = 0;
            while (*pJson != '\0')
            {
                if (skipch)
                    skipch = 0;
                else if (*pJson == '\\') //  "\" sequence
                    skipch = 1;
                else if (*pJson == quote)
                {
                    pElem->dataType = JsonReader::dataType::String;
                    pJson++;
                    break;
                }
                pElem->bytelen++;
                pJson++;
            };
        };
        return pJson;
    };


    int JsonReader::readTextLen(const char *pJson)
    {
        int len = 0;
        while ((*pJson > ' ') && // any ctrl char incl '\0'
               (*pJson != ',') && (*pJson != '}') && (*pJson != ']'))
        {
            len++;
            pJson++;
        }
        return len;
    }

// compare two json elements
// returns: 0 if they are identical strings, else 1
//
    int JsonReader::jReadStrcmp(struct JsonReader::jReadElement *j1,
                                struct JsonReader::jReadElement *j2)
    {
        int i;
        if ((j1->dataType != JsonReader::dataType::String) ||
            (j2->dataType != JsonReader::dataType::String) || (j1->bytelen != j2->bytelen))
            return 1;

        for (i = 0; i < j1->bytelen; i++)
            if (((char *) (j1->pValue))[i] != ((char *) (j2->pValue))[i])
                return 1;
        return 0;
    }

// read unsigned int from string
    const char *JsonReader::jRead_atoi(const char *p, unsigned int *result)
    {
        unsigned int x = 0;
        while (*p >= '0' && *p <= '9')
        {
            x = (x * 10) + (*p - '0');
            ++p;
        }
        *result = x;
        return p;
    }

// read long int from string
//
    const char *JsonReader::jRead_atol(const char *p, long *result)
    {
        long x = 0;
        int neg = 0;
        if (*p == '-')
        {
            neg = 1;
            ++p;
        }
        while (*p >= '0' && *p <= '9')
        {
            x = (x * 10) + (*p - '0');
            ++p;
        }
        if (neg)
        {
            x = -x;
        }
        *result = x;
        return p;
    }

#define valid_digit(c) ((c) >= '0' && (c) <= '9')

// read float from string
// *CAUTION* does not handle exponents
//
//
    const char *JsonReader::jRead_atof(const char *p, float *result)
    {
        float sign, value;

        // Get sign, if any.
        sign = 1.0;
        if (*p == '-')
        {
            sign = -1.0;
            p += 1;
        }
        else if (*p == '+')
        {
            p += 1;
        }

        // Get digits before decimal point or exponent, if any.
        for (value = 0.0; valid_digit(*p); p += 1)
        {
            value = value * 10.0 + (*p - '0');
        }

        // Get digits after decimal point, if any.
        if (*p == '.')
        {
            float pow10 = 10.0;
            p += 1;
            while (valid_digit(*p))
            {
                value += (*p - '0') / pow10;
                pow10 *= 10.0;
                p += 1;
            }
        }
        *result = sign * value;
        return p;
    }

// jRead_strcpy
// read element into destination buffer and add '\0' terminator
// - always copies element irrespective of dataType (unless it's an error)
// - destBuffer is always '\0'-terminated (even on zero length returns)
// - returns length in destBuffer
//
    int JsonReader::strcpy(char *destBuffer,
                           int destLength,
                           struct JsonReader::jReadElement *pElement)
    {
        int i = 0;
        if (pElement->error == JsonReader::Error::Ok)
        {
            for (i = 0; (i < pElement->bytelen) && (i < destLength - 1); i++)
                *destBuffer++ = pElement->pValue[i];
        }
        *destBuffer = '\0';
        return i;
    }

    const char *JsonReader::readCountObject(const char *pJson,
                                            struct JsonReader::jReadElement *pResult,
                                            int keyIndex)
    {
        struct JsonReader::jReadElement jElement;
        JsonReader::dataType jTok;
        const char *sp;
        pResult->dataType = JsonReader::dataType::Object;
        pResult->error = JsonReader::Error::Ok;
        pResult->elements = 0;
        pResult->pValue = pJson;
        sp = jReadFindTok(pJson + 1, &jTok); // check for empty object
        if (jTok == JsonReader::dataType::EndObject)
        {
            pJson = sp + 1;
        }
        else
        {
            while (1)
            {
                pJson = jReadGetString(++pJson, &jElement, '\"');
                if (jElement.dataType != JsonReader::dataType::String)
                {
                    pResult->error = JsonReader::Error::ExpectedKey; // Expected "key"
                    break;
                }
                if (pResult->elements == keyIndex) // if passed keyIndex
                {
                    *pResult = jElement; // we return "key" at this index
                    pResult->dataType = JsonReader::dataType::Key;
                    return pJson;
                }
                pJson = jReadFindTok(pJson, &jTok);
                if (jTok != JsonReader::dataType::Colon)
                {
                    pResult->error = JsonReader::Error::ExpectedColon; // Expected ":"
                    break;
                }
                pJson = read(++pJson, emptyString, &jElement);
                if (pResult->error != JsonReader::Error::Ok)
                    break;
                pJson = jReadFindTok(pJson, &jTok);
                pResult->elements++;
                if (jTok == JsonReader::dataType::EndObject)
                {
                    pJson++;
                    break;
                }
                if (jTok != JsonReader::dataType::Comma)
                {
                    pResult->error = JsonReader::Error::ExpectedComma; // Expected "," in object
                    break;
                }
            }
        }
        if (keyIndex >= 0)
        {
            // we wanted a "key" value - that we didn't find
            pResult->dataType = JsonReader::dataType::Error;
            pResult->error =
                    JsonReader::Error::ObjectKeyNotFoundBadIndex; // Object key not found (bad index)
        }
        else
        {
            pResult->bytelen = pJson - (char *) pResult->pValue;
        }
        return pJson;
    }


    const char *JsonReader::readCountArray(const char *pJson,
                                           struct JsonReader::JsonReader::jReadElement *pResult)
    {
        struct JsonReader::jReadElement jElement;
        JsonReader::dataType jTok;
        const char *sp;
        pResult->dataType = JsonReader::dataType::Array;
        pResult->error = JsonReader::Error::Ok;
        pResult->elements = 0;
        pResult->pValue = pJson;
        sp = jReadFindTok(pJson + 1, &jTok); // check for empty array
        if (jTok == JsonReader::dataType::EndArray)
        {
            pJson = sp + 1;
        }
        else
        {
            while (1)
            {
                pJson = JsonReader::read(++pJson, emptyString, &jElement); // array value
                if (pResult->error != JsonReader::Error::Ok)
                    break;
                pJson = jReadFindTok(pJson, &jTok); // , or ]
                pResult->elements++;
                if (jTok == JsonReader::dataType::EndArray)
                {
                    pJson++;
                    break;
                }
                if (jTok != JsonReader::dataType::Comma)
                {
                    pResult->error = JsonReader::Error::ExpectedCommaInArray; // Expected "," in array
                    break;
                }
            }
        }
        pResult->bytelen = pJson - (char *) pResult->pValue;
        return pJson;
    }

// jReadArrayStep()
// - reads one value from an array
// - assumes pJsonArray points at the start of an array or array element
//
    const char *JsonReader::readArrayStep(const char *pJsonArray,
                                          struct JsonReader::jReadElement *pResult)
    {
        JsonReader::dataType jTok;

        pJsonArray = jReadFindTok(pJsonArray, &jTok);
        switch (jTok)
        {
            case JsonReader::dataType::Array: // start of array
            case JsonReader::dataType::Comma: // element separator
                return JsonReader::read(++pJsonArray, emptyString, pResult);

            case JsonReader::dataType::EndArray:                     // end of array
                pResult->error = JsonReader::Error::EndOfArrayFound; // End of array found
                break;
            default:                                                      // some other error
                pResult->error = JsonReader::Error::ExpectedCommaInArray; // expected comma in array
                break;
        }
        pResult->dataType = JsonReader::dataType::Error;
        return pJsonArray;
    }

// jRead
// - reads a complete JSON <value>
// - matches pQuery against pJson, results in pResult
// returns: pointer into pJson
//
// Note: is recursive
//
    const char *
    JsonReader::read(const char *pJson, const char *pQuery, struct JsonReader::jReadElement *pResult)
    {
        return JsonReader::readParam(pJson, pQuery, pResult, NULL);
    }

    bool JsonReader::hasValue(const char *pJson, const char *pQuery)
    {
        struct JsonReader::jReadElement result;

        JsonReader::read(pJson, pQuery, &result);
        if (result.error == JsonReader::Error::Ok)
        {
            return true;
        }
        return false;
    }

    const char *JsonReader::readParam(const char *pJson,
                                      const char *pQuery,
                                      struct JsonReader::jReadElement *pResult,
                                      int *queryParams)
    {
        JsonReader::dataType qTok, jTok;
        int bytelen;
        unsigned int index, count;
        struct JsonReader::jReadElement qElement, jElement;

        pJson = jReadFindTok(pJson, &jTok);
        pQuery = jReadFindTok(pQuery, &qTok);

        pResult->dataType = jTok;
        pResult->bytelen = 0;
        pResult->elements = 0;
        pResult->error = JsonReader::Error::Ok;
        pResult->pValue = pJson;

        if ((qTok != JsonReader::dataType::Eol) && (qTok != jTok))
        {
            pResult->dataType = JsonReader::dataType::Error;
            pResult->error = JsonReader::Error::NotMatchQuery; // JSON does not match Query
            return pJson;
        }

        switch (jTok)
        {
            case JsonReader::dataType::Error: // general error, eof etc.
                pResult->dataType = JsonReader::dataType::Error;
                pResult->error = JsonReader::Error::ErrorReadingValue; // Error reading JSON value
                break;

            case JsonReader::dataType::Object: // "{"
                if (qTok == JsonReader::dataType::Eol)
                    return readCountObject(pJson, pResult, -1); // return length of object

                pQuery = jReadFindTok(++pQuery, &qTok); // "('key'...", "{NUMBER", "{*" or EOL
                if (qTok != JsonReader::dataType::String)
                {
                    index = 0;
                    switch (qTok)
                    {
                        case JsonReader::dataType::Number:
                            pQuery = jRead_atoi((char *) pQuery, &index); // index value
                            break;
                        case JsonReader::dataType::QueryParams:
                            pQuery++;
                            index = (queryParams != NULL) ? *queryParams++ : 0; // substitute parameter
                            break;
                        default:
                            pResult->error = JsonReader::Error::BadObjectKey; // Bad Object key
                            return pJson;
                    }
                    return readCountObject(pJson, pResult, index);
                }

                pQuery = jReadGetString(pQuery, &qElement, QUERY_QUOTE); // qElement = query 'key'
                //
                // read <key> : <value> , ... }
                // loop 'til key matched
                //
                while (1)
                {
                    pJson = jReadGetString(++pJson, &jElement, '\"');
                    if (jElement.dataType != JsonReader::dataType::String)
                    {
                        pResult->dataType = JsonReader::dataType::Error;
                        pResult->error = JsonReader::Error::ExpectedKey; // Expected "key"
                        break;
                    }
                    pJson = jReadFindTok(pJson, &jTok);
                    if (jTok != JsonReader::dataType::Colon)
                    {
                        pResult->dataType = JsonReader::dataType::Error;
                        pResult->error = JsonReader::Error::ExpectedColon; // Expected ":"
                        break;
                    }
                    // compare object keys
                    if (jReadStrcmp(&qElement, &jElement) == 0)
                    {
                        // found object key
                        return JsonReader::readParam(++pJson, pQuery, pResult, queryParams);
                    }
                    // no key match... skip this value
                    pJson = JsonReader::read(++pJson, emptyString, pResult);
                    pJson = jReadFindTok(pJson, &jTok);
                    if (jTok == JsonReader::dataType::EndObject)
                    {
                        pResult->dataType = JsonReader::dataType::Error;
                        pResult->error = JsonReader::Error::ObjectKeyNotFound; // Object key not found
                        break;
                    }
                    if (jTok != JsonReader::dataType::Comma)
                    {
                        pResult->dataType = JsonReader::dataType::Error;
                        pResult->error = JsonReader::Error::ExpectedComma; // Expected "," in object
                        break;
                    }
                }
                break;
            case JsonReader::dataType::Array: // "[NUMBER" or "[*"
                //
                // read index, skip values 'til index
                //
                if (qTok == JsonReader::dataType::Eol)
                    return readCountArray(pJson, pResult); // return length of object

                index = 0;
                pQuery = jReadFindTok(++pQuery, &qTok); // "[NUMBER" or "[*"
                if (qTok == JsonReader::dataType::Number)
                {
                    pQuery = jRead_atoi(pQuery, &index); // get array index
                }
                else if (qTok == JsonReader::dataType::QueryParams)
                {
                    pQuery++;
                    index = (queryParams != NULL) ? *queryParams++ : 0; // substitute parameter
                }

                count = 0;
                while (1)
                {
                    if (count == index)
                        return JsonReader::readParam(
                                ++pJson, pQuery, pResult, queryParams); // return value at index
                    // not this index... skip this value
                    pJson = JsonReader::read(++pJson, emptyString, &jElement);
                    if (pResult->error != JsonReader::Error::Ok)
                        break;
                    count++;
                    pJson = jReadFindTok(pJson, &jTok); // , or ]
                    if (jTok == JsonReader::dataType::EndArray)
                    {
                        pResult->error =
                                JsonReader::Error::ArrayElementNotFoundBadIndex; // Array element not found
                        // (bad index)
                        break;
                    }
                    if (jTok != JsonReader::dataType::Comma)
                    {
                        pResult->dataType = JsonReader::dataType::Error;
                        pResult->error =
                                JsonReader::Error::ExpectedCommaInArray; // Expected "," in array
                        break;
                    }
                }
                break;
            case JsonReader::dataType::String: // "string"
                pJson = jReadGetString(pJson, pResult, '\"');
                break;
            case JsonReader::dataType::Number: // number (may be -ve) int or float
            case JsonReader::dataType::Bool:   // true or false
            case JsonReader::dataType::Null:   // null
                bytelen = readTextLen(pJson);
                pResult->dataType = jTok;
                pResult->bytelen = bytelen;
                pResult->pValue = pJson;
                pResult->elements = 1;
                pJson += bytelen;
                break;
            default:
                pResult->error = JsonReader::Error::UnexpectedCharacter; // unexpected character (in
                // pResult->dataType)
        }
        // We get here on a 'terminal value'
        // - make sure the query string is empty also
        pQuery = jReadFindTok(pQuery, &qTok);
        if (pResult->error == JsonReader::Error::Ok && (qTok != JsonReader::dataType::Eol))
            pResult->error = JsonReader::Error::TerminalFoundBeforeEndQuery; // terminal value found
        // before end of query
        if (pResult->error != JsonReader::Error::Ok)
        {
            pResult->dataType = JsonReader::dataType::Error;
            pResult->elements = pResult->bytelen = 0;
            pResult->pValue = pJson; // return pointer into JSON at error point
        }
        return pJson;
    }

//--------------------------------------------------------------------
// Optional helper functions
// - simple routines to extract values from JSON
// - does coercion of types where possible
// - always returns a value (e.g. 0 or "" on error)
//
// Note: by default, pass NULL for queryParams
//       unless you are using '*' in the query for indexing
//

// read_long
// - reads signed long value from JSON
// - returns number from NUMBER or STRING elements (if possible)
//   returns 1 or 0 from BOOL elements
//   otherwise returns 0
//
    long JsonReader::read_long(const char *pJson, const char *pQuery, int *queryParams)
    {
        struct JsonReader::jReadElement elem;
        long result;
        JsonReader::readParam(pJson, pQuery, &elem, queryParams);
        if ((elem.dataType == JsonReader::dataType::Error) ||
            (elem.dataType == JsonReader::dataType::Null))
            return 0;
        if (elem.dataType == JsonReader::dataType::Bool)
            return *((char *) elem.pValue) == 't' ? 1 : 0;

        jRead_atol((char *) elem.pValue, &result);
        return result;
    }

    int JsonReader::read_int(const char *pJson, const char *pQuery, int *queryParams)
    {
        return (int) read_long(pJson, pQuery, queryParams);
    }

// read_float
// - returns float from JSON
// - returns number from NUMBER or STRING elements
//   otherwise returns 0.0
//
    float JsonReader::read_float(const char *pJson, const char *pQuery, int *queryParams)
    {
        struct JsonReader::jReadElement elem;
        float result;
        JsonReader::readParam(pJson, pQuery, &elem, queryParams);
        if (elem.dataType == JsonReader::dataType::Error)
            return 0.0;
        jRead_atof((char *) elem.pValue, &result);
        return result;
    }

// read_string
// Copy string to pDest and '\0'-terminate it (upto destlen total bytes)
// returns: character length of string (excluding '\0' terminator)
//
// Note: any element can be returned as a string
//
    int JsonReader::read_string(
            const char *pJson, const char *pQuery, char *pDest, int destlen, int *queryParams)
    {
        struct JsonReader::jReadElement elem;
        int i;

        *pDest = '\0';
        JsonReader::readParam(pJson, pQuery, &elem, queryParams);
        if (elem.dataType == JsonReader::dataType::Error)
            return 0;

        for (i = 0; (i < elem.bytelen) && (i < destlen - 1); i++)
            *pDest++ = elem.pValue[i];
        *pDest = '\0';
        return elem.bytelen;
    }
}