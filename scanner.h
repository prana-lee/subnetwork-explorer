// scanner.h
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
#ifndef SCANNER_H
#define SCANNER_H
 
#include <string>

#define MAX_SCANNER_CONFIG		16

#define SCANNER_READY			1
#define SCANNER_OPT_SCAN_ALL		2
#define SCANNER_OPT_SCAN_1024		3
#define SCANNER_OPT_NOSCAN		4

// modules
#define SCANNER_M_HTTP			5
#define SCANNER_M_FTP			6
#define SCANNER_M_SUNRPC		7
#define SCANNER_M_CUPS			8
#define SCANNER_M_NETBIOS		9
#define SCANNER_M_MAIL			10
#define SCANNER_M_LPD			11
#define SCANNER_M_SSH			12
typedef bool (*scanner_port_modules) (int);

class Scanner
{
   public:
      Scanner();
      ~Scanner();
      virtual bool set_hostname(const char *name);
      string get_hostname();
      bool set_arguments(string *args);
      bool ping();
      virtual bool scan_port(int port) = 0;
      void scan();
      bool set_time_out(int descriptor, int sec, int usec);
      bool get_config(int config_number);
      void set_config(int config_number);
      void unset_config(int config_number);
      void print_result_for_port(int port_number);
      
   protected:
      bool is_fqdn; // fully qualified domain name
      struct hostent *host_info;
      string *host_name;
      string *arguments;
      int scanner_config[MAX_SCANNER_CONFIG];
};

#endif
