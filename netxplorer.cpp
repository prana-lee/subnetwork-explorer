// netxplorer.cpp
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
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "scanner.h"
#include "connect_scanner.h"
#include "half_scanner.h"
using namespace std;

int host_array[4] = {0,0,0,1};

const int CONNECT_SCANNER 	= 0;
const int HALF_CONNECT_SCANNER	= 1;

int scanner_type = CONNECT_SCANNER;
string *format_arguments;

// watch out
char *host_format;

 char logo[] = 
"\n==============================================================================\n\
SubNetwork Explorer v0.0.3BETA1-test1-prerelease\n\
Copyright (C) 2000 by Prana <prana@cyest.org>\n\
WARNING: This is still a BETA software and still in very early development stage\n\
\t If this program still crashes the computer because of the fork(),\n\
\t please file a bug report\n\n\
Please visit  for updates\n\
==============================================================================\n\n";
// the code below should be cleaned
 char the_usage[]= 
 "%s [options] [host range]\n\
 OPTIONS:\n\
 -? : show this help screen\n\
 -c : perform a connect() port scanning\n\
 -f : perform a half connection scan\n\
 -a : scan all port from 1-65535\n\
 -p : only scan privileged port from 1-1024\n\
 -n : no scan - only query hostnames (DEFAULT)\n\
 -s : scan specific port only:\n\
      -s cups	 : scan CUPS printing server\n\
      -s ftp	 : scan ftp server\n\
      -s http 	 : scan http server\n\
      -s mail 	 : scan mail server\n\
      -s lpd	 : scan lpd printing server\n\
      -s netbios : scan netbios\n\
      -s sunrpc  : scan sunrpc\n\
      -s ssh 	 : scan ssh\n\
 -v : try to retrieve versions/informations from port\n\
 [host range] : the network you want to scan\n\n\
 EXAMPLE:\n\
 %s 207.168.230.* will display the hostnames from 207.168.230.0-255 *WITHOUT* scanning\n\
 %s localhost -s http -s cups will scan 'localhost' for http and CUPS server\n\
 %s -c -a 192.168.1.* will scan from 192.168.1.0-255 and perform connect() scan from port 1-65535\n\n\
 NOTE: Host *MUST* be in number format (eg: 127.0.*.*) if you want to do multiple host\n\
 scanning with the '*' asterisk .\n\
 This program will then do a forking for each hosts. Without a parameter,\n\
 the result will only display the host names\n";

////////////////////////////////////////////////////////////////////////////////////////
void usage(char *progname) {
/*
   Display the usage
*/
 fprintf(stdout,the_usage,progname,progname,progname, progname);
 exit(1);
}

////////////////////////////////////////////////////////////////////////////////////////
void handle_single_host(string *phost)
/*
   Just for a single connection
   THIS is _NOT_ the focus
*/
{
   Scanner *connecter;
   bool connected;

   if (scanner_type == HALF_CONNECT_SCANNER)
      connecter = new Half_Connect_Scanner();
   else
      connecter = new Connect_Scanner();
   connecter->set_arguments(format_arguments);
   connected = connecter->set_hostname(phost->c_str());      
   if (connected)
   {
      cout << "Scanning " << connecter->get_hostname() << endl;
      connecter->scan();
   }
}

////////////////////////////////////////////////////////////////////////////////////////
void rotate_host_ip(Scanner *tool, const string &basic, int i, int n, int max)
// rotate it here
// fork, etc goes here...
{
   int x, nextproc, status;
   string temp = "";
   
   i == max-1 ? x = 1 : x = 0;
   if (i < max) // only run if it's less than.
     for (host_array[i] = x; host_array[i] < n; host_array[i]++)
       { 
       if (i < n)       
          rotate_host_ip(tool, basic,i+1,n, max); // recursive here
       temp = basic;
       if (host_array[i+1] != n)
       {
       for (int index = 0; index < max; index++)
          {
          // the code below must be changed to snprintf(...) for next release
             if (index==max-1)
                sprintf(host_format,"%d",host_array[index]);
             else
                sprintf(host_format,"%d.",host_array[index]);
   	     temp = temp + host_format;
          } // end for
          // unix fork () -- maybe changed later to posix thread instead .. dunno         
          if (tool->set_hostname(temp.c_str()))
          {
             if (nextproc = fork())
             {
                cout << "Scanning " << tool->get_hostname() << endl;
                tool->scan();
                waitpid(nextproc, &status, WUNTRACED);
                exit(0);
             } // end if nextproc
//             else perror("fork error");
          } // end if set host name doesn't fail
       } // end if host_array[i+1] != n
   } // end for host_array
}

////////////////////////////////////////////////////////////////////////////////////////
void handle_multiple_hosts(const string &the_host)
/*
   Multiple host xxx.yyy.zzz.* or xxx.*.*.*
*/
{
   char *tempchar[4];  
   int dotcount = 0, dotpos = 0, pos = 0 ,rangedot = 0;
   Scanner *connecter;
   string *basic_host = new string;
   
   // Parse first;   
   while ( (the_host [pos] != '*') && (pos < the_host.length()) )
   {
      if ( (dotpos = the_host.find(".",pos)) != -1 )
      {  
         rangedot = dotpos-pos;
         *basic_host += the_host.substr(pos,rangedot) + ".";
         if ( rangedot && (rangedot <= 3) )
         {
            dotcount++;
            pos = dotpos+1;
         } // end if dotpos
         else
         {
            cerr << "Invalid hostname: \"" << the_host
                 << "\" - Unable to parse :-(" << endl;
            exit(1);
         } // end else dotpos
      } // end dotpos = the_host->find
   } // end while

   cout << "Scanning multiple hosts" << endl;
   connecter = new Connect_Scanner();
   connecter->set_arguments(format_arguments);   
   rotate_host_ip(connecter, *basic_host,0x00,0xFF,0x04-dotcount);
   delete connecter;
   delete basic_host;
}

////////////////////////////////////////////////////////////////////////////////////////
void handle_arguments(int argc, char *argv[])
// parameters
{
#if !defined(__linux__)
	extern char *optarg;
	extern int optind;
#endif

   int ch;
   string *hostname = new string;
   
   if (argc <= 1) usage(argv[0]);
   while ((ch = getopt(argc, argv, "cafpnvs:h?")) != -1)
   {// getopt_long
      switch (ch)
      {
         case 'a':// scan all port
            *format_arguments+="-all";
            break;
            
      	 case 'c':// connect
            break;
            
         case 'f':// half
            scanner_type = HALF_CONNECT_SCANNER;
            break;
              
         case 'p':// privileged port only
            *format_arguments+="-1024";
            break;
         
         case 'n':// default - no scanning -- must be cleaned up
            *format_arguments+="-noscan";
            break;
         
         case 's':// scan options
            if (optarg)
            {
               *format_arguments+="+";
               *format_arguments+=optarg;
            }
            break;
         case 'v':// verbose
            *format_arguments+="-verbose";
            break;
         case 'h':// help
         case '?':// usage
         default:usage(argv[0]);
      } // end switch
   } // end while
   *hostname = argv[optind++];
   if (hostname->find("*") != -1)
      handle_multiple_hosts(*hostname);
   else
      handle_single_host(hostname);
   delete (hostname);
}

////////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
// Main
{
   cerr << logo;
   
   format_arguments = new string();
   host_format = new char[4]; // yeah 
   handle_arguments(argc,argv);
   delete []host_format;
   delete format_arguments;
   return 0;
}
