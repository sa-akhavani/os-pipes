#ifndef MAIN_H
#define MAIN_H
#include <iostream>
#include <cstdlib>
#include <dirent.h>
#include <cstring>
#include <fstream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <vector>
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
#endif