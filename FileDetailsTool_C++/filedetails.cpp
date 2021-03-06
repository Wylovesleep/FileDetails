// FileDetailsTool_C++.cpp: 定义控制台应用程序的入口点。
//
#include "stdafx.h"
#include "filedetails.h"
#include <io.h>


static int str_to_hex(string string, unsigned char *arrMd5, int len)
{
	BYTE high, low;
	int idx, ii = 0;
	for (idx = 0; idx<len; idx += 2)
	{
		high = string[idx];
		low = string[idx + 1];

		if (high >= '0' && high <= '9')
			high = high - '0';
		else if (high >= 'A' && high <= 'F')
			high = high - 'A' + 10;
		else if (high >= 'a' && high <= 'f')
			high = high - 'a' + 10;
		else
			return -1;

		if (low >= '0' && low <= '9')
			low = low - '0';
		else if (low >= 'A' && low <= 'F')
			low = low - 'A' + 10;
		else if (low >= 'a' && low <= 'f')
			low = low - 'a' + 10;
		else
			return -1;

		arrMd5[ii++] = (high << 4 | low);
	}
	return 0;
}

string Md5ToBase64(string SrcMd5)
{
	Base64 *base = new Base64();
	int len = SrcMd5.length();
	unsigned char *arrMd5 = new unsigned char[len / 2 + 1];
	if (str_to_hex(SrcMd5, arrMd5, len) == -1)
	{
		cout << "string to hex error " << endl;
		return "string to hex error";
	}
//	const unsigned char *arrMd5 = reinterpret_cast<const unsigned char*>(SrcMd5.c_str());
	string encode = base->Encode(arrMd5, len/2);
	delete[] arrMd5;
	return encode;
}

string GetSourcePath(void)
{
	string SourcePath;
	cout << "请输入游戏路径,例如：E:\\HTC_GAME\\95a472f1-0a8b-495b-aa2f-6df838c16788" << endl;
	getline(cin,SourcePath);
//	cout << "输入的路径为："<<SourcePath << endl;                   //打印信息

	return SourcePath;
}

void DealFilePath(string &srcpath)
{
	int pos = 0;
//	int i = 0;

	while (1)
	{
		pos = srcpath.find("\\", pos);
		if (pos == -1)
		{
			break;
		}
//		cout << i++;
		srcpath.insert(pos, "\\");
		pos += 2;
	}
}

/*
传入文件path，获取文件id
srcpath位用户输入的地址
path是获取的文件path
mode：GameFiles为1，ImageFiles为2，configFile为0，其他为-1.
*/
string DealFileId(string srcpath,string path, int mode)
{
	int pos = 0;
	string flaggamefiles = "game/";
	string flagimagefiles = "image/";
	
	while (1)
	{
		pos = path.find("\\\\", pos);
		if (pos == -1)
		{
			break;
		}
		path.replace(pos, 2, "/");
		pos++;
	}
	
	if (mode == 1)
	{
		path = path.erase(0, path.find(flaggamefiles) + flaggamefiles.size());		
	}
	else if (mode == 2)
	{
		path = path.erase(0, path.find(flagimagefiles) + flagimagefiles.size());
	}
	else if (mode == 0)//configfile在根目录下，没有上级目录所以id和path相同，不需要处理。
	{

	}

	

	return path;
}

/*
获取传入的path路径下所有文件的路径（包含文件名）及size，分别存入传入的容器files和vctsize中，会忽略文件名为FileDetails的文件。
*/
void GetFileName(string path, vector<FileDetails> &filedetails)
{

	//文件句柄  
	intptr_t   hFile = 0;
	//文件信息  
	struct _finddata_t fileinfo;
	string p;
	static string tpath = path;
	FileDetails fdvar;

	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			//如果是目录,迭代之  
			//如果不是,加入列表  
			if (fileinfo.attrib & _A_SUBDIR)
			{
				//文件名不等于"."&&文件名不等于"..
				//.表示当前目录
				//..表示当前目录的父目录
				//判断时，两者都要忽略，不然就无限递归跳不出去了！
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)   
				{
//					cout << "文件夹" << endl;
					GetFileName(p.assign(path).append("\\").append(fileinfo.name), filedetails);
				}
			}
			else
			{
				string temp("FileDetails");
				p.assign(path).append("\\").append(fileinfo.name);
				cout << "文件名： " << fileinfo.name << endl << "size： " << fileinfo.size << endl;

				if (p.find(temp) == -1 && p.find("vive.htc") == -1)     //忽略FileDetails文件和游戏压缩包
				{
					fdvar.path = p;
					fdvar.size = fileinfo.size;
					filedetails.push_back(fdvar);
				}
			}
		}
		while (_findnext(hFile, &fileinfo) == 0);
//		while (_findnext64(hFile, &fileinfo) == 0);
		//while (_findnext(hFile, &fileinfo) == 0);
		//while (_findnext64i32(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
	
}

