#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>
#include <sys/shm.h>
#include <errno.h>

int main()
{
	int lck,redr,ret;
	FILE *fp;
	char ch, s[20];
	struct sembuf semops[3];
	struct seminfo *__buf;
	semops[0].sem_num = 0;
	semops[0].sem_op = 0;
	semops[0].sem_flg = 0;

	semops[1].sem_num = 0;
	semops[1].sem_op = 1;
	semops[1].sem_flg = 0;

	semops[2].sem_num = 0;
	semops[2].sem_op = 1;
	semops[2].sem_op = 0;

	lck = semget(0x1234, 2, IPC_CREAT); //lock, wlock;

	if(lck>=0)
		printf("Creating Semaphore:Done\n");
	printf("Lock value:%i\n",semctl(lck, 0, GETVAL));
	printf("Waiting lock value:%i\n\n",semctl(lck, 1, GETVAL));


	printf("Reader/Writer (1/0):");
	scanf("%d",&redr);
	redr = redr>0?1:0;
	printf("\nTrying to Enter in CS...\n");

	if(redr){
		semops[0].sem_op = 1;semops[0].sem_num = 1;
		semop(lck,semops,1); //increments waiting lock
		printf("\nNew waiting lock:%i",semctl(lck,1,GETVAL));

		semops[0].sem_op = 0; semops[0].sem_num = 0; //checking for writing lock
		semop(lck, semops , 1);

		//entered		
		printf("\n\nReading shared file:\n-------\n");
		fp = fopen("shared.txt","r");
		while(!feof(fp)){
			ch = fgetc(fp);
			printf("%c",ch);
		}

		printf("\n\nPress any key to exit CS");
		scanf("%d",&ret);
		
		semops[0].sem_op = -1;	semops[0].sem_num = 1;
		semop(lck,semops,1);
		printf("\nNew waiting lock:%i",semctl(lck,1,GETVAL));
		fclose(fp);
	}
	else{ //writer
		semops[0].sem_op = 0; semops[1].sem_op = 0; semops[2].sem_op = 1;
		semops[0].sem_num = 1; semops[1].sem_num =0; semops[2].sem_num = 0;
		semop(lck, semops , 3); //waiting for both locks and incrementing waiting lock to one

		//entered
		printf("\n\nEntered, New lock:%i",semctl(lck,1,GETVAL));
		printf("\nWaiting lock:%i",semctl(lck,0,GETVAL));
		printf("\n\nEnter text to store\n---------\n");
		scanf("%s",&s);
		fp = fopen("shared.txt","a");
		fputs(s,fp);
		fclose(fp);

		printf("\nPress any key to exit CS:");
		scanf("%d",ret);
		semops[0].sem_op = -1; semops[0].sem_num = 0;
		semop(lck, semops, 1);
		printf("\n\nNew lock:%i",semctl(lck,0,GETVAL));
		printf("\nWaiting lock:%i",semctl(lck,1,GETVAL));
		//fclose(fp);
	}


	return 0;
}