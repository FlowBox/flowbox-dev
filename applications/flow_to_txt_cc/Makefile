# Copyright 2011,2012 Dominik Schatzmann (mail@dominikschatzmann.ch)
#
# This file is part of FlowBox. FlowBox is free software: you can redistribute 
# it and/or modify it under the terms of the GNU Lesser General Public 
# License as published by the Free Software Foundation, either version 3 
# of the License, or (at your option) any later version.
#
# FlowBox is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with FlowBox.  If not, see <http://www.gnu.org/licenses/>.

TOP := $(shell pwd)

FlowBoxHeader = -I$(TOP)/../../libflowbox/include
FlowBoxLib = -L$(TOP)/../../libflowbox/lib -lflowbox -lpthread

# Compiler
CC = g++
# Compiler flages
CFLAGS  = -Wall -Wextra -g3 $(FlowBoxHeader)
# Linker flags
LDFLAGS = $(FlowBoxLib)

all: main_csg_reader.exe

clean:
	 rm main_csg_reader main_csg_reader.o flow_to_txt.o 

main_csg_reader.exe: main_csg_reader.o
	 $(CC) -g -o main_csg_reader main_csg_reader.o flow_to_txt.o $(LDFLAGS) 

main_csg_reader.o: main_csg_reader.cc flow_to_txt.o
	 $(CC) $(CFLAGS) -c -g main_csg_reader.cc

flow_to_txt.o: flow_to_txt.cc
	 $(CC) $(CFLAGS) -c -g flow_to_txt.cc 
