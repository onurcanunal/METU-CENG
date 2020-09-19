#include<stdio.h>
#include<pthread.h>
#include "monitor.h"
#include <unistd.h>

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

using namespace std;

int idle_time;
int travel_time;
int in_out_time;
bool ended = false;

struct personProp {
    int weight;
    int initialFloor;
    int destinationFloor;
    int priority;
    bool entered;
};

vector<personProp> allPersons;

class Elevator:public Monitor{   
    int npersons; // number of persons
    int nfloors;  // number of floors that the building has
    int personCapacity;
    int numberOfPersonInTheElevator;
    int weightCapacity;
    int totalWeightInTheElevator; 
    int currentFloor;
    int carriedCount;
    int currentState;

    Condition emptyDestinationQueue;
    Condition canMakeRequest;
    Condition canEnter;
    Condition canLeave;
    Condition priorityCondition;
    
    vector<int> destinationQueue;
    vector<int> personQueue;
    vector<int> floorPriorityCount;

public:
    Elevator(int np, int nf, int pc, int wc) : emptyDestinationQueue(this), canMakeRequest(this), canEnter(this), canLeave(this), priorityCondition(this){
        npersons = np;
        nfloors = nf;
        personCapacity = pc;
        weightCapacity = wc;
        numberOfPersonInTheElevator = 0;
        totalWeightInTheElevator = 0;
        currentFloor = 0;
        currentState = 0;
        carriedCount = 0;
        floorPriorityCount.resize(nf);
    }

    void insertIntoPersonQueue(int newPersonId){
        if(personQueue.empty()){
            personQueue.push_back(newPersonId);
        }
        else{
            int personDestinationFloor = allPersons[newPersonId].destinationFloor;
            int personIndex = 0;
            int queueLength = personQueue.size();
            if(currentState == 1){
                for(int i=0; i<queueLength; i++){
                    if(personDestinationFloor > allPersons[personQueue[i]].destinationFloor){
                        personIndex++;
                    }
                    else{
                        break;
                    }
                }
            }
            else if(currentState == -1){
                for(int i=0; i<queueLength; i++){
                    if(personDestinationFloor < allPersons[personQueue[i]].destinationFloor){
                        personIndex++;
                    }
                    else{
                        break;
                    }
                }
            }
            personQueue.insert(personQueue.begin() + personIndex, newPersonId);
        }
    }

    void insertIntoDestinationQueue(int newDestination){
        if(destinationQueue.empty()){
            if(currentFloor != newDestination){
                destinationQueue.push_back(newDestination);
            }
        }
        else{
            int destinationIndex = 0;
            int queueLength = destinationQueue.size();
            
            if(currentState == 1){
                for(int i=0; i<queueLength; i++){
                    if(newDestination > destinationQueue[i]){
                        destinationIndex++;
                    }
                    else{
                        break;
                    }
                }
            }
            else if(currentState == -1){
                for(int i=0; i<queueLength; i++){
                    if(newDestination < destinationQueue[i]){
                        destinationIndex++;
                    }
                    else{
                        break;
                    }
                }
            }

            if(newDestination != currentFloor && destinationIndex == destinationQueue.size()){
                destinationQueue.push_back(newDestination);
            }
            else if(newDestination != currentFloor && newDestination != destinationQueue[destinationIndex]){
                destinationQueue.insert(destinationQueue.begin() + destinationIndex, newDestination);
            }
        }
    }

    void showPersonInformation(int id, string actionType){
        int weight = allPersons[id].weight;
        int personInitialFloor = allPersons[id].initialFloor;
        int personDestinationFloor = allPersons[id].destinationFloor;
        string priority = (allPersons[id].priority == 1) ? "hp" : "lp";
        cout << "Person (" << id << ", " << priority << ", " << personInitialFloor << " -> " << personDestinationFloor << ", " << weight << ") ";
        if(actionType == "request"){
            cout << "made a request" << endl;
        }
        else if(actionType == "enter"){
            cout << "entered the elevator" << endl;
        }
        else if(actionType == "leave"){
            cout << "has left the elevator" << endl;
        }
    }

    void showElevatorInformation(){
        string state;
        if(currentState == 1){
            state = "Moving-up";
        }
        else if(currentState == -1){
            state = "Moving-down";
        }
        else{
            state = "Idle";
        }
        cout << "Elevator (" << state << ", " << totalWeightInTheElevator << ", " << numberOfPersonInTheElevator << ", " << currentFloor << " ->";

        for(int i=0; i<destinationQueue.size(); i++){
            if(i==0){
                cout << " " << destinationQueue[i];
            }
            else{
                cout << "," << destinationQueue[i];
            }
        }

        cout << ")" << endl;
    }

