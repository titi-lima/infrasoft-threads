#include <iostream>
#include <pthread.h>
#include <vector>

using std::cin;
using std::cout;

bool isLatin = true;
int **matrix, N;

void *checkColumns(void *ptr) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N - 1; j++) {
            for (int k = j + 1; k < N; k++) {
                cout << i << ' ' << j << ' ' << k << '\n';
                if (matrix[i][j] == matrix[i][k]) {
                    isLatin = false;
                    pthread_exit(NULL);
                }
                cout << "morreu\n";
            }
        }
    }
    pthread_exit(NULL);
}

void *checkRows(void *ptr) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N - 1; j++) {
            for (int k = j + 1; k < N; k++) {
                if (matrix[j][i] == matrix[k][i]) {
                    isLatin = false;
                    pthread_exit(NULL);
                }
            }
        }
    }
    pthread_exit(NULL);
}

int main() {
    pthread_t c, r;

    cout << "insira o numero de linhas da matriz quadrada: ";
    cin >> N;
    matrix = new int *[N];

    cout << "insira a matriz " << N << 'x' << N << ":\n";
    for (int i = 0; i < N; i++) {
        matrix[i] = new int[N];
        for (int j = 0; j < N; j++) {
            cin >> matrix[i][j];
        }
    }
    pthread_create(&c, NULL, checkColumns, NULL);
    pthread_create(&r, NULL, checkRows, NULL);

    pthread_join(c, NULL);
    pthread_join(r, NULL);

    if (isLatin)
        cout << "eh uma matriz latina\n";
    else
        cout << "nao eh uma matriz latina\n";

    for (int i = 0; i < N; i++) delete[] matrix[i];
    delete[] matrix;

    return 0;
}
