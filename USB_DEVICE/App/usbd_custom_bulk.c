
/* Includes ------------------------------------------------------------------*/
#include <usbd_custom_bulk.h>
#include "usbd_ctlreq.h"


static uint8_t USBD_CUSTOM_BULK_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t USBD_CUSTOM_BULK_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t USBD_CUSTOM_BULK_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);

static uint8_t USBD_CUSTOM_BULK_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum);
static uint8_t USBD_CUSTOM_BULK_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum);
static uint8_t USBD_CUSTOM_BULK_EP0_RxReady(USBD_HandleTypeDef  *pdev);
#ifndef USE_USBD_COMPOSITE
static uint8_t *USBD_CUSTOM_BULK_GetFSCfgDesc(uint16_t *length);
static uint8_t *USBD_CUSTOM_BULK_GetHSCfgDesc(uint16_t *length);
static uint8_t *USBD_CUSTOM_BULK_GetOtherSpeedCfgDesc(uint16_t *length);
static uint8_t *USBD_CUSTOM_BULK_GetDeviceQualifierDesc(uint16_t *length);
#endif /* USE_USBD_COMPOSITE  */


USBD_StatusTypeDef USBD_CUSTOM_BULK_SendData(USBD_HandleTypeDef *pdev, uint8_t* Buf, uint16_t Len)
{
    // Check if the USB device is configured and ready
    if (pdev->dev_state != USBD_STATE_CONFIGURED)
    {
        return USBD_FAIL;
    }

    // Transmit data to host using Bulk IN endpoint
    return USBD_LL_Transmit(pdev, CUSTOM_BULK_EPIN_ADDR, Buf, Len);
}


USBD_ClassTypeDef  USBD_CUSTOM_BULK =
{
  USBD_CUSTOM_BULK_Init,
  USBD_CUSTOM_BULK_DeInit,
  USBD_CUSTOM_BULK_Setup,
  NULL, /*EP0_TxSent*/
  USBD_CUSTOM_BULK_EP0_RxReady, /*EP0_RxReady*/ /* STATUS STAGE IN */
  USBD_CUSTOM_BULK_DataIn, /*DataIn*/
  USBD_CUSTOM_BULK_DataOut,
  NULL, /*SOF */
  NULL,
  NULL,
#ifdef USE_USBD_COMPOSITE
  NULL,
  NULL,
  NULL,
  NULL,
#else
  USBD_CUSTOM_BULK_GetHSCfgDesc,
  USBD_CUSTOM_BULK_GetFSCfgDesc,
  USBD_CUSTOM_BULK_GetOtherSpeedCfgDesc,
  USBD_CUSTOM_BULK_GetDeviceQualifierDesc,
#endif /* USE_USBD_COMPOSITE  */
};

