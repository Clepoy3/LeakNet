#include "winquake.h"
#include "tier0/vcrmode.h"
//#include "vstdlib/random.h"

#include <winsock.h>

#include <float.h> // VXP
//#include <fstream> // VXP

#include "stun_request.h"
//#pragma comment(lib, "ws2_32.lib")

//using namespace std;

int stun_xor_addr(char* StunHostname, short StunPort, short local_port, char* return_ip_port)
{
    struct sockaddr_in servaddr;
    struct sockaddr_in localaddr;
    unsigned char buf[STUN_MAXLINE];
    int sockfd, i;
    unsigned char bindingReq[20];
    //int stun_method,msg_length;
    short attr_type;
    short attr_length;
    short port;
    //short n;
	int n;
 
    ULONG addr;
    if( (addr = inet_addr(StunHostname)) == INADDR_NONE )
    {
        struct hostent* he = gethostbyname(StunHostname);
        if( NULL == he )
        {
            //printf("Couldn't gethostbyname\n");
            return -1;
        }
        addr = *(DWORD*)(he->h_addr_list[0]);
    }
 
    // create socket UDP
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if( INVALID_SOCKET == sockfd )
    {
        //printf("Couldn't socket\n");
		closesocket(sockfd);
        return -2;
    }
 
    // server
    memset(&servaddr, 0, sizeof(servaddr));
 
    servaddr.sin_addr.S_un.S_addr = addr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(StunPort);
 
    // local
    memset(&localaddr, 0, sizeof(localaddr));
    localaddr.sin_family = AF_INET;
    localaddr.sin_port = htons(local_port);
 
    n = bind(sockfd, (struct sockaddr *)&localaddr, sizeof(localaddr));
    if( SOCKET_ERROR == n )
    {
        //printf("Couldn't bind socket\n");
		closesocket(sockfd);
        return -3;
    }
 
    //printf("bind result=%d\n",n);
    //printf("socket opened to  %s:%d  at local port %d\n", StunHostname, StunPort, local_port);
 
    //## first bind
    * (short *)(&bindingReq[0]) = htons(0x0001);    // stun_method
    * (short *)(&bindingReq[2]) = htons(0x0000);    // msg_length
    * (int *)(&bindingReq[4])   = htonl(0x2112A442);// magic cookie
    *(int *)(&bindingReq[8]) = htonl(0x63c7117e);   // transacation ID
    *(int *)(&bindingReq[12])= htonl(0x0714278f);
    *(int *)(&bindingReq[16])= htonl(0x5ded3221);
 
    //printf("Send data ...\n");
 
    n = sendto(sockfd, (const char*)bindingReq, sizeof(bindingReq), 0, (struct sockaddr*)&servaddr, sizeof(servaddr));
    if (n == -1)
    {
        //printf("Couldn't sendto\n");
		closesocket(sockfd);
        return -4;
    }
 
    // time wait
    //Sleep(1);
    //printf("Read recv ...\n");

	struct timeval tv;
	//tv.tv_sec = 0;
	//tv.tv_usec = 1 * 1000;
	tv.tv_sec = 1000;
	if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(tv)) < 0)
	{
		closesocket(sockfd);
		return -5;
	}
 
    //n = recvfrom(sockfd, (char*)buf, STUN_MAXLINE, 0, NULL, 0);
	n = g_pVCR->Hook_recvfrom(sockfd, (char*)buf, STUN_MAXLINE, 0, NULL, 0);
    if (n == -1)
    {
        //printf("Couldn't recvfrom\n");
		closesocket(sockfd);
        return -5;
    }
 
    //printf("Response from server:\n");

	bool done = false;
 
    if (*(short *)(&buf[0]) == htons(0x0101))
    {
        //printf("STUN binding resp: success !\n");
 
        // parse XOR
        n = htons(*(short *)(&buf[2]));
        i = 20;
        while(i<sizeof(buf))
        {
            attr_type = htons(*(short *)(&buf[i]));
            attr_length = htons(*(short *)(&buf[i+2]));
            if (attr_type == 0x0020)
            {
                // parse : port, IP 
 
                port = ntohs(*(short *)(&buf[i+6]));
                port ^= 0x2112;
                /*
                printf("@port = %d\n",(unsigned short)port);
                printf("@ip   = %d.",buf[i+8] ^ 0x21);
                printf("%d.",buf[i+9] ^ 0x12);
                printf("%d.",buf[i+10] ^ 0xA4);
                printf("%d\n",buf[i+11] ^ 0x42);
                */

				//printf( "%d is %s a number", (buf[i+8]^0x21), (_isnan(buf[i+8]^0x21) ? "not" : "") );
				//cout << (buf[i+8]^0x21) << "is " << (_isnan(buf[i+8]^0x21) ? "not " : "") << "a number" << endl;

				if ( _isnan(buf[i+8]^0x21) )
				{
					//printf("Server sended a bunch of crap\n");
					closesocket(sockfd);
					return -6;
				}

                //sprintf(return_ip_port,"%d.%d.%d.%d:%d",
                //    buf[i+8]^0x21, buf[i+9]^0x12,
                //    buf[i+10]^0xA4, buf[i+11]^0x42, port);

				sprintf( return_ip_port, "%d.%d.%d.%d",
					buf[i+8]^0x21, buf[i+9]^0x12,
					buf[i+10]^0xA4, buf[i+11]^0x42 );
    
				done = true;
                break;
            }
            i+=(4+attr_length);
        }
    }
 
    // TODO: bind again
    closesocket(sockfd);

	if ( !done )
		return -7;

    //printf("socket closed !\n");

    return 0;
}