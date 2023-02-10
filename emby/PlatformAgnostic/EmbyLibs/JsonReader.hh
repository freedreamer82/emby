// jRead.h
// author: Marco Garzola
//
#ifndef JREAD_H
#define JREAD_H

#include <stdint.h>
#include <cstring>
#include <stdio.h>

/*
 * Examples:
  {
    "astring":"This is a string",
    "anumber":42,
    "myarray":[ "zero", 1, {"description":"element 2"}, null ],
    "yesno":true,
    "PI":"3.1415926",
    "foo":null,
     "myobj":{ "key1":"value1", "key2":"value2" },
     "Numbers":[
              { "Name":"Fred",   "Ident":12345 },
              { "Name":"Jim",    "Ident":"87654" },
              { "Name":"Zaphod", "Ident":"0777621" }
            ]
   }

############# Examples 1
struct jReadElement result;
JsonReader::get().read( pJson, "{'myarray'", &result );

result.dataType= JREAD_ARRAY
result.elements= 4
result.byteLen=  46
result.pValue -> [ "zero", 1, {"description":"element 2"}, null ]

############# Examples 2
JsonReader::get().read( pJson, "{'myarray' [2 {'description'", &result );
result.pValue -> "element 2"

############# Examples 3
 #define NAMELEN 32
 struct NamesAndNumbers{      // our application wants the JSON "Numbers" array data
    char Name[NAMELEN];      // in an array of these structures
    long Number;
};
...
struct NamesAndNumbers people[42];
struct jReadElement element;
int i;
  JsonReader::get().read( pJson, "{'Numbers'", &element );    // we expect "Numbers" to be an array
  if( element.dataType == JREAD_ARRAY )
  {
      for( i=0; i<element.elements; i++ )    // loop for no. of elements in JSON
      {
          JsonReader::get().read_string( pJson, "{'Numbers'[*{'Name'", people[i].Name, NAMELEN, &i
); people[i].Number=   JsonReader::get().read_long( pJson, "{'Numbers'[*{'Ident'", &i );
      }
  }

############# Examples 4
JsonReader::get().read(test, "{'myobj' {'key1'", &result);
 -> value1
JsonReader::get().read(test, "{'myobj' {'key2'", &result);
 -> value2
 *
 "{'keyname'"           Object element "keyname", returns value of that key
  "{NUMBER"              Object element[NUMBER], returns keyname of that element
  "[NUMBER"              Array element[NUMBER], returns value from array
 *
// * */
// You may have noticed that the query string uses 'single quotes' to enclose key names instead of
// JSON-required "double quotes" - this is just to make it easier to type in a query string
// (there is an option to change this). Having to use double quotes just ends up messy, e.g.,
// uncomment this if you really want to use double quotes in query strings instead of '
// jRead( pJson, "{'myarray' [2 {'description'", &result );
// jRead( pJson, "{\"myarray\"[2{\"description\"", &result );      \\ ugh!
//#define JREAD_DOUBLE_QUOTE_IN_QUERY
//------------------------------------------------------
// jReadElement
// - structure to return JSON elements
// - error=0 for valid returns
//
// *NOTES*
//    the returned pValue pointer points into the passed JSON
//    string returns are not '\0' terminated.
//    bytelen specifies the length of the returned data pointed to by pValue
//

class JsonReader
{
public:
    enum class dataType : uint8_t
    {
        Error,
        Object,     // "{"
        Array,      // "["
        String,     // "string"
        Number,     // number (may be -ve) int or float
        Bool,       // true or false
        Null,       // null
        Key,        // object "key"
        Colon,      // ":"
        Eol,        // end of input string (ptr at '\0')
        Comma,      // ","
        EndObject,  // "}"
        EndArray,   // "]"
        QueryParams // "*" query string parameter
    };

    enum class Error : uint8_t
    {
        Ok,                           // 0
        NotMatchQuery,                // 1
        ErrorReadingValue,            // 2
        ExpectedKey,                  // 3
        ExpectedColon,                // 4
        ObjectKeyNotFound,            // 5
        ExpectedComma,                // 6
        TerminalFoundBeforeEndQuery,  // 7
        UnexpectedCharacter,          // 8
        ExpectedCommaInArray,         // 9
        ArrayElementNotFoundBadIndex, // 10
        ObjectKeyNotFoundBadIndex,    // 11
        BadObjectKey,                 // 12
        EndOfArrayFound,              // 13
        EndOfObjectFound,             // 14
        UnknownError                  // 15
    };

    static JsonReader& get();

    struct jReadElement
    {
        JsonReader::dataType dataType; // one of JREAD_...
        int16_t              elements; // number of elements (e.g. elements in array or object)
        int bytelen; // byte length of element (e.g. length of string, array text "[ ... ]" etc.)
        const char*       pValue; // pointer to value string in JSON text
        JsonReader::Error error;  // error value if dataType == JREAD_ERROR
    };

