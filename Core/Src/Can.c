#include "Can.h"
#include "stm32f103xb.h"
#include "main.h"
#include "FreeRTOS.h"
#include "app_task.h"


#define OBD_PID_ENGINE_COOLANT_TEMP       0x05U

#define OBD_PID_ENGINE_RPM                0x0CU

#define OBD_PID_VEHICLE_SPEED             0x0DU

#define OBD_PID_THROTTLE_POSITION         0x11U



void Can_Init()
{

    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN; //AFIO clock
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;  //GPIO clock
    RCC->APB1ENR |= RCC_APB1ENR_CAN1EN;  //CAN clock

    //PA11 : Input Floating (MODE11 = 00, CNF11 = 01)
    GPIOA->CRH &= ~(GPIO_CRH_MODE11 | GPIO_CRH_CNF11);
    GPIOA->CRH |=  GPIO_CRH_CNF11_0;

    // PA12 : AF Push Pull, 50 MHz (MODE12 = 11, CNF12 = 10)
    GPIOA->CRH &= ~(GPIO_CRH_MODE12 | GPIO_CRH_CNF12);
    GPIOA->CRH |=  GPIO_CRH_MODE12;
    GPIOA->CRH |=  GPIO_CRH_CNF12_1;


    //CAN Config....

    CAN1->MCR &= ~CAN_MCR_SLEEP; // remove it from sleep and wake up the CAN pheripharals
    CAN1->MCR |= CAN_MCR_INRQ;
    while ((CAN1->MSR & CAN_MSR_INAK) == 0);

    CAN1->MCR |= CAN_MCR_ABOM;          // 
    CAN1->MCR &= ~CAN_MCR_NART;      //Auto Transmition
    CAN1->MCR &= ~CAN_MCR_TXFP;        //Transmit Priority by CAN Identifier 

    /*
 * Bit Time = (SyncSeg + TS1 + TS2) × TQ
 *
 * SyncSeg = 1 TQ
 * TS1     = 13 TQ
 * TS2     = 2 TQ
 *
 * Total = 16 TQ
 *
 * TQ = (BRP + 1) / PCLK1
 *
 * BRP = 0  --> Prescaler = 1
 *
 * Bit Rate =
 * 8 MHz / (1 × 16)
 * = 500 kbps
 */

CAN1->BTR = 0;

/* Synchronization Jump Width = 1 TQ */
CAN1->BTR |= (0U << CAN_BTR_SJW_Pos);

/* Time Segment 1 = 13 TQ (Register Value = 12) */
CAN1->BTR |= (12U << CAN_BTR_TS1_Pos);

/* Time Segment 2 = 2 TQ (Register Value = 1) */
CAN1->BTR |= (1U << CAN_BTR_TS2_Pos);

/* Baud Rate Prescaler = 1 (Register Value = 0) */
CAN1->BTR |= (0U << CAN_BTR_BRP_Pos);

/* Normal Mode */
CAN1->BTR &= ~CAN_BTR_LBKM;
CAN1->BTR &= ~CAN_BTR_SILM;

    /*=========================================================
      Filter Configuration (Accept All)
    =========================================================*/

    /* Enter Filter Initialization Mode */
    CAN1->FMR |= CAN_FMR_FINIT;

    /* Filter 0 -> Mask Mode */
    CAN1->FM1R &= ~(1U << 0);

    /* Filter 0 -> 32-bit Scale */
    CAN1->FS1R |= (1U << 0);

    /* Assign Filter 0 to FIFO0 */
    CAN1->FFA1R &= ~(1U << 0);

    /* Accept All IDs */
    CAN1->sFilterRegister[0].FR1 = 0x00000000;
    CAN1->sFilterRegister[0].FR2 = 0x00000000;

    /* Activate Filter 0 */
    CAN1->FA1R |= (1U << 0);

    /* Leave Filter Initialization Mode */
    CAN1->FMR &= ~CAN_FMR_FINIT;

    /* Leave Initialization Mode */
    CAN1->MCR &= ~CAN_MCR_INRQ;

    /* Wait until Normal Mode */
    while (CAN1->MSR & CAN_MSR_INAK);

}

