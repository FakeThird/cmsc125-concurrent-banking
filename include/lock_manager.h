#ifndef LOCKMNGR_H
#define LOCKMNGR_H

typedef struct {
    int tx_id;
    int waiting_for_tx;  // -1 if not waiting
    int waiting_for_account;
} WaitForEntry;

#endif