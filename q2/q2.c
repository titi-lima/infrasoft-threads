#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

int intersecao[5] = {0,0,0,0,0};

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;//indicar interseção vazia

//função q irá acrescentar 1 trem na interseção
void inc(int i, int num){
    pthread_mutex_lock(&mutex);
    while(intersecao[i] == 2) {//se a interseção estiver cheia vai dormir na variavel de condição
        pthread_cond_wait(&cond, &mutex);
    }
    intersecao[i]++;//acrescenta o trem
    printf("thread %d entrou na intersecao %d. Tem %d trens aqui\n",num, i, intersecao[i]);
    pthread_mutex_unlock(&mutex);
}

//função q irá tirar um trem da interseção
int dec(int i, int num){
    pthread_mutex_lock(&mutex);
    intersecao[i]--;//retira o trem
    printf("thread %d saiu da interseção %d, restaram %d trens\n", num, i, intersecao[i]);
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mutex);
}

void* rotina(void* arg){
    int *num = (int*)arg;//se essa conversão acontecer depois de arg ter mudado
    int i = 0;
    for(i = 0; i < 5; i++){
        inc(i, *num);
        usleep(500000);
        dec(i, *num);
    }
    pthread_exit(NULL);
}


int main(){
    pthread_t trem[10];
    int* num = (int*)malloc(10*sizeof(int));
    while(1){
        for (int i = 0; i < 10; i++){
            num[i] = i;
            pthread_create(&trem[i], NULL, &rotina, (void*)&num[i]);
            printf("a thread %d foi criada\n", i);
        }
        for (int i = 0; i < 10; i++){
            pthread_join(trem[i], NULL);
        }
    }


    return 0;
}
