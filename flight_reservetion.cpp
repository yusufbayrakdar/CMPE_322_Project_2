/*
*   Operating System Project 2
*   Yusuf Sabri Bayrakdar
*   2016400378
*
*   Code compile status   = Compiling
*   Code work status      = Working
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
/*End Of The Libraries*/

/*Definitions*/
#define project Operating_System_Project_2
using namespace std;
pthread_t tid; /* the thread identifier */
pthread_attr_t attr; /* set of attributes for the thread */
void *client(void *param); /* the declaration function executed by the thread */
void *servant(void *param); /* the declaration function executed by the thread */
int coming_order=1;//It count threads and warn the main thread when all threads are done
mutex m;//Mutex lock
vector<int> seats;//Keeps seat number 
string clients[101];//Keeps client names in a list
int client_seat[101];//Keeps client reservation
int sleep_time[101];//Keeps sleep numbers of clients
int order[101];//This list keeps threads order,it stores order before threads sleeping
long totalSeat=0;//Total seat nuber
int reserv_seat;//Reservation variable
string myName;//Keeps client name to put in a list
bool alive_pthread=true;//Warn main when threads are done
int barrier=1;//Wait all threads to pick a random sleep number
ofstream out ("output.txt");//Create output.txt
/*End Of The Definitions*/

int main(int argc,char *argv[]){
    totalSeat = strtol(argv[1], NULL, 10);//Get total seat number
    if (argc != 2) { // If there is no convinient argument, exit
        fprintf(stderr,"usage: a.out <integer value>\n");
        return -1;
    }
    if(totalSeat<50||totalSeat>100){//If total seat number is not between 50 and 100, exit
        fprintf(stderr,"Enter a number between 50 and 100 please!\n");
        return -1;
    }
    cout<<"Number of total seats: "<<totalSeat<<endl;//Write total seat number to terminal
    out<<"Number of total seats: "<<totalSeat<<endl;//Write total seat number to output.txt
    //Create thread list
    for(int i = 0; i < totalSeat; i++)//Fill the seats 2D array as all seats are available;so 0 means available, otherwise it is full.
    {
        seats.push_back(0);
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
    while(barrier!=totalSeat+1);//Wait other threads to pick a random number to sleep
    for(int i = 1; i < totalSeat+1; i++)//Every thread knows its sleep number and it check all threads and cout all sleep number which less than the sleep number of the searching thread.
    {//Therefore each thread knows the order
        int x=0;
        m.lock();
        if(id==i){
            for(int i = 1; i < 101; i++)
            {
                // cout<<sleep_time[i]<<" "<<sleep_time[id]<<endl;
                if(sleep_time[i]!=0&&sleep_time[i]<=sleep_time[id]){//Count all threads which have less sleep number than the searching thread
                    x++;
                }
            }
        int y=0;
        for(int i = 0; i < 101; i++)//Check whether is there any thread in the same order as searching thread,Every thread checks it.
        {
            if(order[i]==x){//If so count them
                y++;
            }
        }
        x=x-y;//If there are some threads which has same order then differentiate their orders
        order[id]=x;//Put the order in order list according to threads id's to say that "its your order number, use it creating client name" to thread.
        x=0;//refresh variable
        y=0;//refresh variable
        }
        m.unlock();
    }
    usleep(sleep*1000);//Sleep a few milisec with a random sleep number,I multiply the sleep variable because usleep wants mikrosecond not milisecond.
    reserv_seat=rand()%(totalSeat-1);//Choose a seat to reserve after wake up
    m.lock();
    coming_order++;//This is for checking threads are working or they are done.
    reserv_seat++;//This is for seats begin with number 1.
    myName="Client"+to_string(order[id]);//Take the client name according to their order
    clients[id]=myName;//Put this name in a list with index of the thread
    while(true){//Run until it finds an available seat 
        if (seats[reserv_seat]==0) {//If seat is available then do reserve the seat
            seats[reserv_seat]=order[id];//If the seat available than its picked now, so we have to register the seat as full.
            client_seat[id]=reserv_seat+1;//Register clients seat
            break;//If we can find an available seat, then break the infinite loop.
        }
        reserv_seat=rand()%totalSeat;//If chosen seat is not available then pick another one
    }
    m.unlock();//For preemption I divide this section of code and unlock the mutex

        m.lock();//After giving a chance to preemption I lock mutex again
        pthread_attr_init(&attr);/* get the default attributes defined by pthread */
        pthread_create(&tid,&attr,servant,new int (id));/* create the thread with giving an ID, default attributes, where to start and argument for the method */
        pthread_join(tid,NULL);/* parent (client) thread waits for the child (servant) thread to exit */   
        m.unlock();
	    pthread_exit(0); // terminate the client thread
}
// The execution of the server thread will begin in this function
void *servant(void *ptr) //This is servant thread and it writes the output to output.txt and terminal.Besides, it warn main thread when all threads are done
{
    int id = *(int*)ptr;//Convert pointer to int to use it as index
    delete (int*)ptr;//Release memory of pointer
    cout<<clients[id]<<" reserves "<<client_seat[id]<<endl;//Write clients reservation to terminal
    out<<clients[id]<<" reserves "<<client_seat[id]<<endl;//Write clients reservation to output.txt
    if(coming_order==totalSeat+1)//Check whether threads are done or not
        alive_pthread=false;//If so, then warn main thread
	pthread_exit(0); // terminate the servant thread
}
/*End Of File*/