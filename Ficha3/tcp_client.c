/**********************************************************************
 * CLIENTE liga ao servidor (definido em argv[1]) no porto especificado
 * (em argv[2]), escrevendo a palavra predefinida (em argv[3]).
 * USO: >cliente <enderecoServidor>  <porto>  <Palavra>
 **********************************************************************/
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>

#define BUF_SIZE	1024
void erro(char *msg);

int main(int argc, char *argv[]) {
  char endServer[100];
  int fd;
  struct sockaddr_in addr;
  struct hostent *hostPtr;

  if (argc != 3) {
    printf("./cliente <host> <port>\n");
    exit(-1);
  }

  strcpy(endServer, argv[1]);
  if ((hostPtr = gethostbyname(endServer)) == 0)
    erro("Não consegui obter endereço");

  bzero((void *) &addr, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = ((struct in_addr *)(hostPtr->h_addr))->s_addr;
  addr.sin_port = htons((short) atoi(argv[2]));

  if ((fd = socket(AF_INET,SOCK_STREAM,0)) == -1)
	  erro("socket");
  if (connect(fd,(struct sockaddr *)&addr,sizeof (addr)) < 0)
	  erro("Connect");
  /* Resposta do Servidor */
  char response[BUF_SIZE];
  int n = read(fd, response, BUF_SIZE - 1);
  if (n > 0) {
    response[n] = '\0';
    printf("%s\n", response);
    char buffer[BUF_SIZE];
    while(1){
      // recolhe o requested domain
	  fgets(buffer, BUF_SIZE, stdin);
      buffer[strcspn(buffer, "\n")] = '\0';
      write(fd, buffer, BUF_SIZE);
      // se requested domain for "SAIR", então termina logo.
      if(strcmp(buffer, "SAIR") == 0){
      	break;
      }
      
      // ler o IP obtido
      n = read(fd, response, BUF_SIZE - 1);
      if (n > 0) {
        response[strcspn(response, "\n")] = '\0';
        printf("%s\n", response);
      }
    }
  }
	n = read(fd, response, BUF_SIZE);
	if(n > 0){
    	response[strcspn(response, "\n")] = '\0';
    	printf("%s\n", response);
	}
  close(fd);
  exit(0);
}

void erro(char *msg) {
  printf("Erro: %s\n", msg);
	exit(-1);
}
