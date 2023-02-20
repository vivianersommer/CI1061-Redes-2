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
    struct hostent *RegistroDNS;
    char NomeHost[MAXNOMEHOST];

    signal();

    /* Verifica argumentos */
    if (argc != 2) {
        printf("Uso correto: ./server <porta>\n");
        return 1;
    }

    /* Abre arquivo de log */
    logfile = fopen("serverudp.log","w");
    /* Tenta obter meu proprio endereco IP */
    gethostname(NomeHost, MAXNOMEHOST);
    if ((RegistroDNS = gethostbyname(NomeHost)) == NULL) {
        printf("Nao sei o meu proprio IP!\n");
        return 2;
    }
    EnderecLocal.sin_port = htons(atoi(argv[1]));
    EnderecLocal.sin_family = RegistroDNS->h_addrtype;
    bcopy( (char*)RegistroDNS->h_addr, (char *)&EnderecLocal.sin_addr, RegistroDNS->h_length );

    fprintf(logfile,"Obtido meu proprio endereco IP.\n");
    printf("EnderecLocal.sin_port = %d\n", EnderecLocal.sin_port);
    printf("EnderecLocal.sin_family = %d\n", EnderecLocal.sin_family);
    
    for (int i = 0; i <  RegistroDNS->h_length; i++){
        printf("nderecLocal.sin_addr[%d]= %d\n", i,  RegistroDNS->h_addr[i]);
    }


    /* Tenta abrir um socket! */
    if ((se = socket(RegistroDNS->h_addrtype,SOCK_DGRAM,0)) < 0) {
        printf("Nao abri o socket!\n");
        return 3;
    }
    fprintf(logfile,"Aberto o socket.\n");

    /* Tenta fazer o bind para solicitar o uso da porta para o sistema
     * operacional */
    if (bind(se, (struct sockaddr *)&EnderecLocal, sizeof(EnderecLocal)) < 0) {
        printf("Nao fiz o bind. Porta em uso!\n");
        return 4;
    }
    fprintf(logfile,"Aberto a porta pelo bind.\n");

    /* Le e repassa todos os pacotes */
    while (1) {
        unsigned int tt;
        int x;
        recvfrom(se,buffer,SIZE,0, (struct sockaddr *) &EnderecCliente, &tt);
        sscanf(buffer,"%d",&x);
        fprintf(logfile,"Recebido pacote # %d.\n",x);
        printf("Recebido pacote # %d.\n",x);
        sendto(se, buffer, SIZE, 0, (struct sockaddr *) &EnderecCliente, tt);
        fprintf(logfile,"Enviado pacote # %d.\n",x);
        fflush(logfile);
    }
    fclose(logfile);
    close(se);
    return 0;
}