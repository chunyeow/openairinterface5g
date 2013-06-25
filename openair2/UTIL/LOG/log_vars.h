

// main log variables
log_t *g_log;

// vars for the log thread
LOG_params log_list[2000];
int log_list_head = 0;
int log_list_tail = 0;
int log_list_nb_elements = 0;
pthread_mutex_t log_lock;
pthread_cond_t log_notify;
int log_shutdown;
