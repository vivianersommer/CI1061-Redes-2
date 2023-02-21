#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#define PACKETS 100000
#define SIZE 1024

int main(int argc, char *argv[]) {

    int skt;
    struct hostent *dadosServidor;
    struct sockaddr_in enderecoServidor;
    FILE *logCliente;
    char dados[SIZE];

    if (argc != 3) {
        printf("Você esqueceu de passar alguns parâmetros necessários, por favor rode do seguinte modo: \n");
        printf("./cliente : <nome_do_servidor> <porta> \n");
        exit(1);
    }

    logCliente = fopen("logCliente.txt","w");
    
    dadosServidor = gethostbyname(argv[1]);
    if (dadosServidor == NULL) {
        printf("Nao foi possível obter o IP do nome do servidor fornecido por parametro!\n");
        exit(1);
    }
    fprintf(logCliente,"Obtido IP do servidor.\n");

    bcopy( (char *)dadosServidor->h_addr, (char *)&enderecoServidor.sin_addr, dadosServidor->h_length );
    enderecoServidor.sin_family = dadosServidor->h_addrtype;
    enderecoServidor.sin_port = htons(atoi(argv[2]));

    skt = socket(dadosServidor->h_addrtype, SOCK_DGRAM, 0);
    if (skt < 0) {
        printf("Nao foi possível abrir o socket!\n");
        exit(1);
    }
    fprintf(logCliente,"Aberto um novo socket.\n");

    fprintf(logCliente,"Iniciando envio de pacotes...\n");
    for (int i=0; i<PACKETS; i++) {
        sprintf(dados,"%*d",SIZE-1,i);
        dados[SIZE-1]='\0';
        if (sendto(skt,dados,SIZE,0,(struct sockaddr *) &enderecoServidor, sizeof(enderecoServidor)) != SIZE) {
            printf("Nao foi possivel transmitir para o servidor!\n");
            exit(1);
        }
        fprintf(logCliente,"Enviado pacote # %d.\n",i);
    }
    fprintf(logCliente,"Terminado envio de pacotes.\n");
    fclose(logCliente);

    close(skt);
    return 0;
}