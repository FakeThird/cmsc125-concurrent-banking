/*
 * file: main.c
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>


int main(int argc, char *argv[])
{
    // Command-line Options:
    // --accounts=FILE: Initial account balances
    // --trace=FILE: Transaction workload
    // --deadlock=prevention|detection: Deadlock strategy
    // --tick-ms=N: Milliseconds per tick (default: 100)
    // --verbose: Print detailed operation logs
    char accounts_file[256] = "";

    for (int i = 1; i < argc; i++) {
        if (strncmp(argv[i], "--accounts=", 12) == 0) {
            strncpy(accounts_file, argv[i] + 12, sizeof(accounts_file) - 1);
            printf("account in command line detected.");
        } 
    }

    FILE *accounts = fopen(accounts_file, "r");
    if (!accounts_file) {
        printf("account file not found.");
        return 1;
    } else {
        printf("account file found.");
    }
    
    return 1;
}