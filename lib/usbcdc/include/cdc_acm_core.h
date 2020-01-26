/*!
    \file  cdc_acm_core.h
    \brief the header file of CDC ACM driver

    \version 2020-01-25, V1.0.0, USB to UART bridge for GD32VF103
*/

/*
    Copyright (c) 2019, GigaDevice Semiconductor Inc.
    Copyright (c) 2020, LoBo

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

#ifndef CDC_ACM_CORE_H
#define CDC_ACM_CORE_H

#include "usbd_enum.h"
#include "usb_ch9_std.h"
#include "usbd_transc.h"

// ==== Conditional compile ====
#define USE_DISPLAY                             1
#define USE_LEDS                                1
#define UART_USE_BREAK_TO_CHANGE                1

// ==== Constants ====
#define UART_RXBUFFER_SIZE                      4096
#define UART_DEFAULT_BDR                        115200

#define UART0_TXPIN                             GPIO_PIN_9
#define UART0_RXPIN                             GPIO_PIN_10
#define UART0_RTSPIN                            GPIO_PIN_13
#define UART0_DTRPIN                            GPIO_PIN_14
#define UART0_PORT                              GPIOA
#define UART0_RTS_DTR_MODE                      GPIO_MODE_AF_OD

#define UART2_TXPIN                             GPIO_PIN_10
#define UART2_RXPIN                             GPIO_PIN_11
#define UART2_RTSPIN                            GPIO_PIN_13
#define UART2_DTRPIN                            GPIO_PIN_14
#define UART2_PORT                              GPIOB
#define UART2_RTS_DTR_MODE                      GPIO_MODE_AF_PP

// ==== USB/CDC constants ====
#define USB_DESCTYPE_CS_INTERFACE               0x24
#define USB_CDC_ACM_CONFIG_DESC_SIZE            0x43
#define CDC_ACM_DESC_SIZE                       0x3A
#define CDC_ACM_DESC_TYPE                       0x21
#define SEND_ENCAPSULATED_COMMAND               0x00
#define GET_ENCAPSULATED_RESPONSE               0x01
#define SET_COMM_FEATURE                        0x02
#define GET_COMM_FEATURE                        0x03
#define CLEAR_COMM_FEATURE                      0x04
#define SET_LINE_CODING                         0x20
#define GET_LINE_CODING                         0x21
#define SET_CONTROL_LINE_STATE                  0x22
#define SEND_BREAK                              0x23
#define NO_CMD                                  0xFF
// Custom CDC commands
#define SHOW_STATUS                             0xE0
#define SELECT_UART                             0xE1
#define GET_STATUS                              0xE2

#pragma pack(1)

typedef struct
{
    uint32_t dwDTERate;   /* data terminal rate */
    uint8_t  bCharFormat; /* stop bits */
    uint8_t  bParityType; /* parity */
    uint8_t  bDataBits;   /* data bits */
}line_coding_struct;

typedef struct
{
    usb_desc_header header;               /*!< descriptor header, including type and size. */
    uint8_t  bDescriptorSubtype;          /*!< bDescriptorSubtype: header function descriptor */
    uint16_t  bcdCDC;                     /*!< bcdCDC: low byte of spec release number (CDC1.10) */
} usb_descriptor_header_function_struct;

typedef struct
{
    usb_desc_header header;               /*!< descriptor header, including type and size. */
    uint8_t  bDescriptorSubtype;          /*!< bDescriptorSubtype:  call management function descriptor */
    uint8_t  bmCapabilities;              /*!< bmCapabilities: D0 is reset, D1 is ignored */
    uint8_t  bDataInterface;              /*!< bDataInterface: 1 interface used for call management */
} usb_descriptor_call_managment_function_struct;

typedef struct
{
    usb_desc_header header;               /*!< descriptor header, including type and size. */
    uint8_t  bDescriptorSubtype;          /*!< bDescriptorSubtype: abstract control management desc */
    uint8_t  bmCapabilities;              /*!< bmCapabilities: D1 */
} usb_descriptor_acm_function_struct;

typedef struct
{
    usb_desc_header header;               /*!< descriptor header, including type and size. */
    uint8_t  bDescriptorSubtype;          /*!< bDescriptorSubtype: union func desc */
    uint8_t  bMasterInterface;            /*!< bMasterInterface: communication class interface */
    uint8_t  bSlaveInterface0;            /*!< bSlaveInterface0: data class interface */
} usb_descriptor_union_function_struct;

#pragma pack()

typedef struct
{
    usb_desc_config                config;
    usb_desc_itf                   cdc_loopback_interface;
    usb_descriptor_header_function_struct             cdc_loopback_header;
    usb_descriptor_call_managment_function_struct     cdc_loopback_call_managment;
    usb_descriptor_acm_function_struct                cdc_loopback_acm;
    usb_descriptor_union_function_struct              cdc_loopback_union;
    usb_desc_ep                    cdc_loopback_cmd_endpoint;
    usb_desc_itf                   cdc_loopback_data_interface;
    usb_desc_ep                    cdc_loopback_out_endpoint;
    usb_desc_ep                    cdc_loopback_in_endpoint;
} usb_descriptor_configuration_set_struct;

extern void* const usbd_strings[USB_STRING_COUNT];
extern const usb_desc_dev device_descriptor;
extern usb_descriptor_configuration_set_struct configuration_descriptor;
extern usb_class_core usbd_cdc_cb;

extern line_coding_struct linecoding;
extern uint32_t receive_count;
extern uint32_t send_count;
extern __IO uint32_t uart_receive_count;
extern __IO uint32_t uart_send_count;
extern __IO uint32_t uart_error_count;
extern __IO uint8_t packet_sent;
extern __IO uint8_t packet_receive;
extern __IO uint32_t receive_length;
extern __IO uint32_t active_uart;
extern __IO uint32_t uart_change_req;

// ==== function declarations ====

// initialize the CDC ACM device
uint8_t cdc_acm_init(usb_dev *pudev, uint8_t config_index);
// de-initialize the CDC ACM device
uint8_t cdc_acm_deinit(usb_dev *pudev, uint8_t config_index);
// handle the CDC ACM class-specific requests
uint8_t cdc_acm_req_handler(usb_dev *pudev, usb_req *req);
// handle CDC ACM data
uint8_t cdc_acm_data_in_handler(usb_dev *pudev, uint8_t ep_id);
uint8_t cdc_acm_data_out_handler(usb_dev *pudev, uint8_t ep_id);

// command data received on control endpoint
uint8_t cdc_acm_EP0_RxReady(usb_dev  *pudev);

void lcd_showSettings(void);

#endif  // CDC_ACM_CORE_H
