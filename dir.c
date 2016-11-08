#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <dirent.h>
#include <string.h>
#include <cstring>
using namespace std;
#define MAX_FILES_NUMBER 5000

unsigned int get_files(string dirname, string* files)
{
	int i = 0;
	struct dirent *pDirent;
	DIR *pDir;

	if (pDir == NULL) {
		cout <<"Cannot open directory " << dirname << endl;
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
	unsigned int num_files;
	unsigned int process_count;
	string files[MAX_FILES_NUMBER];

	if (argc < 3) {
		cout << "Usage: ./a.out <dirname> <process_count>\n";
		return 1;
	}

	process_count = atoi(argv[2]);
	num_files = get_files(argv[1], files);

	for (int j = 0; j <= num_files; ++j) {
		cout << files[j] << endl;
	}

	return 0;
}