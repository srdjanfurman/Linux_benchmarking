#include "test_app.h"


static double cpu_usage_min = 400.0;
static double cpu_usage_max = 0.0;
static double vruntime_min = 1000000000.0;
static double vruntime_max = 0.0;
static unsigned long dirty_min = 1000000.0;
static unsigned long dirty_max = 0;
static num rss_usage_min = 1000000000;
static num rss_usage_max = 0;

/* Find out PID based on the name of a program */
int find_pid_by_name (char *prg_name)
{
	FILE *f_pid_by_name;
	char pid_of_cmd[50] = {0};
	int pid_value = -1;

	if (prg_name != 0)
	{
		strcpy (pid_of_cmd, "pidof ");
		strcat (pid_of_cmd, prg_name);
		strcat (pid_of_cmd,  " > /tmp/pidof");
		system (pid_of_cmd);

		f_pid_by_name = fopen ("/tmp/pidof", "r");
		if (!f_pid_by_name)
		{
			perror ("/tmp/pidof");
			exit (0);				/* No PID - exit here. */
		}

		fscanf (f_pid_by_name, "%d", &pid_value);
		fclose (f_pid_by_name);
	}
	return pid_value;
}


/* Get number from string */
double get_num (const char *str)
{
	while (*str)
	{
		double number;
		if (sscanf (str, "%lf", &number) == 1)
		{
			return number;
		}
		str++;
	}
	return -1;	/* No number found. */
}


/* Test if process is running */
int proc_is_running (int pid_num, char *prg_name)
{
	return pid_num == find_pid_by_name (prg_name);
}


/* /proc/uptime */
int get_time_since_boot (long tickspersec)
{
	FILE *f_time_since_boot;
	int sec;
	int ssec;

	f_time_since_boot = fopen ("/proc/uptime", "r");
	if (!f_time_since_boot)
	{
		perror ("/proc/uptime");
		return 1;
	}
	fscanf (f_time_since_boot, "%d.%ds", &sec, &ssec);
	fclose (f_time_since_boot);
	return (sec * tickspersec) + ssec;
}

/* Cls */
void ClearScreen ()
{
	if (!cur_term)
	{
		int result;
		setupterm (NULL, STDOUT_FILENO, &result);
		if (result <= 0)
			return;
	}

	putp (tigetstr("clear"));
}


/* Help */
void print_help ()
{
	printf ("\nCommand line parameters:\n");
	printf ("	--pid_io\n"
"			This file contains I/O statistics for the process.\n");
	printf ("	--pid_sched\n"
"			New file - no data available yet...\n");
	printf ("	--meminfo\n"
"			This file reports statistics about memory usage on the system.\n");
	printf ("\n");
}


/* Loop fscanf */
num fscanf_do_loop (FILE *f_proc_pid_statm, char* frmt_str)
{
	num var;
	fscanf(f_proc_pid_statm, frmt_str, &var);
	return var;
}


/* /proc/[pid]/statm */
int proc_pid_statm (char *pid_num)
{
	FILE *f_proc_pid_statm = NULL;
	char cmd[50] = {0};
	char* format = "%lld ";
	unsigned int i;
	num value[7];

	strcpy (cmd, "/proc/");
	strcat (cmd, pid_num);
	strcat (cmd,  "/statm");

	f_proc_pid_statm = fopen (cmd, "r");
	if (!f_proc_pid_statm)
	{
		return 1;
	}

	for (i=0; i<7; i++)
		value[i] = fscanf_do_loop (f_proc_pid_statm, format);

/*	Total program size in pages. */
	printf ("Total program size is %lld pages.\n", value[0]);
	printf ("------------------------------------\n\n");
	fclose (f_proc_pid_statm);
	return 0;
}


