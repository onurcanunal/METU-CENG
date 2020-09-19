#include "the3.h"


// You  can define extra functions here

// INPUT :
//            n         : number of nodes in the graph
//            edgeList  : edges in the graph
//            scores    : importance scores
// return value : 
//                 number of disconnected components

int Important (int n, int**& edgeList, double*& scores)
{ 
	if(n){
		int i, j, k;
		std::vector<std::vector<int>> allPairsShortestPath(n, std::vector<int>(n));
		std::vector<int> connectedPairs(n);
		int inf = INT_MAX;
		double score;
		int numberOfDisconnectedComponents = 1;

		for(i = 0; i < n; i++){
			for(j = 0; j < n; j++){
				if(i != j && edgeList[i][j] == 0){
					allPairsShortestPath[i][j] = inf;
				}
				else if(i == j){
					allPairsShortestPath[i][j] = 1;
				}
				else{
					allPairsShortestPath[i][j] = edgeList[i][j];
				}
			}
		}

		for(k = 0; k < n; k++){
			for(i = 0; i < n; i++){
				for(j = i; j < n; j++){
					if(i == j || j == k || i == k){
						continue;
					}
					if(allPairsShortestPath[i][k] != inf && allPairsShortestPath[k][j] != inf){
						if(allPairsShortestPath[i][j] > allPairsShortestPath[i][k] + allPairsShortestPath[k][j]){
							allPairsShortestPath[i][j] = allPairsShortestPath[i][k] + allPairsShortestPath[k][j];
							allPairsShortestPath[j][i] = allPairsShortestPath[i][j];
						}
					}
				}
			}
		}

		for(k=0; k < n; k++){
			score = 0;
			for(i = 0; i < n; i++){
				for(j = i; j < n; j++){
					if(j == k || i == k){
						continue;
					}
					if(allPairsShortestPath[i][k] != inf && allPairsShortestPath[k][j] != inf){
						if(i == j){
							score += (allPairsShortestPath[i][k] + allPairsShortestPath[k][j]) / (double) allPairsShortestPath[i][j];
						}
						else{
							score += ((allPairsShortestPath[i][k] + allPairsShortestPath[k][j]) / (double) allPairsShortestPath[i][j]) * 2;
						}
					}

				}
			}
			scores[k] = score;
		}

		connectedPairs = allPairsShortestPath[0];

		for(i = 0; i < n; i++){
			if(connectedPairs[i] == inf){
				numberOfDisconnectedComponents++;
				for(j = i; j < n; j++){
					if(allPairsShortestPath[i][j] != inf){
						connectedPairs[j] = 1;
					}
				}
			}
		}

	   	return numberOfDisconnectedComponents; 
   	}

   	else{
		return 0;
	}
}

