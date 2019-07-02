#include "stats.h"
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>


typedef struct {
    int num_factory;
    int num_made;
    int num_eat;
    double min_delay;
    double avg_delay;
    double max_delay;

    double total_delay;
} stats_t;

int total_factory;
sem_t mutex;
stats_t* stats_summary;

void stats_init(int num_producers){
    stats_summary = malloc(num_producers * sizeof(stats_t));

    total_factory = num_producers; // in order to use stats_display()

    for (int i = 0; i < num_producers; i++){
      stats_summary[i].num_factory = i;
      stats_summary[i].num_made = 0;
      stats_summary[i].num_eat = 0;
      stats_summary[i].min_delay = 0;
      stats_summary[i].avg_delay = 0;
      stats_summary[i].max_delay = 0;

      // we need total delay
      stats_summary[i].total_delay = 0;
    }

}

void stats_cleanup(void){
  free(stats_summary);
}

void stats_record_produced(int factory_number){
  sem_wait(&mutex);

  stats_summary[factory_number].num_made += 1;

  sem_post(&mutex);

}

void stats_record_consumed(int factory_number, double delay_in_ms){

  sem_wait(&mutex);

  stats_summary[factory_number].num_eat += 1;

  // First time!
  if(stats_summary[factory_number].max_delay == 0){
    stats_summary[factory_number].min_delay = delay_in_ms;
    stats_summary[factory_number].max_delay = delay_in_ms;
    stats_summary[factory_number].total_delay = delay_in_ms;
  }
  else{
    if(delay_in_ms > stats_summary[factory_number].max_delay){
      stats_summary[factory_number].max_delay = delay_in_ms;
    }
    else if(delay_in_ms < stats_summary[factory_number].min_delay){
      stats_summary[factory_number].min_delay = delay_in_ms;
    }

    stats_summary[factory_number].total_delay += delay_in_ms;
    stats_summary[factory_number].avg_delay = (stats_summary[factory_number].total_delay)/(stats_summary[factory_number].num_eat);
  }

  sem_post(&mutex);

}

void stats_display(void){

  printf("Statistics: \n");

  printf("%8s%10s%10s%15s%15s%15s\n", "Factory#","#Made","#Eaten","Min Delay[ms]","Avg Delay[ms]","Max Delay[ms]");

  for (int i = 0; i < total_factory; i++){
    if(stats_summary[i].num_made != stats_summary[i].num_eat){
      printf("ERROR: Mismatch between number made and eaten.\n");
    }
    else{
      printf("%8d%8d%8d%15.5f%15.5f%15.5f\n", stats_summary[i].num_factory, stats_summary[i] .num_made, stats_summary[i].num_eat, stats_summary[i].min_delay, stats_summary[i].avg_delay,stats_summary[i].max_delay);
    }

  }
  printf("\n");
}