#ifndef USE_USBD_COMPOSITE
/* USB CUSTOM_BULK device FS Configuration Descriptor */
__ALIGN_BEGIN static uint8_t USBD_CUSTOM_BULK_CfgDesc[USB_CUSTOM_BULK_CONFIG_DESC_SIZ] __ALIGN_END =
{
  0x09,                                               /* bLength: Configuration Descriptor size */
  USB_DESC_TYPE_CONFIGURATION,                        /* bDescriptorType: Configuration */
  LOBYTE(USB_CUSTOM_BULK_CONFIG_DESC_SIZ),             /* wTotalLength: Bytes returned */
  HIBYTE(USB_CUSTOM_BULK_CONFIG_DESC_SIZ),
  0x01,                                               /* bNumInterfaces: 1 interface */
  0x01,                                               /* bConfigurationValue: Configuration value */
  0x00,                                               /* iConfiguration: Index of string descriptor
                                                         describing the configuration */
#if (USBD_SELF_POWERED == 1U)
  0xC0,                                               /* bmAttributes: Bus Powered according to user configuration */
#else
  0x80,                                               /* bmAttributes: Bus Powered according to user configuration */
#endif /* USBD_SELF_POWERED */
  USBD_MAX_POWER,                                     /* MaxPower (mA) */



  /************** Descriptor of CUSTOM HID interface ****************/
  /* 09 */
  0x09,                                               /* bLength: Interface Descriptor size*/
  USB_DESC_TYPE_INTERFACE,                            /* bDescriptorType: Interface descriptor type */
  0x00,                                               /* bInterfaceNumber: Number of Interface */
  0x00,                                               /* bAlternateSetting: Alternate setting */
  0x02,                                               /* bNumEndpoints*/
  0xFF,                                               /* bInterfaceClass: CUSTOM_BULK */
  0x00,                                               /* bInterfaceSubClass : 1=BOOT, 0=no boot */
  0x00,                                               /* nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse */
  0x00,                                               /* iInterface: Index of string descriptor */

  /******************** Descriptor of CUSTOM_BULK *************************/
  /* 18 */
  0x09,                                               /* bLength: CUSTOM_BULK Descriptor size */
  CUSTOM_BULK_DESCRIPTOR_TYPE,                         /* bDescriptorType: CUSTOM_BULK */
  0x11,                                               /* bCUSTOM_BULKUSTOM_BULK: CUSTOM_BULK Class Spec release number */
  0x01,
  0x00,                                               /* bCountryCode: Hardware target country */
  0x01,                                               /* bNumDescriptors: Number of CUSTOM_BULK class descriptors
                                                         to follow */
  0x22,                                               /* bDescriptorType */
  LOBYTE(USBD_CUSTOM_BULK_REPORT_DESC_SIZE),           /* wItemLength: Total length of Report descriptor */
  HIBYTE(USBD_CUSTOM_BULK_REPORT_DESC_SIZE),



  /******************** Descriptor of Custom HID endpoints ********************/
  /* 27 */
  0x07,                                               /* bLength: Endpoint Descriptor size */
  USB_DESC_TYPE_ENDPOINT,                             /* bDescriptorType: */
  CUSTOM_BULK_EPIN_ADDR,                               /* bEndpointAddress: Endpoint Address (IN) */
  USBD_EP_TYPE_BULK,                                               /* bmAttributes: Interrupt endpoint */
  LOBYTE(CUSTOM_BULK_EPIN_SIZE),                       /* wMaxPacketSize: 2 Bytes max */
  HIBYTE(CUSTOM_BULK_EPIN_SIZE),
  CUSTOM_BULK_FS_BINTERVAL,                            /* bInterval: Polling Interval */
  /* 34 */

  0x07,                                               /* bLength: Endpoint Descriptor size */
  USB_DESC_TYPE_ENDPOINT,                             /* bDescriptorType: */
  CUSTOM_BULK_EPOUT_ADDR,                              /* bEndpointAddress: Endpoint Address (OUT) */
  USBD_EP_TYPE_BULK,                                  /* bmAttributes: Interrupt endpoint */
  LOBYTE(CUSTOM_BULK_EPOUT_SIZE),                      /* wMaxPacketSize: 2 Bytes max  */
  HIBYTE(CUSTOM_BULK_EPOUT_SIZE),
  CUSTOM_BULK_FS_BINTERVAL,                            /* bInterval: Polling Interval */
  /* 41 */
};
#endif /* USE_USBD_COMPOSITE  */

/* USB CUSTOM_BULK device Configuration Descriptor */
__ALIGN_BEGIN static uint8_t USBD_CUSTOM_BULK_Desc[USB_CUSTOM_BULK_DESC_SIZ] __ALIGN_END =
{
  /* 18 */
  0x09,                                               /* bLength: CUSTOM_BULK Descriptor size */
  CUSTOM_BULK_DESCRIPTOR_TYPE,                         /* bDescriptorType: CUSTOM_BULK */
  0x11,                                               /* bCUSTOM_BULKUSTOM_BULK: CUSTOM_BULK Class Spec release number */
  0x01,
  0x00,                                               /* bCountryCode: Hardware target country */
  0x01,                                               /* bNumDescriptors: Number of CUSTOM_BULK class descriptors
                                                         to follow */
  0x22,                                               /* bDescriptorType */
  LOBYTE(USBD_CUSTOM_BULK_REPORT_DESC_SIZE),                   /* wItemLength: Total length of Report descriptor */
  HIBYTE(USBD_CUSTOM_BULK_REPORT_DESC_SIZE),
};

