// animcao do algoritmo para varias threads: https://www.youtube.com/watch?v=6ODPqo1dVrs

#include <iostream>
#include <list>
#include <pthread.h>
#include <unistd.h>
#include <vector>

#define INFINITY 1000

using namespace std;

void print(vector<int> &A) {
    for (auto &&a : A) {
        cout << a << " ";
    }
    cout << "\n";
}

typedef struct Parameters Parameters;
// Disjoint sets (union find)
class Floresta {
public:
    int n_arvores;
    vector<int> pai;
    vector<int> rank;
    vector<list<int>> arvore;

    Floresta(int n_nodes) {

        n_arvores = n_nodes;
        pai.resize(n_nodes);
        rank.resize(n_nodes);
        arvore.resize(n_nodes);
        for (int i = 0; i < n_nodes; i++) {
            pai[i] = i;
            rank[i] = 0;
            arvore[i].push_back(i);
        }
    }
    void *Find(int node);
    static void *Find_helper(void *arg);
    void Union(int node1, int node2);
};

struct Parameters {
    int node;
    Floresta *f;
    Parameters(int node, Floresta &f) : node(node), f(&f) {}
};

void *Floresta::Find(int node) {
    if (pai[node] == node) {
        int *aux = new int(pai[node]);
        return (void *)aux;
    }

    int *raiz = (int *)Find(pai[node]);
    int *aux = new int(*raiz);
    pai[node] = *raiz;
    delete raiz;
    return (void *)aux;
}

void *Floresta::Find_helper(void *arg) {
    Parameters *p = static_cast<Parameters *>(arg);
    return (void *)((Floresta *)p->f)->Find(p->node);
}

void Floresta::Union(int node1, int node2) {
    int *raiz1 = (int *)Find(node1);
    int *raiz2 = (int *)Find(node2);

    if (rank[*raiz1] > rank[*raiz2])
        swap(raiz1, raiz2);

    if (rank[*raiz1] == rank[*raiz2])
        rank[*raiz2]++;

    arvore[*raiz2].splice(arvore[*raiz2].end(),
                          arvore[*raiz1]);

    pai[*raiz1] = pai[*raiz2];
    this->n_arvores -= 1;
    delete raiz1;
    delete raiz2;
}
struct aresta {
    int node1;
    int node2;
    int peso;
    aresta(int p) {
        peso = p;
    }
    aresta(int n1, int n2, int p) {
        node1 = n1;
        node2 = n2;
        peso = p;
    }
};

aresta find_best_edge(vector<vector<pair<int, int>>> grafo, Floresta &f, list<int> &arvore) {
    pthread_t c, v;
    struct aresta best_edge(INFINITY);

    // para cada node na arvore
    for (auto node : arvore) {
        // olha todas arestas ligadas a este node
        for (auto a : grafo[node]) {
            int vizinho = a.first;
            int peso_node_vizinho = a.second;

            // apenas arestas que ligam a arvore a outra
            void *rc = NULL, *rv = NULL;
            Parameters *p1 = new Parameters(node, f);
            Parameters *p2 = new Parameters(vizinho, f);
            pthread_create(&c, NULL, &Floresta::Find_helper, p1);
            pthread_create(&v, NULL, &Floresta::Find_helper, p2);
            pthread_join(c, &rc);
            pthread_join(v, &rv);
            int *tn1 = (int *)rc;
            int *tn2 = (int *)rv;

            if (*tn1 == *tn2) {
                delete p1;
                delete tn1;
                delete p2;
                delete tn2;
                continue;
            }

            delete p1;
            delete tn1;
            delete p2;
            delete tn2;

            if (best_edge.peso > peso_node_vizinho) {
                best_edge = aresta(node, vizinho, peso_node_vizinho);
            };
        }
    }
    return best_edge;
}

vector<aresta> find_best_edges(vector<vector<pair<int, int>>> grafo, Floresta &f, vector<aresta> &mst) {
    int n_nodes = grafo.size();
    vector<aresta> best_edges;
    for (int node = 0; node < n_nodes; node++) {
        // achando raiz que representa a arvore
        if (f.pai[node] == node) {
            int raiz = node;

            // melhor aresta que liga esta arvore a outra
            aresta best_edge = find_best_edge(grafo, f, f.arvore[raiz]);
            best_edges.push_back(best_edge);
        }
    }
    return best_edges;
}

vector<aresta> boruvka(vector<vector<pair<int, int>>> grafo) {
    pthread_t c, v;
    vector<aresta> mst;

    int n_nodes = grafo.size();

    Floresta f(n_nodes); // criar conjunto de arvores (floresta)

    // enquanto numero de arvores  > 1:
    while (f.n_arvores > 1) {
        // achar melhor edge para cada arvore
        vector<aresta> best_edges = find_best_edges(grafo, f, mst);

        // commit das arestas e uniao das arvores:
        for (auto best_edge : best_edges) {
            int n1 = best_edge.node1;
            int n2 = best_edge.node2;
            void *rc = NULL, *rv = NULL;
            Parameters *p1 = new Parameters(n1, f);
            Parameters *p2 = new Parameters(n2, f);
            pthread_create(&c, NULL, &Floresta::Find_helper, p1);
            pthread_create(&v, NULL, &Floresta::Find_helper, p2);
            pthread_join(c, &rc);
            pthread_join(v, &rv);
            int *tn1 = (int *)rc;
            int *tn2 = (int *)rv;
            if (*tn1 != *tn2) { // implementacao da thread aqui
                mst.push_back(best_edge);
                f.Union(n1, n2);
            }
            delete p1;
            delete tn1;
            delete p2;
            delete tn2;
        }
    }
    return mst;
}

int main() {
    /* Let us create the following graph
   2   3
(0)--(1)--(2)
|      /  |
6|   8/    |7
|  /      |
(3)-------(4)
    9     */
    int V = 5;
    vector<vector<pair<int, int>>> graph = {
        {make_pair(1, 2), make_pair(3, 6)},                  // 0
        {make_pair(0, 2), make_pair(2, 3)},                  // 1
        {make_pair(1, 3), make_pair(3, 8), make_pair(4, 7)}, // 2
        {make_pair(0, 6), make_pair(2, 8), make_pair(4, 9)}, // 3
        {make_pair(3, 9), make_pair(2, 7)}                   // 4
    };
    vector<aresta> mst = boruvka(graph);
    for (auto a : mst) {
        cout << a.node1 << " - " << a.node2 << " " << a.peso << endl;
    }
    return 0;
}