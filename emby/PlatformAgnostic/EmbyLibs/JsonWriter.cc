#include "JsonWriter.hh"
#include <EmbyLibs/StringUtils.hh>
namespace EmbyLibs
{
//------------------------------------------
// open
// - open writing of
// starting with rootType = JW_OBJECT or JW_ARRAY
// - initialise with user string buffer of length buflen
// - mode=JW_PRETTY adds \n and spaces to prettify output (else JW_COMPACT)
//
    void JsonWriter::open(JsonWriter::NodeType rootType, Type type)
    {
        memset(buffer, 0, buflen); // zap the whole destination buffer (all string terminators)
        bufp = buffer;
        nodeStack[0].nodeType = rootType;
        nodeStack[0].elementNo = 0;
        stackpos = 0;
        error = Error::Ok;
        callNo = 1;
        mode = type;
        putch((rootType == NodeType::Object) ? '{' : '[');
    }

//------------------------------------------
// close
// - closes the root JSON object started by open()
// - returns error code
//
    JsonWriter::Error JsonWriter::close()
    {
        if (error == Error::Ok)
        {
            if (stackpos == 0)
            {
                NodeType node = nodeStack[0].nodeType;
                if (mode == Type::JsonPretty)
                    putch('\n');
                putch((node == NodeType::Object) ? '}' : ']');
            }
            else
            {
                error = Error::NestError; // nesting error, not all objects closed when jwClose() called
            }
        }
        return error;
    }

//------------------------------------------
// End the current array/object
//
    JsonWriter::Error JsonWriter::end()
    {
        if (error == Error::Ok)
        {
            NodeType node;
            int lastElemNo = nodeStack[stackpos].elementNo;
            node = pop();
            if (lastElemNo > 0)
                pretty();
            putch((node == NodeType::Object) ? '}' : ']');
        }
        return error;
    }

//------------------------------------------
// errorPos
// - Returns position of error: the nth call to a JsonWriter function
//
    int JsonWriter::errorPos()
    {
        return callNo;
    }

//------------------------------------------
// Object insert functions
//

// put raw string to object (i.e. contents of rawtext without quotes)
//
    void JsonWriter::obj_raw(const char *key, const char *rawtext)
    {
        if (_jwObj(key) == Error::Ok)
        {
            putraw(rawtext);
        }
    }

// put "quoted" string to object
//
    void JsonWriter::obj_string(const char *key, EmbyLibs::String &st)
    {
        if (_jwObj(key) == Error::Ok)
        {
            putstr(st);
        }
    }

// put "quoted" string to object
//
    void JsonWriter::obj_string(const char *key, const char *value)
    {
        if (_jwObj(key) == Error::Ok)
        {
            putstr(value);
        }
    }

    void JsonWriter::obj_string(const char *key, const char *value, int len)
    {
        if (_jwObj(key) == Error::Ok)
        {
            putstr(value, len);
        }
    }

// put basic types to object...
//
    void JsonWriter::obj_int(const char *key, int value)
    {
        modp_itoa10(value, tmpbuf);
        obj_raw(key, tmpbuf);
    }

    void JsonWriter::obj_double(const char *key, double value)
    {
        modp_dtoa2(value, tmpbuf, 6);
        obj_raw(key, tmpbuf);
    }

    void JsonWriter::obj_float(const char *key, float value, int afterpoint)
    {
        EmbyLibs::float2String(value, tmpbuf, afterpoint);
        obj_raw(key, tmpbuf);
    }

    void JsonWriter::obj_bool(const char *key, int oneOrZero)
    {
        obj_raw(key, (oneOrZero) ? "true" : "false");
    }

    void JsonWriter::obj_null(const char *key)
    {
        obj_raw(key, "null");
    }

// put Object in Object
//
    void JsonWriter::obj_object(const char *key)
    {
        if (_jwObj(key) == Error::Ok)
        {
            putch('{');
            push(NodeType::Object);
        }
    }

// put Array in Object
//
    void JsonWriter::obj_array(const char *key)
    {
        if (_jwObj(key) == Error::Ok)
        {
            putch('[');
            push(NodeType::Array);
        }
    }

//------------------------------------------
// Array insert functions
//

// put raw string to array (i.e. contents of rawtext without quotes)
//
    void JsonWriter::arr_raw(const char *rawtext)
    {
        if (_jwArr() == Error::Ok)
        {
            putraw(rawtext);
        }
    }

// put "quoted" string to array
//
    void JsonWriter::arr_string(const char *value)
    {
        if (_jwArr() == Error::Ok)
        {
            putstr(value);
        }
    }

    void JsonWriter::arr_string(const char *value, int len)
    {
        if (_jwArr() == Error::Ok)
        {
            putstr(value, len);
        }
    }

    void JsonWriter::arr_string(EmbyLibs::String &st)
    {
        if (_jwArr() == Error::Ok)
        {
            putstr(st.data(), st.length());
        }
    }