/*
获取路径及大小并打印出来。
不可用
*/
void OutputFileName(string SourcePath)
{
	int filenum = 0;
	vector<string> Filepath;  //
	vector<_fsize_t> vctsize;
	vector<string>::iterator iter;
	vector<_fsize_t>::iterator iter2;
	
//	GetFileName(SourcePath, Filepath, vctsize);

	for (iter = Filepath.begin(); iter != Filepath.end(); iter++)
	{
		cout <<*iter<<endl;
		filenum++;
	}
	for (iter2 = vctsize.begin(); iter2 != vctsize.end(); iter2++)
	{
		cout << *iter2 << endl;
	}
	cout << "总共" << filenum <<"个文件"<< endl;
}

void GetFileDetails(string srcpath, vector<FileDetails> &filedetails)
{
	vector<FileDetails>::iterator iterfd;
	FileDetails stufiledetails;
	string md5;
	string base64;
	int i = 0;

	GetFileName(srcpath, filedetails);

	for (iterfd = filedetails.begin(); iterfd != filedetails.end(); iterfd++,i++)
	{
		/*
			生成md5
			进行base64编码
		*/
		cout << "正在生成md5..." << endl;
		md5 = getFileMD5((*iterfd).path);  //生成MD5；
		cout << "md5： "<< md5 << endl;
		cout << "base64编码： ";
		base64 = Md5ToBase64(md5); //base64编码md5
		cout << base64 << endl<<endl;
		(*iterfd).md5 = base64;    //base64编码存入

		/*
			处理path
		*/
		(*iterfd).path = (*iterfd).path.erase(0, srcpath.size()+1); //删除上级路径
		DealFilePath((*iterfd).path);               //单斜线改成双斜线

		/*
			判断标志位
			GameFiles为1， ImageFiles为2， configFile为0
		*/
		if ((*iterfd).path.find(GAMEFOLDER) != (*iterfd).path.npos)
		{
			(*iterfd).mode = 1;
		}
		else if ((*iterfd).path.find(IMAGEFOLDER) != (*iterfd).path.npos)
		{
			(*iterfd).mode = 2;
		}
		else
		{
			(*iterfd).mode = 0;
		}

		/*
			根据path生成id
		*/
		(*iterfd).id = DealFileId(srcpath, (*iterfd).path, (*iterfd).mode);

	}
	cout <<endl<< "总共" << i << "个文件。" << endl;
}

void WriteFileDetails(const string srcpath, vector<FileDetails> vctfiledetails)
{
	string filename = srcpath + "\\FileDetails.txt";
	ofstream filedetails;
	string folder1("\"GameFiles\":");
	string folder2("\"ImageFiles\":");
	string folder0("\"configFile\":");
	string id("\"id\"\:\"");   //   "id":"
	string path("\",\"path\"\:\"");    // ","path":"
	string md5("\",\"md5\"\:\"");           // ","md5":"
	string size("\",\"size\"\:");        //","size":
	int count = 0;

	filedetails.open(filename,ios::trunc);  //ios::trunc打开后之前清空文件
	if (filedetails.is_open())
	{
//		cout << "open success" << endl;
		filedetails << "\{";      //写入第一个总的大括号

		//写game文件
		count = 0;
		filedetails << folder1 <<"[";
		for (int i = 0; i < vctfiledetails.size(); i++)
		{
			if (vctfiledetails[i].mode == 1)
			{
				//每个filedetail之前都有逗号隔开，除了第一个，所以引入count来计数
				if (count != 0)
				{
					filedetails << ",";
				}
				filedetails << "{" << id << vctfiledetails[i].id << path << vctfiledetails[i].path << md5 << vctfiledetails[i].md5 << size << vctfiledetails[i].size << "}";
//				filedetails << endl;
				count++;	
			}
		}
		filedetails << "],";
		cout <<endl<< folder1 << "   " << count << endl;

		//写image文件
		count = 0;
		filedetails << folder2 << "[";
		for (int i = 0; i < vctfiledetails.size(); i++)
		{
			if (vctfiledetails[i].mode == 2)
			{
				//每个filedetail之前都有逗号隔开，除了第一个，所以引入count来计数
				if (count != 0)
				{
					filedetails << ",";
				}
				filedetails << "{" << id << vctfiledetails[i].id << path << vctfiledetails[i].path << md5 << vctfiledetails[i].md5 << size << vctfiledetails[i].size << "}";
//				filedetails << endl;
				count++;				
			}
		}
		filedetails << "],";   
		cout << folder2 << "   " << count << endl;

		//写config文件
		count = 0;
		filedetails << folder0;
		//filedetails << "[";    //如果config文件不止一个时需要用到
		for (int i = 0; i < vctfiledetails.size(); i++)
		{
			if (vctfiledetails[i].mode == 0)
			{
				//每个filedetail之前都有逗号隔开，除了第一个，所以引入count来计数
				if (count != 0)
				{
					filedetails << ",";
				}
				filedetails << "{" << id << vctfiledetails[i].id << path << vctfiledetails[i].path << md5 << vctfiledetails[i].md5 << size << vctfiledetails[i].size << "}";
//				filedetails << endl;
				count++;	
			}
		}
		//filedetails << "]";    //如果config文件不止一个时需要用到
		filedetails << "}";      //写入最后一个总大括号
		cout << folder0 <<"   "<< count << endl;
		cout <<endl<< "写入文件成功。" << endl<<endl;
	}
	else
	{
		cout << "打开文件失败" << endl;
	}

	filedetails.close();

}



