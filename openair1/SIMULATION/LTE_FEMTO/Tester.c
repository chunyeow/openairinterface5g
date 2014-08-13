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
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
void main( int argc, char **argv){

	
	clock_t t_ini, t_fin;
	time_t tiempo = time(0);
	struct tm *tlocal ;
	char output[128];
 	int i,j,h,x,z;
 	
 	int interferencias[11];
 	
 	interferencias[0]=-15;
 	interferencias[1]=-5;
 	interferencias[2]=-3;
 	interferencias[3]=-2;
 	interferencias[4]=-1;
 	interferencias[5]=0;
 	interferencias[6]=1;
 	interferencias[7]=2;
 	interferencias[8]=3;
 	interferencias[9]=5;
 	interferencias[10]=15;
	
	FILE *output_fd ;
	output_fd= fopen("TesterControl.txt","w");
	double secs;
	
	
	char **pruebas;
	int n=(4*8*4*11)+1;
	
	pruebas= (char **) malloc(n*sizeof(char *));
	
	
	
	for(i=0;i<n; i++)
	{
		pruebas[i]=(char*)malloc(200*sizeof(char));
	}
	
	
	pruebas[0]= "./femtosim -n1000  -s0 -S25 -b0";
	
	i=1;
	x=1;
	for(j=0;j<8;j++)
	{
		for(h=0;h<4;h++) 
		{

		    for( z=0;z<11;z++)
		    {
				 //printf(" %d %d %d %d %d %d\n",interferencias[z],x,j,h,i,n);
		    
				sprintf(pruebas[i],"./femtosim -n1000  -s0 -S25 -a -I1 -w%d -b100%d -p%d,%d",interferencias[z],x,j,h);
				i++;				
				sprintf(pruebas[i],"./femtosim -n1000  -s0 -S25 -a -I1 -w%d -b200%d -p%d,%d -A1 -D",interferencias[z],x,j,h);
				i++;				
				sprintf(pruebas[i],"./femtosim -n1000  -s0 -S25 -I1 -w%d -b300%d -p%d,%d",interferencias[z],x,j,h);
				i++;				
				sprintf(pruebas[i],"./femtosim -n1000  -s0 -S25 -I1 -w%d -b400%d -p%d,%d -A1 -D",interferencias[z],x,j,h);
				i++;							
				x++;
				
				
				
		    }
		}
	}
	
						

	for(i=0;i<n;i++)
	{		
	 	  printf("\n%s",pruebas[i]);			
	 	  
		t_ini = clock();					
			fprintf(output_fd,"\n%s",pruebas[i]);			
	    	 tiempo = time(0);
			tlocal = localtime(&tiempo);
			strftime(output,128,"%d/%m/%y %H:%M:%S",tlocal);
			  
			fprintf(output_fd,"\n\tInicio: \t%s",output); 
			
			system(pruebas[i]);
				
       
			tiempo = time(0);
			tlocal = localtime(&tiempo);  
			strftime(output,128,"%d/%m/%y %H:%M:%S",tlocal);
			  
			fprintf(output_fd,"\n\tFin: \t%s",output); 
	
	}
	fclose(output_fd);
	
	


}
