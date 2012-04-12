/***********************************************************************************************************************
 Duinomite

 CAN.c

 This module manages the CAN controller

 Copyright 2012 Frank Voorburg - http://www.feaser.com
 This program is free software: you can redistribute it and/or modify it under the terms of the GNU General
 Public License as published by the Free Software Foundation, either version 3 of the License, or (at your
 option) any later version.

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
 implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 for more details.  You should have received a copy of the GNU General Public License along with this program.
 If not, see <http://www.gnu.org/licenses/>.

***********************************************************************************************************************/


/***********************************************************************************************************************
 Include files
***********************************************************************************************************************/
#define INCLUDE_FUNCTION_DEFINES
#include <p32xxxx.h>
#include <plib.h>
#include "../MMBasic/MMBasic.h"
#include "../Maximite.h"


/***********************************************************************************************************************
 Configuration
***********************************************************************************************************************/
#define CAN_CFG_TRANSMIT_FIFO_SIZE       (32u) // fifo size for data transmission
#define CAN_CFG_RECEIVE_FIFO_SIZE        (32u) // fifo size for data reception


/***********************************************************************************************************************
 Macro definitions
***********************************************************************************************************************/
#define CAN_EXT_ID_BIT_MASK              (0x80000000ul)  // bit to indicate that the id is 29-bit instead of 11-bit
#define CAN_COMMUNICATION_SPEED_MIN      (10000ul)       // 10 kbps
#define CAN_COMMUNICATION_SPEED_MAX      (1000000ul)     // 1 Mbps
#define CAN_BAUD_TOLERANCE_PCT           (2u)            // max tolerance on the baudrate configuration
#define CAN_MESSAGE_FIFO_SIZE            (16u)           // # of bytes in the fifo for a CAN message
#define CAN_MESSAGE_MAX_DATA_LEN         (8u)            // maximum number of bytes allowed in a CAN message
#define CAN_TOTAL_FIFO_SIZE              ((CAN_CFG_TRANSMIT_FIFO_SIZE + CAN_CFG_RECEIVE_FIFO_SIZE) * \
                                           CAN_MESSAGE_FIFO_SIZE)


/***********************************************************************************************************************
 Type definitions
***********************************************************************************************************************/
/* bit timing structure */
typedef struct t_can_bit_timing
{
  unsigned char prop;   // CAN propagation segment
  unsigned char phase1; // CAN phase segment 1
  unsigned char phase2; // CAN phase segment 2
} tCanBitTiming;


/***********************************************************************************************************************
 Function prototypes
***********************************************************************************************************************/
static unsigned char CanOpen(unsigned long speed);
static void          CanClose(void);
static unsigned char CanTransmit(unsigned long id, unsigned char len, unsigned char *data);
static unsigned char CanReceive(unsigned long *id, unsigned char *len, unsigned char *data);
static unsigned char CanGetSpeedConfig(unsigned long baud, CAN_BIT_CONFIG *canBitConfig);


/***********************************************************************************************************************
 Local constant declarations
***********************************************************************************************************************/
/* According to the CAN protocol 1 bit-time can be made up of between 8..25 time quanta
 * (TQ). The total TQ in a bit is SYNC + PROP + PHASE1 + PHASE2 with SYNC always being 1.
 * The sample point is (SYNC + PROP + PHASE1) / (SYNC + PROP + PHASE1 + PHASE2) * 100%.
 * This array contains possible and valid time quanta configurations with a sample point
 * between 60..78%.
 */
