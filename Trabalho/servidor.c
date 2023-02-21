#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/time.h>

#define MAXNOMEHOST 30
#define SIZE 1024

double timestamp(){ 
  struct timeval tp;
  gettimeofday(&tp, NULL);
  return((double)(tp.tv_usec/1000.0));
}

int main(int argc, char *argv[]) {

    int skt;
    struct hostent *dadosServidor;
    struct sockaddr_in enderecoServidor, enderecoCliente;
    FILE *logServidor;
    char dados[SIZE];
    char nomeHost[MAXNOMEHOST];
    struct timeval tv;

    if (argc != 2) {
        printf("Você esqueceu de passar alguns parâmetros necessários, por favor rode do seguinte modo: \n");
        printf("./servidor <porta>\n");
        exit(1);
    }

    logServidor = fopen("logServidor.txt","w");

    gethostname(nomeHost, MAXNOMEHOST);
    dadosServidor = gethostbyname(nomeHost);
    if (dadosServidor == NULL) {
        printf("Nao foi possível obter IP do servidor!\n");
        exit(1);
    }
    fprintf(logServidor,"Obtido meu proprio endereco IP.\n");

    bcopy( (char *)dadosServidor->h_addr, (char *)&enderecoServidor.sin_addr, dadosServidor->h_length );
    enderecoServidor.sin_port = htons(atoi(argv[1]));
    enderecoServidor.sin_family = dadosServidor->h_addrtype;

    skt = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (skt < 0) {
        printf("Nao foi possível abrir o socket!\n");
        exit(1);
    }
    fprintf(logServidor,"Aberto o socket.\n");

    if (bind(skt, (struct sockaddr *)&enderecoServidor, sizeof(enderecoServidor)) < 0) {
        printf("Nao foi possível executar o bind. Porta em uso!\n");
        exit(1);
    }
    fprintf(logServidor,"Aberto a porta para comunicação.\n");

    tv.tv_sec = 5;
    tv.tv_usec = 0;
    setsockopt(skt,SOL_SOCKET,SO_RCVTIMEO,(char*)&tv,sizeof(struct timeval));

    while (1) {
        unsigned int tt;
        int x;
        int recvlen = recvfrom(skt,dados,SIZE,0, (struct sockaddr *) &enderecoCliente, &tt);
        if (recvlen < 0){
            fprintf(logServidor,"Servidor finalizando após 5 segundos sem novas mensagens.\n");
            break;
        }
        sscanf(dados,"%d",&x);
        fprintf(logServidor,"Recebido pacote # %d.\n",x);
    }

    fclose(logServidor);
    close(skt);
    return 0;
}