/* /proc/meminfo */
proc_meminfo_tp proc_meminfo ()
{
	FILE *f_proc_meminfo = NULL;
	char cmd[50] = {0};
	unsigned long mem_total;
	unsigned long mem_free;
	unsigned long buffers;
	unsigned long cached;
	unsigned long swap_cached;
	unsigned long active;
	unsigned long inactive;
	unsigned long active_anon;
	unsigned long inactive_anon;
	unsigned long active_file;
	unsigned long inactive_file;
	unsigned long unevictable;
	unsigned long mlocked;
	unsigned long swap_total;
	unsigned long swap_free;
	unsigned long dirty;
	proc_meminfo_tp pm_tp;

	strcpy (cmd, "/proc/");
	strcat (cmd,  "meminfo");

	f_proc_meminfo = fopen (cmd, "r");
	if (!f_proc_meminfo)
	{
		pm_tp.succ_flag = 1;
		return pm_tp;
	}
	char p_kB[] = "kB";
	char p_mem_total[] = "MemTotal: ";
	char p_mem_free[] = "MemFree: ";
	char p_buffers[] = "Buffers: ";
	char p_cached[] = "Cached: ";
	char p_swap_cached[] = "SwapCached: ";
	char p_active[] = "Active: ";
	char p_inactive[] = "Inactive: ";
	char p_active_anon[] = "Active(anon): ";
	char p_inactive_anon[] = "Inactive(anon): ";
	char p_active_file[] = "Active(file): ";
	char p_inactive_file[] = "Inactive(file): ";
	char p_unevictable[] = "Unevictable: ";
	char p_mlocked[] = "Mlocked: ";
	char p_swap_total[] = "SwapTotal: ";
	char p_swap_free[] = "SwapFree: ";
	char p_dirty[] = "Dirty: ";

	fscanf (f_proc_meminfo, "%s %ld %s", p_mem_total, &mem_total, p_kB);
	fscanf (f_proc_meminfo, "%s %ld %s", p_mem_free, &mem_free, p_kB);
	fscanf (f_proc_meminfo, "%s %ld %s", p_buffers, &buffers, p_kB);
	fscanf (f_proc_meminfo, "%s %ld %s", p_cached, &cached, p_kB);
	fscanf (f_proc_meminfo, "%s %ld %s", p_swap_cached, &swap_cached, p_kB);
	fscanf (f_proc_meminfo, "%s %ld %s", p_active, &active, p_kB);
	fscanf (f_proc_meminfo, "%s %ld %s", p_inactive, &inactive, p_kB);
	fscanf (f_proc_meminfo, "%s %ld %s", p_active_anon, &active_anon, p_kB);
	fscanf (f_proc_meminfo, "%s %ld %s", p_inactive_anon, &inactive_anon, p_kB);
	fscanf (f_proc_meminfo, "%s %ld %s", p_active_file, &active_file, p_kB);
	fscanf (f_proc_meminfo, "%s %ld %s", p_inactive_file, &inactive_file, p_kB);
	fscanf (f_proc_meminfo, "%s %ld %s", p_unevictable, &unevictable, p_kB);
	fscanf (f_proc_meminfo, "%s %ld %s", p_mlocked, &mlocked, p_kB);
	fscanf (f_proc_meminfo, "%s %ld %s", p_swap_total, &swap_total, p_kB);
	fscanf (f_proc_meminfo, "%s %ld %s", p_swap_free, &swap_free, p_kB);
	fscanf (f_proc_meminfo, "%s %ld %s", p_dirty, &dirty, p_kB);

	/* Find min/max dirty. */
	if (dirty > dirty_max)
	{
		dirty_max = dirty;
	}

	if (dirty < dirty_min)
	{
		dirty_min = dirty;
	}

/*	Total usable RAM. */
	printf ("Total: %ld\n", mem_total);
/*	The sum of LowFree+HighFree. */
	printf ("Free: %ld\n", mem_free);
/*	Temporary storage for raw disk blocks. */
	printf ("Buffers: %ld\n", buffers);
/*	In-memory cache for files read from the disk (the page cache). */
	printf ("Cached: %ld\n", cached);
/*	Memory that once was swapped out,
	is swapped back in but still also is in the swap file. */
	printf ("Swap cached: %ld\n", swap_cached);
/*	Memory that has been used more recently and
	usually not reclaimed unless absolutely necessary. */
	printf ("Active: %ld\n", active);
/*	Memory which has been less recently used. It is
	more eligible to be reclaimed for other purposes. */
	printf ("Inactive: %ld\n", inactive);
/*	Total amount of swap space available. */
	printf ("Swap total: %ld\n", swap_total);
/*	Amount of swap space that is currently unused. */
	printf ("Swap free: %ld\n", swap_free);
/*	Memory which is waiting to get written back to the disk. */
	printf ("Dirty: %ld\n", dirty);
	printf ("-------------------------------\n\n");

	fclose (f_proc_meminfo);
	pm_tp.succ_flag = 0;
	pm_tp.dirty_min = dirty_min;
	pm_tp.dirty_max = dirty_max;
	return pm_tp;
}


