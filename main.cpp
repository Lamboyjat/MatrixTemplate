#include <iostream>
#include <cassert>
/*#include "assert.h"*/
#include "Matrix.h"
#include "StaticMatricSize.h"


template<typename T, class MD>
void initializeCells(Matrix<T, MD> &m, T rowMultiplier, T colMultiplier) {
    for (unsigned row = 0; row < m.rows(); ++row) {
        for (unsigned col = 0; col < m.columns(); ++col) {
            m(row, col) = row * rowMultiplier + col * colMultiplier;
        }
    }
}

template<typename T>
void cassert(T expected, T actual) {
        if (expected != actual) {
            std::cout << "ERROR: expected " << expected << ", got " << actual << std::endl;
            exit(1);
        }
};

template<typename T, class MD1, class MD2>
void assertEqual(const Matrix<T, MD1> &m1, const Matrix<T, MD2> &m2) {
    if (m1.rows() != m2.rows() || m1.columns() != m2.columns()) {
        std::cout << "ERROR: expected matrix of the same size" << std::endl;
        exit(1);
    }
    for (unsigned r = 0; r < m1.rows(); ++r) {
        for (unsigned c = 0; c < m2.columns(); ++c) {
            cassert(m1(r, c), m2(r, c));
        }
    }
}

//TESTING OTHER METHOD COMPONENT
template<typename T, class MD>
void assertAll(T expected, const Matrix<T, MD> &m) {
    for (unsigned r = 0; r < m.rows(); ++r) {
        for (unsigned c = 0; c < m.columns(); ++c) {
            cassert<T>(expected, m(r, c));
        }
    }
}

template<typename T, class MD1, class MD2>
void assertEquals(const Matrix<T, MD1> &m1, const Matrix<T, MD2> &m2) {
    cassert(m1.columns(), m2.columns());
    cassert(m1.rows(), m2.rows());

    for (unsigned r = 0; r < m1.rows(); ++r) {
        for (unsigned c = 0; c < m1.columns(); ++c) {
            cassert(m1(r, c), m2(r, c));
        }
    }
}

template<class IT>
void testIterator(IT begin, IT end, int size) {
    int k = 0;
    for (auto it = begin; it != end; ++it) {
        cassert(++k, *it);
    }
    cassert(size, k);

    for (auto it = end; it != begin;) {
        --it;
        cassert(k--, *it);
    }
    cassert(0, k);
}

template<typename T, class MD>
void dirtify(Matrix<T, MD> m) {
    for (unsigned r = 0; r < m.rows(); r++) {
        for (unsigned c = 0; c < m.columns(); c++) {
            m(r, c) = 18;
        }
    }
}

template<typename T, class MD>
void dirtify2(Matrix<T, MD> &m) {
    for (unsigned r = 0; r < m.rows(); r++) {
        for (unsigned c = 0; c < m.columns(); c++) {
            m(r, c) = 18;
        }
    }
}

//Implement a template to hold all the method
template<typename T, class MD>
void test(Matrix<T, MD> &m) {
    assertAll(0, m);
    dirtify<T, MD>(m);//Testing deep copy
    assertAll(0, m);
    dirtify2<T, MD>(m);//Testing move
    assertAll(18, m);
    //TEST ASSIGNMENTS
    for (int k = 0; k < 10; ++k) {
        for (unsigned r = 0; r < m.rows(); ++r) {
            for (unsigned c = 0; c < m.columns(); ++c) {
                m(r, c) = k;
                cassert<T>(k, m(r, c));
            }
        }
    }

    //ASSIGN VALUES FROM [1..columns*rows] in row major order
    int k = 0;
    for (unsigned r = 0; r < m.rows(); ++r) {
        for (unsigned c = 0; c < m.columns(); ++c) {
            k++;
            m(r, c) = k;
            cassert<T>(k, m(r, c));
        }
    }

    //TEST ROW-MAJOR ITERATOR
    testIterator(m.beginRowMajor(), m.endRowMajor(), m.size());


    //ASSIGN VALUES FROM [1..columns*rows] in column major order
    k = 0;
    for (unsigned c = 0; c < m.columns(); ++c) {
        for (unsigned r = 0; r < m.rows(); ++r) {
            k++;
            m(r, c) = k;
            cassert<T>(k, m(r, c));
        }
    }

    //TEST COLUMN-MAJOR ITERATOR
    testIterator(m.beginColumnMajor(), m.endColumnMajor(), m.size());


    //TEST TRANSPOSE
    if (m.rows() >= 4 && m.columns() >= 3) {
        m.transpose()(2, 3) = 76;
        cassert<T>(76, m(3, 2));
    }
    assertEquals(m, m.transpose().transpose());

    //TEST DIAGONAL
    if (m.isSquared()) {
        m.diagonal()(2, 0) = 54;
        cassert<T>(54, m(2, 2));

        assertEquals(m.diagonal(), m.diagonal().diagonalMatrix().diagonal());
    }

    //TEST DIAGONAL MATRIX
    if (m.isVector()) {
        auto d = m.diagonalMatrix();
        cassert(m.rows(), d.columns());
        cassert(m.rows(), d.rows());

        for (unsigned r = 0; r < d.rows(); ++r) {
            for (unsigned c = 0; c < d.columns(); ++c) {
                if (r == c) {
                    cassert<T>(m(r, 0), d(r, c));
                } else {
                    cassert<T>(0, d(r, c));
                }
            }
        }
    }

    //TEST SUBMATRIX
    if (m.rows() >= 4 && m.columns() >= 4) {
        auto sm = m.submatrix(2, 3, 2, 1);
        cassert<T>(m(2, 3), sm(0, 0));
        cassert<T>(m(3, 3), sm(1, 0));

        sm(0, 0) = 123;
        cassert<T>(123, m(2, 3));
    }

    assertEquals(m, m.submatrix(0, 0, m.rows(), m.columns()));

    //Cleaning the matrix
    for (unsigned r = 0; r < m.rows(); r++) {
        for (unsigned c = 0; c < m.columns(); c++) {
            m(r, c) = 0;
        }
    }
}


