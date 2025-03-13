#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUFLEN 512	// Tamanho do buffer

void erro(char *s) {
    perror(s);
    exit(1);
}

int main(int argc, char const *argv[]){
    int recv_len;

    if(argc != 3){
        printf("Uso: ./udp_client <ip> <port>\n");
        return 1;
    }

    struct sockaddr_in si_server;

    int s;
    socklen_t slen = sizeof(si_server);
    char buf[BUFLEN];

    // Cria um socket para recepção de pacotes UDP
    if((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        erro("Erro na criação do socket");
    }

    // Preenchimento da socket address structure do servidor
    si_server.sin_family = AF_INET;
    si_server.sin_port = htons(atoi(argv[2]));
    if(inet_aton(argv[1], &si_server.sin_addr) == 0){
        erro("Erro no inet_aton");
    }

    while(1){
        fgets(buf, BUFLEN, stdin);
        buf[strcspn(buf, "\n")] = '\0';
        // "SAIR" para terminar a ligação
        if(strcmp(buf, "SAIR") == 0){
            // Envia a mensagem para o servidor
            if(sendto(s, buf, strlen(buf), 0, (struct sockaddr *) &si_server, slen) == -1){
                erro("Erro no sendto");
            }
            break;
        }

        // Envia a mensagem para o servidor
        if(sendto(s, buf, strlen(buf), 0, (struct sockaddr *) &si_server, slen) == -1){
            erro("Erro no sendto");
        }

	    if((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_server, (socklen_t *)&slen)) == -1) {
            erro("Erro no recvfrom");
        }

        // Para ignorar o restante conteúdo (anterior do buffer)
        buf[recv_len]='\0';
        printf("%s\n", buf);
    } 
    
    close(s);
    return 0;
}