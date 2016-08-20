SubNetwork Explorer v0.0.3BETA1-test1-prerelease
--------------------------------------------------------------------------------
Copyright (C) 2000 Prana 
License: GPLv2

** Note: This tiny utility is now archived and no longer developed. Uploaded to
Github on Aug 20, 2016. 

Description
================================================================================
"SubNetwork Explorer" is a program that scans the subnets of a network
for anonymous Ftp, Netbios, and SunRPC. If no options specified, it will just
display the hostnames without scanning them. Uses 'fork()' to quickly scan the 
subnets all at the same time. This program will check for ftp and tries to login
to it anonymously.

For example:

$ nslookup localhost
Server:  localhost
Address:  127.0.0.1

Non-authoritative answer:
Name:    localhost
Address:  127.0.0.1

$ netxplorer 127.0.*.*
master.localhost
host1.localhost
host2.localhost
host3.localhost

Requirements
================================================================================
* Linux, FreeBSD, Solaris, or any other UN*X
* G++ (GNU C++) compiler
* glibc-devel


Installation
================================================================================

* Just type "make".
* If you feel that the executable size is too big (300k), just strip the
  debug information by typing "strip netxplorer"
* If you want to put it in /usr/bin, type "make install" 
  (you have to login as root) for "make install"


Usage
================================================================================
netxplorer [options] [host range]
 OPTIONS:
 -? : show this help screen
 -c : perform a connect() port scanning
 -f : perform a half connection scan - undetected by Odin's iplog
 -a : scan all port from 1-65535
 -p : only scan privileged port from 1-1024
 -n : no scan - only query hostnames (DEFAULT)
 -s : scan specific port only:
      -s cups	 : scan CUPS printing server
      -s ftp	 : scan ftp server
      -s http 	 : scan http server
      -s mail 	 : scan mail server
      -s lpd	 : scan lpd printing server
      -s netbios : scan netbios
      -s sunrpc  : scan sunrpc
      -s ssh 	 : scan ssh
 -v : try to retrieve versions/informations from port
 [host range] : the network you want to scan

 EXAMPLE:
 netxplorer 207.168.230.* will display the hostnames from 207.168.230.0-255 *WITHOUT* scanning
 netxplorer localhost -s http -s cups will scan 'localhost' for http and CUPS server
 netxplorer -c -a 192.168.1.* will scan from 192.168.1.0-255 and perform connect() scan from port 1-65535

 NOTE: Host *MUST* be in number format (eg: 127.0.*.*) if you want to do multiple host
 scanning with the '*' asterisk .
 This program will then do a forking for each hosts. Without a parameter,
 the result will only display the host names

FAQ
================================================================================
Q: This program is so buggy, I don't like it!!
A: Please note that it's still a BETA version, so it *IS* buggy.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Q: Why isn't it capable of TCP SYN Scan like nmap?
A: Later in v0.0.3BETA
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Q: Why does it only scans for ftp, netbios, and sunrpc, cups, etc?
A: I will add more for feature release, including Proxy & Wingates...
   The code is available, but I want to make more testing
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Q: Do you have the RPM package of the program?
A: Currently no. I will pack it in RPM after it's not that buggy.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
