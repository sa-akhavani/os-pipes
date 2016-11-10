#ifndef MAIN_H
#define MAIN_H
#include <iostream>
#include <stdlib.h>
#include <dirent.h>
#include <cstring>
#include <fstream>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <vector>
#include <string>
#include <signal.h>
using namespace std;

class User
{
public:
	int id;
	int cash;
	User(int id, int cash);
	User();
};

User::User(int id, int cash)
{
	this->id = id;
	this->cash = cash;
}


unsigned int get_files(string dirname, string* files);
bool parse_file(string file);
string get_file_name(string buf);
int get_files_count(string buf);
int get_start_idx(string buf);
int count_files(string tmp);
#endif