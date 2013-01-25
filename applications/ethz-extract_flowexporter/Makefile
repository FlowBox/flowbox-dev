# Copyright 2011-13 Dominik Schatzmann (schadomi@gmail.ch)
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
FlowBoxLib = -L$(TOP)/../../libflowbox/lib -lflowbox

# Compiler
CC = g++
# Compiler flages
CFLAGS  = -Wall -g3 $(FlowBoxHeader)
# Linker flags
LDFLAGS = $(FlowBoxLib)

all: main.exe

clean:
	 rm extract_flow_exporter.o main.o main

main.exe: main.o extract_flow_exporter.o
	 $(CC) -g -o main main.o  extract_flow_exporter.o $(LDFLAGS) 

main.o: main.cc
	 $(CC) $(CFLAGS) -c -g main.cc

extract_flow_exporter.o: extract_flow_exporter.cc
	 $(CC) $(CFLAGS) -c -g extract_flow_exporter.cc 
