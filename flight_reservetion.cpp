/*
*   Yusuf Sabri Bayrakdar
*   2016400378
*   Operating System Project 2
*
*   To compile  => g++ -o flight_reservetion flight_reservetion.cpp
*   To run      => ./flight_reservetion n
*/

/*Libraries*/
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
#include <list>
#include "client.h"
/*End Of The Libraries*/

/*Definitions*/
#define project Operating_System_Project_2
using namespace std;
pthread_t tid; /* the thread identifier */
pthread_attr_t attr; /* set of attributes for the thread */
void *client(void *param); /* the declaration function executed by the thread */
void *servant(void *param); /* the declaration function executed by the thread */
int coming_order=1;
mutex m;
vector<int> seats; 
string clients[101]; 
int client_seat[101];
int sleep_time[101];
int order[101];
long totalSeat=0;
int reserv_seat;
string myName;
bool alive_pthread=true;
int barrier=1;
ofstream out ("output.txt");//Create output.txt
/*End Of The Definitions*/

int main(int argc,char *argv[]){
    totalSeat = strtol(argv[1], NULL, 10);//Get total seat number
    if (argc != 2) { // If there is no convinient argument, exit
        fprintf(stderr,"usage: a.out <integer value>\n");
        return -1;
    }
    if(totalSeat<50||totalSeat>100){//If total seat number is not between 50 and 100, exit
        cout<<"Enter a number between 50 and 100 please!"<<endl;
        return -1;
    }
    cout<<"Number of total seats: "<<totalSeat<<endl;//Write total seat number to terminal
    out<<"Number of total seats: "<<totalSeat<<endl;//Write total seat number to output.txt
    //Create thread list
    for(int i = 0; i < totalSeat; i++)//Fill the seats 2D array as all seats are available;so 0 means available, otherwise it is full.
    {
        // string client_name="Client "+to_string(i);
        seats.push_back(0);
        // cout<<"Seat "<<i+1<<" "<<seats[i]<<endl;
    }
    for(int i = 0; i < totalSeat+1; i++)
    {
        if (i!=totalSeat) {
            pthread_attr_init(&attr);/* get the default attributes defined by pthread */
            pthread_create(&tid,&attr,client,new int(i+1));/* create the thread with giving an ID, default attributes, where to start and argument for the method */
        }
    }
    while(alive_pthread){
        pthread_join(tid,NULL);/* parent thread waits for the child thread to exit */      
    }
    cout<<"All seats are reserved"<<endl;//Write to terminal
    out<<"All seats are reserved";//Write to output.txt
    
    return 0;
}


// The execution of the client thread will begin in this function
void *client(void* ptr) 
{
    int id = *(int*)ptr;//Convert pointer to int to use it as index
    delete (int*)ptr;//Release memory of pointer
    int sleep=rand()%150+50;//Find a random number between 50-200 to sleep
    sleep_time[id]=sleep;
    barrier++;
    while(barrier!=totalSeat+1){//To find out every sleep time wait for everyone
        if(barrier==totalSeat+1)
        break;
    }
    
    for(int i = 1; i < totalSeat+1; i++)
    {
        int x=0;
        m.lock();
        if(id==i){
            for(int i = 1; i < 101; i++)
            {
                // cout<<sleep_time[i]<<" "<<sleep_time[id]<<endl;
                if(sleep_time[i]!=0&&sleep_time[i]<=sleep_time[id]){
                    x++;
                }
            }
        int y=0;
        for(int i = 0; i < 101; i++)
        {
            if(order[i]==x){
                y++;
            }
        }
        x=x-y;
        
        order[id]=x;
        x=0;
        }
        m.unlock();
    }
    usleep(sleep*1000);//Sleep a few milisec
    // if(id==3){
    //     usleep(80*1000);
    // }
    reserv_seat=rand()%(totalSeat-1);//Choose a seat to reserve after wake up
    
    
    m.lock();
    coming_order++;
    reserv_seat++;
    
    myName="Client "+to_string(order[id]);
    clients[id]=myName;
    
    while(true){
        if (seats[reserv_seat]==0) {
            seats[reserv_seat]=order[id];
            client_seat[id]=reserv_seat+1;
            break;
        }
        reserv_seat=rand()%totalSeat;//Choose a seat to reserve after wake up
    }
    m.unlock();//For preemption I divide this section of code and unlock the mutex

        m.lock();//After giving a chance to preemption I lock mutex again
        cout<<clients[id]<<" "<<sleep<<" "<<client_seat[id]<<" "<<id<<endl;
        // cout<<order[id]<<endl;
        pthread_attr_init(&attr);/* get the default attributes defined by pthread */
        pthread_create(&tid,&attr,servant,new int (id));/* create the thread with giving an ID, default attributes, where to start and argument for the method */
        pthread_join(tid,NULL);/* parent (client) thread waits for the child (servant) thread to exit */   
        m.unlock();
	    pthread_exit(0); // terminate the thread
}
// The execution of the server thread will begin in this function
void *servant(void *ptr) 
{
    int id = *(int*)ptr;//Convert pointer to int to use it as index
    delete (int*)ptr;//Release memory of pointer
    // cout<<clients[id]<<" reserves "<<client_seat[id]<<endl;//Write clients reservation to terminal
    out<<clients[id]<<" reserves "<<client_seat[id]<<endl;//Write clients reservation to output.txt
    if(coming_order==totalSeat+1)
        alive_pthread=false;
	pthread_exit(0); // terminate the servant thread
}
