#include <iostream>
#include <tuple>
#include <vector>

using std::cin;
using std::cout;

int main() {
    std::vector<std::vector<std::tuple<int, float>>> matrix = {{std::make_tuple(0, 2.0), std::make_tuple(1, -1.0)},
                                                               {std::make_tuple(0, -1.0), std::make_tuple(1, 2.0), std::make_tuple(2, -1.0)},
                                                               {std::make_tuple(1, -1.0), std::make_tuple(2, 2.0), std::make_tuple(3, -1.0)},
                                                               {std::make_tuple(2, -1.0), std::make_tuple(3, -1.0)}};
    sparseMatrixDenseVector();
    sparseMatrixes();
    sparseMatrixDenseMatrix();

    return 0;
}
