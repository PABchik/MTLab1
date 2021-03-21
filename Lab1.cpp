// Lab1.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <fstream>
using namespace std;

void readMatrixFromFile(string fileName, float**& matrix);
void printMatrix(float**& matrix, const char* msg);
float findDeterminant(float**& matrix);
void gauss(float**& matrix);

int matrixSize;


int main(int argc, const char **argv)
{
    float** matrix;

    readMatrixFromFile(argv[1], matrix);
    printMatrix(matrix, (char*) "matrix from file");
    float result = findDeterminant(matrix);
    printMatrix(matrix, (char*)"matrix after Gauss");

    cout << "\nDeterminant is " << fixed << result << "\n\n";
}


void readMatrixFromFile(string fileName, float**& matrix) {
    ifstream file(fileName);
    if (!file.is_open()) {
        cerr << "Incorrect file name";
        exit(1);
    }
    
    file >> matrixSize;
    cout << matrixSize;

    matrix = new float* [matrixSize];
    for (int i = 0; i < matrixSize; i++) {
        matrix[i] = new float[matrixSize];
        for (int j = 0; j < matrixSize; j++) {
            file >> matrix[i][j];
        }
    }
}

float findDeterminant(float**& matrix) {
    
    gauss(matrix);

    float determinant = 1;
    for (int i = 0; i < matrixSize; i++) {
        determinant *= matrix[i][i];
    }
    
    return determinant;
}

void gauss(float**& matrix) {
    float temp;

    for (int i = 0; i < matrixSize - 1; i++) {
        for (int j = i + 1; j < matrixSize; j++) {
            temp = matrix[i][i] == 0 ? 0 : -matrix[j][i] / matrix[i][i];
            for (int t = 0; t < matrixSize; t++) {
                matrix[j][t] += matrix[i][t] * temp;
            }
        }
    }
}

void printMatrix(float**& matrix, const char* msg) {
    
    cout << "\n" << msg << "\nMatrix:\n\n";
    
    for (int i = 0; i < matrixSize; i++) {
        for (int j = 0; j < matrixSize; j++) {
            cout << fixed << matrix[i][j] << " ";
        }
        cout << "\n";
    }
    cout << "\n";
}
// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
