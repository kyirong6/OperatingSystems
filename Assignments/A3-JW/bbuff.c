#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdbool.h>

#define BUFFER_SIZE 10000

void *buff[BUFFER_SIZE];

int front;
int rear;

sem_t mutex; // Protects accesses to buf
sem_t slots; // Counts available empty slots
sem_t items; // Counts available items

// implement API
void bbuff_init(void)
{

  front = 0;
  rear = 0;

  sem_init(&mutex, 0, 1);           // Binary semaphore for locking
  sem_init(&slots, 0, BUFFER_SIZE); // Initially, buf has n empty slots
  sem_init(&items, 0, 0);           // Initially, buf has zero data items

  return;
}

void bbuff_blocking_insert(void *item)
{
  sem_wait(&slots); // Wait for available slot
  sem_wait(&mutex);

  buff[(++rear) % BUFFER_SIZE] = item;

  sem_post(&mutex);
  sem_post(&items); // Announce available item
}
void *bbuff_blocking_extract(void)
{

  sem_wait(&items); // Wait for available item
  sem_wait(&mutex);

  void *candy = buff[(++front) % BUFFER_SIZE];

  sem_post(&mutex);
  sem_post(&slots); // Announce available slot
  return candy;
}
_Bool bbuff_is_empty(void)
{
  return (front == rear);
}
