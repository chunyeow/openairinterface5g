#ifndef _GAIN_TABLE_H__
#define _GAIN_TABLE_H__
#define MAX_RF_GAIN 150
#define MIN_RF_GAIN 130
struct gain_table_entry {
  unsigned char gain0;
  unsigned char gain1;
};
struct gain_table_entry gain_table[MAX_RF_GAIN-MIN_RF_GAIN+1] = {
25, 34,
28, 37,
31, 39,
34, 42,
36, 45,
39, 47,
42, 50,
44, 53,
47, 55,
49, 58,
52, 60,
54, 63,
57, 65,
59, 68,
62, 70,
64, 73,
67, 75,
69, 78,
72, 80,
74, 83,
77, 85};
#endif /* _GAIN_TABLE_H__ */
