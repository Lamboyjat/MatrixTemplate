//
// Created by Lamin Jatta
//

#ifndef MATRIXTEMPLATE_MATRIXUTILS_H
#define MATRIXTEMPLATE_MATRIXUTILS_H

#include <future>

template<typename T, class O>
class OptimizableMD : public MatrixData<T> {
protected:
    mutable std::mutex optimizeMutex; //Mutex for the method optimize()
private:
    mutable std::shared_future<std::unique_ptr<O>> optimized;
    //I'm saving the pointer to optimized matrix in order to skip accessing it through a future and a unique_ptr
    mutable O *optimizedPointer = NULL;

public:

    OptimizableMD(unsigned int rows, unsigned int columns) :
            MatrixData<T>(rows, columns) {
    }

    OptimizableMD(const OptimizableMD<T, O> &another) :
            MatrixData<T>(another.rows(), another.columns()) {
        //The cached data is not passed around, since it will be too difficult to copy
        if (another.optimizedPointer != NULL) {
            std::cout << "Warning: cached data is lost!\n";
        }
    }

    OptimizableMD(OptimizableMD<T, O> &&another) noexcept :
            MatrixData<T>(another.rows(), another.columns()) {
        //The cached data is not passed around, since it will be too difficult to move
    }

    virtual ~OptimizableMD() {
        if (this->optimized.valid()) {
            this->optimized.wait();
        }
    }

    MATERIALIZE_IMPL

    void virtualWaitOptimized() const override {
        MatrixData<T>::virtualWaitOptimized();
        auto future = this->optimized;
        if (future.valid()) {
            future.wait();
        }
        if (this->optimizedPointer != NULL) {
            this->optimizedPointer->virtualWaitOptimized();
        }
    }

    void virtualOptimize() const override {
        this->optimize();
    }

    void optimize() const {
        std::unique_lock<std::mutex> lock(this->optimizeMutex);
        if (!this->optimizeHasBeenCalled) {
            this->optimized = std::async(std::launch::async, [=] {
                auto ptr = this->virtualCreateOptimizedMatrix();
                ptr->virtualOptimize();
                return ptr;
            }).share();
            this->optimizeHasBeenCalled = true;
        }
    }

private:
    T doGet(unsigned row, unsigned col) const {
        if (this->optimizedPointer == NULL) {
            //I'm saving the pointer to optimized matrix in order to skip accessing it through a future and a unique_ptr
            this->optimizedPointer = optimized.get().get();
        }
        return this->optimizedPointer->get(row, col);
    }


protected:

    /**
     * This method optimizes the multiplication if the multiplication chain involves more than three matrix.
     */
    virtual std::unique_ptr<O> virtualCreateOptimizedMatrix() const = 0;
};


template<typename T>
class MaterializerMD : public OptimizableMD<T, VectorMatrixData<T>> {
private:
    const MatrixData<T> *wrapped;
    unsigned rowOffset, colOffset;

public:
    MaterializerMD(const MatrixData<T> *wrapped, unsigned rowOffset, unsigned colOffset, unsigned rows, unsigned columns)
            : OptimizableMD<T, VectorMatrixData<T>>(rows, columns), rowOffset(rowOffset), colOffset(colOffset), wrapped(wrapped) {
    }

protected:
    std::unique_ptr<VectorMatrixData<T>> virtualCreateOptimizedMatrix() const override {
    auto materialized = this->wrapped->virtualMaterialize(rowOffset, colOffset, this->rows(), this->columns());
    return std::make_unique<VectorMatrixData<T>>(materialized);
    }

    std::vector<const MatrixData<T> *> virtualGetChildren() const override {
    return {this->wrapped};
    }
};

#endif //MATRIXTEMPLATE_MATRIXUTILS_H
