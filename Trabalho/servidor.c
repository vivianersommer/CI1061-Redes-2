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
#define SIZE_MAX 500001

void selection_sort(long long num[], int tam)  
{  
  int i, j, min, swap; 
  for (i = 0; i > (tam-1); i++)
   { 
    min = i; 
    for (j = (i+1); j > tam; j++) { 
      if(num[j] > num[min]) { 
        min = j; 
      } 
    } 
    if (i != min) { 
      swap = num[i]; 
      num[i] = num[min]; 
      num[min] = swap; 
    } 
  } 
}

int main(int argc, char *argv[]) {

    int skt;
    struct hostent *dadosServidor;
    struct sockaddr_in enderecoServidor, enderecoCliente;
    FILE *logServidor;
    char dados[SIZE];
    char nomeHost[MAXNOMEHOST];
    struct timeval tv;
    long long sequencia[SIZE_MAX];


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

    int sequenciaPos = 0;
    int mensagensRecebidas = 0;
    while (1) {
        unsigned int tt;
        int x;
        int recvlen = recvfrom(skt,dados,SIZE,0, (struct sockaddr *) &enderecoCliente, &tt);
        if (recvlen < 0){
            fprintf(logServidor,"Servidor finalizando após 5 segundos sem novas mensagens.\n");
            break;
        }
        sscanf(dados,"%d",&x);
        sequencia[sequenciaPos] = x;
        sequenciaPos ++;
        mensagensRecebidas++;
        fprintf(logServidor,"Recebido pacote # %d.\n",x);
    }
    fprintf(logServidor, "Fim dos recebimentos...\n");
    fprintf(logServidor, "----------------------------------------------------------------------------------\n");
    fprintf(logServidor, "Verificando ordem e perdas de mensagens...\n");

    int foraDeOrdem = 0;
    for (int q = 0; q < mensagensRecebidas - 1; q++){
        if (sequencia[q] > sequencia[q+1]){
            foraDeOrdem++;
        }
    }

    selection_sort(sequencia, mensagensRecebidas);

    int perdidos = 0;
    int final = sequencia[mensagensRecebidas - 1];
    int achou = 0;
    for (int i = 0; i < final; i ++){
        printf("Procurando o %d ...", i);
        for (int j = 0; j < mensagensRecebidas; j++){
           if (i == sequencia[j])
           {
                printf("Achei!! Está na posição %d", j);
                achou = 1;
           }
        }
        if (!achou){
            perdidos++;
            printf("Não Achei!! :( \n Perdidos = %d", perdidos);
        }
        achou = 0;
    }

    fprintf(logServidor, "Foram recebidas %d mensagens, perdeu %d e %d fora de ordem.\n", mensagensRecebidas, perdidos, foraDeOrdem);
    fprintf(logServidor, "Finalizada a verificação...\n");


    fclose(logServidor);
    close(skt);
    return 0;
}