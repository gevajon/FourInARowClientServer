//============================================================================
// Name        : Server.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <ifaddrs.h>

using namespace std;

char place[6][7]; // 6 rows 7 columns board
char msgTransfer[1024] = { '\0' }; // length of a message that will transfer between server and clients
int win = 0;

// function: prints the game board
void display()
{
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 7; j++) {
			cout << place[i][j] << " ";
		}
		cout << endl;
	}
}

// function: checks win situation (row,column,diagonal,reversed diagonal)
int checkWin(char color) {
	//check for row wins
	for (int k = 0; k < 6; k++)
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++) {
				if (place[k][i + j] != color) {
					j = 3;
				} else if (j == 3) {
					win = 1;
					return 1;
				}
			}

	//check for column wins
	for (int k = 0; k < 7; k++)
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 4; j++) {
				if (place[i + j][k] != color)
					j = 3;
				else if (j == 3) {
					win = 1;
					return 1;
				}
			}

	//check for diagonal \ wins
	for (int k = 0; k < 3; k++) {
		for (int i = 0; i < 4; i++) {
			if (place[k + i][3 + i] != color)
				i = 3;
			else if (i == 3) {
				win = 1;
				return 1;
			}

		}
		for (int i = 0; i < 4; i++) {
			if (place[2 + i][k + i] != color)
				i = 3;
			else if (i == 3) {
				win = 1;
				return 1;
			}
		}

	}

	for (int k = 0; k < 2; k++) {
		for (int i = 0; i < 4; i++) {
			if (place[k + i][2 + i] != color)
				i = 3;
			else if (i == 3) {
				win = 1;
				return 1;
			}

		}
		for (int i = 0; i < 4; i++) {
			if (place[1 + i][k + i] != color)
				i = 3;
			else if (i == 3) {
				win = 1;
				return 1;
			}

		}

		for (int i = 0; i < 4; i++) {
			if (place[i][1 + i] != color)
				i = 3;
			else if (i == 3) {
				win = 1;
				return 1;
			}

		}
		for (int i = 0; i < 4; i++) {
			if (place[i][i] != color)
				i = 3;
			else if (i == 3) {
				win = 1;
				return 1;
			}

		}

		//check for diagonal / wins
		for (int k = 0; k < 3; k++) {
			for (int i = 0; i < 4; i++) {
				if (place[3 - i][k + i] != color)
					i = 3;
				else if (i == 3) {
					win = 1;
					return 1;
				}

			}
			for (int i = 0; i < 4; i++) {
				if (place[5 - k - i][3 + i] != color)
					i = 3;
				else if (i == 3) {
					win = 1;
					return 1;
				}

			}
		}

		for (int k = 0; k < 2; k++) {
			for (int i = 0; i < 4; i++) {
				if (place[4 - i][k + i] != color)
					i = 3;
				else if (i == 3) {
					win = 1;
					return 1;
				}

			}
			for (int i = 0; i < 4; i++) {
				if (place[5 - k - i][2 + i] != color)
					i = 3;
				else if (i == 3) {
					win = 1;
					return 1;
				}

			}
		}
		for (int i = 0; i < 4; i++) {
			if (place[5 - i][i] != color)
				i = 3;
			else if (i == 3) {
				win = 1;
				return 1;
			}

		}
		for (int i = 0; i < 4; i++) {
			if (place[5 - i][i + 1] != color)
				i = 3;
			else if (i == 3) {
				win = 1;
				return 1;
			}

		}
		return 0;
	}
}

//function: converts the matrix to one string (in order to send it to the clients)
void convertToOneString(char* msgTransfer) {
	int k = 0;
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 7; j++) {
			msgTransfer[k] = place[i][j];
			k++;
		}
	}
	msgTransfer[45] = '\0';
}

//function: converts the string to a matrix (in order to print board)
void convertFromStringToMatrix(char* msgTransfer) {
	int k = -1;
	for (int k = 0; k < 42; k++) {
		place[k / 7][k % 7] = msgTransfer[k];
	}
}

