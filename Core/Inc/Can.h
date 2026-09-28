#ifndef CAN_H_
#define CAN_H_

#include <stdint.h>
#include <stdbool.h>

/*=========================================================
  CAN Message Structure
=========================================================*/

typedef struct
{
    uint32_t ID;
    uint8_t  DLC;
    uint8_t  Data[8];
} CAN_Message_t;

/*=========================================================
  Function Prototypes
=========================================================*/

/**
 * @brief  Initializes CAN1 peripheral (GPIO, clocks, bit timing,
 *         filters) at 500 kbps assuming PCLK1 = 8 MHz.
 * @note   Blocks until hardware confirms Init mode entry/exit.
 *         Will hang here if CAN transceiver/wiring is not
 *         connected correctly (bus stays in bus-off / no
 *         recessive bits sensed).
 */
void Can_Init(void);

/**
 * @brief  Transmits a standard-ID CAN data frame.
 * @param  msg  Pointer to message to send (ID must be <= 0x7FF,
 *              DLC must be <= 8).
 * @retval true  if message was queued and transmitted successfully.
 * @retval false if no mailbox was free, or if the transmit-complete
 *               wait timed out (bus busy / no ACK from any node).
 * @note   Bounded wait: gives up after a fixed poll-iteration
 *         timeout instead of blocking forever. Since NART is
 *         disabled, the peripheral itself will still keep retrying
 *         under the hood until either it succeeds or this function's
 *         timeout is hit.
 */
bool CAN_Transmit(CAN_Message_t *msg);

/**
 * @brief  Reads a pending standard-ID CAN data frame from FIFO0.
 * @param  msg  Pointer to structure to fill with received data.
 * @retval true  if a message was available and copied into msg.
 * @retval false if FIFO0 was empty.
 * @note   Non-blocking. Assumes standard ID, data frame (not RTR).
 */
bool CAN_Receive(CAN_Message_t *msg);

/*=========================================================
  OBD-II (Mode 01) Helper Functions
=========================================================*/

/**
 * @brief  Builds and sends a Mode 01 OBD-II PID request on the
 *         standard functional broadcast ID (0x7DF).
 * @param  TxMsg  Pointer to a message buffer to build and transmit.
 * @param  PID    The OBD-II parameter ID being requested (e.g. 0x0C
 *                for RPM, 0x0D for speed, 0x05 for coolant temp).
 * @retval true  if the request frame was transmitted successfully.
 * @retval false if transmission failed (see CAN_Transmit).
 */
bool OBD_RequestPID(CAN_Message_t *TxMsg, uint8_t PID);

/**
 * @brief  Waits for and validates an OBD-II ECU response frame.
 * @param  RxMsg  Pointer to a message buffer to receive into.
 * @retval true   if a valid Mode 01 response (ID 0x7E8, Data[1] ==
 *                0x41) was received before the timeout.
 * @retval false  if the poll timed out, or the frame received did
 *                not match the expected ECU response ID/mode.
 * @note   Bounded wait: polls CAN_Receive up to a fixed iteration
 *         count before giving up, so it will not hang forever if
 *         the ECU never responds.
 */
bool OBD_ReadResponse(CAN_Message_t *RxMsg);

/**
 * @brief  Extracts engine RPM from a Mode 01 PID 0x0C response.
 * @param  RxMsg  Pointer to a message already validated by
 *                OBD_ReadResponse.
 * @retval RPM value, or 0 if RxMsg is not a PID 0x0C response.
 */
uint16_t OBD_GetRPM(CAN_Message_t *RxMsg);

/**
 * @brief  Extracts vehicle speed (km/h) from a Mode 01 PID 0x0D
 *         response.
 * @param  RxMsg  Pointer to a message already validated by
 *                OBD_ReadResponse.
 * @retval Speed in km/h, or 0 if RxMsg is not a PID 0x0D response.
 */
uint8_t OBD_GetVehicleSpeed(CAN_Message_t *RxMsg);

/**
 * @brief  Extracts coolant temperature (°C) from a Mode 01 PID
 *         0x05 response.
 * @param  RxMsg  Pointer to a message already validated by
 *                OBD_ReadResponse.
 * @retval Temperature in °C, or 0 if RxMsg is not a PID 0x05
 *         response (note: 0 is also a valid temperature reading,
 *         so callers needing to distinguish "no data" from "0°C"
 *         should check Data[2] themselves before calling).
 */
uint8_t OBD_GetCoolantTemperature(CAN_Message_t *RxMsg);

uint8_t OBD_GetThrottlePosition(CAN_Message_t *RxMsg);


#endif /* CAN_H */