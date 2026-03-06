// argc = 1
// argv[0]
// ./Myexe

// argc = 2
// argv[0]  argv[1]
// ./Myexe  /home/Demo

// argc = 3
// argv[0]  argv[1]      argv[1]
// ./Myexe  /home/Demo    5

/////////////////////////////////////////////////////////////////////////////////////////////
//
//          Header File Inclusion
//
/////////////////////////////////////////////////////////////////////////////////////////////

#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>
#include<pthread.h>
#include<signal.h>
#include<time.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>
#include<sys/statvfs.h>  // For harddisk

#define _GNU_SOURCE
/////////////////////////////////////////////////////////////////////////////////////////////
//
//          Global Vairable creation
//
/////////////////////////////////////////////////////////////////////////////////////////////


// sig_atomic_t --> no one can access it
// volatile --> complier will not optimize it
// stop_flag--> user defined name

static volatile sig_atomic_t stop_flag = 0; // variable

// ctrl + c
void sigint_handler(int sig)
{
    (void)sig; // when sig varibale not used complier will generate error to avoid these (void)sig; will be in use

    printf("Marvellous System Logger is terminating...\n");

    // Tell the threads to stop execution

    stop_flag = 1;
    
}

// Structue which holds all system information
typedef struct      // Anonymous structure
{
    double cpu;     // CPU usage percentage
    double mem;     // RAM usage percentage
    double disk;    // Harddisk  usage percentage

}Sanpshot;          // Sanpshot used to create object using these name


// Global object which holds informantion
static Sanpshot snap; // Sanpshot is object to structure

// Mutex lock for critical section
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

// Default path for disk
static char * disk_path = "/"; // (/) is root of filesubsystem

// Sleep timer for log
static int interval_sec = 2;  // put entry in log for every 2 second

/////////////////////////////////////////////////////////////////////////////////////////////
//
//          Helper function defination
//
/////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////
//
// Function to collect cpu information
//
/////////////////////////////////////////////////////////////////////////////////////////////

static double cpu_percent()
{

    // Logic to fetch cpu information

    return 0.0;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
// Function to collect memory information
//
/////////////////////////////////////////////////////////////////////////////////////////////

static double mem_percent()
{

    // Logic to fetch memory information

    return 0.0;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
// Function to collect disk information
//
/////////////////////////////////////////////////////////////////////////////////////////////

static double disk_percent(char *path)
{

    // Logic to fetch disk information

    return 0.0;
}


// Thread proc for thread which collects system information
static void * collector_thread(void *arg)
{
    (void)arg;

    double c = 0.0, m = 0.0, d = 0.0;

    printf("Inside Collector thread\n");

     // when we press ctrl+c 0 becmoes 1 and when its not 0 it will stop
     // Enter if ctrl+c is not arrived
    while(!stop_flag)  
    {
        // Calculate the current resource usage
         c = cpu_percent();
         m = mem_percent();
         d = disk_percent(disk_path);

        // start the critical section 
         snap.cpu = c;
         snap.mem = m;
         snap.disk = d;

         pthread_mutex_lock(&mtx);

         snap.cpu = c;
         snap.mem = m;
         snap.disk = d;

        pthread_mutex_unlock(&mtx);
    }

    return NULL;
} 

// Thread proc for thread which writes log
static void * logger_thread(void *arg)
{
    (void)arg;

    int fd = 0;
    fd = open("Marvellous_log.txt",O_CREAT | O_WRONLY | O_APPEND ,0666);

    char welcome[] = "Marvellous System Logger";
    
    write(fd,welcome,strlen(welcome));
    printf("Inside Logger thread\n");

    while(!stop_flag)
    {
        double m = 0.0, d = 0.0, c = 0.0;
        int i = 0;
        pthread_mutex_lock(&mtx);
        
        d = snap.disk;
        c = snap.cpu;
        m = snap.mem;
        
        pthread_mutex_unlock(&mtx);
        // Write the information of structure snap into the file
        // preapre string using sprintf
        char line[256];
        // write that string int log file
        // write(fd,line,strlen(line));

        // sleep for intervel

        for(i = 0; i < interval_sec && !stop_flag ; i++)
        {
            sleep(1);
        }
    }

    close(fd);
    return NULL;
} 


/////////////////////////////////////////////////////////////////////////////////////////////
//
//          Entry point function of project
//
/////////////////////////////////////////////////////////////////////////////////////////////


int main(int argc, char *argv[])
{
    // ./Myexe  /home/Demo 
    if(argc == 2)
    {
        disk_path = argv[1];
    }
    // ./Myexe  /home/Demo    5
    else if(argc == 3)
    {
        disk_path = argv[1];
        interval_sec = atoi(argv[2]);       // change the interval time
    }


    printf("Marvellous System Logger\n");

    printf("Path is : %s\n",disk_path);
    printf("Interval is : %d\n",interval_sec);

    // Structure for handling ctrl+c
    struct sigaction sa;
    memset(&sa,0,sizeof(sa));

    sa.sa_handler = sigint_handler;

    sigaction(SIGINT,&sa,NULL);

    // Thread to collect information
    pthread_t t_collect;

    // Thread to write the data into log
    pthread_t t_log;

    // create Thread to collect information
    pthread_create(&t_collect,NULL,collector_thread,NULL);

    // create Thread to write the data into log
    pthread_create(&t_log,NULL,logger_thread,NULL);

    // Main thread waiting for child threads to terminate
    pthread_join(t_collect,NULL);
    pthread_join(t_log,NULL);
    
    printf("Terminating the Marvellous System Logger\n");
    
    return 0;
}