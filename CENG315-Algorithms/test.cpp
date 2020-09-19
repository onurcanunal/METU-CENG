#include <iostream>
#include <vector>
#include <random>
#include <ctime>
#include <algorithm>
#include <cassert>
#include "the3.h"

void printVector(std::vector<int>& array){

    std::cout << "size: " << array.size() << std::endl;
    for (unsigned i = 0; i < array.size(); ++i){
        std::cout << array[i] << " ";
    }
    std::cout << std::endl;
}

int Important (int n, int**& edgeList, double*& scores);

bool TestTask()
{
    
  clock_t begin;
  clock_t end;
  double duration;
  int expectedTime = 500;
  int true_answer = -1;
  int solution = -1;
  
  if ((begin = clock ()) == -1) 
      std::cerr << "clock err" << std::endl;
  int n = 6;
  int** edgeList = new int*[n];
  for(int i =0; i<n; i++){
    edgeList[i] = new int[n];
  }
  int arr[][6] = {0, 0, 0, 3, 0, 2,
                  0, 0, 3, 0, 2, 0,
                  0, 3, 0, 0, 0, 0,
                  3, 0, 0, 0, 0, 4,
                  0, 2, 0, 0, 0, 0,
                  2, 0, 0, 4, 0, 0};

  for(int i=0; i<n; i++){
      for(int j=0; j<n; j++){
        edgeList[i][j] = arr[i][j];
      }
    }

  double *scores = new double[n];                
  solution =Important (n, edgeList, scores);


  if ((end = clock ()) == -1) 
      std::cerr << "clock err" << std::endl;
      
  duration = ((double)end-begin) / CLOCKS_PER_SEC * 1000000;
  
  std::cout << "Elapsed time  " << duration << " vs " << expectedTime << " microsecs" << std::endl;

  return (solution == true_answer);
}





int main()
{

    if (TestTask())
      std::cout << "PASSED TASK" << std::endl << std::endl;
    else
      std::cout << "FAILED TASK" << std::endl << std::endl;
                        
    return 0;
}