static const tCanBitTiming canTiming[] =
{                       /*  TQ | PROP | PHASE1 | PHASE2 | SP  */
                        /* ---------------------------------- */
    { 1, 1, 2 },       /*   5 |   1  |   1    |   2    | 60% */
    { 1, 2, 2 },       /*   6 |   1  |   2    |   2    | 67% */
    { 2, 2, 2 },       /*   7 |   2  |   2    |   2    | 71% */
    { 3, 2, 2 },       /*   8 |   3  |   2    |   2    | 75% */
    { 4, 2, 2 },       /*   9 |   4  |   2    |   2    | 78% */
    { 3, 3, 3 },       /*  10 |   3  |   3    |   3    | 70% */
    { 4, 3, 3 },       /*  11 |   4  |   3    |   3    | 73% */
    { 5, 3, 3 },       /*  12 |   5  |   3    |   3    | 75% */
    { 6, 3, 3 },       /*  13 |   6  |   3    |   3    | 77% */
    { 5, 4, 4 },       /*  14 |   5  |   4    |   4    | 71% */
    { 6, 4, 4 },       /*  15 |   6  |   4    |   4    | 73% */
    { 7, 4, 4 },       /*  16 |   7  |   4    |   4    | 75% */
    { 8, 4, 4 },       /*  17 |   8  |   4    |   4    | 76% */
    { 7, 5, 5 },       /*  18 |   7  |   5    |   5    | 72% */
    { 8, 5, 5 },       /*  19 |   8  |   5    |   5    | 74% */
    { 8, 6, 5 },       /*  20 |   8  |   6    |   5    | 75% */
    { 8, 7, 5 },       /*  21 |   8  |   7    |   5    | 76% */
    { 8, 7, 6 },       /*  22 |   8  |   7    |   6    | 73% */
    { 8, 8, 6 },       /*  23 |   8  |   8    |   6    | 74% */
    { 8, 8, 7 },       /*  24 |   8  |   8    |   7    | 71% */
    { 8, 8, 8 }        /*  25 |   8  |   8    |   8    | 68% */
};


/***********************************************************************************************************************
 Local data declarations
***********************************************************************************************************************/
static unsigned char canMessageFifoArea[CAN_TOTAL_FIFO_SIZE];	// FIFO message area
static unsigned char canOpened; // flag to keep track of CAN controller state


/***********************************************************************************************************************
 Initializes the CAN driver. This function is typically called once upon system initialization.

 Args:    none

 Returns: none
***********************************************************************************************************************/
void CanInit(void)
{
    // enable the CAN controller
    CANEnableModule(CAN1,TRUE);
    // set CAN controller state to closed by default
    canOpened = 0;
}


/***********************************************************************************************************************
 Configures the CAN controller for the specified communication speed. By default the acceptance filter is configured
 to accept all CAN message identifiers, both 11-bit STD and 29-bit EXT. Once the configuration is complete the CAN
 controller synchronizes to the CAN bus. The configuration of the CAN controller's channels is:
   - CAN_CHANNEL0 -> transmission of CAN messages.
   - CAN_CHANNEL1 -> reception of CAN messages.

 Args:    speed - CAN communication speed in bits/sec. should be in the range 10kbps - 1Mbps

 Returns: 1 if successful, 0 otherwise.
***********************************************************************************************************************/
static unsigned char CanOpen(unsigned long speed)
{
    CAN_BIT_CONFIG canBitConfig;

    // take the CAN controller offline and switch to configuration mode
    CanClose();
    // configure the bittiming
    if (CanGetSpeedConfig(speed, &canBitConfig) == 0)
    {
        // cannot find a fitting bittiming configuration for this baudrate
        return 0;
    }
    CANSetSpeed(CAN1, &canBitConfig, CLOCKFREQ, speed);
    // assign buffer area to the CAN module
    CANAssignMemoryBuffer(CAN1, canMessageFifoArea, CAN_TOTAL_FIFO_SIZE);
    // configure channel 0 for transmission, RTR disabled and low modium priority
    CANConfigureChannelForTx(CAN1, CAN_CHANNEL0, CAN_CFG_TRANSMIT_FIFO_SIZE, CAN_TX_RTR_DISABLED, \
                             CAN_LOW_MEDIUM_PRIORITY);
    // configure channel 1 for message reception
    CANConfigureChannelForRx(CAN1, CAN_CHANNEL1, CAN_CFG_RECEIVE_FIFO_SIZE, CAN_RX_FULL_RECEIVE);
    // configure acceptance filter to accept all messages, both 11-bit STD and 29-bit EXT
    CANConfigureFilterMask  (CAN1, CAN_FILTER_MASK0, 0, CAN_EID, CAN_FILTER_MASK_ANY_TYPE);
    CANLinkFilterToChannel  (CAN1, CAN_FILTER0, CAN_FILTER_MASK0, CAN_CHANNEL1);
    CANEnableFilter         (CAN1, CAN_FILTER0, TRUE);
    // exit configuration mode and bring the CAN controller online
    CANSetOperatingMode(CAN1, CAN_NORMAL_OPERATION);
    while(CANGetOperatingMode(CAN1) != CAN_NORMAL_OPERATION) { ; }
    // set CAN controller state to opened
    canOpened = 1;
    // still here so all is okay
    return 1;
}


