// Matrix Utilities for Capacity Calculations
// by Florian Kaltenberger
// Created 10.10.2007

# include "PHY/TOOLS/defs.h"

struct complexf **square_mat(struct complexf **H, int rows, int cols, struct complexf **HH);

struct complexf **addI_scale_mat(struct complexf **H, int rows, float scale);

float det(struct complexf **H);

float norm_fro_sq(struct complexf**H, int rows, int cols);
