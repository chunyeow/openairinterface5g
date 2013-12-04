#ifndef __PHY_TOOLS_DEFS__H__
#define __PHY_TOOLS_DEFS__H__

/** @addtogroup _PHY_DSP_TOOLS_


* @{

*/
 
#ifndef NO_OPENAIR1
#include "PHY/defs.h"
#else
#include <stdint.h>
#define u32 uint32_t
#define s32 int32_t
#define u16 uint16_t
#define s16 int16_t
#define u8 uint8_t
#define s8 int8_t
#define msg printf
#endif

#ifdef __SSE2__
#include <emmintrin.h>
#include <xmmintrin.h>
#endif
#ifdef __SSE3__
#include <pmmintrin.h>
#include <tmmintrin.h>
#endif


//defined in rtai_math.h
#ifndef _RTAI_MATH_H
struct complex
{
  double x;
  double y;
};
#endif

struct complexf
{
  float r;
  float i;
};

struct complex16
{
  s16 r;
  s16 i;	
};

struct complex32
{
  s32 r;
  s32 i;
};

#ifndef EXPRESSMIMO_TARGET
/*!\fn void multadd_real_vector_complex_scalar(s16 *x,s16 *alpha,s16 *y,u32 N)
This function performs componentwise multiplication and accumulation of a complex scalar and a real vector.
@param x Vector input (Q1.15)  
@param alpha Scalar input (Q1.15) in the format  |Re0 Im0|
@param y Output (Q1.15) in the format  |Re0  Im0 Re1 Im1|,......,|Re(N-1)  Im(N-1) Re(N-1) Im(N-1)|
@param N Length of x WARNING: N>=8

The function implemented is : \f$\mathbf{y} = y + \alpha\mathbf{x}\f$
*/
void multadd_real_vector_complex_scalar(s16 *x,
					s16 *alpha,
					s16 *y,
					u32 N
					);

/*!\fn void multadd_complex_vector_real_scalar(s16 *x,s16 alpha,s16 *y,u8 zero_flag,u32 N)
This function performs componentwise multiplication and accumulation of a real scalar and a complex vector.
@param x Vector input (Q1.15) in the format |Re0 Im0|Re1 Im 1| ... 
@param alpha Scalar input (Q1.15) in the format  |Re0|
@param y Output (Q1.15) in the format  |Re0  Im0 Re1 Im1|,......,|Re(N-1)  Im(N-1) Re(N-1) Im(N-1)|
@param zero_flag Set output (y) to zero prior to accumulation
@param N Length of x WARNING: N>=8

The function implemented is : \f$\mathbf{y} = y + \alpha\mathbf{x}\f$
*/
void multadd_complex_vector_real_scalar(s16 *x,
					s16 alpha,
					s16 *y,
					u8 zero_flag,
					u32 N);


/*!\fn s32 mult_cpx_vector(s16 *x1,s16 *x2,s16 *y,u32 N,s32 output_shift)
This function performs optimized componentwise multiplication of two Q1.15 vectors in repeated format.

@param x1 Input 1 in the format  |Re0  Im0 Re0 Im0|,......,|Re(N-1)  Im(N-1) Re(N-1) Im(N-1)|
@param x2 Input 2 in the format  |Re0 -Im0 Im0 Re0|,......,|Re(N-1) -Im(N-1) Im(N-1) Re(N-1)|
@param y  Output in the format  |Re0  Im0 Re0 Im0|,......,|Re(N-1)  Im(N-1) Re(N-1) Im(N-1)|
@param N  Length of Vector WARNING: N must be a multiple of 8 (4x loop unrolling and two complex multiplies per cycle) 
@param output_shift Number of bits to shift output down to Q1.15 (should be 15 for Q1.15 inputs) WARNING: log2_amp>0 can cause overflow!! 

The function implemented is : \f$\mathbf{y} = \mathbf{x_1}\odot\mathbf{x_2}\f$
*/
s32 mult_cpx_vector(s16 *x1, 
		    s16 *x2, 
		    s16 *y, 
		    u32 N, 
		    s32 output_shift);

