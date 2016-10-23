#include <stdio.h>		/* printf (), sprintf (), fprintf (), fscanf () */
#include <unistd.h>		/* sleep (), usleep () */
#include <dirent.h>		/* PATH_MAX */
#include <string.h>		/* strcat (), strcmp (), strcpy () */
#include <stdlib.h>		/* exit (), system ()*/
#include <term.h>		/* ClearScreen () */

#include "test_app.h"


/* main */
int main (int argc, char *argv[])
{
	char *process_name;
	char pid_str[10];
	int pid_num;
	unsigned int loop_counter = 0;
	proc_pid_stat_tp cpu_u_c;
	proc_meminfo_tp r_proc_m;
	proc_pid_sched_tp r_proc_p;
	proc_pid_io_tp r_w_bytes;

	if (argc < 2)
	{
		fprintf (stderr, "Usage: %s <process name>\n", argv[0]);
		return 1;
	}

	if (!strcmp (argv[1], "-h"))
	{
		print_help ();
		exit (0);
	}

	pid_num = find_pid_by_name (argv[2]);
	if (pid_num == -1)
	{
		process_name = argv[1];
		pid_num = find_pid_by_name (argv[1]);
		if (pid_num == -1)
		{
			perror ("Exit");
			exit (0);
		}
	}
	else
		process_name = argv[2];

	sprintf (pid_str, "%d", pid_num);
#if PRINT_ON_CLEAR_SCREEN
/*	system ("clear");*/
	ClearScreen ();
	system ("setterm -cursor off");
#endif
/*	printf ("Kernel version number:\n");*/
/*	system ("uname -r");*/
	printf ("\n\n");
	system ("cat /proc/version");
	printf ("The system page size is %ld bytes.\n", sysconf (_SC_PAGESIZE));
	proc_pid_statm (pid_str);
	sleep (2);

	while (proc_is_running (pid_num, process_name))
	{
		cpu_u_c = proc_pid_stat (pid_str);
		if (cpu_u_c.succ_flag)
		{
			perror ("/proc/[pid]/stat");	/* No such file or directory */
			printf ("Ooops!\n");
			/*exit (0);*/					/* If no stat - exit here. */
		}

		if (!strcmp (argv[1], "--pid_io"))
		{
			r_w_bytes = proc_pid_io (pid_str);
			if (r_w_bytes.succ_flag)
			{
				perror ("/proc/[pid]/io");
				printf ("\n");
			}
		}

		if (!strcmp (argv[1], "--pid_sched"))
		{
			r_proc_p = proc_pid_sched (pid_str);
			if (r_proc_p.succ_flag)
			{
				perror ("/proc/[pid]/sched");
				printf ("\n");
			}
		}

		if (!strcmp (argv[1], "--meminfo"))
		{
			r_proc_m = proc_meminfo ();
		}

		if (!strcmp (argv[1], "-h"))
		{
				 print_help ();
		}
		/* usleep (250000); */
		sleep (1);

#if PRINT_ON_CLEAR_SCREEN
/*		system ("clear");*/
		ClearScreen ();
#endif
	loop_counter++;
	}

	double cpu_sum =
			cpu_u_c.cpu0_usage_counter +
			cpu_u_c.cpu1_usage_counter +
			cpu_u_c.cpu2_usage_counter +
			cpu_u_c.cpu3_usage_counter;

	double cpu0_repr = (cpu_u_c.cpu0_usage_counter / cpu_sum) * 100.0;
	double cpu1_repr = (cpu_u_c.cpu1_usage_counter / cpu_sum) * 100.0;
	double cpu2_repr = (cpu_u_c.cpu2_usage_counter / cpu_sum) * 100.0;
	double cpu3_repr = (cpu_u_c.cpu3_usage_counter / cpu_sum) * 100.0;

	/* Mandatory report */
	printf ("*** Overall results in process %s\n\n", cpu_u_c.tcomm);
	printf ("Core/CPU representation\n");
	printf ("---------------------------------------------------\n");
	printf ("CPU0: %lf %c\n", cpu0_repr, 37);
	printf ("CPU1: %lf %c\n", cpu1_repr, 37);
	printf ("CPU2: %lf %c\n", cpu2_repr, 37);
	printf ("CPU3: %lf %c\n\n", cpu3_repr, 37);

	printf ("Average CPU usage\n");
	printf ("---------------------------------------------------\n");
	printf ("CPU usage: %lf %c\n", cpu_u_c.cpu_usage_sum / loop_counter, 37);
	printf ("CPU usage (min): %lf %c\n", cpu_u_c.cpu_usage_min, 37);
	printf ("CPU usage (max): %lf %c\n\n", cpu_u_c.cpu_usage_max, 37);

	printf ("Resident Set Size usage\n");
	printf ("---------------------------------------------------\n");
	printf ("RSS usage (min): %lld pages\n", cpu_u_c.rss_usage_min);
	printf ("RSS usage (max): %lld pages\n", cpu_u_c.rss_usage_max);
	printf ("\n\n");

	/* Optional reports */
	if (!strcmp (argv[1], "--pid_io") && r_w_bytes.succ_flag == 0)
	{
		printf ("*** I/O results in process %s\n\n", cpu_u_c.tcomm);
		printf ("---------------------------------------------------\n");
		printf (
			"Directly read from disk: %llu bytes\n\n", r_w_bytes.read_bytes);
		printf (
			"Sent to the storage layer: %llu bytes\n\n", r_w_bytes.write_bytes);
	}

	if (!strcmp (argv[1], "--pid_sched"))
	{
		printf ("*** Scheduler (CFS) results in process %s\n\n", cpu_u_c.tcomm);
		printf ("Task's out-of-balance execution history\n");
		printf ("---------------------------------------------------\n");
		printf ("Virtual run time (min): %lf ns\n", r_proc_p.vruntime_min);
		printf ("Virtual run time (max): %lf ns\n\n", r_proc_p.vruntime_max);
	}

	if (!strcmp (argv[1], "--meminfo"))
	{
		printf ("*** Memory info (general)\n\n");
		printf ("---------------------------------------------------\n");
		printf ("Dirty memory (min): %ld kB\n", r_proc_m.dirty_min);
		printf ("Dirty memory (max): %ld kB\n\n", r_proc_m.dirty_max);
	}

/*
	printf ("CPU0     CPU1     CPU2     CPU3\n");
	printf ("-------------------------------\n");
	for (i=0; i<150; i++)
		printf ("%.2f     %.2f     %.2f     %.2f\n",
				cpu0_usage_buff[i],
				cpu1_usage_buff[i],
				cpu2_usage_buff[i],
				cpu3_usage_buff[i]);
*/
	exit (0);
}

