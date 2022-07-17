#include "Matrix.h"

void Matrix::ConvertToMutable()
{
    IMatrixOperator* NewSummator = _summator->GetMutable();
    delete _summator;
    _summator = NewSummator;
    IMatrixOperator* NewMultiplier= _multiplier->GetMutable();
    delete _multiplier;
    _multiplier = NewMultiplier;
    IMatrixStorage* NewStorage = _data->GetMutable();
    delete _data;
    _data = NewStorage;
}

Matrix* Matrix::Add(Matrix* other)
{
    if (_summator->GetOperatorDifficulty() <= other->_summator->GetOperatorDifficulty()) {
        return _summator->Execute(this, other, true);
    }
    else {
        return other->_summator->Execute(other, this, false);
    }
}

Matrix* Matrix::Mult(Matrix* other)
{
    if (_multiplier->GetOperatorDifficulty() <= other->_multiplier->GetOperatorDifficulty()) {
        return _multiplier->Execute(this, other, true);
    }
    else {
        return other->_multiplier->Execute(other, this, false);
    }
}

int Matrix::GetHeight()
{
    return _data->GetHeight();
}

int Matrix::GetWidth()
{
    return _data->GetWidth();
}

 int& Matrix::GetElement(int y, int x)
{
     std::pair<bool, int&> reqw = _data->GetElement(y, x);
     if (reqw.first) {
         return reqw.second;
     }
     else {
         ConvertToMutable();
         return _data->GetElement(y, x).second;
     }
    
}

const int& Matrix::GetConstElement(int y, int x)
{
    return _data->GetConstElement(y, x);
}

bool Matrix::IsElementMutable(int y, int x)
{
    return _data->GetElement(y,x).first;
}

int DefaultStorage::GetHeight()
{
    return _sizey;
}

int DefaultStorage::GetWidth(){
    return _sizex;
}

const int& DefaultStorage::GetConstElement(int y, int x)
{
    return GetElement(y, x).second;
}

std::pair<bool,int&> DefaultStorage::GetElement(int y, int x)
{
    if (y >= GetHeight() || y < 0) {
        throw std::overflow_error("unexpected y index");
    }
    if (x >= GetWidth() || x < 0) {
        throw std::overflow_error("unexpected x index");
    }
    return std::pair<bool,int&> (true, _data[y * GetWidth() + x]);
}

IMatrixStorage* DefaultStorage::GetMutable()
{
    return this;
}

Matrix* DefaultMultiplier::Execute(Matrix* A, Matrix* B, bool IsALeft)
{
    std::pair<Matrix*,Matrix*> args =IMatrixOperator::Normalise(A, B, IsALeft);
    Matrix* answer = MatrixFactory::CreateDiagonalMatrix(args.first->GetHeight(),args.second->GetWidth());
    int summary = 0;
    for (int i = 0; i < args.first->GetHeight(); i++) {
        for (int j = 0; j < args.second->GetWidth(); j++) {
            summary = 0;
            for (int sumIndex = 0; sumIndex < args.first->GetWidth(); sumIndex++) {
                summary += (args.first->GetConstElement(i, sumIndex) * args.second->GetConstElement(sumIndex, j));
            }
            answer->GetElement(i, j) = summary;
        }
    }
    return answer;
}

int DefaultMultiplier::GetOperatorDifficulty()
{
    return 3;
}

IMatrixOperator* DefaultMultiplier::GetMutable()
{
    return this;
}

std::pair<Matrix*, Matrix*> IMatrixOperator::Normalise(Matrix*A, Matrix*B, bool IsALeft)
{

    Matrix* left = B;
    Matrix* right = A;
    if (IsALeft) {
        left = A;
        right = B;
    }
    if (left->GetWidth() != right->GetHeight()) {
        throw std::runtime_error("UnexpectedMatrixSize");
    }

    return std::pair<Matrix*, Matrix*>(left,right);
}

Matrix* DefaultSummator::Execute(Matrix* A, Matrix* B, bool IsALeft)
{
    if (A->GetHeight() != B->GetHeight() || A->GetWidth() != B->GetWidth()) {
        throw std::runtime_error("UnexpectedMatrixSize");
    }
    Matrix* answer = MatrixFactory::CreateDefaultMatrix(A->GetHeight(), A->GetWidth());
    for (int i = 0; i < A->GetHeight(); i++) {
        for (int j = 0; j < A->GetWidth(); j++) {
            answer->GetElement(i, j) =( A->GetConstElement(i, j) + B->GetConstElement(i, j));
        }
    }
    return answer;
}

int DefaultSummator::GetOperatorDifficulty()
{
    return 2;
}

IMatrixOperator* DefaultSummator::GetMutable()
{
    return this;
}

Matrix* MatrixFactory::CreateDefaultMatrix(int szy, int szx)
{
    IMatrixStorage* strj = new DefaultStorage(szy, szx);
    IMatrixOperator* defmult = new DefaultMultiplier();
    IMatrixOperator* defadd = new DefaultSummator();
    Matrix* ans = new Matrix(strj, defmult, defadd);
    return ans;
}

