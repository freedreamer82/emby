#include "MainWorker.hh"
#include <EmbyConsole/ConsoleWorker.hh>
#include <EmbyConsole/ConsoleUart.hh>
#include <EmbyLog/LogLevel.hh>
#include <EmbyMachine/Serial.hh>
#include "Console/ConsoleCommands.hh"
#include <EmbyLog/Log.hh>

#ifdef EMBY_BUILD_X86
#include <EmbyConsole/ConsoleTelnet.hh>
#include <EmbyLog/StdLogProcessor.hh>
#include <cstdio>
#endif

EmbyLog_MODULE_LOG_CLASS("main");

using namespace EmbyLibs;
using namespace EmbySystem;


bool MainWorker::onErrors(EmbySystem::ErrorCode *err, EmbySystem::SystemError::Status status)
{
    auto error = err->getDescription();
    log_error("This is fake error: @%d, %d ,%s", err->getTime(), int(err->getCode()), error.c_str());
    return false;
}

MainWorker::MainWorker() : m_thread(this, "Main", STACK_SIZE, PRIORITY)
{
    EmbySystem::SystemError::SystemErrorCallback cb;
    cb.attach(this, &MainWorker::onErrors);
    SystemError::get().setErrorCallback(cb);
    m_thread.start();
}

MainWorker::~MainWorker()
{
}

#ifdef EMBY_BUILD_ARM
#define CONSOLE_UART "uart3"

static void processArm()
{
    EmbyMachine::Serial::Serial_Config uartConfig;
    uartConfig.baudRate = EmbyMachine::Serial::Serial_BaudRate::Serial_BaudRate_115200;
    uartConfig.flowCtrl = EmbyMachine::Serial::Serial_FlowCtrl::Serial_FlowCtrl_None;
    uartConfig.mode = EmbyMachine::Serial::Serial_Mode::Serial_Mode_TxRx;
    uartConfig.parity = EmbyMachine::Serial::Serial_Parity::Serial_Parity_None;
    uartConfig.stopBits = EmbyMachine::Serial::Serial_StopBits::Serial_StopBits_1;
    uartConfig.wordLen = EmbyMachine::Serial::Serial_WordLen::Serial_WordLen_8;
    static ConsoleUart console = ConsoleUart(EmbyLog::logMaskFrom(EmbyLog::LogLevel::Debug), CONSOLE_UART, &uartConfig);

    console.askLogin(true);
    static ConsoleCommandsProject::Context cc;
    static ConsoleCommandsProject appCommands(cc);
    console.setApplicationCommands(&appCommands);

#ifdef EMBY_RTOS
    ConsoleWorker::get().addConsole(&console);
#endif

    while (true)
    {
#ifdef EMBY_RTOS
        console.doStep();
#endif
        EmbySystem::delayMs(50);
    }
}

#elif EMBY_BUILD_X86

int static testSerials()
{
    // Configurazione seriale
    EmbyMachine::Serial::Serial_Config config;
    config.baudRate = EmbyMachine::Serial::Serial_BaudRate::Serial_BaudRate_115200;
    config.flowCtrl = EmbyMachine::Serial::Serial_FlowCtrl::Serial_FlowCtrl_None;
    config.mode = EmbyMachine::Serial::Serial_Mode::Serial_Mode_TxRx;
    config.parity = EmbyMachine::Serial::Serial_Parity::Serial_Parity_None;
    config.stopBits = EmbyMachine::Serial::Serial_StopBits::Serial_StopBits_1;
    config.wordLen = EmbyMachine::Serial::Serial_WordLen::Serial_WordLen_8;

    // Usa una sola seriale di test (loopback locale)
    EmbyMachine::Serial serial("/dev/ttyTest1", &config, true);

    // Flush iniziale
    serial.flush();

    // Test: scrivi e poi leggi sulla stessa porta
    EmbyLibs::String testMsg = EmbyLibs::String("HelloSerial");
    serial.write(testMsg);
    log_info("Messaggio inviato su serial: %S", testMsg);

    EmbySystem::delayMs(100); // Attendi propagazione
    EmbyLibs::String received = serial.readline(5000); // Timeout 2s
    log_info("Messaggio ricevuto su serial: %S", received);

    // Verifica
    if (received == "HelloSerial") {
        log_info("TEST OK: readline funziona!");
        return 0;
    } else {
        log_info("TEST FAIL: messaggio non corrisponde!->%S !!!",received);
        return 1;
    }
}


#define CONSOLE_UART "/dev/ttyTest"

static void processx86()
{
    static StdLogProcessor logger(EmbyLog::logMaskFrom(EmbyLog::LogLevel::Info));
    EmbyLog::LogBus::get().registerProcessor(&logger);
    
    static EmbyConsole::ConsoleTelnet console = EmbyConsole::ConsoleTelnet(EmbyLog::logMaskFrom(EmbyLog::LogLevel::Debug), 3000, 2);
    
    static ConsoleCommandsProject::Context cc;
    static ConsoleCommandsProject appCommands(cc);
    console.setApplicationCommands(&appCommands);
    //console.start();
    log_info("Starting telnet localhost:3000");


    EmbyMachine::Serial::Serial_Config uartConfig;
    uartConfig.baudRate = EmbyMachine::Serial::Serial_BaudRate::Serial_BaudRate_115200;
    uartConfig.flowCtrl = EmbyMachine::Serial::Serial_FlowCtrl::Serial_FlowCtrl_None;
    uartConfig.mode = EmbyMachine::Serial::Serial_Mode::Serial_Mode_TxRx;
    uartConfig.parity = EmbyMachine::Serial::Serial_Parity::Serial_Parity_None;
    uartConfig.stopBits = EmbyMachine::Serial::Serial_StopBits::Serial_StopBits_1;
    uartConfig.wordLen = EmbyMachine::Serial::Serial_WordLen::Serial_WordLen_8;
    static ConsoleUart consoleserial = ConsoleUart(EmbyLog::logMaskFrom(EmbyLog::LogLevel::Debug), CONSOLE_UART, &uartConfig);

    ConsoleWorker::get().addConsole(&consoleserial);
    log_info("Starting serial console on %s", CONSOLE_UART);

    testSerials();
    int errcode = 0;
    while (true)
    {
        EmbySystem::delayMs(10);
    }
}
#endif

[[noreturn]] void MainWorker::doWork()
{
#ifdef EMBY_BUILD_ARM
    processArm();
#elif EMBY_BUILD_X86
    processx86();
#endif

    EmbySystem::reboot();
}
