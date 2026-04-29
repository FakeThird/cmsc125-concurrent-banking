#ifndef LOCKMNGR_H
#define LOCKMNGR_H

#include <stdbool.h>

#define MAX_TRANSACTIONS 100

typedef struct
{
    int tx_id;
    int waiting_for_tx; // -1 if not waiting
    int waiting_for_account;
} WaitForEntry;

extern int num_active_transactions;

void record_wait(int tx_id, int account_id, int holder_tx);
bool has_cycle(int tx_id, bool *visited, bool *rec_stack);
bool detect_deadlock();

#endif