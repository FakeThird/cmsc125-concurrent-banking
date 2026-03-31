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
    char trace_file[256]  = "";
    int  tick = 100;

    for (int i = 1; i < argc; i++) {
        if (strncmp(argv[i], "--accounts=", 11) == 0) {
            strncpy(accounts_file, argv[i] + 11, sizeof(accounts_file) - 1);
            printf("account file in command line detected.");
        } else if (strncmp(argv[i], "--trace=", 7) == 0) {
            strncpy(trace_file, argv[i] + 7, sizeof(trace_file) - 1);
            printf("trace file in command line detected.");
        } else if (strncmp(argv[i], "--tick-ms=", 10) == 0) {
            tick = atoi(argv[i] + 10);
            if (tick <= 0) {
                fprintf(stderr, "Error: Tick speed must be a positive integer.\n");
                return 1;
            }
            printf("tick input in command line detected.");
        } else {
            fprintf(stderr, "Unknown/Missing argument(s): %s\n", argv[i]);
            return 1;
        }
    }

    FILE *accounts = fopen(accounts_file, "r");
    if (!accounts) {
        printf("account file not found.");
        return 1;
    } else {
        printf("account file found.");
    }

    FILE *trace = fopen(trace_file, "r");
    if (!trace) {
        printf("trace file not found.");
        return 1;
    } else {
        printf("trace file found.");
    }
    
    
    return 1;
}