#ifndef _GAIN_TABLE_H__
#define _GAIN_TABLE_H__
#define MAX_RF_GAIN 150
#define MIN_RF_GAIN 130
struct gain_table_entry {
  unsigned char gain0;
  unsigned char gain1;
};
struct gain_table_entry gain_table[MAX_RF_GAIN-MIN_RF_GAIN+1] = {
46, 56,
49, 58,
51, 61,
54, 63,
56, 66,
59, 68,
62, 71,
64, 73,
67, 76,
69, 78,
72, 81,
74, 83,
77, 85,
79, 88,
82, 90,
84, 93,
87, 95,
89, 98,
92, 100,
94, 103,
97, 106};
#endif /* _GAIN_TABLE_H__ */
