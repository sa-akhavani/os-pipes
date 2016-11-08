#include <iostream>
#include <cstdlib>
#include <dirent.h>
#include <cstring>
#include <fstream>
using namespace std;
#define MAX_FILES_NUMBER 5000

unsigned int get_files(string dirname, string* files)
{
	int i = 0;
	struct dirent *pDirent;
	DIR *pDir;

	if (pDir == NULL) {
		cout << "Cannot open directory " << dirname << endl;
		return -1;
	}

	pDir = opendir(dirname.c_str());
	while ((pDirent = readdir(pDir)) != NULL) {
		if (strcmp(pDirent->d_name, "..") == 0 || strcmp(pDirent->d_name, ".") == 0 )
			continue;
		files[i] = pDirent->d_name;
		i++;
	}
	closedir (pDir);
	return (i-1);
}

int main (int argc, char *argv[])
{
	char data[1000];
	unsigned int num_files;
	unsigned int process_count;
	string files[MAX_FILES_NUMBER];

	if (argc < 3)
	{
		cout << "Usage: ./a.out <dirname> <process_count>\n";
		return 1;
	}

	process_count = atoi(argv[2]);
	string dirname = argv[1];
	num_files = get_files(argv[1], files); // files of the DIR directory are in "files" array

	// printing file names
	for (int i = 0; i <= num_files; ++i)
		cout << files[i] << endl;

	// opening files
	for (int i = 0; i <= num_files; ++i)
	{
		ifstream infile;
		infile.open((dirname + files[i]).c_str()); 
		cout << "Reading from the file: " << dirname + files[i] << endl; 

		string line;
		while(getline(infile, line))
			cout << line << endl;
		// close the opened file.
		infile.close();	
		cout << "Files Ended ^__^" << endl;
	}
	return 0;
}