#ifndef RWWLOCK_H
#define RWWLOCK_H

#include <semaphore.h>

typedef struct _rwwlock_t
{
    sem_t read_lock;  // Semaphore to control access to reader count
    sem_t write_lock; // Semaphore to control access to the critical section
    sem_t entry_lock; // Semaphore to prevent new readers when a writer is waiting
    int readers;      // Number of readers currently accessing the resource
} rwwlock_t;

void rwwlock_init(rwwlock_t *rww);
void rwwlock_acquire_writelock(rwwlock_t *rww);
void rwwlock_release_writelock(rwwlock_t *rww);
void rwwlock_acquire_readlock(rwwlock_t *rww);
void rwwlock_release_readlock(rwwlock_t *rww);

void rwwlock_init(rwwlock_t *rww)
{
    sem_init(&rww->read_lock, 0, 1);  // Binary semaphore
    sem_init(&rww->write_lock, 0, 1); // Binary semaphore
    sem_init(&rww->entry_lock, 0, 1); // Binary semaphore
    rww->readers = 0;
}

void rwwlock_acquire_writelock(rwwlock_t *rww)
{
    sem_wait(&rww->entry_lock); // Prevent new readers
    sem_wait(&rww->write_lock); // Acquire write lock
}

void rwwlock_release_writelock(rwwlock_t *rww)
{
    sem_post(&rww->write_lock); // Release write lock
    sem_post(&rww->entry_lock); // Allow new readers
}

void rwwlock_acquire_readlock(rwwlock_t *rww)
{
    sem_wait(&rww->entry_lock); // Ensure no new writers
    sem_post(&rww->entry_lock); // Allow other readers to enter

    sem_wait(&rww->read_lock); // Lock to update readers count
    rww->readers++;
    if (rww->readers == 1)
    {
        sem_wait(&rww->write_lock); // First reader locks the write lock
    }
    sem_post(&rww->read_lock); // Unlock after updating readers count
}

void rwwlock_release_readlock(rwwlock_t *rww)
{
    sem_wait(&rww->read_lock); // Lock to update readers count
    rww->readers--;
    if (rww->readers == 0)
    {
        sem_post(&rww->write_lock); // Last reader unlocks the write lock
    }
    sem_post(&rww->read_lock); // Unlock after updating readers count
}

#endif // RWWLOCK_H
