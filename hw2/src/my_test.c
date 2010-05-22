#include <stdio.h>
#include <string.h>
#include "hw2_syscalls.h"

void print_switch_info(struct switch_info);

int main(int argc, char** argv)
{

  int res, i;

  struct switch_info* arr = (struct switch_info*)malloc(MAX_SWITCHES_HISTORY * sizeof(struct switch_info)); 

  //res = get_scheduling_statistic(arr);
  //sleep(10);
  sched_yield();
  res = get_scheduling_statistic(arr);

  if (res >= 0){
    for(i = 0; i < res; i++)
      print_switch_info(arr[i]);
  }
  printf("Total printed: %d, errno: %d \n", res, errno);

  free(arr);

  return 0;
  

  /*
  
  int criti, time, cost, i;
  for(i = 1; i < argc - 1; i += 3){
    criti = atoi(argv[i]);
    time = atoi(argv[i+1]);
    cost = atoi(argv[i+2]);
    
    // execute  processes here
  }

  struct switch_info* arr = (struct switch_info*)malloc(MAX_SWITCHES_HISTORY * sizeof(struct switch_info)); 
  
  res = get_scheduling_statistic(arr);

  if (res >= 0){
    for(i = 0; i < res; i++)
      print_switch_info(arr[i]);
  }
  printf("Total printed: %d, errno: %d \n", res, errno);

  free(arr);

  */
  


  return 0;

}


void print_switch_info(struct switch_info info)
{
  printf("%d\t%d\t%d\t%d\t%lu\t%d\t%d\n", 
	 info.previous_pid,
	 info.next_pid,
	 info.previous_policy,
	 info.next_policy,
	 info.time,
	 info.time_slice,
	 info.reason);
}






