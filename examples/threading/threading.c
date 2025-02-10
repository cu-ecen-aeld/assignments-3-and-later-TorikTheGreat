#include "threading.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

// Optional: use these functions to add debug or error prints to your application
#define DEBUG_LOG(msg,...)
//#define DEBUG_LOG(msg,...) printf("threading: " msg "\n" , ##__VA_ARGS__)
#define ERROR_LOG(msg,...) printf("threading ERROR: " msg "\n" , ##__VA_ARGS__)

void* threadfunc(void* thread_param)
{

    // TODO: wait, obtain mutex, wait, release mutex as described by thread_data structure
    // hint: use a cast like the one below to obtain thread arguments from your parameter
    //struct thread_data* thread_func_args = (struct thread_data *) thread_param;

	int wait_ret;
	int mutex_ret;

	// Get the argument data. The cast changes the generic pointer 
	// to a specific one to the thread_data struct.
	struct thread_data* data = (struct thread_data *) thread_param;

	// First wait
	wait_ret = usleep(data->wait_to_obtain_ms);
	if (wait_ret == -1){
		ERROR_LOG("Failed first wait");
		data->thread_complete_success = false;
		return data;
	}

	// Obtain mutex
	mutex_ret = pthread_mutex_lock(data->mutex);
	if (mutex_ret != 0){
		ERROR_LOG("Failed to obtain mutex");
		data->thread_complete_success = false;
		return data;
	}

	// Second wait
	wait_ret = usleep(data->wait_to_release_ms);
	if (wait_ret == -1){
		ERROR_LOG("Failed second wait");
		data->thread_complete_success = false;
		return data;
	}

	// Release mutex
	mutex_ret = pthread_mutex_unlock(data->mutex);
	if (mutex_ret != 0){
		ERROR_LOG("Failed to release mutex");
		data->thread_complete_success = false;
		return data;
	}

	data->thread_complete_success = true;
	return data;
}


bool start_thread_obtaining_mutex(pthread_t *thread, pthread_mutex_t *mutex,int wait_to_obtain_ms, int wait_to_release_ms)
{
    /**
     * TODO: allocate memory for thread_data, setup mutex and wait arguments, pass thread_data to created thread
     * using threadfunc() as entry point.
     *
     * return true if successful.
     *
     * See implementation details in threading.h file comment block
     */

	int thread_ret;

	// Allocate data memory
	struct thread_data* data = malloc(sizeof(struct thread_data));
	if(!data){
		ERROR_LOG("Failed to allocate memory for thread_data structure");
		return false;
	}

	// Init arguments
	data->wait_to_obtain_ms = wait_to_obtain_ms;
	data->wait_to_release_ms = wait_to_release_ms;
	data->mutex = mutex;
	data->thread_complete_success = false;

	// Create thread
	thread_ret = pthread_create(thread, NULL, threadfunc, data);
	if(thread_ret != 0){
		ERROR_LOG("Failed to create thread");
		free(data);
		return false;
	}


	return true;
}