/* /proc/[pid]/sched */
proc_pid_sched_tp proc_pid_sched (char *pid_num)
{
/*	Timing fields are in [ns]. */
	FILE *f_read_proc_pid_sched = NULL;
	char temp[1000];
	char cmd[50] = {0};
	char *p;
	char *q_sum_exec_runtime = "se.sum_exec_runtime";
	char *q_wait_max = "se.statistics.wait_max";
	char *q_nr_switches = "nr_switches";
	char *q_avg_atom = "avg_atom";
	char *q_avg_per_cpu = "avg_per_cpu";
	char *q_vruntime = "se.vruntime";
	int nr_switches;					/* nr_switches */
	double sum_exec_runtime;			/* se.sum_exec_runtime */
	double wait_max;					/* se.statistics.wait_max */
	double avg_atom;					/* se.sum_exec_runtime / nr_switches */
	double avg_per_cpu;					/* avg_per_cpu */
	double vruntime;					/* se.vruntime */
	proc_pid_sched_tp ppsched_tp;

	strcpy (cmd, "/proc/");
	strcat (cmd, pid_num);
	strcat (cmd,  "/sched");

	f_read_proc_pid_sched = fopen (cmd, "r");
	if (!f_read_proc_pid_sched)
	{
		ppsched_tp.succ_flag = 1;
		return ppsched_tp;
	}

	while (fgets (temp, sizeof (temp), f_read_proc_pid_sched))
	{
		p = strtok (temp, ":,");

		if (!strncmp (q_sum_exec_runtime, p, strlen (q_sum_exec_runtime)))
		{
			sum_exec_runtime = get_num (strtok (NULL, ":,"));
		}

		if (!strncmp (q_wait_max, p, strlen (q_wait_max)))
		{
			wait_max = get_num (strtok (NULL, ":,"));
		}

		if (!strncmp (q_nr_switches, p, strlen (q_nr_switches)))
		{
			nr_switches = get_num (strtok (NULL, ":,"));
		}

		if (!strncmp (q_avg_atom, p, strlen (q_avg_atom)))
		{
			avg_atom = get_num (strtok (NULL, ":,"));
		}

		if (!strncmp (q_avg_per_cpu, p, strlen (q_avg_per_cpu)))
		{
			avg_per_cpu = get_num (strtok (NULL, ":,"));
		}

		if (!strncmp (q_vruntime, p, strlen (q_vruntime)))
		{
			vruntime = get_num (strtok (NULL, ":,"));
		}
	}

	/* Find min/max virtual run time. */
	if (vruntime > vruntime_max)
	{
		vruntime_max = vruntime;
	}

	if (vruntime < vruntime_min)
	{
		vruntime_min = vruntime;
	}

/*	The number of context switches. */
	printf ("Number of context switches: %d\n", nr_switches);
/*	Accumulated amount of time spent on the CPU. */
	printf ("Time spent on the CPU: %lf\n", sum_exec_runtime);
/*	Max wait time... */
	printf ("Max wait time: %lf\n", wait_max);
/*	Task's execution history. */
	printf ("Task's execution history: %lf\n", vruntime);
/*
	The average time ('scheduling atom') a task has spent executing on the CPU
	between two context-switches.
	The lower this value, the more context-switching happy a task is.

	double avg_atom = sum_exec_runtime;
	if (nr_switches)
		avg_atom = sum_exec_runtime / nr_switches;
*/
	printf ("Scheduling atom: %lf\n", avg_atom);
/*	Max wait time... */
	printf ("Time per CPU: %lf\n", avg_per_cpu);
	printf ("----------------------------------\n\n");

	fclose (f_read_proc_pid_sched);
	ppsched_tp.succ_flag = 0;
	ppsched_tp.vruntime_min = vruntime_min;
	ppsched_tp.vruntime_max = vruntime_max;
	return ppsched_tp;
}


