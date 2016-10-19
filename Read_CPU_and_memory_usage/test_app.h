#ifndef TEST_APP_H_
#define TEST_APP_H_

#include <stdio.h>		/* printf (), sprintf (), fprintf (), fscanf () */
#include <unistd.h>		/* sleep (), usleep () */
#include <dirent.h>		/* PATH_MAX */
#include <string.h>		/* strcat (), strcmp (), strcpy () */
#include <stdlib.h>		/* exit (), system ()*/
#include <term.h>		/* ClearScreen () */

/*
	Use define below for logging / printing.

	Argument <arg> is optional.

	Example for logging:
	PRINT_ON_CLEAR_SCREEN 0
	./test_app <arg> test_prg > test_log.log

	Example for printing:
	PRINT_ON_CLEAR_SCREEN 1
	./test_app <arg> test_prg
*/
#define PRINT_ON_CLEAR_SCREEN 1

typedef long long int num;


typedef struct {
	num size;
	num resident;
	num share;
	num text;
	num lib;
	num data;
	num dt;
} vars;


typedef struct {
	unsigned int succ_flag;
	unsigned int cpu0_usage_counter;
	unsigned int cpu1_usage_counter;
	unsigned int cpu2_usage_counter;
	unsigned int cpu3_usage_counter;
	num rss_usage_min;
	num rss_usage_max;
	double cpu_usage_min;
	double cpu_usage_max;
	double cpu_usage_sum;
	char tcomm[PATH_MAX];
} proc_pid_stat_tp;


typedef struct {
	unsigned int succ_flag;
	double vruntime_min;
	double vruntime_max;
} proc_pid_sched_tp;


typedef struct {
	unsigned int succ_flag;
	num read_bytes;
	num write_bytes;
} proc_pid_io_tp;


typedef struct {
	unsigned int succ_flag;
	unsigned long dirty_min;
	unsigned long dirty_max;
} proc_meminfo_tp;


int find_pid_by_name (char *prg_name);
int proc_is_running (int pid_num, char *prg_name);
int get_time_since_boot (long tickspersec);
proc_meminfo_tp proc_meminfo ();
double get_num (const char *str);
int proc_pid_statm (char *pid_num);
proc_pid_sched_tp proc_pid_sched (char *pid_num);
proc_pid_io_tp proc_pid_io (char *pid_num);
proc_pid_stat_tp proc_pid_stat (char *pid_num);
void ClearScreen ();
void print_help ();

#endif /* TEST_APP_H_ */
