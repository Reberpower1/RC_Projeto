/*******************************************************************************
 * SERVIDOR no porto 9000, à escuta de novos clientes.  Quando surgem           *
 * novos clientes os dados por eles enviados são lidos e descarregados no ecra. *
 *******************************************************************************/
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <arpa/inet.h>

#define SERVER_PORT 9000
#define BUF_SIZE 1024

char *filename = "domains.txt";
char *ip;
int client_count = 0;

void process_client(int fd);
void erro(char *msg);

char* read_file(const char *filename, char *requested_domain) {
  FILE *file = fopen(filename, "r");
  if(!file){
    perror("Erro ao abrir o ficheiro\n");
    exit(1);
  }
  char domain[BUF_SIZE];
  char ip[BUF_SIZE];
  while(fscanf(file, "%s %s\n", domain, ip) == 2){
    if(strcmp(requested_domain, domain) == 0){
      fclose(file);
      return strdup(ip);
    }
  }
  fclose(file);
  return NULL;
}

int main() {
  int fd, client;
  struct sockaddr_in addr, client_addr;
  int client_addr_size;

  bzero((void *) &addr, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(SERVER_PORT);

  if ( (fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	erro("na funcao socket");
  if ( bind(fd,(struct sockaddr*)&addr,sizeof(addr)) < 0)
	erro("na funcao bind");
  if( listen(fd, 5) < 0)
	erro("na funcao listen");
  client_addr_size = sizeof(client_addr);
  while (1) {
    //clean finished child processes, avoiding zombies
    //must use WNOHANG or would block whenever a child process was working
    while(waitpid(-1,NULL,WNOHANG)>0);
    //wait for new connection
    client = accept(fd,(struct sockaddr *)&client_addr,(socklen_t *)&client_addr_size);
    if (client > 0) {
      client_count++;
      if (fork() == 0) {
        close(fd);
        process_client(client);
        exit(0);
      }
      close(client);
    }
  }
  return 0;
}


void process_client(int fd) {
  char buffer[BUF_SIZE] = "Bem-vindo ao servidor de nomes do DEI. Indique o nome do domínio.\n";
  write(fd, buffer, strlen(buffer));
  char requested_domain[BUF_SIZE];
  while(1){
    int n = read(fd, requested_domain, BUF_SIZE - 1);
    requested_domain[strcspn(requested_domain, "\n")] = '\0';
    if (n > 1) {
      if(strcmp(requested_domain, "SAIR") == 0){
      	break;
      }
    
      char *ip = read_file(filename, requested_domain);
      if(ip != NULL){
          snprintf(buffer, BUF_SIZE*3, "O nome de domínio %s tem associado o endereço IP %s\n", requested_domain, ip);
      }
      else{
        snprintf(buffer, BUF_SIZE*2, "O nome de domínio %s não tem um IP associado.\n", requested_domain);
      }
      write(fd, buffer, strlen(buffer));
    }
  }
  snprintf(buffer, BUF_SIZE, "Até Logo!\n");
  write(fd, buffer, BUF_SIZE);
  close(fd);
}

void erro(char *msg){
	printf("Erro: %s\n", msg);
	exit(-1);
}
