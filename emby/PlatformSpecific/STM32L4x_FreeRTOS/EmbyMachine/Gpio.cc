///**
// * @addtogroup EmbyMachine
// * @{
// * @file EmbyMachine/Kinetis/Gpio.cc
// * @author Marco  Garzola
// * @version 1.0
// * @date 22/09/2015
// *
// */
//
////#include <IO_Map.h>
//extern "C"
//{
//	#include <IO_Map.h>
//	#include "SIM_PDD.h"
//	#include <GPIO_PDD.h>
//	#include <PORT_PDD.h>
//
//}
//#include <EmbyMachine/Gpio.hh>
//#include <EmbyDebug/assert.hh>
//
//
//
//namespace EmbyMachine
//{
//
//	static void
//	_getPort(Gpio::GpioPort port, GPIO_MemMapPtr* base)
//    {
//        switch (port)
//        {
//        case Gpio::GPIO_PORTS_A:
//            *base = PTA_BASE_PTR;
//            break;
//        case Gpio::GPIO_PORTS_B:
//            *base = PTB_BASE_PTR;
//            break;
//        case Gpio::GPIO_PORTS_C:
//            *base = PTC_BASE_PTR;
//            break;
//        case Gpio::GPIO_PORTS_D:
//            *base = PTD_BASE_PTR;
//            break;
//        case Gpio::GPIO_PORTS_E:
//            *base = PTE_BASE_PTR;
//            break;
//        default:
//        	EmbyDebug_ASSERT(0);
//        }
//    }
//
//	static uint32_t
//	_getClockID(Gpio::GpioPort port)
//	{
//		uint32_t id = 0;
//		switch (port)
//		{
//		case Gpio::GPIO_PORTS_A:
//			id = SIM_PDD_CLOCK_GATE_PORTA;
//			break;
//		case Gpio::GPIO_PORTS_B:
//			id = SIM_PDD_CLOCK_GATE_PORTB;
//			break;
//		case Gpio::GPIO_PORTS_C:
//			id = SIM_PDD_CLOCK_GATE_PORTC;
//			break;
//		case Gpio::GPIO_PORTS_D:
//			id = SIM_PDD_CLOCK_GATE_PORTD;
//			break;
//		case Gpio::GPIO_PORTS_E:
//			id = SIM_PDD_CLOCK_GATE_PORTE;
//			break;
//		default:
//			EmbyDebug_ASSERT(0);
//		}
//		return id;
//	}
//
//    Gpio::Gpio(Gpio::GpioPort port, uint32_t pin) : m_pinNo(pin) , m_port(port)
//    {
//    	EmbyDebug_ASSERT(m_pinNo < 32);
//    	GPIO_MemMapPtr base;
//        _getPort(m_port,&base);
//
//        SIM_PDD_SetClockGate(SIM_BASE_PTR, _getClockID(m_port), PDD_ENABLE);
//
//        switch (m_port)
//		{
//		case Gpio::GPIO_PORTS_A:
//			PORT_PDD_SetPinMuxControl(PORTA_BASE_PTR, pin, PORT_PDD_MUX_CONTROL_ALT1);
//			break;
//		case Gpio::GPIO_PORTS_B:
//			PORT_PDD_SetPinMuxControl(PORTB_BASE_PTR, pin, PORT_PDD_MUX_CONTROL_ALT1);
//			break;
//		case Gpio::GPIO_PORTS_C:
//			PORT_PDD_SetPinMuxControl(PORTC_BASE_PTR, pin, PORT_PDD_MUX_CONTROL_ALT1);
//			break;
//		case Gpio::GPIO_PORTS_D:
//			PORT_PDD_SetPinMuxControl(PORTD_BASE_PTR, pin, PORT_PDD_MUX_CONTROL_ALT1);
//			break;
//		case Gpio::GPIO_PORTS_E:
//			PORT_PDD_SetPinMuxControl(PORTE_BASE_PTR, pin, PORT_PDD_MUX_CONTROL_ALT1);
//			break;
//		default:
//			EmbyDebug_ASSERT(0);
//		}
//
//    }
//
//    Gpio::~Gpio()
//    {
//    }
//
//    void
//	Gpio::setDirection(GpioDirection dir)
//    {
//    	EmbyDebug_ASSERT(m_pinNo < 32);
//
//    	GPIO_MemMapPtr base;
//    	_getPort(m_port,&base);
//
//    	if(dir == GPIO_PIN_INPUT)
//    	{
//    		GPIO_PDDR_REG(base) = GPIO_PDDR_REG(base) & ~(1U << m_pinNo);
//    	}
//    	else
//    	{
//    		GPIO_PDDR_REG(base) = GPIO_PDDR_REG(base) | (1U << m_pinNo);
//    	}
//    }
//
//    Gpio::GpioDirection
//   	Gpio::getPinDirection( )
//    {
//		EmbyDebug_ASSERT(m_pinNo < 32);
//
//		GPIO_MemMapPtr base;
//		_getPort(m_port,&base);
//
//		if( GPIO_PDDR_REG(base) & (1U << m_pinNo) )
//		{
//			return GPIO_PIN_OUTPUT;
//		}
//		else if (GPIO_PDDR_REG(base) & (1U << m_pinNo) )
//		{
//			return GPIO_PIN_INPUT;
//		}
//		return GPIO_PIN_UNKWNOWN;
//    }
//
//    void
//	Gpio::toggleValue( )
//    {
//    	EmbyDebug_ASSERT(m_pinNo < 32);
//
//        GPIO_MemMapPtr base;
//        _getPort(m_port,&base);
//
//        GPIO_PTOR_REG(base) |= (1U << m_pinNo);
//    }
//
//    Gpio::GpioLevel
//	Gpio::getValue( )
//    {
//    	EmbyDebug_ASSERT(m_pinNo < 32);
//
//        GPIO_MemMapPtr base;
//        _getPort(m_port,&base);
//
//        return (GpioLevel)((GPIO_PDOR_REG(base) >> m_pinNo) & 0x1U);
//    }
//
//    bool
//    Gpio::setValue(GpioLevel level)
//    {
//    	EmbyDebug_ASSERT(m_pinNo < 32);
//
//    	if( getPinDirection( ) == GPIO_PIN_INPUT )
//    	{
//    		return false;
//    	}
//
//		GPIO_MemMapPtr base;
//		_getPort(m_port,&base);
//
//		if(level == GPIO_LOW)
//		{
//			GPIO_PCOR_REG(base) |= (1U << m_pinNo);
//		}
//		else if ( level == GPIO_HIGH )
//		{
//			GPIO_PSOR_REG(base) |= (1U << m_pinNo);
//		}
//
//		return true;
//    }
//
//}  // end of namespace EmbyMachine