bool CAN_Transmit(CAN_Message_t *msg)
{
    uint8_t MailBox;

    /*-----------------------------------------
      Find an Empty Mailbox
    -----------------------------------------*/

    if (CAN1->TSR & CAN_TSR_TME0)
    {
        MailBox = 0;
    }
    else if (CAN1->TSR & CAN_TSR_TME1)
    {
        MailBox = 1;
    }
    else if (CAN1->TSR & CAN_TSR_TME2)
    {
        MailBox = 2;
    }
    else
    {
    	serial_usart1_char_print("Mailbox is not empty!!!!!!!!!!!!!!!!\r\n");
        return false;
    }

    /*-----------------------------------------
      Clear Mailbox Registers
    -----------------------------------------*/

    CAN1->sTxMailBox[MailBox].TIR  = 0;
    CAN1->sTxMailBox[MailBox].TDTR = 0;
    CAN1->sTxMailBox[MailBox].TDLR = 0;
    CAN1->sTxMailBox[MailBox].TDHR = 0;

    /*-----------------------------------------
      Standard Identifier (11-bit)
    -----------------------------------------*/

    CAN1->sTxMailBox[MailBox].TIR |= ((uint32_t)(msg->ID) << 21);

    /* Data Frame */
    CAN1->sTxMailBox[MailBox].TIR &= ~CAN_TI0R_RTR;

    /* Standard Identifier */
    CAN1->sTxMailBox[MailBox].TIR &= ~CAN_TI0R_IDE;

    /*-----------------------------------------
      Data Length Code
    -----------------------------------------*/

    CAN1->sTxMailBox[MailBox].TDTR = (msg->DLC & 0x0F);

    /*-----------------------------------------
      Data Bytes 0 - 3
    -----------------------------------------*/

    CAN1->sTxMailBox[MailBox].TDLR =
          ((uint32_t)msg->Data[0] << 0)
        | ((uint32_t)msg->Data[1] << 8)
        | ((uint32_t)msg->Data[2] << 16)
        | ((uint32_t)msg->Data[3] << 24);

    /*-----------------------------------------
      Data Bytes 4 - 7
    -----------------------------------------*/

    CAN1->sTxMailBox[MailBox].TDHR =
          ((uint32_t)msg->Data[4] << 0)
        | ((uint32_t)msg->Data[5] << 8)
        | ((uint32_t)msg->Data[6] << 16)
        | ((uint32_t)msg->Data[7] << 24);

    /*-----------------------------------------
      Request Transmission
    -----------------------------------------*/

    CAN1->sTxMailBox[MailBox].TIR |= CAN_TI0R_TXRQ;

    /*-----------------------------------------
      Wait Until Transmission Completes
    -----------------------------------------*/

    uint32_t Timeout = 0;

  while (!(CAN1->TSR & (CAN_TSR_RQCP0 << (MailBox * 8))))
  {
      if(++Timeout > 1000000)
      {
    	  serial_usart1_char_print("Tx timeout!!!!!!!!!!!!!!!!!!!\r\n");
          return false;
      }
  } 

    /*-----------------------------------------
      Check Success
    -----------------------------------------*/

    if (CAN1->TSR & (CAN_TSR_TXOK0 << (MailBox * 8)))
    {
        /* Clear Request Complete Flag */
        CAN1->TSR |= (CAN_TSR_RQCP0 << (MailBox * 8));

        return true;
    }

    /* Clear Request Complete Flag */
    CAN1->TSR |= (CAN_TSR_RQCP0 << (MailBox * 8));

    serial_usart1_char_print("checksum failed!!!!!!!!!!!!!\r\n");
    return false;
}


