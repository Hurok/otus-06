#ifndef MATRIX_H
#define MATRIX_H

#include <memory>
#include <optional>
#include <vector>
#include <map>
#include <set>
#include <iostream>

/*!
 * \brief Описывает позицию элемента матрицы.
 */
struct MatrixPos {
    int row = 0;
    int col = 0;

    bool isValid() const noexcept {
        return row >= 0 && col >= 0;
    }

    bool operator == (const MatrixPos &other) const noexcept {
        return row == other.row && col == other.col;
    }

    bool operator < (const MatrixPos &other) const noexcept {
        return row == other.row ? col < other.col
                                : row < other.row;
    }
};

/*!
 * \brief Бесконечная матрица.
 */
template <typename T, T DefaultValue = T{}, bool autoSolveUnterminatedValues = false>
class Matrix
{
public:
    /*!
     * \brief Прокси для работы с элементами строки матрицы. При запросе несуществующих значений, они будут добавлены в матрицу со значением по умолчанию.
     */
    struct ProxyMatrixCol {
        ProxyMatrixCol(std::map<MatrixPos, T> &values, std::set<MatrixPos> &undeterminateValues, int row)
            : m_values(values),
              m_undeterminateValues(undeterminateValues),
              m_row(row)
        { }

        T &operator[](int col) noexcept {
            MatrixPos pos{m_row, col};
            if (!m_values.count(pos)) {
                m_values.insert(std::make_pair(pos, DefaultValue));
            }

            m_undeterminateValues.insert(pos);
            return m_values.find(pos)->second;
        }

    private:
        std::map<MatrixPos, T> &m_values;
        std::set<MatrixPos> &m_undeterminateValues;
        const int m_row;
    };

    /*!
     * \brief Проксти для работы с элементами строки матрицы без возможности изменения значений.
     */
    struct ConstProxyMatrixCol {
        ConstProxyMatrixCol(const std::map<MatrixPos, T> &values, const T &defaultValue, int row)
            : m_values(values),
              m_defaultValue(defaultValue),
              m_row(row)
        { }

        const T operator[](int col) const noexcept {
            MatrixPos pos{m_row, col};
            if (m_values.count(pos) > 0) {
                return m_values.at(pos);
            }

            return m_defaultValue;
        }

    private:
        const std::map<MatrixPos, T> &m_values;
        const T &m_defaultValue;
        const int m_row;
    };

public:
    Matrix() = default;
    ~Matrix() = default;

    ConstProxyMatrixCol operator[](int row) const noexcept {
        return ConstProxyMatrixCol{m_values, m_defaultValue, row};
    }

    ProxyMatrixCol operator[](int row) noexcept {
        return ProxyMatrixCol{m_values, m_undeterminateValues, row};
    }

    /*!
     * \brief Добавляет элемент в матрицу. Если элемент уже существовал, то заменяет его.
     */
    void insert(int row, int col, T &&value) noexcept {
        MatrixPos pos{row, col};
        m_values.insert_or_assign(pos, std::move(value));
        m_cols = std::max(m_cols, col + 1);
        m_rows = std::max(m_rows, row + 1);
    }

    /*!
     * \brief Возвращает элемент по указанным индексам. Если элемент отсутствует, то возвращается m_defaultValue.
     */
    const T &at(int row, int col) const noexcept {
        const auto pos = MatrixPos{row, col};
        if (m_values.count(pos) > 0)
            return m_values.at(pos);

        return m_defaultValue;
    }

    /*!
     * \brief Общее количество элементов в матрице. Если было запрошено значение через ProxyMatrixCol, то будет произведен пересчет использлуемых элементов в матрице.
     */
    int size() const noexcept {
        solveUndeterminateValues();
        return m_values.size();
    }

    /*!
     * \brief Количество строк. Если было запрошено значение через ProxyMatrixCol, то будет произведен пересчет используемых строк в матрице.
     */
    int rows() const noexcept {
        solveUndeterminateValues();
        return m_rows;
    }

    /*!
     * \brief Количество колонок. Если было запрошено значение через ProxyMatrixCol, то будет произведен пересчет используемых колонок в матрице.
     */
    int cols() const noexcept {
        solveUndeterminateValues();
        return m_cols;
    }

private:
    /*!
     * \brief Удаляет нулевые элементы из матрицы, если они там имеются. Удаляются только те элементы, которые были созданы неявно через ProxyMatrixCol.
     */
    void solveUndeterminateValues() const noexcept {
        if (!m_undeterminateValues.empty()) {
            for (const auto &key : m_undeterminateValues) {
                auto it = m_values.find(key);
                if (it != m_values.end()) {
                    if (it->second == DefaultValue) {
                        std::cout << "remove " << it->first.row << ":" << it->first.col << " value" << std::endl;
                        m_values.erase(it);
                    } else {
                        m_cols = std::max(m_cols, it->first.col + 1);
                        m_rows = std::max(m_rows, it->first.row + 1);
                    }
                }
            }

            m_undeterminateValues.clear();
        }
    }

private:
    mutable std::map<MatrixPos, T> m_values;
    const T m_defaultValue = DefaultValue;
    mutable int m_cols = 0;
    mutable int m_rows = 0;
    mutable std::set<MatrixPos> m_undeterminateValues;
};

#endif // MATRIX_H
