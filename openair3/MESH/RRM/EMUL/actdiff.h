/*!
*******************************************************************************

\file       actdiff.h

\brief      Fichier d'entête 

            Il contient les declarations de type, des defines relatif aux 
            fonctions à la gestion des actions differees.

\author     BURLOT Pascal

\date       13/08/08

   
\par     Historique:
            $Author$  $Date$  $Revision$
            $Id$
            $Log$

*******************************************************************************
*/

#ifndef ACTDIFF_H
#define ACTDIFF_H

#ifdef __cplusplus
extern "C" {
#endif

///< \brief action differee ( a traiter ulterieurement )        
typedef struct actdiff_s {
    double            tv   ; ///< date du traitement
    int               id   ; ///< ID de l'action differee
    sock_rrm_t       *sock ; ///< socket associe 
    msg_t            *msg  ; ///< message a envoyer
    struct actdiff_s *next ; ///< pointeur sur la prochaine action differee
} actdiff_t ;

actdiff_t *add_actdiff( 
                    actdiff_t **pEntry , 
                    double delai,
                    int   id,
                    sock_rrm_t *sock,
                    msg_t  *msg
                    );

void processing_actdiff( actdiff_t **pEntry ) ;

#ifdef __cplusplus
}
#endif

#endif /* ACTDIFF_H */