/* /proc/[pid]/io */
proc_pid_io_tp proc_pid_io (char *pid_num)
{
	FILE *f_proc_pid_io = NULL;
	char cmd[50] = {0};
	num rchar;
	num wchar;
	num syscr;
	num syscw;
	num read_bytes;
	num write_bytes;
	num cancelled_write_bytes;
	proc_pid_io_tp ppi_tp;

	strcpy (cmd, "/proc/");
	strcat (cmd, pid_num);
	strcat (cmd,  "/io");

	f_proc_pid_io = fopen (cmd, "r");
	if (!f_proc_pid_io)
	{
		ppi_tp.succ_flag = 1;
		return ppi_tp;
	}
	char p_rchar[] = "rchar: ";
	char p_wchar[] = "wchar: ";
	char p_syscr[] = "syscr: ";
	char p_syscw[] = "syscw: ";
	char p_read_bytes[] = "read_bytes: ";
	char p_write_bytes[] = "write_bytes: ";
	char p_cancelled_write_bytes[] = "cancelled_write_bytes: ";

	fscanf (f_proc_pid_io, "%s %lld", p_rchar, &rchar);
	fscanf (f_proc_pid_io, "%s %lld", p_wchar, &wchar);
	fscanf (f_proc_pid_io, "%s %lld", p_syscr, &syscr);
	fscanf (f_proc_pid_io, "%s %lld", p_syscw, &syscw);
	fscanf (f_proc_pid_io, "%s %lld", p_read_bytes, &read_bytes);
	fscanf (f_proc_pid_io, "%s %lld", p_write_bytes, &write_bytes);
	fscanf (f_proc_pid_io, "%s %lld",
			p_cancelled_write_bytes, &cancelled_write_bytes);

/*	I/O counters */

/*	The number of bytes which this task has caused to be read from storage.
	This is simply the sum of bytes which this process passed to read()
	and pread(). Number of bytes the process read, using any read-like system
	call (from files, pipes, tty...). */
	printf ("Read characters: %llu\n", rchar);
/*	The number of bytes which this task has caused, or shall cause to be
	written to disk. Number of bytes the process wrote using any write-like
	system call. */
	printf ("Write characters: %llu\n", wchar);
/*	Number of read-like system call invocations that the process performed. */
	printf ("Read syscalls: %llu\n", syscr);
/*	Number of write-like system call invocations that the process performed. */
	printf ("Write syscalls: %llu\n", syscw);
/*	The number of bytes which this process really did cause to be fetched from
	the storage layer. Number of bytes the process directly read from disk. */
	printf ("Read bytes: %llu\n", read_bytes);
/*	The number of bytes which this process caused to be sent to the storage
	layer. Number of bytes the process originally dirtied in the page-cache
	(assuming they will go to disk later). */
	printf ("Write bytes: %llu\n", write_bytes);
/*	Number of bytes the process "un-dirtied" - e.g. using an "ftruncate" call
	that truncated pages from the page-cache. */
	printf ("Cancelled bytes written: %llu\n", cancelled_write_bytes);
	printf ("-------------------------------\n\n");

	fclose (f_proc_pid_io);
	ppi_tp.read_bytes = read_bytes;
	ppi_tp.write_bytes = write_bytes;
	ppi_tp.succ_flag = 0;
	return ppi_tp;
}


