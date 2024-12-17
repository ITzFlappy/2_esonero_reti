/*
 ============================================================================
 Name        : Client.c
 Author      : Barbaro Gerardo
 Version     :
 Copyright   : Your copyright notice
 Description : Client UDP in C, Ansi-style
 ============================================================================
 */
#include "../../headers.h"

int isValidNumber(char *);
int checkChar(char *);
void ShowMenu();

int main() {
#if defined WIN32
    // Initialize Winsock for Windows
    WSADATA wsa_data;
    int result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
    if (result != NO_ERROR) {
        printf("%s", "Start up failed");
        return -1;
    }
    printf("%s", "Start up done correctly\n");
#endif

    // Socket creation
    int client_socket;
    client_socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (client_socket < 0) {
        errorhandler("Socket creation failed\n");
        clearwinsock();
        return -1;
    }
    printf("%s", "Socket created successfully\n");

    // Setting up the server address structure
    char *address_to_connect = "passwdgen.uniba.it";
    int port = PROTOPORT;
    struct addrinfo hints, *res;

    memset(&hints, 0, sizeof(hints)); // Clear structure memory
	hints.ai_family = AF_INET; // Use IPv4
	hints.ai_socktype = SOCK_DGRAM; // UDP

	// Resolve the server address
	if (getaddrinfo(address_to_connect, NULL, &hints, &res) != 0) {
		errorhandler("getaddrinfo failed\n");
		closesocket(client_socket);
		clearwinsock();
		return -1;
	}

    struct sockaddr_in server_ad;
    memcpy(&server_ad, res->ai_addr, res->ai_addrlen); // Copy resolved address
	freeaddrinfo(res); // Free the addrinfo structure

	server_ad.sin_port = htons(port); // Set the port

    printf("%s ", "Trying to send data to");
    SetColor(1);
    printf("%s:%d\n", address_to_connect, port);
    SetColor(7);

    // Allocate buffer for input data
    char *input_string = malloc(BUFFERSIZE);
    memset(input_string, 0, BUFFERSIZE);

    while (1) {
        memset(input_string, 0, BUFFERSIZE);

        // Display options for generating different types of passwords
        ShowMenu();

        SetColor(10);
        fgets(input_string, BUFFERSIZE - 1, stdin); // Get user input with a size limit
        SetColor(7);

        // Remove newline character at the end of input if it exists
        int string_len = strlen(input_string);
        if ((string_len > 0) && (input_string[string_len - 1] == '\n')) {
            input_string[string_len - 1] = '\0';
        }

        char *string_tokens[3]; // Array to hold tokenized parts of input
        char input_string_cpy[BUFFERSIZE];
        strcpy(input_string_cpy, input_string);
        tokenizer(string_tokens, input_string_cpy); // Tokenize the input

        // Check if the input is h for the menu
		if (strcmp(string_tokens[0], "h") == 0) {

			system("CLS");
			continue; // Ask for input again
		}

        // Check if the input is valid
        if (strcmp(string_tokens[0], "e") == 0) {
            printf("Invalid input. Please enter a valid character and length.\n");
            Sleep(1500);
            system("CLS");
            continue; // Ask for input again
        }

        // Check if client requested to quit
        if (strcmp(string_tokens[0], "q") == 0) {
            printf("%s\n", "Closing connection with server\n");
            break;
        }

        // Check for correct input format and values
        int tokens_number = atoi(string_tokens[2]);
        if (tokens_number != 2 || !checkChar(string_tokens[0]) || isValidNumber(string_tokens[1])) {
            printf("%s", "You must send to server a valid character\n");
            Sleep(1500);
            system("CLS");
            continue; // Ask for input again
        }

        // Send data to the server
        int data_sent;
        data_sent = sendto(client_socket, input_string, string_len, 0, (struct sockaddr *)&server_ad, sizeof(server_ad));
        if (data_sent != string_len) {
            errorhandler("Different number of bytes has been sent to the server\n");
            closesocket(client_socket);
            clearwinsock();
            return -1;
        }
        printf("%s", "Data sent successfully\n");

        // Receive response from server
        int bytes_received = 0;
        char server_string[BUFFERSIZE];
        memset(server_string, 0, BUFFERSIZE);
        int server_len = sizeof(server_ad); // Length of the server address structure
        bytes_received = recvfrom(client_socket, server_string, BUFFERSIZE - 1, 0, (struct sockaddr *)&server_ad, &server_len);
        if (bytes_received <= 0) {
            errorhandler("Retrieve failed or connection closed prematurely\n");
            closesocket(client_socket);
            clearwinsock();
            return -1;
        }

        SetColor(10);
        printf("Password generated: %s\n", server_string);
        SetColor(7);

        Sleep(3000);
        system("CLS");
        printf("%s%s\n%s%s\n", "Previous input -> ", input_string, "Result -> ", server_string);
    }

    // Close socket and clean up Winsock
    closesocket(client_socket);
    clearwinsock();
    printf("%s", "\n");
    system("PAUSE");

    return 0;
}

