
#ifndef S11_SGW_BEARER_MANAGER_H_
#define S11_SGW_BEARER_MANAGER_H_

int s11_sgw_handle_modify_bearer_request(
    NwGtpv2cStackHandleT *stack_p,
    NwGtpv2cUlpApiT      *pUlpApi);

int s11_sgw_handle_modify_bearer_response(
    NwGtpv2cStackHandleT    *stack_p,
    SgwModifyBearerResponse *modify_bearer_response_p);

#endif /* S11_SGW_BEARER_MANAGER_H_ */
