#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"
#include "kernel/fs.h"

// Write a simple version of the UNIX xargs program: 
// read lines from the standard input and run a command for each line,
// upplying the line as arguments to the command.
// Your solution should be in the file user/xargs.c.
//
// Some hints:
//
// Use fork and exec to invoke the command on each line of input. Use wait in the parent to wait for the child to complete the command.
// To read individual lines of input, read a character at a time until a newline ('\n') appears.
// kernel/param.h declares MAXARG, which may be useful if you need to declare an argv array.
// Add the program to UPROGS in Makefile.
// Changes to the file system persist across runs of qemu; to get a clean file system run make clean and then make qemu.

#define STDIN 0


int main(int argc, char** argv) {
  char buf[512];
  char* new_argv[MAXARG];
  char** p = new_argv;
  int n;

  // defaults to "echo command"
  if (argc < 2) {
    *p++ = "echo";
  } else {
    *p++ = argv[1];
  }

  // get argv from comand line
  for (int i = 2; i < argc; ++i) {
    *p++ = argv[i];
  }

  while ((n = read(0, buf, sizeof(buf))) > 0) {
    char **p_copy = p;
    if (fork() == 0) {
      // child exec process
      char split_buf[512];
      char *sbp_begin = split_buf;
      char *sbp = split_buf;
      for (int i = 0; i < n; ++i) {
        if (buf[i] == '\n' || buf[i] == ' ') {
          *sbp++ = 0;
          *p_copy++ = sbp_begin;
          sbp_begin = sbp;
        } else {
          *sbp++ = buf[i];
        }
      }
      exec(new_argv[0], new_argv);
    }
    // parents process wait for child
    wait(0);
  }
  exit(0);
}
