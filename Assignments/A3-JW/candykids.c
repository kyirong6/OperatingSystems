#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <assert.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include "bbuff.h"
#include "stats.h"

_Bool stop_thread = false;

// Candy
typedef struct
{
    int factory_number;
    double time_stamp_in_ms; // tracks when the item was created
} candy_t;

// get the current number of milliseconds
double current_time_in_ms(void)
{
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    return now.tv_sec * 1000.0 + now.tv_nsec / 1000000.0;
}

void *factoryLaunch(void *arg)
{
    int factory_number = *((int *)arg);

    while (!stop_thread)
    {
        int seconds_wait = rand() % 4;
        printf("\tFactory %d ships candy & waits %ds\n", factory_number, seconds_wait);
        candy_t *candy = malloc(sizeof(candy_t));
        candy->factory_number = factory_number;
        candy->time_stamp_in_ms = current_time_in_ms();

        bbuff_blocking_insert(candy);
        stats_record_produced(factory_number);
        sleep(seconds_wait);
    }
    printf("Candy-factory %d done\n", factory_number);

    pthread_exit(NULL);
}

void *kidLaunch(void *arg)
{
    while (1)
    {
        candy_t *candy = bbuff_blocking_extract();
        if (candy != NULL)
        {
            stats_record_consumed(candy->factory_number, current_time_in_ms() - candy->time_stamp_in_ms);
        }
        free(candy);
        candy = NULL; // Avoid still reachable
        sleep(rand() % 2);
    }
}

int main(int argc, char *argv[])
{
    // 1. Extract arguments
    srand((unsigned)time(NULL));
    int factory_num = 0;
    int kid_num = 0;
    int seconds = 0;
    if (argc == 4)
    {
        factory_num = atoi(argv[1]);
        kid_num = atoi(argv[2]);
        seconds = atoi(argv[3]);
    }
    if (factory_num <= 0 || kid_num <= 0 || seconds <= 0)
    {

        printf("Error: All arguments must be positive.\n");
        exit(-1);
    }
    else
    {
        printf("Invalid Input\n");
        exit(-1);
    }

    // 2. Initialize modules
    bbuff_init();
    stats_init(factory_num);

    // 3. Launch candy-factory threads
    pthread_t factories_tids[factory_num];
    int factory_array[factory_num];
    for (int i = 0; i < factory_num; i++)
    {
        factory_array[i] = i;

        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_create(&factories_tids[i], &attr, factoryLaunch, &factory_array[i]);
    }

    // 4. Launch kid threads
    pthread_t kids_tids[kid_num];
    for (int i = 0; i < kid_num; i++)
    {

        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_create(&kids_tids[i], &attr, kidLaunch, NULL);
    }

    // 5. Wait for requested time
    for (int i = 0; i < seconds; i++)
    {
        printf("Time %ds\n", i);
        sleep(1);
    }

    // 6. Stop candy-factory threads
    printf("Stopping candy factories...\n");
    stop_thread = true;

    for (int i = 0; i < factory_num; i++)
    {
        pthread_join(factories_tids[i], NULL);
    }

    // 7. Wait until no more candy
    //
    while (!bbuff_is_empty())
    {
        printf("Waiting for all candy to be consumed\n");
        sleep(1);
    }

    // 8. Stop kid threads
    printf("Stopping kids.\n");
    for (int i = 0; i < kid_num; i++)
    {
        pthread_cancel(kids_tids[i]);
        pthread_join(kids_tids[i], NULL);
    }

    // 9. Print statistics
    stats_display();

    // 10. Cleanup any allocated memory
    stats_cleanup();

    return 0;
}
