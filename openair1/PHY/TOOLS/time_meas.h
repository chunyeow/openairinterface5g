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
#ifdef OMP
#include <omp.h>
#endif
#include <unistd.h>
#include <math.h>
// global var to enable openair performance profiler 
extern int opp_enabled;

typedef struct {

  long long in;
  long long diff;
  long long p_time; /*!< \brief absolute process duration */
  long long diff_square; /*!< \brief process duration square */
  long long max;
  int trials;
} time_stats_t;

static inline void start_meas(time_stats_t *ts) __attribute__((always_inline));
static inline void stop_meas(time_stats_t *ts) __attribute__((always_inline));

void print_meas(time_stats_t *ts, const char* name, time_stats_t * total_exec_time, time_stats_t * sf_exec_time);
double get_time_meas_us(time_stats_t *ts);

#if defined(__i386__)
static inline unsigned long long rdtsc_oai(void) __attribute__((always_inline));
static inline unsigned long long rdtsc_oai(void) {
    unsigned long long int x;
    __asm__ volatile (".byte 0x0f, 0x31" : "=A" (x));
    return x;
}
#elif defined(__x86_64__)
static inline unsigned long long rdtsc_oai(void) __attribute__((always_inline));
static inline unsigned long long rdtsc_oai(void) { 
  unsigned long long a, d;
  __asm__ volatile ("rdtsc" : "=a" (a), "=d" (d));
  return (d<<32) | a;
}
#endif

static inline void start_meas(time_stats_t *ts) {

  if (opp_enabled) {

#ifdef OMP
    int tid;
    
    tid = omp_get_thread_num();
    if (tid==0)
#endif
      {
	ts->trials++;
	ts->in = rdtsc_oai();
      }
  }
}

static inline void stop_meas(time_stats_t *ts) {

  if (opp_enabled) {
    long long out = rdtsc_oai();
    
#ifdef OMP
    int tid;
    tid = omp_get_thread_num();
    if (tid==0)
#endif
      {
	ts->diff += (out-ts->in);
        /// process duration is the difference between two clock points
        ts->p_time = (out-ts->in);
        ts->diff_square += pow((out-ts->in),2);        
	if ((out-ts->in) > ts->max)
	  ts->max = out-ts->in;
	
      }
  }
}

static inline void reset_meas(time_stats_t *ts) {

  if (opp_enabled){
    ts->trials=0;
    ts->diff=0;
    ts->p_time=0;
    ts->diff_square=0;    
    ts->max=0;
  }
}
static inline void copy_meas(time_stats_t *dst_ts,time_stats_t *src_ts) {
  
  if (opp_enabled){
    dst_ts->trials=src_ts->trials;
    dst_ts->diff=src_ts->diff;
    dst_ts->max=src_ts->max;
  }
}

/*static inline double get_mean_meas_us(time_stats_t *ts, double cpu_freq_GHz) {

  return (double) ts->diff/ts->trials/cpu_freq_GHz/1000.0;

  }
*/
static inline double get_cpu_freq_GHz(void) {

  time_stats_t ts;
  reset_meas(&ts);
  start_meas(&ts);
  sleep(1);
  stop_meas(&ts);
  return (double)ts.diff/1000000000;
} 

