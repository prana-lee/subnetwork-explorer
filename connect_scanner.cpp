// connect_scanner.cpp
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

#include <iostream>
#include <iomanip>
#include <cstdio>
#include <cerrno>
#include <string>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <rpc/rpc.h>
#include <rpc/pmap_prot.h>
#include <rpc/pmap_clnt.h>
#include "connect_scanner.h"

using namespace std;

#define ENTER_KEY_CODE '\n'
#define scan_SSH scan_regular
#define scan_SMTP scan_regular

static string *sock_get_buffer(int sock)
// Read from the socket
{
   string *pstring = new string("");
   const int limit_read = 512;
   char read_char;   
   int count = 1;
   int result;
   
   read(sock,&read_char,1);
   (*pstring)+=read_char;   
   result = (read_char != ENTER_KEY_CODE);
   while ((count <= limit_read) && result )
   {
        read(sock,&read_char,1);
        (*pstring)+=read_char;        
        result = (read_char != ENTER_KEY_CODE);
        count++;
   }
  return pstring; 
}

void sock_send_buffer(int sock, char *buffer) {
// Send buffer
   write(sock,buffer,strlen(buffer));
}

Connect_Scanner::Connect_Scanner()
// Constructor
{
   for (int i = 0; i < 0xFFFF; i++)
      modules_for_port[i] = NULL;
     
   modules_for_port[21] = scan_FTP;
   modules_for_port[22] = scan_SSH;
   modules_for_port[25] = scan_SMTP;
   modules_for_port[80] = scan_HTTP;
   modules_for_port[110] = scan_regular;
   modules_for_port[111] = scan_SunRPC;
   modules_for_port[139] = scan_NetBios;
   modules_for_port[143] = scan_regular;
   modules_for_port[631] = scan_CUPS;
}

Connect_Scanner::Connect_Scanner(string *name)
// Constructor - set the hostname
{
   set_hostname(name->c_str());
}

Connect_Scanner::~Connect_Scanner()
// Destructor
{
}

bool Connect_Scanner::scan_port(int port)
/*
   Check existence of port connection
   
   There is a known bug that it will keep scanning until the end
   even though the host doesn't exist. This will be fixed in the
   next release.   
*/
{
   int scan_result;
   int sock_descriptor;
   struct sockaddr_in sock;
     
   if ((sock_descriptor = socket(PF_INET, SOCK_STREAM, 0)) < 0)
     return false;
   bzero(&sock, sizeof(sockaddr_in));
   sock.sin_family = AF_INET;			// set the family to AF_INET
   sock.sin_port = htons(port);			// set the port number
   sock.sin_addr = *((struct in_addr *) host_info->h_addr_list[0]);
   set_time_out(sock_descriptor, 0, 0);
   scan_result = connect(sock_descriptor, (struct sockaddr *) &sock, sizeof(sock));
   // if connect is successful
   if (scan_result < 0)
	switch(errno)
	{
           case ENETUNREACH:
              cerr << "Network can't be reach - maybe it doesn't exist." << endl;
              break;        
           case ETIMEDOUT:
              cerr << "Connection timed out :(" << endl;
              break;
        }
   if (scan_result = !scan_result)
   // This is correct -> "=!" not "!=". It's not wrong.
   {
      print_result_for_port(port);       
      if (modules_for_port[port]) modules_for_port[port] (sock_descriptor);
   }
   close(sock_descriptor);
   return scan_result;
}

bool scan_regular(int sock_descriptor)
// Scan Regular - just fetch the version info
{
   string *s;
 
   s = sock_get_buffer(sock_descriptor);
   cout << *s << endl;
   delete(s);
}

bool scan_NetBios(int sock_descriptor)
//   Samba - not yet...
{
}

bool scan_SunRPC(int sock_descriptor)
//   Sun RPC - copied from rpcinfo and modified it -  eliminate the unnecessary stuff
{
  struct sockaddr_in server_addr;
  struct pmaplist *portmap_list = NULL;
  struct timeval timeout = {0, 10};
  register CLIENT *client;
  struct rpcent *rpc_info;
  bool return_result = false;
  
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PMAPPORT);
  
  setprotoent(true);

  if ((client = clnttcp_create (&server_addr, PMAPPROG, PMAPVERS, &sock_descriptor, 50, 500)) == NULL ||
      (clnt_call (client, PMAPPROC_DUMP, (xdrproc_t) xdr_void, NULL, (xdrproc_t) xdr_pmaplist, (caddr_t)
       &portmap_list, timeout) != RPC_SUCCESS)) 
  {
     cout << "Error: Unable to contact portmapper" << endl;
  }
  else if (portmap_list)
  {
     cout.setf(ios::right);  
     for (; portmap_list != NULL; portmap_list = portmap_list->pml_next)
     {
        rpc_info = getrpcbynumber (portmap_list->pml_map.pm_prog);
        cout << "\tProgram" << setw(7) << portmap_list->pml_map.pm_prog << " ";
        cout.setf(ios::left);
        if (rpc_info)
           cout << "- " << setw(12) << rpc_info->r_name
                << " v" << portmap_list->pml_map.pm_vers << " running on port";
        cout.unsetf(ios::left);
        cout << setw(6) << setiosflags(ios::right) << portmap_list->pml_map.pm_port << "/";
        // This one below is changed too since I don't like the way the original function
        // abuses it like: "if (head->pml_map.pm_prot == IPPROTO_UDP)" blah blah ...
        // it should've called to getprotobyname instead
        cout << (getprotobynumber (portmap_list->pml_map.pm_prot))->p_name;
        cout << endl;
      }
      cout << endl;
      cout.unsetf(ios::right);
      return_result = true;
  }
  endprotoent();
  return return_result;
}

bool scan_HTTP(int sock_descriptor)
// Web server
{
   string *s;
   int is_server;

   sock_send_buffer(sock_descriptor,"HEAD / HTTP/0.9\n\n");
   s = sock_get_buffer(sock_descriptor);
   is_server = s->find("Server:");
   while (is_server==-1)
   {
      delete(s);
      s = sock_get_buffer(sock_descriptor);
      is_server = s->find("Server:");
   }
   cout << *s << endl;
   delete(s);
}

bool scan_FTP(int sock_descriptor)
// Scan anonymous FTP - comment from Prana: -> should've checked more
{
   string *s;
   
   s = sock_get_buffer(sock_descriptor);
   if (s->find("220") != -1)
   {
      cout << *s;
      delete(s);
      sock_send_buffer(sock_descriptor,"USER anonymous\n");
      s = sock_get_buffer(sock_descriptor);
      if (s->find("331") != -1)
      {
         delete(s); // this one
         sock_send_buffer(sock_descriptor,"PASS secure@localhost\n");// in case RFC 822 is enforced
         s = sock_get_buffer(sock_descriptor);
         if (s->find("230") != -1)
            cout << "Anonymous access is allowed" << endl << endl;
         delete (s); // again
      }
//   delete (s); 
   }
//   delete (s);
}

bool scan_CUPS(int sock_descriptor)
// CUPS is a new unix printing system daemon - very good one though
{
   string *s;
   int is_server;
   
   sock_send_buffer(sock_descriptor,"GET / HTTP/0.9\nReferer: http://localhost\n\n");
   s = sock_get_buffer(sock_descriptor);
   is_server = s->find("Server:");
   while (is_server==-1)
   {
      delete(s);
      s = sock_get_buffer(sock_descriptor);
      is_server = s->find("Server:");
   }
   cout << *s << endl;
   delete(s);
}
// EOF

