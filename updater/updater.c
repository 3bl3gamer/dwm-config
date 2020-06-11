#define _DEFAULT_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <X11/Xlib.h>

static const char *BAT_CAPACITY = "/sys/class/power_supply/BAT0/capacity";
static const char *BAT_POWER = "/sys/class/power_supply/BAT0/power_now";
static const char *CPU_TEMP;
static const char *CPU_TEMPS[] = {"/sys/devices/platform/coretemp.0/hwmon/hwmon2/temp1_input",
                                  "/sys/devices/platform/coretemp.0/hwmon/hwmon3/temp1_input",
                                  "/sys/devices/platform/coretemp.0/hwmon/hwmon4/temp1_input",
                                  "/sys/devices/platform/coretemp.0/hwmon/hwmon5/temp1_input",
                                  "/sys/devices/platform/coretemp.0/hwmon/hwmon6/temp1_input",};
#define NUM_CPUS 4

int readi(const char *path) {
	int res;
	FILE *fd = fopen(path, "r");
	if (!fd) return 0;
	fscanf(fd, "%d", &res);
	fclose(fd);
	return res;
}

void find_cpu_temp() {
	int n = sizeof(CPU_TEMPS) / sizeof(CPU_TEMPS[0]);
	for (int i=0; i<n; i++) {
		CPU_TEMP = CPU_TEMPS[i];
		if (access(CPU_TEMP, F_OK) != -1)
			break;
	}
}

int main() {
	int count = 0;
	char full_buf[520]; //достаточно и 128, но make показывает предупреждение
	
	char bat_chart[32];
	int power_sum = 0;
	
	char mem_chart[64];
	
	char cpu_chart[64];
	int prev_total[NUM_CPUS], prev_idle[NUM_CPUS];
	memset(prev_total, 0, sizeof(prev_total));
	memset(prev_idle, 0, sizeof(prev_idle));
	
	time_t cur_time;
	struct tm* tm_info;
	char time_buf[16];
	
	find_cpu_temp();
	
	Display *dpy;
	if (!(dpy = XOpenDisplay(NULL))) {
		fprintf(stderr, "XOpenDisplay failed.\n");
		exit(1);
	}
	
	for (;;) {
		int capacity = readi(BAT_CAPACITY);
		int power = readi(BAT_POWER) / 1000;
		int temp = readi(CPU_TEMP) / 1000;
		
		// filling battery chart data (0,5000:10000)
		bat_chart[0] = '\0';
		if (count % 5 == 0) {
			power_sum /= 5;
			if (power_sum < 10000) {
				sprintf(bat_chart, "0,%d:10000", power_sum);
			} else {
				sprintf(bat_chart, "%d,10000:10000", power_sum/5);
			}
			power_sum = 0;
		}
		power_sum += power;
		
		// filling memory chart data (1024,16,512:2048)
		mem_chart[0] = '\0';
		if (count % 5 == 0) {
			int total=0, free=0, buffers=0, cached=0;
			FILE *fd = fopen("/proc/meminfo", "r");
			fscanf(fd, "MemTotal: %d kB\n", &total);
			fscanf(fd, "MemFree: %d kB\n", &free);
			fscanf(fd, "%*[^\n]\n");
			fscanf(fd, "Buffers: %d kB\n", &buffers);
			fscanf(fd, "Cached: %d kB\n", &cached);
			fclose(fd);
			
			sprintf(mem_chart, "%d,%d,%d:%d",
				total-free-buffers-cached,
				buffers, cached, total);
		}
		
		// filling CPU chart data (100,50,25,10:400)
		cpu_chart[0] = '\0';
		if (count % 5 == 0) {
			int str_offset = 0;
			FILE *fd = fopen("/proc/stat", "r");
			fscanf(fd, "%*[^\n]\n"); // skipping first line with whole CPU info
			for (int i=0; i<NUM_CPUS; i++) {
				int total, user, nice, system, idle=-1, iowait, irq, softirq;
				fscanf(fd, "%*s %d %d %d %d %d %d %d%*[^\n]\n",
					&user, &nice, &system, &idle, &iowait, &irq, &softirq);
				total = user + nice + system + idle + iowait + irq + softirq;
				
				int percent = 100 - (idle - prev_idle[i])*100 / (total - prev_total[i]);
				str_offset += sprintf(cpu_chart + str_offset, "%d,", percent);
				
				prev_total[i] = total;
				prev_idle[i] = idle;
			}
			fclose(fd);
			
			cpu_chart[str_offset-1] = ':';
			sprintf(cpu_chart + str_offset, "%d", NUM_CPUS * 100);
		}
		
		// formating time
		time(&cur_time);
		tm_info = localtime(&cur_time);
		strftime(time_buf, sizeof(time_buf), "%H:%M:%S", tm_info);
		
		sprintf(full_buf, "%d%% %.1fW{%s}{%s}{%s}%d°C %s",
			capacity, power/1000.f, bat_chart, mem_chart, cpu_chart, temp, time_buf);
		
		//printf("%s\n", full_buf);
		XStoreName(dpy, DefaultRootWindow(dpy), full_buf);
		XSync(dpy, False);
		
		// synchronizing with actual seconds
		struct timeval tp;
		gettimeofday(&tp, NULL);
		usleep(1000000 + 10000 - tp.tv_usec);
		
		count++;
	}
	return 0;
}
