#ifndef ACCESS_RESTRICTION_H_
#define ACCESS_RESTRICTION_H_

int split_plmn(uint8_t *plmn, uint8_t mcc[3], uint8_t mnc[3]);

int apply_access_restriction(char *imsi, uint8_t *vplmn);

#endif /* ACCESS_RESTRICTION_H_ */
