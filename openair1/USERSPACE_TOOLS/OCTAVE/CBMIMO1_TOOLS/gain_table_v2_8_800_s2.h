#ifndef _GAIN_TABLE_H__
#define _GAIN_TABLE_H__
#define MAX_RF_GAIN 150
#define MIN_RF_GAIN 130
struct gain_table_entry {
  unsigned char gain0;
  unsigned char gain1;
};
struct gain_table_entry gain_table[MAX_RF_GAIN-MIN_RF_GAIN+1] = {
25, 30,
28, 33,
30, 36,
33, 38,
36, 41,
39, 44,
42, 46,
44, 49,
47, 52,
50, 54,
52, 57,
55, 59,
57, 62,
60, 64,
63, 67,
65, 69,
68, 72,
70, 75,
73, 77,
75, 80,
78, 82};
#endif /* _GAIN_TABLE_H__ */
