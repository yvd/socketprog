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
#include "md5.h"
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
	if((sockfd = socket(PF_INET,SOCK_DGRAM,0)) == -1)//it means socket was not created
	{
		std::cout<<"Unable to create socket\n";
		return -1;
	}
	std::cout<<"sockfd value : "<<sockfd<<"\n";
	my_addr.sin_family = AF_INET; //assigning family set
	my_addr.sin_port = htons(atoi(port[node_id].c_str())); //assigning port number
	my_addr.sin_addr.s_addr = inet_addr(addr[node_id].c_str());//assigning ip address
	memset(&(my_addr.sin_zero),'\0',8);//zero the rest of the struct
	if((bind(sockfd,(struct sockaddr *)&my_addr, sizeof(struct sockaddr))) == -1)//it means binding failed
	{
		std::cout<<"Unable to bind to the socket.\n";
		return -1;
	}
	std::cout<<"Binded!!\n";
	do{  //k gives the number of bytes recieved
		int k = recvfrom(sockfd,recieve_buffer,1000,0,(struct sockaddr*)&client,&clientlength);
		if(k < 0)
		{
			std::cout<<"Error in Receiving data :(.\nContinuing to listen\n";
			continue;
		}
		else if(k > 0)
		{	/* Packet Format
                  IPAdress Port Request md5sum
               eg:  10.6.162.77 1300 1 c19a6e1f83945a2f92193a85cf4b44d4  
			*/
			std::string pkt = std::string(recieve_buffer);//converting the msg to string format
			int psn;
			psn = pkt.find(" ");
			std::string client_addr = pkt.substr(0,psn);
			pkt = pkt.substr(psn + 1);
			psn = pkt.find(" ");
			int client_port = atoi((pkt.substr(0,psn)).c_str());
			pkt = pkt.substr(psn + 1);
			psn = pkt.find(" ");
			std::string req = pkt.substr(0,psn);
			std::string md5str = pkt.substr(psn + 1);
			int z = md5modn(md5str,n);
			if(z == node_id)//current node is the destination node for the client
			{
				int tcpsockfd,k2;
				struct sockaddr_in client_tcp;
				socklen_t client_tcplen;
				char recieve_buffer1[1000];
				//int k2; // Number of bytes

				if((tcpsockfd = socket(PF_INET,SOCK_STREAM,0)) == -1)
                {
                    std::cout<<"Unable to create TCP socket\n";
					return -1;	
                }
                std::cout<<"tcp socket created.YAY!!\n";

                client_tcp.sin_family = AF_INET; //assigning family set
				client_tcp.sin_port = htons(client_port); //assigning port number
				client_tcp.sin_addr.s_addr = inet_addr(client_addr.c_str());//assigning ip address
				memset(&(client_tcp.sin_zero),'\0',8);//zero the rest of the struct

				if(connect(tcpsockfd,(struct sockaddr*)&client_tcp,sizeof(struct sockaddr)) == -1 )
                {
                    std::cout<<"TCP connection cannot be established.\nExiting.......";
                    return -1;
                }

                if(req == "1")
                {
                	std::ofstream out;
                	out.open((finfo[node_id] + std::string(argv[1]) + "/" + md5str + ".txt").c_str());

                	while((k2 = recv(tcpsockfd, recieve_buffer1, 1000, 0)) > 0) {
                		recieve_buffer1[k2] = '\0';
                		std::string stuff(recieve_buffer1);
                		out<<stuff;
                	}

                	out.close();

                	if(k2 < 0) {
                		std::cout<<"Data Receiving Error FLOP\n";
                	}

                	else if(k2 == 0) {
                		std::cout<<"Connection closed on user's side LOLMAXX\n";
                	}
                }

                else{
                	std::ifstream in;
                	int length, bytes_sent;
                	std::string s;
                	in.open((finfo[node_id] + std::string(argv[1]) + "/" + md5str + ".txt").c_str());

                	if(in.is_open()) {
                		while(getline(in, s)) {
                			s += "\n";
                			length = strlen(s.c_str());
                			std::cout<<s;
                			if((bytes_sent = send(tcpsockfd, s.c_str(), length + 1, 0)) > 0) {
                				std::cout<<"Failed to send haha\n";
                			}
                		}
                	}

                	else{std::cout<<"Couldn't open the file man, are you even sober?\n";}
                	in.close();
                }

			}
			else //current node is not the destination node for the client
			{
				std::string dst_addr = addr[z],dst_finfo = finfo[z];//get the info of the destination node
				int dst_port = atoi(port[z].c_str());
				//To send data to that node, create a struct sockaddr for that node
                dest_node.sin_family = AF_INET; //assigning family set
                dest_node.sin_port = htons(dst_port); //assigning port number
                dest_node.sin_addr.s_addr = inet_addr(dst_addr.c_str()); //assigning ip address
                memset(&(dest_node.sin_zero),'\0',8); //zero the rest of the struct
                //Recieved data is now transferred to the destination node.
                //k1 gives the number of bytes sent
                int k1 = sendto(sockfd,recieve_buffer,1000,0,(struct sockaddr*)&dest_node,sizeof(struct sockaddr)); 
                if(k1<0) //Sending failed as number of bytes sent is less than 0  :/
                {
                    std::cout<<"Sending to the Destination Node "<<z<<" failed :/\nExiting.......\n";  
                    return 0;
                }
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
