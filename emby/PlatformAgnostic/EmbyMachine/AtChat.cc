#include "AtChat.hh"
#include "EmbyLibs/StringUtils.hh"

#include "EmbyLog/Log.hh"
#include <EmbyThreading/Thread.hh>
#include <EmbyThreading/ScopedMutex.hh>

using namespace EmbyLibs;
//using namespace AtCommandParser::AtParserResult;

EmbyLog_MODULE_LOG_CLASS("AT");


AtChat::AtChat(const char *&eol, bool urcThread) :
        m_mutex{}, m_thread(nullptr), m_eol{eol},
        m_echo(false),
        m_mode(AtChat_Mode::AtChat_AT_MODE_IDLE)
{
    if (urcThread)
    {
        m_thread = new EmbyThreading::Thread(this, "AtChat", STACK_SIZE, PRIORITY, false);
    }
}

void
AtChat::parseCommand(String const &command)
{

}

bool
AtChat::isEchoEnabled()
{
    EmbyThreading::ScopedMutex lock(m_mutex);

    if (isDeviceOpen())
    {
        write("AT");
        write(m_eol);
        String st = readline(200);
        trim(st);
        if (!st.empty())
        {
            if (!st.empty() && st == "AT")
            {
                m_echo = true;
                st = readline(200);
                flush();
            }
        }
        else
        {
            m_echo = false;

        }
    }

    return m_echo;
}


void
AtChat::doWork()
{
    EmbyThreading::ScopedMutex lock(m_mutex);

    if (isDeviceOpen())
    {
        if (m_mode == AtChat_Mode::AtChat_AT_MODE_IDLE)
        {
            String st = readline(100);
            trim(st);
            if (!st.empty())
            {
                log_trace(st);
            }
        }
    }
    EmbySystem::delayMs(40);
}

void
AtChat::notifyURC(String &msg)
{
    log_trace(msg);
}

bool AtChat::
isAtEscapeReply(String &reply)
{
    bool retval = false;
    if (!reply.empty())
    {
        if (doesStringBeginWith(reply, "OK"))
        {
            retval = true;
        }
        else if (doesStringBeginWith(reply, "ERROR") ||
                 doesStringBeginWith(reply, "+CME ERROR:") ||
                 doesStringBeginWith(reply, "+CMS ERROR:"))
        {
            retval = false;
        }
        else
        {
            retval = false;
        }

    }
    return retval;
}

bool AtChat::
isAtEscapeErrorReply(String &reply)
{
    bool retval = false;
    if (!reply.empty())
    {
        if (doesStringBeginWith(reply, "ERROR") ||
            doesStringBeginWith(reply, "+CME ERROR:") ||
            doesStringBeginWith(reply, "+CMS ERROR:"))
        {
            retval = true;
        }
        else
        {
            retval = false;
        }

    }
    return retval;
}

AtChat::AtResponse
AtChat::cmdProcessor(char const *cmd, AtCommandParser *parser, size_t timeoutMs, bool callAfterEcho)
{
    auto command = String(cmd);
    return cmdProcessor(command, parser, timeoutMs, callAfterEcho);
}

