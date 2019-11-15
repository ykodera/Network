#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>

int main (void){
  int listenfd, connfd;
  pid_t pid;
  char buf[BUFSIZ];
  int nbytes;
  int   SERVER_PORT = 10000;
  struct sockaddr_in servaddr;
  if ((listenfd = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
    perror ("socket");
    exit(1);
  }
  memset (&servaddr, 0, sizeof (servaddr));
  servaddr.sin_len = sizeof(servaddr);
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(SERVER_PORT);
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
    perror("bind");
    exit(1);
  }
  if (listen(listenfd, 10) < 0) {
    perror("listen");
    exit(1);
  }

  for (;;){
    if ((connfd = accept (listenfd, (struct sockaddr *)NULL, NULL)) < 0) {
      perror("accept"); exit(1);
    }
    fprintf(stderr, "accept\n");
    while ((nbytes = read (connfd, buf, sizeof (buf))) > 0 ){
        fwrite(buf,1, nbytes, stderr);
        write(connfd, buf, nbytes);
      }
    close(connfd);
    fprintf(stderr, "close\n");
    
  }
  
}
