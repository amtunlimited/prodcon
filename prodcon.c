/*proj2.c: A Producer-consumer proof of concept with pthreads

Written by: Aaron Tagliaboschi <aaron.tagliaboschi082@topper.wku.edu>
*/

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>

#define BUFFER_SIZE 5
#define LENGTH 100
//Only works when there are not more consumers than producers
//not sure why
#define PRO_T 3
#define CON_T 3

int n=0;
int buffer[BUFFER_SIZE];
int in = 0, out = 0;
sem_t empty, full, mutex;

//Inserts a number into the buffer
void *producer(void* param) {
	int i;
	while(1) {
		sem_wait(&empty);
		sem_wait(&mutex);
		// critical section
			//This checks if the last number has been inserted
			if(n >= LENGTH) {
				in = (in + 1) % BUFFER_SIZE;
				//-1 is the signal that the buffer has ended
				buffer[in] = -1;
				sem_post(&mutex);
				sem_post(&full);
				pthread_exit(0);
			}
			n++;
			in = (in + 1) % BUFFER_SIZE;
			buffer[in] = n;
		sem_post(&mutex);
		sem_post(&full);
	}
}

//Removes a number from the buffer and displays it
void *consumer(void* param) {
	int i;
	while(1) {
		sem_wait(&full);
		sem_wait(&mutex);
		// critical section
			out = (out + 1) % BUFFER_SIZE;
			//Check if the producers have ended
			if(buffer[out] < 0) {
				out--;
				if (out<0)
					out+=5;
				sem_post(&mutex);
				sem_post(&empty);
				pthread_exit(0);
			}
			printf("%d\n", buffer[out]);
		sem_post(&mutex);
		sem_post(&empty);
	}
}

int main(int argc, char* argv[]) {
	int i;
	
	//Initalize the semaphores
	sem_init(&full, 0, 0);
	sem_init(&empty, 0, BUFFER_SIZE);
	sem_init(&mutex, 0, 1);
	
	//Make space for the threads
	pthread_t tid_pro[PRO_T], tid_con[CON_T];
	pthread_attr_t attr;
	
	//Setup the producer and consumer threads
	pthread_attr_init(&attr);
	for(i=0;i<PRO_T;i++)
		pthread_create(&tid_pro[i], &attr, producer, NULL);
	for(i=0;i<CON_T;i++)
		pthread_create(&tid_con[i], &attr, consumer, NULL);
	
	//Wait for the threads to end
	for(i=0;i<PRO_T;i++)
		pthread_join(tid_pro[i], NULL);
	for(i=0;i<CON_T;i++)
		pthread_join(tid_con[i], NULL);
	
	//Everything is done, so end!
	pthread_exit(0);
}