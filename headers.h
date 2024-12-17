/*
 * headers.h
 *
 *  Created on: 15 dic 2024
 *      Author: barba
 */

#ifndef HEADERS_H_
#define HEADERS_H_

#if defined WIN32
	#include <winsock2.h>
	#include <ws2tcpip.h>
	#include <Windows.h>
#else
	#define closesocket close
	#include <sys/socket.h>
	#include <arpa/inet.h>
	#include <unistd.h>
	#include <pthread.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define BUFFERSIZE 512
#define PROTOPORT 60001
#define SERVER_ADDRESS "127.0.0.1"

void clearwinsock();
void errorhandler(char *);
void tokenizer(char * tokens[3], char *);
void SetColor(unsigned short color);

#endif /* HEADERS_H_ */
