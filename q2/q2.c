#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

int intersecao[5] = {0,0,0,0,0};

pthread_mutex_t mutex;
pthread_cond_t empty;
pthread_cond_t fill;

void put(int i, int num){
    pthread_mutex_lock(&mutex);
    while(intersecao[i] == 2) {
        pthread_cond_wait(&empty, &mutex);
    }
    intersecao[i]++;
    printf("thread %d entrou na intersecao %d. Tem %d trens aqui\n",num, i, intersecao[i]);
    if (intersecao[i] == 1){
        pthread_cond_broadcast(&fill);
    }
    pthread_mutex_unlock(&mutex);
}

int get(int i, int num){
    int result;
    pthread_mutex_lock(&mutex);
    while(intersecao[i] == 0){
        pthread_cond_wait(&fill, &mutex);
    }
    intersecao[i]--;
    printf("thread %d saiu da interseção %d, restaram %d trens\n", num, i, intersecao[i]);
    if (intersecao[i] == 1){
        pthread_cond_broadcast(&empty);
    }
    pthread_mutex_unlock(&mutex);
}

void* rotina(void* arg){
    int num = *(int*)arg;
    int i = 0;
    for(i = 0; i < 5; i++){
        put(i, num);
        usleep(500000);
        get(i, num);
    }
    pthread_exit(NULL);
}


int main(){
    pthread_t trem[10];
    int* num = (int*)malloc(sizeof(int)*10);

    for (int i = 0; i < 10; i++){
        *num = i;
        pthread_create(&trem[i], NULL, rotina, (void*)num);
        printf("a thread %d foi criada\n", i);
    }
    for (int i = 0; i < 10; i++){
        pthread_join(trem[i], NULL);
    }

    return 0;
}