#include"head.h"


//指令与字符对照表
map<string, command_id> trans = {
	{"mkdir",mkdir},{"ls",ls} ,{"delete",delete_fat}, {"open",open}, {"read",read}, {"write",write}, {"close",close},{"cd",cd},{"help",help}
};


//构造函数初始化
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

//创建目录函数
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
			cout << "目录已存在！\n";
			return false;
		}
	}
	for (int i = 0; i < work_Catalogue->chlid_files.size(); i++)
	{
		if (work_Catalogue->chlid_files[i]->name == name)
		{
			cout << "目录与已存在的文件重名，请指定其他名称！\n";
			return false;
		}
	}
	catalogue->name = name;
	catalogue->parent_catalogue = work_Catalogue;
	work_Catalogue->chlid_catalogues.push_back(catalogue);
	cout << "目录" << name << "创建成功！\n";
	return true;
}

//列出当前文件
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

//删除文件
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

			cout << "成功删除名为" << name << "的目录和子文件！\n";
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
			cout << "成功删除名为" << name << "的文件！\n";
			return true;

		}
	}
	cout << "找不到名为" << name << "的文件!\n";
	return false;
}

	

//改变工作路径
void FAT_16::cd(string na)
{
	//检查是不是空，空就返回根目录
	if (na == "")
	{
		work_Catalogue = root_Catalogue;
		return;
	}

	//分割路径
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

	//判断向父目录移动
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
					cout << "不能跳转到根目录的父目录！\n";
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

	//向子目录移动
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
			cout << "无法在" << work_Catalogue->name << "下找到" << path[i] << "目录!" << "请检查。\n";
			work_Catalogue = now_work;
			return;
		}
	}
}


//获取当前路径
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

//帮助
void FAT_16::help()
{
	string mkdir = "mkdir <目录名>\n创建一个文件夹\n\n";
	string ls = "ls\n列出当前目录下的内容\n\n";
	string delete_fat = "delete <文件名>\n删除一个文件或者目录\n\n";
	string open_f = "open <路径\\文件名>\n打开一个文件\n\n";
	string read_f = "read\n读取当前打开文件的内容\n\n";
	string write_f = "write <内容>\n向当前打开的文件中写入，默认为追加\n\n";
	string close = "close\n关闭当前文件\n\n";
	string help = "help\n帮助页面\n\n";
	string cd = "cd <路径>\n跳转到目标路径，支持相对路径和绝对路径\n\n";
	cout << endl << mkdir << ls << delete_fat << open_f << read_f << write_f << close << cd << help;
}

//打开文件
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

//打开文件的内部调用
File* FAT_16::open_file(Catalogue* cat, string name)
{
	for (int i = 0; i < cat->chlid_catalogues.size(); i++)
	{
		if (name == cat->chlid_catalogues[i]->name)
		{
			cout << "目录下有同名文件夹\n";
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

//写入文件
bool FAT_16::write_file(string str)
{
	int point;
	if (work_file == NULL)
	{
		cout << "目前无打开的文件\n";
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

//寻找磁盘空块
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
	cout << "FAT满了,找不到空的block\n";
	exit(0);
}

//关闭文件
void FAT_16::close()
{
	work_file = nullptr;
}

//删除文件的调用，用来清理磁盘
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

//读取文件
void FAT_16::read_file()
{

	if (work_file == NULL)
	{
		cout << "目前无打开的文件\n";
		return;
	}
	int n = work_file->first_num;
	if (n == -1)
	{
		cout << "该文件为空\n";
		return;
	}
	while (n != -1)
	{
		cout << Disk[n];
		n = fat[n];
	}
	cout << endl;

}
	



//删除目录项用的递归调用
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




//字符串分割
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

//主循环
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
			cout << "不存在名为\"" << comm[0] << "\"的命令！\n";
		}
		else
		{
			switch (trans[comm[0]])
			{
			case mkdir:
				if (comm[1] == "")
				{
					cout << "用法：\nmkdir <目录名>\n";
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
					cout << "用法：\ndelete <文件名>\n";
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
					cout << "用法：\nwrite <内容>\n";
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



