#!/usr/bin/python

import os, sys, warnings
import serial

def sendToTTY(lineP):
    sys.stdout.write("\nSend:\n" + lineP+"\n")
    g_ser.write(lineP)
    g_ser.flush()



g_ser = serial.Serial('/dev/ttyUSB0', 9600, timeout=1)

buffer = ''
while True:
    buffer += g_ser.read(1)
    #sys.stdout.write("buffer is now:\n" + buffer+"\n")
    buffer = buffer.lstrip('\r')
    if '\r' in buffer:
        at_received, buffer = buffer.split('\r')[-2:]

        at_received = at_received.strip('\n')
        at_received = at_received.strip('\r')

        if at_received == '\n' or at_received == '' or at_received == '\r':
            break
        else:
            sys.stdout.write("Receive stripped:\n" + at_received+"\n")
            # Q0 = enable codes
            # Q1 = disable codes
            if at_received == 'ATE0Q0V1':
                sendToTTY('\rOK\r')

            # ATS0, Automatic Answer
            # This S-parameter controls the automatic answering feature of the modem
            # 0 automatic answering is disabled
            # 1 - 255 enable automatic answering on the ring number specified
            # here value received is 0
            elif at_received.startswith('ATS0='):
                sendToTTY('\rOK\r')

            # AT+CMEEReport mobile equipment error
            #0 Disable +CME ERROR result code. Use ERROR instead.
            #1 Enable +CME ERROR result code and use numeric<err> values
            #2 Enable +CME ERROR result code and use verbose<err> values
            # here value received is 1
            elif at_received.startswith('AT+CMEE='):
                sendToTTY('\rOK\r')

            #  Network registration events
            # AT+CREGNetwork registration (ver. 2)
            # Description:
            #    Controls the presentation of an unsolicited result code +CREG : <stat>
            #    when <n>=1 and there is a change in the ME network registration status or
            #    code +CREG: <stat>[,<lac>,<ci>] when <n>=2 and there is a change of thenetwork cell.
            #    Read command returns the status of result code presentation and an integer <stat>,
            #    which shows whether the network has currently indicated the registration of the ME.
            #    Location information elements <lac> and <ci> are returned only when <n>=2 and ME
            #    is registered in the network.
            # <n>:
            #     0 Disable network registration unsolicited result code. Default value
            #     1 Enable network registration unsolicited result code,+CREG: <stat>
            #     2 Enable network registration and location information unsolicited result code, +CREG:<stat>[,<lac>,<ci>]
            # <stat>:
            #     0 not registered new operator to registered and not searching
            #     1 registered, home network
            #     2 not registered, currently searching a new operator to register with
            #     3 registration denied
            #     4 unknown
            #     5 registered, roaming
            elif at_received == 'AT+CREG=2':
                sendToTTY('\rOK\r')
                sendToTTY('+CREG: 1,00C3,01021A02 \r')

            elif at_received == 'AT+CREG=1':
                sendToTTY('\rOK\r')
                sendToTTY('+CREG: 1\r')

            elif at_received == 'AT+CREG?':
                sendToTTY('+CREG: 1,00C3,01021A02\r\nOK\r')



            #  GPRS registration events
            # +CGREGNetwork registration reporting
            # Packet domain network registration status AT+CGPADDRShow PDP address
            # Description:
            #     Controls the presentation of the unsolicited result code +CGREG: <stat>
            #    when <n>='1' and there is a change in the ME GPRS network registration status or
            #    +CGREG: <stat>[,<lac>,<ci>] when <n>='2' and there is achange of the network cell.
            #    ---
            #    Note: If the GPRS MT also supports circuit mode services,
            #    AT+CREG and the +CREG result code apply to the registration status and location
            #    information for those services. The read command returns the status of result
            #    code presentation and an integer <stat> which shows whether the network has
            #    currently indicated the registration of the ME.
            #    Location information elements <lac> and <ci> are returned only when <n>=2
            #    and ME is registered in the network.
            elif at_received == 'AT+CGREG=1':
                sendToTTY('\rOK\r')
                sendToTTY('+CGREG: 1\r')

            elif at_received == 'AT+CGREG?':
                sendToTTY('+CGREG: 0,1\r\nOK\r')



            #  Call Waiting notifications
            elif at_received == 'AT+CCWA=1':
                sendToTTY('\rOK\r')

            #  Alternating voice/data off
            elif at_received == 'AT+CMOD=0':
                sendToTTY('\rOK\r')

            #  +CSSU unsolicited supp service notifications
            elif at_received == 'AT+CSSN=0,1':
                sendToTTY('\rOK\r')

            #  no connected at_received identification
            elif at_received == 'AT+COLP=0':
                sendToTTY('+COLP:0\r\nOK\r')

            #  HEX character set
            #  AT+CSCSSelect TE character set (ver. 3)
            elif at_received == 'AT+CSCS=\"HEX\"':
                sendToTTY('\rOK\r')

            #  USSD unsolicited
            #  AT+CUSD       Unstructured supplementary service data (ver. 2)
            #  <n>
            #  0       Disable result code presentation in the TA.Defaultvalue
            #  1       Enable result code presentation in the TA
            #  2       Terminate (abort) USSD dialogue. This value is not applicable to the read command response. Notsupported
            elif at_received == 'AT+CUSD=1':
                sendToTTY('\rOK\r')

            #  Enable +CGEV GPRS event notifications, but don't buffer
            #  AT+CGEREPPacket domain event reporting (ver. 1)
            elif at_received == 'AT+CGEREP=1,0':
                sendToTTY('\rOK\r')

            #  SMS PDU mode
            #   AT+CMGF  Message format (ver. 1)
            elif at_received == 'AT+CMGF=0':
                sendToTTY('\rOK\r')

            elif at_received == 'AT+CPIN?':
                sendToTTY('\r+CPIN: READY\r\nOK\r')
                #sendToTTY('\rOK\r')

            #  AT+CFUNSet phone functionality (ver. 2)
            # 0 Minimum functionality, that is, the phone is turnedoff. Default value
            # 1 Full functionality, that is, the phone is turned on
            # 2 Disable phone transmit RF circuits only. Notsupported
            # 3 Disable phone receive RF circuits only. Notsupported
            # 4 Disable phone transmit and receive RF circuits.Note: This is often referred to as "flight mode"
            # 5 GSM only (WCDMA radio off)
            # 6 WCDMA only (GSM radio off)
            elif at_received == 'AT+CFUN?':
                sendToTTY('+CFUN: 1\r\nOK\r')

            elif at_received == 'AT+CFUN=1':
                sendToTTY('\rOK\r')

            # AT+CLCCList current calls
            # Description:
            #    Returns the list of current calls. If command succeeds but no calls areavailable, no information response is sent to TE
            elif at_received == 'AT+CLCC':
                sendToTTY('+CLCC: \r\nOK\r')

            # AT+COPSOperator selection (ver. 2)
            # AT+COPS=[<mode>[,<format>[,<oper>[,AcT]]]]
            #
            # <mode>                    Description
            #  0                        Automatic (<oper> field is ignored.) Default value
            #  1                        Manual (<oper> field will be present)
            #  2                        Deregister from network. Not supported
            #  3                        Set only <format> (for read command +COPS?), donot attempt registration/deregistration (<oper> field is ignored).
            #                           This value is not applicable in readcommand response
            #  4                        Manual/automatic (<oper> field will be present). Ifmanual selection fails, automatic mode (<mode>=0)is entered
            elif at_received == 'AT+COPS=3,0;+COPS?;+COPS=3,1;+COPS?;+COPS=3,2;+COPS?':
                sendToTTY('+COPS: 0,0, \"Free - Mobile\"\r\n+COPS: 0,1, \"Free\"\r\n+COPS: 0,2, \"310170\"\r\nOK\r')

            elif at_received == 'AT+COPS?':
                #sendToTTY('+COPS: 0,0, \"Free - Mobile\"\r\n+COPS: 0,1, \"Free\"\r\n+COPS: 0,2, \"310170\"\r\nOK\r')
                sendToTTY('+COPS: 0,0, \"Free - Mobile\"\r\nOK\r')

            elif at_received == 'AT+COPS=0':
                    #sendToTTY('+COPS: 0,0, \"Free - Mobile\"\r\n+COPS: 0,1, \"Free\"\r\n+COPS: 0,2, \"310170\"\r\nOK\r')
                    sendToTTY('+COPS: 0,0, \"Free - Mobile\"\r\nOK\r')

            #  IMEI
            elif at_received == 'AT+CGSN':
                sendToTTY('+CGSN:534500314160207\r\nOK\r')

            #  Signal quality (ver.1)
            #  Returns received signal strength indication <rssi> and channel bit errorrate <ber> from the phone.
            elif at_received == 'AT+CSQ':
                #31,7
                sendToTTY('+CSQ:8,7\r\nOK\r')



            else:
                sendToTTY('\rOK\r')


