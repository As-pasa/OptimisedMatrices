
#include"Matrix.h"

void Print(Matrix* a) {
	for (int i = 0;i < a->GetHeight(); i++) {
		for (int j = 0;j < a->GetWidth(); j++) {
			std::cout << a->GetConstElement(i, j)<<" ";
		}
		std::cout << "\n";
	}
}

void GetDiagonalMatrixFromConsole(Matrix* matr) {
	int size = (matr->GetHeight() > matr->GetWidth()) ? matr->GetWidth() : matr->GetHeight();
	std::cout << "insert " << size << " elements form main diagonal\n";
	for (int i = 0; i < size; i++) {
		std::cin >> matr->GetElement(i, i);
	}	
}
void GetDefaultMatrixFromConsole(Matrix* matr) {
	int size = matr->GetWidth() * matr->GetHeight();
	std::cout << " insert " << size << " elements\n";
	for (int i = 0; i < size; i++) {
		std::cin>>matr->GetElement(i / matr->GetWidth(), i % matr->GetWidth());
	}
}



int main() {
	Matrix* matr = MatrixFactory::CreateDiagonalMatrix(2, 3);
	Matrix* def = MatrixFactory::CreateDefaultMatrix(2, 2);
	GetDefaultMatrixFromConsole(def);
	GetDiagonalMatrixFromConsole(matr);
	Print(matr);
	Print(def);
	Print(def->Mult( matr));

}

//TODO обрезать костыль, в default mult и default  add берется default matrix  а не  diagonal