#ifndef __USBD_CUSTOM_BULK_IF_H__
#define __USBD_CUSTOM_BULK_IF_H__

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <usbd_custom_bulk.h>


int8_t CUSTOM_BULK_Send(uint8_t* Buf, uint16_t Len);


extern USBD_CUSTOM_BULK_ItfTypeDef USBD_CustomBULK_fops_FS;


#ifdef __cplusplus
}
#endif

#endif /* __USBD_CUSTOM_BULK_IF_H__ */

