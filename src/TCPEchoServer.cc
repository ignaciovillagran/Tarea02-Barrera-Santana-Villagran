/*
 *   C++ sockets on Unix and Windows
 *   Copyright (C) 2002
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "includes/YASL.h"      // For Socket, ServerSocket, and SocketException
#include "includes/json.hpp"
#include <iostream>    // For cerr and cout
#include <cstdlib>     // For atoi()
#include <fstream>

const uint32_t RCVBUFSIZE = 32;    // Size of receive buffer
using json = nlohmann::json;
std::string nombrefinal;

void HandleTCPClient(TCPSocket *sock);
std::string buscar(std::string linea);

int main(int argc, char *argv[]) {
	
	uint16_t echoServPort;   

	// read a JSON file
	std::ifstream i("bin/config.json");
	json j;
	j = json::parse(i);
	std::string ip = j["ip"];
	echoServPort = j["puerto"];
	std::string root_dir = j["root_dir"];
	std::string notFoundFile = j["notFoundFile"];
	
	printf("Ubicacion a usar:\n");
	std::cout << ip;
	printf(":");
	std::cout << echoServPort;
	printf("\n");

	try {
		TCPServerSocket servSock(ip,echoServPort);     // Server Socket object

		for (;;) {   // Run forever
			HandleTCPClient(servSock.accept());       // Wait for a client to connect
		}
	} catch (SocketException &e) {
		std::cerr << e.what() << std::endl;
		exit(EXIT_FAILURE);
	}
	// NOT REACHED

	return EXIT_SUCCESS;
}


// TCP client handling function
void HandleTCPClient(TCPSocket *sock) {

	std::string echoB;
	uint32_t cont = 0;
	const char *cstr;

	std::cout << "Handling client ";
	try {
		std::cout << sock->getForeignAddress() << ":";
	} catch (SocketException e) {
		std::cerr << "Unable to get foreign address" << std::endl;
	}
	try {
		std::cout << sock->getForeignPort();
	} catch (SocketException e) {
		std::cerr << "Unable to get foreign port" << std::endl;
	}
	std::cout << std::endl;

	// Send received string and receive again until the end of transmission
	char echoBuffer[RCVBUFSIZE];
	//char echoBuff[RCVBUFSIZE];
	uint32_t recvMsgSize;
	while ((recvMsgSize = sock->recv(echoBuffer, RCVBUFSIZE)) > 0) { // Zero means
	                                                 // end of transmission
		if(cont == 0) {
			echoB = buscar(echoBuffer);
			cstr = echoB.c_str();
			sock->send(cstr, 1024);
		}
		// Echo message back to client
		//sock->send(echoBuffer, recvMsgSize);
		cont += 1;
	}
	delete sock;
}


std::string buscar(std::string linea) {
	
	std::string content = "",nombre = "",line;
	nombre = linea.substr(0, linea.find(" HTTP/1.1"));
	nombre = nombre.erase(0,4);
	if(nombre == "/favicon.ico"){
		nombrefinal = nombrefinal;
	}else{
		nombrefinal = nombre;
	}

	std::ifstream fs("www-data"+nombrefinal);
	if(fs.good() != false) {
		while(getline(fs,line)) {
			content += line+"\n";
		}
	}else {
		std::cout << nombrefinal;
		printf("no existe\n");
	}
	fs.close();
	return content;
}