    void changeState(){
        if(!destinationQueue.empty()){
            int firstDestinationFloor = destinationQueue[0];
            if(firstDestinationFloor > currentFloor){
                currentState = 1;
            }
            else if(firstDestinationFloor < currentFloor){
                currentState = -1;
            }
        }
        else{
            currentState = 0;
        }
    }

    void travel(){
        __synchronized__;

        if(destinationQueue.empty()){
            return;
        }
        if(currentState == 1){
            currentFloor++;     
        }
        else if(currentState == -1){
            currentFloor--;
        }
        if(currentFloor == destinationQueue[0]){
            destinationQueue.erase(destinationQueue.begin());
            changeState();
            showElevatorInformation();

            if(currentState == 0){
                canMakeRequest.notifyAll();
            }
            if(!personQueue.empty() && (currentFloor == allPersons[personQueue[0]].destinationFloor)){
                canLeave.notifyAll();
            }
            else{
                canEnter.notifyAll(); 
            }

            usleep(in_out_time);
        }
        else{
            showElevatorInformation();
        }
    }

    void idle(){
        __synchronized__;

        while(destinationQueue.empty()){
            canMakeRequest.notifyAll();

            int rs = emptyDestinationQueue.wait(idle_time);

            if(rs == ETIMEDOUT){
                if(ended){
                    break;
                }
                continue;
            }
        }
    }

    void leaveTheElevator(int id){
        __synchronized__;

        int personDestinationFloor = allPersons[id].destinationFloor;

        while(1){
            canLeave.wait();

            if(currentFloor != allPersons[id].destinationFloor){
                continue;
            }
            else{
                int weight = allPersons[id].weight;

                totalWeightInTheElevator -= weight;
                numberOfPersonInTheElevator--;

                for(int i=0; i<personQueue.size(); i++){
                    if(personQueue[i] == id){
                        personQueue.erase(personQueue.begin()+i);
                        break;
                    }
                }

                showPersonInformation(id, "leave");
                showElevatorInformation();
                carriedCount++;
                if(carriedCount == npersons){
                    ended = true;
                    emptyDestinationQueue.notify();
                }
                else{
                    if(personQueue.empty() || allPersons[personQueue[0]].destinationFloor != currentFloor){
                        canEnter.notifyAll();
                    }
                }
                break;
            }
        }
    }

    void makeRequest(int id){
        __synchronized__;

        int personInitialFloor = allPersons[id].initialFloor;
        int personDestinationFloor = allPersons[id].destinationFloor;
        int askedDirection = (personDestinationFloor - personInitialFloor > 0) ? 1 : -1; 
        int weight = allPersons[id].weight;
        int priority = allPersons[id].priority;

        while(!allPersons[id].entered){
            canMakeRequest.wait();

            if((currentState != 0) && ((currentState != askedDirection) || (currentState == 1 && personInitialFloor < currentFloor) || (currentState == -1 && personInitialFloor > currentFloor))){
                continue;
            }
            else{
                insertIntoDestinationQueue(personInitialFloor);
                if(personInitialFloor == currentFloor && ((numberOfPersonInTheElevator != personCapacity) && (totalWeightInTheElevator + weight <= weightCapacity))){
                    if(personDestinationFloor > currentFloor){
                        currentState = 1;
                    }
                    else{
                        currentState = -1;
                    }
                }
                else{
                    changeState();
                }

                showPersonInformation(id, "request");
                showElevatorInformation();

                if(priority == 1){
                    floorPriorityCount[personInitialFloor]++;
                }

                if(personInitialFloor == currentFloor){
                    if(priority == 2 && floorPriorityCount[personInitialFloor] != 0){
                        priorityCondition.wait();
                    }
                    if(!personQueue.empty() && allPersons[personQueue[0]].destinationFloor == currentFloor){
                        canEnter.wait();
                    }
                    if((numberOfPersonInTheElevator == personCapacity) || (totalWeightInTheElevator + weight > weightCapacity)){
                        if(priority == 1){
                            floorPriorityCount[personInitialFloor]--;
                            if(floorPriorityCount[personInitialFloor] == 0){
                                priorityCondition.notifyAll();
                            }
                        }
                        continue;
                    }
                    else{
                        numberOfPersonInTheElevator++;
                        totalWeightInTheElevator += weight;

                        insertIntoDestinationQueue(personDestinationFloor);
                        insertIntoPersonQueue(id);
                        changeState();
                        allPersons[id].entered = true;

                        showPersonInformation(id, "enter");
                        showElevatorInformation(); 

                        if(priority == 1){
                            floorPriorityCount[personInitialFloor]--;
                            if(floorPriorityCount[personInitialFloor] == 0){
                                priorityCondition.notifyAll();
                            }
                        }

                        if(destinationQueue.size() == 1){
                            emptyDestinationQueue.notify();
                        }
                        break;
                    }
                }

                if(destinationQueue.size() == 1){
                    emptyDestinationQueue.notify();
                }
                if(askedDirection != currentState){
                    if(priority == 1){
                        floorPriorityCount[personInitialFloor]--;
                        if(floorPriorityCount[personInitialFloor] == 0){
                            priorityCondition.notifyAll();
                        }
                    }
                    continue;
                }
                while(1){
                    canEnter.wait();
                    if(priority == 2 && floorPriorityCount[personInitialFloor] != 0){
                        priorityCondition.wait();
                    }
                    if(!personQueue.empty() && allPersons[personQueue[0]].destinationFloor == currentFloor){
                        canEnter.wait();
                    }
                    if(currentState != askedDirection){
                        if(priority == 1){
                            floorPriorityCount[personInitialFloor]--;
                            if(floorPriorityCount[personInitialFloor] == 0){
                                priorityCondition.notifyAll();
                            }
                        }
                        break;
                    }
                    else if(personInitialFloor != currentFloor){
                        continue;
                    }
                    else if((numberOfPersonInTheElevator == personCapacity) || (totalWeightInTheElevator + weight > weightCapacity)){
                        if(priority == 1){
                            floorPriorityCount[personInitialFloor]--;
                            if(floorPriorityCount[personInitialFloor] == 0){
                                priorityCondition.notifyAll();
                            }
                        }
                        break;
                    }
                    else{
                        numberOfPersonInTheElevator++;
                        totalWeightInTheElevator += weight;

                        insertIntoDestinationQueue(personDestinationFloor);
                        insertIntoPersonQueue(id);
                        changeState();
                        allPersons[id].entered = true;

                        showPersonInformation(id, "enter");
                        showElevatorInformation(); 

                        if(priority == 1){
                            floorPriorityCount[personInitialFloor]--;
                            if(floorPriorityCount[personInitialFloor] == 0){
                                priorityCondition.notifyAll();
                            }
                        }

                        if(destinationQueue.size() == 1){
                            emptyDestinationQueue.notify();
                        }
                        break;
                    }
                }
            }
        }
    }
};

