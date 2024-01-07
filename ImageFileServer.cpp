// ImageFile.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <WinSock2.h>
#include <mutex>
#include <string>
#include <queue>
#include <fcntl.h>
#pragma comment(lib,"ws2_32.lib")
using namespace std;

#define MAX_THREADS 3

struct ImageData
{
	int** my_image;
	long long r;
	long long c;
};

static queue<ImageData>Q;
static queue<int>Ans;
mutex mtx;
condition_variable cv;

int countrec(int r, int c, int**& img, double R, double C)
{
	if (r < 0 || r >= R || c < 0 || c >= C)
		return 0;
	else if (img[r][c] == 255)
		return 0;
	else if (img[r][c] == -1)
		return 0;
	else if (img[r][c] == 0)
	{
		img[r][c] = -1;
		int count = 1;
		count += countrec(r - 1, c, img, R, C);
		count += countrec(r + 1, c, img, R, C);
		count += countrec(r, c - 1, img, R, C);
		count += countrec(r, c + 1, img, R, C);
		count += countrec(r + 1, c + 1, img, R, C);
		count += countrec(r - 1, c - 1, img, R, C);
		count += countrec(r - 1, c + 1, img, R, C);
		count += countrec(r + 1, c - 1, img, R, C);
		return count;
	}
	else
		throw exception("bara h");
	
		
}

int count_characters(double r, double c, int**& my_image)
{
	int midrow = r / 2;
	int midcol = c / 2;

	int thres = my_image[midrow][midcol];
	thres -= 60;

	for (int i = 0; i < r; i++)
	{
		for (int j = 0; j < c; j++)
		{
			if (my_image[i][j] <= thres)
			{
				my_image[i][j] = 0;
			}
			else if (my_image[i][j] > thres)
			{
				my_image[i][j] = 255;
			}
		}
	}
	
	int count = 0; int ch=0;
	for (int i = 0; i < r; i++)
	{
		for (int j = 0; j < c; j++)
		{
			if (my_image[i][j] == 0 && my_image[i][j] != -1)
			{
				try
				{
					ch=countrec(i, j, my_image, r, c);
					count++;
				}
				catch (exception& e)
				{
					cout<<e.what();
				}
			}
		}
	}
	return count;
}

ImageData pushtoqueue(SOCKET clientsocket)
{
	unique_lock<mutex>lock(mtx);
	int r, c;
	if (recv(clientsocket,(char*)&r, sizeof(int), 0) < 0)
	{
		closesocket(clientsocket);
		//return 1;
	}
	if (recv(clientsocket, (char*)&c, sizeof(int), 0) < 0)
	{
		closesocket(clientsocket);
		//return 1;
	}

	int** mat = new int* [r];
	for (int i = 0; i < r; i++)
		mat[i] = new int[c];
	
	int index = 0; int bytes = 0; int matc = 0;
	for (int i = 0; i < r; i++)
	{
		bytes=recv(clientsocket, (char*)mat[i], c * 4, 0);
		if (bytes <= 0)
		{
			cout << "blocking\n";
		}
	}
	//Q.push({ mat,r,c});
	ImageData D;
	D.my_image = mat; D.r = r;
	D.c = c;
	lock.unlock();

	//cv.notify_one();
	return D;
}
DWORD WINAPI ThreadWrapper(LPVOID lpParam)
{
	int* n = (int*)lpParam;
	SOCKET client = *n;
	ImageData D=pushtoqueue(client);
	Q.push(D);
	return 0;
}
DWORD WINAPI ThreadWrapper_1(LPVOID lpParam)
{
	while (true)
	{
		if (!Q.empty())
		{			
			ImageData data = Q.front();
			Q.pop();
			double r = data.r;
			double c = data.c;

			int answer = count_characters(r, c, data.my_image);
			cout << answer << " ";
		}	
		
	}
	return 0;
}
int main()
{

	WSADATA wsdata;

	if (WSAStartup(MAKEWORD(2, 2), &wsdata) != 0)
	{
		cout << "Failed to initialize Winsock." << endl;
		return 1;
	}

	SOCKET serversocket = socket(AF_INET, SOCK_STREAM, 0);

	if (serversocket == INVALID_SOCKET)
	{
		cout << "Failed to create socket for the server." << endl;
		WSACleanup();
		return 1;
	}

	sockaddr_in serveraddr;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(12345);

	bind(serversocket, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	/*{
		cout << "Bind Failed.\n";
		closesocket(serversocket);
		WSACleanup();
		return 1;
	}*/


	if (listen(serversocket, SOMAXCONN) == SOCKET_ERROR)
	{
		cout << "Listen Failed.\n";
		closesocket(serversocket);
		WSACleanup();
		return 1;
	}

	cout << "Server is now listening....\n";

	HANDLE threads_1;
	int n = 1;
	
		threads_1 = CreateThread(NULL, 0, ThreadWrapper_1, &n, 0, NULL);
		if (threads_1 == NULL)
		{
			cout << "Failed to create Thread " << ". Error code: " << GetLastError() << endl;
			return 1;
		}

	
	while (true)
	{
		SOCKET clientsocket = accept(serversocket, nullptr, nullptr);
		if (clientsocket == SOCKET_ERROR)
		{
			cout << "Failed to accept client.\n";
			closesocket(serversocket);
			WSACleanup();
			return 1;
		}

		HANDLE threads[MAX_THREADS];

		for (int i = 0; i < MAX_THREADS; i++)
		{
			threads[i] = CreateThread(NULL, 0, ThreadWrapper, &clientsocket, 0, NULL);
			if (threads[i] == NULL)
			{
				cout << "Failed to create Thread " << i << ". Error code: " << GetLastError() << endl;
				return 1;
			}
		}

		//WaitForMultipleObjects(MAX_THREADS, threads, TRUE, INFINITE);
		//closesocket(clientsocket);

	}
	
	
	closesocket(serversocket);
	WSACleanup();
	return 0;
}