
#include "../include/buffer_pool.h"
#include "../include/utils.h"

#include <stdbool.h>
#include <stdio.h>

BufferPool buffer_pool;

void init_buffer_pool(BufferPool *pool)
{
    for (int i = 0; i < BUFFER_POOL_SIZE; i++)
    {
        pool->slots[i].in_use     = false;
        pool->slots[i].account_id = -1;
        pool->slots[i].data       = NULL;
    }

    sem_init(&pool->empty_slots, 0, BUFFER_POOL_SIZE);
    sem_init(&pool->full_slots, 0, 0);
    pthread_mutex_init(&pool->pool_lock, NULL);
}

// Load account into buffer pool (producer)
void load_account(BufferPool *pool, int account_id)
{
    sem_wait(&pool->empty_slots); // Wait for empty slot

    pthread_mutex_lock(&pool->pool_lock);

    // Find empty slot and load account
    for (int i = 0; i < BUFFER_POOL_SIZE; i++)
    {
        if (!pool->slots[i].in_use)
        {
            pool->slots[i].account_id = account_id;
            pool->slots[i].data = &bank.accounts[account_id];
            pool->slots[i].in_use = true;
            break;
        }
    }

    pthread_mutex_unlock(&pool->pool_lock);

    sem_post(&pool->full_slots); // Signal slot is full
}

// Unload account from buffer pool (consumer)
void unload_account(BufferPool *pool, int account_id)
{
    sem_wait(&pool->full_slots); // Wait for full slot

    pthread_mutex_lock(&pool->pool_lock);

    // Find and unload account
    for (int i = 0; i < BUFFER_POOL_SIZE; i++)
    {
        if (pool->slots[i].in_use &&
            pool->slots[i].account_id == account_id)
        {
            pool->slots[i].in_use = false;
            pool->slots[i].account_id = -1;
            break;
        }
    }

    pthread_mutex_unlock(&pool->pool_lock);

    sem_post(&pool->empty_slots); // Signal slot is empty
}

// // Get account from buffer pool (with caching)
// Account *buffer_get_account(BufferPool *pool, int account_id)
// {
//     pthread_mutex_lock(&pool->pool_lock);

//     // Check if account is already in pool
//     for (int i = 0; i < BUFFER_POOL_SIZE; i++)
//     {
//         if (pool->slots[i].in_use && pool->slots[i].account_id == account_id)
//         {
//             pthread_mutex_unlock(&pool->pool_lock);
//             return pool->slots[i].data;
//         }
//     }

//     pthread_mutex_unlock(&pool->pool_lock);

//     // Account not in pool - load it
//     sem_wait(&pool->empty_slots); // Wait for empty slot

//     pthread_mutex_lock(&pool->pool_lock);

//     // Find empty slot and load account
//     for (int i = 0; i < BUFFER_POOL_SIZE; i++)
//     {
//         if (!pool->slots[i].in_use)
//         {
//             pool->slots[i].account_id = account_id;
//             pool->slots[i].data = &bank.accounts[account_id];
//             pool->slots[i].in_use = true;
//             Account *result = pool->slots[i].data;
//             pthread_mutex_unlock(&pool->pool_lock);
//             sem_post(&pool->full_slots); // Signal slot is full
//             return result;
//         }
//     }

//     // Should not reach here if semaphores are correct
//     pthread_mutex_unlock(&pool->pool_lock);
//     return NULL;
// }

// // Release account from buffer pool
// void buffer_release_account(BufferPool *pool, int account_id)
// {
//     pthread_mutex_lock(&pool->pool_lock);

//     // Find and unload account
//     for (int i = 0; i < BUFFER_POOL_SIZE; i++)
//     {
//         if (pool->slots[i].in_use &&
//             pool->slots[i].account_id == account_id)
//         {
//             pool->slots[i].in_use = false;
//             pool->slots[i].account_id = -1;
//             break;
//         }
//     }

//     pthread_mutex_unlock(&pool->pool_lock);

//     sem_post(&pool->empty_slots); // Signal slot is empty
// }