#ifndef USE_USBD_COMPOSITE
/* USB Standard Device Descriptor */
__ALIGN_BEGIN static uint8_t USBD_CUSTOM_BULK_DeviceQualifierDesc[USB_LEN_DEV_QUALIFIER_DESC] __ALIGN_END =
{
  USB_LEN_DEV_QUALIFIER_DESC,
  USB_DESC_TYPE_DEVICE_QUALIFIER,
  0x00,
  0x02,
  0x00,
  0x00,
  0x00,
  0x40,
  0x01,
  0x00,
};
#endif /* USE_USBD_COMPOSITE  */

static uint8_t CUSTOMHIDInEpAdd = CUSTOM_BULK_EPIN_ADDR;
static uint8_t CUSTOMHIDOutEpAdd = CUSTOM_BULK_EPOUT_ADDR;
/**
  * @}
  */

/** @defgroup USBD_CUSTOM_BULK_Private_Functions
  * @{
  */

/**
  * @brief  USBD_CUSTOM_BULK_Init
  *         Initialize the CUSTOM_BULK interface
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t USBD_CUSTOM_BULK_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
  UNUSED(cfgidx);
  USBD_CUSTOM_BULK_HandleTypeDef *hhid;

  hhid = (USBD_CUSTOM_BULK_HandleTypeDef *)USBD_malloc(sizeof(USBD_CUSTOM_BULK_HandleTypeDef));

  if (hhid == NULL)
  {
    pdev->pClassDataCmsit[pdev->classId] = NULL;
    return (uint8_t)USBD_EMEM;
  }

  pdev->pClassDataCmsit[pdev->classId] = (void *)hhid;
  pdev->pClassData = pdev->pClassDataCmsit[pdev->classId];

#ifdef USE_USBD_COMPOSITE
  /* Get the Endpoints addresses allocated for this class instance */
  CUSTOMHIDInEpAdd = USBD_CoreGetEPAdd(pdev, USBD_EP_IN, USBD_EP_TYPE_INTR, (uint8_t)pdev->classId);
  CUSTOMHIDOutEpAdd = USBD_CoreGetEPAdd(pdev, USBD_EP_OUT, USBD_EP_TYPE_INTR, (uint8_t)pdev->classId);
#endif /* USE_USBD_COMPOSITE */

  if (pdev->dev_speed == USBD_SPEED_HIGH)
  {
    pdev->ep_in[CUSTOMHIDInEpAdd & 0xFU].bInterval = CUSTOM_BULK_HS_BINTERVAL;
    pdev->ep_out[CUSTOMHIDOutEpAdd & 0xFU].bInterval = CUSTOM_BULK_HS_BINTERVAL;
  }
  else   /* LOW and FULL-speed endpoints */
  {
    pdev->ep_in[CUSTOMHIDInEpAdd & 0xFU].bInterval = CUSTOM_BULK_FS_BINTERVAL;
    pdev->ep_out[CUSTOMHIDOutEpAdd & 0xFU].bInterval = CUSTOM_BULK_FS_BINTERVAL;
  }

  /* Open EP IN */
  (void)USBD_LL_OpenEP(pdev, CUSTOMHIDInEpAdd, USBD_EP_TYPE_BULK, CUSTOM_BULK_EPIN_SIZE);

  pdev->ep_in[CUSTOMHIDInEpAdd & 0xFU].is_used = 1U;

  if (USBD_CUSTOMHID_OUTREPORT_BUF_SIZE < CUSTOM_BULK_EPOUT_SIZE)
  {
    return (uint8_t)USBD_FAIL;
  }

  /* Open EP OUT */
  (void)USBD_LL_OpenEP(pdev, CUSTOMHIDOutEpAdd, USBD_EP_TYPE_BULK, CUSTOM_BULK_EPOUT_SIZE);

  pdev->ep_out[CUSTOMHIDOutEpAdd & 0xFU].is_used = 1U;

  hhid->state = CUSTOM_BULK_IDLE;

  ((USBD_CUSTOM_BULK_ItfTypeDef *)pdev->pUserData[pdev->classId])->Init();

#ifndef USBD_CUSTOMHID_OUT_PREPARE_RECEIVE_DISABLED
  /* Prepare Out endpoint to receive 1st packet */
  (void)USBD_LL_PrepareReceive(pdev, CUSTOMHIDOutEpAdd, hhid->Report_buf, USBD_CUSTOMHID_OUTREPORT_BUF_SIZE);
