#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUFLEN 512	// Tamanho do buffer
#define PORT 9875	// Porto para recepção das mensagens

void erro(char *s) {
	perror(s);
	exit(1);
}

char hex[20];
char bin[BUFLEN];

char* conv_hex(int num){
	int index = 0;
	int tmp;
	while(num > 0){
		tmp = num % 16;

		if(tmp < 10){
			hex[index] = tmp + '0';
		}
		else{
			hex[index] = tmp + 'A' - 10;
		}
		index++;
		num = num / 16;
	}
	hex[index] = '\0';
	for(int i = 0; i < index / 2; i++){
		char temp = hex[i];
		hex[i] = hex[index - 1 - i];
		hex[index - 1 - i] = temp;
	}
	return hex;
}

char* conv_bin(int num){
	int index = 0;
	int bit_count = 0;
	while(num > 0){
		if(bit_count == 4){
			bin[index++] = ' ';
			bit_count = 0;
		}
		bin[index++] = (num%2) + '0';
		num = num / 2;
		bit_count++;
	}
	if(bit_count != 4){
		while(bit_count < 4){
			bin[index++] = '0';
			bit_count++;
		}
	}
	bin[index] = '\0';
	for(int i = 0; i < index / 2; i++){
		char temp = bin[i];
		bin[i] = bin[index - 1 - i];
		bin[index - 1 - i] = temp;
	}
	return bin;
}

int main(void) {

  	struct sockaddr_in si_server, si_client;

	int s,recv_len;
	socklen_t slen = sizeof(si_client);
	char buf[BUFLEN];

	// Cria um socket para recepção de pacotes UDP
	if((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
		erro("Erro na criação do socket");
	}

  // Preenchimento da socket address structure
	si_server.sin_family = AF_INET;
	si_server.sin_port = htons(PORT);
	si_server.sin_addr.s_addr = htonl(INADDR_ANY);

	// Associa o socket à informação de endereço
	if(bind(s,(struct sockaddr*)&si_server, sizeof(si_server)) == -1) {
		erro("Erro no bind");
	}
	while(1){
		// Espera recepção de mensagem (a chamada é bloqueante)
		if((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_client, (socklen_t *)&slen)) == -1) {
			erro("Erro no recvfrom");
		}
		// Para ignorar o restante conteúdo (anterior do buffer)
		buf[strcspn(buf, "\n")] = '\0';
		if(strcmp(buf, "SAIR") == 0){
			break;
		}
	
		printf("Recebi uma mensagem do sistema com o endereço %s e o porto %d\n", inet_ntoa(si_client.sin_addr), ntohs(si_client.sin_port));
		char response[BUFLEN];
		printf("Buffer recebido: %d\n", atoi(buf));
		sprintf(response, "Número em Binário: %s\nNúmero em hexadecimal: 0x%s\n", conv_bin(atoi(buf)), conv_hex(atoi(buf)));
		printf("Resposta: %s\n", response);
		sendto(s, response, strlen(response), 0, (struct sockaddr *) &si_client, slen);
	}
	// Fecha socket e termina programa
	close(s);
	return 0;
}
