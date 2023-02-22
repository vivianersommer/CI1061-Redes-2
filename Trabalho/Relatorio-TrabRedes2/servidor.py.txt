import socket
import struct
import sys

MAXNOMEHOST = 30
SIZE = 1024
SIZE_MAX = 500001


def selection_sort(num, tam):
    for i in range(tam - 1):
        min = i
        for j in range(i + 1, tam):
            if num[j] > num[min]:
                min = j
        if i != min:
            num[i], num[min] = num[min], num[i]


def main():
    if len(sys.argv) != 2:
        print("Você esqueceu de passar alguns parâmetros necessários, por favor rode do seguinte modo:")
        print("python3 servidor.py <porta>")
        sys.exit(1)

    log_servidor = open("logServidorPython.txt", "w")

    nome_host = socket.gethostname()
    dados_servidor = socket.gethostbyname(nome_host)
    if not dados_servidor:
        print("Nao foi possível obter IP do servidor!\n")
        sys.exit(1)
    log_servidor.write("Obtido meu proprio endereco IP.\n")

    endereco_servidor = (dados_servidor, int(sys.argv[1]))
    skt = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
    skt.bind(endereco_servidor)
    log_servidor.write("Aberto o socket.\n")
    log_servidor.write("Aberto a porta para comunicação.\n")

    tv = struct.pack('ll', 5, 0)
    skt.setsockopt(socket.SOL_SOCKET, socket.SO_RCVTIMEO, tv)

    sequencia = []
    while True:
        dados, endereco_cliente = skt.recvfrom(SIZE)
        if not dados:
            log_servidor.write("Servidor finalizando após 5 segundos sem novas mensagens.\n")
            break
        x = int(dados.decode('utf-8'))
        sequencia.append(x)
        log_servidor.write(f"Recebido pacote # {x}.\n")
    log_servidor.write("Fim dos recebimentos...\n")
    log_servidor.write("----------------------------------------------------------------------------------\n")
    log_servidor.write("Verificando ordem e perdas de mensagens...\n")

    mensagens_recebidas = len(sequencia)
    fora_de_ordem = 0
    for q in range(mensagens_recebidas - 1):
        if sequencia[q] > sequencia[q + 1]:
            fora_de_ordem += 1

    selection_sort(sequencia, mensagens_recebidas)
    perdidos = 0
    final = sequencia[mensagens_recebidas - 1]
    for i in range(final):
        if i not in sequencia:
            perdidos += 1

    log_servidor.write(f"Foram recebidas {mensagens_recebidas} mensagens, perdeu {perdidos} e {fora_de_ordem} fora de ordem.\n")
    log_servidor.write("Finalizada a verificação...\n")

    log_servidor.close()
    skt.close()


if __name__ == "__main__":
    main()
