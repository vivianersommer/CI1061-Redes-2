#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>

#define TAMFILA 5
#define MAXNOMEHOST 30
#define SIZE 1024

int main(int argc, char *argv[]) {

    FILE *logfile;
    int se;
    char buffer[SIZE];
    struct sockaddr_in EnderecLocal, EnderecCliente;
    struct hostent *dadosServidor;
    char NomeHost[MAXNOMEHOST];

    if (argc != 2) {
        printf("Você esqueceu de passar alguns parâmetros necessários, por favor rode do seguinte modo: \n");
        printf("./servidor <porta>\n");
        exit(1);
    }

    gethostname(NomeHost, MAXNOMEHOST);
    printf("NomeHost = %s\n", NomeHost);
    dadosServidor = gethostbyname(NomeHost);
    if (dadosServidor == NULL) {
        printf("Nao foi possível obter IP do servidor!\n");
        exit(1);
    }

    EnderecLocal.sin_family = AF_INET;
    
    bcopy( (char *)dadosServidor->h_addr, (char *)&EnderecLocal.sin_addr, dadosServidor->h_length );
    for (int i = 0; i < dadosServidor->h_length; i++){
        printf("h->h_addr_list[i] = %d\n",   dadosServidor->h_addr_list[i]);
    }
    EnderecLocal.sin_port = htons(atoi(argv[1]));

    printf("Configurações do enredeco -----------------------------\n");
    printf("EnderecLocal.sin_port = %d\n", EnderecLocal.sin_port);
    printf("EnderecLocal.sin_family = %d\n", EnderecLocal.sin_family);
    printf("EnderecRemoto.sin_addr= %d\n",  EnderecLocal.sin_addr);
    printf("-----------------------------------------------------\n");

    se = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (se < 0) {
        printf("Nao foi possível abrir o socket!\n");
        exit(1);
    }

    if (bind(se, (struct sockaddr *)&EnderecLocal, sizeof(EnderecLocal)) < 0) {
        printf("Nao foi possível executar o bind. Porta em uso!\n");
        return 4;
    }

    while (1) {
        unsigned int tt;
        int x;
        recvfrom(se,buffer,SIZE,0, (struct sockaddr *) &EnderecCliente, &tt);
        sscanf(buffer,"%d",&x);
        printf("Recebido pacote # %d.\n",x);
        // sendto(se, buffer, SIZE, 0, (struct sockaddr *) &EnderecCliente, tt);
    }

    close(se);
    return 0;
}