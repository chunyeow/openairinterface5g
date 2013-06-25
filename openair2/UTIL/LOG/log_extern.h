
extern log_t *g_log;

extern LOG_params log_list[2000];
extern pthread_mutex_t log_lock;
extern pthread_cond_t log_notify;
extern int log_shutdown;