/*!\fn s32 mult_cpx_vector_norep(s16 *x1,s16 *x2,s16 *y,u32 N,s32 output_shift)
This function performs optimized componentwise multiplication of two Q1.15 vectors with normal formatted output.

@param x1 Input 1 in the format  |Re0  Im0 Re0 Im0|,......,|Re(N-1)  Im(N-1) Re(N-1) Im(N-1)|
@param x2 Input 2 in the format  |Re0 -Im0 Im0 Re0|,......,|Re(N-1) -Im(N-1) Im(N-1) Re(N-1)|
@param y  Output in the format  |Re0  Im0 Re1 Im1|,......,|Re(N-2)  Im(N-2) Re(N-1) Im(N-1)|
@param N  Length of Vector WARNING: N must be a multiple of 8 (4x loop unrolling and two complex multiplies per cycle) 
@param output_shift Number of bits to shift output down to Q1.15 (should be 15 for Q1.15 inputs) WARNING: log2_amp>0 can cause overflow!! 

The function implemented is : \f$\mathbf{y} = \mathbf{x_1}\odot\mathbf{x_2}\f$
*/
s32 mult_cpx_vector_norep(s16 *x1, 
			  s16 *x2, 
			  s16 *y, 
			  u32 N, 
			  s32 output_shift);


/*!\fn s32 mult_cpx_vector_norep2(s16 *x1,s16 *x2,s16 *y,u32 N,s32 output_shift)
This function performs optimized componentwise multiplication of two Q1.15 vectors with normal formatted output.

@param x1 Input 1 in the format  |Re0  Im0 Re0 Im0|,......,|Re(N-1)  Im(N-1) Re(N-1) Im(N-1)|
@param x2 Input 2 in the format  |Re0 -Im0 Im0 Re0|,......,|Re(N-1) -Im(N-1) Im(N-1) Re(N-1)|
@param y  Output in the format  |Re0  Im0 Re1 Im1|,......,|Re(N-2)  Im(N-2) Re(N-1) Im(N-1)|
@param N  Length of Vector WARNING: N must be a multiple of 8 (no unrolling and two complex multiplies per cycle) 
@param output_shift Number of bits to shift output down to Q1.15 (should be 15 for Q1.15 inputs) WARNING: log2_amp>0 can cause overflow!! 

The function implemented is : \f$\mathbf{y} = \mathbf{x_1}\odot\mathbf{x_2}\f$
*/
s32 mult_cpx_vector_norep2(s16 *x1, 
			   s16 *x2, 
			   s16 *y, 
			   u32 N, 
			   s32 output_shift);
 
s32 mult_cpx_vector_norep_conj(s16 *x1, 
			       s16 *x2, 
			       s16 *y, 
			       u32 N, 
			       s32 output_shift);

s32 mult_cpx_vector_norep_conj2(s16 *x1, 
				s16 *x2, 
				s16 *y, 
				u32 N, 
				s32 output_shift);

/*!\fn s32 mult_cpx_vector2(s16 *x1,s16 *x2,s16 *y,u32 N,s32 output_shift)
This function performs optimized componentwise multiplication of two Q1.15 vectors.

@param x1 Input 1 in the format  |Re0  Im0 Re0 Im0|,......,|Re(N-1)  Im(N-1) Re(N-1) Im(N-1)|
@param x2 Input 2 in the format  |Re0 -Im0 Im0 Re0|,......,|Re(N-1) -Im(N-1) Im(N-1) Re(N-1)|
@param y  Output in the format  |Re0  Im0 Re0 Im0|,......,|Re(N-1)  Im(N-1) Re(N-1) Im(N-1)|
@param N  Length of Vector WARNING: N must be a multiple of 2 (2 complex multiplies per cycle)
@param output_shift Number of bits to shift output down to Q1.15 (should be 15 for Q1.15 inputs) WARNING: log2_amp>0 can cause overflow!! 

The function implemented is : \f$\mathbf{y} = \mathbf{x_1}\odot\mathbf{x_2}\f$
*/
s32 mult_cpx_vector2(s16 *x1, 
		     s16 *x2, 
		     s16 *y, 
		     u32 N, 
		     s32 output_shift);

