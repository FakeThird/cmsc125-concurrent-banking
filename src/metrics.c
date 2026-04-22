#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "../include/bank.h"
#include "../include/metrics.h"

void print_accounts_to_file(const char *filename)
{
    FILE *file = fopen(filename, "w");
    if (!file)
    {
        fprintf(stderr, "Error: could not open output file '%s'\n", filename);
        return;
    }

    fprintf(file, "# AccountID  FinalBalanceCentavos\n\n");

    for (int i = 0; i < bank.num_accounts; i++)
    {
        fprintf(file, "%-4d %d\n",
                bank.accounts[i].account_id,
                bank.accounts[i].balance_centavos);
    }

    fclose(file);
    printf("Final account balances written to '%s'\n", filename);
}