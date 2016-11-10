#include "main.h"

#define MAX_FILES_NUMBER 100000
#define MAX_BUFF_LEN 100000
#define FIFO_NAME "mozi"

int main(int argc, char *argv[])
{
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

	int pfds[process_count][2];
	pid_t pid[process_count];
	int file_per_process;

	if (num_files % process_count == 0)
		file_per_process = num_files / process_count;
	else
		file_per_process = num_files / process_count + 1;

	int current_file_number = 0;



	// FIFO
	int fifo_rfd, fifo_wfd;
	pid_t fifo_pid;
	mknod(FIFO_NAME, S_IFIFO | 0666, 0);
	if ((fifo_pid = fork()) == -1)
	{
		cout << "Fork Error\n";
		return -1;
	}
	if (fifo_pid == 0)
	{
		// child
		fifo_rfd = open(FIFO_NAME, O_RDONLY);

		ofstream outfile;
		outfile.open("output.txt");


		vector<string> cor_files;
		vector<string> healthy_files;
		int showed = 0;

		int corrupt_count = 0;
		string corrupt_files[MAX_FILES_NUMBER];
		int remain_files = num_files;
		int corupted_files_showd = 0;
		int nbytes;
		char buf[MAX_BUFF_LEN];
		int idx = 0;
		string tmp = "";
		while(1)
		{
			nbytes = read(fifo_rfd, buf, MAX_BUFF_LEN);
			if (nbytes == 0)
				continue;
			idx++;

			for (int r = 0; r < nbytes; r++)
			{
				if (buf[r] == '\0')
					continue;
				tmp += buf[r];
			}

			// cout << "tmp: " << tmp << endl;	
			string tmp_name = "";
			for (int r = 0; r < tmp.length(); ++r)
			{
				if (tmp[r] != ':')
					tmp_name += tmp[r];
				else if (tmp[r+1] == 't')
				{
					bool consists = false;
					for (int z = 0; z < healthy_files.size(); ++z)
					{
						if (healthy_files[z] == tmp_name)
						{
							consists = true;
							break;
						}
					}
					if (consists == false)		
					{
						healthy_files.push_back(tmp_name);
						// cout << "removed\n";
						remain_files--;
					}
					tmp_name = "";
					r+=2;
				}
				else
				{
					// cout << tmp_name << endl;
					bool consists = false;
					for (int z = 0; z < cor_files.size(); ++z)
					{
						if (cor_files[z] == tmp_name)
						{
							consists = true;
							break;
						}
					}
					if (consists == false)
					{
						// cout << "added!" << endl;
						cor_files.push_back(tmp_name);
						remain_files--;
					}
					tmp_name = "";
					r += 2;
				}
			}
		
			// cout << "rm: " << remain_files << endl;
			// cout << "cor: " << cor_files.size() << endl;
			// cout << "ncor " << healthy_files.size() << endl;
 
			if ((cor_files.size()+healthy_files.size()) - (showed*10) >= 10 || remain_files == 0)
			{
				int idx = corupted_files_showd;
				outfile << cor_files.size() - corupted_files_showd << endl;
				corupted_files_showd = cor_files.size();
				for (int i = idx; i < cor_files.size(); ++i)
					outfile << cor_files[i] << endl;
				showed++;
			}
		}
		outfile.close();
		exit(0);
	}
	else
	{
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
				// cout << buf << endl;
				// cout << idx << endl;
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

					idx = k+2;
					file_names[z] = get_file_name(new_file);
					// cout << "fz: " << new_file << endl;
					correct_or_not[z] = parse_file(new_file);
				}

				// open write FIFO
				fifo_wfd = open(FIFO_NAME, O_WRONLY);
				for (int z = 0; z < files_count; ++z)
				{
					// cout << "file: " << file_names[z] << endl;
					if (correct_or_not[z] == true)
					{
						write(fifo_wfd, (file_names[z] + ":t|" ).c_str(), (file_names[z] + ":t|" ).length() + 1);
						// cout << "---" << file_names[z] << endl;
					}
					else
					{
						write(fifo_wfd, (file_names[z] + ":f|" ).c_str(), (file_names[z] + ":f|" ).length() + 1);
						// cout << "---" << file_names[z] << endl;
					}
				}
				// close(fifo_wfd);
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


		// handle quit
		string inst;
		while(cin >> inst)
		{
			if (inst == "quit")
			{
				kill(0, SIGKILL);
				return 0;
			}
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

int count_files(string tmp)
{
	int cnt = 0;
	for (int i = 0; i < tmp.length(); ++i)
	{
		if (tmp[i] == '|')
			cnt++;	
	}	
	return cnt;
}