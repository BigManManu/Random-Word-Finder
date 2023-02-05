#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<time.h>
#define BUFFER_SIZE 2000000000L
#define THREADS 3
#define LEN 8
int dictionary_size, nthreads = 1;
//char* buffer[THREADS];
char* dict[100000];
char **buffer;
int verbose = 0, timeOn = 0, len[100], arrLen = 2;
long totalBinarySearchTime;
pthread_mutex_t mutex;
static long get_nanos(void) {
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
	return (long)ts.tv_sec * 1000000000L + ts.tv_nsec;
}
int binsearch(char* dictionaryWords[],int listSize,char* keyword){
	int mid, low = 0, high = listSize - 1;
	while (high >= low) {
		mid = (high + low) / 2;
		if (strcmp(dictionaryWords[mid],keyword) < 0)
			low = mid + 1;
		else if (strcmp(dictionaryWords[mid],keyword)>0)
			high = mid - 1;
		else
			return mid;
	}
	return -1; //not found
}
void* word_puzzle_solver(void* id){
	char c = 0;
	int i, n, j;
	char* buf = buffer[(int)id];
	if (verbose) fprintf(stderr, "Note: Thread #%d: %s\n", (int) id, "started!");
	for(j = 0; j < arrLen; j++) {
		n = BUFFER_SIZE/nthreads - len[j];
		for(i = 0; i < n;i++){
			if(c)
				buf[i + len[j] - 1] = c;
			c = buf[i + len[j]];
			buf[i + len[j]] = '\0';
			long start = get_nanos();
			int result = binsearch(dict, dictionary_size, buf + i);
			pthread_mutex_lock(&mutex);
			totalBinarySearchTime += (get_nanos()-start);
			pthread_mutex_unlock(&mutex);
			if(result + 1)
				printf("Thread #%d: %s\n",(int)id, buf + i);
		}
	}
}
void argumentLen(char *var) {
	int j = 0, k =0;
	arrLen = 0;
	char *ptr;
	ptr = strtok(var, ",");
	while (ptr != NULL) {
		int num = atoi(ptr);
		len[j] = num;
		j++;
		ptr = strtok(NULL, ",");
	}
	while (1) {
		if (len[k] == 0)
			break;
		arrLen++;
		k++;
	}
}
void main(int argc, char** argv){
	long startTime = get_nanos();
	len[0] = 8, len[1] = 9;
	for (int i = 1; i < argc; i++) {
            if (strcmp(argv[i], "-len") == 0) {
            	len [0] = 0, len[1] = 0;
		argumentLen(argv[i + 1]);
	    }
            if (strcmp(argv[i], "-nthreads") == 0)
            	nthreads = atoi(argv[i + 1]);
            if (strcmp(argv[i], "-verbose") == 0)
            	verbose = 1;
            if (strcmp(argv[i], "-time") == 0)
            	timeOn = 1;
    	}
	buffer = (char**)malloc(nthreads*sizeof(char*));
	int thread_number, j;
	char temp[100];
	pthread_t threadID[nthreads];
	pthread_mutex_init(&mutex, NULL);
	char line[1000];
	char * pointer;
	int size = BUFFER_SIZE/nthreads;	
	FILE* f = fopen("dict.txt", "r");
	dictionary_size = 0;
	while(fgets(line, 1000, f)){
		sscanf(line, "%s\n", temp);
		if(strlen(temp) == 0)
			continue;
		dict[dictionary_size] = (char*) malloc(sizeof(temp)+1);
		strcpy(dict[dictionary_size++], temp);
	}
	fclose(f);
	for(thread_number = 0; thread_number < nthreads;thread_number++){
		buffer[thread_number] = (char*) malloc(size + 1);
		if(!fgets(buffer[thread_number], size + 1, stdin)){
			fprintf(stderr, "Error: can't read the input stream!");
			break;
		}
		if(pthread_create(threadID + thread_number, NULL, word_puzzle_solver, (void *) thread_number)){
			fprintf(stderr, "Error: Too many threads are created!\n");
			break;
		}
	}
	for(j = 0; j < thread_number;j++){
		pthread_join(threadID[j], NULL);
		if (verbose) fprintf(stderr, "Note: Thread %d joined!\n", j);
	}
	if (verbose) {
		fprintf(stderr, "Note: Total time: %ld nanoseconds using %d threads!\n", 
			get_nanos()-startTime, thread_number);
	}
	if (timeOn) {
		printf("Total time using binary search: %ld nanoseconds\n", totalBinarySearchTime);
	}
}