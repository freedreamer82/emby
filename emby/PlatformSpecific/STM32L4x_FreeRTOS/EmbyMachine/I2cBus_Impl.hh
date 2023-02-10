/**
 * @addtogroup EmbyMachine
 * 
 * @{
 * @file EmbyMachine/I2cBus_Impl.hh
 * @author apagani
 * @version 1.0
 * @date Jan 15, 2015
 */

#if !defined( EMBYMACHINE_I2CBUS_IMPL_HH )
#define EMBYMACHINE_I2CBUS_IMPL_HH

extern "C"
{
#include "stm32l4xx.h"
}

namespace EmbyMachine
{

    struct I2cBus_Impl
    {
        //I2C_MemMapPtr m_baseReg;
    	I2C_HandleTypeDef m_handle;
    };
}



#endif // EMBYMACHINE_I2CBUS_IMPL_HH