/* /proc/[pid]/stat */
proc_pid_stat_tp proc_pid_stat (char *pid_num)
{
	FILE *f_proc_pid_stat = NULL;
	/* System number of clock ticks per second. */
	long tickspersec = sysconf (_SC_CLK_TCK);
	char cmd[50] = {0};
	proc_pid_stat_tp ppstat_tp;
	num pid;
	char state;
	int i;
	char* format = "%lld ";
	num value[38];

	strcpy (cmd, "/proc/");
	strcat (cmd, pid_num);
	strcat (cmd,  "/stat");

	f_proc_pid_stat = fopen (cmd, "r");
	if (!f_proc_pid_stat)
	{
		ppstat_tp.succ_flag = 1;
		return ppstat_tp;
	}

	fscanf (f_proc_pid_stat, "%lld ", &pid);			/* pid */
	fscanf (f_proc_pid_stat, "%s ", ppstat_tp.tcomm);
	fscanf (f_proc_pid_stat, "%c ", &state);

	for (i=0; i<38; i++)
	{
		if (i == 19)
			format = "%llu ";
		else
			format = "%lld ";
		value[i] = fscanf_do_loop (f_proc_pid_stat, format);
	}

	int uptime = get_time_since_boot (tickspersec);
	num total_time = value[10] + value[11]; /* + cutime + cstime; */ /* in jiffies */
	double seconds = (uptime - value[18]) / (double)tickspersec;
	double cpu_usage = 100 * ((total_time / tickspersec) / seconds);

	/* Find min/max avg CPU usage. */
	if (cpu_usage > cpu_usage_max)
	{
		cpu_usage_max = cpu_usage;
	}

	if (cpu_usage < cpu_usage_min)
	{
		cpu_usage_min = cpu_usage;
	}

	/* Find min/max RSS usage. */
	if (value[20] > rss_usage_max)
	{
		rss_usage_max = value[20];
	}

	if (value[20] < rss_usage_min)
	{
		rss_usage_min = value[20];
	}

	/* Set CPU usage flags. */
	switch (value[36])
	{
		case 0:
			ppstat_tp.cpu0_usage_counter++;
				break;
		case 1:
			ppstat_tp.cpu1_usage_counter++;
				break;
		case 2:
			ppstat_tp.cpu2_usage_counter++;
				break;
		case 3:
			ppstat_tp.cpu3_usage_counter++;
				break;
		default:
				printf ("No active core.\n");	/* We shouldn't get here... */
				break;
	}
	ppstat_tp.cpu_usage_sum += cpu_usage;

/*	Application name. */
	printf ("Application name: %s\n", ppstat_tp.tcomm);
/*	The process ID. */
	printf ("PID: %llu\n", pid);
	printf ("State: %c\n", state);
	printf ("Priority: %llu\n", value[14]);	/* priority */
	printf ("Threads: %llu\n", value[16]);	/* num_threads */
/*	Resident Set Size: number of pages the process has in real memory.
 *	RSS * memory_page_size = memory_usage
 *	memory_usage is memory usage by the test program <prg_name> */
	printf ("RSS: %llu\n", value[20]);
/*	CPU number last executed on.
	Current and last cpu in which it was executed. */
	printf ("CPU (core active): %llu\n\n", value[35]);
/*	Amount of time that this process has been scheduled in user mode. */
	printf ("User time: %llu\n", value[10]);		/* clock ticks */
/*	Amount of time that this process has been scheduled in kernel mode. */
	printf ("System time: %llu\n", value[11]);		/* clock ticks */
	printf ("Total time: %llu\n", total_time);	/* clock ticks */
/*	The time the process started after system boot. */
	printf ("Start time: %llu\n", value[18]);	/* clock ticks */
/*	The total number of seconds the system has been up
	since its last restart. */
	printf ("Up time: %d\n", uptime);			/* seconds */
/*	Avg CPU usage since the application start. */
	printf ("CPU (average usage): %lf\n", cpu_usage);
	printf ("-------------------------------\n\n");

	fclose (f_proc_pid_stat);
	ppstat_tp.cpu_usage_min = cpu_usage_min;
	ppstat_tp.cpu_usage_max = cpu_usage_max;
	ppstat_tp.rss_usage_min = rss_usage_min;
	ppstat_tp.rss_usage_max = rss_usage_max;
	ppstat_tp.succ_flag = 0;
	return ppstat_tp;
}