Matrix* MatrixFactory::CreateDiagonalMatrix(int y, int x)
{
    DiagonalStorage* dat = new DiagonalStorage(y, x);
    DiagonalMultiplier* mult = new DiagonalMultiplier();
    DefaultSummator* summ = new DefaultSummator();
    Matrix* ans = new Matrix(dat, mult, summ);
    return ans;

}

const int& DiagonalStorage::GetConstElement(int y, int x)
{
    if (y < 0 || y >= GetHeight()) {
        throw std::runtime_error("Unexpected y size");
    }
    if (x < 0 || x >= GetWidth()) {
        throw std::runtime_error("Unexpected x size");
    }
    if (x != y) { return 0; }
    return _data[x];
}

std::pair<bool, int&> DiagonalStorage::GetElement(int y, int x)
{
    if (y < 0 || y >= GetHeight()) {
        throw std::runtime_error("Unexpected y index");
    }
    if (x < 0 || x >= GetWidth()) {
        throw std::runtime_error("Unexpected x index");
    }
    if (x != y) { return std::pair<bool, int&>(false, _fictiveZero); }
    return std::pair<bool, int&>(true, _data[x]);
}

IMatrixStorage* DiagonalStorage::GetMutable()
{
    DefaultStorage* answer = new DefaultStorage(GetHeight(), GetWidth());
    for (int i = 0; i < _data.size(); i++){
        answer->GetElement(i, i).second = _data[i];
    }
    return answer;
}

Matrix* DiagonalMultiplier::Execute(Matrix* A, Matrix* B, bool IsALeft)
{
    //умножение слева
    Matrix* answer;
    if (IsALeft) {
        if (A->GetWidth() != B->GetHeight()) {
            throw std::runtime_error("UnexpectedMatrixSize");
        }
        answer = MatrixFactory::CreateDiagonalMatrix(A->GetHeight(), B->GetWidth());
        int Yiter = 0;
        while (Yiter<A->GetHeight() && Yiter<A->GetWidth())
        {
            for (int i = 0; i < B->GetWidth(); i++) {
                int summ= B->GetConstElement(Yiter, i) * A->GetConstElement(Yiter, Yiter);
                answer->GetElement(Yiter, i) = summ;
            }
            Yiter++;
        }
        return answer;
    }
    //умножение справа
    else {
        if (A->GetHeight() != B->GetWidth()) {
            throw std::runtime_error("unexpectedMatrixSize");
        }
        answer = MatrixFactory::CreateDiagonalMatrix(B->GetHeight(), A->GetWidth());
        
        for (int i = 0; i < B->GetHeight(); i++) {
            int Xiter = 0;
            while (Xiter < A->GetHeight() && Xiter < A->GetWidth()) {
                answer->GetElement(i, Xiter) = B->GetConstElement(i, Xiter) * A->GetConstElement(Xiter, Xiter);
                Xiter++;
            }
        }
        return answer;
    }
}

int DiagonalMultiplier::GetOperatorDifficulty()
{
    return 2;
}

IMatrixOperator* DiagonalMultiplier::GetMutable()
{
    return new DefaultMultiplier();
}

std::pair<bool, int&> UnitStorage::GetElement(int y, int x)
{
    if (y < 0 || y >= GetHeight()) {
        throw std::runtime_error("Unexpected y size");
    }
    if (x < 0 || x >= GetWidth()) {
        throw std::runtime_error("Unexpected x size");
    }
    _fictiveOne = 1;
    _fictiveZero = 0;
    if(x==y)return std::pair<bool, int&>(false, _fictiveOne);
    return std::pair<bool, int&>(false, _fictiveZero);
    
}

const int& UnitStorage::GetConstElement(int y, int x)
{
    if (y < 0 || y >= GetHeight()) {
        throw std::runtime_error("Unexpected y size");
    }
    if (x < 0 || x >= GetWidth()) {
        throw std::runtime_error("Unexpected x size");
    }
    if (x != y) { return 0; }
    return 1;
}

IMatrixStorage* UnitStorage::GetMutable()
{
    IMatrixStorage* ans = new DiagonalStorage(GetHeight(), GetWidth());
    for (int i = 0; i < GetHeight(); i++) { ans->GetElement(i, i).second = 1; }
    return ans;
}

int UnitStorage::GetWidth()
{
    return _size;
}

int UnitStorage::GetHeight()
{
    return _size;
}

Matrix* UnitMultiplier::Execute(Matrix* A, Matrix* B, bool IsALeft)
{
    if (IsALeft) {
        Matrix* ans = MatrixFactory::CreateDiagonalMatrix(B->GetHeight(), B->GetWidth());
        for (int i = 0; i < B->GetHeight(); i++) {
            for (int j = 0; j < B->GetWidth(); j++) {
                ans->GetElement(i, j) = B->GetConstElement(i, j);
            }
        }
        return ans;
    }
    else {
        Matrix* ans = MatrixFactory::CreateDiagonalMatrix(A->GetHeight(), A->GetWidth());
        for (int i = 0; i < A->GetHeight(); i++) {
            for (int j = 0; j < A->GetWidth(); j++) {
                ans->GetElement(i, j) = A->GetConstElement(i, j);
            }
        }
        return ans;
    }
}

int UnitMultiplier::GetOperatorDifficulty()
{
    return 2;
}

IMatrixOperator* UnitMultiplier::GetMutable()
{
    return new DiagonalMultiplier();
}
