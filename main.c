#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define BUF_MAX 4096


static char *usage = \
  " [PID]\n"
  "\tFollows the lineage of parent processes up until a process beginning with '-' is found i.e a login shell or until pid 1 is found\n"
  "\tIf no pid is supplied, start with the pid of this process"
  "\treturns 0 when a login shell is found, 1 on failure";

void print_usage (char *proc_name) {
  printf("%s: %s\n", proc_name, usage);
}

typedef struct {
  pid_t pid;
  char cmd_head;
} pid_info_t;

pid_info_t get_ppid(const pid_t pid) {
  static char buffer[BUF_MAX];
  FILE *proc_file, *cmd_file;
  pid_info_t ret;
  
  sprintf(buffer, "/proc/%d/stat", pid);
  if ( (proc_file = fopen(buffer, "r") )) {
      if ( fread(buffer, 1, BUF_MAX, proc_file) > 0) {
	strtok(buffer, " "); //skip pid
	strtok(NULL, " "); //skip comm
	strtok(NULL, " "); // skip state
	
	ret.pid = atoi(strtok(NULL, " ")); //get ppid

	sprintf(buffer, "/proc/%d/cmdline", pid);
	if (! (cmd_file = fopen(buffer, "r")) ) {
	  ret.pid = 0;
	  return ret;
	}
	if ( fread(buffer, 1, BUF_MAX, cmd_file) <= 0 ) {
	  ret.pid = 0;
	} else {
	  ret.cmd_head = buffer[0];
	}
	fclose(cmd_file);
      } else {
	ret.pid = 0;
      }
  } else {
      ret.pid = 0;
      return ret;
  }
  fclose(proc_file);
  return ret;
}


int main (int argc, char **argv) {
  pid_t pid;
  pid_info_t r;

  if (argc < 2) {
    pid = getpid();
  } else {
    pid = atoi(argv[1]);
    if (pid == 0) {
      fprintf(stderr, "Supplied pid is either not a number or zero\n");
      exit(-1);
    }
  }
  
  r = get_ppid(pid);
  if (r.pid == 0) {
    fprintf(stderr, "Failed to get the first parent process\n");
    exit(-1);
  }

  while (r.pid != 0 && r.pid != 1 && r.cmd_head != '-') {
    pid = r.pid;
    r = get_ppid(r.pid);
  }
  
  if (r.pid == 0) {
    exit(-1);
  }

  printf("%d", pid);
  
  return 0;
}
