// connect_scanner.h
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
#ifndef CONNECT_SCANNER_H
#define CONNECT_SCANNER_H
 
#include "scanner.h"
#include <string>
#include <netinet/in.h>
  
class Connect_Scanner : public Scanner
// TCP full connect - easy stuff first
{
   public:
      Connect_Scanner();
      Connect_Scanner(string *name);
      ~Connect_Scanner();
      bool set_arguments(const string &arg);
      virtual bool scan_port(int portnumber);
   private:
      // this is only for the specific stuff...
      scanner_port_modules modules_for_port[0xFFFF];
      
};

bool scan_regular	(int sock_descriptor);
bool scan_NetBios	(int sock_descriptor);
bool scan_FTP	 	(int sock_descriptor);
bool scan_HTTP   	(int sock_descriptor);
bool scan_SunRPC 	(int sock_descriptor);
bool scan_NetBios	(int sock_descriptor);
bool scan_CUPS		(int sock_descriptor);
#endif
