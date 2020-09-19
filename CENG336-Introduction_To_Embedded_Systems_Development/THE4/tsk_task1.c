#include "common.h"

/*********************************
 * GROUP70                       *
 * Ali Ilker SIGIRCI - 2171940   *
 * Onurcan UNAL      - 2095966   *
**********************************/

/************************************************************************************************************
 * ------------------------------ TASK1 ---------------------------------------------------------------------
 *
 * In Task1, we try to explore map with robot movements. When the robot moves,
 * it continuously update map[4][16] common variable according to sensor response.
 * This common map variable also exists in LCD task to update LCD concurrently with the robot movements.
 * 
 * In each iteration of the main while loop, we receive data just once. Then, according to value of this data,
 * we determined which information to transmit to the simulator.
 * Moreover, we try to move the robot in each iteration to minimize mininum and maximum ms.
 * 
 ************************************************************************************************************/

/**********************************************************************
 * ----------------------- GLOBAL VARIABLES ---------------------------
 **********************************************************************/

char data[40];
char data_size;

//Motion Commands
char forw[3] = "$F:";       //Move Forward
char rright[3] = "$R:";     //Turn Right
char rleft[3] = "$L:";      //Turn Left
char stop[3] = "$S:";       //Stop

char ending[5] = "$END:";   //END Command

int x, y, z, t;             //Variables to get sensor values

int up, down, left, right;  //Variables for coordinate plane

//Variables holding current place of robot
int currx = 0;              //Current place of robot in x-axis
int curry = 0;              //Current place of robot in y-axis

int currOrient = 180;       //Current orientaton of robot

int k, m;

int moving = 0;             //The amount of movement to understand whether the robot arrives in the center of the next cell or not (when it is 50)
int rotation = 0;           //The amount of rotation to understand whether the robot rotated 90 or not

int rotationFlag = 0;       //It is set to 1 when the robot rotates right, set to -1 when the robot rotates left; and 0 when there is no rotation
int moveFlag = 0;           //It is set to 1 when the robot moves

int unexplored = 62;        //The current amount of unexplored cell. When it becomes 0 and the robot is in the target cell, simulation ends
extern char map[4][16];     //It is a common variable which holds explored cells

/**********************************************************************
 * ----------------------- LOCAL FUNCTIONS ----------------------------
 **********************************************************************/

TASK(TASK1) 
{
    SetRelAlarm(ALARM_TSK1, 100, 50);
    PIE1bits.RC1IE = 1;       // enable EUSART1 receive interrupt
    while(1){
        WaitEvent(ALARM_EVENT);
        ClearEvent(ALARM_EVENT);
        data_size = receiveBuffer_pop(data);
        if(data_size != 0){
            if(data[0] == 'G' && data[1] == 'O'){ transmitBuffer_push(stop,3); continue;}   //If GO command is received from simulator send STOP
            
            else if(data[0] == 'D'){                                                        //Sensor response
                x = data[1]; y = data[2]; z = data[3]; t = data[4];
                
                //Update Map
                
                //Update coordinate plane variables according to current orientation of the robot
                if(currOrient == 0) {up=x; down=z; left=t; right=y;}
                else if(currOrient == 90) {up=y; down=t; left=x; right=z;}
                else if(currOrient == 180) {up=z; down=x; left=y; right=t;}
                else if(currOrient == 270) {up=t; down=y; left=z; right=x;}

                data[0] = '$'; data[1] = 'M'; data[5]=':';
                if(currx != 0){                                                             //The possibility of robot be in first row - edge case
                    if(map[currx-1][curry] == 'X'){                                         //Only update the map if it is unexplored
                        if(up == 0) map[currx-1][curry] = ' ';                              //Empty cell
                        else map[currx-1][curry] = 0xFF;                                    //Block cell
                        data[2]=currx-1; data[3]=curry; data[4]=up;
                        transmitBuffer_push(data,6);
                        unexplored--;
                    }    
                }
                if(curry != 15){                                                            //The possibility of robot be in last column - edge case
                    if(map[currx][curry+1] == 'X'){                                         //Only update the map if it is unexplored
                        if(right == 0) map[currx][curry+1] = ' ';                           //Empty cell
                        else map[currx][curry+1] = 0xFF;                                    //Block cell
                        data[2]=currx; data[3]=curry+1; data[4]=right;
                        transmitBuffer_push(data,6);
                        unexplored--;
                    }
                }
                if(currx != 3){                                                             //The possibility of robot be in last row - edge case
                    if(map[currx+1][curry] == 'X'){                                         //Only update the map if it is unexplored
                        if(down == 0) map[currx+1][curry] = ' ';                            //Empty cell
                        else map[currx+1][curry] = 0xFF;                                    //Block cell
                        data[2]=currx+1; data[3]=curry; data[4]=down;
                        transmitBuffer_push(data,6);
                        unexplored--;
                    }
                }
                if(curry != 0){                                                             //The possibility of robot be in first column - edge case
                    if(map[currx][curry-1] == 'X'){                                         //Only update the map if it is unexplored
                        if(left == 0) map[currx][curry-1] = ' ';                            //Empty cell
                        else map[currx][curry-1] = 0xFF;                                    //Block cell
                        data[2]=currx; data[3]=curry-1; data[4]=left;
                        transmitBuffer_push(data,6);
                        unexplored--;
                    }
                }
                //Check all explored end simulation when the conditions are met
                if(unexplored == 0 && (currx == 0 && curry == 15)){
                    transmitBuffer_push(ending,5);
                    break;
                }
            }
            
            else if(data[0] == 'E'){                                                        //Encoder response
                if(rotationFlag == 1){                                                      //Rotate right
                    rotation += data[1];
                    if(rotation == 90){                                                     //After 90 degree right rotation move forward
                        rotationFlag = 0;
                        moveFlag = 1;
                        currOrient = (currOrient + 270)%360;                                //Update current orientation accordingly
                        rotation = 0;
                        x = y;
                        y = 1;
                        transmitBuffer_push(forw,3);
                        continue;
                    }
                }
                else if(rotationFlag == -1){                                                //Rotate left
                    rotation += data[1];
                    if(rotation == 90){                                                     //After 90 degree left rotation send STOP command
                        rotationFlag = 0;
                        currOrient = (currOrient + 90)%360;                                 //Update current orientation accordingly
                        rotation = 0;
                        transmitBuffer_push(stop,3);
                        continue;
                    }
                }
                else if(moveFlag == 1){                                                      //Move
                    moving += data[1];
                    if(moving == 50){                                                        //When the robot is in the center of the next cell send STOP command
                        moveFlag = 0;                                                        //and update current location of the robot accordingly
                        if(currOrient == 0) currx--;            
                        else if(currOrient == 90) curry--;
                        else if(currOrient == 180) currx++;
                        else if(currOrient == 270) curry++;
                        moving = 0;
                        transmitBuffer_push(stop,3);
                        continue;
                    }
                }
            }
            if(y == 0){                                                                       //If right of the robot is empty ROTATE RIGHT and MOVE FORWARD
                rotationFlag = 1;
                transmitBuffer_push(rright,3);
            }
            else if(x == 0){                                                                  //If front of the robot is empty MOVE FORWARD
                moveFlag = 1;
                transmitBuffer_push(forw,3);
            }
            else{                                                                             //Else ROTATE LEFT
                rotationFlag = -1;
                transmitBuffer_push(rleft,3);
            }
        }
    }
	TerminateTask();
}

/* End of File : tsk_task1.c */
