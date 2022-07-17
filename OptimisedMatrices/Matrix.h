#pragma once
#include <iostream>
#include<vector>
class IMatrixOperator;
class IMatrixStorage;
class Matrix;
class MatrixFactory;


class Matrix
{
protected:
	IMatrixOperator* _multiplier;
	IMatrixOperator* _summator;
	IMatrixStorage* _data;
	//Unit matrix -> diagonal matrix -> default matrix
	void ConvertToMutable();
public:
	Matrix(IMatrixStorage* dat, IMatrixOperator* mult, IMatrixOperator* sum) :_multiplier(mult), _summator(sum), _data(dat) {};
	//Check which of 2 operators to use and executes the best
	Matrix* Add(Matrix* other);
	Matrix* Mult(Matrix* other);
	//delegate request to storage
	int GetHeight();
	int GetWidth();
	// check if element is constant, if so convert the whole matrix to more flexible type(unit->diagonal->default)
	int& GetElement(int y,int x);
	// more effective than the prev one, used when you do not need to modify the value
	const int& GetConstElement(int y,int x);
	//checks, if matrix can modify selected value
	bool IsElementMutable(int y, int x);
};

class IMatrixStorage {
public:
	virtual std::pair<bool,int&> GetElement(int y, int x)=0;
	virtual const int& GetConstElement(int y, int x)=0;
	virtual IMatrixStorage* GetMutable()=0;
	virtual int GetHeight()=0;
	virtual int GetWidth() = 0;
};

class IMatrixOperator {
protected:
	static std::pair<Matrix*, Matrix*> Normalise(Matrix*,Matrix*,bool);
public:
	virtual Matrix* Execute(Matrix* A, Matrix* B, bool IsALeft)=0;
	virtual int GetOperatorDifficulty()=0;
	virtual IMatrixOperator* GetMutable()=0;
};

class DefaultStorage :public IMatrixStorage {
protected:
	std::vector<int> _data;
	int _sizex;
	int _sizey;
public:
	DefaultStorage(int y, int x) :_sizex(x),_sizey(y),_data(x*y) {}
	DefaultStorage(int y, int x,std::vector<int> data) :_sizex(x), _sizey(y), _data(data) {}
	int GetHeight() override;
	int GetWidth() override;
	const int& GetConstElement(int y, int x) override;
	std::pair<bool, int&> GetElement(int y, int x) override;
	IMatrixStorage* GetMutable() override;
};

class DefaultMultiplier :public IMatrixOperator {
public:
	Matrix* Execute(Matrix* A, Matrix* B, bool IsALeft) override;
	int GetOperatorDifficulty() override;
	IMatrixOperator* GetMutable() override;
};

class DefaultSummator :public IMatrixOperator {
public:
	Matrix* Execute(Matrix* A, Matrix* B, bool IsALeft)override;
	int GetOperatorDifficulty()override;
	IMatrixOperator* GetMutable()override;
};

class DiagonalStorage :public DefaultStorage {
private:
	int _fictiveZero = 0;
public:
	DiagonalStorage(int y, int x) :DefaultStorage(y, x) { _data = std::vector<int>((y < x) ? y : x); };
	const int& GetConstElement(int y, int x)override;
	std::pair<bool, int&> GetElement(int y, int x)override;
	IMatrixStorage* GetMutable()override;
};

class DiagonalMultiplier :public IMatrixOperator {
public:
	Matrix* Execute(Matrix* A, Matrix* B, bool IsALeft)override;
	int GetOperatorDifficulty()override;
	IMatrixOperator* GetMutable()override;
};

class UnitStorage :public IMatrixStorage {
private:
	int _size;
	int _fictiveOne;
	int _fictiveZero;


public:
	std::pair<bool, int&> GetElement(int y, int x)override;
	const int& GetConstElement(int y, int x)override;
	IMatrixStorage* GetMutable()override;
	int GetWidth()override;
	int GetHeight()override;

};
class UnitMultiplier :public IMatrixOperator {
	Matrix* Execute(Matrix* A, Matrix* B, bool IsALeft)override;
	int GetOperatorDifficulty()override;
	IMatrixOperator* GetMutable()override;
};


class MatrixFactory {
public:
	static Matrix* CreateDefaultMatrix(int, int);
	static Matrix* CreateDiagonalMatrix(int, int);
};



