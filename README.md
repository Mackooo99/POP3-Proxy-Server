# POP3-Proxy-Server

POP3 Proxy Server is an application that intermediates communication between an email client and an email server. It listens for connection requests on a predefined TCP port and forwards them to the POP3 server on port 110. Each communication direction is handled in a separate thread.

Features:

    Forwards requests from the client to the server and responses from the server to the client

    Supports user blocking based on the email address provided in the USER command

    Tested with an email client and a POP3 server
    
    
    
Build and run
	
Client:
	make client_clean
	make client
	
Proxy:
	make proxy_clean
	make proxy
	
RUN:
	./client
	./proxy