struct pParam{
    Elevator *el;
    int personId;
};

void *elevatorController(void *p){
    Elevator *el = (Elevator *) p;

    while(1){
        el->idle();

        usleep(travel_time);
        
        el->travel();

        if(ended){
            break;
        }
    }
}

void *person(void *p){
    pParam *pp = (pParam *) p;
    Elevator *el = pp->el;
    int personId = pp->personId;

    el->makeRequest(personId);

    el->leaveTheElevator(personId);
}

int main(int argc, char *argv[]){
    pthread_t *persons, elevatorControllerThread;

    string fileName = argv[1];
    string inp;
    string arg;
    fstream f;
    f.open(fileName);
    getline(f, inp);
    istringstream ss(inp);
    vector<int> elevatorArgs;

    while(ss >> arg){
        elevatorArgs.push_back(atoi(arg.c_str()));
    }

    int nfloors = elevatorArgs[0];
    int npersons = elevatorArgs[1];
    int weightCapacity = elevatorArgs[2];
    int personCapacity = elevatorArgs[3];
    travel_time = elevatorArgs[4];
    idle_time = elevatorArgs[5];
    in_out_time = elevatorArgs[6];

    vector<vector<int>> personArgs(npersons, vector<int> (4, 0));
    for(int i=0; i<npersons; i++){
        int j=0;
        getline(f, inp);
        istringstream ss(inp);
        while(ss >> arg){
            personArgs[i][j] = atoi(arg.c_str());
            j++;
        }
    }
    f.close();

    for(int i=0; i<npersons; i++){
        personProp newPerson;

        newPerson.weight = personArgs[i][0];
        newPerson.initialFloor = personArgs[i][1];
        newPerson.destinationFloor = personArgs[i][2];
        newPerson.priority = personArgs[i][3];
        newPerson.entered = false;

        allPersons.push_back(newPerson);
    }

    pParam *pparams = new pParam[npersons];
    Elevator el(npersons, nfloors, personCapacity, weightCapacity);

    persons = new pthread_t[npersons];

    for(int i=0; i<npersons; i++){
        pparams[i].personId = i;
        pparams[i].el = &el;

        pthread_create(&persons[i], NULL, person, (void *) (pparams + i));
    }

    pthread_create(&elevatorControllerThread, NULL, elevatorController, (void *) &el);

    for(int i=0; i<npersons; i++){
        pthread_join(persons[i], NULL);
    }

    pthread_join(elevatorControllerThread, NULL);

    delete[] pparams;
    delete[] persons;

    return 0;
}