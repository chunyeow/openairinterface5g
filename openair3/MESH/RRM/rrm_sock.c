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

/*!
*******************************************************************************

\file       rrm_sock.c

\brief      RRM (Radio Ressource Manager ) Socket

            Ceux sont les fonctions relatives à la communication avec les 
            autres entites: 
                - RRC , 
                - CMM , 
                - PUSU

\author     BURLOT Pascal

\date       10/07/08

   
\par     Historique:
            $Author$  $Date$  $Revision$
            $Id$
            $Log$

*******************************************************************************
*/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>


#include "debug.h"
#include "L3_rrc_defs.h"
#include "rrm_util.h"
#include "rrm_sock.h"

//! \brief  Taille maximale de la charge utile
#define SIZE_MAX_PAYLOAD    16384 //mod_lor_10_05_24




#include <sys/socket.h>
#include <netinet/in.h>   //mod_lor_10_01_25
#include <arpa/inet.h>   //mod_lor_10_01_25
#include <sys/un.h>



/*!
*******************************************************************************
\brief  This function opens a unix socket for the rrm communication
        ( no-connected mode / UDP DATAGRAM )
\return  The return value is a socket handle
*/
int open_socket( 
    sock_rrm_t *s   ,   ///< socket descriptor
    char *path_local,   ///< local socket path if unix socket
    char *path_dest ,   ///< host  Socket path if unix socket
    int rrm_inst        ///< instance of the rrm entity
    ) 
{ /* Unix socket */
    int     socket_fd ;
    int     len ;
        
    if ((socket_fd = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1) 
    {
        perror("unix socket");
        return -1 ;
    }
        
    memset(&(s->un_local_addr), 0, sizeof(struct    sockaddr_un));
    s->un_local_addr.sun_family = AF_UNIX;
    sprintf(s->un_local_addr.sun_path,"%s%d", path_local, rrm_inst );
    unlink(s->un_local_addr.sun_path);
        
    len = strlen((s->un_local_addr).sun_path) + sizeof((s->un_local_addr).sun_family);
        
    if (bind(socket_fd, (struct sockaddr *)&(s->un_local_addr), len) == -1) 
    {
        perror("bind");
        return -1 ;
    }
        
    memset(&(s->un_dest_addr), 0, sizeof(struct     sockaddr_un));
    s->un_dest_addr.sun_family = AF_UNIX;
    sprintf(s->un_dest_addr.sun_path,"%s%d", path_dest, rrm_inst );
    
    s->s = socket_fd ;
    return socket_fd ; 
}

/*!
*******************************************************************************
\brief  This function closes a RRM socket 
\return none
*/
void close_socket( 
    sock_rrm_t *sock  ///< the socket handle 
    ) 
{
    shutdown(sock->s, SHUT_RDWR);
    close(sock->s);
}

/*!
*******************************************************************************
\brief  This function send a buffer message to the unix socket  
\return if OK then "0" is returned else "-1"
*/
int send_msg( 
    sock_rrm_t *s   ,///< socket descriptor
    msg_t *msg       ///< the message to send   
    ) 
{ /* Unix socket */
    int                 ret     = 0 ;
    char                *buf    = NULL;
    struct  msghdr      msghd ;
    struct  iovec       iov;
    int                 taille  = sizeof(msg_head_t)  ;
    
    if ( msg == NULL )
        return -1 ;
    
    if ( msg->data != NULL ) 
        taille += msg->head.size ;
        
    buf = RRM_MALLOC(char, taille);
    if (buf ==NULL) 
        ret =  -1 ; 
    else
    {
        memcpy( buf , &(msg->head) , sizeof(msg_head_t) ) ;
        memcpy( buf+sizeof(msg_head_t), msg->data, msg->head.size ) ;
        
        iov.iov_base      = (void *)buf;
        iov.iov_len       = taille ;
        
        msghd.msg_name          = (void *)&(s->un_dest_addr);
        msghd.msg_namelen       = sizeof(s->un_dest_addr);
        msghd.msg_iov           = &iov;
        msghd.msg_iovlen        = 1;            
        msghd.msg_control       = NULL ;
        msghd.msg_controllen    =   0 ;
                
        if ( sendmsg(s->s, &msghd, 0) < 0 )
        {
            ret = -1; 
            perror("sendmsg:unix socket unix");
        }
    }
    
    RRM_FREE(buf) ; 
    RRM_FREE(msg->data) ;
    RRM_FREE(msg) ;
    
    return ret ;
}

/*!
*******************************************************************************
\brief  This function read a buffer from a unix socket 
\return the function returns a message pointer. If the pointeur is NULL, a error
         is happened. 
*/
char *recv_msg( 
    sock_rrm_t *s   ///< socket descriptor
    ) 
{ /* Unix socket */
    char                *buf = NULL;
    char                *msg = NULL;
    struct  msghdr      msghd ;
    struct  iovec       iov;
    int                 size_msg ;
    msg_head_t          *head  ;
    int                 ret ;
    
    int taille =  SIZE_MAX_PAYLOAD ;

    buf                 = RRM_CALLOC( char,taille);
    if ( buf == NULL ) {
        fprintf(stderr,"error recvmsg_un: 0x%02x\n", msghd.msg_flags) ;
        return NULL ;
    }
        
    iov.iov_base        = (void *)buf;
    iov.iov_len         = taille ;
    msghd.msg_name      = (void *)&(s->un_dest_addr);
    msghd.msg_namelen   = sizeof(s->un_dest_addr);
    msghd.msg_iov       = &iov;
    msghd.msg_iovlen    = 1;    
    msghd.msg_control   = NULL ;
    msghd.msg_controllen= 0 ;       

    ret = recvmsg(s->s, &msghd , 0 ) ; 
    if ( ret <= 0  )
    {
        perror("PB recvmsg_un");
        RRM_FREE(buf);
        return NULL ;
    }
    
    if (msghd.msg_flags != 0 )
    {
        fprintf(stderr,"error recvmsg_un: 0x%02x\n", msghd.msg_flags) ;
        RRM_FREE(buf);          
        return NULL ;       
    }

    head        = (msg_head_t *) buf  ;
    size_msg    = sizeof(msg_head_t) + head->size ;
    
    msg         = RRM_CALLOC(char , size_msg ) ;
    if ( msg != NULL )
        memcpy( msg , buf , size_msg ) ;
        
    RRM_FREE( buf ) ;
    
    return msg ;
}

//mod_lor_10_01_25++
/*!
*******************************************************************************
\brief  This function opens a internet socket for the rrm communication
        ( no-connected mode / UDP DATAGRAM ) -> considered address: IPv4
\return  The return value is a socket handle
*/
int open_socket_int( 
    sock_rrm_int_t *s,  ///< socket descriptor
    unsigned char *path_local ,  ///< local socket path if internet socket
    int local_port,     ///< local socket port if internet socket
    unsigned char *path_dest ,   ///< dest socket path if internet socket
    int dest_port,      ///< dest socket port if internet socket
    int rrm_inst        ///< instance of the rrm entity
    ) 
{ /* Internet socket */
    int     socket_fd ;
    int     len ;
    unsigned long int tmp; 
    unsigned char local_test [4];
    if (path_local == NULL)
        fprintf(stderr,"path_local = NULL\n  ");//dbg
    
   
    if ((socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) 
    {
        perror("internet socket");
        return -1 ;
    }
    
  
    //!Setting local address
    memset(&(s->in_local_addr), 0, sizeof(struct    sockaddr_in));
    s->in_local_addr.sin_family = AF_INET;
    s->in_local_addr.sin_port = htons(local_port);
    memcpy(&tmp,path_local,4);
    //fprintf(stderr,"\nOpen sock: path_local %X \n", tmp);//dbg
    s->in_local_addr.sin_addr.s_addr = tmp;

    
    len = sizeof(s->in_local_addr);
 
    if (bind(socket_fd, (struct sockaddr *)&(s->in_local_addr), len) == -1) 
    {
        perror("bind internet");
        return -1 ;
    }
  
    //!Setting destination address
    memset(&(s->in_dest_addr), 0, sizeof(struct    sockaddr_in));
    s->in_dest_addr.sin_family = AF_INET;
    s->in_dest_addr.sin_port = htons(7000);
    memcpy(&tmp,path_dest,4);
    //fprintf(stderr,"Open sock: path_dest %X \n", tmp);//dbg
    s->in_dest_addr.sin_addr.s_addr = tmp;
    
    s->s = socket_fd ;
   //fprintf(stderr,"IP address %X \n", s->in_local_addr.sin_addr.s_addr);//dbg
   // fprintf(stderr,"IP dest %X \n", s->in_dest_addr.sin_addr.s_addr);//dbg
    return socket_fd ; 
}



/*!
*******************************************************************************
\brief  This function read a buffer from a internet socket 
\return the function returns a message pointer. If the pointeur is NULL, a error
         is happened. 
*/
char *recv_msg_int( 
    sock_rrm_int_t *s   ///< socket descriptor
    ) 
{ /* Internet socket */
    char                *buf = NULL;
    char                *msg = NULL;
    int                 size_msg ;
    msg_head_t          *head  ;
    int                 ret ;
    //struct  sockaddr_in newS;
    //socklen_t len_addr = sizeof(struct sockaddr_in);
    
    int taille =  SIZE_MAX_PAYLOAD ;
    //fprintf(stderr,"RF dentro recv_from \n  ");//dbg

    buf                 = RRM_CALLOC( char,taille);
    if ( buf == NULL ) {
        return NULL ;
    }
        
    //fprintf(stderr,"RF s: %d\n  ", s->s);//dbg
    
    
    //ret = recvfrom(s->s, buf, taille, 0,(struct sockaddr *)&(newS), &len_addr) ;
    ret = recvfrom(s->s, buf, taille, 0,NULL, 0) ; //mod_lor_10_04_23
    //fprintf(stderr,"RF dopo recv s = %d\n  ",s->s);//dbg 
    if ( ret <= 0  )
    {
        //fprintf(stderr,"RF ret %d\n  ",ret );//dbg
        perror("PB recvfrom_in");
        RRM_FREE(buf);
        return NULL ;
    }
    
    
    head        = (msg_head_t *) buf  ;
    size_msg    = sizeof(msg_head_t) + head->size ;
    
    msg         = RRM_CALLOC(char , size_msg ) ;
    if ( msg != NULL )
        memcpy( msg , buf , size_msg ) ;
        
    RRM_FREE( buf ) ;
    //fprintf(stderr,"dim_msg %d\n  ",sizeof(msg) );//dbg
    //fprintf(stderr,"RF s at the end: %d\n  ", s->s);//dbg
    return msg ;
}


/*!
*******************************************************************************
\brief  This function send a buffer message to the internet socket  
\return if OK then "0" is returned else "-1"
*/
int send_msg_int( 
    sock_rrm_int_t *s                       ,///< socket descriptor
    msg_t *msg                           ///< the message to send  
    
    ) 
{ /* Internet socket */
    int                 ret     = 0 ;
    char                *buf    = NULL; 
    int                 taille  = sizeof(msg_head_t)  ;
    //fprintf(stderr,"Send IP msg socket -> %d\n  ",s->s );//dbg
    
    if ( msg == NULL )
        return -1 ;
    if ( msg->data != NULL ) 
        taille += msg->head.size ;
        
    buf = RRM_MALLOC(char, taille);
    if (buf ==NULL)
        ret =  -1 ; 
        
    else
    {
        memcpy( buf , &(msg->head) , sizeof(msg_head_t) ) ;
        memcpy( buf+sizeof(msg_head_t), msg->data, msg->head.size ) ;
        //fprintf(stderr,"Sendto IP msg on socket %d msg_type %d\n  ",s->s, msg->head.msg_type );//dbg    
        //fprintf(stderr,"to %X \n\n\n\n\n", s->in_dest_addr.sin_addr.s_addr);//dbg    
        if ( sendto(s->s, buf, taille, 0, (struct  sockaddr *)&(s->in_dest_addr), sizeof(struct  sockaddr_in)) < 0 )
        {
            ret = -1; 
            perror("sendmsg:socket ip");
        }
    }
    //fprintf(stderr,"Before free ret %d\n  ", ret);//dbg
    RRM_FREE(buf) ; 
    RRM_FREE(msg->data) ;
    RRM_FREE(msg) ;
    //fprintf(stderr,"After free ret %d\n  ", ret);//dbg
    return ret ;
}

/*!
*******************************************************************************
\brief  This function closes a RRM socket 
\return none
*/
void close_socket_int( 
    sock_rrm_int_t *sock  ///< the socket handle 
    ) 
{
    shutdown(sock->s, SHUT_RDWR);
    close(sock->s);
}

//mod_lor_10_01_25--
