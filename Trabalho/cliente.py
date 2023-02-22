import socket
import sys 

PACKETS = 100000
SIZE = 1024

if len(sys.argv) != 3:
    print("Você esqueceu de passar alguns parâmetros necessários, por favor rode do seguinte modo: ")
    print("./cliente : <nome_do_servidor> <porta> ")
    exit(1)

log_cliente = open("logClientePython.txt", "w")

dados_servidor = socket.gethostbyname(sys.argv[1])
if dados_servidor == None:
    print("Nao foi possível obter o IP do nome do servidor fornecido por parametro!")
    exit(1)
log_cliente.write("Obtido IP do servidor.\n")

endereco_servidor = (dados_servidor, int(sys.argv[2]))

skt = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
log_cliente.write("Aberto um novo socket.\n")

log_cliente.write("Iniciando envio de pacotes...\n")
for i in range(PACKETS):
    dados = str(i).zfill(SIZE-1)
    if skt.sendto(dados.encode(), endereco_servidor) != SIZE:
        print("Nao foi possivel transmitir para o servidor!")
        exit(1)
    log_cliente.write(f"Enviado pacote #{i}.\n")
log_cliente.write("Terminado envio de pacotes.\n")
log_cliente.close()

skt.close()
