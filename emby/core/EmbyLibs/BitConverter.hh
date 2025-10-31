/**
 * @addtogroup EmbyLibs
 * @{
 * @file BitConverter.hh
 * @author Garzola Marco
 *
 * @brief
 * @version 2.0
 * @date 14/11/2014
 *
 */

#ifndef BitConverter_H
#define BitConverter_H

#include <climits>
#include <stdint.h>

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/


namespace EmbyLibs
{

	template <typename T>
	T swapEndian(T u)
	{
		static_assert (CHAR_BIT == 8, "CHAR_BIT != 8");

		union
		{
			T u;
			unsigned char u8[sizeof(T)];
		} source, dest;

		source.u = u;

		for (size_t k = 0; k < sizeof(T); k++)
			dest.u8[k] = source.u8[sizeof(T) - k - 1];

		return dest.u;
	}
}


#endif
