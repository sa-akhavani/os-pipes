#include "main.h"

#define MAX_FILES_NUMBER 100000
#define MAX_BUFF_LEN 100000

int main(int argc, char *argv[])
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
	for (int i = 0; i < num_files; ++i)
		cout << files[i] << endl;

	int pfds[2];
	pid_t pid;

	pipe(pfds);

	if ((pid = fork()) == -1)
	{
		cout << "Fork Error\n";
		return -1;
	}
	if (pid == 0)	// child
	{
		char buf[MAX_BUFF_LEN];
		int nbytes;
		vector<string> corrupted_file_names;
		bool file_is_correct;
		nbytes = read(pfds[0], buf, 100000);
		cout << buf;
		file_is_correct = parse_file(buf);
		if (!file_is_correct)
			cout << "file is HAROOMI\n";
	}
	else			// parent
	{
		ifstream infile;
		infile.open((dirname + files[2]).c_str()); 
		cout << "Reading from the file: " << dirname + files[2] << endl; 
		string text = "";
		string line;
		while(getline(infile, line))
		{
			text+=line;
			text+='\n';
		}
		write(pfds[1], text.c_str(), text.size());
		infile.close();
		cout << "Files Ended ^__^" << endl;
		exit(0);		
	}

	cout << "the end" << endl;

	return 0;
}

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
	return i;
}

bool parse_file(string file)
{
	bool correct = true;
	vector<User> users;
	int id;
	unsigned int cash;
	cout << "parsing\n";
	for(int i = 0; i < file.length(); i++)
	{
		string line = "";

		while(file[i] != '\n')
		{
			line += file[i];
			i++;
		}

		if(line.find("->") != string::npos)
		{
			// transaction
			string sender = "";
			string reciever = "";
			string cash = "";
			int j = 0;
			while(line[j] == ' ')
			{
				j++;
				continue;
			}
			while(line[j] != ' ' && line[j] != '-' && line[j] != ':')
			{
				sender+=line[j];
				j++;
			}
			while(line[j] == ' ' || line[j] == '-' || line[j] == '>')
			{
				j++;
				continue;
			}
			while(line[j] != ' ' && line[j] != ':')
			{
				reciever+=line[j];
				j++;
			}
			while(line[j] == ' ' || line[j] == ':')
			{
				j++;
				continue;
			}
			while(line[j] != ' ' && line[j] != '\n' && j < line.length())
			{
				cash+=line[j];
				j++;
			}
			for (int z = 0; z < users.size(); ++z)
			{
				/* code */
				if(users[z].id == atoi(sender.c_str()) )
				{
					// cout << "found! " << users[z].id << endl;
					if(users[z].cash < atoi(cash.c_str()))
					{
						cout << users[z].id << "  is HAROOMI" << endl;
						correct = false;
					}
					else
					{
						cout << users[z].id << "  is GOOD" << endl;
					}

					users[z].cash -= atoi(cash.c_str());
					for(int k = 0; k < users.size(); k++)
					{
						if(users[k].id == atoi(reciever.c_str()) )
						{
							users[k].cash += atoi(cash.c_str());
							cout << users[k].id << " has: " << users[k].cash << endl;
							break;
						}
					}
					cout << users[z].id << " has: " << users[z].cash << endl;
					
					break;
				}
			}			
		}
		else
		{
			// initial value
			string user = "";
			string cash = "";
			int j = 0;
			while(line[j] == ' ')
			{
				j++;
				continue;
			}
			while(line[j] != ' ' && line[j] != ':')
			{
				user+=line[j];
				j++;
			}
			while(line[j] == ' ' || line[j] == ':')
			{
				j++;
				continue;
			}
			while(line[j] != ' ' && j < line.length())
			{
				cash+=line[j];
				j++;
			}
			User tmp(atoi(user.c_str()), atoi(cash.c_str()));
			users.push_back(tmp);
			cout << "user: " << tmp.id << " | cash: " << tmp.cash << endl;
		}
	}

	return correct;

}