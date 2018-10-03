#pragma once

//winsock2 library
#pragma comment(lib, "ws2_32.lib")

class TCPSocket;

bool NetSystemStartup();

void NetSystemShutdown();

TCPSocket* CreateListenSocket(unsigned short port);