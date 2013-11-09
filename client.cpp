#include <iostream>
#include <fstream>
#include "vector"
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>
#include <string.h>

std::vector<std::string> addr,port,finfo;//addr stores the addresses,port stores the port numbers and finfo stores the file info
int n;// number of nodes

void loadcfg()// loads FileMesh.cfg i.e sets n, addr and finfo according to the file.
{
	std::string f;
	f = "FileMesh.cfg";
	std::ifstream input_file;
	input_file.open(f.c_str());
	while(input_file.good())
	{
		std::string str, par;
		int psn;
		getline(input_file,str);
		
		if(str.length() < 2)
		{
			break;
		}
		psn = str.find(":");
		par = str.substr(0,psn);
		addr.push_back(par);
		str = str.substr(psn+1);
		psn = str.find(" ");
		par = str.substr(0,psn);
		port.push_back(par);
		str = str.substr(psn+1);
		finfo.push_back(str);
	}
	input_file.close();
	n = addr.size();
}

int main(int argc, char const *argv[])
{
	int sockfd,opflag,node1;
	struct sockaddr_in my_addr;
	loadcfg();
	std::cout<<"Input Operation:\n1)Uploading File 2)Downloading File : ";
	std::cin>>opflag;
	if(!(opflag == 1 || opflag == 2))
	{
		std::cout<<"\nInvalid Input.\nExiting .....\n";
		return -1;
	}
	std::string fname;
	std::cout<<"\nEnter the file name you wanted to ";
	if(opflag == 1)
		std::cout<<"upload: ";
	else
		std::cout<<"download: ";
	std::cin>>fname;
	std::cout<<"\nInput number of the node you wanted to contact: ";
	std::cin>>node1;
	if(node1 > n -1)
	{
		std::cout<<"\nInvalid Node Number.\nExiting ....\n";
		return -1;
	}
	return 0;
}