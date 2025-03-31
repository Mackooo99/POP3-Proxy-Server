// Client

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <regex.h>

#define DEFAULT_BUFLEN 1024
#define PROXY_IP "127.0.0.1"
#define PROXY_PORT 27015



void error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}


// Function to validate email format
int isValidEmail(const char *email) {
    regex_t regex;
    const char *pattern = "^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$";
    if (regcomp(&regex, pattern, REG_EXTENDED) != 0)
        return 0;
    int result = regexec(&regex, email, 0, NULL, 0);
    regfree(&regex);
    return result == 0;
}


int main(){

	int sock;
	struct sockaddr_in proxy;
	char buffer[DEFAULT_BUFLEN];
	int n;
	 

	// create a socket
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0)
		error("ERROR opening socket");

	// setup server address structure
	proxy.sin_family = AF_INET;
	proxy.sin_port = htons(PROXY_PORT);  //proxy port
	proxy.sin_addr.s_addr = inet_addr(PROXY_IP);  // proxy IP address
	
	
	// connect to the server
	if (connect(sock, (struct sockaddr*)&proxy, sizeof(proxy)) < 0)
        	error("ERROR connecting");
        	
		
	// read welcome message from server
	memset(buffer, 0, DEFAULT_BUFLEN);
	if ((n = recv(sock, buffer, DEFAULT_BUFLEN, 0)) < 0)
		error("ERROR reading from socket");
	printf("Server message: %s\n", buffer); 
	
	
	// send email address

	char email[DEFAULT_BUFLEN];

	do{
		printf("Enter your email: ");
		scanf("%s", email);
		if (!isValidEmail(email)) 
			printf("Invalid email format. Please try again.\n");

	} while (!isValidEmail(email));

	strcat(email, "\r\n");

	if ((n = send(sock, email, strlen(email), 0)) < 0)
		error("ERROR writing to socket");   
		
		
		
	// read response from server
	memset(buffer, 0, DEFAULT_BUFLEN);
	if ((n = recv(sock, buffer, DEFAULT_BUFLEN, 0))< 0)
		error("ERROR reading from socket");
	printf("Server response: %s\n", buffer);

	// Check if user is blacklisted
    if (strstr(buffer, "black list") != NULL) {
        close(sock);
        return 0;
    }

	// Display menu and get user command
	while (1) {
		printf("Commands:\n");
		printf("1. USER <username>\n");
		printf("2. PASS <password>\n");
		printf("3. STAT\n");
		printf("4. QUIT\n");
		printf("Enter command (e.g., USER john): ");

		char userCommand[DEFAULT_BUFLEN];
		scanf(" %[^\n]", userCommand);  // Read the whole line including spaces
		strcat(userCommand, "\r\n"); // Ensure correct POP3 format


		// Send the user command to the server
		if ((n = send(sock, userCommand, strlen(userCommand), 0)) < 0)
		    error("ERROR writing to socket");

		// Read and display the response from the server
		memset(buffer, 0, DEFAULT_BUFLEN);
		if ((n = recv(sock, buffer, DEFAULT_BUFLEN, 0)) <= 0)
		{
		    printf("Connection closed by server.\n");
			break;
		}
		printf("Server response: %s\n", buffer);

		// Break the loop if the user enters "QUIT"
		if (strncmp(userCommand, "QUIT", 4) == 0) {
		    break;
		}
		
		if(strcmp(buffer, "-ERR Invalid username\r\nGoodbye!\n") == 0 || strcmp(buffer, "-ERR Invalid password\r\nGoodbye!\n") == 0)
			break;
	}


	close(sock);

	return 0;
}
