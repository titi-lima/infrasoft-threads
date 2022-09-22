#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

//definindo que o numero de votos (arquivos) será 10
//o número de candidatos 3
//e o número de threads 5
#define N 10
#define C 3
#define T 5

//batch é o lote em que cada thread irá trabalhar do array de votos
int batch = N/T;
//o vetor de candidatos tem tamanho C + 1 para incluir o voto em branco
int candidatos[C + 1];

pthread_mutex_t mutex[N];

int votos[N];

void* lerArquivo(void* arg){
    int i = *(int*)arg;
    FILE *file = NULL;
    char str[5];

    for (int j = i; j < i + batch; j++){
        pthread_mutex_lock(&mutex[j]);

        sprintf(str, "%d.in", j + 1);//serve para indicar o nome correto do arquivo a ser aberto
        file = fopen(str, "r");
        fscanf(file, "%d", &votos[j]);
        candidatos[votos[j]]++;//incrementa o candidato que recebeu o voto
        fclose(file);

        pthread_mutex_unlock(&mutex[j]);
    }
    pthread_exit(NULL);
}

int main(){
    pthread_t thread[T];

    //criando as 10 threads
    for(int i = 0; i < T; i++){
        pthread_mutex_init(&mutex[i], NULL);
    }
    for (int i = 0; i < T; i++){
        int *parte = malloc(sizeof(int));
        *parte = i * batch; //responsavel por definir qual posição do vetor cada thread começará a ler
        pthread_create(&thread[i], NULL, lerArquivo, parte);
    }

    for (int i = 0; i < T; i++){
        pthread_join(thread[i], NULL);
    }
    for(int i = 0; i < T; i++){
        pthread_mutex_destroy(&mutex[i]);
    }

    //aqui embaixo esta printando o que foi pedido no enunciado
    float branco = candidatos[0];

    printf("votos em branco = %d, representando %.2f por cento dos votos\n", candidatos[0],100* branco/N);

    for(int i = 1; i <= C; i++){
        float num = candidatos[i];
        printf("candidato %d recebeu %d votos, representando %.2f por cento do total de votos\n", i, candidatos[i], 100*num/N);
    }

    return 0;
}
