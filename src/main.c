/*
 * file: main.c
 */
#include "../include/bank.h"

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
    char trace_file[256]  = "";
    char deadlock[16]  = "detection";
    int  tick = 100;
    bool verbose = false;

    for (int i = 1; i < argc; i++) {
        if (strncmp(argv[i], "--accounts=", 11) == 0) {
            strncpy(accounts_file, argv[i] + 11, sizeof(accounts_file) - 1);
            printf("account file in command line detected.\n");
        } else if (strncmp(argv[i], "--trace=", 8) == 0) {
            strncpy(trace_file, argv[i] + 8, sizeof(trace_file) - 1);
            printf("trace file in command line detected.\n");
        } else if (strncmp(argv[i], "--deadlock=", 11) == 0) {
            strncpy(deadlock, argv[i] + 11, sizeof(deadlock) - 1);
            printf("deadlock in command line detected.\n");
        } else if (strncmp(argv[i], "--tick-ms=", 10) == 0) {
            tick = atoi(argv[i] + 10);
            if (tick <= 0) {
                fprintf(stderr, "Error: Tick speed must be a positive integer.\n");
                return 1;
            }
            printf("tick input in command line detected.\n");
        } else if (strncmp(argv[i], "--verbose", 9) == 0) {
            verbose = true;
            printf("verbose input in command line detected.\n");
        } else {
            fprintf(stderr, "Unknown/Missing argument(s): %s\n", argv[i]);
            return 1;
        }
    }
 
    int num_accounts = 0;
    Account *accounts = load_accounts(accounts_file, &num_accounts);
    if (accounts == NULL) {
        printf("account loading did not occur.\n");
        return 1;
    } else {
        printf("account loading successful.\n");
    }

    bank.num_accounts = num_accounts;
    for(int i = 0; i < num_accounts; i++) {
        bank.accounts[i] = accounts[i];
    }

    FILE *trace = fopen(trace_file, "r");
    if (!trace) {
        printf("trace file not found.\n");
        return 1;
    } else {
        printf("trace file found.\n");
    }
    
    if (!(strncmp(deadlock, "detection", 16) == 0 || strncmp(deadlock, "prevention", 16) == 0)) {
        printf("deadlock strategy not avaialble.\n");
        return 1;
    } else {
        printf("deadlock strategy avaialble.\n");
    }
    return 1;
}