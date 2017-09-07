/*****************************************************************
 *
 *                      Event Logger Interface
 *
 *****************************************************************/
#ifndef _EVENTLOG_IF_H
#define _EVENTLOG_IF_H

#include "GenericTypeDefs.h"
//#include <stdio.h>


/*****************************************************************
*  Macros
*****************************************************************/
#define  MAINT_SUBSYSTEM         ("MAINT")
#define  NETWORK_SUBSYSTEM       ("NWK  ")
#define  PROFILESERVER_SUBSYSTEM ("PSERV")

#define  LOG_SEV_CRITICAL ("CR")
#define  LOG_SEV_MAJOR    ("MJ")
#define  LOG_SEV_MINOR    ("MN")
#define  LOG_SEV_INFO     ("IN")
#define  LOG_SEV_THRESH   ("TH")
#define  LOG_SEV_DEBUG    ("DB")

#define  LOGTEXT_MAX_SIZE  (60)

#define  STUFF				(1)	// define stuff
#define  NEWSTUFF			(2)

/*****************************************************************
*  Typedefs
*****************************************************************/
typedef enum
{
   LOG_SUB_MT = 0,
   LOG_SUB_NET,
   LOG_SUB_PS,
} LOG_SUBSYSTEM;
   
typedef enum
{
   LOG_SEV_CR = 0,
   LOG_SEV_MJ,
   LOG_SEV_MN,
   LOG_SEV_IN,
   LOG_SEV_TH,
   LOG_SEV_DB,
} LOG_SEVERITY;
   
typedef enum
{
   LOG_DIR_FIRST = 0,
   LOG_DIR_NEXT,
} LOG_DIRECTION;

#ifndef TIME_T
#define TIME_T
typedef long time_t;
#endif

typedef struct
{
   time_t   timestamp;
   LOG_SEVERITY   sev;
   LOG_SUBSYSTEM  sub;
   char  logText[LOGTEXT_MAX_SIZE];
} eventLog_Rec_t;
   

/*****************************************************************
*  Functions
*****************************************************************/

/*****************************************************************
* MAINT_writeEventLog()
*
* Desc:    Provides methods to write log events by subsystem.
*
* Where:
*          mLog - Maintenance subsystem logger
*          nLog - Network subsystem logger
*          pLog - Profile Server logger
*
* sev:     severity, one of {CRITICAL,MAJOR,MINOR,INFO,THRESH,DEBUG}
*
* fmt:     standard printf format (40 chars max, including \0)
*
* argN:    optional argument(s) to printf format.  Max three args.
*
* example: mLog(CRITICAL,"something failed at %d",cnt);
*****************************************************************/
#define  mLOG(sev,fmt,arg1,arg2,arg3) (MAINT_writeEventLog(LOG_SUB_MT,  sev, fmt, arg1, arg2, arg3))
#define  nLOG(sev,fmt,arg1,arg2,arg3) (MAINT_writeEventLog(LOG_SUB_NET, sev, fmt, arg1, arg2, arg3))
#define  pLOG(sev,fmt,arg1,arg2,arg3) (MAINT_writeEventLog(LOG_SUB_PS,  sev, fmt, arg1, arg2, arg3))

void MAINT_writeEventLog(LOG_SUBSYSTEM sub, LOG_SEVERITY sev, char* fmt, UINT32* arg1, UINT32* arg2, UINT32* arg3);


/*****************************************************************
* MAINT_readEventLog()
*
* direction = first, next, or last.
* logEntry  = memory to place the requested log record in.
*
* Returns TRUE if successful, FALSE if no record found
*****************************************************************/
BOOL MAINT_readEventLog(LOG_DIRECTION direction, eventLog_Rec_t* logEntry);


/*****************************************************************
* MAINT_initEventLog()
*
* Initializes an empty event log.
*****************************************************************/
void MAINT_initEventLog(void);


#endif /* _EVENTLOG_IF_H */
