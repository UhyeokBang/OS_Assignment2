#ifndef SEQLOCK_H
#define SEQLOCK_H

#include <semaphore.h>
#include <stdatomic.h>

typedef struct _seqlock_t
{
    atomic_uint counter; // Atomic counter for sequence number
    sem_t write_lock;    // Semaphore for write lock
} seqlock_t;

void seqlock_init(seqlock_t *seq);
void seqlock_write_lock(seqlock_t *seq);
void seqlock_write_unlock(seqlock_t *seq);
unsigned seqlock_read_begin(seqlock_t *seq);
unsigned seqlock_read_retry(seqlock_t *seq, unsigned cnt);

void seqlock_init(seqlock_t *seq)
{
    atomic_init(&seq->counter, 0);    // Initialize counter to 0
    sem_init(&seq->write_lock, 0, 1); // Initialize write lock
}

void seqlock_write_lock(seqlock_t *seq)
{
    sem_wait(&seq->write_lock);         // Acquire the write lock
    atomic_fetch_add(&seq->counter, 1); // Increment the counter (make it odd)
}

void seqlock_write_unlock(seqlock_t *seq)
{
    atomic_fetch_add(&seq->counter, 1); // Increment the counter (make it even)
    sem_post(&seq->write_lock);         // Release the write lock
}

unsigned seqlock_read_begin(seqlock_t *seq)
{
    return atomic_load(&seq->counter); // Read the counter
}

unsigned seqlock_read_retry(seqlock_t *seq, unsigned cnt)
{
    unsigned current = atomic_load(&seq->counter); // Read the counter again
    // Retry if the counter is different or odd
    return (current != cnt || (current & 1) != 0);
}

#endif // SEQLOCK_H
