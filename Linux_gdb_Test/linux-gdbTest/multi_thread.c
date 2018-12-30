#include<stdio.h>
#include<pthread.h>

void *pthread1_run(void *arg)
{
	int count = 10;
	while (count--)
	{
		sleep(1);
		printf("i am new pthread1 %lu\n", pthread_self());
	}
	pthread_exit(NULL);
	return 0;
}

void *pthread2_run(void *arg)
{
	int count = 10;
	while (count--)
	{
		sleep(1);
		printf("i am new pthread2 %lu\n", pthread_self());
	}
	pthread_exit(NULL);
	return 0;
}

int main()
{
	pthread_t tid1;
	pthread_t tid2;
	pthread_create(&tid1, NULL, pthread1_run, NULL);
	pthread_create(&tid2, NULL, pthread2_run, NULL);
	pthread_join(tid1, NULL);
	pthread_join(tid2, NULL);
	return 0;
}