/*!\fn s32 mult_cpx_vector_add(s16 *x1,s16 *x2,s16 *y,u32 N,s32 output_shift)
This function performs optimized componentwise multiplication of two Q1.15 vectors. The output IS ADDED TO y. WARNING: make sure that output_shift is set to the right value, so that the result of the multiplication has the comma at the same place as y.

@param x1 Input 1 in the format  |Re0  Im0 Re0 Im0|,......,|Re(N-1)  Im(N-1) Re(N-1) Im(N-1)|
@param x2 Input 2 in the format  |Re0 -Im0 Im0 Re0|,......,|Re(N-1) -Im(N-1) Im(N-1) Re(N-1)|
@param y  Output in the format  |Re0  Im0 Re0 Im0|,......,|Re(N-1)  Im(N-1) Re(N-1) Im(N-1)|
@param N  Length of Vector WARNING: N>=4
@param output_shift Number of bits to shift output down to Q1.15 (should be 15 for Q1.15 inputs) WARNING: log2_amp>0 can cause overflow!! 

The function implemented is : \f$\mathbf{y} += \mathbf{x_1}\odot\mathbf{x_2}\f$
*/

s32 mult_cpx_vector_add(s16 *x1, 
			s16 *x2, 
			s16 *y, 
			u32 N, 
			s32 output_shift);


s32 mult_cpx_vector_h_add32(s16 *x1, 
			    s16 *x2, 
			    s16 *y, 
			    u32 N, 
			    s16 sign);

s32 mult_cpx_vector_add32(s16 *x1, 
			  s16 *x2, 
			  s16 *y, 
			  u32 N);

s32 mult_vector32(s16 *x1, 
		  s16 *x2, 
		  s16 *y, 
		  u32 N);

s32 mult_vector32_scalar(s16 *x1, 
			 s32 x2, 
			 s16 *y, 
			 u32 N);

s32 mult_cpx_vector32_conj(s16 *x, 
			   s16 *y, 
			   u32 N);

s32 mult_cpx_vector32_real(s16 *x1, 
			   s16 *x2, 
			   s16 *y, 
			   u32 N);

s32 shift_and_pack(s16 *y, 
		   u32 N, 
		   s32 output_shift);

/*!\fn s32 mult_cpx_vector_h(s16 *x1,s16 *x2,s16 *y,u32 N,s32 output_shift,s16 sign)
This function performs optimized componentwise multiplication of the vector x1 with the conjugate of the vector x2. The output IS ADDED TO y. WARNING: make sure that output_shift is set to the right value, so that the result of the multiplication has the comma at the same place as y.

@param x1 Input 1 in the format  |Re0  Im0 Re0 Im0|,......,|Re(N-1)  Im(N-1) Re(N-1) Im(N-1)|
@param x2 Input 2 in the format  |Re0  Im0 Re0 Im0|,......,|Re(N-1) Im(N-1) Re(N-1) Im(N-1)|
@param y  Output in the format  |Re0  Im0 Re0 Im0|,......,|Re(N-1)  Im(N-1) Re(N-1) Im(N-1)|
@param N  Length of Vector (complex samples) WARNING: N>=4
@param output_shift Number of bits to shift output down to Q1.15 (should be 15 for Q1.15 inputs) WARNING: log2_amp>0 can cause overflow!! 
@param sign +1..add, -1..substract

The function implemented is : \f$\mathbf{y} = \mathbf{y} + \mathbf{x_1}\odot\mathbf{x_2}^*\f$
*/
s32 mult_cpx_vector_h(s16 *x1, 
		      s16 *x2, 
		      s16 *y, 
		      u32 N, 
		      s32 output_shift,
		      s16 sign);

/*!\fn s32 mult_cpx_matrix_h(s16 *x1[2][2],s16 *x2[2][2],s16 *y[2][2],u32 N,u16 output_shift,s16 hermitian)
This function performs optimized componentwise matrix multiplication of the 2x2 matrices x1 with the 2x2 matrices x2. The output IS ADDED TO y (i.e. make sure y is initilized correctly). WARNING: make sure that output_shift is set to the right value, so that the result of the multiplication has the comma at the same place as y.

@param x1 Input 1 in the format  |Re0  Im0 Re0 Im0|,......,|Re(N-1)  Im(N-1) Re(N-1) Im(N-1)|
@param x2 Input 2 in the format  |Re0  Im0 Re0 Im0|,......,|Re(N-1) Im(N-1) Re(N-1) Im(N-1)|
@param y  Output in the format  |Re0  Im0 Re0 Im0|,......,|Re(N-1)  Im(N-1) Re(N-1) Im(N-1)|
@param N  Length of Vector (complex samples) WARNING: N>=4
@param output_shift Number of bits to shift output down to Q1.15 (should be 15 for Q1.15 inputs) WARNING: log2_amp>0 can cause overflow!! 
@param hermitian if !=0 the hermitian transpose is returned (i.e. A^H*B instead of A*B^H)
*/
s32 mult_cpx_matrix_h(s16 *x1[2][2], 
		    s16 *x2[2][2], 
		    s16 *y[2][2], 
		    u32 N, 
		    u16 output_shift,
		    s16 hermitian); 


