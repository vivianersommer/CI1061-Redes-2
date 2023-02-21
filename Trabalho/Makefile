
all: cliente servidor

clean:
	@echo "Removendo .log e executáveis - Inicio ...."
	rm -f cliente servidor *.log
	@echo "Removendo .log e executáveis - Finalizado ...."
	@echo "Matando porta - Inicio ...."
	@fuser -k 8087/udp || true
	@echo "Matando porta - Finalizado ...."
