  ///AAA
  #include <pthread.h>
  #include <stdio.h>
  #include <stdlib.h>
  #include <stdbool.h>
  #include <unistd.h>

  typedef struct argument
  {
    pthread_mutex_t ** baguette;
    int * N;
    int * positionDuPhil;
    
  }argument;

  void mange(int id) {
  printf("Philosophe [%d] mange\n",id);
  }

  void* philosophe ( void* myStructure )
  {
    argument * arge =  myStructure;
    int PHILOSOPHES = *(arge->N);
    int position = *(arge->positionDuPhil);
    pthread_mutex_t * baguette = *(arge->baguette);

    int left = position;
    int right = (left + 1) % PHILOSOPHES;
    int nbrOfCycles=2; // 100000

    while(nbrOfCycles) {
        
        if(left<right) {
        printf("Philosophe [%d] pense\n",position);
        pthread_mutex_lock(&baguette[left]);
        printf("Philosophe [%d] possède baguette gauche [%d]\n",position,left);
        pthread_mutex_lock(&baguette[right]);
        printf("Philosophe [%d] possède baguette droite [%d]\n",position,right);
        }
        else {
        printf("Philosophe [%d] pense\n",position);
        pthread_mutex_lock(&baguette[right]);
        pthread_mutex_lock(&baguette[left]);
        }
        mange(position);
        pthread_mutex_unlock(&baguette[left]);
        printf("Philosophe [%d] a libéré baguette gauche [%d]\n",position,left);
        pthread_mutex_unlock(&baguette[right]);
        printf("Philosophe [%d] a libéré baguette droite [%d]\n",position,right);
        nbrOfCycles--;
    }
    free(arge->positionDuPhil);
    free(arge);
    pthread_exit(NULL);
    return (NULL);
  }

  int main ( int argc, char *argv[])
  {

  if (argc != 2)
    {
        printf("Need 1 arguments");
        return EXIT_FAILURE;
    }

    int PHILOSOPHES = atoi(argv[1]);     
    printf("This is the number of Philosopher %d\n", PHILOSOPHES);

    pthread_t * phil = (pthread_t *) malloc(sizeof(pthread_t *) * PHILOSOPHES);
    pthread_mutex_t * baguette = (pthread_mutex_t*) malloc(sizeof(pthread_mutex_t) * (PHILOSOPHES));
    for (int i = 0; i < PHILOSOPHES; i++)
    {
        argument * lePhilo = (argument *) malloc(sizeof(lePhilo));

        int * positionCopy = (int *) malloc(sizeof(int));
        *positionCopy = i;

        lePhilo->baguette = &baguette;
        lePhilo->N = &PHILOSOPHES;
        lePhilo->positionDuPhil = positionCopy;

        pthread_create(&phil[i], NULL, philosophe, lePhilo);        
    }

    for (int i = 0; i < PHILOSOPHES; i++)
        pthread_join(phil[i], NULL);
    
    free(phil);
    free(baguette);
  return (EXIT_SUCCESS);
  }
