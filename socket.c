/*
*	Com a intenção de facilitar o entendimento do smallcode do protótipo do
* projeto, reescrevi o smallcode de forma mais clara com nome de variáveis
* mais claros evidenciando para que elas servem facilitando a compreenção
* da lógica que utilizei.
*      Abraço a todos...
*
* 		By User_X a.k.a Victor Flores.
*/

// -----------------------------------------------------------------------------------------------------------------------

/*
* P.S.: Lembrando que o código é livre para ser alterado e será adicionado à
* licença GPL. Não sou o autor, todos nós seremos, ja que todos iremos alterá-lo
*
*/

//------------------------------------------------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>

// estes headers foram desenolvidos por mim, antes de continuar o programa, acredito dar uma lida e testa-los em outros programas de autoria de vc's ::
// para se familiarizar e também fazer um tipo de debug neles ::

#include "headers/dump_io.h"
#include "headers/shell_fnct.h"
#include "headers/exceptionss.h"

// inicialmente, ele rodará apenas em linux, eu imagino. Então criaremos uma portabilidade para ele....

#define BACKLOG 5
#define TRUE 1
#define buff_lenght 64

int main(int argc, char ** argv){
	int sockfd, new_sockfd;			// descritores para o socket ::
	
	unsigned short int yes = 1;		// variável para settar como TRUE a função setsockopt() para SO_REUSEADDR ::
	int port = 0;					// guardará a porta que remos nos conectar ::
	
	int while_forkid;				// guardará o id do primeiro fork que manipulara a aceitação de conexões ::
	int accept_forkid;				//      "             "      segundo fork que manipulará a conexão em si ::
	int connect_forkid;                           // guardará o id do terceiro fork usado para conectar ao cliente ::
	
	socklen_t sin_size;				// armazenará o tamanho da estrutura host/client do tipo sockaddr_in ::
	
	struct sockaddr_in host;          	// estrutura do servidor ::
	struct sockaddr_in client;        	// estrutura do cliente ::
	
	char buff_send[buff_lenght];
	char buff_recv[buff_lenght];
	
	if(argc < 2 || argc > 2){
		printf("Usage: %s <port>\n", argv[0]);
		exit(1);
	}
	
	port = atoi(argv[1]);
	
	if((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0){		//Criando o socket::
		fatal("creating the socket");						// fatal() presente no header exceptionss.h ::
	}
	
	if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) < 0){	// Settando o socket para reutilizar um endereço mesmo que ja em uso ::
		fatal("on SO_REUSEADDR");
	}
	
	// Configuração do socket ::
	
	host.sin_family = AF_INET; 		// Familia de SOCKET, LER SOBRE PARA MELHOR ENTENDIMENTO ::
	host.sin_port = htons(port);      	// Porta que será usada no socket ::
	host.sin_addr.s_addr = 0;        	// Informando que qualquer addr pode acessar esse servidor ::
	memset(&(host.sin_zero), '\0', 8);	// preenchendo o resto da estrutura com \0 ::
	
	if(bind(sockfd, (struct sockaddr *)&host, sizeof(struct sockaddr_in)) < 0){  // propriamente abrindo a porta e deixando o socket lah ::
		fatal("binding to socket");
	}
	
	if(listen(sockfd, BACKLOG) < 0){
		fatal("Listenning from socket");  // Deixando o socket na escuta ::
	}
	
	sin_size = sizeof(struct sockaddr_in);  // guardando o tamanho da estrutura sockaddr_in, explico ja o por que ::
	while_forkid = fork();
	if(while_forkid){
		while(TRUE){												  // while infinito que para forçar ficar aceitando conexões ::
		
			new_sockfd = accept(sockfd, (struct sockaddr *)&client, &sin_size); // a instrução accept espera como ultimo parametro... ::
															  // o tamanho da estrutura no tipo socklen_t, por isso o uso do sin_size, que ficaria... ::
			if(new_sockfd < 0){										  // muito custoso fazer o cast em cada vez no while, em termos de processador ::					 
				fatal("Accpeting connection on the socket"); 				 
			}else{
				accept_forkid = fork();
				if(accept_forkid){
					printf("\nIt Work's !!!\n");									 // se houver alguma conexão, imprime a msg na tela e envia para o cliente a mesma msg ::
					strcpy(buff_send, "\nIt Works !!!\n");
					str_format(buff_send);
					send(new_sockfd, buff_send, strlen(buff_send) , 0);
					close(new_sockfd);
					return 0;
				}
			}
			
		}
		close(sockfd);
	}
	return 0;
}