/*!\fn s32 mult_cpx_matrix_vector(s32 *x1[2][2],s32 *x2[2],s32 *y[2],u32 N,u16 output_shift)
This function performs optimized componentwise matrix-vector multiplication of the 2x2 matrices x1 with the 2x1 vectors x2. The output IS ADDED TO y (i.e. make sure y is initilized correctly). WARNING: make sure that output_shift is set to the right value, so that the result of the multiplication has the comma at the same place as y.

@param x1 Input 1 in the format  |Re0  Im0 Re0 Im0|,......,|Re(N-1)  Im(N-1) Re(N-1) Im(N-1)|
@param x2 Input 2 in the format  |Re0  -Im0 Im0 Re0|,......,|Re(N-1) -Im(N-1) Im(N-1) Re(N-1)|
@param y  Output in the format  |Re0  Im0 Re0 Im0|,......,|Re(N-1)  Im(N-1) Re(N-1) Im(N-1)| WARNING: y must be different for x2
@param N  Length of Vector (complex samples) WARNING: N>=4
@param output_shift Number of bits to shift output down to Q1.15 (should be 15 for Q1.15 inputs) WARNING: log2_amp>0 can cause overflow!! 
*/
s32 mult_cpx_matrix_vector(s32 *x1[2][2], 
		    s32 *x2[2], 
		    s32 *y[2], 
		    u32 N, 
		    u16 output_shift);

/*!\fn void init_fft(u16 size,u8 logsize,u16 *rev)
\brief Initialize the FFT engine for a given size
@param size Size of the FFT
@param logsize log2(size)
@param rev Pointer to bit-reversal permutation array
*/

void init_fft(u16 size, 
	      u8 logsize, 
	      u16 *rev);

/*!\fn void fft(s16 *x,s16 *y,s16 *twiddle,u16 *rev,u8 log2size,u8 scale,u8 input_fmt)
This function performs optimized fixed-point radix-2 FFT/IFFT.
@param x Input
@param y Output in format: [Re0,Im0,Re0,Im0, Re1,Im1,Re1,Im1, ....., Re(N-1),Im(N-1),Re(N-1),Im(N-1)]
@param twiddle Twiddle factors
@param rev bit-reversed permutation
@param log2size Base-2 logarithm of FFT size
@param scale Total number of shifts (should be log2size/2 for normalized FFT)
@param input_fmt (0 - input is in complex Q1.15 format, 1 - input is in complex redundant Q1.15 format)
*/
void fft(s16 *x,   
	 s16 *y,
	 s16 *twiddle,
	 u16 *rev,
	 u8 log2size,
	 u8 scale,
	 u8 input_fmt
	 );

void ifft1536(s16 *sigF,s16 *sig);

void ifft6144(s16 *sigF,s16 *sig);

void ifft12288(s16 *sigF,s16 *sig);

void ifft18432(s16 *sigF,s16 *sig);

void ifft3072(s16 *sigF,s16 *sig);

void ifft24576(s16 *sigF,s16 *sig);

void fft1536(s16 *sigF,s16 *sig);

void fft6144(s16 *sigF,s16 *sig);

void fft12288(s16 *sigF,s16 *sig); 

void fft18432(s16 *sigF,s16 *sig);

void fft3072(s16 *sigF,s16 *sig);

void fft24576(s16 *sigF,s16 *sig); 