#endif /* USBD_CUSTOMHID_OUT_PREPARE_RECEIVE_DISABLED */

  return (uint8_t)USBD_OK;
}

/**
  * @brief  USBD_CUSTOM_BULK_Init
  *         DeInitialize the CUSTOM_BULK layer
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t USBD_CUSTOM_BULK_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
  UNUSED(cfgidx);

#ifdef USE_USBD_COMPOSITE
  /* Get the Endpoints addresses allocated for this class instance */
  CUSTOMHIDInEpAdd = USBD_CoreGetEPAdd(pdev, USBD_EP_IN, USBD_EP_TYPE_INTR, (uint8_t)pdev->classId);
  CUSTOMHIDOutEpAdd = USBD_CoreGetEPAdd(pdev, USBD_EP_OUT, USBD_EP_TYPE_INTR, (uint8_t)pdev->classId);
#endif /* USE_USBD_COMPOSITE */

  /* Close CUSTOM_BULK EP IN */
  (void)USBD_LL_CloseEP(pdev, CUSTOMHIDInEpAdd);
  pdev->ep_in[CUSTOMHIDInEpAdd & 0xFU].is_used = 0U;
  pdev->ep_in[CUSTOMHIDInEpAdd & 0xFU].bInterval = 0U;

  /* Close CUSTOM_BULK EP OUT */
  (void)USBD_LL_CloseEP(pdev, CUSTOMHIDOutEpAdd);
  pdev->ep_out[CUSTOMHIDOutEpAdd & 0xFU].is_used = 0U;
  pdev->ep_out[CUSTOMHIDOutEpAdd & 0xFU].bInterval = 0U;

  /* Free allocated memory */
  if (pdev->pClassDataCmsit[pdev->classId] != NULL)
  {
    ((USBD_CUSTOM_BULK_ItfTypeDef *)pdev->pUserData[pdev->classId])->DeInit();
    USBD_free(pdev->pClassDataCmsit[pdev->classId]);
    pdev->pClassDataCmsit[pdev->classId] = NULL;
    pdev->pClassData = NULL;
  }

  return (uint8_t)USBD_OK;
}

/**
  * @brief  USBD_CUSTOM_BULK_Setup
  *         Handle the CUSTOM_BULK specific requests
  * @param  pdev: instance
  * @param  req: usb requests
  * @retval status
  */
