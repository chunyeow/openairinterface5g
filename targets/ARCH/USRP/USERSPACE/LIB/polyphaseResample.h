#ifndef _POLYPHASERESAMPLE_H_
#include <cmath>
//#include "coeff_3072_to_25.h"

void buildPolyphaseCache(const int P, const int Q, int *&branch_table, int *&offset_table, const int filter_len = 50000);


/**
 * @brief polyphase resampler
 * @param in_data input data, layout as [real,imag,real,imag...]
 * @param in_sample_count count of input samples, i.e. a complex 
 *        cosisting of two short int counts as one
 * @param out_data output data, layout same as in_data
 * @param out_sample_count sample count of result, as return
 * @param filter_coeff filter with only real coeffcients
 * @param filter_len count of coeffcients
 * @param P order of upper sampling
 * @param Q order of down sampling
 **/

void polyphaseResample(short *in_data, unsigned int in_sample_count,
    short *out_data, unsigned int out_sample_count,
    short *filter_coeff, unsigned int filter_len,
    const int P, const int Q, const unsigned int fix_shift,
    int *branch_table, int *offset_table);
#endif