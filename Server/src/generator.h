/*
 * generator.h
 *
 *  Created on: 15 dic 2024
 *      Author: barba
 */

#ifndef GENERATOR_H_
#define GENERATOR_H_

#include <time.h>

char * type_switcher(char *, char *, char *);
void ShowOnline(int current_clients);
void handle_client(void *arg);

void initialize_random();
char* generate_custom(int length, const char* charset, int charset_size);
char* generate_numeric(int length);
char* generate_alpha(int length);
char* generate_mixed(int length);
char* generate_secure(int length);
char* generate_unambiguous(int length);

int current_clients = 0;

#endif /* GENERATOR_H_ */
