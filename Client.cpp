//============================================================================
// Name        : Client.cpp
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
#include <arpa/inet.h>

using namespace std;

char place[6][7]; // 6 rows 7 columns board
char msgTransfer[1024] = { '\0' }; // length of a message that will transfer between server and clients
char color[1024] = { '\0' }; // color of each client (R for Red , B for Blue)
int clientSocket = socket(AF_INET, SOCK_STREAM, 0);

//function: in order to send messages
void initMSG(char* msg)
{
	for (int i=0;i<1024;i++)
		msg[i] = '\0';
}

//function: converts the matrix to one string (in order to send it to the clients)
void convertToOneString(char* msgTransfer)
{
	int k=0;
	for (int i=0;i<6;i++)
	{
		for (int j=0;j<7;j++)
		{
			msgTransfer[k]=place[i][j];
			k++;
		}
	}
	msgTransfer[45]='\0';
}

//function: converts the string to matrix (in order to print board)
void convertFromStringToMatrix(char* msgTransfer) {
	int k = -1;
	for (int k = 0; k < 42; k++) {
		place[k/7][k % 7] = msgTransfer[k];
	}
}

//function: for the current player
void turn() {
	//menu for current player
	int choose = 0;
	int move = 0;
	int bool_move = 0;
	int i = 0;
	char msg[1024] = { '\0' };
	i = 0;
	while (1) {
		cout << "press 1 to make an insert move" << endl;
		cout << "press 2 to make a pop move" << endl;
		cout << "press 3 to send message to the other player" << endl;
		cout << "press 4 to end your turn" << endl;
		cin >> choose;
		switch (choose) {
		case 1:
		{
			cout << "choose a number between 1-7" << endl;
			cin >> move;
			if ((move >= 1) && (move <= 7))
			{
				if (bool_move == 1)
				{
					cout << "you already made a move!"<< endl;
					break;
				}
				else
				{
					int last = -1;
					while((place[last+1][move-1]=='*') && (last < 6))
						 last+=1;
					if (last==-1)
						cout << "column is full!" << endl;
					else
					{
						place[last][move-1] = color[0];
						bool_move = 1; //move was made
					}
					//i++;
					break;
				}
			}
			else
			{
				cout << "wrong number!" << endl;
				break;
			}
			break;
		}
		case 2:
		{
			cout << "choose a number between 1-7" << endl;
			cin >> move;
			if ((move >= 1) && (move <= 7))
			{
				if (bool_move == 1)
				{
					cout << "you already made a move!"<< endl;
					break;
				}
				else
				{
					if(place[5][move-1]!=color[0])
					{
						cout << "you can not make this move!"<< endl;
						break;
					}
					else
					{
						for(int i=0;i<5;i++)
							place[5-i][move-1]=place[5-i-1][move-1];
						place[0][move-1]='*';
						bool_move=1;
					}
					break;
				}
			}
			else
			{
				cout << "wrong number!" << endl;
				break;
			}
			break;
		}
		case 3:
		{
			cout << "type your message" << endl;
			cin.getline(msg,sizeof(msg)); // for enter
			cin.getline(msg,sizeof(msg));
			send(clientSocket, msg, sizeof(msg), 0); //sends the message to the other client
			break;
		}
		case 4:
		{
			if (bool_move == 1) {
				initMSG(msgTransfer);
				sprintf(msgTransfer, "End of turn");
				send(clientSocket, msgTransfer, 1024, 0);
				initMSG(msgTransfer);
				return;
			} else {
				cout << "You didn't make a move!" << endl;
				break;
			}
		}
		}
	}
}

//function: for the player who doesnt play atm
void waiting()
{
	while (1)
	{
		recv(clientSocket, msgTransfer, 1024, 0);
		if(strcmp(msgTransfer,"End of turn")==0)
		{
			cout<<"End of turn"<<endl;
			return;
		}
		cout << "Received from other Client:" << msgTransfer << endl;
	}
}

// function: prints the game board
void display()
{
	for (int i=0;i<6;i++)
	{
		for (int j=0;j<7;j++)
		{
			cout<<place[i][j]<<" ";
		}
		cout<<endl;
	}
}

//Main Program : Client
int main(int argc, char* argv[])
{
	if (clientSocket < 0)
	{
		cout << "Failed to create client" << endl;
	}
	struct sockaddr_in serverAdd;
	int port = atoi(argv[1]);
	char srvIP[20] = {'\0'};
	serverAdd.sin_family = AF_INET;
	serverAdd.sin_port = htons(port);

	cout << "Please enter Server's ip address" << endl;
	cin >> srvIP;

	serverAdd.sin_addr.s_addr = inet_addr(srvIP);


	connect(clientSocket, (struct sockaddr*) &serverAdd, sizeof(serverAdd));
	recv(clientSocket, msgTransfer, 1024, 0);
	cout << "Received from Server:" << msgTransfer << endl;
	send(clientSocket, inet_ntoa(serverAdd.sin_addr), 1024, 0);

	// Client is being asked to choose a color

	recv(clientSocket, msgTransfer, 1024, 0);
	cout << "Received from Server:" << msgTransfer << endl;
	if (strcmp(msgTransfer, "Client 1 chose a color") != 0)
	{
		cin >> color;
		strcpy(msgTransfer, color);
		send(clientSocket, msgTransfer, 1024, 0);
	}
	else
		recv(clientSocket, msgTransfer, 1024, 0);
	color[0] = msgTransfer[0];
	// Client is being asked to choose a bit
	char bit[1024] = { '\0' };
	recv(clientSocket, msgTransfer, 1024, 0);
	cout << "Received from Server:" << msgTransfer << endl;
	cin >> bit;
	strcpy(msgTransfer, bit);
	send(clientSocket, msgTransfer, 1024, 0);

	while (1)
	{
		//receive board status from the server
		recv(clientSocket, msgTransfer, 1024, 0);
		if((strcmp(msgTransfer,"You win!")==0) || (strcmp(msgTransfer,"You lose!")==0) || (strcmp(msgTransfer,"Its a tie!")==0))
		{
			cout << "Received from Server:" << msgTransfer << endl;
			break;
		}
		convertFromStringToMatrix(msgTransfer);
		cout << "Received board:" << msgTransfer << endl;
		//receive turn message from the server
		recv(clientSocket, msgTransfer, 1024, 0);
		cout << "Received from Server:" << msgTransfer << endl;

		display();
		if (strcmp(msgTransfer, "This is your turn") == 0)
		{
			initMSG(msgTransfer);
			turn();

			convertToOneString(msgTransfer);
			cout<< msgTransfer<<endl;
			send(clientSocket,msgTransfer,1024,0);
		}
		if (strcmp(msgTransfer, "Please wait for your turn") == 0)
			waiting();
	}

}
