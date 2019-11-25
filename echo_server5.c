#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/wait.h>



void waitall (int sig) {

  int stat;
  pid_t pid;
  while ((pid = waitpid(-1, &stat, WNOHANG)) >= 0) {
    // -1: 任意のプロセス
    // WNOHANG: 終了したプロセスがないときに待たない
    fprintf (stderr, "waitpid: %d %d\n", pid, stat);
  }
}


int main (void){
  int listenfd, connfd;
  pid_t pid;
  char buf[BUFSIZ];
  int nbytes;
  int SERVER_PORT = 10000;
  struct sockaddr_in servaddr;
  if ((listenfd = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
    perror ("socket"); exit(1);
  }
  memset (&servaddr, 0, sizeof (servaddr));
  servaddr.sin_len = sizeof(servaddr);
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(SERVER_PORT);
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  if (bind (listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
    perror("bind"); exit(1);
  }
  if (listen(listenfd, 10) < 0) {
    perror("listen"); exit(1);
  }

  // 子プロセスが終了したら直ぐに全てwaitするための準備
  signal (SIGCHLD, waitall);
  
  for (;;){
    if ((connfd = accept (listenfd, (struct sockaddr *)NULL, NULL)) < 0) {
      perror ("accept"); exit(1);
    }
    fprintf(stderr, "accept\n");
    pid = fork();
    if (pid == -1) {
      perror("fork"); exit(1);
    }
    fprintf (stderr, "fork: %d\n", pid);
    if (pid == 0) { /*child */
      FILE *fp;
      fputs ("child\n", stderr);
      close (listenfd);
      
      // これ以降は標準入出力ライブラリを使用可能
      if ((fp = fdopen (connfd, "r+")) == NULL) {
        perror ("fdopen"); exit(1);        
      }
      setlinebuf (fp); /* 行バッファリングを指定 */

      while (fgets (buf, sizeof (buf), fp) != NULL) {
        fputs ("child: ", stderr);
        fputs (buf, stderr);
        fputs (buf, fp); /* クライアントへ送信 */
      }
      fclose (fp);
      fputs ("child: exit\n", stderr);
      exit(0);
    } else { /* parent */
    close(connfd);
    fprintf(stderr, "parent\n");    
    }
  }
}
