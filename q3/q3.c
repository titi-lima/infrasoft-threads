#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define n 2 //numero de threads escritoras 
#define m 3 //numero de threads leitoras

pthread_mutex_t mutex_escritora = PTHREAD_MUTEX_INITIALIZER; //mutex da função escritora.
pthread_mutex_t mutex_leitora = PTHREAD_MUTEX_INITIALIZER; //mutex da variavel QuantLeit.
pthread_mutex_t mutex_querescrever = PTHREAD_MUTEX_INITIALIZER; //mutex da variavel querescrever.
pthread_cond_t Leit = PTHREAD_COND_INITIALIZER; //variavel de condição relacionada a threads leitoras operantes
pthread_cond_t NovaLeit = PTHREAD_COND_INITIALIZER; //variável de condição relacionada ao inicio de novas threads leitoras
int querescrever=0; //variavel que indica quando uma thread escritora está esperando, =0(nenhuma esperando) e =1(alguma esperando)
int QuantLeit=0; //quantidade de threads leitoras que estão lendo no momento. 
int cont=0; //contador que vai ajudar a indicar qual elemento do array será lido/sobrescrito e qual elemento será escrito.
char array[5] = {'v','a','s','c','o'}; //array que sera alterado
char alfb[26] = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'};
//array auxiliar para as escritoras escolherem qual letra escrever

void *ler(int *b) {
    while(1) {pthread_mutex_lock(&mutex_querescrever); //impede que a variavel querescrever seja alterada por alguma escritora
        while(querescrever==1) pthread_cond_wait(&NovaLeit, &mutex_querescrever); //impede que novas threads leitoras surjam/operem quando uma escritora estiver esperando (otimiza a saída) e permite que outras leitoras passem do lock anterior
        pthread_mutex_unlock(&mutex_querescrever);
        pthread_mutex_lock(&mutex_leitora); //impede leitura de QuantLeit por alguma escritora
        int div1=cont%5; //escolhendo qual elemento ler
        QuantLeit++;//incrementa numero de threads leitoras no momento
        pthread_mutex_unlock(&mutex_leitora);
        printf("o elemento lido foi %c na posição %d | T%d\n", array[div1], div1, *b); //a leitura fora do lock/unlock impede que as leitoras se bloqueem
        pthread_mutex_lock(&mutex_leitora); //impede leitura de QuantLeit por alguma escritora
        QuantLeit--;//decrementa numero de threads leitoras no momento
        cont++;
        if(QuantLeit==0) pthread_cond_broadcast(&Leit); //verifica se não há mais leitoras operando, para que as escritoras sejam liberadas
        pthread_mutex_unlock(&mutex_leitora);
    }
    pthread_exit(NULL);
}

void *escrever(int *a) {
    while(1) {
        pthread_mutex_lock(&mutex_escritora); //impede que mais de uma thread escritora opere ao mesmo tempo
        pthread_mutex_lock(&mutex_querescrever); //impede que threads leitoras leiam a variavel querescrever enquanto ela e alterada
        querescrever=1; //sinaliza para as threads leitoras que uma escritora esta no aguardo
        pthread_mutex_unlock(&mutex_querescrever);
        pthread_mutex_lock(&mutex_leitora); //impede que o valor de QuantLeit seja alterado por alguma leitora
        while(QuantLeit>0) pthread_cond_wait(&Leit, &mutex_leitora); //aguarda ate que nenhuma thread leitora esteja atuando
        pthread_mutex_unlock(&mutex_leitora);
        int div2=cont%5; //escolhendo qual elemento vai ser sobrescrito
        int div3=cont%26; //escolhendo qual letra vai substituir o elemento
        array[div2]=alfb[div3]; //regiao critica
        printf("o elemento %c foi escrito na posição %d | T%d\n", alfb[div3], div2, *a);
        printf("%s\n", array);
        pthread_mutex_lock(&mutex_querescrever); //impede leitura da variavel querescrever pelas leitoras
        querescrever=0; //thread escritora nao esta mais na espera
        pthread_cond_broadcast(&NovaLeit); //sinal para avisar que novas leitoras podem iniciar agora
        pthread_mutex_unlock(&mutex_querescrever); //variavel pode ser lida pelas leitoras
        cont++;
        pthread_mutex_unlock(&mutex_escritora); //outra thread escritora pode atuar agora
    }
    pthread_exit(NULL);
}

int main() {
    pthread_t t1[n], t2[m];
    int *idEsc[n]; //array com ids das escritoras
    for(int i=0; i<n; i++) { //criando escritoras
        idEsc[i]=(int*) malloc(sizeof(int));
        *idEsc[i]=i;
        pthread_create(&t1[i], NULL, escrever, (void*) idEsc[i]); 
    }
    int *idLei[m]; //array com ids das leitoras
    for(int j=0; j<m; j++) { //criando leitoras
        idLei[j]=(int*) malloc(sizeof(int));
        *idLei[j]=j;
        pthread_create(&t2[j], NULL, ler, (void*) idLei[j]); 
    }
    pthread_exit(NULL);
}