100 PRINT "CANview program - shows all received CAN messages on screen"
110 PRINT " "
120 REM declare variables for message reception
130 DIM rxId : DIM rxType : DIM rxLen : DIM rxData(8) : DIM rxOk
140 REM set CAN baudrate to 500 kbps and connect to the CAN bus
150 CANOPEN 500000
160 REM start the program loop
170 DO
180 REM quit program when user pressed 'q'
190 IF INKEY$ = "q" THEN EXIT
200 REM check if a message was received
210 CANRCV rxId, rxType, rxLen, rxData(0), rxOk
220 IF rxOk = 1 THEN
230 REM process message data
240 id$ = HEX$(rxId)
250 IF rxType = 0 THEN type$ = "STD" ELSE type$ = "EXT"
260 len$ = STR$(rxLen)
270 d0$ = HEX$(rxData(0))+SPACE$(1)
280 d1$ = HEX$(rxData(1))+SPACE$(1)
290 d2$ = HEX$(rxData(2))+SPACE$(1)
300 d3$ = HEX$(rxData(3))+SPACE$(1)
310 d4$ = HEX$(rxData(4))+SPACE$(1)
320 d5$ = HEX$(rxData(5))+SPACE$(1)
330 d6$ = HEX$(rxData(6))+SPACE$(1)
340 d7$ = HEX$(rxData(7))+SPACE$(1)
350 REM display the message
360 PRINT id$,type$,len$,d0$;d1$;d2$;d3$;d4$;d5$;d6$;d7$
370 ENDIF
380 LOOP
390 CANCLOSE
400 END
