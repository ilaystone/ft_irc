#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#define PORT 6667

int main(int argc, char const *argv[])
{
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) 
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }
    printf("\nConnection GOOD \n");
	while (1)
	{		
		// send(sock, "NICK nickname\r\n", 14, 0);
        // sleep(1);
		// send(sock, "USER username +i * :real name\r\n", 14, 0);
        // sleep(1);
		// send(sock, "PASS 123\r\n", 14, 0);
        // sleep(1);
		send(sock, "JOIN #chan1,#chan2,#chan3,#chan4,#chan5,#chan6\r\n", 14, 0);
        sleep(1);
		send(sock, "JOIN 0\r\n", 14, 0);
        sleep(1);
	} 
    return 0;
}