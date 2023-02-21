import socket
import sys

PACKETS = 50000
SIZE = 1024

if len(sys.argv) != 3:
    print("Você esqueceu de passar alguns parâmetros necessários, por favor rode do seguinte modo: ")
    print("./cliente : <nome_do_servidor> <porta>")
    sys.exit(1)

log_cliente = open("logCliente.txt", "w")

endereco_servidor = (socket.gethostbyname(sys.argv[1]), int(sys.argv[2]))

skt = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

log_cliente.write("Aberto um novo socket.\n")

log_cliente.write("Iniciando envio de pacotes...\n")
for i in range(PACKETS):
    dados = f"{i:>{SIZE-1}}"
    skt.sendto(dados.encode(), endereco_servidor)
    log_cliente.write(f"Enviado pacote # {i}.\n")

log_cliente.write("Terminado envio de pacotes.\n")
log_cliente.close()

skt.close()
