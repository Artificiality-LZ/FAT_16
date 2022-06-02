#include"head.h"


//ָ�����ַ����ձ�
map<string, command_id> trans = {
	{"mkdir",mkdir},{"ls",ls} ,{"delete",delete_fat}, {"open",open}, {"read",read}, {"write",write}, {"close",close},{"cd",cd},{"help",help}
};


//���캯����ʼ��
FAT_16::FAT_16()
{
	root_Catalogue = new Catalogue;
	root_Catalogue->name = "root";
	root_Catalogue->parent_catalogue = nullptr;
	work_Catalogue = root_Catalogue;
	work_file = nullptr;
	for (int i = 0; i < DISK_MAXLEN; i++)
	{
		fat[i] = -2;
	}
}

//����Ŀ¼����
bool FAT_16::mkdir(string name)
{
	Catalogue* catalogue = new Catalogue;
	if (catalogue == nullptr)
	{
		return false;
	}
	for (int i = 0; i < work_Catalogue->chlid_catalogues.size(); i++)
	{
		if (work_Catalogue->chlid_catalogues[i]->name == name)
		{
			cout << "Ŀ¼�Ѵ��ڣ�\n";
			return false;
		}
	}
	for (int i = 0; i < work_Catalogue->chlid_files.size(); i++)
	{
		if (work_Catalogue->chlid_files[i]->name == name)
		{
			cout << "Ŀ¼���Ѵ��ڵ��ļ���������ָ���������ƣ�\n";
			return false;
		}
	}
	catalogue->name = name;
	catalogue->parent_catalogue = work_Catalogue;
	work_Catalogue->chlid_catalogues.push_back(catalogue);
	cout << "Ŀ¼" << name << "�����ɹ���\n";
	return true;
}

//�г���ǰ�ļ�
void FAT_16::ls()
{
	cout << ".\\\n";
	for (int i = 0; i < work_Catalogue->chlid_catalogues.size(); i++)
	{
		cout << "-" << work_Catalogue->chlid_catalogues[i]->name << endl;
	}
	for (int i = 0; i < work_Catalogue->chlid_files.size(); i++)
	{
		cout << work_Catalogue->chlid_files[i]->name << endl;
	}
}

//ɾ���ļ�
bool FAT_16::delete_fat(string name)
{
	for (int i = 0; i < work_Catalogue->chlid_catalogues.size(); i++)
	{
		
		if (work_Catalogue->chlid_catalogues[i]->name == name)
		{
			Catalogue* temp;
			del_recursion(work_Catalogue->chlid_catalogues[i], i);
			temp = work_Catalogue->chlid_catalogues[i];
			work_Catalogue->chlid_catalogues.erase(work_Catalogue->chlid_catalogues.begin() + i);
			delete temp;

			cout << "�ɹ�ɾ����Ϊ" << name << "��Ŀ¼�����ļ���\n";
			return true;
		}
	}
	for (int i = 0; i < work_Catalogue->chlid_files.size(); i++)
	{
		if (work_Catalogue->chlid_files[i]->name == name)
		{
			delete_fatdata(work_Catalogue->chlid_files[i]->first_num, work_Catalogue->chlid_files[i]->end_num);
			delete work_Catalogue->chlid_files[i];
			work_Catalogue->chlid_files.erase(work_Catalogue->chlid_files.begin() + i);
			cout << "�ɹ�ɾ����Ϊ" << name << "���ļ���\n";
			return true;

		}
	}
	cout << "�Ҳ�����Ϊ" << name << "���ļ�!\n";
	return false;
}

	

