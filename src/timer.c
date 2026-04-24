#define _POSIX_C_SOURCE 200809L
#include "../include/timer.h"

#include <pthread.h>

volatile int global_tick = 0;
volatile int simulation_running = 0;
int tick_interval_ms = 100;
pthread_mutex_t tick_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t tick_changed = PTHREAD_COND_INITIALIZER;

// Timer thread increments clock every TICK_INTERVAL_MS
void *timer_thread(void *arg)
{
    // Supress for now
    (void)arg;
    while (simulation_running)
    {
        usleep(tick_interval_ms * 1000); // Sleep to simulate a tick

        pthread_mutex_lock(&tick_lock);
        global_tick++;

        // Wake all transactions waiting for this tick
        pthread_cond_broadcast(&tick_changed); // Wake waiting

        pthread_mutex_unlock(&tick_lock);
    }
    return NULL;
}

// Transactions wait until their start_tick
void wait_until_tick(int target_tick)
{
    pthread_mutex_lock(&tick_lock);
    while (global_tick < target_tick)
    {
        pthread_cond_wait(&tick_changed, &tick_lock);
    }
    pthread_mutex_unlock(&tick_lock);
}

int get_tick(void)
{
    pthread_mutex_lock(&tick_lock);
    int t = global_tick;
    pthread_mutex_unlock(&tick_lock);
    return t;
}