/***********************************************************************************************************************
 Closes the CAN connection by placing the CAN controller back in its configuration mode.
 Args:    none

 Returns: none
***********************************************************************************************************************/
static void CanClose(void)
{
    // set CAN controller state to closed
    canOpened = 0;
    // take the CAN controller offline and switch to configuration mode
    CANSetOperatingMode(CAN1, CAN_CONFIGURATION);
    while(CANGetOperatingMode(CAN1) != CAN_CONFIGURATION)  { ; }
}


/***********************************************************************************************************************
 Transmits a CAN message if the CAN controller has an idle transmit message slot. Otherwise the message is queued so
 it is automatically transmitted once a transmit message slot becomes available.

 Args:    id   - CAN message identifier. Setting bit 31 (CAN_EXT_ID_BIT_MASK) configures the ID as 29-bit EXT.
          len  - number of data bytes in the CAN message.
          data - pointer to byte array with the CAN message data.

 Returns: 1 if successful, 0 upon error or when the queue is full.
***********************************************************************************************************************/
static unsigned char CanTransmit(unsigned long id, unsigned char len, unsigned char *data)
{
    CANTxMessageBuffer *message;
    unsigned char byteCnt;

    // check that the data length is valid
    if (len > CAN_MESSAGE_MAX_DATA_LEN)
    {
        // length if too large for a CAN message
        return 0;
    }
    // make sure there is a connection with the CAN bus
    if (canOpened == 0)
    {
        // no connection to the bus, likely that CanOpen hasn't been called yet
        return 0;
    }
    // get a pointer to the next fifo buffer for the channel
    message = CANGetTxMessageBuffer(CAN1, CAN_CHANNEL0);
    // check if the fifo is full
    if(message == NULL)
    {
        // transmit fifo full
        return 0;
    }
    // start by clearing the buffer
    message->messageWord[0] = 0;
    message->messageWord[1] = 0;
    message->messageWord[2] = 0;
    message->messageWord[3] = 0;
    // store the message identifier
    if ((id & CAN_EXT_ID_BIT_MASK) == 0)
    {
        // store the 11-bit STD identifier
        message->msgSID.SID = id & 0x7ff;
        message->msgEID.IDE = 0;
    }
    else
    {
        // store the 29-bit EXT identifier
        id &= ~CAN_EXT_ID_BIT_MASK;
        message->msgSID.SID = (id >> 18) & 0x7ff;
        message->msgEID.EID = id & 0x3ffff;
        message->msgEID.IDE = 1;
    }
    // set the dlc
    message->msgEID.DLC	= len;
    // store the message data
    for (byteCnt=0; byteCnt<len; byteCnt++)
    {
        message->data[byteCnt] = data[byteCnt];
    }
    // inform the CAN module that the message is prepared adn ready for further processing
    CANUpdateChannel(CAN1, CAN_CHANNEL0);
    // direct CAN module to pass pending message in the fifo on to the hardware CAN message slot(s)
    CANFlushTxChannel(CAN1, CAN_CHANNEL0);
    // still here so all is okay
    return 1;
}


/***********************************************************************************************************************
 Check if a CAN message was received and if so obtains it from the reception fifo.

 Args:    id   - CAN message identifier. Setting bit 31 (CAN_EXT_ID_BIT_MASK) configures the ID as 29-bit EXT.
          len  - number of data bytes in the CAN message.
          data - pointer to byte array with the CAN message data.

 Returns: 1 if successful, 0 when the fifo is empty.
***********************************************************************************************************************/
static unsigned char CanReceive(unsigned long *id, unsigned char *len, unsigned char *data)
{
    CANRxMessageBuffer *rxMsg;
    unsigned char byteCnt;

    // make sure there is a connection with the CAN bus
    if (canOpened == 0)
    {
        // no connection to the bus, likely that CanOpen hasn't been called yet
        return 0;
    }
    // try to obtain a message from the reception fifo
    rxMsg = (CANRxMessageBuffer *)CANGetRxMessage(CAN1, CAN_CHANNEL1);

    // check if a message was read
    if(rxMsg == NULL)
    {
        // no message present in the fifo
        return 0;
    }
    // determine if the message has an 11-bit STD identifier
    if (rxMsg->msgEID.IDE == 0)
    {
        // store the message 11-bit STD identifier
        *id = rxMsg->msgSID.SID;
    }
    else
    {
        // store the message 29-bit EXT identifier
        *id = rxMsg->msgEID.EID | (rxMsg->msgSID.SID << 18) | CAN_EXT_ID_BIT_MASK;
    }
    // set the message length
    *len = rxMsg->msgEID.DLC;
    // copy the message data
    for (byteCnt=0; byteCnt<*len; byteCnt++)
    {
        data[byteCnt] = rxMsg->data[byteCnt];
    }
    // inform the CAN module that we're done processing this message
    CANUpdateChannel(CAN1, CAN_CHANNEL1);
    // successfully receive a CAN message
    return 1;
}