static uint8_t USBD_CUSTOM_BULK_Setup(USBD_HandleTypeDef *pdev,
                                     USBD_SetupReqTypedef *req)
{
  USBD_CUSTOM_BULK_HandleTypeDef *hhid = (USBD_CUSTOM_BULK_HandleTypeDef *)pdev->pClassDataCmsit[pdev->classId];
  uint16_t len = 0U;
#ifdef USBD_CUSTOMHID_CTRL_REQ_GET_REPORT_ENABLED
  uint16_t ReportLength = 0U;
#endif /* USBD_CUSTOMHID_CTRL_REQ_GET_REPORT_ENABLED */
  uint8_t  *pbuf = NULL;
  uint16_t status_info = 0U;
  USBD_StatusTypeDef ret = USBD_OK;

  if (hhid == NULL)
  {
    return (uint8_t)USBD_FAIL;
  }

  switch (req->bmRequest & USB_REQ_TYPE_MASK)
  {
    case USB_REQ_TYPE_CLASS:
      switch (req->bRequest)
      {
        case CUSTOM_BULK_REQ_SET_PROTOCOL:
          hhid->Protocol = (uint8_t)(req->wValue);
          break;

        case CUSTOM_BULK_REQ_GET_PROTOCOL:
          (void)USBD_CtlSendData(pdev, (uint8_t *)&hhid->Protocol, 1U);
          break;

        case CUSTOM_BULK_REQ_SET_IDLE:
          hhid->IdleState = (uint8_t)(req->wValue >> 8);
          break;

        case CUSTOM_BULK_REQ_GET_IDLE:
          (void)USBD_CtlSendData(pdev, (uint8_t *)&hhid->IdleState, 1U);
          break;

        case CUSTOM_BULK_REQ_SET_REPORT:
#ifdef USBD_CUSTOMHID_CTRL_REQ_COMPLETE_CALLBACK_ENABLED
          if (((USBD_CUSTOM_BULK_ItfTypeDef *)pdev->pUserData[pdev->classId])->CtrlReqComplete != NULL)
          {
            /* Let the application decide when to enable EP0 to receive the next report */
            ((USBD_CUSTOM_BULK_ItfTypeDef *)pdev->pUserData[pdev->classId])->CtrlReqComplete(req->bRequest,
                                                                                            req->wLength);
          }
#endif /* USBD_CUSTOMHID_CTRL_REQ_COMPLETE_CALLBACK_ENABLED */
#ifndef USBD_CUSTOMHID_EP0_OUT_PREPARE_RECEIVE_DISABLED

          if (req->wLength > USBD_CUSTOMHID_OUTREPORT_BUF_SIZE)
          {
            /* Stall EP0 */
            USBD_CtlError(pdev, req);
            return USBD_FAIL;
          }

          hhid->IsReportAvailable = 1U;

          (void)USBD_CtlPrepareRx(pdev, hhid->Report_buf, req->wLength);
#endif /* USBD_CUSTOMHID_EP0_OUT_PREPARE_RECEIVE_DISABLED */
          break;
#ifdef USBD_CUSTOMHID_CTRL_REQ_GET_REPORT_ENABLED
        case CUSTOM_BULK_REQ_GET_REPORT:
          if (((USBD_CUSTOM_BULK_ItfTypeDef *)pdev->pUserData[pdev->classId])->GetReport != NULL)
          {
            ReportLength = req->wLength;

            /* Get report data buffer */
            pbuf = ((USBD_CUSTOM_BULK_ItfTypeDef *)pdev->pUserData[pdev->classId])->GetReport(&ReportLength);
          }

          if ((pbuf != NULL) && (ReportLength != 0U))
          {
            len = MIN(ReportLength, req->wLength);

            /* Send the report data over EP0 */
            (void)USBD_CtlSendData(pdev, pbuf, len);
          }
          else
          {
#ifdef USBD_CUSTOMHID_CTRL_REQ_COMPLETE_CALLBACK_ENABLED
            if (((USBD_CUSTOM_BULK_ItfTypeDef *)pdev->pUserData[pdev->classId])->CtrlReqComplete != NULL)
            {
              /* Let the application decide what to do, keep EP0 data phase in NAK state and
                 use USBD_CtlSendData() when data become available or stall the EP0 data phase */
              ((USBD_CUSTOM_BULK_ItfTypeDef *)pdev->pUserData[pdev->classId])->CtrlReqComplete(req->bRequest,
                                                                                              req->wLength);
            }
            else
            {
              /* Stall EP0 if no data available */
              USBD_CtlError(pdev, req);
            }
#else
            /* Stall EP0 if no data available */
            USBD_CtlError(pdev, req);
#endif /* USBD_CUSTOMHID_CTRL_REQ_COMPLETE_CALLBACK_ENABLED */
          }
          break;
#endif /* USBD_CUSTOMHID_CTRL_REQ_GET_REPORT_ENABLED */

        default:
          USBD_CtlError(pdev, req);
          ret = USBD_FAIL;
          break;
      }
      break;

    case USB_REQ_TYPE_STANDARD:
      switch (req->bRequest)
      {
        case USB_REQ_GET_STATUS:
          if (pdev->dev_state == USBD_STATE_CONFIGURED)
          {
            (void)USBD_CtlSendData(pdev, (uint8_t *)&status_info, 2U);
          }
          else
          {
            USBD_CtlError(pdev, req);
            ret = USBD_FAIL;
          }
          break;

        case USB_REQ_GET_DESCRIPTOR:
          if ((req->wValue >> 8) == CUSTOM_BULK_REPORT_DESC)
          {
            len = MIN(USBD_CUSTOM_BULK_REPORT_DESC_SIZE, req->wLength);
            pbuf = ((USBD_CUSTOM_BULK_ItfTypeDef *)pdev->pUserData[pdev->classId])->pReport;
          }
          else
          {
            if ((req->wValue >> 8) == CUSTOM_BULK_DESCRIPTOR_TYPE)
            {
              pbuf = USBD_CUSTOM_BULK_Desc;
              len = MIN(USB_CUSTOM_BULK_DESC_SIZ, req->wLength);
            }
          }

          if (pbuf != NULL)
          {
            (void)USBD_CtlSendData(pdev, pbuf, len);
          }
          else
          {
            USBD_CtlError(pdev, req);
            ret = USBD_FAIL;
          }
          break;

        case USB_REQ_GET_INTERFACE:
          if (pdev->dev_state == USBD_STATE_CONFIGURED)
          {
            (void)USBD_CtlSendData(pdev, (uint8_t *)&hhid->AltSetting, 1U);
          }
          else
          {
            USBD_CtlError(pdev, req);
            ret = USBD_FAIL;
          }
          break;

        case USB_REQ_SET_INTERFACE:
          if (pdev->dev_state == USBD_STATE_CONFIGURED)
          {
            hhid->AltSetting = (uint8_t)(req->wValue);
          }
          else
          {
            USBD_CtlError(pdev, req);
            ret = USBD_FAIL;
          }
          break;

        case USB_REQ_CLEAR_FEATURE:
          break;

        default:
          USBD_CtlError(pdev, req);
          ret = USBD_FAIL;
          break;
      }
      break;

    default:
      USBD_CtlError(pdev, req);
      ret = USBD_FAIL;
      break;
  }
  return (uint8_t)ret;
}