//Main Program : Server
int main(int argc, char* argv[]) {
	int servSocket = socket(AF_INET, SOCK_STREAM, 0);
	int client1Socket, client2Socket;
	char srvIP[20] = {'\0'};

	if (servSocket < 0) {
		cout << "Failed to create socket" << endl;
	}
	struct sockaddr_in serverAdd, client1, client2;
	int port = atoi(argv[1]);
	socklen_t clientLen = sizeof(client1);
	serverAdd.sin_family = AF_INET;
	serverAdd.sin_port = htons(port);


    struct ifaddrs *ifap, *ifa;
    struct sockaddr_in *sa;
    char *addr;
    getifaddrs(&ifap);
    for (ifa = ifap; ifa; ifa = ifa->ifa_next)
    {
        if (ifa->ifa_addr && ifa->ifa_addr->sa_family==AF_INET)
        {
            sa = (struct sockaddr_in *) ifa->ifa_addr;
            addr = inet_ntoa(sa->sin_addr);
            printf("Interface: %s\tAddress: %s\n", ifa->ifa_name, addr);
        }
    }

    freeifaddrs(ifap);

    cout << "Please enter Server's ip address" << endl;
    cin >> srvIP;
    serverAdd.sin_addr.s_addr = inet_addr(srvIP);

	if (bind(servSocket, (struct sockaddr*) &serverAdd, sizeof(serverAdd))
			== -1) {
		cout << "failed to bind" << endl;
		return -1;
	}
	listen(servSocket, 2); // in order to listen to 2 clients
	client1Socket = accept(servSocket, (struct sockaddr*) &client1,
			&(clientLen));
	client2Socket = accept(servSocket, (struct sockaddr*) &client2,
			&(clientLen));

	// The server send Hello message to clients and receive their IP addresses
	sprintf(msgTransfer, "Hello Client 1");
	send(client1Socket, msgTransfer, 1024, 0);
	recv(client1Socket, msgTransfer, 1024, 0);
	cout << "IP of Client 1:" << msgTransfer << endl;
	sprintf(msgTransfer, "Hello Client 2");
	send(client2Socket, msgTransfer, 1024, 0);
	recv(client2Socket, msgTransfer, 1024, 0);
	cout << "IP of Client 2:" << msgTransfer << endl;

	// The server generates an empty board
	for (int i = 0; i <= 5; i++) {
		for (int j = 0; j <= 6; j++) {
			place[i][j] = '*';
		}
	}
	display();

	// The server asks the first player (first to connect) to choose a color
	char color1[1024] = { "\0" }, color2[1024] = { "\0" };
	sprintf(msgTransfer, "Please choose a Color - R or B");
	send(client1Socket, msgTransfer, 1024, 0);
	recv(client1Socket, msgTransfer, 1024, 0);
	if (strcmp(msgTransfer, "R") == 0) {
		strcpy(color1, "R");
		strcpy(color2, "B");
	} else if (strcmp(msgTransfer, "B") == 0) {
		strcpy(color1, "B");
		strcpy(color2, "R");
	}

	sprintf(msgTransfer, "Client 1 chose a color");
	send(client2Socket, msgTransfer, 1024, 0);
	send(client2Socket, color2, 1024, 0);
	cout << "The color for client 1:" << color1 << endl;
	cout << "The color for client 2:" << color2 << endl;

	// The server asks both player to choose a bit
	int bit1 = 0, bit2 = 0;
	int first = 0, second = 0;
	sprintf(msgTransfer, "Please choose a bit - 1 or 0");
	send(client1Socket, msgTransfer, 1024, 0);
	recv(client1Socket, msgTransfer, 1024, 0);
	bit1 = atoi(msgTransfer);

	sprintf(msgTransfer, "Please choose a bit - 1 or 0");
	send(client2Socket, msgTransfer, 1024, 0);
	recv(client2Socket, msgTransfer, 1024, 0);
	bit2 = atoi(msgTransfer);

	if (bit1 ^ bit2 == 0) {
		cout << "First to play is: " << "Client1" << endl;
		cout << "Second to play is: " << "Client2" << endl;
		first = client1Socket;
		second = client2Socket;
	} else {
		cout << "First to play is: " << "Client2" << endl;
		cout << "Second to play is: " << "Client1" << endl;
		first = client2Socket;
		second = client1Socket;
	}
	int won=0; // flag created for tie
	while (1) {
		//sends board to both clients
		convertToOneString(msgTransfer);
		cout << "Board: " << msgTransfer << endl;
		send(client1Socket, msgTransfer, 1024, 0);
		send(client2Socket, msgTransfer, 1024, 0);

		if(win==1)
			break;

		//send to both players who's turn is
		sprintf(msgTransfer, "This is your turn");
		send(first, msgTransfer, 1024, 0);
		sprintf(msgTransfer, "Please wait for your turn");
		send(second, msgTransfer, 1024, 0);

		//sends message between players
		int ingame = 1;
		while (ingame) {
			char endmsg[1024] = "End of turn";
			recv(first, msgTransfer, 1024, 0);
			if (strcmp(endmsg, msgTransfer) == 0) {
				ingame = 0;
				send(second, msgTransfer, 1024, 0);
			} else {
				send(second, msgTransfer, 1024, 0);
			}
		}

		int temp = first;
		first = second;
		second = temp;
		recv(second, msgTransfer, 1024, 0);
		convertFromStringToMatrix(msgTransfer);
		display();

		//check for wins
		if (checkWin(color1[0]) == 1)
		{
			if (checkWin(color2[0]) == 1)
			{
				cout << "It's a tie" << endl;
				win=1;
				won = -1;
			}
			else
			{
				cout << "Client 1 wins, IP:" << inet_ntoa(client1.sin_addr) << endl;
				win=1;
				won = client1Socket;
			}
		}
		else
		 {
			if (checkWin(color2[0]) == 1)
			{
				cout << "Client 2 wins, IP:" << inet_ntoa(client2.sin_addr) << endl;
				win=1;
				won = client2Socket;
			}
			else
				cout << "No win" << endl;
		}
	}

	if(won==client1Socket)
	{

	sprintf(msgTransfer, "You win!");
	send(client1Socket, msgTransfer, 1024, 0);

	convertToOneString(msgTransfer);
	send(client2Socket, msgTransfer, 1024, 0);
	sprintf(msgTransfer, "You lose!");
	send(client2Socket, msgTransfer, 1024, 0);
	}

	if(won==client2Socket)
	{
		sprintf(msgTransfer, "You win!");
		send(client2Socket, msgTransfer, 1024, 0);

		sprintf(msgTransfer, "You lose!");
		send(client1Socket, msgTransfer, 1024, 0);
	}

	if(won==-1)
		{
			sprintf(msgTransfer, "Its a tie!");
			send(client2Socket, msgTransfer, 1024, 0);

			sprintf(msgTransfer, "Its a tie!");
			send(client1Socket, msgTransfer, 1024, 0);
		}


}