/***********************************************************************************************************************
 Search algorithm to match the desired baudrate to a possible bus timing configuration

 Args:    baud         - the desired CAN communication speed in bits per second
          canBitConfig - pointer to bit configuration structure where the results should be stored.

 Returns: 1 if successful, 0 when now fitting configuration could be found.
***********************************************************************************************************************/
static unsigned char CanGetSpeedConfig(unsigned long baud, CAN_BIT_CONFIG *canBitConfig)
{
  signed char    cnt;
  unsigned char  tqSetting;
  unsigned long  baudActual;
  signed char    deviationPct;
  unsigned short prescaler;

  // loop through all possible time quanta configurations to find a match
  for (cnt=(sizeof(canTiming)/sizeof(canTiming[0]))-1; cnt>=0; cnt--)
  {
      // determine how many timequanta in one bit for this configuration
      tqSetting = (1+canTiming[cnt].prop+canTiming[cnt].phase1+canTiming[cnt].phase2);
      // determine required prescaler
      prescaler = CLOCKFREQ / (baud * tqSetting);
      // determine the actual baudrate this setting would result in
      baudActual = CLOCKFREQ / (prescaler * tqSetting);
      // determine deviation from requested baudrate
      deviationPct = ((signed long)(baudActual - baud) * 100) / baud;
      // get the absolute value because it could be a negative percentage
      deviationPct *= (deviationPct < 0) ? -1 : 1;
      // check if this is within the accepted baudrate tolerance
      if (deviationPct <= CAN_BAUD_TOLERANCE_PCT)
      {
          // store the bittiming configuration
          canBitConfig->phaseSeg2Tq            = canTiming[cnt].phase2 - 1;
          canBitConfig->phaseSeg1Tq            = canTiming[cnt].phase1 - 1;
          canBitConfig->propagationSegTq       = canTiming[cnt].prop - 1;
          canBitConfig->phaseSeg2TimeSelect    = TRUE;
          canBitConfig->sample3Time            = FALSE;
          canBitConfig->syncJumpWidth          = CAN_BIT_1TQ;
          // found a good bus timing configuration
          return 1;
      }
  }
  // could not find a good bus timing configuration
  return 0;
} /*** end of CanGetSpeedConfig ***/


/***********************************************************************************************************************
 MMBasic command handler for command: CANOPEN speed
                                                  Example: "CANOPEN 500000"

 Args:    none

 Returns: none
***********************************************************************************************************************/
void cmd_canOpen(void)
{
    int speed;

    // max 1 argument expected
    getargs(&cmdline, 1, ",");
    // display help in case no arguments are specified
    if(argc == 0)
    {
        error("Not enough arguments. Command syntax: CANOPEN <speed>");
        return;
    }
    if(argc != 1)
    {
        error("Invalid syntax");
        return;
    }
    speed = getinteger(argv[0]);
    // verify the speed parameter
    if ( (speed < CAN_COMMUNICATION_SPEED_MIN) || (speed > CAN_COMMUNICATION_SPEED_MAX) )
    {
        // invalid communication speed specified
        error("Invalid CAN communication speed (must be 10kbps - 1Mbps)");
        return;
    }
    // attempt to open the link with the CAN bus
    if (CanOpen(speed) == 0)
    {
        error("Could not find valid bittiming configuration to match this speed");
    }
}


