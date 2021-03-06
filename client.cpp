#include <iostream>
#include <fstream>
#include "vector"
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>
#include <cstring>
#include "md5.h"
#include <cstdlib>
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
	int sockfd,opflag,node,node_id,dst_port,k1,tcpsockfd,tcp1sockfd;
	struct sockaddr_in my_addr,node_addr,node1;
	std::string fname,md5sum = "",dst_addr,pkt="",op;
	char buffer[1000];
	loadcfg();
	
	std::cout<<"Input Operation:\n1)Uploading File 2)Downloading File : ";
	std::cin>>opflag;
	if(!(opflag == 1 || opflag == 2))
	{
		std::cout<<"\nInvalid Input.\nExiting .....\n";
		return -1;
	}
	
	std::cout<<"\nEnter the file name you wanted to ";
	if(opflag == 1)
	{
		std::cout<<"upload: ";
		op = "1";
	}
	else
	{
		std::cout<<"download: ";
		op = "2";
	}
	std::cin>>fname;
	std::cout<<"\nInput number of the node you wanted to contact: ";
	std::cin>>node;
	if(node > n -1)
	{
		std::cout<<"\nInvalid Node Number.\nExiting ....\n";
		return -1;
	}

	if(opflag == 1)//uploading file or store
	{
		system(("md5sum "+fname +"| awk '{print($1)}' > tmp").c_str()); 
        std::ifstream tempfile;
        tempfile.open("tmp");
        std::string str1;
        getline(tempfile,str1);
        md5sum=str1;
        tempfile.close();
        system("rm tmp");
        std::cout<<md5modn(md5sum,n)<<" "<<str1<<"\n";
        node_id = md5modn(md5sum,n);
	}
	else  //downloading file
	{
	}

    if((sockfd = socket(PF_INET,SOCK_DGRAM,0)) == -1)
    {
    	std::cout<<"Unable to create socket\n";
        return 0;
    }
    std::cout<<"Created socket :)\n";
    my_addr.sin_family = AF_INET; //assigning family set
	my_addr.sin_port = htons('4000'); //assigning port number
	my_addr.sin_addr.s_addr = inet_addr("127.0.0.1");//assigning ip address
	memset(&(my_addr.sin_zero),'\0',8);//zero the rest of the struct

    if(bind(sockfd,(struct sockaddr*)&my_addr,sizeof(my_addr))<0)
    {
        std::cout<<"Unable to bind to the socket.\n";
        return 0;
    }
    std::cout<<"Binded :)\n";
    dst_port = atoi(port[node_id].c_str());
    dst_addr = addr[node_id];
    node_addr.sin_family = AF_INET; //assigning family set
    node_addr.sin_port = htons(dst_port); //assigning port number
    node_addr.sin_addr.s_addr = inet_addr(dst_addr.c_str()); //assigning ip address
    memset(&(node_addr.sin_zero),'\0',8); //zero the rest of the struct
    pkt = dst_addr + " " + port[node_id] + " " + op + " " + md5sum;
    k1 = sendto(sockfd,pkt.c_str(),1000,0,(struct sockaddr*)&node_addr,sizeof(struct sockaddr));
    if(k1<0)
    {
        std::cout<<"Sending to the Destination Node "<<node_id<<" failed :/\nExiting.......\n";  
        return 0;
    }
    close(sockfd);


    if((tcpsockfd = socket(PF_INET,SOCK_STREAM,0)) < 0)
    {
     	std::cout<<"Unable to create socket\n";
        return 0;
    }


    if(bind(tcpsockfd,(struct sockaddr*)&my_addr,sizeof(my_addr))<0)
    {
        std::cout<<"Unable to bind to the socket.\n";
        return 0;
    }

    if(listen(tcpsockfd, 10) < 0){
        std::cout<<"Unable to listen.\nExiting.......\n";
        return 0;               
    }
    socklen_t size = sizeof(struct sockaddr_in);
    if ((tcp1sockfd = accept(tcpsockfd, (struct sockaddr *)&node1, &size )) < 0 ){
        std::cout<<"Accepting via tcp socket failed.\nExiting.......\n";
        return 0;        
    }
    
    if(opflag == 1)
    {
    	int length, bytes_sent;
    	std::ifstream in;
    	std::string s;
    	in.open(fname.c_str());
    	if(in.is_open()) {
    		while(getline(in, s)){
    			s += "\n";
    			length = strlen(s.c_str());
    			
    			if((bytes_sent = send(tcp1sockfd,s.c_str(), length+1, 0)) < 0) {
    				std::cout<<"lololol sending failed\n";
				return 0;
			}
		}
	}
	
	else std::cout<<"Could not open the file lolololol";
    }
    
    else
    {
    	char buff[1024];
    	int n_bytes;
    	std::ofstream out;
    	out.open((md5sum+".txt").c_str());
    	
    	while((n_bytes = recv(tcp1sockfd, buff, 1024, 0)) > 0) {
    		buff[n_bytes] = '\0';
    		std::string stuff(buff);
    		std::cout<<stuff<<"\n";
    		out<<stuff<<"\n";
    	}
    	
    	out.close();
    	
    	if(n_bytes < 0) std::cout<<"Error in receving lololol\n";
    	else if(n_bytes == 0) std::cout<<"File retrieved\n";
    	
    }
    
    close(tcp1sockfd);
    close(tcpsockfd);

	return 0;
}
