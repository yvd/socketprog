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

int md5modn(int a, int b)
{
	return 1;//lol
}

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
		std::cout<<"You forgot to give the node number :P\nExiting.......\n";
		return -1;
	}
	int node_id = atoi(argv[1]),c;
	char recieve_buffer[1000];
	loadcfg();
	int sockfd;
	struct sockaddr_in my_addr, client, dest_node;
	socklen_t clientlength = sizeof(client);
	sockfd = socket(PF_INET,SOCK_DGRAM,0);
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(atoi(port[node_id].c_str()));
	my_addr.sin_addr.s_addr = inet_addr(addr[node_id].c_str());
	memset(&(my_addr.sin_zero),'\0',8);//zero the rest of the struct
	bind(sockfd,(struct sockaddr *)&my_addr, sizeof(struct sockaddr));
	
	do{  //k gives the number of bytes recieved
		int k = recvfrom(sockfd,recieve_buffer,1000,0,(struct sockaddr*)&client,&clientlength);
		if(k < 0)
		{
			std::cout<<"Error in Receiving data :(.\nContinuing to listen\n";
			continue;
		}
		else if(k > 0)
		{	
			std::string pkt = std::string(recieve_buffer);//converting the msg to string format
			int md5sum = 0;//md5sum
			int z = md5modn(md5sum,n);
			if(z != node_id)//current node is not the destination node for the client
			{
				std::string dst_addr = addr[z],dst_finfo = finfo[z];//get the info of the destination node
				int dst_port = atoi(port[z].c_str());
				//To send data to that node, create a struct sockaddr for that node
                dest_node.sin_family = AF_INET; //assigning family set
                dest_node.sin_port = htons(dst_port); //assigning a port number
                dest_node.sin_addr.s_addr = inet_addr(dst_addr.c_str()); //assigning ip address
                memset(&(dest_node.sin_zero),'\0',8); //the rest of the struct is given value zero
                //Recieved data is now transferred to the destination node.
                //k1 gives the number of bytes sent
                int k1 = sendto(sockfd,recieve_buffer,1000,0,(struct sockaddr*)&dest_node,sizeof(struct sockaddr)); 
                if(k1<0) //Sending failed as number of bytes sent is less than 0  :/
                {
                    std::cout<<"Sending to the Destination Node "<<z<<" failed :/\nExiting.......\n";  
                    return 0;
                }
			}
			else
			{
				//ping node z and continue listening?
			}
		}
	}
	while(1);
	c = close(sockfd);
	std::cout<<addr[0]<<" "<<addr[1]<<"\n";
	std::cout<<port[0]<<" "<<port[1]<<"\n";
	std::cout<<"number of nodes is: "<<n<<"\n";
	return 0;
}
