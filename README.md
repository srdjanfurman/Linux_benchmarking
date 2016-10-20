# Linux benchmarking

## Description
Program displays CPU and memory usage of program <prg_name> using linux /proc/ virtual fs.
http://www.tldp.org/LDP/sag/html/proc-fs.html

**Build:**
```
gcc -Wall test_app.c -o test_app -lcurses
```
**Requirements:**
```
sudo apt-get install libncurses5
```
**Usage:**
```
./test_app <prg_name>
Alternative: top -p <pid>
```

**Example usage:**
```
./test_app
./test_app --pid_io aio_tio
./test_app --pid_sched aio_tio
./test_app --meminfo aio_tio

./test_app --pid_io aio_tio > test_log.log
```

**NOTE:**
```
/proc/ virtual fs explanation:
http://man7.org/linux/man-pages/man5/proc.5.html
```
