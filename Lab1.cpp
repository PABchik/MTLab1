#include <iostream>
#include <fstream>
#include <omp.h>
#include <string>
#include <ctime>

using namespace std;

int readMatrixFromFile(string fileName, float**& matrix);
void printMatrix(float**& matrix, const char* msg);
float findDeterminant(float**& matrix);
void findDeterminantAndWriteResults(float**& matrix, float& avgTime);
void gauss(float**& matrix);

void testAMinusOne(float**& matrix);
void testAMinusOne5(float**& matrix);
void testStaticParam(float**& matrix);

int matrixSize;
int a;
int numThreads = 0;
int dynamic = 2;

int main(int argc, const char **argv)
{
    if (argc != 3) {
        cerr << "Incorrect program arguments used. There should be 2 argumets: input_file_name and threads number [-1;" << omp_get_max_threads() << "]";
        return 1;
    }

    float** matrix;
    a = stoi(argv[2]);

    if (a < -1) {
        cerr << "Incorrect threads number. There is allowed only values from -1 until " << omp_get_max_threads();
        return 1;
    } else if (a > omp_get_max_threads()) {
        cerr << "Incorrect threads number. You can use only " << omp_get_max_threads() << " threads";
        return 1;
    }
    //a = 7;
    if (a == -1) {
        numThreads = 1;
    } else {
        numThreads = a == 0 ? omp_get_max_threads() : a;
    }

    if (readMatrixFromFile(argv[1], matrix) == 1) {
        return 1;
    }

    clock_t start = clock();
    float result = findDeterminant(matrix);
    float execTime = clock() - start;

    printf("Determinant: %g\n", result);
    printf("\nTime (%i thread(s)) %f ms\n", a == -1 ? a : numThreads, execTime);
    
    //testAMinusOne5(matrix);
    //testAMinusOne(matrix);
    //testStaticParam(matrix);

    for (int i = 0; i < matrixSize; i++) 
        delete[] matrix[i];
    delete[] matrix;
    return 0;
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

void testStaticParam(float**& matrix) {
    for (int k = 2; k < 9; k++) {
        numThreads = k;
        for (int j = 1; j < 11; j++) {
            dynamic = j;
            float avgTime = 0;
            for (int i = 0; i < 10; i++)
                findDeterminantAndWriteResults(matrix, avgTime);
            cout << "\n\nstatic param is: " << j << " num threads: " << k << " Time:  " << avgTime / 100 << "\n\n";
            
        }
    }
}

void testAMinusOne(float**& matrix) {
    for (int k = -1; k < 2; k += 2) {
        a = k;
        numThreads = 1;
        float avgTime = 0;
        for (int i = 0; i < 100; i++)
            findDeterminantAndWriteResults(matrix, avgTime);
        cout << "\n\n num threads: " << k << " Time:  " << avgTime / 100 << "\n\n";
    }
}

void testAMinusOne5(float**& matrix) {
    float avgTime = 0;
    float avgTime1 = 0;
    for (int i = 0; i < 5; i++) {
        for (int k = -1; k < 2; k += 2) {
            a = k;
            numThreads = 1;
            

            findDeterminantAndWriteResults(matrix, k == -1? avgTime : avgTime1);
            
        }
    }
    cout << "\n\n num threads: " << -1 << " AVG Time:  " << avgTime / 100 << "\n\n";
    cout << "\n\n num threads: " << 1 << " AVG Time:  " << avgTime1 / 100 << "\n\n";
}

void findDeterminantAndWriteResults(float**& matrix, float& avgTime) {
    clock_t start = clock();
    float result = findDeterminant(matrix);
    float execTime = clock() - start;
    
    //printf("\nNum threads: %d   executed for %f\n", a, execTime);
    avgTime += execTime;
}

int readMatrixFromFile(string fileName, float**& matrix) {
    ifstream file(fileName);
    if (!file.is_open()) {
        cerr << "Incorrect file name";
        return 1;
    }
    
    file >> matrixSize;

    try {
        matrix = new float* [matrixSize];
        for (int i = 0; i < matrixSize; i++) {
            matrix[i] = new float[matrixSize];
            for (int j = 0; j < matrixSize; j++) {
                file >> matrix[i][j];
            }
        }
    }
    catch (bad_alloc& ba) {
        cerr << "Error. Bad Alloc " << ba.what() << "\n";
        return 1;
    }
    return 0;
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

//use this for static param test
//#pragma omp parallel for if(a > -1) num_threads(numThreads) schedule(static, dynamic)
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