/*!\fn int rotate_cpx_vector(s16 *x,s16 *alpha,s16 *y,u32 N,u16 output_shift, u8 format)
This function performs componentwise multiplication of a vector with a complex scalar.
@param x Vector input (Q1.15)  in the format  |Re0  Im0 Re0 Im0|,......,|Re(N-1)  Im(N-1) Re(N-1) Im(N-1)|
@param alpha Scalar input (Q1.15) in the format  |Re0 Im0|
@param y Output (Q1.15) in the format  |Re0  Im0 Re0 Im0|,......,|Re(N-1)  Im(N-1) Re(N-1) Im(N-1)|
@param N Length of x WARNING: N>=4
@param output_shift Number of bits to shift output down to Q1.15 (should be 15 for Q1.15 inputs) WARNING: log2_amp>0 can cause overflow!!
@param format Format 0 indicates that alpha is in shuffled format during multiply (Re -Im Im Re), whereas 1 indicates that input is in this format (i.e. a matched filter)

The function implemented is : \f$\mathbf{y} = \alpha\mathbf{x}\f$
*/
s32 rotate_cpx_vector(s16 *x, 
		      s16 *alpha, 
		      s16 *y, 
		      u32 N, 
		      u16 output_shift,
		      u8 format); 

/*!\fn s32 rotate_cpx_vector2(s16 *x,s16 *alpha,s16 *y,u32 N,u16 output_shift,u8 format)
This function performs componentwise multiplication of a vector with a complex scalar.
@param x Vector input (Q1.15)  in the format  |Re0  Im0 Re0 Im0|,......,|Re(N-1)  Im(N-1) Re(N-1) Im(N-1)|
@param alpha Scalar input (Q1.15) in the format  |Re0 Im0|
@param y Output (Q1.15) in the format  |Re0  Im0 Re0 Im0|,......,|Re(N-1)  Im(N-1) Re(N-1) Im(N-1)|
@param N Length of x WARNING: N must be multiple of 2 (the routine performs two complex multiplies per cycle)
@param output_shift Number of bits to shift output down to Q1.15 (should be 15 for Q1.15 inputs) WARNING: log2_amp>0 can cause overflow!!
@param format Format 0 indicates that alpha is in shuffled format during multiply (Re -Im Im Re), whereas 1 indicates that input is in this format (i.e. a matched filter)
The function implemented is : \f$\mathbf{y} = \alpha\mathbf{x}\f$
*/
s32 rotate_cpx_vector2(s16 *x, 
		       s16 *alpha, 
		       s16 *y, 
		       u32 N, 
		       u16 output_shift,
		       u8 format);

/*!\fn s32 rotate_cpx_vector_norep(s16 *x,s16 *alpha,s16 *y,u32 N,u16 output_shift)
This function performs componentwise multiplication of a vector with a complex scalar.
@param x Vector input (Q1.15)  in the format  |Re0  Im0|,......,|Re(N-1) Im(N-1)|
@param alpha Scalar input (Q1.15) in the format  |Re0 Im0|
@param y Output (Q1.15) in the format  |Re0  Im0|,......,|Re(N-1) Im(N-1)|
@param N Length of x WARNING: N>=4
@param output_shift Number of bits to shift output down to Q1.15 (should be 15 for Q1.15 inputs) WARNING: log2_amp>0 can cause overflow!!

The function implemented is : \f$\mathbf{y} = \alpha\mathbf{x}\f$
*/
s32 rotate_cpx_vector_norep(s16 *x, 
			       s16 *alpha, 
			       s16 *y, 
			       u32 N, 
			       u16 output_shift);



/*!\fn s32 add_cpx_vector(s16 *x,s16 *alpha,s16 *y,u32 N)
This function performs componentwise addition of a vector with a complex scalar.
@param x Vector input (Q1.15)  in the format  |Re0  Im0 Re0 Im0|,......,|Re(N-1)  Im(N-1) Re(N-1) Im(N-1)|
@param alpha Scalar input (Q1.15) in the format  |Re0 Im0|
@param y Output (Q1.15) in the format  |Re0  Im0 Re0 Im0|,......,|Re(N-1)  Im(N-1) Re(N-1) Im(N-1)|
@param N Length of x WARNING: N>=4

The function implemented is : \f$\mathbf{y} = \alpha + \mathbf{x}\f$
*/
s32 add_cpx_vector(s16 *x, 
		   s16 *alpha, 
		   s16 *y, 
		   u32 N);

s32 add_cpx_vector32(s16 *x, 
		      s16 *y, 
		      s16 *z, 
		      u32 N);

s32 add_real_vector64(s16 *x, 
		      s16 *y, 
		      s16 *z, 
		      u32 N);

s32 sub_real_vector64(s16 *x, 
		      s16* y, 
		      s16 *z, 
		      u32 N);

