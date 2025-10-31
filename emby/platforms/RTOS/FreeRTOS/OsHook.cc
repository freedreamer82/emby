//#ifndef OSHOOK_HH
//#define OSHOOK_HH
#include <stdint.h>

extern "C"
{
        //vApplicationStackOverflowHook( ( TaskHandle_t ) pxCurrentTCB, pxCurrentTCB->pcTaskName )
        //{

        //}
        void vApplicationStackOverflowHook(void) {
                #ifdef SAIL_BUILD_DEBUG
                          while(1)
                          {

                          }
                #endif
        }

        void vApplicationIdleHook(void) {
            //Low Power in idle task.
            __asm volatile("dsb");
            __asm volatile("wfi");
            __asm volatile("isb");
        }

        void vApplicationTickHook(void) {

        }

        void vApplicationMallocFailedHook() {
        #ifdef SAIL_BUILD_DEBUG
            while(1)
            {

            }
        #endif
        }
}
//#endif
