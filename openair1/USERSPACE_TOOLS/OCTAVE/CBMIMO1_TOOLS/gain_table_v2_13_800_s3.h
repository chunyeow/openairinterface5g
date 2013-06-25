#ifndef _GAIN_TABLE_H__
#define _GAIN_TABLE_H__
#define MAX_RF_GAIN 150
#define MIN_RF_GAIN 130
struct gain_table_entry {
  unsigned char gain0;
  unsigned char gain1;
};
struct gain_table_entry gain_table[MAX_RF_GAIN-MIN_RF_GAIN+1] = {
32, 32,
35, 35,
38, 38,
40, 40,
43, 43,
46, 46,
48, 48,
51, 51,
54, 53,
56, 56,
59, 58,
61, 61,
64, 63,
67, 66,
69, 68,
72, 71,
75, 73,
77, 76,
80, 79,
82, 81,
85, 84};
#endif /* _GAIN_TABLE_H__ */
