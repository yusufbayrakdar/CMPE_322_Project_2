/*
*   Yusuf Sabri Bayrakdar
*   2016400378
*   Operating System Project 2
*
*   To compile => 
*/

/*Libriries*/
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <cmath>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <mutex>
#include "seat.h"
/*End Of The Definitions*/

/*Definitions*/
#define project Operating_System_Project_2
using namespace std;
pthread_t tid; /* the thread identifier */
pthread_attr_t attr; /* set of attributes for the thread */
void *client(void *param); /* the declaration function executed by the thread */
void *servant(void *param); /* the declaration function executed by the thread */
int coming_order=1;
mutex m;
long totalSeat=0;
vector<seat> seat_list;
int reserv_seat;
bool reThink=false;
/*End Of The Definitions*/

int main(int argc,char *argv[]){
    totalSeat = strtol(argv[1], NULL, 10);//Get total seat number
    if (argc != 2) { // If there is not just one argument, exit
        fprintf(stderr,"usage: a.out <integer value>\n");
        return -1;
    }
    if(totalSeat<50||totalSeat>100){//If total seat number is not between 50 and 100, exit
        cout<<"Enter a number between 50 and 100 please!"<<endl;
        return -1;
    }
    cout<<"Welcome to Fight Reservation"<<endl;
    ofstream out ("output.txt");//Create output.txt
    out<<"Number of total seats: "<<totalSeat<<endl;//Write total seat number to output.txt
    //Create thread list
    for(int i = 1; i < totalSeat+1; i++)//Fill the seats 2D array as all seats are available;so 0 means available, otherwise it is full.
    {
        string client_name="Client "+to_string(i);
        seat new_seat;
        new_seat.available=false;
        new_seat.name="Client "+to_string(i);
        seat_list.push_back(new_seat);
    }
    for(int i = 0; i < totalSeat+1; i++)
    {
        if (i!=totalSeat-1) {
            pthread_attr_init(&attr);/* get the default attributes defined by pthread */
            pthread_create(&tid,&attr,client,argv[1]);/* create the thread with giving an ID, default attributes, where to start and argument for the method */

            
        }
    }
    
    while(coming_order!=totalSeat+1){
        pthread_join(tid,NULL);/* parent thread waits for the child thread to exit */      
    }
    
    
    
    
    
    // cout<<"argv "<<totalSeat<<endl;
    return 0;
}


// The execution of the thread will begin in this function
void *client(void *param) 
{
    int sleep=rand()%150+50;//Find a random number between 50-200 to sleep
    usleep(sleep*1000);//Sleep a few milisec
    
    
    m.lock();
    reserv_seat=rand()%totalSeat;//Choose a seat to reserve after wake up
    string myName="Client "+to_string(coming_order);
    coming_order++;
    cout<<myName<<" sleep "<<sleep<<" seat "<<reserv_seat<<endl;
        pthread_attr_init(&attr);/* get the default attributes defined by pthread */
        pthread_create(&tid,&attr,servant,param);/* create the thread with giving an ID, default attributes, where to start and argument for the method */
        pthread_join(tid,NULL);/* parent thread waits for the child thread to exit */ 

       
    
    m.unlock();
	pthread_exit(0); // terminate the thread
}

// The execution of the thread will begin in this function
void *servant(void *param) 
{
    cout<<" I am the servant thread of client "<<to_string(coming_order-1)<<" my client seat is "<<reserv_seat<<endl;
    if(seat_list[reserv_seat].available==false)
        seat_list[reserv_seat].available=true;
    
    else
    {
        cout<<" Reject"<<endl;
    }
    
	pthread_exit(0); // terminate the thread
}
