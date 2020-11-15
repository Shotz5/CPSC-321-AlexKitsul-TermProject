// REPURPOSED ASSIGNMENT 1 SHELL FOR USE IN THIS

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/wait.h>
#include<string.h>
#include"shell.h"
#include"disk.h"

#define LGREEN "\x1B[1;32m"
#define YELLOW "\x1B[0;33m"
#define BLUE "\x1B[0;34m"
#define RED "\x1B[0;31m"
#define RESET "\x1B[0m"

// Main driver for application
int main(int argc, char *argv[]) {
    // Begin normal shell
    while(1) {
        printWorkingDir();

        char *input = getUserInput();

        if (input != NULL) {
            char **tokens = parseInput(input);
            if (tokens != NULL) {
                // Theoretically with the way this is designed other syscalls (env, pf, etc.) can occur, unexpected results may occur...
                // ... and were not tested
                // CD is special-case handled solely in C, everything else handled by syscalls

                struct block testBlock;
                for (int i = 0; i < 128; i++) {
                    testBlock.block[i] = 0xaa; // The noise I hear in my hend while programming
                }

                int record = 0;
                struct block readBlock;

                if (strcmp(tokens[0], "disk_write") == 0) {
                    record = disk_write(testBlock);
                } else if (strcmp(tokens[0], "disk_read") == 0) {
                    disk_read(record, &readBlock);
                    for (int i = 0; i < 128; i++) {
                        printf("%02x ", readBlock.block[i]);
                    }
                    printf("\n");
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

// Executes the Syscall supplied by the input with args provided
int exefunc(char **arguments) {

    int pos = 0;
    int overwrite = 0;
    int append = 0;
    int terminal;

    // If > or >> are encountered, obtain file name for output and remove > or >> from arguments
    while(arguments[pos] != NULL) {
        if (strcmp(arguments[pos], ">") == 0) {
            overwrite = 1;
            if (arguments[pos + 1] == NULL) {
                printf("Invalid file name\n");
                return -1;
            }
            // Removes '>'
            free(arguments[pos]);
            arguments[pos] = arguments[pos + 1];
            arguments[pos + 1] = NULL;
            break;
        } else if (strcmp(arguments[pos], ">>") == 0) {
            append = 1;
            if (arguments[pos + 1] == NULL) {
                printf("Invalid file name\n");
                return -1;
            }
            // Removes '>>'
            free(arguments[pos]);
            arguments[pos] = arguments[pos + 1];
            arguments[pos + 1] = NULL;
            break;
        } else {
            pos++;
        }
    }

    // Open or create a file based on input from above, set STDOUT to file
    if (overwrite == 1 || append == 1) {
        int file = -1;
        if (overwrite == 1) {
            file = open(arguments[pos], O_RDWR | O_CREAT | O_TRUNC, 0666);
        } else if (append == 1) {
            file = open(arguments[pos], O_RDWR | O_CREAT | O_APPEND, 0666);
        }

        if (file == -1) {
            printf("Could not open file\n");
            return -1;
        }

        free(arguments[pos]);
        arguments[pos] = NULL;

        terminal = dup(STDOUT_FILENO); // Saves termindal STDOUT filenumber
        dup2(file, STDOUT_FILENO); // Sets STDOUT filenumber to file
        close(file);
    }

    // Fork out and execute, main thread waits for child to finish then continues
    // There is a bug where upon an invalid command, you have to type exit multiple times
    // Only applicable for invalid input though, don't know why
    int status;
    if (fork() == 0) {
        execvp(arguments[0], arguments);
        exit(EXIT_SUCCESS);
    } else {
        wait(&status);
        if (overwrite == 1 || append == 1) {
            dup2(terminal, STDOUT_FILENO);
            close(terminal);
        }
    }
    return 0;
}

// Changes current working directory to one requested by user
int cd(char **arguments) {
    // If no dir given or ~ given, cd to /home
    if (arguments[1] == NULL || strcmp(arguments[1], "~") == 0) {
        if(chdir("/home") != 0) {
            printf("Unknown error, could not change dir");
            return -1;
        }
        return 0;
    }

    // Self explanatory
    if (arguments[2] != NULL) {
        printf("Too many arguments\n");
        return -1;
    }

    // Move to any other dir
    if (chdir(arguments[1]) != 0) {
        printf("Invalid directory. Please try again.\n");
        return -1;
    }

    return 0;
}