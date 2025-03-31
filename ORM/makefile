# To "make"       run in terminal: "make client" or "make server"
# To "make clean" run in terminal: "make client_clean" or "make server_clean"


CLIENT_C = client.c
PROXY_C = proxy.c

client:$(CLIENT_C)
	make -f makefile.client
client_clean:
	make clean -f makefile.client
proxy:$(PROXY_C)
	make -f makefile.proxy
proxy_clean:
	make clean -f makefile.proxy