/**
  * @brief  USBD_CUSTOM_BULK_SendReport
  *         Send CUSTOM_BULK Report
  * @param  pdev: device instance
  * @param  buff: pointer to report
  * @param  ClassId: The Class ID
  * @retval status
  */
#ifdef USE_USBD_COMPOSITE
uint8_t USBD_CUSTOM_BULK_SendReport(USBD_HandleTypeDef *pdev,
                                   uint8_t *report, uint16_t len, uint8_t ClassId)
{
  USBD_CUSTOM_BULK_HandleTypeDef *hhid = (USBD_CUSTOM_BULK_HandleTypeDef *)pdev->pClassDataCmsit[ClassId];
#else
uint8_t USBD_CUSTOM_BULK_SendReport(USBD_HandleTypeDef *pdev,
                                   uint8_t *report, uint16_t len)
{
  USBD_CUSTOM_BULK_HandleTypeDef *hhid = (USBD_CUSTOM_BULK_HandleTypeDef *)pdev->pClassDataCmsit[pdev->classId];
#endif /* USE_USBD_COMPOSITE */

  if (hhid == NULL)
  {
    return (uint8_t)USBD_FAIL;
  }

#ifdef USE_USBD_COMPOSITE
  /* Get Endpoint IN address allocated for this class instance */
  CUSTOMHIDInEpAdd = USBD_CoreGetEPAdd(pdev, USBD_EP_IN, USBD_EP_TYPE_INTR, ClassId);
#endif /* USE_USBD_COMPOSITE */

  if (pdev->dev_state == USBD_STATE_CONFIGURED)
  {
    if (hhid->state == CUSTOM_BULK_IDLE)
    {
      hhid->state = CUSTOM_BULK_BUSY;
      (void)USBD_LL_Transmit(pdev, CUSTOMHIDInEpAdd, report, len);
    }
    else
    {
      return (uint8_t)USBD_BUSY;
    }
  }
  return (uint8_t)USBD_OK;
}
#ifndef USE_USBD_COMPOSITE
/**
  * @brief  USBD_CUSTOM_BULK_GetFSCfgDesc
  *         return FS configuration descriptor
  * @param  speed : current device speed
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static uint8_t *USBD_CUSTOM_BULK_GetFSCfgDesc(uint16_t *length)
{
  USBD_EpDescTypeDef *pEpInDesc = USBD_GetEpDesc(USBD_CUSTOM_BULK_CfgDesc, CUSTOM_BULK_EPIN_ADDR);
  USBD_EpDescTypeDef *pEpOutDesc = USBD_GetEpDesc(USBD_CUSTOM_BULK_CfgDesc, CUSTOM_BULK_EPOUT_ADDR);

  if (pEpInDesc != NULL)
  {
    pEpInDesc->wMaxPacketSize = CUSTOM_BULK_EPIN_SIZE;
    pEpInDesc->bInterval = CUSTOM_BULK_FS_BINTERVAL;
  }

  if (pEpOutDesc != NULL)
  {
    pEpOutDesc->wMaxPacketSize = CUSTOM_BULK_EPOUT_SIZE;
    pEpOutDesc->bInterval = CUSTOM_BULK_FS_BINTERVAL;
  }

  *length = (uint16_t)sizeof(USBD_CUSTOM_BULK_CfgDesc);
  return USBD_CUSTOM_BULK_CfgDesc;
}

/**
  * @brief  USBD_CUSTOM_BULK_GetHSCfgDesc
  *         return HS configuration descriptor
  * @param  speed : current device speed
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static uint8_t *USBD_CUSTOM_BULK_GetHSCfgDesc(uint16_t *length)
{
  USBD_EpDescTypeDef *pEpInDesc = USBD_GetEpDesc(USBD_CUSTOM_BULK_CfgDesc, CUSTOM_BULK_EPIN_ADDR);
  USBD_EpDescTypeDef *pEpOutDesc = USBD_GetEpDesc(USBD_CUSTOM_BULK_CfgDesc, CUSTOM_BULK_EPOUT_ADDR);

  if (pEpInDesc != NULL)
  {
    pEpInDesc->wMaxPacketSize = CUSTOM_BULK_EPIN_SIZE;
    pEpInDesc->bInterval = CUSTOM_BULK_HS_BINTERVAL;
  }

  if (pEpOutDesc != NULL)
  {
    pEpOutDesc->wMaxPacketSize = CUSTOM_BULK_EPOUT_SIZE;
    pEpOutDesc->bInterval = CUSTOM_BULK_HS_BINTERVAL;
  }

  *length = (uint16_t)sizeof(USBD_CUSTOM_BULK_CfgDesc);
  return USBD_CUSTOM_BULK_CfgDesc;
}

/**
  * @brief  USBD_CUSTOM_BULK_GetOtherSpeedCfgDesc
  *         return other speed configuration descriptor
  * @param  speed : current device speed
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static uint8_t *USBD_CUSTOM_BULK_GetOtherSpeedCfgDesc(uint16_t *length)
{
  USBD_EpDescTypeDef *pEpInDesc = USBD_GetEpDesc(USBD_CUSTOM_BULK_CfgDesc, CUSTOM_BULK_EPIN_ADDR);
  USBD_EpDescTypeDef *pEpOutDesc = USBD_GetEpDesc(USBD_CUSTOM_BULK_CfgDesc, CUSTOM_BULK_EPOUT_ADDR);

  if (pEpInDesc != NULL)
  {
    pEpInDesc->wMaxPacketSize = CUSTOM_BULK_EPIN_SIZE;
    pEpInDesc->bInterval = CUSTOM_BULK_FS_BINTERVAL;
  }

  if (pEpOutDesc != NULL)
  {
    pEpOutDesc->wMaxPacketSize = CUSTOM_BULK_EPOUT_SIZE;
    pEpOutDesc->bInterval = CUSTOM_BULK_FS_BINTERVAL;
  }

  *length = (uint16_t)sizeof(USBD_CUSTOM_BULK_CfgDesc);
  return USBD_CUSTOM_BULK_CfgDesc;
}
#endif /* USE_USBD_COMPOSITE  */

/**
  * @brief  USBD_CUSTOM_BULK_DataIn
  *         handle data IN Stage
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t USBD_CUSTOM_BULK_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
  UNUSED(epnum);

  /* Ensure that the FIFO is empty before a new transfer, this condition could
  be caused by  a new transfer before the end of the previous transfer */
  ((USBD_CUSTOM_BULK_HandleTypeDef *)pdev->pClassDataCmsit[pdev->classId])->state = CUSTOM_BULK_IDLE;

  return (uint8_t)USBD_OK;
}

