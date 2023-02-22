#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <time.h>

/* Alunos:
            Viviane da Rosa Sommer   
            Vinícius Oliveira dos Santos */

#define PACKETS 100000
#define SIZE 1024

int main(int argc, char *argv[]) {


    int skt;
    struct hostent *dadosServidor;
    struct sockaddr_in enderecoServidor;
    FILE *logCliente;
    char dados[SIZE];

    // analisa se foram passados os 2 parâmetros ao rodar o programa --------------------------------------------
    if (argc != 3) {
        printf("Você esqueceu de passar alguns parâmetros necessários, por favor rode do seguinte modo: \n");
        printf("./cliente : <nome_do_servidor> <porta> \n");
        exit(1);
    }
    // -----------------------------------------------------------------------------------------------------------


    // cria o arquivo de log do Cliente --------------------------------------------------------------------------
    logCliente = fopen("logCliente.txt","w");
    fprintf(logCliente,"Cliente iniciado.\n");
    // -----------------------------------------------------------------------------------------------------------
    
    // procura o IP do nome do servidor --------------------------------------------------------------------------
    dadosServidor = gethostbyname(argv[1]);
    if (dadosServidor == NULL) {
        printf("Nao foi possível obter o IP do nome do servidor fornecido por parametro!\n");
        exit(1);
    }
    fprintf(logCliente,"Obtido IP do servidor.\n");
    // -----------------------------------------------------------------------------------------------------------

    // alocal em enderecoServidor, as informações necessárias para abrir socket ----------------------------------
    bcopy( (char *)dadosServidor->h_addr, (char *)&enderecoServidor.sin_addr, dadosServidor->h_length );
    enderecoServidor.sin_family = dadosServidor->h_addrtype;
    enderecoServidor.sin_port = htons(atoi(argv[2]));
    // -----------------------------------------------------------------------------------------------------------

    // abertura do socket para o servidor ------------------------------------------------------------------------
    skt = socket(dadosServidor->h_addrtype, SOCK_DGRAM, 0);
    if (skt < 0) {
        printf("Nao foi possível abrir o socket!\n");
        exit(1);
    }
    fprintf(logCliente,"Aberto um novo socket.\n");
    // -----------------------------------------------------------------------------------------------------------

    // envio das mensagens ao servidor ---------------------------------------------------------------------------
    // em cada mensagem, é enviada em seu payload seu número de sequência
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
    // -----------------------------------------------------------------------------------------------------------


    // após finalizar o envio, fecha o arquivo de log e o socket -------------------------------------------------
    fprintf(logCliente,"Terminado envio de pacotes.\n");
    fclose(logCliente);
    close(skt);
    // -----------------------------------------------------------------------------------------------------------

    return 0;
}