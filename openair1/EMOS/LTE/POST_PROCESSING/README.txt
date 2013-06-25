This folder contains all the matlab code for the post processing

There are two levels of post processing
 1) Integrity checks that visualize some of the recorded data
 2) Capacity evaluations of the channel measurements 

ad 1) The files for the integrity check are located in the POST_PROCESSING directory.
The top level files are:
 - start_post_processing_quick.m
     This is the top level script that invokes all of the other files. START HERE!
 - post_processing_eNb_quick.m
     This script processes the eNB measurements
 - post_processing_UE_quick.m
     This script processes the UE measurements
 - post_processing_UE_nomadic_quick.m
     This script processes the nomadic measurements
 - plot_results_UE_quick.m
     This script plots the results after a post_processing_UE_quick.m 
 - plot_all_ue.m
     This script collects results from severals directories and plots them 
 - plot_results_UE_quick_common.m
     Common to the two above
 - plot_results_eNb_quick.m
     This script plots the results after a post_processing_eNB_quick.m 
 - plot_results_I0_quick.m
     This script plots only the IO results after a post_processing_UE_quick.m 
 - eval_nomadic_points.m
     This script produces a table after from the nomadic measuremetns


ad 2) The files for the capacity evaluations are loacted in the POST_PROCESSING/CapCom directory.
The top level files are
 - integrity_check_1.m  (start_capcom.m)
     This is the top level script that invokes all of the other files. START HERE!
     This script produces a .mat file for every .EMOS file in the same directory.
 - integrity_check_11.m (start_capcom_multi_dir.m)
     This is the same as before but going over multiple directories.
- Concatenate_results_UE_quick.m
     This script collects all the .mat files in a specific directory. Call this before plotting the results.
 - plot_results_cat.m
     Plots the results after the above script

