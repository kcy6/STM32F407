/**
  ******************************************************************************
  * @file    usbd_desc_template.c
  * @author  MCD Application Team
  * @brief   This file provides the USBD descriptors and string formatting method.
  *          This template should be copied to the user folder,
  *          renamed and customized following user needs.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2015 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                      www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_conf.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define USBD_VID                      0x0483
#define USBD_PID                      0xaaaa  /* Replace '0xaaaa' with your device product ID */
#define USBD_LANGID_STRING            0xbbb  /* Replace '0xbbb' with your device language ID */
#define USBD_MANUFACTURER_STRING      "xxxxx" /* Add your manufacturer string */
#define USBD_PRODUCT_HS_STRING        "xxxxx" /* Add your product High Speed string */
#define USBD_PRODUCT_FS_STRING        "xxxxx" /* Add your product Full Speed string */
#define USBD_CONFIGURATION_HS_STRING  "xxxxx" /* Add your configuration High Speed string */
#define USBD_INTERFACE_HS_STRING      "xxxxx" /* Add your Interface High Speed string */
#define USBD_CONFIGURATION_FS_STRING  "xxxxx" /* Add your configuration Full Speed string */
#define USBD_INTERFACE_FS_STRING      "xxxxx" /* Add your Interface Full Speed string */

/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
uint8_t *USBD_Class_DeviceDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t *USBD_Class_LangIDStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t *USBD_Class_ManufacturerStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t *USBD_Class_ProductStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t *USBD_Class_SerialStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t *USBD_Class_ConfigStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t *USBD_Class_InterfaceStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);

#if (USBD_CLASS_USER_STRING_DESC == 1)
uint8_t *USBD_Class_UserStrDescriptor(USBD_SpeedTypeDef speed, uint8_t idx, uint16_t *length);
#endif /* USB_CLASS_USER_STRING_DESC */

#if ((USBD_LPM_ENABLED == 1) || (USBD_CLASS_BOS_ENABLED == 1))
uint8_t *USBD_USR_BOSDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
#endif

/* Private variables ---------------------------------------------------------*/
USBD_DescriptorsTypeDef Class_Desc =
{
  USBD_Class_DeviceDescriptor,
  USBD_Class_LangIDStrDescriptor,
  USBD_Class_ManufacturerStrDescriptor,
  USBD_Class_ProductStrDescriptor,
  USBD_Class_SerialStrDescriptor,
  USBD_Class_ConfigStrDescriptor,
  USBD_Class_InterfaceStrDescriptor,
#if (USBD_CLASS_USER_STRING_DESC == 1)
  USBD_CLASS_UserStrDescriptor,
#endif

#if ((USBD_LPM_ENABLED == 1) || (USBD_CLASS_BOS_ENABLED == 1))
  USBD_USR_BOSDescriptor,
#endif
};

/* USB Standard Device Descriptor */
#if defined ( __ICCARM__ ) /*!< IAR Compiler */
#pragma data_alignment=4
#endif
__ALIGN_BEGIN uint8_t USBD_DeviceDesc[USB_LEN_DEV_DESC] __ALIGN_END =
{
  0x12,                       /* bLength */
  USB_DESC_TYPE_DEVICE,       /* bDescriptorType */
#if ((USBD_LPM_ENABLED == 1) || (USBD_CLASS_BOS_ENABLED == 1))
  0x01,                       /*bcdUSB */     /* changed to USB version 2.01
                                              in order to support BOS Desc */
#else
  0x00,                       /* bcdUSB */
#endif
  0x02,
  0x00,                       /* bDeviceClass */
  0x00,                       /* bDeviceSubClass */
  0x00,                       /* bDeviceProtocol */
  USB_MAX_EP0_SIZE,           /* bMaxPacketSize */
  LOBYTE(USBD_VID),           /* idVendor */
  HIBYTE(USBD_VID),           /* idVendor */
  LOBYTE(USBD_PID),           /* idVendor */
  HIBYTE(USBD_PID),           /* idVendor */
  0x00,                       /* bcdDevice rel. 2.00 */
  0x02,
  USBD_IDX_MFC_STR,           /* Index of manufacturer string */
  USBD_IDX_PRODUCT_STR,       /* Index of product string */
  USBD_IDX_SERIAL_STR,        /* Index of serial number string */
  USBD_MAX_NUM_CONFIGURATION  /* bNumConfigurations */
}; /* USB_DeviceDescriptor */


