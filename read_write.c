    #include <pthread.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <stdbool.h>
    #include <unistd.h>
    #include <semaphore.h>


    pthread_mutex_t mutex_readcount; //protege readCount
    pthread_mutex_t mutex_writecount; // protege writeCount
    pthread_mutex_t z; // Pour bloque d'abord le deuxieme readers pour qu'il execute une seule fois la partie line 55-67.

    sem_t wsem;  // exclusif acces for database
    sem_t rsem; // to Block readers

    int readcount = 0; // nombre de readers variable partagé
    int writecount = 0; // nombre de readers variable partagé

    typedef struct read_write
    {
        int *position;
        void *numbrExe;
    }read_write;
    

    void writer(void *arg){
        read_write * arge = arg;
        int position  = *(arge->position);
        int numbrExe = *(int *)(arge->numbrExe);
        while(numbrExe > 0){
            //prepare_data();
            printf("Writer %d is trying to enter into database for modifying the data\n",position);
            pthread_mutex_lock(&mutex_writecount);
                //section critique
                writecount +=1;
                if (writecount == 1)
                {
                    sem_wait(&rsem);
                }
            pthread_mutex_unlock(&mutex_writecount);

            sem_wait(&wsem);
            // section critique, un seul writer à la fois
            printf("Writer %d is writing into the database\n",position);
            for (int i=0; i<10000; i++);
            sem_post(&wsem);

            pthread_mutex_lock(&mutex_writecount);
            writecount-=1;
            if (writecount==0)
            {
                sem_post(&rsem);
            }
            pthread_mutex_unlock(&mutex_writecount);
            printf("Writer %d is leaving the database\n",position);
            numbrExe--;
        }
    }

    void reader(void *arg){   
        read_write * arge = arg;
        int position  = *(arge->position);
        int numbrExe = *(int *)(arge->numbrExe);
        while(numbrExe > 0){
                
                //int temp = *(int*) arg;
                printf("Reader %d is trying to enter into the Database for reading the data\n",position);

                pthread_mutex_lock(&z);
                sem_wait(&rsem);

                pthread_mutex_lock(&mutex_readcount);
                    // section critique
                    readcount++;
                if (readcount==1){
                    // arrivée du premier reader
                    sem_wait(&wsem);
                    }
                pthread_mutex_unlock(&mutex_readcount);
                sem_post(&rsem);
                pthread_mutex_unlock(&z);

                //read_database();
                printf("Reader %d is reading the database\n",position);
                for (int i=0; i<10000; i++);
                pthread_mutex_lock(&mutex_readcount);
                    // section critique
                    readcount--;
                    if(readcount==0){ 
                        // départ du dernier reader
                        sem_post(&wsem);
                        }
                pthread_mutex_unlock(&mutex_readcount);
            //process_data();
            printf("Reader %d is leaving the database\n",position);
            numbrExe--;
            }
    }   




    int main ( int argc, char *argv[]){
        sem_init(&rsem, 0, 1);
        sem_init(&wsem, 0, 1);
        pthread_mutex_init(&mutex_readcount, NULL);
        pthread_mutex_init(&mutex_writecount, NULL);
        pthread_mutex_init(&z, NULL);


        
        if (argc < 3)
        {
            printf("You need to give 2 parameters\n");
            return EXIT_FAILURE;
        }

        
        int nombreExeR = 640;
        int nombreExeW = 2560;

        int readers= atoi(argv[1]);
        int writers = atoi(argv[2]);

        int nombreThProd = readers;
        int nombreThCons = writers;

        int exeThreadProd = nombreExeR/nombreThProd;
        int resteProd = nombreExeR%nombreThProd;

        int lastProd = exeThreadProd+resteProd;

        int exeThreadConsom = nombreExeW/nombreThCons;
        int restConsom = nombreExeW%nombreThCons;

        int lastCons = exeThreadConsom+restConsom;


        void *pointerREAD1 = &exeThreadProd;
        void *pointerREAD2 = &lastProd;

        void *pointerWrite1 = &exeThreadConsom;
        void *pointerWrite2 = &lastCons;

        printf("%d\n",exeThreadProd);
        printf("%d\n",lastProd);
        printf("%d\n",exeThreadConsom);
        printf("%d\n",lastCons);
        
        

        pthread_t readerThread[readers],writerThread[writers];

        printf("You have %d Threadreader\n", readers);
        printf("You have %d Threadwriter\n", writers);

        for (size_t i = 0; i < readers; i++)
        {
            read_write * readerrr = (read_write*) malloc(sizeof(reader));
            int * positionCopy = (int *) malloc(sizeof(int));
            *positionCopy = i;
            if(i < readers-1){
                readerrr->numbrExe = pointerREAD1;
                readerrr->position = positionCopy;
                pthread_create(&readerThread[i], NULL, reader, readerrr);
            }
            else
            {
                readerrr->numbrExe = pointerREAD2;
                readerrr->position = positionCopy;
                pthread_create(&readerThread[i], NULL, reader, readerrr);
            }
        }

        for(size_t i = 0; i < writers; i++)
        {
            read_write * writerrr = (read_write*) malloc(sizeof(reader));
            int * positionCopy = (int *) malloc(sizeof(int));
            *positionCopy = i;
            if(i < readers-1){
                writerrr->numbrExe = pointerWrite1;
                writerrr->position = positionCopy;
                pthread_create(&writerThread[i], NULL, writer, writerrr);
            }
            else
            {
                writerrr->numbrExe = pointerWrite2  ;
                writerrr->position = positionCopy;
                pthread_create(&writerThread[i], NULL, writer, writerrr);
            }
        }
        
        for (size_t i = 0; i < readers; i++)
        {
            pthread_join(writerThread[i], NULL);
        }
        for (size_t i = 0; i < writers; i++)
        {
        pthread_join(readerThread[i], NULL);
        }

        sem_destroy(&wsem);
        sem_destroy(&rsem);
        pthread_mutex_destroy(&mutex_readcount);
        pthread_mutex_destroy(&mutex_writecount);
        pthread_mutex_destroy(&z);
        return EXIT_SUCCESS;
    }
