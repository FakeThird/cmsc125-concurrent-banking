#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "../include/bank.h"
#include "../include/metrics.h"

// main.c defined
extern int initial_account_count;

void print_accounts_to_file(const char *filename)
{
    FILE *file = fopen(filename, "w");
    if (!file)
    {
        fprintf(stderr, "Error: could not open output file '%s'\n", filename);
        return;
    }

    int final_account_total = 0;
    for (int i = 0; i < bank.num_accounts; i++)
    {
        final_account_total += bank.accounts[i].balance_centavos;
    }

    fprintf(file, "# AccountID  FinalBalanceCentavos\n\n");

    for (int i = 0; i < bank.num_accounts; i++)
    {
        fprintf(file, "%-4d %d\n",
                bank.accounts[i].account_id,
                bank.accounts[i].balance_centavos);
    }

    fprintf(file, "\n# Initial total : %d centavos\n", initial_account_count);
    fprintf(file, "# Final total : %d centavos\n", final_account_total);

    fprintf(file, "# Conservation of money check: %s\n",
            (initial_account_count == final_account_total) ? "PASSED" : "FAILED");

    fclose(file);

    printf("\nInitial total : %d centavos\n", initial_account_count);
    printf("Final total : %d centavos\n", final_account_total);
    printf("Conservation of money check: %s\n",
           (initial_account_count == final_account_total) ? "PASSED" : "FAILED");
    printf("Final account balances written to '%s'\n", filename);
}