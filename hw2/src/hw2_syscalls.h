#ifndef GET_SCHEDULING_STATISTIC_H
#define GET_SCHEDULING_STATISTIC_H

#include <errno.h>

#define MAX_SWITCHES_HISTORY    150

/*
 * Reasons for context_switch. Used by monitoring mechanism.
 */
#define REASON_TIME_SLICE       1
#define REASON_WAITING          2
#define REASON_PRIO             3
#define REASON_CREATED          4
#define REASON_ENDED            5
#define REASON_YIELD            6


/*
 * This struct contains information on a single task switching.
 */
struct switch_info
{
  int previous_pid;
  int next_pid;
  int previous_policy;
  int next_policy;
  unsigned long time;
  int time_slice;
  int reason;
};


int prod_query_remaining_time(int pid)
{
  int res;

  __asm__
    (
     "movl $243, %%eax;"
     "movl %1,   %%ebx;"
     "int $0x80;"
     "movl %%eax, %0"
     : "=m" (res)
     : "m" (pid)
     : "%eax", "%ebx"
     );
  
  if(res < 0) {
    errno = res;
    res = -1;
  }

  return res;
}

int prod_query_expited_time(int pid)
{
  int res;

  __asm__
    (
     "movl $244, %%eax;"
     "movl %1,   %%ebx;"
     "int $0x80;"
     "movl %%eax, %0"
     : "=m" (res)
     : "m" (pid)
     : "%eax", "%ebx"
     );
  
  if(res < 0){
    errno = res;
    res = -1;
  }
  return res;
}

int get_scheduling_statistic(struct switch_info* switches_history)
{
  int res;

  __asm__
    (
     "movl $245, %%eax;"
     "movl %1,   %%ebx;"
     "int $0x80;"
     "movl %%eax, %0"
     : "=m" (res)
     : "m" (switches_history)
     : "%eax", "%ebx"
     );
  
  if(res == -1)
    errno = -EFAULT;
  return res;
}

#endif
