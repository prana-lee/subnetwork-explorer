#/*****************************************************************************
#/*  Makefile for SubNetwork Explorer                                         *
#/*  Copyright (C) 2000 Prana 
#/*                                                                           *
#/*  This program is free software; you can redistribute it and/or modify     *
#/*  it under the terms of the GNU General Public License as published by     *
#/*  the Free Software Foundation; either version 2 of the License, or        *
#/*  (at your option) any later version.                                      *
#/*                                                                           *
#/*  This program is distributed in the hope that it will be useful,          *
#/*  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
#/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
#/*  GNU General Public License for more details.                             *
#/*                                                                           *
#/*  You should have received a copy of the GNU General Public License        *
#/*  along with this program; if not, write to the Free Software              *
#/*  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111 USA      *
#/*****************************************************************************/

CPPFLAGS= -g -frtti -fwritable-strings -O2 -Werror -I/usr/include
CPP=g++
BIN := /usr/bin
RM=rm -f

# From .o to .cpp
%.o : %.cpp
	${CPP} ${CPPFLAGS} -c $< -o $@

netxplorer: netxplorer.o scanner.o connect_scanner.o half_scanner.o
	$(CPP) $(CPPFLAGS) $^ -o $@

netxplorer.o: netxplorer.cpp scanner.h connect_scanner.h

connect_scanner.o: connect_scanner.cpp scanner.h connect_scanner.h

half_scanner.o: half_scanner.cpp scanner.h half_scanner.h

scanner.o: scanner.cpp scanner.h

clean:
	rm netxplorer *.o *.bak

install : netxplorer
	strip netxplorer
	install $< $(BIN)
