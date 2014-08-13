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

// ******* FFT function **********************************************************

#define PI		3.14159264359
#define SWAP(a,b) tempr=(a); (a)=(b); (b)=tempr

void FFT(float *data,      // the vector with the outputs
			    // real part in even indexes
			    // imaginary part in the odd indexes
			    // for power conservation, the output should be normalized by 1/sqrt(nn)
	 unsigned long nn,  // number of complex samples
	 int isign)         // isign = 1  -> FFT,  -1 -> iFFT

   {
   unsigned long n,mmax,m,j,istep,i;
   float wtemp,wr,wpr,wpi,wi,theta;
   float tempr,tempi;

   n = nn << 1;
   j = 1;
   for( i = 1 ; i < n ; i += 2){             // The bit-reversal
      if(j > i){
	 SWAP(data[j-1],data[i-1]);        // Exchanging the two complex number
	 SWAP(data[j],data[i]);
	 }
      m = n >> 1;
      while((m >= 2)&&(j > m)){
	 j -= m;
	 m >>= 1;
	 }
      j += m;
      }
   mmax = 2;
   while(n > mmax){
      istep = mmax << 1;
      theta = isign*(-2*PI/mmax);
      wtemp = sin(0.5 * theta);
      wpr = -2.0 * wtemp * wtemp;
      wpi = sin(theta);
      wr = 1.0;
      wi = 0.0;
      for (m = 1; m < mmax; m += 2){
	 for (i = m; i <= n; i += istep){
	      j = i + mmax;
	      tempr     = wr*data[j-1] - wi*data[j];
	      tempi     = wr*data[j] + wi*data[j-1];
	      data[j-1]   = data[i-1] - tempr;
	      data[j] = data[i] - tempi;
	      data[i-1]   += tempr;
	      data[i] += tempi;
	      }
	 wr = (wtemp=wr)*wpr - wi*wpi + wr;
	 wi = wi*wpr + wtemp*wpi + wi;
	 }
      mmax = istep;
      }
   }


