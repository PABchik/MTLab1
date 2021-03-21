#include <iostream>
#include <fstream>
#include <omp.h>
#include <string>

using namespace std;

void readMatrixFromFile(string fileName, float**& matrix);
void printMatrix(float**& matrix, const char* msg);
float findDeterminant(float**& matrix);
void findDeterminantAndWriteResults(float**& matrix, float& avgTime);
void gauss(float**& matrix);

int matrixSize;
int a;
int numThreads = 0;
int dynamic = 2;

int main(int argc, const char **argv)
{
    if (argc != 3) {
        cerr << "Incorrect program arguments used. There should be 2 argumets: input_file_name and threads number [-1;" << omp_get_max_threads() << "]";
        exit(1);
    }

    float** matrix;
    a = stoi(argv[2]);

    if (a < -1) {
        cerr << "Incorrect threads number. There is allowed only values from -1 until " << omp_get_max_threads();
        exit(1);
    } else if (a > omp_get_max_threads()) {
        cerr << "Incorrect threads number. You can use only " << omp_get_max_threads() << " threads";
        exit(1);
    }
    //a = 0;
    if (a == -1) {
        numThreads = 1;
    } else {
        numThreads = a == 0 ? omp_get_max_threads() : a;
    }

    readMatrixFromFile(argv[1], matrix);

    clock_t start = clock();
    float result = findDeterminant(matrix);
    float execTime = clock() - start;

    printf("Determinant: %g\n", result);
    printf("\nTime (%i thread(s)) %f ms\n", a == -1 ? a : numThreads, execTime);
    
    for (int i = 0; i < matrixSize; i++) 
        delete[] matrix[i];
    delete[] matrix;
    exit(0);
}

void testDynamicParam(float**& matrix) {
    for (int k = 2; k < 9; k++) {
        numThreads = k;
        for (int j = 1; j < 11; j++) {
            dynamic = j;
            float avgTime = 0;
            for (int i = 0; i < 100; i++)
                findDeterminantAndWriteResults(matrix, avgTime);
            cout << "\n\ndynamic param is: " << j << " num threads: " << k << " Time:  " << avgTime / 100 << "\n\n";
        }
    }
}

void findDeterminantAndWriteResults(float**& matrix, float& avgTime) {
    clock_t start = clock();
    float result = findDeterminant(matrix);
    float execTime = clock() - start;
    avgTime += execTime;
}

void readMatrixFromFile(string fileName, float**& matrix) {
    ifstream file(fileName);
    if (!file.is_open()) {
        cerr << "Incorrect file name";
        exit(1);
    }
    
    file >> matrixSize;

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

    float* resultsArr = new float[numThreads];
    for (int i = 0; i < numThreads; i++) {
        resultsArr[i] = 1;
    }

#pragma omp parallel if (a > -1) num_threads(numThreads)
    {
        float localResult = 1;

#pragma omp for schedule(dynamic)
        for (int i = 0; i < matrixSize; i++) {
            localResult *= matrix[i][i];
        }
        resultsArr[omp_get_thread_num()] = localResult;
    }
    float determinant = 1;

    for (int i = 0; i < numThreads; i++) {
        determinant *= resultsArr[i];
    }

    delete[] resultsArr;
    return determinant;
}

void gauss(float**& matrix) {
    
    for (int i = 0; i < matrixSize - 1; i++) {        
#pragma omp parallel for if(a > -1) num_threads(numThreads) schedule(dynamic, dynamic)
        for (int j = i + 1; j < matrixSize; j++) {
            float temp;
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
            cout.width(8);
            cout.precision(2);
            cout << fixed << matrix[i][j] << " ";
        }
        cout << "\n";
    }
    cout << "\n";
}