    bool hasValue(const char* pJson, const char* pQuery);

    //------------------------------------------------------
    // The JSON reader function
    //
    // - reads a '\0'-terminated JSON text string from pJson
    // - traverses the JSON according to the pQuery string
    // - returns the result value in pResult
    //
    // returns: pointer into pJson after the queried value
    //
    // e.g.
    //    With JSON like: "{ ..., "key":"value", ... }"
    //
    //    jRead( pJson, "{'key'", &result );
    // returns with:
    //    result.dataType= JREAD_STRING, result.pValue->'value', result.bytelen=5
    //
    const char* read(const char* pJson, const char* pQuery, struct jReadElement* pResult);

    // version of jRead which allows one or more queryParam integers to be substituted
    // for array or object indexes marked by a '*' in the query
    //
    // e.g. jReadParam( pJson, "[*", &resultElement, &arrayIndex );
    //
    // *!* CAUTION *!*
    // You can supply an array of integers which are indexed for each '*' in pQuery
    // however, horrid things will happen if you don't supply enough parameters
    //
    const char* readParam(const char*          pJson,
                          const char*          pQuery,
                          struct jReadElement* pResult,
                          int*                 queryParams);

    // Array Stepping function
    // - assumes pJsonArray is JSON source of an array "[ ... ]"
    // - returns next element of the array in pResult
    // - returns pointer to end of element, to be passed to next call of jReadArrayStep()
    // - if end of array is encountered, pResult->error = 13 "End of array found"
    //
    // e.g.
    //   With JSON like:   "{ ...  "arrayInObject":[ elem1,elem2,... ], ... }"
    //
    //   pJson= jRead( pJson, "{'arrayInObject'", &theArray );
    //   if( theArray.dataType == JREAD_ARRAY )
    //   {
    //       char *pArray= (char *)theArray.pValue;
    //       jReadElement arrayElement;
    //       int index;
    //       for( index=0; index < theArray.elements; index++ )
    //       {
    //           pArray= jReadArrayStep( pArray, &arrayElement );
    //           ...
    //
    // Note: this significantly speeds up traversing arrays.
    //
    const char* readArrayStep(const char* pJsonArray, struct jReadElement* pResult);

    //------------------------------------------------------
    // Optional Helper Functions
    //
    long  read_long(const char* pJson, const char* pQuery, int* queryParams = NULL);
    int   read_int(const char* pJson, const char* pQuery, int* queryParams = NULL);
    float read_float(const char* pJson, const char* pQuery, int* queryParams = NULL);
    int   read_string(
            const char* pJson, const char* pQuery, char* pDest, int destlen, int* queryParams = NULL);

    //------------------------------------------------------
    // Optional String output Functions
    //
    char* readTypeMessage(char* destBuffer,
                          int   destLength,
                          int   datatype); // get string describing data type
    char* readErrorMessage(char* destBuffer,
                           int   destLength,
                           int   error); // get string describing error code

    // copy element to '\0'-terminated buffer
    int strcpy(char* destBuffer, int destLength, struct jReadElement* pElement);

    // jReadCountArray
    // - used when query ends at an array, we want to return the array length
    // - on entry pJson -> "[... "
    // - used to skip unwanted values which are arrays
    //
    const char* readCountArray(const char* pJson, struct JsonReader::jReadElement* pResult);


    // readCountObject
    // - used when query ends at an object, we want to return the object length
    // - on entry pJson -> "{... "
    // - used to skip unwanted values which are objects
    // - keyIndex normally passed as -1 unless we're looking for the nth "key" value
    //   in which case keyIndex is the index of the key we want
    //
    const char*
    readCountObject(const char* pJson, struct JsonReader::jReadElement* pResult, int keyIndex);

    // jReadTextLen
    // - used to identify length of element text
    // - returns no. of chars from pJson upto a terminator
    // - terminators: ' ' , } ]
    //
    int readTextLen(const char* pJson);
private:
    const char* jReadSkipWhitespace(const char* sp);
    const char* jReadFindTok(const char* sp, JsonReader::dataType* tokType);
    const char*
    jReadGetString(const char* pJson, struct JsonReader::jReadElement* pElem, char quote);


    //------------------------------------------------------
    // Other jRead utilities which may be useful...
    //
    const char* jRead_atoi(const char* p, unsigned int* result); // string to unsigned int
    const char* jRead_atol(const char* p, long* result);         // string to signed long
    const char* jRead_atof(const char* p,
                           float*      result); // string to float (does not do exponents)
    int         jReadStrcmp(struct jReadElement* j1,
                            struct jReadElement* j2); // compare STRING elements
};

// end of jRead.h

#endif /* JREAD_H */
