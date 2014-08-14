/*******************************************************************************
    OpenAirInterface 
    Copyright(c) 1999 - 2014 Eurecom

    OpenAirInterface is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.


    OpenAirInterface is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with OpenAirInterface.The full GNU General Public License is 
   included in this distribution in the file called "COPYING". If not, 
   see <http://www.gnu.org/licenses/>.

  Contact Information
  OpenAirInterface Admin: openair_admin@eurecom.fr
  OpenAirInterface Tech : openair_tech@eurecom.fr
  OpenAirInterface Dev  : openair4g-devel@eurecom.fr
  
  Address      : Eurecom, Campus SophiaTech, 450 Route des Chappes, CS 50193 - 06904 Biot Sophia Antipolis cedex, FRANCE

 *******************************************************************************/

/* This message asks for task initialization */
MESSAGE_DEF(INITIALIZE_MESSAGE, MESSAGE_PRIORITY_MED, IttiMsgEmpty, initialize_message)

/* This message asks for task activation */
MESSAGE_DEF(ACTIVATE_MESSAGE,   MESSAGE_PRIORITY_MED, IttiMsgEmpty, activate_message)

/* This message asks for task deactivation */
MESSAGE_DEF(DEACTIVATE_MESSAGE, MESSAGE_PRIORITY_MED, IttiMsgEmpty, deactivate_message)

/* This message asks for task termination */
MESSAGE_DEF(TERMINATE_MESSAGE,  MESSAGE_PRIORITY_MAX, IttiMsgEmpty, terminate_message)

/* Test message used for debug */
MESSAGE_DEF(MESSAGE_TEST,       MESSAGE_PRIORITY_MED, IttiMsgEmpty, message_test)

/* Error message  */
MESSAGE_DEF(ERROR_LOG,          MESSAGE_PRIORITY_MAX, IttiMsgEmpty, error_log)
/* Warning message  */
MESSAGE_DEF(WARNING_LOG,        MESSAGE_PRIORITY_MAX, IttiMsgEmpty, warning_log)
/* Notice message  */
MESSAGE_DEF(NOTICE_LOG,         MESSAGE_PRIORITY_MED, IttiMsgEmpty, notice_log)
/* Info message  */
MESSAGE_DEF(INFO_LOG,           MESSAGE_PRIORITY_MED, IttiMsgEmpty, info_log)
/* Debug message  */
MESSAGE_DEF(DEBUG_LOG,          MESSAGE_PRIORITY_MED, IttiMsgEmpty, debug_log)

/* Generic log message for text */
MESSAGE_DEF(GENERIC_LOG,        MESSAGE_PRIORITY_MED, IttiMsgEmpty, generic_log)