//�ı乤��·��
void FAT_16::cd(string na)
{
	//����ǲ��ǿգ��վͷ��ظ�Ŀ¼
	if (na == "")
	{
		work_Catalogue = root_Catalogue;
		return;
	}

	//�ָ�·��
	string p = na;
	vector<string> path;
	for (int i = 0; p.find("\\") != string::npos; i++)
	{
		int index = p.find("\\");
		path.push_back(p.substr(0, index));
		p = p.substr(index + 1, p.length() - index);
	}
	path.push_back(p);

	Catalogue* now_work = work_Catalogue;
	bool found = false;
	bool full_ = true;

	int i = 0;

	//�ж���Ŀ¼�ƶ�
	if (path[0].find(".") != string::npos)
	{
		full_ = true;
		for (int j = 0; j < path[0].length(); j++)
		{
			if (path[0][j] != '.')
			{

				full_ = false;
				break;
			}
		}
		if (full_)
		{
			i = 1;
			for (int j = 0; j < path[0].length() - 1; j++)
			{
				if (work_Catalogue->parent_catalogue != nullptr)
				{
					work_Catalogue = work_Catalogue->parent_catalogue;
				}
				else
				{
					work_Catalogue = now_work;
					cout << "������ת����Ŀ¼�ĸ�Ŀ¼��\n";
					return;
				}
			}
		}
		else
		{
			work_Catalogue = root_Catalogue;
		}
	}
	else if(!path.size())
	{
		work_Catalogue = root_Catalogue;
	}

	//����Ŀ¼�ƶ�
	for (i; i < path.size(); i++)
	{
		found = false;
		if (path[i] == "")
		{
			found = true;
			continue;
		}
		for (int j = 0; j < work_Catalogue->chlid_catalogues.size(); j++)
		{

			if (work_Catalogue->chlid_catalogues[j]->name == path[i])
			{
				work_Catalogue = work_Catalogue->chlid_catalogues[j];
				found = true;
				break;
			}
		}
		if (!found)
		{
			cout << "�޷���" << work_Catalogue->name << "���ҵ�" << path[i] << "Ŀ¼!" << "���顣\n";
			work_Catalogue = now_work;
			return;
		}
	}
}


//��ȡ��ǰ·��
string FAT_16::get_path()
{
	string re = ">:";
	if (work_file != nullptr)
	{
		re += "[" + work_file->name + "]:";
	}
	Catalogue* pa = work_Catalogue;
	while (pa->parent_catalogue != nullptr)
	{
		re = "\\" + pa->name + re;
		pa = pa->parent_catalogue;
	}
	re = "<root" + re;
	return re;
}

//����
void FAT_16::help()
{
	string mkdir = "mkdir <Ŀ¼��>\n����һ���ļ���\n\n";
	string ls = "ls\n�г���ǰĿ¼�µ�����\n\n";
	string delete_fat = "delete <�ļ���>\nɾ��һ���ļ�����Ŀ¼\n\n";
	string open_f = "open <·��\\�ļ���>\n��һ���ļ�\n\n";
	string read_f = "read\n��ȡ��ǰ���ļ�������\n\n";
	string write_f = "write <����>\n��ǰ�򿪵��ļ���д�룬Ĭ��Ϊ׷��\n\n";
	string close = "close\n�رյ�ǰ�ļ�\n\n";
	string help = "help\n����ҳ��\n\n";
	string cd = "cd <·��>\n��ת��Ŀ��·����֧�����·���;���·��\n\n";
	cout << endl << mkdir << ls << delete_fat << open_f << read_f << write_f << close << cd << help;
}

//���ļ�
bool FAT_16::open_pre(string path_name)
{
	string path;
	string name;
	if (path_name.find("\\") == string::npos)
	{
		name = path_name;
		path = "";
	}
	else
	{
		path = path_name.substr(0, path_name.find_last_of("\\"));
		name = path_name.substr(path_name.find_last_of("\\") + 1, path_name.length() - path_name.find_last_of("\\"));
	}
	Catalogue* temp = work_Catalogue;
	cd(path);
	work_file = open_file(work_Catalogue, name);
	work_Catalogue = temp;
	//cout << path << endl;
	//cout << name << endl;
	return true;
}

//���ļ����ڲ�����
File* FAT_16::open_file(Catalogue* cat, string name)
{
	for (int i = 0; i < cat->chlid_catalogues.size(); i++)
	{
		if (name == cat->chlid_catalogues[i]->name)
		{
			cout << "Ŀ¼����ͬ���ļ���\n";
			return nullptr;
		}
	}
	for (int i = 0; i < cat->chlid_files.size(); i++)
	{
		if (cat->chlid_files[i]->name == name)
		{
			return cat->chlid_files[i];
		}

	}
	short num = 0;
	num = find_vacant_block(0);
	File* t = new File;
	t->first_num = num;
	t->end_num = num;
	t->name = name;
	fat[num] = -1;
	cat->chlid_files.push_back(t);
	return t;
}

