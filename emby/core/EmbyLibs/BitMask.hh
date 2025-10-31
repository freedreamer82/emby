/**
 * @addtogroup EmbyLibs
 * @{
 * @file BitMask.hh
 * @author Garzola Marco
 *
 * @brief Simple Bitmask management : Bit Index start from 0.
 * @version 2.0
 * @date 14/11/2014
 *
 */

#ifndef BitMask_H
#define BitMask_H

#include <stdint.h>

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/


namespace EmbyLibs
{

#define   BitMask_MAX_ALL_SET_MASK            0xFFFFFFFFFFFFFFFF  //8 byte FF
#define BitMask_MAX_ALL_SET_MASK            0xFFFFFFFFFFFFFFFF  //8 byte FF

#define BitMask_BIT(b)                      (1 << (b))

#define BitMask_GET_BIT(arg, bitNum)        ((arg) & BitMask_BIT(bitNum))
#define BitMask_GET_MASK(arg, mask)         ((arg) & (mask))
#define BitMask_DIFF_MASK(arg, mask)        ((arg) ^ (mask))
#define BitMask_SET_BIT(arg, bitNum)        ((arg) |=  BitMask_BIT(bitNum))
#define BitMask_SET_MASK(arg, mask)         ((arg) |= (mask))
#define BitMask_CLR_BIT(arg, bitNum)        ((arg) &= ~BitMask_BIT(bitNum))
#define BitMask_CLR_MASK(arg, mask)         ((arg) &= ~(mask))
#define BitMask_FLIP_BIT(arg, bitNum)       ((arg) ^=  BitMask_BIT(bitNum))
#define BitMask_FLIP_MASK(arg, mask)        ((arg) ^= (mask))
#define BitMask_IS_EMPTY(arg)               ((arg) == 0)
#define BitMask_CHK_ANY_FLAG(arg, mask)     (((arg) & (mask)) != 0)
#define BitMask_CHK_ALL_FLAGS(arg, mask)    (((arg) & (mask)) == (mask))
#define BitMask_DOWN_SHIFT(arg, shift)      ((arg) >> (shift))
#define BitMask_UP_SHIFT(arg, shift)        ((arg) << (shift))
#define BitMask_SET_ALL(mask)\
    ((mask) |= (BitMask_MAX_ALL_SET_MASK))
#define BitMask_CLEAR_ALL(mask)\
    ((mask) &= (~BitMask_MAX_ALL_SET_MASK))

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

}
#endif /* BitMask_H */
