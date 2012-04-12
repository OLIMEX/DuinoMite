100 PRINT "CANecho program - sends all received CAN messages back"
110 PRINT " "
120 REM declare variables for message reception and transmission
130 DIM rxId : DIM rxType : DIM rxLen : DIM rxData(8) : DIM rxOk : DIM txOk
140 REM set CAN baudrate to 500 kbps and connect to the CAN bus
150 CANOPEN 500000
160 REM start the program loop
170 DO
180 REM quit program when user pressed 'q'
190 IF INKEY$ = "q" THEN EXIT
200 REM check if a message was received
210 CANRCV rxId, rxType, rxLen, rxData(0), rxOk
220 IF rxOk = 1 THEN
230 REM echo the message back
240 CANSEND rxId, rxType, rxLen, rxData(0), txOk
250 IF txOk = 1 THEN
260 PRINT "CAN message with id "+HEX$(rxId)+" transmitted"
270 ELSE
280 PRINT "CAN mesage could not be transmitted"
290 ENDIF
300 ENDIF
310 LOOP
320 CANCLOSE
330 END