/***********************************************************************************************************************
 MMBasic command handler for command CANCLOSE

 Args:    none

 Returns: none
***********************************************************************************************************************/
void cmd_canClose(void)
{
    checkend(cmdline);
    CanClose();
}


/***********************************************************************************************************************
 MMBasic command handler for command CANSEND id, type, len, data(), ok
                                             - id   -> CAN message ID (0..7FF for STD and 0..1FFFFF for EXT)
                                             - type -> 0 for 11-bit STD CAN message ID, 1 for 29-bit EXT
                                             - data -> the CAN message data bytes as an array
                                             - ok   -> 1 when a message was transmitted or 0 if the transmit queue
                                                       was full or no connectio with CAN bus is present
                                                      in the reception fifo.
                                                  Examples: "DIM txData(2)"
                                                            "DIM txOk"
                                                            "txData(0) = &H55"
                                                            "txData(1) = &HAA"
                                                            "CANSEND &H123,0,2,txData(0), txOk"

 Args:    none

 Returns: none
***********************************************************************************************************************/
void cmd_canTransmit(void)
{
    unsigned long id;
    unsigned char type;
    unsigned char dlc;
    unsigned char data[CAN_MESSAGE_MAX_DATA_LEN];
    unsigned char byteCnt;
    void         *ptr;
    float        *fp;
    unsigned char result;

    // max 9 arguments expected. make sure to read out enough
    getargs(&cmdline, 12, ",");
    // display help in case no arguments are specified
    if(argc < 9)
    {
        error("Not enough arguments. Command syntax: CANSEND <id>,<type>,<len>,<data()>,<ok>");
        return;
    }
    if(argc > 9)
    {
        error("Too many arguments. Command syntax: CANSEND <id>,<type>,<len>,<data()>,<ok>");
        return;
    }
    // read out the length
    dlc = getinteger(argv[4]);
    if ( (dlc > CAN_MESSAGE_MAX_DATA_LEN) || (dlc == 0) )
    {
        error("Invalid CAN message length (must be 1..8)");
        return;
    }
    // read out the id type
    type = getinteger(argv[2]);
    if (type > 2)
    {
        error("Invalid CAN message type (must be 0 for 11-bit STD or 1 for 29-bit EXT)");
        return;
    }
    // read out the id
    id = getinteger(argv[0]);
    if (type == 0)
    {
        if (id > 0x7ff)
        {
            error("Invalid CAN message id (must be 0..7FFh)");
            return;
        }
    }
    else
    {
        if (id > 0x1fffff)
        {
            error("Invalid CAN message id (must be 0..1FFFFFh)");
            return;
        }
        // set the EXT bit
        id |= CAN_EXT_ID_BIT_MASK;
    }
    // try to acccess the array with the message data
    ptr = findvar(argv[6], V_NOFIND_NULL);
    if (ptr == NULL)
    {
        error("Array argument with CAN message data not found");
        return;
    }
    if (vartbl[VarIndex].type != T_NBR)
    {
        error("CAN message data array is not numeric");
        return;
    }
    if (vartbl[VarIndex].dims[1] != 0)
    {
        error("CAN message data array must be one dimensional");
        return;
    }
    if (vartbl[VarIndex].dims[0] == 0)
    {
        error("CAN message data argument is not an array");
        return;
    }
    if ((((float *) ptr - vartbl[VarIndex].val.fa) + dlc) > (vartbl[VarIndex].dims[0] + 1 - OptionBase))
    {
        error("Insufficient elements in CAN message data array");
        return;
    }
    // init pointer to first data array element
    fp = (float *)ptr;
    // read out the message data
    for (byteCnt=0; byteCnt<dlc; byteCnt++)
    {
        if (fp[byteCnt] > 255)
        {
            error("Invalid CAN data byte (must be 0..255)");
            return;
        }
        data[byteCnt] = fp[byteCnt];
    }
    // attempt to send the CAN message
    result = CanTransmit(id, dlc, data);
    // try to acccess the variable with the ok return value
    ptr = findvar(argv[8], V_NOFIND_NULL);
    if (ptr == NULL)
    {
        error("Variable <ok> not found");
        return;
    }
    // cast pointer to float pointer
    fp = (float *)ptr;
    // no message present in the reception fifo
    if (result == 0)
    {
        // could not transmit the message. either the fifo is full or no open connection to the CAN bus is present
        *fp = 0;
    }
    else
    {
        // successfully transmittted the message
        *fp = 1;
    }
}


