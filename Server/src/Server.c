/*
 ============================================================================
 Name        : Server_UDP.c
 Author      : Barbaro Gerardo
 Version     :
 Copyright   : Your copyright notice
 Description : UDP Server in C, ANSI-style
 ============================================================================
 */
#include "../../headers.h"
#include "generator.h"

int main(int argc, char *argv[])
{
    int port;
    char *start_address_server = "";
    // Check command line arguments for server address and port
    if (argc > 1) {
        start_address_server = argv[1];
        port = atoi(argv[2]);
    } else {
        port = PROTOPORT;
        start_address_server = SERVER_ADDRESS;
        if (port < 0) {
            printf("%d is a bad port number\n", port);
            return 0;
        }
        printf("%s", "This port is available\n");
    }
    printf("%s ", "Trying to start server with");
    SetColor(1);
    printf("%s:%d\n", start_address_server, port);
    SetColor(7);

#if defined WIN32
    // Initialize Winsock on Windows
    WSADATA wsa_data;
    int result;
    result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
    if (result != NO_ERROR) {
        errorhandler("Start up failed\n");
        return 0;
    }
    printf("Start up done correctly\n");
#endif

    // Socket creation
    int server_socket;
    server_socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (server_socket < 0) {
        errorhandler("Socket creation failed\n");
        clearwinsock();
        return -1;
    }
    printf("%s", "Socket created successfully\n");

    // Set up server address structure
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(start_address_server);
    server_address.sin_port = htons(port);

    // Bind socket to the specified port and address
    int server_bind;
    server_bind = bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address));
    if (server_bind < NO_ERROR) {
        errorhandler("Binding failed\n");
        closesocket(server_socket);
        clearwinsock();
        return -1;
    }
    printf("%s", "Binding successful\n");

    printf("%s", "Waiting for a client to send data\n\n");

    // Loop to receive and handle client messages
    struct sockaddr_in client_address;
    int client_len = sizeof(client_address);
    char client_data[BUFFERSIZE];

    while (1) {
        memset(client_data, 0, BUFFERSIZE);
        int bytes_received = recvfrom(server_socket, client_data, BUFFERSIZE - 1, 0, (struct sockaddr *)&client_address, &client_len);
        if (bytes_received < 0) {
            errorhandler("Error receiving data\n");
            continue;
        }

        // Null-terminate the received data
        client_data[bytes_received] = '\0';

        printf("\nData received from client ");
        SetColor(1);
        printf("%s:%d", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
        SetColor(7);
        printf(" : %s\n", client_data);

        // Process the received data
        char *string_tokens[3];
        tokenizer(string_tokens, client_data );

        if (strcmp(string_tokens[0], "q") == 0) {
            printf("Client requested to quit.\n");
            continue; // In a real application, you might want to handle this differently
        } else {
            printf("Generating password...\n");
            char password_result[BUFFERSIZE];
            memset(password_result, 0, BUFFERSIZE);

            char *switcher = type_switcher(string_tokens[0], string_tokens[1], string_tokens[2]);
            if (switcher != NULL) {
                strcpy(password_result, switcher);
                free(switcher); // Free the dynamically allocated memory
            } else {
                strcpy(password_result, "Error: Invalid type");
            }

            int string_len = strlen(password_result);
            printf("Password generated -> %s\n\n", password_result);
            if (sendto(server_socket, password_result, string_len, 0, (struct sockaddr *)&client_address, client_len) < 0) {
                errorhandler("Error sending data\n");
            }
        }
    }

    closesocket(server_socket);
    clearwinsock();
    return 0;
}

// Function to select password type based on client's request
char * type_switcher(char *type, char *length, char *token_num) {
    int token_number = atoi(token_num);

    if (token_number > 2) {
        printf("%d\n", token_number);
        printf("%s", "Parsing error");
        return NULL;
    }

    int number = atoi(length);
    char *pw_result = NULL;
    printf("Type is: %c\n", type[0]);

    // Generate password based on type character
    switch (type[0]) {
    	case 'h':

                break;
        case 'n':
            pw_result = generate_numeric(number);
            break;
        case 'a':
            pw_result = generate_alpha(number);
            break;
        case 'm':
            pw_result = generate_mixed(number);
            break;
        case 's':
            pw_result = generate_secure(number);
            break;
        case 'u':
			pw_result = generate_unambiguous(number);
			break;
        default:
            printf("Invalid type \n");
            return NULL;
    }

    return pw_result;
}

// Function to tokenize the client input string into an array
void tokenizer(char *tokens[3], char *string) {
    if (string == NULL || strlen(string) == 0) {
        printf("Error: Input is empty\n");
        tokens[0] = "e";  // Indicates an error
        return;
    }

    char *type = string;
    short temp = 0;
    char *token_string;
    token_string = strtok(type, " ");

    // Check if the token_string is not null
    if (token_string == NULL || strlen(token_string) != 1) {
        printf("Error: Invalid or empty token\n");
        tokens[0] = "e";  // Indicates an error
        return;
    }

    if (strcmp(token_string, "q") == 0) {
        tokens[0] = "q";
        tokens[1] = NULL;
        tokens[2] = "1";
    } else {
        while (token_string != NULL) {
            tokens[temp] = token_string;
            temp++;
            token_string = strtok(NULL, " ");
        }
        if (temp == 2) {
            char str_temp_var[BUFFERSIZE];
            snprintf(str_temp_var, BUFFERSIZE, "%d", temp);
            tokens[temp] = str_temp_var;
        }
    }
}

// Function to clean up Winsock resources on Windows
void clearwinsock() {
#if defined WIN32
    WSACleanup();
#endif
}

// Error handling function for printing messages
void errorhandler(char * string) {
    printf("%s", string);
}

void SetColor(unsigned short color) {
    HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hCon, color);
}

// Initializes the random number generator once
void initialize_random() {
    static int initialized = 0;
    if (!initialized) {
        srand((unsigned int)time(NULL));
        initialized = 1;
    }
}

// Generates a random string using a specified character set
char* generate_custom(int length, const char* charset, int charset_size) {
    initialize_random();
    char* result = (char*)malloc((length + 1) * sizeof(char));
    if (result == NULL) {
        printf("Memory allocation failed\n");
        return NULL;
    }

    for (int i = 0; i < length; i++) {
        result[i] = charset[rand() % charset_size];
    }

    result [length] = '\0';
    return result;
}

// Generates a numeric password
char* generate_numeric(int length) {
    return generate_custom(length, "0123456789", 10);
}

// Generates an alphabetic password
char* generate_alpha(int length) {
    return generate_custom(length, "abcdefghijklmnopqrstuvwxyz", 26);
}

// Generates a mixed alphanumeric password
char* generate_mixed(int length) {
    return generate_custom(length, "abcdefghijklmnopqrstuvwxyz0123456789", 36);
}

// Generates a secure password with special characters
char* generate_secure(int length) {
    return generate_custom(length, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()-_=+[]{}|;:,.<>?", 86);
}

// Generates an unambiguous password
char* generate_unambiguous(int length) {
    return generate_custom(length, "2346789abcdefghjkmnpqrstuvwxyzABCDEFGHJKMNPQRSTUVWXYZ!@#$%^&*()-_=+[]{}|;:,.<>?", 78);
}


