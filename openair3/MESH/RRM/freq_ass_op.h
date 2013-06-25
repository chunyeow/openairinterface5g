/*!
*******************************************************************************

\file    	freq_ass_op.h

\brief   	Fichier d'entete contenant les declarations des types, des defines ,
			et des fonctions relatives a la gestion des frequences.

\author  	IACOBELLI Lorenzo

\date    	21/10/09

   
\par     Historique:
			$Author$  $Date$  $Revision$
			$Id$
			$Log$

*******************************************************************************
*/

#ifndef FREQ_ASS_OP_H
#define FREQ_ASS_OP_H

#ifdef __cplusplus
extern "C" {
#endif

//mod_lor_10_05_28: declaration of function for sensing and frequency allocation    
unsigned int evalaute_sens_info(
    Sens_node_t *Sens_db,
    unsigned int Start_f,
    unsigned int Final_f
    );

void take_decision( 
	Sens_node_t *SensDB     , //!< pointer to the sensing database
	unsigned int Ch_id      , //!< channel ID
	unsigned int *is_free  //mod_lor_10_05_28 ->char instead of int
	
	);
	
unsigned int take_decision_sens( 
	Sens_node_t *SensDB     , //!< pointer to the sensing database
	Sens_ch_t *finalSensChann, //!< info that will be returned after decision
	unsigned int Ch_id       //!< channel ID
	
	);
	
unsigned int find_available_channels(
    Sens_node_t *Sens_db,
    CHANNELS_DB_T **channels_db
    );
    
unsigned int check_SN_channels(
    Sens_node_t *Sens_db,
    CHANNELS_DB_T *channels_db,
    unsigned int *used_channels,
    unsigned int nb_used_ch
    );
    
void take_local_decision(
    Sens_ch_t *Sens_info
    );
    
unsigned int take_ch_coll_decision( 
	Sens_node_t *SensDB     , //!< pointer to the sensing database
	Sens_ch_t *finalSensChann, //!< info that will be returned after decision
	unsigned int Ch_id       //!< channel ID
	);
	
void disconnect_user( 
    Instance_t    inst     , //!< identification de l'instance
    L2_ID         L2_id      //!< L2_id of the SU 
    );


// ---------------------------------------------------------------------------


#ifdef __cplusplus
}
#endif

#endif /* FREQ_ASS_OP_H */
