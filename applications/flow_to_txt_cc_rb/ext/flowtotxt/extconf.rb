require 'mkmf'
################################################################################
# FlowBox: Make Makefile
################################################################################
# Author: Dominik Schatzmann
# use this file to autogenerate the Makefile to build the c++ extensions

appname = 'flowtotxt'

# WHERE IS THE FLOWBOX C++ LIB ?
flowbox_cc = ENV['FLOWBOX_CC']

if flowbox_cc != nil
    dir_config('flowbox', flowbox_cc+'/include', flowbox_cc+'/lib/')
    unless have_library('flowbox')
      puts "ERROR: We used the Bash Enviroment Variable 'FLOWBOX_CC'"
      puts " -- BUT: have_library('flowbox') didn't worked"
      exit(-1)
    end
else
  unless have_library('flowbox')
    puts "Solution: Add the path to <git_root>/flow_box_cc/ to your bash variables"
    puts "   $> FLOWBOX_CC=<git_root>/flow_box_cc/include"
    puts "   $> export FLOWBOX_CC"
    puts " -- "
    exit(-1)
  end
end

# add standart C++ libs
$LIBS += " -lstdc++ "
$CFLAGS += " -std=gnu++0x "

def error_exit message
  $stderr.puts message
  exit false
end
def error_lib_missing lib, package
  msg  = "ERROR: #{lib} was not found on this system"
  msg += " possible soluation: $> sudo apt-get install #{package}"
  puts msg
  exit
end
# require some standart libs
[
  ['pthread', 'glibc' ],
  ['bz2', 'libbz2-dev' ]
].each do |lib, packet|
  unless have_library(lib)
  error_exit error_lib_missing(lib, packet)
  end
end

# create the extension
create_makefile(appname)

