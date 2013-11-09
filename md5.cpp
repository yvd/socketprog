#include "md5.h"
int md5modn(std::string hex,int n)//converts a hex string to int(decimal)
{
	std::string str = "";
	int i,k = hex.length(),num=0;
	for(i = 0;i < k; i++)
	{
		std::string conv;
		switch(hex[i])
		{
			case '0':	
				conv = "0000";
				break;
			case '1':
				conv = "0001";
				break;
			case '2':
				conv = "0010";
				break;
			case '3':
				conv = "0011";
				break;
			case '4':
				conv = "0100";
				break;
			case '5':
				conv = "0101";
				break;
			case '6':
				conv = "0110";
				break;
			case '7':
				conv = "0111";
				break;
			case '8':
				conv = "1000";
				break;
			case '9':
				conv = "1001";
				break;
			case 'A':
				conv = "1010";
				break;
			case 'B':
				conv = "1011";
				break;
			case 'C':
				conv = "1100";
				break;
			case 'D':
				conv = "1101";
				break;
			case 'E':
				conv = "1110";
				break;
			case 'F':
				conv = "1111";				
				break;
			case 'a':
				conv = "1010";
				break;
			case 'b':
				conv = "1011";
				break;
			case 'c':
				conv = "1100";
				break;
			case 'd':
				conv = "1101";
				break;
			case 'e':
				conv = "1110";
				break;
			case 'f':
				conv = "1111";
				break;	
		}
		str = str + conv;
	}
	k = str.length();
    int A[k],val;
    A[0] = 1;
    for(int i=1;i<k;i++)
    {
        val = 2*A[i-1];
        if(val>=n) 
        	val = val - n;
        A[i] = val;
    }

	for(i = 0;i<k;i++)
	{
		if(str[i] == '1')
			num = num + A[k-i-1];
	}
	return num%n;
}