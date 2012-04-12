100 PRINT "CANgen program - sends a periodic CAN message"
110 PRINT " "
120 REM declare variables for message transmission
130 DIM txData(1) : DIM txOk : DIM txCounter
140 txCounter = 0
150 REM set CAN baudrate to 500 kbps and connect to the CAN bus
160 CANOPEN 500000
170 REM start periodic timer
180 SETTICK 1000,270
190 REM start the program loop
200 DO
210 REM quit program when user pressed 'q'
220 IF INKEY$ = "q" THEN EXIT
230 LOOP
240 REM stop periodic timer
250 SETTICK 0,0
260 CANCLOSE
265 END
270 REM transmit the message
275 txData(0) = txCounter
280 CANSEND &H15e,0,1,txData(0),txOk
290 REM increment the counter
295 PRINT "CAN message with counter value "+STR$(txCounter)+" transmitted"
300 txCounter = txData(0) + 1
310 IF txCounter > 255 THEN txCounter = 0
320 IRETURN
