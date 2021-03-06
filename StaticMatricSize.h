//
// Created by Lamin and Merjem
//


#pragma clang diagnostic push
#pragma ide diagnostic ignored "HidingNonVirtualFunction"

#ifndef MATRIXTEMPLATE_STATICMATRICSIZE_H
#define MATRIXTEMPLATE_STATICMATRICSIZE_H

#include "Matrix.h"

template<unsigned ROWS, unsigned COLUMNS, typename T, class MD = VectorMatrixData<T>>
class StaticSizeMatrix : public Matrix<T, MD> {
private:
    //This allows us to access protected members of another StaticSizeMatrix of a different size
    template<unsigned R, unsigned C, typename U, class MDD> friend
    class StaticSizeMatrix;

    //This allows us to access protected members of StaticSizeMatrix from Matrix
    template<typename U, class MDD> friend
    class Matrix;

protected:
    explicit StaticSizeMatrix(const MD &data) : Matrix<T, MD>(data) {
        if (data.columns() != COLUMNS) {
            Utils::error("Invalid columns count");
        } else if (data.rows() != ROWS) {
            Utils::error("Invalid rows count");
        }
    }

public:
    StaticSizeMatrix() : Matrix<T, MD>(ROWS, COLUMNS) {
    }

    template<unsigned ROW, unsigned COL>
    typename std::enable_if<ROW >= 0 && COL >= 0 && ROW < ROWS && COL < COLUMNS, MatrixCell<T, MD>>::type get() {
        return (*this)(ROW, COL);
    }

    template<unsigned ROW, unsigned COL>
    typename std::enable_if<ROW >= 0 && COL >= 0 && ROW < ROWS && COL < COLUMNS, T>::type get() const {
        return (*this)(ROW, COL);
    }


    /**
     * @return a submatrix. The method is only enabled when the bounds are valid
     */
    template<unsigned ROW_OFFSET, unsigned COL_OFFSET, unsigned ROW_COUNT, unsigned COL_COUNT>
    typename std::enable_if<ROW_OFFSET + ROW_COUNT <= ROWS &&
                            COL_OFFSET + COL_COUNT <= COLUMNS, StaticSizeMatrix<ROW_COUNT, COL_COUNT, T, SubmatrixMD<T, MD>>>::type
    submatrix() {
        return StaticSizeMatrix<ROW_COUNT, COL_COUNT, T, SubmatrixMD<T, MD>>(
                SubmatrixMD<T, MD>(ROW_OFFSET, COL_OFFSET, ROW_COUNT, COL_COUNT, this->data));
    }

    template<unsigned ROW_OFFSET, unsigned COL_OFFSET, unsigned ROW_COUNT, unsigned COL_COUNT>
    const typename std::enable_if<
            ROW_OFFSET + ROW_COUNT <= ROWS &&
            COL_OFFSET + COL_COUNT <= COLUMNS, StaticSizeMatrix<ROW_COUNT, COL_COUNT, T, SubmatrixMD<T, MD>>>::type
    submatrix() const {
        return StaticSizeMatrix<ROW_COUNT, COL_COUNT, T, SubmatrixMD<T, MD>>(
                SubmatrixMD<T, MD>(ROW_OFFSET, COL_OFFSET, ROW_COUNT, COL_COUNT, this->data));
    }

    using Matrix<T, MD>::submatrix;

    /**
     * @return the transposed matrix
     */
    StaticSizeMatrix<COLUMNS, ROWS, T, TransposedMD<T, MD>> transpose() {
        return StaticSizeMatrix<COLUMNS, ROWS, T, TransposedMD<T, MD>>(TransposedMD<T, MD>(this->data));
    }

    const StaticSizeMatrix<COLUMNS, ROWS, T, TransposedMD<T, MD>> transpose() const {
        return StaticSizeMatrix<COLUMNS, ROWS, T, TransposedMD<T, MD>>(TransposedMD<T, MD>(this->data));
    }

    /**
     * This method is added only on squared matrix
     */
    template<unsigned R = ROWS, unsigned C = COLUMNS>
    typename std::enable_if<R == C, StaticSizeMatrix<ROWS, 1, T, DiagonalMD<T, MD>>>::type diagonal() {
        return StaticSizeMatrix<ROWS, 1, T, DiagonalMD<T, MD>>(DiagonalMD<T, MD>(this->data));
    }

    template<unsigned R = ROWS, unsigned C = COLUMNS>
    const typename std::enable_if<R == C, StaticSizeMatrix<ROWS, 1, T, DiagonalMD<T, MD>>>::type diagonal() const {
        return StaticSizeMatrix<ROWS, 1, T, DiagonalMD<T, MD>>(DiagonalMD<T, MD>(this->data));
    }

    /**
    * Can only be called on a vector.
    * @return an immutable diagonal square matrix that has this vector as diagonal and <code>0</code> (zero) in all other positions.
    */
    template<unsigned C = COLUMNS>
    const typename std::enable_if<C == 1, StaticSizeMatrix<ROWS, ROWS, T, DiagonalMatrixMD<T, MD>>>::type diagonalMatrix() const {
        return StaticSizeMatrix<ROWS, ROWS, T, DiagonalMatrixMD<T, MD>>(DiagonalMatrixMD<T, MD>(this->data));
    }

    /**
     * Multiplies the two given matrices
     */
    template<unsigned C, class MD2>
    const StaticSizeMatrix<ROWS, C, T, MultiplyMD<T, MD, MD2>>
    operator*(const StaticSizeMatrix<COLUMNS, C, T, MD2> &another) const {
        return StaticSizeMatrix<ROWS, C, T, MultiplyMD<T, MD, MD2>>(
                MultiplyMD<T, MD, MD2>(this->data, another.data));
    }

    using Matrix<T, MD>::operator*;


    /**
     * Adds the two given matrices
     */
    template<class MD2>
    const StaticSizeMatrix<ROWS, COLUMNS, T, Sum<T, MD, MD2>>
    operator+(const StaticSizeMatrix<ROWS, COLUMNS, T, MD2> &another) const {
        return StaticSizeMatrix<ROWS, COLUMNS, T, Sum<T, MD, MD2>>(
                Sum<T, MD, MD2>(this->data, another.data));
    }


    template<class MD2>
    const StaticSizeMatrix<ROWS, COLUMNS, T, Sum<T, MD, MD2>>
    operator+(const Matrix<T, MD2> &another) const {
        return StaticSizeMatrix<ROWS, COLUMNS, T, Sum<T, MD, MD2>>(
                Sum<T, MD, MD2>(this->data, another.data));
    }

    StaticSizeMatrix<ROWS, COLUMNS, T, VectorMatrixData<T>> copy() const {
        return StaticSizeMatrix<ROWS, COLUMNS, T, VectorMatrixData<T>>(VectorMatrixData<T>::template toVector<MD>(this->data));
    }

    template<typename U>
    StaticSizeMatrix<ROWS, COLUMNS, U, MatrixCaster<U, MD>> cast() const {
        return StaticSizeMatrix<ROWS, COLUMNS, U, MatrixCaster<U, MD>>(MatrixCaster<U, MD>(this->data));
    }

};

#endif //MATRIXTEMPLATE_STATICMATRICSIZE_H
#pragma clang diagnostic pop