#include "polyphaseResample.h"
#include <iostream>
#include <stdio.h>
using namespace std;

// table_len should be decied by the length of the filter output
// but a default value of 50000 is adequate for our app
void buildPolyphaseCache(const int P, const int Q, int *&branch_table, int *&offset_table, const int table_len) {
	branch_table = new int[table_len];
	offset_table = new int[table_len];
	for (int i = 0; i < table_len; ++i) {
		branch_table[i] = (i * Q) % P;
		offset_table[i] = (i * Q - branch_table[i]) / P;
	}
}

void polyphaseResample(short *in_data, unsigned int in_sample_count,
		short *out_data, unsigned int out_sample_count,
		short *filter_coeff, unsigned int filter_len,
		const int P, const int Q, const unsigned int fix_shift,
		int *branch_table, int *offset_table) {
	buildPolyphaseCache( P, Q, branch_table, offset_table, 50000);
	// calculate the sample count of output
	out_sample_count = (int)(in_sample_count * (float)P / (float)Q);

	short *output_ptr = out_data;
	unsigned int output_ix = 0;
	short const *input_end = in_data + in_sample_count * 2;
	short const *output_end = out_data + out_sample_count * 2;
	short const *filter_end = filter_coeff + filter_len;
	while (output_ptr < output_end) 
	{
		int output_branch = branch_table[output_ix];
		int input_offset = offset_table[output_ix];

		short *input_ptr = in_data + 2 * input_offset;
		short *filter_ptr = filter_coeff + output_branch;

		while (input_ptr >= input_end) {
			input_ptr -= 2;
			filter_ptr += P;
		}

		int sum_real = 0, sum_imag = 0;
		while ((input_ptr >= in_data) && (filter_ptr < filter_end)) {
			sum_real += (int)(*input_ptr) * (int)(*filter_ptr);
			sum_imag += (int)(*(input_ptr + 1)) * (int)(*filter_ptr);
			input_ptr -= 2;
			filter_ptr += P;
		}

		*output_ptr = (short)(sum_real >> fix_shift);
		*(output_ptr + 1) = (short)(sum_imag >> fix_shift);

		output_ptr += 2;
		output_ix++;
	}
}



