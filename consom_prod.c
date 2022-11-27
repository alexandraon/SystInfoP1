#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define N 8 // places dans le buffer
pthread_mutex_t mutex;
sem_t empty;
sem_t full;
int in=0;
int out=0;
int buffer[N];


// Producteur
void producer(void * nombreExe)
{ 
  int n = *((int *) nombreExe);
  while(n > 0)
  {
        sem_wait(&empty); // attente d'une place libre
        pthread_mutex_lock(&mutex);
        // section critique

        buffer[in] = 1;
        printf("Producer :Insert Item %d at %d\n\n",buffer[in],in);
        in = (in+1)%N;
        for (int i=0; i<10000; i++);
         
        pthread_mutex_unlock(&mutex);
        sem_post(&full); // il y a une place remplie en plus
        n--;
  }
}

// Consommateur
void consumer(void * nombreExe)
{
 int n = *((int *) nombreExe);
 while(n > 0)
 {
   sem_wait(&full); // attente d'une place remplie
   pthread_mutex_lock(&mutex);
    // section critique
    int item = buffer[out];
    printf("Consumer  : Remove Item %d from %d\n\n",item, out);
    buffer[out] = 0;
    out = (out+1)%N;
    for (int i=0; i<10000; i++);
   
    pthread_mutex_unlock(&mutex);
    sem_post(&empty); // il y a une place libre en plus
    n--;
     
    
 }
}

int main(int argc, char const *argv[])
{
    int nombreExe = 8192;

    pthread_mutex_init(&mutex, NULL);
    sem_init(&empty, 0 , N);  // buffer vide
    sem_init(&full, 0 , 0);   // buffer vide


    int pro1= atoi(argv[1]);
    int cons1 = atoi(argv[2]);

    pthread_t pro[pro1],con[cons1]; 
    int nombreThProd = pro1;
    int nombreThCons = cons1;

    int exeThreadProd = nombreExe/nombreThProd;
    int resteProd = nombreExe%nombreThProd;

    int lastProd = exeThreadProd+resteProd;

    int exeThreadConsom = nombreExe/nombreThCons;
    int restConsom = nombreExe%nombreThCons;

    int lastCons = exeThreadConsom+restConsom;


    void *pointerProd1 = &exeThreadProd;
    void *pointerProd2 = &lastProd;

    void *pointerCons1 = &exeThreadConsom;
    void *pointerCons2 = &lastCons;

    for(int i = 0; i < nombreThProd; i++) {
        if (i < nombreThProd-1){
            pthread_create(&pro[i], NULL, (void *)producer, pointerProd1);
        }
        else{
            pthread_create(&pro[i], NULL, (void *)producer, pointerProd2);
        }
    }
    for(int i = 0; i < nombreThCons; i++) {
        if (i < nombreThProd-1){
            pthread_create(&con[i], NULL, (void *)consumer,pointerCons1);
        }
      else{
            pthread_create(&con[i], NULL, (void *)consumer,pointerCons2);
      }
    }

    for(int i = 0; i < nombreThProd; i++) {
        pthread_join(pro[i], NULL);
    }
    for(int i = 0; i < nombreThCons; i++) {
        pthread_join(con[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    sem_destroy(&empty);
    sem_destroy(&full);

    return 0;
}
