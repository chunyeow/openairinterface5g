#ifndef S11_SGW_SESSION_MANAGER_H_
#define S11_SGW_SESSION_MANAGER_H_

int s11_sgw_handle_create_session_request(
    NwGtpv2cStackHandleT *stack_p,
    NwGtpv2cUlpApiT      *pUlpApi);

int s11_sgw_handle_create_session_response(
    NwGtpv2cStackHandleT     *stack_p,
    SgwCreateSessionResponse *create_session_response_p);

int s11_sgw_handle_delete_session_request(
    NwGtpv2cStackHandleT *stack_p,
    NwGtpv2cUlpApiT      *pUlpApi);

int s11_sgw_handle_delete_session_response(
    NwGtpv2cStackHandleT     *stack_p,
    SgwDeleteSessionResponse *delete_session_response_p);

#endif /* S11_SGW_SESSION_MANAGER_H_ */
