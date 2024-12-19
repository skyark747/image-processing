#include <iostream>
#include <WinSock2.h>
#include <mutex>
#include <string>
#include <queue>
#include <fcntl.h>
#include <iomanip>
#pragma comment(lib, "ws2_32.lib")
using namespace std;

struct ImageData
{
	int **my_image;
	long long r;
	long long c;
};

static queue<ImageData> Q;
static queue<int> Ans;

bool countrec(int r, int c, int **img, double R, double C, bool **image)
{
	if (r < 0 || r >= R || c < 0 || c >= C)
		return false;
	else if (img[r][c] == 255)
		return false;

	image[r][c] = true;

	if (c >= 0 && img[r][c - 1] == 0 && image[r][c - 1] == false)
		countrec(r, c - 1, img, R, C, image);
	else if (c <= C && img[r][c + 1] == 0 && image[r][c + 1] == false)
		countrec(r, c + 1, img, R, C, image);
	else if (r >= 0 && img[r - 1][c] == 0 && image[r - 1][c] == false)
		countrec(r - 1, c, img, R, C, image);
	else if (r <= R && img[r + 1][c] == 0 && image[r + 1][c] == false)
		countrec(r + 1, c, img, R, C, image);

	return true;
}

int count_characters(double r, double c, int **&my_image)
{
	bool **image = new bool *[r];
	for (int i = 0; i < r; i++)
	{
		image[i] = new bool[c];
		for (int j = 0; j < c; j++)
		{
			image[i][j] = false;
		}
	}

	int midrow = r / 2;
	int midcol = c / 2;

	int thres = my_image[midrow][midcol];
	thres -= 60; // adjust for your picture

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

	int count = 0;
	for (int i = 0; i < r; i++)
	{
		for (int j = 0; j < c; j++)
		{
			if (image[i][j] == false && image[i][j] == 0)
			{
				countrec(i, j, my_image, r, c, image);
				count++;
			}
		}
	}
	for (int i = 0; i < r; i++)
		delete image[i];
	delete[] image;
	return count;
}

ImageData pushtoqueue(SOCKET clientsocket)
{
	int r, c;
	if (recv(clientsocket, (char *)&r, sizeof(int), 0) < 0)
	{
		closesocket(clientsocket);
	}
	if (recv(clientsocket, (char *)&c, sizeof(int), 0) < 0)
	{
		closesocket(clientsocket);
	}

	int **mat = new int *[r];
	for (int i = 0; i < r; i++)
		mat[i] = new int[c];

	int index = 0;
	int bytes = 0;
	int matc = 0;
	for (int i = 0; i < r; i++)
	{
		bytes = recv(clientsocket, (char *)mat[i], c * 4, 0);
		if (bytes <= 0)
		{
			cout << "blocking\n";
		}
	}
	ImageData D;
	D.my_image = mat;
	D.r = r;
	D.c = c;
	return D;
}
DWORD WINAPI ThreadWrapper(LPVOID lpParam)
{
	int *n = (int *)lpParam;
	SOCKET client = *n;
	ImageData D = pushtoqueue(client);
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

	bind(serversocket, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
	{
		cout << "Bind Failed.\n";
		closesocket(serversocket);
		WSACleanup();
		return 1;
	}

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

		HANDLE threads;

		threads = CreateThread(NULL, 0, ThreadWrapper, &clientsocket, 0, NULL);
	}

	closesocket(serversocket);
	WSACleanup();
	return 0;
}