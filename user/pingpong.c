#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// program that uses UNIX system calls to ''ping-pong'' a byte between two processes over a pair of pipes,
// one for each direction. The parent should send a byte to the child;
// the child should print "<pid>: received ping", where <pid> is its process ID, 
// write the byte on the pipe to the parent, and exit; 
// the parent should read the byte from the child, 
// print "<pid>: received pong", and exit. Your solution should be in the file user/pingpong.c.
//


int main(int argc, char *argv[]) {
  int p[2];
  char buf[5];

  if(pipe(p) < 0) {
    fprintf(2, "system call pipe error");
  }

  if(fork() == 0) {
    // child
    // read ping from parent
    read(p[0], buf, sizeof(buf));
    fprintf(1, "%d: received %s", getpid(), buf);
    close(p[0]);
    // write pong to parent
    write(p[1], "pong\n", 5);
    close(p[1]);
  } else {
    // parent
    // send ping to child
    write(p[1], "ping\n", 5);
    close(p[1]);
    // wait child process exit
    wait(0);
    // read pong from child
    read(p[0], buf, sizeof(buf));
    fprintf(1,  "%d: received %s", getpid(), buf);
    close(p[0]);
  }
  exit(0);
}
