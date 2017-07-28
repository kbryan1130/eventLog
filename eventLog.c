/*****************************************************************
 *
 *                      Event Log
 *
 *****************************************************************
 * Desc:    Maintenance process to support event logging. 
 *****************************************************************/

#include <plib.h>
#include <peripheral/rtcc.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
//#include <stdlib.h>

#include "eventLog_if.h"


extern time_t mTimestamp;

/*****************************************************************
*  Macros
*****************************************************************/
#define MAX_LOG_ENTRIES (50) //(256)


/*****************************************************************
*  Typedefs
*****************************************************************/


/*****************************************************************
*  Global Data
*****************************************************************/
WORD  mFirst  = 0;
WORD  mWRnext = 0;
WORD  mRDnext = 0;

WORD stuff;

eventLog_Rec_t eventLog[MAX_LOG_ENTRIES];

BOOL   mRollover = FALSE;
UINT32 mNumLogEntries = 0;


/*****************************************************************
* MAINT_initEventLog()
*
* Initializes an empty event log.
*****************************************************************/
void MAINT_initEventLog(void)
{
   WORD n;

   /* clear the log */
   for (n=0; n<MAX_LOG_ENTRIES; n++)
   {
      memset(&(eventLog[n]), 0, sizeof(eventLog_Rec_t));
   }
   
   /* initialize all event log markers for an empty log file */
   mFirst  = 0;
   mWRnext = 0;
   mRDnext = 0;
   mRollover = FALSE;

   mNumLogEntries = 0;
}


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
void MAINT_writeEventLog(LOG_SUBSYSTEM sub, LOG_SEVERITY sev, char* fmt, 
                         UINT32* arg1, UINT32* arg2, UINT32* arg3)
{
   eventLog_Rec_t *logRec = &(eventLog[mWRnext]);
   
   taskENTER_CRITICAL();
   
   logRec->timestamp = mTimestamp;
   logRec->sev = sev;
   logRec->sub = sub;

   memset(&(logRec->logText[0]), 0, sizeof(char)*LOGTEXT_MAX_SIZE);
   
   if (arg3)
      sprintf(&(logRec->logText[0]), fmt, *arg1, *arg2, *arg3);
   else if (arg2)      
      sprintf(&(logRec->logText[0]), fmt, *arg1, *arg2);
   else if (arg1)
      sprintf(&(logRec->logText[0]), fmt, *arg1);
   else
      sprintf(&(logRec->logText[0]), fmt);

   if (++mWRnext >= MAX_LOG_ENTRIES)
   {
      mWRnext   = 0;
      mRollover = TRUE;
   }
   if (mRollover)
      if (++mFirst >= MAX_LOG_ENTRIES) mFirst = 0;
      
   taskEXIT_CRITICAL();

   ++mNumLogEntries;
}


/*****************************************************************
* MAINT_readEventLog()
*
* direction = first, or next record.
* logEntry  = buffer to place the requested log record in.
*
* Returns TRUE if successful, FALSE if no record found
*****************************************************************/
BOOL MAINT_readEventLog(LOG_DIRECTION direction, eventLog_Rec_t* logEntry)
{
   if (mFirst == mWRnext)  // empty log
   {
      return (FALSE);
   }
   else if (logEntry)
   {
      if (direction == LOG_DIR_FIRST)
      {
         memcpy(logEntry, &(eventLog[mFirst]), sizeof(eventLog_Rec_t));
         mRDnext = mFirst;
      }
      else if (direction == LOG_DIR_NEXT)
      {
         if (mRDnext == mFirst)  return (FALSE); // wrapped
         if (mRDnext == mWRnext) return (FALSE); // EOF
         
         memcpy(logEntry, &(eventLog[mRDnext]), sizeof(eventLog_Rec_t));
      }
      
      if (++mRDnext >= MAX_LOG_ENTRIES) mRDnext = 0;

      return (TRUE);
   }   
}


