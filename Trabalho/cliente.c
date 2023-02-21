#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#include <pthread.h>

#define PACKETS 10
#define SIZE 1024

int sd;
struct hostent *h;
struct sockaddr_in remoteServAddr;

void enviarMensagem() {

    char dados[SIZE];
    for (int i=0; i<PACKETS; i++) {
        sprintf(dados,"%*d",SIZE-1,i);
        dados[SIZE-1]='\0';
        if (sendto(sd,dados,SIZE,0,(struct sockaddr *) &remoteServAddr, sizeof(remoteServAddr)) != SIZE) {
            printf("Nao foi possivel transmitir para o servidor!\n");
            exit(1);
        }
        printf("Enviado pacote # %d.\n",i);
    }

    sprintf(dados,"%*cX",SIZE-2,' ');
    dados[SIZE-1] = '\0';
    if (sendto(sd,dados,SIZE,0,(struct sockaddr *) &remoteServAddr, sizeof(remoteServAddr)) != SIZE) {
        printf("Nao foi possivel transmitir para o servidor!\n");
        exit(1);
    }
    printf("Fim do envio\n");
}


int main(int argc, char *argv[]) {

    if (argc != 3) {
        printf("Você esqueceu de passar alguns parâmetros necessários, por favor rode do seguinte modo: \n");
        printf("./cliente : <nome_do_servidor> <porta> \n");
        exit(1);
    }

    h = gethostbyname(argv[1]);
    if (h == NULL) {
        printf("Nao foi possível obter o IP do nome do servidor fornecido por parametro!\n");
        exit(1);
    }

    remoteServAddr.sin_family = AF_INET;

    bcopy( (char *)h->h_addr, (char *)&remoteServAddr.sin_addr, h->h_length );
    for (int i = 0; i < h->h_length; i++){
        printf("h->h_addr_list[i] = %d\n",   h->h_addr_list[i]);
    }
    remoteServAddr.sin_port = htons(atoi(argv[2]));

    printf("Configurações do enredeco -----------------------------\n");
    printf("EnderecRemoto.sin_port = %d\n", remoteServAddr.sin_port);
    printf("EnderecRemoto.sin_family = %d\n", remoteServAddr.sin_family);
    printf("EnderecRemoto.sin_addr= %d\n",  remoteServAddr.sin_addr.s_addr);
    printf("-----------------------------------------------------\n");

    sd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sd < 0) {
        printf("Nao foi possível abrir o socket!\n");
        exit(1);
    }

    enviarMensagem();

    close(sd);
    return 0;
}