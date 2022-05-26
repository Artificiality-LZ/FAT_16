#pragma once

#include<iostream>
#include<sstream>
#include<map>
#include<vector>
using namespace std;
#define DISK_MAXLEN 2560

//ָ���ö������
enum command_id { mkdir = 0, ls, delete_fat, open, read, write, close, cd, help };



struct File;
//Ŀ¼�ṹ��
struct Catalogue
{
	string name;
	vector<Catalogue*> chlid_catalogues;
	vector<File*> chlid_files;
	Catalogue* parent_catalogue = nullptr;
};

//�ļ��ṹ��
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


	//ɾ��Ŀ¼���õĵݹ����
	void del_recursion(Catalogue* catalogue, int i);

	//���ļ����ڲ�����
	File* open_file(Catalogue* cat, string name);

	//Ѱ�ҿյĴ��̿�
	int find_vacant_block(int n);

	//ɾ���ļ�ʱ�ͷŴ��̿ռ�
	void delete_fatdata(unsigned int first_num, unsigned int end_num);

public:
	//���캯����ʼ��
	FAT_16();

	//����Ŀ¼����
	bool mkdir(string name);

	//�г���ǰ�ļ�
	void ls();

	//ɾ���ļ�
	bool delete_fat(string name);

	//���ļ�
	bool open_pre(string path_name);

	//���ļ�
	void read_file();

	//д���ļ�
	bool write_file(string str);

	//�ر��ļ�
	void close();

	//�ı乤��·��
	void cd(string na);

	//��ȡ��ǰ·��
	string get_path();

	//����
	void help();	
};

//��ѭ��
void main_cycle();

//�ַ����ָ�
vector<string> string_sp(string command);