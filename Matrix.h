#ifndef MATRIX_MATRIX_H
#define MATRIX_MATRIX_H

#include <memory>
#include <string>
#include <iostream>
#include "MultipleMethod.h"
#include "Sum.h"
#include "Multiplication.h"
#include "Iterator.h"
#include "MatrixCell.h"


template<unsigned ROWS, unsigned COLUMNS, typename T, class MD>
class StaticSizeMatrix;

/**
 * The only class exposed to the user of the library. It provides every method needed to use the matrix.
 * @tparam T the type of the data contained in each matrix cell
 */
template<typename T, class MD = VectorMatrixData<T>>
class Matrix {
	private:
		//This allows us to access protected members of another Matrix of a different type
		template<typename U, class MDD> friend
		class Matrix;

		//This allows us to access protected members of another StaticSizeMatrix of a different size
		template<unsigned R, unsigned C, typename U, class MDD> friend
		class StaticSizeMatrix;


	protected:
		MD data; //Pointer to the class holding the data

		/** Private constructor that accepts a pointer to the data */
		explicit Matrix(MD data) : data(data) {}

	public:

		/**
		 * Base constructor that creates a new matrix of the given size
		 * @param rows number of rows
		 * @param columns number of columns
		 */
		explicit Matrix(unsigned rows, unsigned columns) : data(VectorMatrixData<T>(rows, columns)) {
		}

		Matrix(const Matrix<T, MD> &other) : data(other.data.copy()) {}

		/**
		 * Move constructor. Default behaviour.
		 * @param other the other matrix
		 */
		Matrix(Matrix<T, MD> &&other) noexcept = default;

		MD &getData() {
			return this->data;
		}

		const T operator()(unsigned row, unsigned col) const {
			if (row < 0 || row >= this->rows()) {
				Utils::error("Row out of bounds");
			} else if (col < 0 || col >= this->columns()) {
				Utils::error("Column out of bounds");
			}
			return this->data.get(row, col);
		}

		MatrixCell<T, MD> operator()(unsigned row, unsigned col) {
			return MatrixCell<T, MD>(this->data, row, col);
		}

		/**
		 * @return the number of columns
		 */
		unsigned columns() const {
			return this->data.columns();
		}

		/**
		 * @return the number of rows
		 */
		unsigned rows() const {
			return this->data.rows();
		}

		/**
		 * @return the total number of cells (rows*columns)
		 */
		unsigned size() const {
			return rows() * columns();
		}

		Matrix<T, SubmatrixMD<T, MD>> submatrix(unsigned rowOffset, unsigned colOffset, unsigned rows, unsigned columns) {
			return Matrix<T, SubmatrixMD<T, MD>>(SubmatrixMD<T, MD>(rowOffset, colOffset, rows, columns, this->data));
		}

		const Matrix<T, SubmatrixMD<T, MD>> submatrix(unsigned rowOffset, unsigned colOffset, unsigned rows, unsigned columns) const {
			return Matrix<T, SubmatrixMD<T, MD>>(SubmatrixMD<T, MD>(rowOffset, colOffset, rows, columns, this->data));
		}

		template<unsigned ROW_COUNT, unsigned COL_COUNT>
		StaticSizeMatrix<ROW_COUNT, COL_COUNT, T, SubmatrixMD<T, MD>> submatrix(unsigned rowOffset, unsigned colOffset) {
			return StaticSizeMatrix<ROW_COUNT, COL_COUNT, T, SubmatrixMD<T, MD>>(
					SubmatrixMD<T, MD>(rowOffset, colOffset, ROW_COUNT, COL_COUNT, this->data));
		}

		template<unsigned ROW_COUNT, unsigned COL_COUNT>
		const StaticSizeMatrix<ROW_COUNT, COL_COUNT, T, SubmatrixMD<T, MD>> submatrix(unsigned rowOffset, unsigned colOffset) const {
			return StaticSizeMatrix<ROW_COUNT, COL_COUNT, T, SubmatrixMD<T, MD>>(
					SubmatrixMD<T, MD>(rowOffset, colOffset, ROW_COUNT, COL_COUNT, this->data));
		}

		Matrix<T, TransposedMD<T, MD>> transpose() {
			return Matrix<T, TransposedMD<T, MD>>(TransposedMD<T, MD>(this->data));
		}

		const Matrix<T, TransposedMD<T, MD>> transpose() const {
			return Matrix<T, TransposedMD<T, MD>>(TransposedMD<T, MD>(this->data));
		}

