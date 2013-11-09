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

std::vector<std::string> addr,port,finfo;//addr stores the addresses and finfo stores the file info
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
		int fnum,psn,i;
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
	if(argc != 2)
	{
		std::cout<<"Dude give an argument.Exiting.......\n";
		return -1;
	}
	int node_id = atoi(argv[1]),c;
	char buffer[10000];
	loadcfg();
	int sockfd;
	struct sockaddr_in my_addr, client;
	socklen_t clientlength = sizeof(client);
	sockfd = socket(PF_INET,SOCK_DGRAM,0);
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(atoi(port[node_id].c_str()));
	my_addr.sin_addr.s_addr = inet_addr(addr[node_id].c_str());
	memset(&(my_addr.sin_zero),'\0',8);//zero the rest of the struct
	bind(sockfd,(struct sockaddr *)&my_addr, sizeof(struct sockaddr));
	
	do{
		int k = recvfrom(sockfd,buffer,10000,0,(struct sockaddr*)&client,&clientlength);
		if(k < 0)
		{
			cout<<"Error in Receiving data.Exiting...........\n";
			return -1;
		}
		else if(k > 0)
		{
			
		}
	}while(1);
	c = close(sockfd);
	std::cout<<addr[0]<<" "<<addr[1]<<"\n";
	std::cout<<port[0]<<" "<<port[1]<<"\n";
	std::cout<<"number of nodes is: "<<n<<"\n";
	return 0;
}