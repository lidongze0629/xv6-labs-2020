#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// Your goal is to use pipe and fork to set up the pipeline.
// The first process feeds the numbers 2 through 35 into the pipeline.
// For each prime number, you will arrange to create one process that reads from
// its left neighbor over a pipe and writes to its right neighbor over another pipe.
// Since xv6 has limited number of file descriptors and processes, the first process can stop at 35.
//

int fork_primes(void) {
  int pid;

  pid = fork();
  if (pid == -1) {
    fprintf(2, "system call fork error");
    exit(1);
  }
  return pid;
}

int create_pipe(int *p) {
  // do not need to write to parent pipe
  close(p[1]);
  int init_prime_of_pipe = -1;
  int rfln = -1; // read_from_left_neighbor
  
  int child_pipe_exist = 0;
  int _p[2];

  while (read(p[0], &rfln, sizeof(rfln)) > 0) {
    if (init_prime_of_pipe == -1) {
      init_prime_of_pipe = rfln;
      fprintf(1, "prime %d\n", init_prime_of_pipe);
    } else {
      if ((rfln % init_prime_of_pipe) != 0) {
        if (child_pipe_exist) {
          write(_p[1], &rfln, sizeof(rfln));
        } else {
          // create a pipe
          if (pipe(_p) < 0) {
            fprintf(2, "system call pipe error");
            exit(1);
          }
          if (fork_primes() == 0) {
            create_pipe(_p);
          } else {
            close(_p[0]);
            write(_p[1], &rfln, sizeof(rfln));
          }
          child_pipe_exist = 1;
        }
      } // end if
      // else do nothing but drop
    }
  }
  // after read all value from left neighbor (read return 0)
  // wait for child process exit.
  close(_p[1]);
  wait(0);
  return 0;
}

int main(int argc, char *argv[]) {
  int p[2];
  
  if (pipe(p) < 0) {
    fprintf(2, "system call pipe error");
    exit(1);
  }

  if (fork_primes() == 0) {
    // child
    create_pipe(p);  
  } else {
    // parent
    // also the first process
    // close read fd
    close(p[0]);
    // send 2 through 35 to pipeline
    for (int i = 2; i <= 35; ++i) {
      write(p[1], &i, sizeof(i));
    }
    // send over, close write fd
    close(p[1]);
    // wait all child exit
    wait(0);
  }
  exit(0);
}