/* USB Device LPM BOS descriptor */
#if (USBD_LPM_ENABLED == 1)
#if defined ( __ICCARM__ ) /*!< IAR Compiler */
#pragma data_alignment=4
#endif
__ALIGN_BEGIN  uint8_t USBD_BOSDesc[USB_SIZ_BOS_DESC] __ALIGN_END =
{
  0x5,
  USB_DESC_TYPE_BOS,
  0xC,
  0x0,
  0x1,  /* 1 device capability */
  /* device capability */
  0x7,
  USB_DEVICE_CAPABITY_TYPE,
  0x2,
  0x6, /*LPM capability bit set */
  0x0,
  0x0,
  0x0
};
#endif

/* USB Device Billboard BOS descriptor Template */
#if (USBD_CLASS_BOS_ENABLED == 1)
#if defined ( __ICCARM__ ) /*!< IAR Compiler */
#pragma data_alignment=4
#endif
__ALIGN_BEGIN  uint8_t USBD_BOSDesc[USB_SIZ_BOS_DESC] __ALIGN_END =
{
  0x05,                                /* bLength */
  USB_DESC_TYPE_BOS,                   /* Device Descriptor Type */
  USB_SIZ_BOS_DESC,                    /* Total length of BOS descriptor and all of its sub descs */
  0x00,
  0x04,                                /* The number of separate device capability descriptors in the BOS */

  /* ----------- Device Capability Descriptor: CONTAINER_ID ---------- */
  0x14,                                /* bLength */
  0x10,                                /* bDescriptorType: DEVICE CAPABILITY Type */
  0x04,                                /* bDevCapabilityType: CONTAINER_ID */
  0x00,                                /* bReserved */
  0xa7, 0xd6, 0x1b, 0xfa,              /* ContainerID: This is a Unique 128-bit number GUID */
  0x91, 0xa6, 0xa8, 0x4e,
  0xa8, 0x21, 0x9f, 0x2b,
  0xaf, 0xf7, 0x94, 0xd4,

  /* ----------- Device Capability Descriptor: BillBoard ---------- */
  0x34,                                /* bLength */
  0x10,                                /* bDescriptorType: DEVICE CAPABILITY Type */
  0x0D,                                /* bDevCapabilityType: BILLBOARD_CAPABILITY */
  USBD_BB_URL_STRING_INDEX,            /* iAddtionalInfoURL: Index of string descriptor providing a URL where the user can go to get more
                                        detailed information about the product and the various Alternate Modes it supports */

  0x02,                                /* bNumberOfAlternateModes: Number of Alternate modes supported. The
                                        maximum value that this field can be set to is MAX_NUM_ALT_MODE. */

  0x00,                                /* bPreferredAlternateMode: Index of the preferred Alternate Mode. System
                                        software may use this information to provide the user with a better user experience. */

  0x00, 0x00,                          /* VCONN Power needed by the adapter for full functionality 000b = 1W */

  0x01, 0x00, 0x00, 0x00,              /* bmConfigured. 01b: Alternate Mode configuration not attempted or exited */
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x21, 0x01,                          /* bcdVersion = 0x0121 */
  0x00,                                /* bAdditionalFailureInfo */
  0x00,                                /* bReserved */
  LOBYTE(USBD_VID),
  HIBYTE(USBD_VID),                    /* wSVID[0]: Standard or Vendor ID. This shall match one of the SVIDs
                                        returned in response to a USB PD Discover SVIDs command */

  0x00,                                /* bAlternateMode[0] Index of the Alternate Mode within the SVID as
                                        returned in response to a Discover Modes command. Example:
                                        0 