// Clear the Winsock environment for Windows
void clearwinsock() {
#if defined WIN32
    WSACleanup();
#endif
}

// Error handler function
void errorhandler(char *string) {
    printf("%s", string);
}

// Tokenizer function to split input into tokens
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
        printf("Error: Invalid or empty input\n");
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

// Checks if the first character of input is a valid option for password type
int checkChar(char *string) {
    if (string[0] == 'h' ||string[0] == 'n' || string[0] == 'a' || string[0] == 'm' || string[0] == 's'|| string[0] == 'u') {
        return 1;
    }
    return 0;
}

// Validates if input is a valid number and within the range for password length
int isValidNumber(char *string) {
    int length = strlen(string);

    for (int i = 0; i < length; i++) {
        if (!isdigit(string[i])) {
            printf("Entered input is not a number\n");
            return 1;
        }
    }

    int number = atoi(string);

    if (number < 6 || number > 32) {
        printf("Number is out of the allowed range (6-32)\n");
        return 1;
    }

    return 0;
}

void ShowMenu(){
    // Save default color
    unsigned short defaultColor = 7; // White
    unsigned short greenColor = 10; // Green

    printf("Password Generator Help Menu\nCommands:\n");

    SetColor(greenColor);
    printf("h        ");
    SetColor(defaultColor);
    printf(": show this ");
    SetColor(greenColor);
    printf("help menu");
    SetColor(defaultColor);
    printf(" (shows available commands)\n");

    SetColor(greenColor);
    printf("n LENGTH ");
    SetColor(defaultColor);
    printf(": generate ");
    SetColor(greenColor);
    printf("numeric password");
    SetColor(defaultColor);
    printf(" (digits only)\n");

    SetColor(greenColor);
    printf("a LENGTH ");
    SetColor(defaultColor);
    printf(": generate ");
    SetColor(greenColor);
    printf("alphabetic password");
    SetColor(defaultColor);
    printf(" (lowercase letters)\n");

    SetColor(greenColor);
    printf("m LENGTH ");
    SetColor(defaultColor);
    printf(": generate ");
    SetColor(greenColor);
    printf("mixed password");
    SetColor(defaultColor);
    printf(" (lowercase letters and numbers)\n");

    SetColor(greenColor);
    printf("s LENGTH ");
    SetColor(defaultColor);
    printf(": generate ");
    SetColor(greenColor);
    printf("secure password");
    SetColor(defaultColor);
    printf(" (uppercase, lowercase letters, numbers, symbols)\n");

    SetColor(greenColor);
    printf("u LENGTH ");
    SetColor(defaultColor);
    printf(": generate ");
    SetColor(greenColor);
    printf("unambiguous secure password");
    SetColor(defaultColor);
    printf(" (no similar-looking characters)\n");

    SetColor(greenColor);
    printf("q        ");
    SetColor(defaultColor);
    printf(": ");
    SetColor(greenColor);
    printf("quit");
    SetColor(defaultColor);
    printf("  application\n");

    printf("\n");
    SetColor(greenColor);
	printf("LENGTH ");
	SetColor(defaultColor);
    printf("must be between ");
    SetColor(greenColor);
    printf("6");
    SetColor(defaultColor);
    printf(" and ");
    SetColor(greenColor);
	printf("32");
	SetColor(defaultColor);
	printf(" characters\n\n");

    printf("Ambiguous characters excluded in the '");
    SetColor(greenColor);
    printf("u");
    SetColor(defaultColor);
    printf("' option:\n");
    printf("0 O o (zero and letters O)\n");
    printf("1 l I i (one and letters l, I)\n");
    printf("2 Z z (two and letter Z)\n");
    printf("5 S s (five and letter S)\n");
    printf("8 B (eight and letter B)\n");
}


void SetColor(unsigned short color){
	HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hCon,color);
}
