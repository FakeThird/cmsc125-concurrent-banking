#include <stdlib.h>
#include <string.h> 
#include "../include/transaction.h"
#include "../include/timer.h"
#include "../include/bank.h"

#include <stdio.h>

void* execute_transaction(void* arg) {
    Transaction* tx = (Transaction*)arg;
    
    // Wait until scheduled start time
    wait_until_tick(tx->start_tick);
    
    tx->actual_start = global_tick;
    
    for (int i = 0; i < tx->num_ops; i++) {
        Operation* op = &tx->ops[i];
        
        int tick_before = global_tick;
        
        switch (op->type) {
            case OP_DEPOSIT:
                deposit(op->account_id, op->amount_centavos);
                break;
                
            case OP_WITHDRAW:
                if (!withdraw(op->account_id, op->amount_centavos)) {
                    // Insufficient funds - abort transaction
                    tx->status = TX_ABORTED;
                    return NULL;
                }
                break;
                
            case OP_TRANSFER:
                if (!transfer(op->account_id, op->target_account,
                              op->amount_centavos)) {
                    tx->status = TX_ABORTED;
                    return NULL;
                }
                break;
                
            case OP_BALANCE:
                int balance = get_balance(op->account_id);
                printf("T%d: Account %d balance = PHP %d.%02d\n", 
                       tx->tx_id, op->account_id, 
                       balance / 100, balance % 100);
                break;
        }
        
        tx->wait_ticks += (global_tick - tick_before);
    }
    
    tx->actual_end = global_tick;
    tx->status = TX_COMMITTED;
    return NULL;
}

Transaction *load_transactions(const char *filename, int *num_transactions)
{
    int capacity = 4;
    int count = 0;
    Transaction *transactions = malloc(capacity * sizeof(Transaction));
    if (!transactions)
        return NULL;

    FILE *file = fopen(filename, "r");
    if (!file)
    {
        free(transactions);
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

        char tx_label[16], op_str[16];
        int start_tick, account_id;

        if (sscanf(p, "%15s %d %15s %d", tx_label, &start_tick, op_str, &account_id) < 4)
            continue;

        int tx_id = atoi(tx_label + 1);  // "T3" -> 3

        // Find existing transaction with this tx_id
        Transaction *tx = NULL;
        for (int i = 0; i < count; i++)
        {
            if (transactions[i].tx_id == tx_id)
            {
                tx = &transactions[i];
                break;
            }
        }

        // New transaction ID — allocate a slot
        if (tx == NULL)
        {
            if (count >= capacity)
            {
                capacity *= 2;
                Transaction *tmp = realloc(transactions, capacity * sizeof(Transaction));
                if (!tmp)
                {
                    free(transactions);
                    fclose(file);
                    return NULL;
                }
                transactions = tmp;
            }
            tx = &transactions[count];
            tx->tx_id      = tx_id;
            tx->start_tick = start_tick;
            tx->num_ops    = 0;
            tx->status     = TX_RUNNING;
            tx->wait_ticks = 0;
            count++;
            print_transaction(tx); 
        }

        if (tx->num_ops >= 256)
        {
            fprintf(stderr, "Warning: T%d exceeded max ops, skipping line\n", tx_id);
            continue;
        }

        Operation *op = &tx->ops[tx->num_ops];
        op->account_id      = account_id;
        op->amount_centavos = 0;
        op->target_account  = -1;

        if (strcmp(op_str, "DEPOSIT") == 0)
        {
            sscanf(p, "%*s %*d %*s %*d %d", &op->amount_centavos);
            op->type = OP_DEPOSIT;
        }
        else if (strcmp(op_str, "WITHDRAW") == 0)
        {
            sscanf(p, "%*s %*d %*s %*d %d", &op->amount_centavos);
            op->type = OP_WITHDRAW;
        }
        else if (strcmp(op_str, "TRANSFER") == 0)
        {
            sscanf(p, "%*s %*d %*s %*d %d %d", &op->target_account, &op->amount_centavos);
            op->type = OP_TRANSFER;
        }
        else if (strcmp(op_str, "BALANCE") == 0)
        {
            op->type = OP_BALANCE;
        }
        else
        {
            fprintf(stderr, "Warning: unknown op '%s' on T%d, skipping\n", op_str, tx_id);
            continue;
        }

        tx->num_ops++;
    }

    fclose(file);
    *num_transactions = count;
    return transactions;
}

void print_transaction(const Transaction *tx)
{
    const char *status_str[] = { "RUNNING", "COMMITTED", "ABORTED" };

    printf("T%d | start_tick: %d | ops: %d | status: %s\n",
           tx->tx_id,
           tx->start_tick,
           tx->num_ops,
           status_str[tx->status]);

    for (int i = 0; i < tx->num_ops; i++)
    {
        const Operation *op = &tx->ops[i];
        switch (op->type)
        {
            case OP_DEPOSIT:
                printf("  [%d] DEPOSIT  account %d  amount %d\n",
                       i, op->account_id, op->amount_centavos);
                break;
            case OP_WITHDRAW:
                printf("  [%d] WITHDRAW account %d  amount %d\n",
                       i, op->account_id, op->amount_centavos);
                break;
            case OP_TRANSFER:
                printf("  [%d] TRANSFER account %d -> %d  amount %d\n",
                       i, op->account_id, op->target_account, op->amount_centavos);
                break;
            case OP_BALANCE:
                printf("  [%d] BALANCE  account %d\n",
                       i, op->account_id);
                break;
        }
    }
}