void testBasicStuff() {
    Matrix<int> sq(10, 10);
    StaticSizeMatrix<10, 10, int> sqStatic;
    StaticSizeMatrix<15, 20, int> static1;
    StaticSizeMatrix<20, 3, int> static2;
    Matrix<int> rect(5, 10);
    Matrix<int> vector(10, 1);
    auto multiplied1 = (sq * sq.transpose()).copy();
    StaticSizeMatrix<5, 5, int> multiplied2 = (sqStatic * sqStatic.transpose()).diagonal().diagonalMatrix().submatrix<5, 5>(1, 1).copy();

    auto transposed = static1.transpose();
    auto submatrix = static1.submatrix<6, 7, 4, 5>();
    auto submatrix2 = rect.submatrix<4, 5>(1, 4);

    int read = static1.get<7, 0>();
    static1.get<1, 0>() = read;

    test<int>(submatrix);
    test<int>(submatrix2);
    test<int>(sq);
    test<int>(static1);
    test<int>(transposed);
    test<int>(multiplied1);
    test<int>(multiplied2);
    test<int>(rect);
    test<int>(vector);
}







int main() {
    std::cout << "Testing, Matrix Multiplication! A, B, C, D" << std::endl;

    StaticSizeMatrix<4, 9, double> mAd;
    StaticSizeMatrix<4, 9, int> mA;
    StaticSizeMatrix<9, 7, int> mB;
    StaticSizeMatrix<7, 8, int> mC;
    StaticSizeMatrix<8, 2, int> mD;
    initializeCells(mA, 12, 5);
    initializeCells<double>(mAd, 12, 5);

    initializeCells(mB, 7, 13);
    initializeCells(mC, 3, 8);
    initializeCells(mD, 2, 4);

    std::cout << "Matrix A" << std::endl;
    mA.print("%2d");
    std::cout << "-------------------------" << std::endl << std::endl;

    std::cout << "Matrix B" << std::endl;
    mB.print("%2d");
    std::cout << "-------------------------" << std::endl << std::endl;

    std::cout << "Matrix C" << std::endl;
    mC.print("%2d");
    std::cout << "-------------------------" << std::endl << std::endl;

    std::cout << "Matrix D" << std::endl;
    mD.print("%2d");
    std::cout << "-------------------------" << std::endl << std::endl;

    auto multiplicationAB = mA * mB;
    auto multiplicationBC = mB * mC;
    auto multiplicationCD = mC * mD;
    auto multiplicationABC = multiplicationAB * mC;
    auto multiplicationABCD = multiplicationAB * multiplicationCD;
    auto multiplicationABCD2 = mA * mB * mC * mD;

    auto sum = mA + mAd.cast<int>();

    std::cout << "Matrix AxBxC" << std::endl;
    multiplicationABC.print("%2d"); // This should perform AxB and (AxB)xC
    std::cout << "-------------------------" << std::endl << std::endl;

    std::cout << "Matrix AxB" << std::endl;
    multiplicationAB.print("%2d"); //This shouldn't perform any multiplication, since AxB is already computed
    std::cout << "-------------------------" << std::endl << std::endl;

    std::cout << "Matrix AxBxCxD" << std::endl;
    multiplicationABCD.print("%4d");//This should compute the multiplication (AxB)xC and (AxBxC)xD
    std::cout << "-------------------------" << std::endl << std::endl;

    std::cout << "Matrix AxBxCxD computed from scratch" << std::endl;
    multiplicationABCD2.print("%4d");//This should perform AxB, CxD and (AxB)x(CxD)
    std::cout << "-------------------------" << std::endl << std::endl;

    /*assert<int>(1034658912, multiplicationABCD.get<3, 1>());*/
    assertEqual(multiplicationABCD, multiplicationABCD2);
    std::cout << "ALL TESTS PASSED FOR MULTIPLICATION" << std::endl;


    std::cout << "START TEST" << std::endl;

    std::cout << "Matrix Sum computed from scratch" << std::endl;
    sum.print("%4d");//This should perform AxB, CxD and (AxB)x(CxD)
    std::cout << "-------------------------" << std::endl << std::endl;

    std::cout << "Matrix A+B computed from scratch" << std::endl;
    //This should perform AxB, CxD and (AxB)x(CxD)
    std::cout << "-------------------------" << std::endl << std::endl;

    std::cout << "Testing basic stuff" << std::endl;

    testBasicStuff();


    return 0;
}