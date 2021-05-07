//
// Created by Lamin Jatta

#ifndef MATRIXTEMPLATE_SUM_H
#define MATRIXTEMPLATE_SUM_H

#include "MatrixUtils.h"
/**
 * Implementation of <code>MatrixData</code> that exposes the sum of the two given matrices
 * @tparam T type of the data
 */
template<typename T, class MD1, class MD2>
class Sum : public BiMatrixWrapper<T, MD1, MD2> {
public:
    Sum(MD1 left, MD2 right) : BiMatrixWrapper<T, MD1, MD2>(left, right, left.rows(), left.columns()) {
        if (left.rows() != right.rows() || left.columns() != right.columns()) {
            Utils::error("Sum between incompatible sizes");
        }
    }

    MATERIALIZE_IMPL

            Sum<T, MD1, MD2> copy() const {
        return Sum<T, MD1, MD2>(this->left.copy(), this->right.copy());
    }

private:

    T doGet(unsigned row, unsigned col) const {
        return this->left.get(row, col) + this->right.get(row, col);
    }
};

/**
 * Implementation of <code>MatrixData</code> that exposes the sum of a list given matrices
 * @tparam T type of the data
 */
template<typename T, class MD>
class MultiSum : public MultiMatrixWrapper<T, MD> {
public:
    explicit MultiSum(std::deque<MD> wrapped) : MultiMatrixWrapper<T, MD>(wrapped, wrapped[0].rows(), wrapped[0].columns()) {
        for (auto &m:wrapped) {
            if (m.rows() != this->rows() || m.columns() != this->columns()) {
                Utils::error("Sum between incompatible sizes");
            }
        }
    }

    MATERIALIZE_IMPL

            MultiSum<T, MD> copy() const {
        return MultiSum<T, MD>(this->copyWrapped());
    }

private:
    T doGet(unsigned row, unsigned col) const {
        T ret = 0;
        for (auto it = this->wrapped.begin(); it < this->wrapped.end(); it++) {
            ret += it->get(row, col);
        }
        return ret;
    }
};

#endif //MATRIXTEMPLATE_SUM_H
