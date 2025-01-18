#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <errno.h>
#include <pthread.h>
#include <time.h>

#define MAX_DOCTORS 11
#define PATIENTS 18
#define TIME_LIMIT 8
sem_t doctors_ready;

struct doctor{
  int doctor_id;
  int current_patient;
  int is_free;
};

struct doctor doctors[MAX_DOCTORS];

int selection(int x)
{
  if (doctors[x].is_free == 0)
        {
          int number = rand() % MAX_DOCTORS;
	  return selection(number);
        }
  else
    doctors[x].is_free = 0;
    return x;
}


void consult(int select)
  {
    int consult_time = rand() % TIME_LIMIT + 2;
    sem_wait(&doctors_ready);
    usleep(consult_time);
    printf("Doctor nr. %d\n",doctors[select].doctor_id);
    printf("Pacientul nr. %d: Timp de consultatie: %d minute \n\n",doctors[select].current_patient,consult_time);
    doctors[select].is_free = 1;
    doctors[select].current_patient=0;
    sem_post(&doctors_ready);
  }

void * get_doctor(void * x)
{
   int dr_number =  rand() % MAX_DOCTORS;
   int pat_number = (int) x;
   int select = selection(dr_number);
   doctors[select].current_patient = pat_number;
   consult(select);
   return NULL;
}


int main()
{
 srand(time(NULL));
 pthread_t thr[PATIENTS];
 //Initializare
 for(int i=0; i< MAX_DOCTORS;i++)
{
   doctors[i].doctor_id = rand() % 1000;
   doctors[i].current_patient = 0;
   doctors[i].is_free = 1;
}
 if(sem_init(&doctors_ready, 0, MAX_DOCTORS))
  {
    perror("Cabinet inchis !!");
    return errno;
  }
//Consultatii
 for(int i=0; i<PATIENTS;i++)
  {
    int dr_number = rand() % MAX_DOCTORS;
    int wait_time = rand() % 4;
    if (pthread_create(&thr[i],NULL,&get_doctor,i))
	{
	 perror("Nu se gaseste pacientul");
	 return errno;
	}
    sleep(wait_time);
  }
 for(int i=0;i<PATIENTS;i++)
 {
    if(pthread_join(thr[i],NULL))
	{
	  perror("Ceva nu a mers..Oprire simulare!!");
	  return errno;
	}
  }
 sem_destroy(&doctors_ready);
 return 0;
}


