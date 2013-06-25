/*!
*******************************************************************************

\file       emul_interface.h

\brief      Fichier d'entête 

            Il contient les declarations de type, des defines relatif aux 
            fonctions d'emulation  des interfaces du RRM (Radio Resource 
            Management ).

\author     BURLOT Pascal

\date       15/07/08

   
\par     Historique:
            $Author$  $Date$  $Revision$
            $Id$
            $Log$

*******************************************************************************
*/


#ifndef EMUL_INTERFACE_H
#define EMUL_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif

extern pthread_mutex_t   cmm_transact_exclu;
extern pthread_mutex_t   rrc_transact_exclu;

extern unsigned int      cmm_transaction; 
extern unsigned int      rrc_transaction;

extern pthread_mutex_t   actdiff_exclu;
extern actdiff_t        *list_actdiff;
extern unsigned int      cnt_actdiff;

extern transact_t       *cmm_transact_list; 
extern transact_t       *rrc_transact_list;

#ifdef __cplusplus
}
#endif

#endif /* EMUL_INTERFACE_H */
