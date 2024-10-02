#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include<stdio.h>

int main(int argc, char *argv[]) 
{

    //Shared memory id
    int shared_mem_id;
    //Begin initializing the shared memory ID for both variables; size of the 2 integers
    shared_mem_id = shmget(IPC_PRIVATE, 2*sizeof(int), IPC_CREAT | 0666);
  
    //Pointer to shared memory
    int *shared_mem_ptr;
    //Insert the shared memory ID to the memory pointer
    shared_mem_ptr = (int*) shmat(shared_mem_id, NULL, 0);

  //Now check if the inesertion was successful
  if(shared_mem_ptr == (int *)  -1)
  {
      perror("The insertion of the shared memory ID was unsuccessful");
      exit(0);
  }

  //Pointer to memory id with their respective shared variables
  //for BankAccount and the Turn
    int *BankAccount = &shared_mem_ptr[0], *Turn = &shared_mem_ptr[1];

    //Initialize the shared variables
    *BankAccount = 0;
    *Turn = 0;

    //Set the seed random number generator for the time
    srand(time(NULL));



    //Check shared memory ID init success
    if(shared_mem_id < 0)
    {
        perror("The shared memory ID initialization process was unsuccessful!");
        exit(0);
    }



    //Beginning the process
    pid_t pid;
    pid = fork();

    //Check if the child process was successfully created
    if(pid < 0)
    {
        perror("The creation of the child process was unsuccessful");
        exit(0);
    }

    //The Child process
    if(pid == 0)
    {
        //For loop iterator
        int i = 0;

        for(i = 0; i < 25; i++)
        {
            //Cause child process to sleep for a random interal of 0 - 5 seconds
            sleep(rand() % 5 + 0);

            //Assign the local variable and copy the value from BankAccount
            int account = *BankAccount;
            //Wait unitl child process is ready
            while(*Turn != 1); 

            //Random amout within balance
            int balance = rand() % 50 + 0;
            printf("Poor Student needs $%d\n", balance);

            //Check to see if balance is less than or equal to the current account amount
            if(balance <= account)
            {
                //Withdraw the money
                account -= balance;
                printf("Poor Student: Withdraws $%d / Balance = $%d\n", balance, account);
            }
            else
            {
                printf("Poor Student: Not Enough Cash ($%d)\n", account);
            }

            //Now update the amount within the Bank Acount
            *BankAccount = account;
            //Now it's the parent's turn
            *Turn = 0;
        }

    }
    else 
    {
        int i = 0;
        //The Parent process
        for(i = 0; i < 25; i++)
        {
            //Parent process sleeps for a random number btween 0 - 5 seconds
            sleep(rand() % 5 + 0);

            //Copy the current amount within the Bank Account to the local account variable
            int account = *BankAccount;
            //Wait until it's the paren'ts turn 
            while(*Turn != 0);

            //If the bank account is less than or equal 100, deposit 
            if(account <= 100)
            {
                //Randomly generate a balance between 0 and 100
                int balance = rand() % 100 + 0;

                //Check to see if the balance amount is an even number
                if(balance % 2 == 0)
                {
                    //Deposit amount into the account
                    account += balance;
                    printf("Dear old Dad: Deposits: $%d / Balance = $%d\n", balance, account);
                }
                else
                {
                    printf("Dear old Dad: Doesn't have any money to give\n");
                }
            }
            else
            {
                printf("Dear old Dad: Thinks student has enough Cash ($%d)\n", account);
            }
            //Update the bank account
            *BankAccount = account;
            //Now it's the child' turn
            *Turn = 1;
        }
        //Allow the parent to wait for the child
        wait(NULL);

        //Free and remove the shared memory
        shmdt(shared_mem_ptr);
        shmctl(shared_mem_id, IPC_RMID, NULL);
    }

    return 0;
}