    void JsonWriter::arr_int(int value)
    {
        modp_itoa10(value, tmpbuf);
        arr_raw(tmpbuf);
    }

    void JsonWriter::arr_double(double value)
    {
        modp_dtoa2(value, tmpbuf, 6);
        arr_raw(tmpbuf);
    }

    void JsonWriter::arr_float(float value, int afterpoint)
    {
        EmbyLibs::float2String(value, tmpbuf, afterpoint);
        arr_raw(tmpbuf);
    }

    void JsonWriter::arr_bool(int oneOrZero)
    {
        arr_raw((oneOrZero) ? "true" : "false");
    }

    void JsonWriter::arr_null()
    {
        arr_raw("null");
    }

    void JsonWriter::arr_object()
    {
        if (_jwArr() == Error::Ok)
        {
            putch('{');
            push(NodeType::Object);
        }
    }

    void JsonWriter::arr_array()
    {
        if (_jwArr() == Error::Ok)
        {
            putch('[');
            push(NodeType::Array);
        }
    }

//============================================================================
// Internal functions
//
    void JsonWriter::pretty()
    {
        int i;
        if (mode == Type::JsonPretty)
        {
            putch('\n');
            for (i = 0; i < stackpos + 1; i++)
                putraw("  ");
        }
    }

// Push / Pop node stack
//
    void JsonWriter::push(NodeType nodeType)
    {
        if ((stackpos + 1) >= JWRITE_STACK_DEPTH)
        {
            error = Error::StackFull; // array/object nesting > JWRITE_STACK_DEPTH
        }
        else
        {
            nodeStack[++stackpos].nodeType = nodeType;
            nodeStack[stackpos].elementNo = 0;
        }
    }

    JsonWriter::NodeType JsonWriter::pop()
    {
        NodeType retval = nodeStack[stackpos].nodeType;
        if (stackpos == 0)
        {
            error = Error::StackEmpty; // stack underflow error (too many 'end's)
        }
        else
        {
            stackpos--;
        }
        return retval;
    }

    void JsonWriter::putch(const char c)
    {
        if ((unsigned int) (bufp - buffer + 1) >= buflen)
        {
            error = Error::BufferFull;
        }
        else
        {
            *bufp++ = c;
        }
    }

// put string enclosed in quotes
//
    void JsonWriter::putstr(const char *str)
    {
        putch('\"');
        while (*str != '\0')
        {
            putch(*str++);
        }
        putch('\"');
    }

    void JsonWriter::putstr(const char *str, int len)
    {
        putch('\"');
        int i = 0;
        while (i < len)
        {
            putch(*str++);
            i++;
        }
        putch('\"');
    }

// put string enclosed in quotes
//
    void JsonWriter::putstr(EmbyLibs::String &st)
    {
        putch('\"');
        for (auto i = 0; i < st.length(); i++)
        {
            putch(st[i]);
        }
        putch('\"');
    }


// put string enclosed in quotes
//
    void JsonWriter::putraw(EmbyLibs::String &st)
    {
        volatile int len = st.length();
        for (auto i = 0; i < len; i++)
        {
            if (st.data())
            {
                putch(st[i]);
            }
        }
    }

// put raw string
//
    void JsonWriter::putraw(const char *str)
    {
        while (*str != '\0')
        {
            putch(*str++);
        }
    }

// *common Object function*
// - checks error
// - checks current node is OBJECT
// - adds comma if reqd
// - adds "key" :
//
    JsonWriter::Error JsonWriter::_jwObj(const char *key)
    {
        if (error == Error::Ok)
        {
            callNo++;
            if (nodeStack[stackpos].nodeType != NodeType::Object)
                error = Error::NotObject; // tried to write Object key/value into Array
            else if (nodeStack[stackpos].elementNo++ > 0)
                putch(',');
            pretty();
            putstr(key);
            putch(':');
            if (mode == Type::JsonPretty)
            {
                putch(' ');
            }
        }
        return error;
    }

// *common Array function*
// - checks error
// - checks current node is ARRAY
// - adds comma if reqd
//
    JsonWriter::Error JsonWriter::_jwArr()
    {
        if (error == Error::Ok)
        {
            callNo++;
            if (nodeStack[stackpos].nodeType != NodeType::Array)
            {
                error = Error::NotArray; // tried to write array value into Object
            }
            else if (nodeStack[stackpos].elementNo++ > 0)
            {
                putch(',');
            }
            pretty();
        }
        return error;
    }

//=================================================================
//
// modp value-to-string functions
// - modified for C89
//
// We use these functions as they are a lot faster than sprintf()
//
// Origin of these routines:
/*
 * <pre>
 * Copyright &copy; 2007, Nick Galbreath -- nickg [at] modp [dot] com
 * All rights reserved.
 * http://code.google.com/p/stringencoders/
 * Released under the bsd license.
 * </pre>
 */

