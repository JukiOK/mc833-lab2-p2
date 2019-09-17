#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define MAXLINE 4096

int main(int argc, char **argv) {
   int    sockfd, n;
   char   recvline[MAXLINE + 1];
   char   error[MAXLINE + 1];
   struct sockaddr_in servaddr;
   struct sockaddr_in servaddr_returned;
   socklen_t servaddr_len = sizeof(servaddr_returned);

   if (argc != 3) {
      strcpy(error,"uso: ");
      strcat(error,argv[0]);
      strcat(error," <IPaddress>");
      perror(error);
      exit(1);
   }

   // cria um endpoint para comunicação e retorna um file descriptor (sockfd) que referencia esse socket.
   if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      perror("socket error");
      exit(1);
   }

   bzero(&servaddr, sizeof(servaddr)); // Apaga sizeof(servaddr) bytes de servaddr colocando '\0' em cada um dos bytes da estrutura do servidor que será preechida
   servaddr.sin_family = AF_INET; // Protocolo IPv4
   short port = atoi(argv[2]);
   servaddr.sin_port   = htons(port); // Recebe porta que vai conectar
   if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) { // Converte a string passada como argumento do programa na estrutura de formato IPv4 e preenche essa estrutura no campo sinaddr de servaddr
      perror("inet_pton error");
      exit(1);
   }

   // Conecta o socket referenciado por sockfd no endereco especificado por servaddr
   if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
      perror("connect error");
      exit(1);
   }

   // Código adicionado
   // Atribui o endereco ao qual o sockfd esta associado ao buffer apontado por servaddr_returned de tamanho servaddr_len
   if (getsockname(sockfd, (struct sockaddr *) &servaddr_returned, &servaddr_len) < 0) {
      perror("get socket name error");
      exit(1);
   }

   // inet_ntoa converte o endereço ipv4 da estrutura servaddr_returned para uma string contendo o endereço ipv4 na notação padrão
   printf("Conexão com o servidor estabelecida: %s\n", inet_ntoa(servaddr_returned.sin_addr));
   // ntohs converte a porta em sin_port da ordem de bytes da rede para a ordem padrão do sistema de um short
   printf("Porta: %d\n\n", (int) ntohs(servaddr_returned.sin_port));

   // Fim do código adicionado

   // read vai tentar ouvir até 4096 bytes de sockfd e atribuir isso ao buffer recvline. A função retornará o valor total de bytes lidos
   // Essa leitura acontecerá até n retornar 0 (fim de arquivo) ou um número negativo (erro)
   char command[MAXLINE];
   while (1) {
     fgets (command, MAXLINE, stdin);
     write(sockfd, command, strlen(command));
     if( (n = read(sockfd , recvline , MAXLINE)) > 0 ) {
         recvline[n] = 0;
         printf("Resposta do servidor:\n");
         printf("%s\n", recvline);
     }
   }

   exit(0);
}
