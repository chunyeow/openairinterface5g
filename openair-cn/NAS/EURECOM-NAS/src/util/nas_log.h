/*****************************************************************************
			Eurecom OpenAirInterface 3
			Copyright(c) 2012 Eurecom

Source		log.h

Version		0.1

Date		2012/02/28

Product		NAS stack

Subsystem	Utilities

Author		Frederic Maurel

Description	Usefull logging functions

*****************************************************************************/
#ifndef __NAS_LOG_H__
#define __NAS_LOG_H__

/****************************************************************************/
/*********************  G L O B A L    C O N S T A N T S  *******************/
/****************************************************************************/

/* -----------------------
 * Logging severity levels
 * -----------------------
 *	OFF	: Disables logging trace utilities.
 *	DEBUG	: Only used for debug purpose. Should be removed from the code.
 *	INFO	: Informational trace
 *	WARNING	: The program displays the warning message and doesn't stop.
 *	ERROR	: The program displays the error message and usually exits or
 *		  runs appropriate procedure.
 *	FUNC	: Prints trace when entering/leaving to/from function. Usefull
 *		  to display the function's calling tree information at runtime.
 *	ON	: Enables logging traces excepted FUNC.
 *	ALL	: Turns on ALL logging traces.
 */
#define LOG_OFF		0x00	/* No trace				*/
#define LOG_DEBUG	0x01	/* Debug trace				*/
#define LOG_INFO	0x02	/* Informational trace			*/
#define LOG_WARNING	0x04	/* Warning trace			*/
#define LOG_ERROR	0x08	/* Error trace				*/
#define LOG_FUNC	0x10	/* Entering/Leaving function trace	*/
#define LOG_HEX		0x20	/* Dump trace				*/

#define LOG_ON		0x0F	/* All traces excepted FUNC and HEX	*/
#define LOG_ALL		0xFF	/* All traces				*/

/* Logging severity type */
typedef enum
{
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    FUNC_IN,
    FUNC_OUT,
    LOG_SEVERITY_MAX
} log_severity_t;

/****************************************************************************/
/************************  G L O B A L    T Y P E S  ************************/
/****************************************************************************/

/****************************************************************************/
/********************  G L O B A L    V A R I A B L E S  ********************/
/****************************************************************************/

/****************************************************************************/
/******************  E X P O R T E D    F U N C T I O N S  ******************/
/****************************************************************************/

#define LOG_TRACE log_data(__FILE__, __LINE__); log_trace
#define LOG_DUMP(a, b) log_dump((a),(b));

#define LOG_FUNC_IN LOG_TRACE(FUNC_IN, "Entering %s()", __FUNCTION__)
#define LOG_FUNC_OUT LOG_TRACE(FUNC_OUT, "Leaving %s()", __FUNCTION__)
#define LOG_FUNC_RETURN(rETURNcODE)                                            \
do {                                                                           \
    LOG_TRACE(FUNC_OUT, "Leaving %s(rc = %ld)", __FUNCTION__,                  \
    (long) rETURNcODE);                                                        \
    return (rETURNcODE);                                                       \
} while(0)

void nas_log_init(char filter);
void log_data(const char* filename, int line);
void log_trace(log_severity_t severity, const char* data, ...);
void log_dump(const char* data, int len);

#endif /* __NAS_LOG_H__*/
