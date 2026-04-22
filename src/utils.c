#include <stdio.h>
#include <stdlib.h>
#include "../include/bank.h"

Account *find_account(int account_id)
{
    for (int i = 0; i < bank.num_accounts; i++)
    {
        if (bank.accounts[i].account_id == account_id)
            return &bank.accounts[i];
    }
    return NULL;
}