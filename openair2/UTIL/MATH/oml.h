/*******************************************************************************

  Eurecom OpenAirInterface
  Copyright(c) 1999 - 2011 Eurecom

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

/*! \file oml.h
* \brief Data structure for OCG of OpenAir emulator
* \author N. Nikaein and A. Hafsaoui
* \date 2011
* \version 1.0
* \company Eurecom
* \email: openair_tech@eurecom.fr
* \note
* \warning
*/


#ifndef __OML_H__
#define __OML_H__


#include <math.h>
#include <stdlib.h>
#include  "../OTG/otg.h"

#define PI 3.14159265


/*! \fn void set_taus_seed(unsigned int seed_type);
* \brief initialize seeds used for the generation of taus random values
* \param[in] initial value
* \param[out] 
* \note 
* @ingroup  _oml
*/
void set_taus_seed(unsigned int seed_type);

/*! \fn inline unsigned int taus(unsigned int comp);
* \brief compute random number 
* \param[in] integer
* \param[out] 
* \note 
* @ingroup  _oml
*/
inline unsigned int taus(unsigned int comp);


/*! \fn void init_seeds(int seed);
* \brief init values for wichman_hill algo
* \param[in] seed
* \param[out] 
* \note 
* @ingroup  _oml
*/
void init_seeds(int seed);

/*! \fn double wichman_hill() ;
* \brief generates uniform random number with wichman_hill algo
* \param[in]
* \param[out] random number: wichman_hill
* \note 
* @ingroup  _oml
*/
double wichman_hill(void) ;

/*! \fn double uniform_rng();
* \brief generates uniform random number with algo: wichman_hill / random() / Taus
* \param[in]
* \param[out] random number 
* \note 
* @ingroup  _oml
*/
double uniform_rng(void);

/*! \fn double uniform_dist(double min, double max);
* \brief 
* \param[in] double min 
* \param[in] double max
* \param[out] uniform number
* \note 
* @ingroup  _oml
*/
double uniform_dist(int min, int max);

/*! \fn double gaussian_dist(double mean, double std_dev);
* \brief 
* \param[in] double mean
* \param[in] double standard deviation 
* \param[out] exponential gaussian number
* \note 
* @ingroup  _oml
*/
double gaussian_dist(double mean, double std_dev);

/*! \fn double exponential_dist(double lambda);
* \brief 
* \param[in] double lambda 
* \param[out] exponential random number
* \note 
* @ingroup  _oml
*/

double exponential_dist(double lambda);

/*! \fn double poisson_dist(double lambda);
* \brief generates random numbers for the poisson distribution
* \param[in] lambda used for poisson distrib configuration
* \param[out] poisson random number
* \note 
* @ingroup  _oml
*/
double poisson_dist(double lambda);

/*! \fn double weibull_dist(double scale, double shape);
* \brief generates random numbers for the Weibull distribution with scale parameter, and shape parameter. 
* \param[in] scale parameter, and shape parameter.
* \param[out] weibull random number
* \note Formula (http://www.xycoon.com/wei_random.htm)
* @ingroup  _oml
*/
double weibull_dist(double scale, double shape);

/*! \fn double pareto_dist(double scale, double shape);
* \brief enerates random numbers for the pareto distribution with scale parameter, and shape parameter. 
* \param[in] double scale 
* \param[in] double shape
* \param[out] pareto random number
* \note Formula (http://www.xycoon.com/par_random.htm)
* @ingroup  _oml
*/
double pareto_dist(double scale, double shape);

/*! \fn double gamma_dist(double scale, double shape);
* \brief generates random numbers for the gamma distribution with scale parameter, and shape parameter. 
* \param[in] double scale 
* \param[in] double shape
* \param[out] gamma random number
* \note  Formula (http://www.xycoon.com/gamma_random.htm)
* @ingroup  _oml
*/
double gamma_dist(double scale, double shape);

/*! \fn double cauchy_dist(double scale, double shape);
* \brief generates random numbers for the cauchy distribution with scale parameter, and shape parameter. 
* \param[in] double scale 
* \param[in] double shape
* \param[out] cauchy random number
* \note Formula(http://www.xycoon.com/nor_relationships3.htm)
* @ingroup  _oml
*/
double cauchy_dist(double scale, double shape);


/*! \fn double double lognormal_dist(double mean, double std_dev)
* \brief generates random numbers for the log normal distribution with mean parameter and standard deviation parameter. 
* \param[in] double mean 
* \param[in] double std_dev
* \param[out] lognormal random number
* \note Formula(http://www.xycoon.com/nor_relationships3.htm)
* @ingroup  _oml
*/
double lognormal_dist(double mean, double std_dev);


#endif
 