/**
  * @brief  USBD_CUSTOM_BULK_DataOut
  *         handle data OUT Stage
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t USBD_CUSTOM_BULK_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
  UNUSED(epnum);
  USBD_CUSTOM_BULK_HandleTypeDef *hhid;

  if (pdev->pClassDataCmsit[pdev->classId] == NULL)
  {
    return (uint8_t)USBD_FAIL;
  }

  hhid = (USBD_CUSTOM_BULK_HandleTypeDef *)pdev->pClassDataCmsit[pdev->classId];

  /* USB data will be immediately processed, this allow next USB traffic being
  NAKed till the end of the application processing */

#ifdef USBD_CUSTOMHID_REPORT_BUFFER_EVENT_ENABLED
  ((USBD_CUSTOM_BULK_ItfTypeDef *)pdev->pUserData[pdev->classId])->OutEvent(hhid->Report_buf);
#else
  ((USBD_CUSTOM_BULK_ItfTypeDef *)pdev->pUserData[pdev->classId])->OutEvent(hhid->Report_buf[0],
                                                                           hhid->Report_buf[1]);
#endif /* USBD_CUSTOMHID_REPORT_BUFFER_EVENT_ENABLED */

  return (uint8_t)USBD_OK;
}


/**
  * @brief  USBD_CUSTOM_BULK_ReceivePacket
  *         prepare OUT Endpoint for reception
  * @param  pdev: device instance
  * @retval status
  */
