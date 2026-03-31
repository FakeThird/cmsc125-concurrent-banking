
#include "../include/bank.h"

#include <stdbool.h>

int get_balance(int account_id) {
    Account* acc = &bank.accounts[account_id];
    
    pthread_rwlock_rdlock(&acc->lock);
    int balance = acc->balance_centavos;
    pthread_rwlock_unlock(&acc->lock);
    
    return balance;
}

void deposit(int account_id, int amount_centavos) {
    Account* acc = &bank.accounts[account_id];
    
    pthread_rwlock_wrlock(&acc->lock);
    acc->balance_centavos += amount_centavos;
    pthread_rwlock_unlock(&acc->lock);
}

bool withdraw(int account_id, int amount_centavos) {
    Account* acc = &bank.accounts[account_id];
    
    pthread_rwlock_wrlock(&acc->lock);
    
    if (acc->balance_centavos < amount_centavos) {
        pthread_rwlock_unlock(&acc->lock);
        return false;  // Insufficient funds
    }
    
    acc->balance_centavos -= amount_centavos;
    pthread_rwlock_unlock(&acc->lock);
    return true;
}

bool transfer(int from_id, int to_id, int amount_centavos) {
    // This is where deadlock can occur!
    // See Part 3 for proper implementation
}