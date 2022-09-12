#include <iostream>
#include <unistd.h>
#include <utility>
#include <vector>

using std::cin;
using std::cout;

std::vector<std::vector<std::pair<int, float>>> matrix = {{std::make_pair(0, 2.0), std::make_pair(1, -1.0)},
                                                          {std::make_pair(0, -1.0), std::make_pair(1, 2.0), std::make_pair(2, -1.0)},
                                                          {std::make_pair(1, -1.0), std::make_pair(2, 2.0), std::make_pair(3, -1.0)},
                                                          {std::make_pair(2, -1.0), std::make_pair(3, 2.0)}};

std::vector<std::vector<std::pair<int, float>>> sparse_matrix2 = {{std::make_pair(0, 1.0)},
                                                                  {std::make_pair(1, 1.0)},
                                                                  {std::make_pair(2, 1.0)},
                                                                  {std::make_pair(3, 1.0)}};

float dense_matrix[4][4] = {{1.0, 0, 0, 0},
                            {0, 1.0, 0, 0},
                            {0, 0, 1.0, 0},
                            {0, 0, 0, 1.0}};

// tanto a sparse_matrix2 como a dense_matrix sao matrizes identidade para facilitar o calculo

float vetor[4] = {1.0, 2.0, 3.0, 4.0};

int aux[4] = {0, 1, 2, 3};

void printMatrixByColumn(float **multMatrix) {
    cout << '\n';
    for (int j = 0; j < 4; j++) {
        for (int i = 0; i < 4; i++) {
            cout << multMatrix[i][j] << ' ';
        }
        cout << '\n';
    }
}
void printMatrixByRow(float **multMatrix) {
    cout << '\n';
    for (int j = 0; j < 4; j++) {
        for (int i = 0; i < 4; i++) {
            cout << multMatrix[j][i] << ' ';
        }
        cout << '\n';
    }
}

void *sparseMatrixDenseVectorHelper(void *arg) {
    float *result = new float[4]();
    int i = *(int *)arg;
    for (int k = 0; k < 4; k++) {
        for (int j = 0; j < 4; j++) {
            if (matrix[k].size() <= j || matrix[k][j].first > i) break; // o primeiro evita segfault, o segundo eh uma otimizacao
            if (matrix[k][j].first == i) {
                result[k] = matrix[k][j].second * vetor[i];
                break;
            }
        }
    }
    return result; // esse result eh a i-esima coluna
}

void sparseMatrixDenseVector() {
    pthread_t it[4];
    float **result = new float *[4];
    for (int i = 0; i < 4; i++) {
        pthread_create(&it[i], NULL, sparseMatrixDenseVectorHelper, (void *)&aux[i]); // o i vai mudando
    }
    for (int i = 0; i < 4; i++) pthread_join(it[i], (void **)&result[i]);
    printMatrixByColumn((float **)result);

    for (int i = 0; i < 4; i++) delete[] result[i];
    delete[] result;
}

void *sparseMatrixesHelper(void *arg) {
    float *result = new float[4]();
    int i = *(int *)arg;
    for (int l = 0; l < 4; l++) {
        for (int k = 0; k < 4; k++) {
            for (int j = 0; j < 4; j++) {
                if (sparse_matrix2[k].size() <= j || matrix[i].size() <= l || matrix[i][l].first < sparse_matrix2[k][j].first) break;
                if (matrix[i][l].first == sparse_matrix2[k][j].first) {
                    result[matrix[i][l].first] += matrix[i][l].second * sparse_matrix2[k][j].second;
                }
            }
        }
    }
    return result; // esse result eh a i-esima linha
}
void sparseMatrixes() {
    pthread_t it[4];
    float **result = new float *[4];
    for (int i = 0; i < 4; i++) {
        pthread_create(&it[i], NULL, sparseMatrixesHelper, (void *)&aux[i]); // o i vai mudando
        // sleep(1);
    }
    for (int i = 0; i < 4; i++) pthread_join(it[i], (void **)&result[i]);
    printMatrixByRow((float **)result);

    for (int i = 0; i < 4; i++) delete[] result[i];
    delete[] result;
}

void *sparseMatrixDenseMatrixHelper(void *arg) {
    float *result = new float[4]();
    int i = *(int *)arg;
    for (int k = 0; k < 4; k++) {
        for (int j = 0; j < matrix[i].size(); j++) {
            result[k] += dense_matrix[matrix[i][j].first][k] * matrix[i][j].second;
        }
    }
    return result; // esse result eh a i-esima linha
}

void sparseMatrixDenseMatrix() {
    pthread_t it[4];
    float **result = new float *[4];
    for (int i = 0; i < 4; i++) {
        pthread_create(&it[i], NULL, sparseMatrixDenseMatrixHelper, (void *)&aux[i]); // o i vai mudando
    }
    for (int i = 0; i < 4; i++) pthread_join(it[i], (void **)&result[i]);
    printMatrixByRow((float **)result);

    for (int i = 0; i < 4; i++) delete[] result[i];
    delete[] result;
}

int main() {
    sparseMatrixDenseVector();
    sparseMatrixes();          // matrix x sparse_matrix2
    sparseMatrixDenseMatrix(); // matrix x dense_matrix

    return 0;
}
