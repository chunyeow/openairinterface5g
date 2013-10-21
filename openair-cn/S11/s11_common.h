#ifndef S11_COMMON_H_
#define S11_COMMON_H_

#define S11_DEBUG(x, args...) fprintf(stdout, "[S11] [D]"x, ##args)
#define S11_INFO(x, args...)  fprintf(stdout, "[S11] [I]"x, ##args)
#define S11_WARN(x, args...)  fprintf(stdout, "[S11] [W]"x, ##args)
#define S11_ERROR(x, args...) fprintf(stderr, "[S11] [E]"x, ##args)

NwRcT s11_ie_indication_generic(NwU8T  ieType,
                                NwU8T  ieLength,
                                NwU8T  ieInstance,
                                NwU8T *ieValue,
                                void  *arg);

#endif /* S11_COMMON_H_ */
