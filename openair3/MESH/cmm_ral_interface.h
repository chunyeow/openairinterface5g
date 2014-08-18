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
  
  Address      : Eurecom, Campus SophiaTech, 450 Route des Chappes, CS 50193 - 06410 Biot Sophia Antipolis cedex, FRANCE

 *******************************************************************************/

/**  @addtogroup _mesh_layer3_ 
 @{
 */


/** 
  \brief Establish a new classification rule.
  @returns status indication
*/
int cmm_class_setup_req(L3ID_T L3id,           //!< L3 ID type (IPv4 Addr,IPv6 Addr,MPLS Labels)
			void *InA,             //!< L3 ID A
			void *InB,             //!< L3 ID B
			RBID_T Rbid,           //!< L2 RBID
			L3QOS_T L3QoS,         //!< L3 QoS type (DSCP,EXP,ICMPV6)
			void *QoS_id,          //!< L3 QoS id (DSCP #, EXP #, ICMPv6 message type)
			Transaction_t Trans_id  //!< Transaction ID
			);

/**
  \brief Release a classification rule which was previously established.
  @returns status indication
*/
int cmm_class_release_req(L3ID_T L3id,           //!< L3 ID type (IPv4 Addr,IPv6 Addr,MPLS Labels)
			  void *InA,             //!< L3 ID A
			  void *InB,             //!< L3 ID B
			  RBID_T Rbid,           //!< L2 RBID
			  L3QOS_T L3QoS,         //!< L3 QoS type (DSCP,EXP,ICMPV6)
			  void *QoS_id,          //!< L3 QoS id (DSCP #, EXP #, ICMPv6 message type)
			  Transaction_t Trans_id  //!< Transaction ID
			  );


/** @} */
