#include <iostream>
#include <WinSock2.h>
#include <string>
#include <vector>
#include <fstream>
#include <queue>
#include <fstream>
#include "Image.h"
#pragma warning(disable : 4996)
#pragma comment(lib, "ws2_32.lib")
using namespace std;

void client()
{
    string pic;
    int n;
    cout << "Select pictures to process (1,2,3).\n";
    cin >> n;

    if (n == 1)
    {
        pic = "/path/to/picture";
    }
    else if (n == 2)
    {
        pic = "/path/to/picture";
    }
    else if (n == 3)
    {
        pic = "/path/to/picture";
    }
    Image my_image = (pic.c_str());

    char buffer[10];
    int r = my_image.get_rows();
    int c = my_image.get_cols();
    send(clientsocket, (char *)&r, sizeof(int), 0);
    Sleep(0.3); //adjust accordingly
    send(clientsocket, (char *)&c, sizeof(int), 0);

    int **mat = new int *[r];
    for (int i = 0; i < r; i++)
        mat[i] = new int[c];

    for (int i = 0; i < r; i++)
    {
        for (int j = 0; j < c; j++)
        {
            mat[i][j] = my_image[i][j];
        }
    }
    for (int i = 0; i < my_image.get_rows(); i++)
    {
        send(clientsocket, (char *)mat[i], c * 4, 0);
    }
    closesocket(clientsocket);
    WSACleanup();
    for(int i = 0; i <r; i++)
    {
        delete mat[i];
    }
    delete[] mat;
}


int main()
{
    WSADATA wsdata;
    int i = 0;
    if (WSAStartup(MAKEWORD(2, 2), &wsdata) != 0)
    {
        cout << "Failed to initialize Winsock." << endl;
        return 1;
    }
        SOCKET clientsocket = socket(AF_INET, SOCK_STREAM, 0);

        if (clientsocket == INVALID_SOCKET)
        {
            cout << "Failed to create socket for the client." << endl;
            WSACleanup();
            return 1;
        }

        sockaddr_in serveraddr;
        serveraddr.sin_family = AF_INET;
        serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");
        serveraddr.sin_port = htons(12345);
        if (connect(clientsocket, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) == SOCKET_ERROR)
        {
            cout << "Failed to connect to the server.\n";
            closesocket(clientsocket);
            WSACleanup();
            return 1;
        }
        cout << "client connected to the server....\n";
        client();
        return 0;
}