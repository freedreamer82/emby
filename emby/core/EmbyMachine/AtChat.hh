#if !defined( AT_CHAT_HH)
#define AT_CHAT_HH

#include <EmbyLibs/CircularBuffer.hh>
#include <EmbyLibs/String.hh>
#include <EmbyThreading/Thread.hh>
#include <EmbyThreading/Worker.hh>
#include <EmbyThreading/Mutex.hh>

#include <cstring>

using namespace EmbyLibs;

typedef class AtChat AtChat;

class AtCommandParser
{
public:
    virtual ~AtCommandParser() = default;

    enum class AtParserResult : uint8_t
    {
        AtParserResult_HANDLED,                /**< line has been processed*/
        AtParserResult_UNHANDLED,              /**< line has not been used*/
        AtParserResult_TERMINATE_SUCCESS,      /**< the command parser completed with success*/
        AtParserResult_OK_SWITCH_2_DATA_MODE,  /**< ok, change to Data mode before leaving Cmd processor*/
        AtParserResult_TERMINATE_FAILURE,     /**< the command parser completed with failure*/
    };

    void setContext(void *context)
    {
        m_context = context;
    }

    virtual AtCommandParser::AtParserResult handler(AtChat &chat, String &command) = 0;

protected:
    void *m_context;
};

class AtChat : protected EmbyThreading::Worker
//				   public EmbyExec::Listener<EmbyMachine::Serial>
{
public:

    struct AtChat_RetryPolicy
    {
        int16_t triesNo;
        int timeoutSec;
        int delayMsBetwTries; //if  100 ms by default
    };

    static uint16_t constexpr
    DEFAULT_TIMEOUT_MS = 800;

    static constexpr const char *EOL_LF = "\n";
    static constexpr const char *EOL_CLCR = "\r\n";
    static constexpr const char *EOL_CR = "\n";

    enum class AtResponse : uint8_t
    {
        AtResponse_OK = 0,
        AtResponse_OK_SWITCHED_2_DATA_MODE,
        AtResponse_ERROR,
        AtResponse_TIMEOUT,
        AtResponse_UNKNOWN
    };

    enum class AtChat_Mode : uint8_t
    {
        AtChat_AT_MODE,
        AtChat_AT_MODE_IDLE,
        AtChat_DATA_MODE,
        AtChat_MODE_UNKNOWN
    };

    virtual ~AtChat()
    {};

    AtResponse cmdProcessor(String &cmd, AtCommandParser *parser, size_t timeoutMs = DEFAULT_TIMEOUT_MS,
                            bool callAfterEcho = false);

    AtResponse cmdProcessor(char const *cmd, AtCommandParser *parser, size_t timeoutMs = DEFAULT_TIMEOUT_MS,
                            bool callAfterEcho = false);

    bool isAtEscapeReply(String &reply);

    bool isAtEscapeErrorReply(String &reply);

    EmbyThreading::Mutex &
    getMutex()
    {
        return m_mutex;
    }

    virtual size_t
    write(EmbyLibs::String &text);

    virtual size_t
    write(void const *buffer, size_t length) = 0;

    virtual void
    write(char const *text)
    {
        write(text, strlen(text));
    }

    virtual size_t
    read(void *buffer, size_t bufferSize) = 0;

    bool isEchoEnabled();


protected:


    AtChat(const char *&eol, bool urcThread = false);

    void notifyURC(String &msg);

    bool enableEcho();

    bool disableEcho();

    virtual void doWork();

    virtual EmbyLibs::String readline(int32_t timeoutMs = 0) = 0;

    virtual bool isDeviceOpen() = 0;

    virtual void flush() = 0;

    static size_t const STACK_SIZE = 400;
    static auto const PRIORITY =  EmbyThreading::Thread::Priority::VeryLow;

    AtChat &operator=(AtChat const &copy);

    void parseCommand(String const &command);

    bool
    handleParser(AtCommandParser *parser);

    EmbyThreading::Mutex m_mutex;
    EmbyThreading::Thread *m_thread;
    bool m_echo;
    AtChat_Mode m_mode;
    String m_eol;
    //EmbyLibs::CircularBuffer<uint8_t,128> m_buff;
};

#endif
