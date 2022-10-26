#### Support of Multithreading in C

The C programming language does not have built-in library support for multithreaded programming. Even though C is a general-purpose programming language that is widely used in embedded systems, system programming, and so forth, some vendors have developed libraries that deal with multithreading to achieve parallelism and concurrency.

The library to develop portable multithreaded applications is pthread.h; that is, the POSIX thread library. POSIX stands for *portable operating system interface*. POSIX threads are lightweight and designed to be very easy to implement. The pthread.h library is an external third-party library that helps you effectively do tasks.

**Note**  You can develop multithreaded programs with the pthread.h library in all Unix-based operating systems but not for Windows. if you want to develop an application that works on Windows, the windows.h library is very effective for multithreading-supported Windows operating systems.

 

The following are few of the functions in the pthread.h library that create, manipulate, and exit the threads.

•    pthread_create

•    pthread_join

•    pthread_self

•    pthread_equal

•    pthread_exit

•    pthread_cancel

 

##### pthread_create

pthread_create creates a new thread with a thread descriptor. A descriptor is an information container of the thread state, execution status, the process that it belongs to, related threads, stack reference information, and thread-specific resource information allocated by the process. This function takes four arguments as parameters. The return type of this function is an integer.

The following shows the syntax.

**int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void * (*start_routine)(void *), void *arg);**

The following describes the parameters.

•    pthread_t is a thread descriptor variable that takes the thread descriptor, has an argument, and returns the thread ID, which is an unsigned long integer.

•    pthread_attr_t is an argument that determines all the properties assigned to a thread. If it is a normal default thread, then you set the attribute value to NULL; otherwise, the argument is changed based on the programmer’s requirements.

•    start_routine is an argument that points to the subroutines that execute by thread. The return type for this parameter is an void type because it typecasts return types explicitly. This argument takes a single value as a parameter. If you want to pass multiple arguments, a heterogeneous datatype should be passed that might be a struct.

•    args is a parameter that depends on the previous parameter; it takes multiple parameters as an argument.

 

##### pthread_join

This function waits for the termination of another thread. It takes two parameters as arguments and returns the integer type. It returns 0 on successful termination and –1 if any failure occurs.

The following shows the syntax.

**int pthread_join(pthread_t, *thread, void **thread_return)**

The following describes the parameters.

•    thread takes the ID of the thread that is currently waiting for termination

•    thread_return is an argument that points to the exit status of the termination thread, which is a NULL value.

##### pthread_self

This function returns the thread ID of the currently running thread. The return type of this thread is an integer or the thread_t descriptor. It takes zero parameters as arguments.

The following shows the syntax.

pthread_t pthread_self()

or

int pthread_self()

##### pthread_equal

This function checks whether two threads are equal or not. This function compares two thread identifiers. If the two threads are equal, then the function returns a nonzero value. If the threads are not equal, then it is zero. It takes two parameters as arguments and returns the integer as output.

The following shows the syntax.

**int pthread_equal(pthread_t thread1, pthread_t thread2);**

The following describes the parameters.

thread1 and thread2 are the IDs for the first and second thread, respectively.

```c

// C program to demonstrate working of pthread_equal()
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>

pthread_t tmp_thread;

void* func_one(void* ptr)
{
	// in this field we can compare two thread
	// pthread_self gives a current thread id
	if (pthread_equal(tmp_thread, pthread_self())) {
		printf("equal\n");
	} else {
		printf("not equal\n");
	}
}

// driver code
int main()
{
	// thread one
	pthread_t thread_one;

	// assign the id of thread one in temporary
	// thread which is global declared r
	tmp_thread = thread_one;

	// create a thread
	pthread_create(&thread_one, NULL, func_one, NULL);

	// wait for thread
	pthread_join(thread_one, NULL);
}

```

##### pthread_exit

This function terminates a calling thread. It takes one argument as a parameter and returns nothing.

The following shows the syntax.

**void pthread_exit(void *retval);**

The following describes the parameters.

retval is the return value of a thread that you want to detach it.

##### pthread_cancel

This function is used for thread cancellation. It takes one parameter as an argument and returns an integer value.

The following shows the syntax.

**int pthread_cancel(pthread_t thread);** 

The following describes the parameter.

pthread is the thread ID of the thread that you want to cancel.
