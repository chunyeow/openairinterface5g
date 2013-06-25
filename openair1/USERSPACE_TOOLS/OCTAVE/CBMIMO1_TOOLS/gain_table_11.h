#ifndef _GAIN_TABLE_H__
#define _GAIN_TABLE_H__
#define MAX_RF_GAIN 150
#define MIN_RF_GAIN 96
struct gain_table_entry {
  unsigned char gain0;
  unsigned char gain1;
};
struct gain_table_entry gain_table[MAX_RF_GAIN-MIN_RF_GAIN+1] = {
