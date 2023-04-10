#include "Utils.h"
#include "Matrix.h"

static constexpr auto kRowsMax = 10;
static constexpr auto kColsMax = kRowsMax;

int main(int, char**) {   
    try {
        Matrix<int, 0> m;

        // заполнение главной и второстепенной диагоналей матрицы
        {
            for (auto row = 0, col = kColsMax - 1, colPrimary = 0; row < kRowsMax && col >= 0 && colPrimary < kColsMax; row++, col--, colPrimary++) {
                m[row][col] = col;
                m[row][colPrimary] = colPrimary;
            }
        }

        // вывод фрагмента от [1,1] до [8,8]
        for (auto row = 1; row <= 8; ++row) {
            for (auto col = 1; col <= 8; ++col) {
                std::cout << m[row][col] << " ";
            }

            std::cout << std::endl;
        }

        // количество занятых ячеек
        std::cout << m.size() << std::endl;

        // вывод всех занятых ячеек вместе с их позициями

    } catch (std::exception &e) {
        std::cout << e.what() << std::endl;
    }

    return EXIT_SUCCESS;
}