/***********************************************************************************************************************
 MMBasic command handler for command CANRCV id, type, len, data(), ok
                                            - id   -> CAN message ID (0..7FF for STD and 0..1FFFFF for EXT)
                                            - type -> 0 for 11-bit STD CAN message ID, 1 for 29-bit EXT
                                            - data -> the CAN message data bytes as an array
                                            - ok   -> 1 when a message was received or 0 if no message was present
                                                      in the reception fifo.
                                                  Examples: "DIM rxId"
                                                            "DIM rxType"
                                                            "DIM rxLen"
                                                            "DIM rxData(8)"
                                                            "DIM rxOk"
                                                            "CANRCV rxId,rxType,rxLen,rxData(0),rxOk"

 Args:    none

 Returns: none
***********************************************************************************************************************/
void cmd_canReceive(void)
{
    unsigned long rxId;
    unsigned char rxDlc;
    unsigned char rxData[CAN_MESSAGE_MAX_DATA_LEN];
    unsigned char rxOk;
    unsigned char byteCnt;
    float         *ptr;

    // max 9 argument expected. make sure to read out enough
    getargs(&cmdline, 12, ",");
    // display help in case no arguments are specified
    if(argc < 9)
    {
        error("Not enough arguments. Command syntax: CANRCV <id>,<type>,<len>,<data()>,<ok>");
        return;
    }
    if(argc > 9)
    {
        error("Too many arguments. Command syntax: CANRCV <id>,<type>,<len>,<data()>,<ok>");
        return;
    }
    // check if there is actually a CAN message present in the reception fifo
    rxOk = CanReceive(&rxId, &rxDlc, rxData);
    // try to acccess the variable with the ok return value
    ptr = findvar(argv[8], V_NOFIND_NULL);
    if (ptr == NULL)
    {
        error("Variable <ok> not found");
        return;
    }
    // no message present in the reception fifo
    if (rxOk == 0)
    {
        // no message was available in the reception queue
        *ptr = 0;
        return;
    }
    // a new message was received
    *ptr = 1;
    // try to acccess the variable where the id type is stored
    ptr = findvar(argv[2], V_NOFIND_NULL);
    if (ptr == NULL)
    {
        error("Variable <type> not found");
        return;
    }
    // set the type
    if ((rxId & CAN_EXT_ID_BIT_MASK) == 0)
    {
        // it's a 11-bit STD id
        *ptr = 0;
    }
    else
    {
        // it's a 29-bit EXT id
        *ptr = 1;
        rxId &= ~CAN_EXT_ID_BIT_MASK;
    }
    // try to acccess the variable where the id is stored
    ptr = findvar(argv[0], V_NOFIND_NULL);
    if (ptr == NULL)
    {
        error("Variable <id> not found");
        return;
    }
    // set the id
    *ptr = rxId;
    // try to acccess the variable where the dlc is stored
    ptr = findvar(argv[4], V_NOFIND_NULL);
    if (ptr == NULL)
    {
        error("Variable <len> not found");
        return;
    }
    // set the dlc
    *ptr = rxDlc;
    // try to acccess the array with the message data
    ptr = findvar(argv[6], V_NOFIND_NULL);
    if (ptr == NULL)
    {
        error("Array argument <data> not found");
        return;
    }
    if (vartbl[VarIndex].type != T_NBR)
    {
        error("Array argument <data> is not numeric");
        return;
    }
    if (vartbl[VarIndex].dims[1] != 0)
    {
        error("Array argument <data> must be one dimensional");
        return;
    }
    if (vartbl[VarIndex].dims[0] == 0)
    {
        error("Array argument <data> is not an array");
        return;
    }
    if ((((float *) ptr - vartbl[VarIndex].val.fa) + rxDlc) > (vartbl[VarIndex].dims[0] + 1 - OptionBase))
    {
        error("Insufficient elements in array argument <data>");
        return;
    }
    // store the message data
    for (byteCnt=0; byteCnt<rxDlc; byteCnt++)
    {
        ptr[byteCnt] = rxData[byteCnt];
    }
}

/************************************************* end of CAN.c *******************************************************/
