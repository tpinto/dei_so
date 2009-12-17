#include "lasttweet.h"#include <pthread.h>#define DEBUGtypedef struct {  char content[150];  long id;  char keyword[150];} tweet_t;pthread_mutex_t mutex;pthread_cond_t coiso_cheio;pthread_cond_t vai_fox;pthread_t threads[];tweet_t tweets[];void *Workout(void * k){  char * keyword = (char *) k;  while(1){    pthread_mutex_lock(&mutex);    while(){      pthread_cond_signal(&coiso_cheio);      pthread_cond_wait(&vai_fox, &mutex);    }        pthread_mutex_unlock(&mutex);  }  pthread_exit(NULL);}int main (int argc, char const *argv[]){  threads = malloc(argc*sizeof(pthread_t));  tweets = malloc(argc*sizeof(tweet_t));  int rc;  int i;  for(i=1;i<argc;i++){    #ifdef DEBUG      printf("Creating thread #%d for keyword '%s'\n",i,argv[i]);    #endif    rc = pthread_create(&threads[i], NULL, Workout, (void *) argv[i]);    //if (rc){    //  #ifdef DEBUG    //    printf("ERROR; pthread_create() returned %d\n", rc);    //  #endif    //  exit(-1);    //}  }  pthread_exit(NULL);  return 0;}