    static void strreverse(char *begin, char *end)
    {
        char aux;
        while (end > begin)
            aux = *end, *end-- = *begin, *begin++ = aux;
    }

/** \brief convert an signed integer to char buffer
 *
 * \param[in] value
 * \param[out] buf the output buffer.  Should be 16 chars or more.
 */
    void JsonWriter::modp_itoa10(int32_t value, char *str)
    {
        char *wstr = str;
        // Take care of sign
        unsigned int uvalue = (value < 0) ? -value : value;
        // Conversion. Number is reversed.
        do
            *wstr++ = (char) (48 + (uvalue % 10));
        while (uvalue /= 10);
        if (value < 0)
            *wstr++ = '-';
        *wstr = '\0';

        // Reverse string
        strreverse(str, wstr - 1);
    }

/**
 * Powers of 10
 * 10^0 to 10^9
 */
    static const double pow10[] = {
            1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000};

/** \brief convert a floating point number to char buffer with a
 *         variable-precision format, and no trailing zeros
 *
 * This is similar to "%.[0-9]f" in the printf style, except it will
 * NOT include trailing zeros after the decimal point.  This type
 * of format oddly does not exists with printf.
 *
 * If the input value is greater than 1<<31, then the output format
 * will be switched exponential format.
 *
 * \param[in] value
 * \param[out] buf  The allocated output buffer.  Should be 32 chars or more.
 * \param[in] precision  Number of digits to the right of the decimal point.
 *    Can only be 0-9.
 */
    void JsonWriter::modp_dtoa2(double value, char *str, int prec)
    {
        /* if input is larger than thres_max, revert to exponential */
        const double thres_max = (double) (0x7FFFFFFF);
        int count;
        double diff = 0.0;
        char *wstr = str;
        int neg = 0;
        int whole;
        double tmp;
        uint32_t frac;

        /* Hacky test for NaN
         * under -fast-math this won't work, but then you also won't
         * have correct nan values anyways.  The alternative is
         * to link with libmath (bad) or hack IEEE double bits (bad)
         */
        if (!(value == value))
        {
            str[0] = 'n';
            str[1] = 'a';
            str[2] = 'n';
            str[3] = '\0';
            return;
        }

        if (prec < 0)
        {
            prec = 0;
        }
        else if (prec > 9)
        {
            /* precision of >= 10 can lead to overflow errors */
            prec = 9;
        }

        /* we'll work in positive values and deal with the
           negative sign issue later */
        if (value < 0)
        {
            neg = 1;
            value = -value;
        }

        whole = (int) value;
        tmp = (value - whole) * pow10[prec];
        frac = (uint32_t) (tmp);
        diff = tmp - frac;

        if (diff > 0.5)
        {
            ++frac;
            /* handle rollover, e.g.  case 0.99 with prec 1 is 1.0  */
            if (frac >= pow10[prec])
            {
                frac = 0;
                ++whole;
            }
        }
        else if (diff == 0.5 && ((frac == 0) || (frac & 1)))
        {
            /* if halfway, round up if odd, OR
               if last digit is 0.  That last part is strange */
            ++frac;
        }

        /* for very large numbers switch back to native sprintf for exponentials.
           anyone want to write code to replace this? */
        /*
          normal printf behavior is to print EVERY whole number digit
          which can be 100s of characters overflowing your buffers == bad
        */
        if (value > thres_max)
        {
            std::sprintf(str, "%e", neg ? -value : value);
            return;
        }

        if (prec == 0)
        {
            diff = value - whole;
            if (diff > 0.5)
            {
                /* greater than 0.5, round up, e.g. 1.6 -> 2 */
                ++whole;
            }
            else if (diff == 0.5 && (whole & 1))
            {
                /* exactly 0.5 and ODD, then round up */
                /* 1.5 -> 2, but 2.5 -> 2 */
                ++whole;
            }

            // vvvvvvvvvvvvvvvvvvv  Diff from modp_dto2
        }
        else if (frac)
        {
            count = prec;
            // now do fractional part, as an unsigned number
            // we know it is not 0 but we can have leading zeros, these
            // should be removed
            while (!(frac % 10))
            {
                --count;
                frac /= 10;
            }
            //^^^^^^^^^^^^^^^^^^^  Diff from modp_dto2

            // now do fractional part, as an unsigned number
            do
            {
                --count;
                *wstr++ = (char) (48 + (frac % 10));
            } while (frac /= 10);
            // add extra 0s
            while (count-- > 0)
                *wstr++ = '0';
            // add decimal
            *wstr++ = '.';
        }

        // do whole part
        // Take care of sign
        // Conversion. Number is reversed.
        do
            *wstr++ = (char) (48 + (whole % 10));
        while (whole /= 10);
        if (neg)
        {
            *wstr++ = '-';
        }
        *wstr = '\0';
        strreverse(str, wstr - 1);
    }
//=================================================================

/* end of JsonWriter.cpp */
}