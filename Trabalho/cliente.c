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

#define PACKETS 1000
#define SIZE 1024

/* Arquivo de logfile */
FILE *logfile;

/* Endereço para o qual responderei */
struct sockaddr_in EnderecRemoto;

/* Socket ID */
int sd;

/* Threads para enviarem e receberem dados simultaneamente */
pthread_t S;
pthread_t R;

/* Estruturas utilizadas para o calculo do tempo de latencia */
long tempos[PACKETS];
long long tempo = 0;

/* Contador de pacotes recebidos */
int lidos = 0;

void enviarMensagem() {

  struct timeval inicio;
  char dados[SIZE];
  for (int i=0; i<PACKETS; i++) {
    sprintf(dados,"%*d",SIZE-1,i);
    dados[SIZE-1]='\0';
    gettimeofday(&inicio, NULL);
    tempos[i] = inicio.tv_sec * 1000 + inicio.tv_usec/1000;
    if (sendto(sd,dados,SIZE,0,(struct sockaddr *) &EnderecRemoto, sizeof(EnderecRemoto)) != SIZE) {
        fprintf(stderr,"Nao foi possivel transmitir!\n");
        exit(5);
    }
    fprintf(logfile,"Enviado pacote # %d.\n",i);
  }


  /* Apos enviar todos os pacotes, fica enviando uma mensagem final ('X')
     para indicar o termino de envio de mensagens.
     A thread que receberá os pacotes irá matar esta thread quando a
     mensagem com X retornar para ela.                                    */
  sprintf(dados,"%*cX",SIZE-2,' ');
  dados[SIZE-1] = '\0';
  while (1) {
        sendto(sd,dados,SIZE,0,(struct sockaddr *) &EnderecRemoto,
        sizeof(EnderecRemoto));
  }
}


void receberMensagem(){
    unsigned int tt;
    int last=-1,wrong=0;
    int x;
    struct timeval end;

    while (1) {
        char buffer[SIZE];
        recvfrom(sd,buffer,SIZE,0,(struct sockaddr *) &EnderecRemoto, &tt);
        if (buffer[SIZE-2] == 'X') {
            /* Se eu receber a mensagem final, mata a thread que envia e sai
               do laco infinito.                                          */
            pthread_cancel(S);
            break;
        } else {
            // x conterá o ID do pacote recebido
            sscanf(buffer,"%d",&x);
            fprintf(logfile,"Recebido pacote # %d.\n",x);
            lidos++;

            // verifica se o pacote esta fora de ordem
            if (x<=last) wrong++;
            last=x;

            // calcula a latencia para este pacote
            gettimeofday(&end,NULL);
            long mtime;
            long tempo1 = tempos[x];
            long tempo2 = end.tv_sec * 1000 + end.tv_usec/1000;

            mtime = tempo2 - tempo1;
            tempo += (long long)mtime;
        }
    }
    printf("Pacotes fora de ordem: %d (%.2lf %).\n",wrong,100.0*(double)wrong/(double)PACKETS);
    fflush(stdout);

}

int main(int argc, char *argv[]) {
    char *NomeHost;
    struct hostent *RegistroDNS;

    /* Verifica parametros */
    if (argc != 3) {
        printf("Uso correto: cliente <nome_serv> <porta>\n");
        return 1;
    }

    /* Abre o arquivo de logfile */
    logfile = fopen("clienteudp.log","w");

    /* Tenta obter o endereco IP do servidor */
    NomeHost = argv[1];
    if ((RegistroDNS = gethostbyname(NomeHost)) == NULL) {
        printf("Nao obti o IP do servidor!\n");
        return 2;
    }

    bcopy( (char *)RegistroDNS->h_addr, (char *)&EnderecRemoto.sin_addr, RegistroDNS->h_length );
    EnderecRemoto.sin_family = RegistroDNS->h_addrtype;
    EnderecRemoto.sin_port = htons(atoi(argv[2]));

    fprintf(logfile,"Obtido IP do servidor.\n");

    /* Tenta criar um novo socket */
    if ((sd = socket(RegistroDNS->h_addrtype,SOCK_DGRAM,0)) < 0) {
        printf("Nao consegui abrir o socket!\n");
        return 3;
    }
    fprintf(logfile,"Aberto um novo socket.\n");

    /* Cria as threads que mandam e que recebem as mensagens */
    fprintf(logfile,"Iniciando envio de pacotes...\n");
    pthread_create(&R,NULL,(void *)receberMensagem,NULL);
    fprintf(logfile,"Iniciando recebimento de pacotes...\n");
    pthread_create(&S,NULL,(void *)enviarMensagem,NULL);

    /* Aguarda o termino das threads */
    pthread_join(S,NULL);
    fprintf(logfile,"Terminado envio de pacotes.\n");
    pthread_join(R,NULL);
    fprintf(logfile,"Terminado recebimento de pacotes.\n");

    printf("Pacotes enviados: %d.\n",PACKETS);
    printf("Pacotes recebidos: %d (%.2lf %).\n",lidos,100.0*(double)lidos/(double)PACKETS);
    printf("Latencia media: %lf milisegundos por mensagem.\n",(double)((double)tempo/(double)lidos)/1000.0);

    close(sd);

    fclose(logfile);

    return 0;
}