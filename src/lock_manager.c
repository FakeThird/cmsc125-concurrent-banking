
#include "../include/lock_manager.h"

#include <pthread.h>
#include <stdbool.h>

WaitForEntry wait_graph[MAX_TRANSACTIONS];
pthread_mutex_t graph_lock;

// When a transaction blocks on a lock, record it
void record_wait(int tx_id, int account_id, int holder_tx) {
    pthread_mutex_lock(&graph_lock);
    wait_graph[tx_id].tx_id = tx_id;
    wait_graph[tx_id].waiting_for_tx = holder_tx;
    wait_graph[tx_id].waiting_for_account = account_id;
    pthread_mutex_unlock(&graph_lock);
}

// DFS-based cycle detection
bool has_cycle(int tx_id, bool* visited, bool* rec_stack) {
    visited[tx_id] = true;
    rec_stack[tx_id] = true;
    
    int next = wait_graph[tx_id].waiting_for_tx;
    if (next != -1) {
        if (!visited[next]) {
            if (has_cycle(next, visited, rec_stack)) {
                return true;
            }
        } else if (rec_stack[next]) {
            return true;  // Cycle detected!
        }
    }
    
    rec_stack[tx_id] = false;
    return false;
}

bool detect_deadlock() {
    pthread_mutex_lock(&graph_lock);
    
    bool visited[MAX_TRANSACTIONS] = {false};
    bool rec_stack[MAX_TRANSACTIONS] = {false};
    
    // Actual Code: for (int i = 0; i < num_active_transactions; i++) {
    for (int i = 0; i < 100; i++) {
        if (!visited[i]) {
            if (has_cycle(i, visited, rec_stack)) {
                pthread_mutex_unlock(&graph_lock);
                return true;
            }
        }
    }
    
    pthread_mutex_unlock(&graph_lock);
    return false;
}