s32 add_real_vector64_scalar(s16 *x, 
			     long long int a, 
			     s16 *y, 
			     u32 N);

/*!\fn s32 add_vector16(s16 *x,s16 *y,s16 *z,u32 N)
This function performs componentwise addition of two vectors with Q1.15 components.
@param x Vector input (Q1.15)  
@param y Scalar input (Q1.15) 
@param z Scalar output (Q1.15) 
@param N Length of x WARNING: N must be a multiple of 32

The function implemented is : \f$\mathbf{z} = \mathbf{x} + \mathbf{y}\f$
*/
s32 add_vector16(s16 *x, 
		 s16 *y, 
		 s16 *z, 
		 u32 N);

s32 add_vector16_64(s16 *x, 
		    s16 *y, 
		    s16 *z, 
		    u32 N);

s32 complex_conjugate(s16 *x1, 
		      s16 *y, 
		      u32 N);

void bit8_txmux(s32 length,s32 offset);

void bit8_rxdemux(s32 length,s32 offset);

#ifdef USER_MODE
/*!\fn s32 write_output(const char *fname, const char *vname, void *data, int length, int dec, char format);
\brief Write output file from signal data
@param fname output file name
@param vname  output vector name (for MATLAB/OCTAVE)
@param data   point to data 
@param length length of data vector to output
@param dec    decimation level
@param format data format (0 = real 16-bit, 1 = complex 16-bit,2 real 32-bit, 3 complex 32-bit,4 = real 8-bit, 5 = complex 8-bit)
*/
s32 write_output(const char *fname, const char *vname, void *data, int length, int dec, char format);
#endif

void Zero_Buffer(void *,u32);
void Zero_Buffer_nommx(void *buf,u32 length);

void mmxcopy(void *dest,void *src,int size);

/*!\fn s32 signal_energy(int *,u32);
\brief Computes the signal energy per subcarrier
*/
s32 signal_energy(s32 *,u32);

/*!\fn s32 signal_energy_nodc(s32 *,u32);
\brief Computes the signal energy per subcarrier, without DC removal
*/
s32 signal_energy_nodc(s32 *,u32);

/*!\fn double signal_energy_fp(double **, double **,u32, u32,u32);
\brief Computes the signal energy per subcarrier
*/
double signal_energy_fp(double **s_re, double **s_im, u32 nb_antennas, u32 length,u32 offset);

/*!\fn double signal_energy_fp2(struct complex *, u32);
\brief Computes the signal energy per subcarrier
*/
double signal_energy_fp2(struct complex *s, u32 length);


s32 iSqrt(s32 value);
u8 log2_approx(u32);
u8 log2_approx64(unsigned long long int x);
s16 invSqrt(s16 x);
u32 angle(struct complex16 perrror);

/*!\fn s32 phy_phase_compensation_top (u32 pilot_type, u32 initial_pilot,
				u32 last_pilot, s32 ignore_prefix);
Compensate the phase rotation of the RF. WARNING: This function is currently unused. It has not been tested!
@param pilot_type indicates whether it is a CHBCH (=0) or a SCH (=1) pilot
@param initial_pilot index of the first pilot (which serves as reference)
@param last_pilot index of the last pilot in the range of pilots to correct the phase
@param ignore_prefix set to 1 if cyclic prefix has not been removed (by the hardware)

*/

#else // EXPRESSMIMO_TARGET

#define fft(x,y,twiddle,rev,log2size,scale,input_fmt) \
(((twiddle)==0) ? \
 (fft(1<<(log2size),((unsigned long*)(x)),((unsigned long*)(y)))) : \
 (ifft(1<<(log2size),((unsigned long*)(x)),((unsigned long*)(y)))))

#define mult_cpx_vector(x1,x2,y,N,os)  component_wise_product(N,(unsigned long *)(x1),(unsigned long *)(x2),(unsigned long *)(y))

#define mult_cpx_vector2(x1,x2,y,N,os)  component_wise_product(N,(unsigned long *)(x1),(unsigned long *)(x2),(unsigned long *)(y))

#define add_vector16(x,y,z,N) component_wise_addition(N,(unsigned long*)(x),(unsigned long*)(y),(unsigned long*)(z)) 

#endif // EXPRESSMIMO_TARGET

s8 dB_fixed(u32 x);