		Matrix<T, DiagonalMD<T, MD>> diagonal() {
			return Matrix<T, DiagonalMD<T, MD>>(DiagonalMD<T, MD>(this->data));
		}

		const Matrix<T, DiagonalMD<T, MD>> diagonal() const {
			return Matrix<T, DiagonalMD<T, MD>>(DiagonalMD<T, MD>(this->data));
		}

		/**
		* Can only be called on a vector.
		* @return an immutable diagonal square matrix that has this vector as diagonal and <code>0</code> (zero) in all other positions.
		*/
		const Matrix<T, DiagonalMatrixMD<T, MD>> diagonalMatrix() const {
			return Matrix<T, DiagonalMatrixMD<T, MD>>(DiagonalMatrixMD<T, MD>(this->data));
		}

		/**
		 * Multiplies the two given matrices
		 */
		template<class MD2>
		const Matrix<T, MultiplyMD<T, MD, MD2>> operator*(const Matrix<T, MD2> &another) const {
			return Matrix<T, MultiplyMD<T, MD, MD2>>(
					MultiplyMD<T, MD, MD2>(this->data, another.data));
		}

		/**
		 * Adds the two given matrices
		 */
		template<class MD2>
		const Matrix<T, Sum<T, MD, MD2>> operator+(const Matrix<T, MD2> &another) const {
			return Matrix<T, Sum<T, MD, MD2>>(
					Sum<T, MD, MD2>(this->data, another.data));
		}

		template<unsigned ROWS, unsigned COLUMNS, class MD2>
		const StaticSizeMatrix<ROWS, COLUMNS, T, Sum<T, MD2, MD>>
		operator+(const StaticSizeMatrix<ROWS, COLUMNS, T, MD2> &another) const {
			return another + (*this);
		}

		/**
 		 * @return true if this matrix is a square (has the same number of rows and columns)
 		 */
		bool isSquared() const {
			return rows() == columns();
		}

		/**
		 * @return true if this matrix is a vector (has only one column)
		 */
		bool isVector() const {
			return columns() == 1;
		}

		/**
		  * @return true if this matrix is a covector (has only one row)
		*/
		bool isCovector() const {
			return rows() == 1;
		}


		/**
		 * @return an iterator on the first position. This iterator moves from left to right, and then top to bottom.
		 */
		MatrixRowMajorIterator<T, MD> beginRowMajor() const {
			return MatrixRowMajorIterator<T, MD>(this->data, 0, 0);
		}

		/**
		 * @return an iterator on the last position. This iterator moves from left to right, and then top to bottom.
		 */
		MatrixRowMajorIterator<T, MD> endRowMajor() const {
			return MatrixRowMajorIterator<T, MD>(this->data, rows(), 0);
		}

		/**
		 * @return an iterator on the first position. This iterator moves from top to bottom, and then left to right.
		 */
		MatrixColumnMajorIterator<T, MD> beginColumnMajor() const {
			return MatrixColumnMajorIterator<T, MD>(this->data, 0, 0);
		}

		/**
		* @return an iterator on the last position. This iterator moves from top to bottom, and then left to right.
		*/
		MatrixColumnMajorIterator<T, MD> endColumnMajor() const {
			return MatrixColumnMajorIterator<T, MD>(this->data, 0, columns());
		}

		Matrix<T, VectorMatrixData<T>> copy() const {
			return Matrix<T, VectorMatrixData<T>>(VectorMatrixData<T>::template toVector<MD>(this->data));
		}

		template<typename U>
		Matrix<U, MatrixCaster<U, MD>> cast() const {
			return Matrix<U, MatrixCaster<U, MD>>(MatrixCaster<U, MD>(this->data));
		}

		/**
		 * Prints the content of this matrix to the standard output
		 * @param format the format string to use when printing values
		 * @param separator the separator between each column
		 */
		void print(const char *format, const char *separator = "  ") const {
			for (unsigned row = 0; row < this->rows(); ++row) {
				for (unsigned col = 0; col < this->columns(); ++col) {
					if (col > 0) {
						std::cout << separator;
					}
					//Since we are mixing cout and format, buffers need to be flushed
					std::cout.flush();
					printf(format, (T) (*this)(row, col));
					fflush(stdout);
				}
				std::cout << std::endl;
			}
		}
};

#endif //MATRIX_MATRIX_H
