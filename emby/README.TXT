
/***************************************************************************
*   
*                           -----     EMBY     ----
*
*
*                                Embedded Why? -
*
*
*    Emby is a C++ Framework developed with the purpose to be used in both
*    Embedded and x86 environment.
*
*       
*    Author : Marco        Garzola
*    Author : Francesco    Cusumano
*
*    Forked from a work designed with Masssimiliano Pagani
*       
/***************************************************************************





Changelog:

Version 0.1
        - Added more class and made it compilable on Linux x86 machine and Freertos on ARM


cmake -DEMBY_FOLDER=$PWD/emby  -DEMBY_CONFIG=src/emby_config.h -DEMBY_PLATFORM=X86_Linux .
cmake -DEMBY_FOLDER=$PWD/emby  -DEMBY_CONFIG=src/emby_config.h -DSTM32L4x_FreeRTOS_DEVICE=STM32L431xx -DEMBY_PLATFORM=STM32L4x_FreeRTOS  -DSTM32L4x_FreeRTOS_STARTUP=${PWD}/src/stm32l431cc/startup_stm32l431cctx.S -DSTM32L4x_FreeRTOS_CONF_DIR=${PWD}/src/stm32l431cc/  -DSTM32L4x_FreeRTOS_LINKER=${PWD}/src/stm32l431cc/STM32L431CCTX_FLASH.ld .