s8 dB_fixed2(u32 x,u32 y);

s32 phy_phase_compensation_top (u32 pilot_type, u32 initial_pilot,
				u32 last_pilot, s32 ignore_prefix);

/*!\fn void phy_phase_compensation (s16 *ref_sch, s16 *tgt_sch, s16 *out_sym, s32 ignore_prefix, s32 aa, struct complex16 *perror_out);
This function is used by the EMOS to compensate the phase rotation of the RF. It has been designed for symbols of type CHSCH or SCH, but cannot be used for the data channels.
@param ref_sch reference symbol
@param tgt_sch target symbol
@param out_sym output of the operation
@param ignore_prefix  set to 1 if cyclic prefix has not been removed (by the hardware)
@param aa antenna index
@param perror_out phase error (output parameter)
*/
void phy_phase_compensation (s16 *ref_sch, s16 *tgt_sch, s16 *out_sym, s32 ignore_prefix, s32 aa, struct complex16 *perror_out );

s32 dot_product(s16 *x,
		s16 *y,
		u32 N, //must be a multiple of 8
		u8 output_shift);

void dft12(int16_t *x,int16_t *y);
void dft24(int16_t *x,int16_t *y,u8 scale_flag);
void dft36(int16_t *x,int16_t *y,u8 scale_flag);
void dft48(int16_t *x,int16_t *y,u8 scale_flag);
void dft60(int16_t *x,int16_t *y,u8 scale_flag);
void dft72(int16_t *x,int16_t *y,u8 scale_flag);
void dft96(int16_t *x,int16_t *y,u8 scale_flag);
void dft108(int16_t *x,int16_t *y,u8 scale_flag);
void dft120(int16_t *x,int16_t *y,u8 scale_flag);
void dft144(int16_t *x,int16_t *y,u8 scale_flag);
void dft180(int16_t *x,int16_t *y,u8 scale_flag);
void dft192(int16_t *x,int16_t *y,u8 scale_flag);
void dft216(int16_t *x,int16_t *y,u8 scale_flag);
void dft240(int16_t *x,int16_t *y,u8 scale_flag);
void dft288(int16_t *x,int16_t *y,u8 scale_flag);
void dft300(int16_t *x,int16_t *y,u8 scale_flag);
void dft324(int16_t *x,int16_t *y,u8 scale_flag);
void dft360(int16_t *x,int16_t *y,u8 scale_flag);
void dft384(int16_t *x,int16_t *y,u8 scale_flag);
void dft432(int16_t *x,int16_t *y,u8 scale_flag);
void dft480(int16_t *x,int16_t *y,u8 scale_flag);
void dft540(int16_t *x,int16_t *y,u8 scale_flag);
void dft576(int16_t *x,int16_t *y,u8 scale_flag);
void dft600(int16_t *x,int16_t *y,u8 scale_flag);
void dft648(int16_t *x,int16_t *y,u8 scale_flag);
void dft720(int16_t *x,int16_t *y,u8 scale_flag);
void dft864(int16_t *x,int16_t *y,u8 scale_flag);
void dft900(int16_t *x,int16_t *y,u8 scale_flag);
void dft960(int16_t *x,int16_t *y,u8 scale_flag);
void dft972(int16_t *x,int16_t *y,u8 scale_flag);
void dft1080(int16_t *x,int16_t *y,u8 scale_flag);
void dft1152(int16_t *x,int16_t *y,u8 scale_flag);
void dft1200(int16_t *x,int16_t *y,u8 scale_flag);

void dft64(int16_t *x,int16_t *y,int scale);
void dft128(int16_t *x,int16_t *y,int scale);
void dft256(int16_t *x,int16_t *y,int scale);
void dft512(int16_t *x,int16_t *y,int scale);
void dft1024(int16_t *x,int16_t *y,int scale);
void dft2048(int16_t *x,int16_t *y,int scale);
void idft64(int16_t *x,int16_t *y,int scale);
void idft128(int16_t *x,int16_t *y,int scale);
void idft256(int16_t *x,int16_t *y,int scale);
void idft512(int16_t *x,int16_t *y,int scale);
void idft1024(int16_t *x,int16_t *y,int scale);
void idft2048(int16_t *x,int16_t *y,int scale);
/** @} */ 


#endif //__PHY_TOOLS_DEFS__H__
