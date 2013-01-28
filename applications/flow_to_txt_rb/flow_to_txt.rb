#!/usr/bin/env ruby
################################################################################
# This is a simple flow to text example                                        #
################################################################################
# 
# Just an example to show who you can build your own application using the 
# the LOCAL FlowBox installation using the Ruby Wrapper.
#
# LOCAL: Does not assume that FlowBox is installed on the host. 
#        We have to specifiy the location, where we find the
#        C++ Lib and the Ruby Wrapper Lib
#
# 
# Copyright (c) 2011-13, Dominik Schatzmann

# FETCH BASH VARIABLES #########################################################
['FLOWBOX_CC', 'FLOWBOX_RUBY', 'FLOWBOX_RAILS'].each do |env|
  if File.exists?(ENV[env]) == false
    puts "Please set the '#{env}' bash variable or overwrite it manually"
    exit(-1)
  end
end
$LOAD_PATH << "#{ENV['FLOWBOX_CC']}/lib"
$LOAD_PATH << "#{ENV['FLOWBOX_RUBY']}/lib"

require 'flowbox'
require 'yaml'

# To optimize memory allocation, we use fixed set of flow containers
pool = FlowBox::Core::FlowContainerPool.new()
pool.set_capacity(1)

## User INPUT ##################################################################
if (ARGV.size != 2 or 
    File.exists?(ARGV[0]) == false
)
  puts "use ./this_script /path/to/network.yaml /path/to/folder/with/flows"
  puts "flows -- folder containing the csg flow data"
  puts "flow exporters -- a string containing the flow exporters "
  puts " 'ip-domain-netflowversion,ip-domain-netflowversion, ...'"
  exit(-1)
end

netflow_folder = ARGV[0]
exporter_string = ARGV[1].strip()

# a buffer to interconnect my dummy module with the 'flow parser unit'
buffer_reader_me = FlowBox::Core::FlowContainerBuffer.new()

# PARSER #######################################################################
# A unit that reads flows from compressed files
#-------------------------------------------------------------------------------
# If you don't know the exporters of your network -- you can use the 
# 'extract_flow_exporter' (<git_flowbox>/applications/ethz-extract_flowexporter)
# to extract the required info

# feed the parser with inforamtion about the flow exporters that should be 
# processe. Note that flows from other exporters are ignored.
i = 1
flow_exporters_a = []
exporter_string.split(',').each do |exporter_s|
  exporter_a = exporter_s.strip().split('-')
  throw "invalid exporter string '#{exporter_s}'"if exporter_a.size() != 3
  exporter_h = {}
  exporter_h[:id] = i
  exporter_h[:address] = exporter_a[0]
  exporter_h[:observation_domain] = exporter_a[1].to_i
  exporter_h[:format] = exporter_a[2].to_i
  flow_exporters_a << exporter_h
  i += 1
end

parser = FlowBox::Driver::Console::Parser.new()
parser.configure(

  # what kind of flow data do you like to read? Currently we support only 'netflow'
  :format => 'netflow',
  # the interval in sec between statistic reports are genereated
  :stat_interval_s => 10, 
  # outage detection:  max accepted interval between two flows
  :export_time_window_s => 300, 

  # what kind of input source should we use to read the flows
  # such as Socket, File ...
  # currently only the option 'csgfile' is supported
  :source => 'csgfile',

  # the path to the flow files 
  :csgfile_input => netflow_folder, 
  # how often should we poll the folder for new files (-1 == never)
  # this option can be used to convert the measurement system into an online-system
  :csgfile_scan_interval_s => -1,
  # should files be deleted after processing
  :csgfile_remove_files_after => false,
  
  # the next element?
  # output buffer (use "NULL" to indicate the end of the processing chain)
  :output => buffer_reader_me,

  # flow exporters
  :flow_exporters => flow_exporters_a
)
parser.start()

# FLOWBLOCK ####################################################################
# Encapsulate your code for rapid prototyping
#-------------------------------------------------------------------------------
perflow = FlowBox::Core::Flowblock.new()
perflow.configure(

  # :input => Specifiy the input buffer
  #  => FlowBox::Core::FlowContainerBuffer
  :input => buffer_reader_me,

  # :output => Specifiy the output buffer
  #  => FlowBox::Core::FlowContainerBuffer
  #  => 'NULL' End of Chain
  :output => 'NULL'
)
#
flows = 0
packets = 0
bytes = 0
t1 = Thread.new do 
  sleep(2)
  perflow.each do |flow|
    if flow.class == FlowBox::Core::Flow
      if flow.valid == false
        next
      else
        flows += 1
        packets += flow.packets()
        bytes += flow.bytes()
        puts flow
      end
    else
      throw "What the hell should I do with this object: #{flow.class}"
    end
  end
end

## Wait on 
start_s = Time.now.to_i
while parser.finished() == false or t1.alive?  
  sleep(1)
  now = Time.now.to_i
  puts "Main, #{Time.now.to_i}, #{flows}, #{flows/(now - start_s)}"
  STDOUT.flush()
end
