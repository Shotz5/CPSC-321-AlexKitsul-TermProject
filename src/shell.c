// CODE SOURCED FROM MY ASSIGNMENT 1 PROJECT, HEAVILY MODIFIED
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/wait.h>
#include<string.h>
#include"shell.h"
#include"disk.h"

// Main driver for application
int main(int argc, char *argv[]) {
    
    // Parition our 'hard drive'
    partition();

    // Begin normal shell
    while(1) {
        printWorkingDir();

        char *input = getUserInput();

        if (input != NULL) {
            char **tokens = parseInput(input);
            if (tokens != NULL) {
                if (strcmp(tokens[0], "make_file") == 0) {
                    make_file();
                } else if (strcmp(tokens[0], "write_file") == 0) {
                    write_file(tokens[1], atoi(tokens[2]));
                    printf("Wrote %s to file %s\n", tokens[1], tokens[2]);
                } else if (strcmp(tokens[0], "delete_file") == 0) {
                    delete_file(atoi(tokens[1]));
                }

                // Free everything we did out of RAM
                free(input);
                int pos = 0;
                while(tokens[pos] != NULL) {
                    free(tokens[pos]);
                    pos++;
                }
                free(tokens);
            }
        }
    }
}

// Print current working dir to UI
int printWorkingDir() {
    char *cwd = getcwd(NULL, 0);
    if (cwd == NULL) {
        printf("Critical unknown error, could not get working dir\n");
        exit(EXIT_FAILURE);
    }
    printf(YELLOW "Alex's " BLUE "Hacker " RED "Shell " LGREEN "%s" RESET " > ", cwd);
    free(cwd);
    return 0;
}

// Take in typed line from user, return pointer to typed line
char *getUserInput() {
    char *input = NULL;
    size_t buffer = 0; // Getline automatically allocates memory, but needs a RAM address for it to find that out for some reason

    if(getline(&input, &buffer, stdin) == -1) {
        printf("Getline error\n");
        return NULL;
    }

    // Checks if input is special case that doesn't need to proceed further
    if (strcmp(input, "exit\n") == 0) {
        free(input);
        exit(EXIT_SUCCESS);
    } else if (strcmp(input, "\n") == 0) {
        free(input);
        return NULL;
    }

    return input;
}

// Parse out spaces and quotes for exec to read, returns double pointer to arrays of 'words' null terminated
char **parseInput(char *input) {
    int words = count(input);

    if (words == -1) {
        printf("Unable to match delimiters\n");
        return NULL;
    }

    char **tokens = malloc((words + 1) * (sizeof(char*)));
    if (!tokens) {
        printf("Could not allocate memory. Critical Error\n");
        exit(EXIT_SUCCESS);
    }
    
    int start = 0;
    int pos = 0;
    int index = 0;

    // Because strtok doesn't work with quotation marks I had to do this dog U+1F4A9
    while(input[pos] != '\n') {
        // Finds beginning and end of quote section
        if (input[pos] == '"') {
            start++;
            pos++;
            while(input[pos] != '"') {
                pos++;
            }
        // " of space-seperated section
        } else {
            while(input[pos] != ' ' && input[pos] != '\n') {
                pos++;
            }
        }

        // Build char array for word
        char *container = malloc(((pos - start) + 1) * sizeof(char));
        for (int i = start; i < pos; i++) {
            container[i - start] = input[i];
        }
        container[pos - start] = '\0';

        tokens[index] = container;
        index++;

        // Check for end of line and ensure skip over end quote, mark next start at prev end
        if (input[pos] != '\n' && input[pos] != '"') {
            pos++;
        } else if (input[pos] == '"' && input[pos + 1] != '\n') {
            pos += 2;
        } else if (input[pos] == '"' && input[pos + 1] == '\n') {
            pos++;
        }

        start = pos;
    }

    // NULL terminate tokens double pointer
    tokens[index] = NULL;

    return tokens;
}


// Count number of 'words' seperated by spaces in input for proper memory alloc, items in quotes are considered one word
int count(char *input) {
    int words = 1;
    int pos = 0;

    while(input[pos] != '\n') {
        if (input[pos] == ' ') {
            words++;
        } else if (input[pos] == '"') {
            pos++;
            while(input[pos] != '"') {
                if (input[pos] == '\n') {
                    free(input);
                    return -1;
                }
                pos++;
            }
            words++;
        }
        pos++;
    }

    return words;
}