//д���ļ�
bool FAT_16::write_file(string str)
{
	int point;
	if (work_file == NULL)
	{
		cout << "Ŀǰ�޴򿪵��ļ�\n";
		return false;
	}
	point = work_file->first_num;
	while (fat[point] != -1)
	{
		point = fat[point];
	}

	for (int i = 0; i < str.length(); i++)
	{
		Disk[point] = str[i];
		fat[point] = -3;
		int temp = find_vacant_block(point);;
		fat[point] = temp;
		point = temp;
	}
	fat[point] = -1;
	work_file->end_num = point;
	return true;
}

//Ѱ�Ҵ��̿տ�
int FAT_16::find_vacant_block(int n)
{
	for (int i = n; i < DISK_MAXLEN; i++)
	{
		if (fat[i] == -2)
		{
			return i;
		}
	}
	for (int i = 0; i < n; i++)
	{
		if (fat[i] == -2)
		{
			return i;
		}
	}
	cout << "FAT����,�Ҳ����յ�block\n";
	exit(0);
}

//�ر��ļ�
void FAT_16::close()
{
	work_file = nullptr;
}

//ɾ���ļ��ĵ��ã������������
void FAT_16::delete_fatdata(unsigned int first_num, unsigned int end_num)
{
	while (first_num != end_num)
	{
		int temp = fat[first_num];
		fat[first_num] = -2;
		first_num = temp;
	}
	fat[first_num] = -2;

}

//��ȡ�ļ�
void FAT_16::read_file()
{

	if (work_file == NULL)
	{
		cout << "Ŀǰ�޴򿪵��ļ�\n";
		return;
	}
	int n = work_file->first_num;
	if (n == -1)
	{
		cout << "���ļ�Ϊ��\n";
		return;
	}
	while (n != -1)
	{
		cout << Disk[n];
		n = fat[n];
	}
	cout << endl;

}
	



//ɾ��Ŀ¼���õĵݹ����
void FAT_16::del_recursion(Catalogue* catalogue, int i)
{
	if (catalogue->chlid_catalogues.size() == 0)
	{
		for (int i = 0; i < catalogue->chlid_files.size(); i++)
		{
			delete_fatdata(catalogue->chlid_files[i]->first_num, catalogue->chlid_files[i]->end_num);
			delete catalogue->chlid_files[i];
			catalogue->chlid_files.erase(catalogue->chlid_files.begin() + i);
		}
		if (catalogue->parent_catalogue != nullptr)
		{
			catalogue->parent_catalogue->chlid_catalogues.erase(catalogue->parent_catalogue->chlid_catalogues.begin() + i);
			delete catalogue;
		}
	}
	else
	{
		for (int j = 0; j < catalogue->chlid_catalogues.size(); j++)
		{
			del_recursion(catalogue->chlid_catalogues[j], j);
		}
	}
}




//�ַ����ָ�
vector<string> string_sp(string command)
{
	vector<string> re;
	string comm;
	string na;
	if (command.find(" ") != string::npos)
	{
		int index = command.find(" ");
		comm = command.substr(0, index);
		na = command.substr(index + 1, command.length() - index);
	}
	else
	{
		comm = command;
	}
	re.push_back(comm);
	re.push_back(na);
	return re;
}

//��ѭ��
void main_cycle()
{
	FAT_16* fat_16 = new FAT_16;
	//fat->ls();

	while (true)
	{
		string command;
		cout << fat_16->get_path();
		getline(cin,command);
		vector<string>comm = string_sp(command);
		if (comm[0] == "")
			continue;



		if (trans.count(comm[0]) == 0)
		{
			cout << "��������Ϊ\"" << comm[0] << "\"�����\n";
		}
		else
		{
			switch (trans[comm[0]])
			{
			case mkdir:
				if (comm[1] == "")
				{
					cout << "�÷���\nmkdir <Ŀ¼��>\n";
				}
				else
				{
					fat_16->mkdir(comm[1]);
				}
				break;
			case ls:fat_16->ls(); break;
			case delete_fat:
				if (comm[1] == "")
				{
					cout << "�÷���\ndelete <�ļ���>\n";
				}
				else
				{
					fat_16->delete_fat(comm[1]);
				}; break;
			case open: 
				fat_16->open_pre(comm[1]);
				
				break;
			case read: fat_16->read_file(); break;

			case write:
				if (comm[1] == "")
				{
					cout << "�÷���\nwrite <����>\n";
				}
				else
				{
					fat_16->write_file(comm[1]); break;
				}
				

			case close:fat_16->close(); break;

			case cd: fat_16->cd(comm[1]);break;

			case help:fat_16->help(); break;
			}
		}
		
	}
}