AtChat::AtResponse
AtChat::cmdProcessor(String &cmd, AtCommandParser *parser, size_t timeoutMs, bool callAfterEcho)
{
    EmbyThreading::ScopedMutex sm(m_mutex);

    if (!isDeviceOpen())
    {
        return AtChat::AtResponse::AtResponse_UNKNOWN;
    }

    AtCommandParser::AtParserResult result =
            AtCommandParser::AtParserResult::AtParserResult_UNHANDLED;

    trim(cmd);
    write(cmd);
    write(m_eol);

    bool isLineHandledByParser = false;
    bool isParsingCompleted = false;

    auto atResp = AtResponse::AtResponse_UNKNOWN;

    uint64_t start = EmbySystem::upTimeMs();

    do
    {
        auto reply = readline(200);
        trim(reply);
        if (!reply.empty())
        {
            log_trace(reply);
            if (m_mode == AtChat_Mode::AtChat_AT_MODE) /**Inside at cmd..means after an Echo*/
            {
                if (parser)
                {
                    result = parser->handler(*this, reply);

                    switch (result)
                    {
                        case AtCommandParser::AtParserResult::AtParserResult_HANDLED:           /*< line has been processed*/
                            isLineHandledByParser = true;
                            break;
                        case AtCommandParser::AtParserResult::AtParserResult_UNHANDLED:         /*< line has not been used*/
                            isLineHandledByParser = false;
                            break;
                        case AtCommandParser::AtParserResult::AtParserResult_TERMINATE_SUCCESS: /*< the command parser completed with success*/
                            isLineHandledByParser = true;
                            //	isParsingCompleted    = true;
                            //	atRes = Modem_ATCMD_INPROGRESS; /**Continue...*/

                            //					if (customAt!=NULL && customAt->type == Modem_AT_CUSTOM_MODE &&
                            //						customAt->customMode == Modem_AT_CMD_NO_REPLY)
                            //					{
                            //						atRes = Modem_ATCMD_OK;
                            //					}
                            break;
                        case AtCommandParser::AtParserResult::AtParserResult_TERMINATE_FAILURE:  /*< the command parser completed with failure*/
                            //atRes = Modem_ATCMD_INPROGRESS; /**Continue...*/
                            atResp = AtResponse::AtResponse_ERROR;
                            isParsingCompleted = true;
                            isLineHandledByParser = true;
                            break;
                        case AtCommandParser::AtParserResult::AtParserResult_OK_SWITCH_2_DATA_MODE:  /*< the command parser completed */
                            atResp = AtResponse::AtResponse_OK_SWITCHED_2_DATA_MODE;
                            isLineHandledByParser = true;
                            break;

                        default:
                            EmbyDebug_ASSERT_FAIL();
                            break;
                    }
                }
                if (!isLineHandledByParser)
                {
                    /**Line has not been handled or Parsing is concluded!->
                     Now check if is an AT end reply...*/
                    if (isAtEscapeReply(reply))
                    {
                        //do nothing
                        atResp = AtResponse::AtResponse_OK;
                        isParsingCompleted = true;
                    }
                        /* Could be a custom AT end reply...*/
                        //	else if (_isCustomEscapeReply(self, customAt))
                        //{
                        //	atRes = Modem_ATCMD_OK;
                        //}
                        /**Or could be a data mode Request*/
                    else if (doesStringBeginWith(reply, "CONNECT"))
                    {
                        atResp = AtResponse::AtResponse_OK_SWITCHED_2_DATA_MODE;
                        isParsingCompleted = true;
                    }
                    else /*Other unsolicited event received, handle separately*/
                    {
                        notifyURC(reply);
                    }
                }
            }
            else if (m_mode == AtChat_Mode::AtChat_AT_MODE_IDLE) /**Echo not arrived yet*/
            {
                /**Outside AT cmd*/
//#warning "if I compare the whole string sometimes it fail...do we miss chars????? fix ME!!"
                //	if( doesStringBeginWith(reply,cmd) )
                if (reply.find(cmd) != std::string::npos)
                    //	if (reply.compare(0, cmd.length()/2, cmd ))
                {
                    /**Is an echo*/
                    m_mode = AtChat_Mode::AtChat_AT_MODE;
                    if (callAfterEcho)
                    {
                        parser->handler(*this, reply);
                    }
                }
                else
                {
                    /** Mmmh.. something goes wrong , give a chance
                    *   to Unsolicited Handler */
                    notifyURC(reply);
                }
            }
        }

        if ((EmbySystem::upTimeMs() - start) > timeoutMs)
        {
            isParsingCompleted = true;
            atResp = AtResponse::AtResponse_TIMEOUT;
            break;
        }

        EmbySystem::delayMs(20);
    } while (!isParsingCompleted);


    if (m_mode == AtChat_Mode::AtChat_AT_MODE)
    {
        //just to be sure..
        m_mode = AtChat_Mode::AtChat_AT_MODE_IDLE;
    }


    return atResp;
}

size_t
AtChat::write(EmbyLibs::String &text)
{
    EmbyThreading::ScopedMutex sm(m_mutex);

    int i = 0;
    auto begin = text.begin();
    while (begin != text.end())
    {
        i += write(&(*begin), 1);
        ++begin;
    }

    return i;
}
