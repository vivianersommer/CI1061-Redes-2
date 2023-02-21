import socket
import sys

SIZE = 1024
MAXNOMEHOST = 128
SEQUENCE_MAX = 100000

def selection_sort(arr, n):
    for i in range(n):
        min_idx = i
        for j in range(i+1, n):
            if arr[j] < arr[min_idx]:
                min_idx = j
        arr[i], arr[min_idx] = arr[min_idx], arr[i]

def main():
    skt = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    if len(sys.argv) != 2:
        print("Você esqueceu de passar alguns parâmetros necessários, por favor rode do seguinte modo: ")
        print("./servidor <porta>")
        sys.exit(1)

    enderecoServidor = ('', int(sys.argv[1]))
    skt.bind(enderecoServidor)

    sequencia = [0] * SEQUENCE_MAX
    sequenciaPos = 0
    mensagensRecebidas = 0

    while True:
        dados, enderecoCliente = skt.recvfrom(SIZE)
        if not dados:
            print("Servidor finalizando após 5 segundos sem novas mensagens.")
            break
        x = int(dados.decode('utf-8'))
        sequencia[sequenciaPos] = x
        sequenciaPos += 1
        mensagensRecebidas += 1
        print(f"Recebido pacote #{x}.")

    print("Fim dos recebimentos...")
    print("----------------------------------------------------------------------------------")
    print("Verificando ordem e perdas de mensagens...")

    foraDeOrdem = 0
    print(f"mensagensRecebidas = {mensagensRecebidas}")
    for q in range(mensagensRecebidas - 1):
        if sequencia[q] > sequencia[q+1]:
            foraDeOrdem += 1
            print(f"Achei um fora de ordem, os meliantes: {sequencia[q]} e {sequencia[q+1]}")

    selection_sort(sequencia, mensagensRecebidas)

    q = 0
    perdidos = 0
    while q < mensagensRecebidas:
        if q == sequencia[q]:
            q += 1
        else:
            q = sequencia[q]
            perdidos += 1

    print(f"Foram recebidas {mensagensRecebidas} mensagens, perdeu {perdidos} e {foraDeOrdem} fora de ordem.")
    print("Finalizada a verificação...")

    skt.close()

if __name__ == '__main__':
    main()
