#ifndef TIMER_H
#define TIMER_H

#define MAX_ACCOUNTS 100

void* timer_thread(void* arg);
void wait_until_tick(int target_tick);

#endif