uint8_t USBD_CUSTOM_BULK_ReceivePacket(USBD_HandleTypeDef *pdev)
{
  USBD_CUSTOM_BULK_HandleTypeDef *hhid;

  if (pdev->pClassDataCmsit[pdev->classId] == NULL)
  {
    return (uint8_t)USBD_FAIL;
  }

#ifdef USE_USBD_COMPOSITE
  /* Get OUT Endpoint address allocated for this class instance */
  CUSTOMHIDOutEpAdd = USBD_CoreGetEPAdd(pdev, USBD_EP_OUT, USBD_EP_TYPE_INTR, (uint8_t)pdev->classId);
#endif /* USE_USBD_COMPOSITE */

  hhid = (USBD_CUSTOM_BULK_HandleTypeDef *)pdev->pClassDataCmsit[pdev->classId];

  /* Resume USB Out process */
  (void)USBD_LL_PrepareReceive(pdev, CUSTOMHIDOutEpAdd, hhid->Report_buf, USBD_CUSTOMHID_OUTREPORT_BUF_SIZE);

  return (uint8_t)USBD_OK;
}


/**
  * @brief  USBD_CUSTOM_BULK_EP0_RxReady
  *         Handles control request data.
  * @param  pdev: device instance
  * @retval status
  */
static uint8_t USBD_CUSTOM_BULK_EP0_RxReady(USBD_HandleTypeDef *pdev)
{
  USBD_CUSTOM_BULK_HandleTypeDef *hhid = (USBD_CUSTOM_BULK_HandleTypeDef *)pdev->pClassDataCmsit[pdev->classId];

  if (hhid == NULL)
  {
    return (uint8_t)USBD_FAIL;
  }

  if (hhid->IsReportAvailable == 1U)
  {
#ifdef USBD_CUSTOMHID_REPORT_BUFFER_EVENT_ENABLED
    ((USBD_CUSTOM_BULK_ItfTypeDef *)pdev->pUserData[pdev->classId])->OutEvent(hhid->Report_buf);
#else
    ((USBD_CUSTOM_BULK_ItfTypeDef *)pdev->pUserData[pdev->classId])->OutEvent(hhid->Report_buf[0],
                                                                             hhid->Report_buf[1]);
#endif /* USBD_CUSTOMHID_REPORT_BUFFER_EVENT_ENABLED */
    hhid->IsReportAvailable = 0U;
  }

  return (uint8_t)USBD_OK;
}

#ifndef USE_USBD_COMPOSITE
/**
  * @brief  DeviceQualifierDescriptor
  *         return Device Qualifier descriptor
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static uint8_t *USBD_CUSTOM_BULK_GetDeviceQualifierDesc(uint16_t *length)
{
  *length = (uint16_t)sizeof(USBD_CUSTOM_BULK_DeviceQualifierDesc);

  return USBD_CUSTOM_BULK_DeviceQualifierDesc;
}
#endif /* USE_USBD_COMPOSITE  */
/**
  * @brief  USBD_CUSTOM_BULK_RegisterInterface
  * @param  pdev: device instance
  * @param  fops: CUSTOMHID Interface callback
  * @retval status
  */
uint8_t USBD_CUSTOM_BULK_RegisterInterface(USBD_HandleTypeDef *pdev,
                                          USBD_CUSTOM_BULK_ItfTypeDef *fops)
{
  if (fops == NULL)
  {
    return (uint8_t)USBD_FAIL;
  }

  pdev->pUserData[pdev->classId] = fops;

  return (uint8_t)USBD_OK;
}


