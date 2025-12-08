#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function to handle errors
void send_error(const char* message) {
    fprintf(stderr, "Error: %s\n", message);
    exit(EXIT_FAILURE);
}

// Function to display info messages
void send_info(const char* message) {
    printf("%s\n", message);
}