/**
 * @addtogroup EmbyMachine/Kinetis/I2cBusImplementation.hh
 * @{
 * @file I2cBusImplementation.hh
 * @author apagani
 * @version 1.0
 * @date Jan 15, 2015
 */

#if !defined( EMBYMACHINE_LINUX_X86_I2C_IMPL_HH )
#define EMBYMACHINE_LINUX_X86_I2C_IMPL_HH

namespace EmbyMachine
{

    struct I2cBus_Impl
    {
        void* m_baseReg;
    };
}



#endif // EMBYMACHINE_I2CBUSIMPLEMENTATION_HH
