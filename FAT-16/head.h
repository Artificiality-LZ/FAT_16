#pragma once

#include<iostream>
#include<sstream>
#include<map>
#include<vector>
using namespace std;
#define DISK_MAXLEN 2560

//指令的枚举类型
enum command_id { mkdir = 0, ls, delete_fat, open, read, write, close, cd, help };



struct File;
//目录结构体
struct Catalogue
{
	string name;
	vector<Catalogue*> chlid_catalogues;
	vector<File*> chlid_files;
	Catalogue* parent_catalogue = nullptr;
};

//文件结构体
struct File
{
	string name;
	unsigned int first_num = 0;
	unsigned int end_num = 0;
};



class FAT_16
{
	Catalogue* root_Catalogue;
	Catalogue* work_Catalogue;
	File* work_file;

	char Disk[DISK_MAXLEN];
	int fat[DISK_MAXLEN];


	//删除目录项用的递归调用
	void del_recursion(Catalogue* catalogue, int i);

	//打开文件的内部调用
	File* open_file(Catalogue* cat, string name);

	//寻找空的磁盘块
	int find_vacant_block(int n);

	//删除文件时释放磁盘空间
	void delete_fatdata(unsigned int first_num, unsigned int end_num);

public:
	//构造函数初始化
	FAT_16();

	//创建目录函数
	bool mkdir(string name);

	//列出当前文件
	void ls();

	//删除文件
	bool delete_fat(string name);

	//打开文件
	bool open_pre(string path_name);

	//读文件
	void read_file();

	//写入文件
	bool write_file(string str);

	//关闭文件
	void close();

	//改变工作路径
	void cd(string na);

	//获取当前路径
	string get_path();

	//帮助
	void help();	
};

//主循环
void main_cycle();

//字符串分割
vector<string> string_sp(string command);