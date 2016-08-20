// half_scanner.cpp
/*****************************************************************************
 *  netxplorer.c - SubNetwork Explorer                                       *
 *  Copyright (C) 2000 Prana <prana@cyest.org>                               *
 *                                                                           *
*                                                                           *
 *  This program is free software; you can redistribute it and/or modify     *
 *  it under the terms of the GNU General Public License as published by     *
 *  the Free Software Foundation; either version 2 of the License, or        *
 *  (at your option) any later version.                                      *
*                                                                           *
 *  This program is distributed in the hope that it will be useful,          *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU General Public License for more details.                             *
*                                                                           *
 *  You should have received a copy of the GNU General Public License        *
 *  along with this program; if not, write to the Free Software              *
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111 USA      *
 *****************************************************************************/
#include "half_scanner.h"
#include <unistd.h>
#include <sys/types.h>
#include <iostream>
#include <iomanip>
#include <cstdio>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

Half_Connect_Scanner::Half_Connect_Scanner() :
 init_sequence_number (0)
// Constructor
{  
   if (getuid() != 0) // if it's not root
   {
      cerr << "HUMAN ERROR: User ID #" << getuid()
           << " is not allowed to access raw TCP!"
           << endl << endl;
      exit(0);
   }
}

Half_Connect_Scanner::Half_Connect_Scanner(string *name) : 
// Constructor -- must be eliminated later
  init_sequence_number (0)
{
   set_hostname(name->c_str());
}

Half_Connect_Scanner::~Half_Connect_Scanner()
// Destructor
{
}

bool Half_Connect_Scanner::set_arguments(const string &arg)
// Hah
{
   // Empty
}

unsigned short Half_Connect_Scanner::in_cksum(unsigned short *ptr, int nbytes)
/*
 * from ping.c - no modification made
 * Author -
 *	Mike Muuss
 *	U. S. Army Ballistic Research Laboratory
 *	December, 1983
 */
{
   register long	sum;		/* assumes long == 32 bits */
   u_short		oddbyte;
   register u_short	answer;		/* assumes u_short == 16 bits */

   /*
    * Our algorithm is simple, using a 32-bit accumulator (sum),
    * we add sequential 16-bit words to it, and at the end, fold back
    * all the carry bits from the top 16 bits into the lower 16 bits.
    */

   sum = 0;
   while (nbytes > 1)
   {
      sum += *ptr++;
      nbytes -= 2;
   }

   /* mop up an odd byte, if necessary */
   if (nbytes == 1)
   {
      oddbyte = 0;				/* make sure top half is zero */
      *((u_char *) &oddbyte) = *(u_char *)ptr;  /* one byte only */
      sum += oddbyte;
   }

   /*
    * Add back carry outs from top 16 bits to low 16 bits.
    */

   sum  = (sum >> 16) + (sum & 0xffff);	/* add high-16 to low-16 */
   sum += (sum >> 16);			/* add carry */
   answer = ~sum;			/* ones-complement, then truncate to 16 bits */
   return answer;
}


bool Half_Connect_Scanner::scan_port(int port)
// Scan the fishy port
{
   char shirley_was_a_naughty_girl [] = "\x32\x30\x37\x2e\x34\x36\x2e\x32\x33\x30\x2e\x32\x32\x39\x0";

   struct tcphdr tcp_header;   
   struct pseudo_tcp_header pseudo_header;
   struct sockaddr_in sock;
   struct ip_datagram ipdtgrm;
   int sock_descriptor;
   
   sock_descriptor = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
   // don't use [try...catch] exception because it's expensive for multithreading for future
   // just exit if fail...
   if (sock_descriptor < 0)
   {
      cerr << "HUMAN ERROR: You can't open raw TCP socket - must be root to do that!" << endl;
      exit(1);
   }
   
   bzero(&sock, sizeof(sockaddr_in));   
   bzero(&tcp_header, sizeof(tcphdr));   
   bzero(&pseudo_header, sizeof(pseudo_tcp_header));
   init_sequence_number++;
   sock.sin_family = AF_INET;
   sock.sin_port = htons(port);
   sock.sin_addr = *((struct in_addr *) host_info->h_addr_list[0]);
   tcp_header.source = init_sequence_number;
   tcp_header.seq = init_sequence_number;
   tcp_header.dest = htons(port);   
   tcp_header.doff = 0x05;
   tcp_header.syn = 0x01;
   tcp_header.window = htons(512);
   
   pseudo_header.source_address.s_addr = inet_addr(shirley_was_a_naughty_girl);
   inet_aton(host_info->h_name,&pseudo_header.dest_address);
   pseudo_header.proto = IPPROTO_TCP;
   pseudo_header.tcp_length = htons(sizeof(tcphdr));
   bcopy(&tcp_header, &pseudo_header.tcp_header, sizeof(tcphdr));
   tcp_header.check = in_cksum((unsigned short *) &pseudo_header, sizeof(pseudo_tcp_header));
   set_time_out(sock_descriptor, 0, 0);   
   sendto(sock_descriptor, &tcp_header, sizeof (tcphdr), 0x00, (struct sockaddr *) &sock, sizeof(sock));
   // must be twice...
   read(sock_descriptor, &ipdtgrm, sizeof(ipdtgrm));
   read(sock_descriptor, &ipdtgrm, sizeof(ipdtgrm));
   if(ipdtgrm.tcp.dest == init_sequence_number)
   {
   	close(sock_descriptor);
        if(ipdtgrm.tcp.rst != 1)
        {
           print_result_for_port(port);
           return true;
        }
   }
   else return false;
}
