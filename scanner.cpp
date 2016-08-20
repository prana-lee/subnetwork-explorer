// scanner.cpp
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

#include <string>
#include <iostream>
#include <iomanip>
#include <cctype>
#include <cerrno>
#include <unistd.h>
#include <netdb.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/time.h>
#include "scanner.h"

Scanner::Scanner() :
/*
   Constructor
*/
   host_info (NULL)
{
   host_name = new string;
   for (int i=0; i < MAX_SCANNER_CONFIG; i++)
      scanner_config[i] = 0;
   arguments = new string;
}

Scanner::~Scanner()
/*
   Destructor
*/
{
   delete (arguments);
   delete (host_name);
}

bool Scanner::set_hostname(const char *name)
/*
   Set the host name for scanning
*/
{  
   *host_name = name;
   int charcount = 0;
   unsigned long qaddr = inet_addr(name);
   
   for (int i=0; i < strlen(name); i++)
      if (isalpha(name[i])) charcount++;
         
   if (charcount)
      host_info = gethostbyname(host_name->c_str());
   else
      host_info = gethostbyaddr( (char *) &qaddr, 4, AF_INET);
   if (host_info)
      set_config(SCANNER_READY);
   is_fqdn = charcount;
   return (host_info != NULL);
}

string Scanner::get_hostname()
/*
   Get the host name
*/
{
   string the_names = host_info->h_name;
   struct in_addr **p_addresses;
   char **p_aliases;
  
   the_names += " / ";
   ////////////////////////// Aliases ///////////////////////////
   for (p_aliases = host_info->h_aliases;*p_aliases;p_aliases++)
   {
      the_names += *p_aliases;
      the_names += " / ";
   }
   ////////////////////////// Addresses /////////////////////////
   for (p_addresses = (struct in_addr **) host_info->h_addr_list;
      	*p_addresses;p_addresses++)
   {
      the_names += inet_ntoa(**p_addresses);
      if (*(p_addresses+1)) the_names+=", ";
   }
   return the_names;
}

void Scanner::scan()
// scan
{
   int max = 0;
   
   if (get_config(SCANNER_OPT_SCAN_ALL))
      max = 0xFFFF;
   else if (get_config(SCANNER_OPT_SCAN_1024))
      max = 1024;
   
   if (max)
      for (int x = 0; x < max; x++)
         scan_port(x);
   else
   {  // definitely inefficient.. will clean up in v0.0.4BETA
      if (get_config(SCANNER_M_HTTP)) scan_port(80);
      if (get_config(SCANNER_M_FTP)) scan_port(21);
      if (get_config(SCANNER_M_SUNRPC)) scan_port(111);
      if (get_config(SCANNER_M_CUPS)) scan_port(631);
      if (get_config(SCANNER_M_NETBIOS)) scan_port(139);
      if (get_config(SCANNER_M_MAIL)) scan_port(25);
      if (get_config(SCANNER_M_LPD)) scan_port(515);
      if (get_config(SCANNER_M_SSH)) scan_port(22);
   }
}

bool Scanner::set_arguments(string *arg)
/*
   Set the scanning option...
   This needs to be cleaned up
*/
{
   if (arg)
   {
      *arguments=*arg;
      if (arguments->find("+http")!=-1) set_config(SCANNER_M_HTTP);
      if (arguments->find("+ftp")!=-1) set_config(SCANNER_M_FTP);
      if (arguments->find("+sunrpc")!=-1) set_config(SCANNER_M_SUNRPC);
      if (arguments->find("+cups")!=-1) set_config(SCANNER_M_CUPS);
      if (arguments->find("+netbios")!=-1) set_config(SCANNER_M_NETBIOS);
      if (arguments->find("+mail")!=-1) set_config(SCANNER_M_MAIL);
      if (arguments->find("+lpd")!=-1) set_config(SCANNER_M_LPD);
      if (arguments->find("+ssh")!=-1) set_config(SCANNER_M_SSH);
      if (arguments->find("-noscan")!=-1) set_config(SCANNER_OPT_NOSCAN);
      if (arguments->find("-1024")!=-1) set_config(SCANNER_OPT_SCAN_1024);
      if (arguments->find("-all")!=-1) set_config(SCANNER_OPT_SCAN_ALL);
   }
}

void Scanner::set_config(int config_number)
/*
   Set the config
*/
{
   scanner_config[config_number / sizeof(int)] |= (1 << (config_number % sizeof(int)));
}

void Scanner::unset_config(int config_number)
/*
   Unset the config
*/
{
   scanner_config[config_number / sizeof(int)] &= !(1 << (config_number % sizeof(int)));
}

bool Scanner::get_config(int config_number)
/*
   Get the config
*/
{
   return scanner_config[config_number / sizeof(int)] & (1 << (config_number % sizeof(int)));
}


void Scanner::print_result_for_port(int port)
/*
   Print the port
*/
{
   struct servent *service_info;   
   
   cout << "Port:" << setw(5) << port;
   if (service_info=getservbyport(htons(port),"tcp")) // Note -> NOT ==, just =. It's not wrong.
      cout << " " << service_info->s_name;
   cout << endl;
}

bool Scanner::set_time_out(int descriptor, int sec, int usec)
{
   struct timeval timeout;
   fd_set fread;

   fcntl(descriptor,F_SETFL,O_ASYNC);   
   FD_ZERO(&fread);
   FD_SET(descriptor,&fread);
   timeout.tv_sec = sec;
   timeout.tv_usec = usec;
   return (select (descriptor+1, &fread, NULL, NULL, &timeout) != -1);
}
