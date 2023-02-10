#pragma once
//
// author: Marco Garzola

// JsonWriter.hpp
//
// A *really* simple JSON writer in C++
// - a collection of functions to generate JSON semi-automatically
//
// Example:
//		JsonWriter jw( buffer, buflen );		// create JsonWriter to write into
// supplied buffer 		jw.open( JW_OBJECT, JW_PRETTY );	// open root node as object
// jw.obj_string(
//"key", "value" ); 		jw.obj_int( "int", 1 ); 		jw.obj_array( "anArray");
// jw.arr_int( 0 ); 			jw.arr_int( 1
//); 			jw.arr_int( 2 ); 		jw.end();   //end array
//		err= jw.close();					// close root object
//
// results in:
//
//		{
//		    "key": "value",
//		    "int": 1,
//		    "anArray": [
//		        0,
//		        1,
//		        2
//		    ]
//		}
//
// Note that JsonWriter handles string quoting and getting commas in the right place.
// If the sequence of calls is incorrect
// e.g.
//		jw.open( buffer, buflen, JW_OBJECT, 1 );
//		jw.obj_string( "key", "value" );
//			jw.arr_int( 0 );
//      ...
//
// then the error code returned from jw.close() would indicate that you attempted to
// put an array element into an object (instead of a key:value pair)
// To locate the error, the supplied buffer has the JSON created upto the error point
// and a call to jw.errorPos() would return the function call at which the error occurred
// - in this case 3, the 3rd function call "jw.arr_int(0)" is not correct at this point.
//
// The root JSON type can be JW_OBJECT or JW_ARRAY.
//
// For more information on each function, see the methods below.
//
//
// Original JsonWriter in C: TonyWilk, Mar 2015
// This C++ version: TonyWilk, Mar 2018
//
#include <stdint.h>
#include <cstring>
#include <stdio.h>
#include <EmbyLibs/String.hh>

class JsonWriter
{
public:
    static constexpr uint16_t
    JWRITE_STACK_DEPTH = 10; // max nesting depth of objects/arrays

    enum class Error : uint8_t
    {
        Ok,
        BufferFull,
        NotArray,
        NotObject,
        StackFull,
        StackEmpty,
        NestError
    };

    enum class Type : uint8_t
    {
        JsonCompact,
        JsonPretty
    };

    enum class NodeType : uint8_t
    {
        Object = 1,
        Array
    };

    JsonWriter(char *pbuffer, int buf_len)
            : buffer(pbuffer), buflen(buf_len - 1)
    {
        buffer[buflen] = '\0'; // last byte close string
        open(NodeType::Object, Type::JsonCompact);
    };

    // open
    // - initialises JsonWriter
    // - rootType is the base JSON type: JW_OBJECT or JW_ARRAY
    // - isPretty controls 'prettifying' the output: JW_PRETTY or JW_COMPACT
    void open(NodeType rootType, Type mode);

    // close
    // - closes the element opened by open()
    // - returns error code (0 = JWRITE_OK)
    // - after an error, all following JsonWriter calls are skipped internally
    //   so the error code is for the first error detected
    JsonWriter::Error close();

    // errorPos
    // - if jwClose returned an error, this function returns the number of the JsonWriter function
    // call
    //   which caused that error.
    int errorPos();

    // Object insertion functions
    // - used to insert "key":"value" pairs into an object
    //
    void obj_string(const char *key, const char *value);

    void obj_string(const char *key, EmbyLibs::String &st);

    void obj_string(const char *key, const char *value, int len);

    void obj_int(const char *key, int value);

    void obj_double(const char *key, double value);

    void obj_float(const char *key, float value, int afterpoint = 2);

    void obj_bool(const char *key, int oneOrZero);

    void obj_null(const char *key);

    void obj_object(const char *key);

    void obj_array(const char *key);

    // Array insertion functions
    // - used to insert "value" elements into an array
    //
    void arr_string(const char *value);

    void arr_string(const char *value, int len);

    void arr_string(EmbyLibs::String &st);

    void arr_int(int value);

    void arr_double(double value);

    void arr_float(float value, int afterpoint = 2);

    void arr_bool(int oneOrZero);

    void arr_null();

    void arr_object();

    void arr_array();

    // jwEnd
    // - defines the end of an Object or Array definition
    Error end();

    // these 'raw' routines write the JSON value as the contents of rawtext
    // i.e. enclosing quotes are not added
    // - use if your app. supplies its own value->string functions
    //
    void obj_raw(const char *key, const char *rawtext);

    void arr_raw(const char *rawtext);

    char *getBuffer()
    {
        return buffer;
    }

    uint16_t getBufferLen()
    {
        return strlen(buffer);
    }

    uint16_t getBufferSize()
    {
        return buflen;
    }

    void putraw(EmbyLibs::String &st);

private:
    // Variables:
    char *buffer;     // pointer to application's buffer
    uint16_t buflen;     // length of buffer
    char *bufp;       // current write position in buffer
    char tmpbuf[32]; // local buffer for int/double convertions
    Error error;      // error code
    int callNo;     // API call on which error occurred
    struct jwNodeStack
    {
        NodeType nodeType;
        int16_t elementNo;
    } nodeStack[JWRITE_STACK_DEPTH]; // stack of array/object nodes
    int16_t stackpos;
    Type mode; // 1= pretty output (inserts \n and spaces)
    // private methods:
    void putch(const char c);

    void putstr(const char *str);

    void putstr(const char *str, int len);

    void putstr(EmbyLibs::String &st);

    void putraw(const char *str);

    void modp_itoa10(int32_t value, char *str);

    void modp_dtoa2(double value, char *str, int prec);

    void pretty();

    JsonWriter::NodeType pop();

    void push(JsonWriter::NodeType nodeType);

    Error _jwObj(const char *key);

    Error _jwArr();
};

/* end of JsonWriter.hpp */
