
#define _POSIX_C_SOURCE 200809L
#include "../include/bank.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int get_balance(int account_id)
{
    Account *acc = &bank.accounts[account_id];

    pthread_rwlock_rdlock(&acc->lock);
    int balance = acc->balance_centavos;
    pthread_rwlock_unlock(&acc->lock);

    return balance;
}

void deposit(int account_id, int amount_centavos)
{
    Account *acc = &bank.accounts[account_id];

    pthread_rwlock_wrlock(&acc->lock);
    acc->balance_centavos += amount_centavos;
    pthread_rwlock_unlock(&acc->lock);
}

bool withdraw(int account_id, int amount_centavos)
{
    Account *acc = &bank.accounts[account_id];

    pthread_rwlock_wrlock(&acc->lock);

    if (acc->balance_centavos < amount_centavos)
    {
        pthread_rwlock_unlock(&acc->lock);
        return false; // Insufficient funds
    }

    acc->balance_centavos -= amount_centavos;
    pthread_rwlock_unlock(&acc->lock);
    return true;
}

bool transfer(int from_id, int to_id, int amount_centavos)
{
    // This is where deadlock can occur!
    // See Part 3 for proper implementation
    Account *a = find_account(from_id);
    Account *b = find_account(to_id);
    if (!a || !b){
        return false; // Invalid accounts
    }
    Account *first = (from_id < to_id) ? a : b;
    Account *second = (from_id < to_id) ? b : a;

    pthread_rwlock_wrlock(&first->lock);
    pthread_rwlock_wrlock(&second->lock);

    bool success = true;
    if (first->balance_centavos < amount_centavos)
    {
        pthread_rwlock_unlock(&first->lock);
        pthread_rwlock_unlock(&second->lock);
        success = false; // Insufficient funds
    }

    first->balance_centavos -= amount_centavos;
    second->balance_centavos += amount_centavos;
    return success;
}

Account *load_accounts(const char *filename, int *num_accounts)
{
    int capacity = 4;
    int count = 0;
    Account *accounts = malloc(capacity * sizeof(Account));
    if (!accounts)
        return NULL;

    FILE *file = fopen(filename, "r");
    if (!file)
    {
        free(accounts);
        return NULL;
    }
    char line[MAX_LINE];
    while (fgets(line, MAX_LINE, file))
    {

        char *p = line;
        while (*p == ' ' || *p == '\t')
            p++;
        if (*p == '#' || *p == '\n' || *p == '\r' || *p == '\0')
            continue;

        if (count >= capacity)
        {
            capacity *= 2;
            Account *tmp = realloc(accounts, capacity * sizeof(Account));
            if (!tmp)
            {
                free(accounts);
                fclose(file);
                return NULL;
            }
            accounts = tmp;
        }
        if (sscanf(p, "%d %d",
                   &accounts[count].account_id,
                   &accounts[count].balance_centavos) == 2)
        {
            print_account(&accounts[count]);
            count++;
        }
    }
    fclose(file);
    *num_accounts = count;
    return accounts;
} 

void print_account(Account* account)
{
    printf("%d \n", account -> account_id);
    printf("%d \n", account -> balance_centavos);
}