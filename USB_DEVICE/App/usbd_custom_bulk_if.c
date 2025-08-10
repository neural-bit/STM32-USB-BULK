
/* Includes ------------------------------------------------------------------*/
#include <usbd_custom_bulk_if.h>
#include <math.h>
#include <stdint.h>
#include <usbd_custom_bulk.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"


int is_streaming = 0;


extern USBD_HandleTypeDef hUsbDeviceFS;






int8_t CUSTOM_BULK_Send(uint8_t* Buf, uint16_t Len)
{
    return USBD_CUSTOM_BULK_SendData(&hUsbDeviceFS, Buf, Len);
}



/** Usb HID report descriptor. */
__ALIGN_BEGIN static uint8_t CUSTOM_BULK_ReportDesc_FS[USBD_CUSTOM_BULK_REPORT_DESC_SIZE] __ALIGN_END =
{
		 /* USER CODE BEGIN 1 */
				  0x06,0x00,0xFF,         /*  Usage Page (vendor defined) ($FF00) global */
				  0x09,0x01,              /*  Usage (vendor defined) ($01) local */
				  0xA1,0x01,              /*  Collection (Application) */
				  0x15,0x00,              /*   LOGICAL_MINIMUM (0) */
				  0x25,0xFF,              /*   LOGICAL_MAXIMUM (255) */
				  0x75,0x08,              /*   REPORT_SIZE (8bit) */

				  // Input Report
				  //0x96,0x00,0x04,         /*   Report Length (1024 REPORT_SIZE) */
				  0x95,0x40,         	  /*   Report Length (64 REPORT_SIZE) */
				  0x09,0x01,              /*   USAGE (Vendor Usage 1) */
				  0x81,0x02,              /*   Input(data,var,absolute) */

				  // Output Report
				  //0x96,0x00,0x04,         /*   Report Length (1024 REPORT_SIZE) */
				  0x95,0x40,         	  /*   Report Length (64 REPORT_SIZE) */
				  0x09,0x01,              /*   USAGE (Vendor Usage 1) */
				  0x91,0x02,              /*   Output(data,var,absolute) */

				  // Feature Report
				  //0x96,0x00,0x04,         /*   Report Length (1024 REPORT_SIZE) */
				  0x95,0x40,         	  /*   Report Length (64 REPORT_SIZE) */
				  0x09,0x01,              /*   USAGE (Vendor Usage 1) */
				  0xB1,0x02,              /*   Feature(data,var,absolute) */
		  /* USER CODE END 1 */
		   0xC0    /*     END_COLLECTION             */
};


extern USBD_HandleTypeDef hUsbDeviceFS;


static int8_t CUSTOM_BULK_Init_FS(void);
static int8_t CUSTOM_BULK_DeInit_FS(void);
static int8_t CUSTOM_BULK_OutEvent_FS(uint8_t event_idx, uint8_t state);



USBD_CUSTOM_BULK_ItfTypeDef USBD_CustomBULK_fops_FS =
{
  CUSTOM_BULK_ReportDesc_FS,
  CUSTOM_BULK_Init_FS,
  CUSTOM_BULK_DeInit_FS,
  CUSTOM_BULK_OutEvent_FS
};


static int8_t CUSTOM_BULK_Init_FS(void)
{
  /* USER CODE BEGIN 4 */
  return (USBD_OK);
  /* USER CODE END 4 */
}


static int8_t CUSTOM_BULK_DeInit_FS(void)
{
  /* USER CODE BEGIN 5 */
  return (USBD_OK);
  /* USER CODE END 5 */
}





static int8_t CUSTOM_BULK_OutEvent_FS(uint8_t event_idx, uint8_t state)
{
  /* USER CODE BEGIN 6 */
  UNUSED(event_idx);
  UNUSED(state);

  USBD_CUSTOM_BULK_HandleTypeDef *hhid = (USBD_CUSTOM_BULK_HandleTypeDef*)hUsbDeviceFS.pClassData;
  uint8_t *buf = hhid->Report_buf;

  if (strncmp((char*)buf, "START", 5) == 0)
  {
	  is_streaming = 1;

  }
  else if (strncmp((char*)buf, "STOP", 4) == 0)
  {
	  is_streaming = 0;

	  //USBD_CUSTOM_BULK_SendData(&hUsbDeviceFS, usb_tx_buf2, 10);

  }


  // Send response
  //USBD_CUSTOM_BULK_SendReport(&hUsbDeviceFS, (uint8_t*)response, strlen(response));


  /* Start next USB packet transfer once data processing is completed */
  if (USBD_CUSTOM_BULK_ReceivePacket(&hUsbDeviceFS) != (uint8_t)USBD_OK)
  {
    return -1;
  }

  return (USBD_OK);
  /* USER CODE END 6 */
}



