// type 1: parent process
// input: DIR - process_count(unnamed pipe) - quit - 

// type 2: check file -> named pipe

// type 3:  sum corrupted files (named pipe)

#include <iostream>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fstream>


using namespace std;

int main(void)
{
	string dirname;
	unsigned int process_count;
	cout << "enter dirname: " << endl;
	cin >> dirname;
	cout << "enter process_count: " << endl;	
	cin	>> process_count;

   // open a file in read mode.
   ifstream infile; 
   infile.open("afile.dat"); 
 
   cout << "Reading from the file" << endl; 
   infile >> data; 

   // write the data at the screen.
   cout << data << endl;
   
   // again read the data from the file and display it.
   infile >> data; 
   cout << data << endl; 

   // close the opened file.
   infile.close();

   return 0;	

	// int pid;
	// for (int i = 0; i < process_count; ++i)
	// {
	// 	if((pid = fork()) == -1)
	// 	{
	// 		cout << "child nubmber: " << i << " failed to be created" << endl;
	// 		return -1;
	// 	}
	// 	if (pid == 0)
	// 	{
	// 		// child
	// 		cout << "child number: " << i << " created" << endl;
	// 	}
	// 	else
	// 	{
	// 		// parent
	// 		cout << "I am parent number: " << i << endl;
	// 	}
	// }







	// int nbytes;
	// int pfds[2];
	// pid_t child_pid;

	// char text[] = "sag\n";
	// char buffer[100];

	// pipe(pfds);

	// if ((child_pid = fork()) == -1)
	// {
 //        perror("fork");
 //        exit(1);		
	// }

	// if (child_pid == 0)
	// {
	// 	// Child process
	// 	close(pfds[0]);	// close pipe's read files descriptor
 //        write(pfds[1], text, (strlen(text)+1));
 //        // exit(0);
	// }
	// else
	// {
	// 	// Parent Process
	// 	close(pfds[1]);	// close pipe's write files descriptor
	// 	nbytes = read(pfds[0], buffer, sizeof(buffer));
	// 	printf("string is: %s", buffer);
	// 	exit(0);
	// }

	// return 0;
}
