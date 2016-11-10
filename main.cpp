#include "main.h"

#define MAX_FILES_NUMBER 100000
#define MAX_BUFF_LEN 100000
#define FIFO_NAME "mozi"

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

	// new

	int pfds[process_count][2];
	pid_t pid[process_count];
	int file_per_process;

	if (num_files % process_count == 0)
		file_per_process = num_files / process_count;
	else
		file_per_process = num_files / process_count + 1;

	int current_file_number = 0;
	
	//	Creation of pipes being started
	for (int i = 0; i < process_count; ++i)
	{
		pipe(pfds[i]);
		if ((pid[i] = fork()) == -1)
		{
			cout << "Fork Error\n";
			return -1;
		}
		if (pid[i] == 0)
		{
			// child [i]
			// cout << "child [" << i << "]\n";
			close(pfds[i][1]);

			int files_count;
			char buf[MAX_BUFF_LEN];

			read(pfds[i][0], buf, MAX_BUFF_LEN);			// read file text
			close(pfds[i][0]);

			files_count = get_files_count(buf);

			string file_names[files_count];
			bool correct_or_not[files_count];


			int idx = get_start_idx(buf);					// skiping files count line
			for (int z = 0; z < files_count; ++z)
			{
				string new_file = "";
				int k;
				for (k = idx; buf[k] != '\0'; ++k)
				{
					if (buf[k] != '|')
					{
						new_file += buf[k];
						continue;
					}
					else
						break;
				}

				idx += k;
				file_names[z] = get_file_name(new_file);
				correct_or_not[z] = parse_file(new_file);
			}

			for (int z = 0; z < files_count; ++z)
			{
				if (correct_or_not[z])
					cout << "file: \"" << file_names[z] << "\" is Correct\n";
				else
					cout << "file: \"" << file_names[z] << "\" is HAROOMI\n";	
			}				

			exit(0);
		}
		else
		{
			// parent [i]
			close(pfds[i][0]);
			string text = "";
			string output = "";
			int j;
			for (j = 0; j < file_per_process && current_file_number < num_files; ++j)
			{	
				ifstream infile;
				infile.open((dirname + files[current_file_number]).c_str());
				
				text += files[current_file_number];
				text += '\n';
				string line;
				while(getline(infile, line))
				{
					text+=line;
					text+='\n';
				}

				infile.close();
				
				text += "|\n";
			
				current_file_number++;
			}

			output += to_string(j);
			output += '\n';
			output += text;
			// cout << "output: \n\"" << output  << '\"' << endl;
			write(pfds[i][1], output.c_str(), output.length() + 1);
			close(pfds[i][1]);
		}
	}

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

int get_files_count(string buf)
{
	string files_count = "";
	for (int i = 0; i < buf.length(); ++i)
	{
		if (buf[i] == ' ')
			continue;
		if(buf[i] == '\n')
			break;
		files_count += buf[i];
	}

	return atoi(files_count.c_str());
}

int get_start_idx(string buf)
{
	int i;
	for ( i = 0; i < buf.length(); ++i)
	{
		if (buf[i] == ' ')
			continue;
		if(buf[i] == '\n')
			break;
	}
	i++;
	return i;
}

string get_file_name(string buf)
{
	string file_name = "";
	for (int i = 0; i < buf.length(); ++i)
	{
		if (buf[i] == ' ')
			continue;
		if(buf[i] == '\n')
			break;
		file_name += buf[i];
	}

	return file_name;
}


bool parse_file(string file)
{
	// cout << "parsing\n";
	int counter = 0;
	bool correct = true;
	vector<User> users;
	int id;
	unsigned int cash;

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
						// cout << users[z].id << " is HAROOMI" << endl;
						correct = false;
					}
					else
					{
						// cout << users[z].id << " is GOOD" << endl;
					}

					users[z].cash -= atoi(cash.c_str());
					for(int k = 0; k < users.size(); k++)
					{
						if(users[k].id == atoi(reciever.c_str()) )
						{
							users[k].cash += atoi(cash.c_str());
							// cout << users[k].id << " has: " << users[k].cash << endl;
							break;
						}
					}
					// cout << users[z].id << " has: " << users[z].cash << endl;
					
					break;
				}
			}			
		}
		else if (line.find(':') != string::npos)
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
			// cout << "user: " << tmp.id << " | cash: " << tmp.cash << endl;
		}
		else
		{
			if (counter == 0)
			{
				counter++;
				continue;
			}
			else
			{
				return correct;
			}
			// cout << "this is file_name\n";
		}
	}

	return correct;
}