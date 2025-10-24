/**
 * @addtogroup EmbyMachine
 * @{
 * @file EmbyMachine/Gpio.hh
 * @author Marco Garzola
 * @version 1.0
 * @date 26/05/2016
 *
 */

#if !defined( EMBYMACHINE_GPIODRIVER_HH )
#define EMBYMACHINE_GPIODRIVER_HH

#include <EmbyLibs/Callback.hh>
#include <cstdint>

namespace EmbyMachine
{
	class Gpio
	{
		public:
			enum class Port: uint8_t
			{
				PortA = 0x00,
				PortB = 0x01,
				PortC = 0x02,
				PortD = 0x03,
				PortE = 0x04,
				PortF = 0x05,
				PortG = 0x06,
				PortH = 0x07,
				PortLast
			};

			enum class Direction: uint8_t
			{
				Input   = 0x00,
				Output  = 0x01,
                                InputPullUp     = 0x02,
                                InputPullDown   = 0x03,
                                OutputPullup    = 0x04,
                                OutputPullDown   = 0x05,
				Unknown
			};

			enum class Level: uint8_t
			{
				Low  = 0x00,
				High = 0x01
			};

			enum class InterruptEdge: uint8_t
			{
				Rising  = 0x00,
				Falling = 0x01,
				Both    = 0x02,
				None    = 0x03
			};

			static uint8_t constexpr PIN_MAX = 31;

			using InterruptCallback = EmbyLibs::Callback<void, Level>;

			Gpio(Port port,
				 uint8_t pin,
				 Direction direction = Direction::Input);

			~Gpio() = default;

			void setDirection(Direction dir);

			Direction getPinDirection();

			void toggleValue();

			Level getValue();

			bool setValue(Level);

			uint8_t getPinNo() const;

			Port getPort() const;

			bool setInterrupt(
					const InterruptCallback& callback,
					InterruptEdge edge = InterruptEdge::Rising);

			bool enableInterrupt() const;

			bool disableInterrupt() const;

			bool isInterruptEnabled() const;

		private:
			uint8_t m_pinNo;
			Port    m_port;
	};

    // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    // ::::::::::::::::::::::::::::::::::::::::::::::::::: inline methods :::
    // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

	inline uint8_t Gpio::getPinNo() const
	{
		return m_pinNo;
	}

	inline Gpio::Port Gpio::getPort() const
	{
		return m_port;
	}

} // end of namespace EmbyMachine

#endif
