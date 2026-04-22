#ifndef TIMER_H
#define TIMER_H

#define MAX_ACCOUNTS 100

extern volatile int global_tick;
extern volatile int all_transactions_done;

void* timer_thread(void* arg);
void wait_until_tick(int target_tick);

#endif