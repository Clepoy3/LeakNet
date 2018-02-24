#include "winquake.h"
#include "tier0/vcrmode.h"
//#include "vstdlib/random.h"

#include <winsock.h>
#include <wininet.h>

#include <float.h> // VXP
//#include <fstream> // VXP

#include "stun_request.h"
//#pragma comment(lib, "ws2_32.lib")

//using namespace std;

int stun_xor_addr(char* StunHostname, short StunPort, short local_port, char* return_ip_port)
{
	if ( !InternetCheckConnection( "http://www.google.com", FLAG_ICC_FORCE_CONNECTION, 0 ) )
	{
		DevWarning( "STUN: Internet is not active or application is blocked by firewall\n" );
		return STUN_NETWORKERROR;
	}

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
        if( he == NULL )
        {
            //printf("Couldn't gethostbyname\n");
			DevWarning( "STUN: Couldn't get IP by hostname\n" );
            return STUN_IPERROR;
        }
        addr = *(DWORD*)(he->h_addr_list[0]);
    }
 
    // create socket UDP
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if( sockfd == INVALID_SOCKET )
    {
        //printf("Couldn't socket\n");
		DevWarning( "STUN: Couldn't create UDP socket\n" );
		closesocket(sockfd);
        return STUN_SOCKETERROR;
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
    if( n == SOCKET_ERROR )
    {
        //printf("Couldn't bind socket\n");
		DevWarning( "STUN: Couldn't bind socket\n" );
		closesocket(sockfd);
        return STUN_BINDINGERROR;
    }
 
    //printf("bind result=%d\n",n);
    //printf("socket opened to  %s:%d  at local port %d\n", StunHostname, StunPort, local_port);
 
    //## first bind
    * (short *)(&bindingReq[0]) = htons(0x0001);    // stun_method
    * (short *)(&bindingReq[2]) = htons(0x0000);    // msg_length
    * (int *)(&bindingReq[4])   = htonl(0x2112A442);// magic cookie
    *(int *)(&bindingReq[8]) = htonl(0x63c7117e);   // transaction ID
    *(int *)(&bindingReq[12])= htonl(0x0714278f);
    *(int *)(&bindingReq[16])= htonl(0x5ded3221);
 
    //printf("Send data ...\n");
 
    n = sendto(sockfd, (const char*)bindingReq, sizeof(bindingReq), 0, (struct sockaddr*)&servaddr, sizeof(servaddr));
    if (n == -1)
    {
        //printf("Couldn't sendto\n");
		DevWarning( "STUN: Couldn't send request\n" );
		closesocket(sockfd);
        return STUN_REQUESTERROR;
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
		DevWarning( "STUN: Couldn't set socket options\n" );
		closesocket(sockfd);
		return STUN_SOPTIONSERROR;
	}
 
    //n = recvfrom(sockfd, (char*)buf, STUN_MAXLINE, 0, NULL, 0);
	n = g_pVCR->Hook_recvfrom(sockfd, (char*)buf, STUN_MAXLINE, 0, NULL, 0);
    if (n == -1)
    {
        //printf("Couldn't recvfrom\n");
		DevWarning( "STUN: Couldn't receive data\n" );
		closesocket(sockfd);
        return STUN_RECEIVEERROR;
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
					DevWarning( "STUN: STUN server cannot send proper IP\n" );
					closesocket(sockfd);
					return STUN_PARSEERROR;
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
	{
		DevWarning( "STUN: Unknown error!\n" );
		return STUN_UNKNOWNERROR;
	}

    //printf("socket closed !\n");
	DevMsg( "STUN: Your external IP address must be %s\n", return_ip_port );

    return STUN_SUCCESS;
}