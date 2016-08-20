// half_scanner.h
/*****************************************************************************
 *  SubNetwork Explorer                                       	             *
 *  Copyright (C) 2000 Prana                              *
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
#ifndef HALF_CONNECT_SCANNER_H
#define HALF_CONNECT_SCANNER_H
 
#include "scanner.h"
#include <string>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>

class Half_Connect_Scanner : public Scanner
// SYN scan
{
   public:
      Half_Connect_Scanner();
      Half_Connect_Scanner(string *name);
      ~Half_Connect_Scanner();
      bool set_arguments(const string &arg);
      virtual bool scan_port(int portnumber);
      unsigned short in_cksum(unsigned short *ptr, int nbytes);
   private:
      // this is only for the specific stuff...
      int init_sequence_number; // initial sequence
};

// RFC 793
struct pseudo_tcp_header
{
   struct in_addr source_address;
   struct in_addr dest_address;
   unsigned char padding;
   unsigned char proto;
   unsigned short tcp_length;
   struct tcphdr tcp_header;
};

// from TCP/IP Illustrated chapter 17
// Encapsulated IP datagram
struct ip_datagram
{
   struct iphdr ip;
   struct tcphdr tcp;
//   unsigned char ignored_data[IP_MAXPACKET];
};

#endif
