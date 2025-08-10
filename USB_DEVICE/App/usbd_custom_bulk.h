
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_CUSTOMBULK_H
#define __USB_CUSTOMBULK_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include  "usbd_ioreq.h"


USBD_StatusTypeDef USBD_CUSTOM_BULK_SendData(USBD_HandleTypeDef *pdev, uint8_t* Buf, uint16_t Len);



#ifndef CUSTOM_BULK_EPIN_ADDR
#define CUSTOM_BULK_EPIN_ADDR                         0x81U
#endif /* CUSTOM_BULK_EPIN_ADDR */

#ifndef CUSTOM_BULK_EPIN_SIZE
#define CUSTOM_BULK_EPIN_SIZE                         64U
#endif /* CUSTOM_BULK_EPIN_SIZE */

#ifndef CUSTOM_BULK_EPOUT_ADDR
#define CUSTOM_BULK_EPOUT_ADDR                        0x01U
#endif /* CUSTOM_BULK_EPOUT_ADDR */

#ifndef CUSTOM_BULK_EPOUT_SIZE
#define CUSTOM_BULK_EPOUT_SIZE                        64U
#endif /* CUSTOM_BULK_EPOUT_SIZE*/

#define USB_CUSTOM_BULK_CONFIG_DESC_SIZ               41U
#define USB_CUSTOM_BULK_DESC_SIZ                      9U

#ifndef CUSTOM_BULK_HS_BINTERVAL
#define CUSTOM_BULK_HS_BINTERVAL                      0x05U
#endif /* CUSTOM_BULK_HS_BINTERVAL */

#ifndef CUSTOM_BULK_FS_BINTERVAL
#define CUSTOM_BULK_FS_BINTERVAL                      0x00U
#endif /* CUSTOM_BULK_FS_BINTERVAL */

#ifndef USBD_CUSTOMBULK_OUTREPORT_BUF_SIZE
#define USBD_CUSTOMBULK_OUTREPORT_BUF_SIZE            64U
#endif /* USBD_CUSTOMBULK_OUTREPORT_BUF_SIZE */

#ifndef USBD_CUSTOM_BULK_REPORT_DESC_SIZE
#define USBD_CUSTOM_BULK_REPORT_DESC_SIZE             163U
#endif /* USBD_CUSTOM_BULK_REPORT_DESC_SIZE */

#define CUSTOM_BULK_DESCRIPTOR_TYPE                   0x21U
#define CUSTOM_BULK_REPORT_DESC                       0x22U

#define CUSTOM_BULK_REQ_SET_PROTOCOL                  0x0BU
#define CUSTOM_BULK_REQ_GET_PROTOCOL                  0x03U

#define CUSTOM_BULK_REQ_SET_IDLE                      0x0AU
#define CUSTOM_BULK_REQ_GET_IDLE                      0x02U

#define CUSTOM_BULK_REQ_SET_REPORT                    0x09U
#define CUSTOM_BULK_REQ_GET_REPORT                    0x01U


typedef enum
{
  CUSTOM_BULK_IDLE = 0U,
  CUSTOM_BULK_BUSY,
} CUSTOM_BULK_StateTypeDef;

typedef struct _USBD_CUSTOM_BULK_Itf
{
  uint8_t *pReport;
#ifdef USBD_CUSTOMBULK_REPORT_DESC_SIZE_ENABLED
  uint16_t wReportDescLen;
#endif /* USBD_CUSTOMBULK_REPORT_DESC_SIZE_ENABLED */
  int8_t (* Init)(void);
  int8_t (* DeInit)(void);
#ifdef USBD_CUSTOMBULK_REPORT_BUFFER_EVENT_ENABLED
  int8_t (* OutEvent)(uint8_t *report_buffer);
#else
  int8_t (* OutEvent)(uint8_t event_idx, uint8_t state);
#endif /* USBD_CUSTOMBULK_REPORT_BUFFER_EVENT_ENABLED */
#ifdef USBD_CUSTOMBULK_CTRL_REQ_COMPLETE_CALLBACK_ENABLED
  int8_t (* CtrlReqComplete)(uint8_t request, uint16_t wLength);
#endif /* USBD_CUSTOMBULK_CTRL_REQ_COMPLETE_CALLBACK_ENABLED */
#ifdef USBD_CUSTOMBULK_CTRL_REQ_GET_REPORT_ENABLED
  uint8_t *(* GetReport)(uint16_t *ReportLength);
#endif /* USBD_CUSTOMBULK_CTRL_REQ_GET_REPORT_ENABLED */
} USBD_CUSTOM_BULK_ItfTypeDef;

typedef struct
{
  uint8_t  Report_buf[USBD_CUSTOMBULK_OUTREPORT_BUF_SIZE];
  uint32_t Protocol;
  uint32_t IdleState;
  uint32_t AltSetting;
  uint32_t IsReportAvailable;
  CUSTOM_BULK_StateTypeDef state;
} USBD_CUSTOM_BULK_HandleTypeDef;

/*
 * HID Class specification version 1.1
 * 6.2.1 HID Descriptor
 */

typedef struct
{
  uint8_t           bLength;
  uint8_t           bDescriptorTypeCHID;
  uint16_t          bcdCUSTOM_BULK;
  uint8_t           bCountryCode;
  uint8_t           bNumDescriptors;
  uint8_t           bDescriptorType;
  uint16_t          wItemLength;
} __PACKED USBD_DescTypeDef;




extern USBD_ClassTypeDef USBD_CUSTOM_BULK;
#define USBD_CUSTOM_BULK_CLASS &USBD_CUSTOM_BULK




#ifdef USE_USBD_COMPOSITE
uint8_t USBD_CUSTOM_BULK_SendReport(USBD_HandleTypeDef *pdev,
                                   uint8_t *report, uint16_t len, uint8_t ClassId);
#else
uint8_t USBD_CUSTOM_BULK_SendReport(USBD_HandleTypeDef *pdev,
                                   uint8_t *report, uint16_t len);
#endif /* USE_USBD_COMPOSITE */
uint8_t USBD_CUSTOM_BULK_ReceivePacket(USBD_HandleTypeDef *pdev);

uint8_t USBD_CUSTOM_BULK_RegisterInterface(USBD_HandleTypeDef *pdev,
                                          USBD_CUSTOM_BULK_ItfTypeDef *fops);



#ifdef __cplusplus
}
#endif

#endif  /* __USB_CUSTOMBULK_H */

