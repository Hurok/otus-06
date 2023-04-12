#ifndef MATRIX_H
#define MATRIX_H

#include <memory>
#include <optional>
#include <vector>
#include <map>
#include <set>
#include <optional>
#include <algorithm>

/*!
 * \brief Описывает позицию элемента матрицы.
 */
struct MatrixPos {
    int row = 0;
    int col = 0;

    bool isValid() const noexcept {
        return row >= 0 && col >= 0;
    }

    bool operator != (const MatrixPos &other) const noexcept {
        return row != other.row || col != other.col;
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
 * \warning Полученные ссылки на элементы матрицы могут стать невалидными после изменения её размерности!
 */
template <typename T, T DefaultValue = T{}, bool autoSolveUndeterminedValues = false>
class Matrix
{
    friend struct MatrixIterator;

public:
    /*!
     * \brief Прокси для работы со значением матрицы.
     */
    struct ProxyMatrixValue {
        ProxyMatrixValue() = default;
        ~ProxyMatrixValue() = default;

        ProxyMatrixValue(const ProxyMatrixValue &other)
            : m_ref{other.m_ref},
              m_pos{other.pos()}
        { }

        ProxyMatrixValue &operator = (const ProxyMatrixValue &other) noexcept {
            if (this != &other) {
                m_ref = other.m_ref;
                m_pos = other.pos();
            }

            return *this;
        }

        ProxyMatrixValue(std::optional<std::reference_wrapper<T>> value, const MatrixPos &pos)
            : m_ref{value},
              m_pos{pos}
        { }

        operator const T &() const noexcept {
            if (m_ref)
                return (*m_ref).get();

            return m_defaultValue;
        }

        ProxyMatrixValue &operator = (const T &other) noexcept {
            if (m_ref)
                (*m_ref).get() = other;

            return *this;
        }

        const T &value() const noexcept {
            if (m_ref)
                return (*m_ref).get();

            return m_defaultValue;
        }

        const MatrixPos &pos() const noexcept {
            return m_pos;
        }

        bool isNull() const noexcept {
            return !m_ref.has_value() || *m_ref == DefaultValue;
        }

    private:
        std::optional<std::reference_wrapper<T>> m_ref; // имеется ссылка только при валидных итераторах или при обращении без итераторов
        MatrixPos m_pos;
        const T m_defaultValue = DefaultValue;
    };

    /*!
     * \brief Прокси для работы с элементами строки матрицы. При запросе несуществующих значений, они будут добавлены в матрицу со значением по умолчанию.
     */
    struct ProxyMatrixRow {
        ProxyMatrixRow(std::map<MatrixPos, T> &values, std::set<MatrixPos> &undeterminateValues, int row)
            : m_values(values),
              m_undeterminateValues(undeterminateValues),
              m_row(row)
        { }

        ProxyMatrixValue operator[](int col) noexcept {
            MatrixPos pos{m_row, col};
            if (!m_values.count(pos)) {
                m_values.insert(std::make_pair(pos, DefaultValue));
            }

            m_undeterminateValues.insert(pos);
            auto it = m_values.find(pos);
            return ProxyMatrixValue{it->second, it->first};
        }

    private:
        std::map<MatrixPos, T> &m_values;
        std::set<MatrixPos> &m_undeterminateValues;
        const int m_row;
    };

    /*!
     * \brief Проксти для работы с элементами строки матрицы без возможности изменения значений.
     */
    struct ConstProxyMatrixRow {
        ConstProxyMatrixRow(const std::map<MatrixPos, T> &values, const T &defaultValue, int row)
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

    /*!
     * \brief Реализация итератора матрицы через итератор контейнера со значениями. Путь наименьшего сопротивления.
     */
    template <typename MapIterator, typename ValueType>
    struct MatrixIterator : public std::iterator<std::input_iterator_tag, ValueType>
    {
        friend Matrix;

        MatrixIterator(const MatrixIterator &other)
            : m_it(other.m_it)
        { }

        bool operator != (MatrixIterator const &other) const {
            return m_it != other.m_it;
        }

        bool operator == (MatrixIterator const &other) const {
            return m_it == other.m_it;
        }

        ValueType &operator *() {
            return m_it->second;
        }

        const ValueType &value() const {
            return m_it->second;
        }

        const MatrixPos &pos() const {
            return m_it->first;
        }

        ValueType *operator ->() {
            return &m_it->second;
        }

        MatrixIterator &operator ++() {
            ++m_it;
            return *this;
        }

    private:
        MatrixIterator(MapIterator it)
            : m_it{it}
        { }

        MapIterator m_it;
    };

public:
    Matrix() = default;
    ~Matrix() = default;

    typedef MatrixIterator<typename std::map<MatrixPos, T>::iterator, T> iterator;
    typedef MatrixIterator<typename std::map<MatrixPos, T>::const_iterator, const T> const_iterator;

    iterator begin() {
        return iterator{m_values.begin()};
    }

    iterator end() {
        return iterator{m_values.end()};
    }

    const_iterator cbegin() const {
        return const_iterator{m_values.cbegin()};
    }

    const_iterator cend() const {
        return const_iterator{m_values.cend()};
    }

    ConstProxyMatrixRow operator[](int row) const noexcept {
        return ConstProxyMatrixRow{m_values, m_defaultValue, row};
    }

    ProxyMatrixRow operator[](int row) noexcept {
        return ProxyMatrixRow{m_values, m_undeterminateValues, row};
    }

    /*!
     * \brief Добавляет элемент в матрицу. Если элемент уже существовал, то заменяет его.
     */
    void insert(int row, int col, T &&value) noexcept {
        MatrixPos pos{row, col};
        if (value != DefaultValue) {
            m_values.insert_or_assign(pos, std::move(value));
            m_cols = std::max(*m_cols, col + 1);
            m_rows = std::max(*m_rows, row + 1);
        } else if (m_values.count(pos) > 0) {
            m_values.erase(m_values.find(pos));
            m_cols.reset();
            m_rows.reset();
        }
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
        if (!m_rows)
            recacheColsRows();

        return *m_rows;
    }

    /*!
     * \brief Количество колонок. Если было запрошено значение через ProxyMatrixCol, то будет произведен пересчет используемых колонок в матрице.
     */
    int cols() const noexcept {
        solveUndeterminateValues();
        if (!m_cols)
            recacheColsRows();

        return *m_cols;
    }

private:
    /*!
     * \brief Обновляет значения количества колонок и строк.
     */
    void recacheColsRows() const noexcept {
        m_rows = 0;
        m_cols = 0;
        for (auto it = cbegin(); it != cend(); ++it) {
            m_rows = std::max(*m_rows, it.pos().row + 1);
            m_cols = std::max(*m_cols, it.pos().col + 1);
        }
    }

    /*!
     * \brief Удаляет 'нулевые' элементы из матрицы, если они там имеются. Удаляются только те элементы, которые были созданы неявно через ProxyMatrixCol.
     */
    void solveUndeterminateValues() const noexcept {
        if (!m_undeterminateValues.empty()) {
            for (const auto &key : m_undeterminateValues) {
                auto it = m_values.find(key);
                if (it != m_values.end()) {
                    if (it->second == DefaultValue) {
                        m_values.erase(it);
                    } else {
                        m_cols = std::max(*m_cols, it->first.col + 1);
                        m_rows = std::max(*m_rows, it->first.row + 1);
                    }
                }
            }

            m_undeterminateValues.clear();
        }
    }

private:
    mutable std::map<MatrixPos, T> m_values;
    const T m_defaultValue = DefaultValue;
    mutable std::optional<int> m_cols = 0;
    mutable std::optional<int> m_rows = 0;
    mutable std::set<MatrixPos> m_undeterminateValues;
};

#endif // MATRIX_H