bool CAN_Receive(CAN_Message_t *msg)
{
    /*-----------------------------------------
      Check if FIFO0 contains a message
    -----------------------------------------*/

    if((CAN1->RF0R & CAN_RF0R_FMP0) == 0)
    {
    	serial_usart1_char_print("Rx FIFO0 is empty!!!!!!!!!!!!\r\n");
        return false;
    }

    /*-----------------------------------------
      Read Standard Identifier
    -----------------------------------------*/

    msg->ID = (CAN1->sFIFOMailBox[0].RIR >> 21) & 0x7FF;

    /*-----------------------------------------
      Read Data Length Code
    -----------------------------------------*/

    msg->DLC = CAN1->sFIFOMailBox[0].RDTR & 0x0F;

    /*-----------------------------------------
      Read Data Bytes 0 - 3
    -----------------------------------------*/

    uint32_t DataLow = CAN1->sFIFOMailBox[0].RDLR;

    msg->Data[0] = (uint8_t)(DataLow);
    msg->Data[1] = (uint8_t)(DataLow >> 8);
    msg->Data[2] = (uint8_t)(DataLow >> 16);
    msg->Data[3] = (uint8_t)(DataLow >> 24);

    /*-----------------------------------------
      Read Data Bytes 4 - 7
    -----------------------------------------*/

    uint32_t DataHigh = CAN1->sFIFOMailBox[0].RDHR;

    msg->Data[4] = (uint8_t)(DataHigh);
    msg->Data[5] = (uint8_t)(DataHigh >> 8);
    msg->Data[6] = (uint8_t)(DataHigh >> 16);
    msg->Data[7] = (uint8_t)(DataHigh >> 24);

    /*-----------------------------------------
      Release FIFO0
    -----------------------------------------*/

    CAN1->RF0R |= CAN_RF0R_RFOM0;

    return true;
}

bool OBD_RequestPID(CAN_Message_t *TxMsg, uint8_t PID)
{
    /* Functional Request ID */
    TxMsg->ID = 0x7DF;

    /* Standard CAN Frame Length */
    TxMsg->DLC = 8;

    /* OBD-II Request Frame */
    TxMsg->Data[0] = 0x02;
    TxMsg->Data[1] = 0x01;
    TxMsg->Data[2] = PID;

    /* Padding */
    TxMsg->Data[3] = 0x00;
    TxMsg->Data[4] = 0x00;
    TxMsg->Data[5] = 0x00;
    TxMsg->Data[6] = 0x00;
    TxMsg->Data[7] = 0x00;

    return CAN_Transmit(TxMsg);
}

bool OBD_ReadResponse(CAN_Message_t *RxMsg)
{
  uint32_t Timeout = 0;

  while(!CAN_Receive(RxMsg))
  {
      if(++Timeout > 1000000)
      {
          return false;
      }
  }

    /* Engine ECU Response */
    if(RxMsg->ID != 0x7E8)
    {
        return false;
    }

    /* Response to Mode 01 */
    if(RxMsg->Data[1] != 0x41)
    {
        return false;
    }

    return true;
}

uint16_t OBD_GetRPM(CAN_Message_t *RxMsg)
{
    if(RxMsg->Data[2] != 0x0C)
    {
        return 0;
    }

    return (((uint16_t)RxMsg->Data[3] << 8) | RxMsg->Data[4]) / 4;
}

uint8_t OBD_GetVehicleSpeed(CAN_Message_t *RxMsg)
{
    if(RxMsg->Data[2] != 0x0D)
    {
        return 0;
    }

    return RxMsg->Data[3];
}

uint8_t OBD_GetCoolantTemperature(CAN_Message_t *RxMsg)
{
    if(RxMsg->Data[2] != 0x05)
    {
        return 0;
    }

    return (int8_t)(RxMsg->Data[3] - 40);
}

uint8_t OBD_GetThrottlePosition(CAN_Message_t *RxMsg)
{
    if(RxMsg->Data[2] != 0x11)
    {
        return 0;
    }

    return (uint8_t)(((uint16_t)RxMsg->Data[3] * 100U) / 255U);
}
