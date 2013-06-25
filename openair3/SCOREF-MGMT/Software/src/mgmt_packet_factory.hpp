/*******************************************************************************

  Eurecom OpenAirInterface
  Copyright(c) 1999 - 2012 Eurecom

  This program is free software; you can redistribute it and/or modify it
  under the terms and conditions of the GNU General Public License,
  version 2, as published by the Free Software Foundation.

  This program is distributed in the hope it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
  more details.

  You should have received a copy of the GNU General Public License along with
  this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.

  The full GNU General Public License is included in this distribution in
  the file called "COPYING".

  Contact Information
  Openair Admin: openair_admin@eurecom.fr
  Openair Tech : openair_tech@eurecom.fr
  Forums       : http://forums.eurecom.fsr/openairinterface
  Address      : Eurecom, 2229, route des crêtes, 06560 Valbonne Sophia Antipolis, France

*******************************************************************************/

/*!
 * \file mgmt_packet_factory.hpp
 * \brief A container with necessary (mostly responses) packet generation functionality
 * \company EURECOM
 * \date 2012
 * \author Baris Demiray
 * \email: baris.demiray@eurecom.fr
 * \note none
 * \bug none
 * \warning none
*/

#ifndef MGMT_PACKET_FACTORY_HPP_
#define MGMT_PACKET_FACTORY_HPP_

#include "packets/mgmt_fac_packet_comm_profile_selection_request.hpp"
#include "packets/mgmt_gn_packet_comm_profile_request.hpp"
#include "packets/mgmt_gn_packet.hpp"
#include "mgmt_information_base.hpp"
#include "util/mgmt_log.hpp"
#include "mgmt_types.hpp"

/**
 * A container with necessary (mostly responses) packet generation functionality
 */
class ManagementPacketFactory {
	public:
		/**
		 * Constructor for ManagementPacketFactory class
		 *
		 * @param mib Management Information Base reference
		 * @param logger Logger object reference
		 */
		ManagementPacketFactory(ManagementInformationBase& mib, Logger& logger);

	public:
		/**
		 * Creates and returns a Set Configuration packet whose type is determined
		 * by the default parameter
		 *
		 * @param itsKeyID ITS key ID, if this has the default value MGMT_GN_ITSKEY_ALL
		 * then this method creates a bulk response, otherwise generated packet will carry
		 * specified ITS key only
		 * @return Pointer to the GeonetPacket created
		 */
		GeonetPacket* createSetConfigurationEventPacket(ItsKeyID itsKeyID = MGMT_GN_ITSKEY_ALL);
		/**
		 * Creates a Communication Profile Response packet according to what
		 * was asked in relevant request packet
		 *
		 * @param request Pointer to the Communication Profile Request packet
		 * @return Pointer to the Communication Profile Response packet
		 */
		GeonetPacket* createCommunicationProfileResponse(GeonetCommunicationProfileRequestPacket* request);
		/**
		 * Creates a Communication Profile Selection Response packet according to what
		 * was asked in relevant request packet
		 *
		 * @param request Pointer to the Communication Profile Selection Request packet
		 * @return Pointer to the Communication Profile Selection Response packet
		 */
		GeonetPacket* createCommunicationProfileSelectionResponse(FacCommunicationProfileSelectionRequestPacket* request);

	private:
		/**
		 * ManagementInformationBase reference used to fetch necessary information
		 * to create certain messages/replies
		 */
		ManagementInformationBase& mib;
		/**
		 * Logger object reference
		 */
		Logger& logger;
};

#endif /* MGMT_PACKET_FACTORY_HPP_ */
