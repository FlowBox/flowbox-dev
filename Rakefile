#encoding: utf-8
################################################################################
# FlowBox
################################################################################
#
# Here we define some common task that will simplify your job
#
# Author: Dominik Schatzmann

# HELPER -----------------------------------------------------------------------
def git_status(path_r)
  Dir.chdir(path_r) do
    fill = 80 - path_r.size()
    fill = 0 if fill < 0
    puts path_r + '-'*fill
    puts `git status`
  end
end

# FlowBox C++ ------------------------------------------------------------------
namespace :libflowbox do
  libflowbox_pr = "libflowbox"
  libflowbox_lib_pr = "#{libflowbox_pr}/lib"
  directory libflowbox_lib_pr
  desc 'clone libflowbox from github'
  task :clone do
    puts `git clone https://github.com/FlowBox/libflowbox.git`
    Dir.chdir(libflowbox_pr) do
      puts `git config branch.autosetuprebase always`
      puts `git checkout -b develop origin/develop`
      puts `git flow init -d`
      puts `git checkout master`
    end
  end
  desc 'install required dev libs'
  task :dependencies do
    #raise 'Must run as root: sudo rake this-task' unless Process.uid == 0
    system('sudo apt-get install cmake liblz-dev libbz2-dev')
  end
  desc 'prepare the Makefile using cmake'
  task :cmake => [libflowbox_lib_pr] do 
    puts "call cmake"
    Dir.chdir(libflowbox_lib_pr) do
      puts `cmake -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug ../`
    end
  end
  desc 'build the libflowbox'
  task :build => [libflowbox_lib_pr, :cmake] do
    Dir.chdir(libflowbox_lib_pr) do
      puts `make all`
    end
  end
  task :clean do
    if File.exists?(libflowbox_lib_pr+'Makefile')
      Dir.chdir(libflowbox_lib_pr) do
       puts `make clean`
      end
    end
  end
  task :clobber do
    if File.exists?(libflowbox_lib_pr+'Makefile') 
      throw "TODO"
    end
  end
  desc 'git status'
  task :git_status do
    git_status(libflowbox_pr)
  end
end

# FlowBox Ruby -----------------------------------------------------------------
namespace :flowboxruby do
  flowboxruby_pr = 'flowbox-ruby'
  desc 'clone flowbox-ruby from github (over ssh)'
  task :clone do
    puts `git clone https://github.com/FlowBox/flowbox-ruby.git`
    Dir.chdir(flowboxruby_pr) do
      puts `git config branch.autosetuprebase always`
      puts `git checkout -b develop origin/develop`
      puts `git flow init -d`
      puts `git checkout master`
    end
  end
  desc 'install required gem packets'
  task :dependencies do 
    Dir.chdir(flowboxruby_pr) do
      puts `bundle install`
    end
  end
  task :clean do
    Dir.chdir(flowboxruby_pr) do
      puts `rake clean`
    end
  end
  task :clobber do
    Dir.chdir(flowboxruby_pr) do
      puts `rake clobber`
    end
  end
  desc 'build flowbox-ruby'
  task :build do 
    Dir.chdir(flowboxruby_pr) do
      puts `rake`
    end
  end
  desc 'git status'
  task :git_status do
    git_status(flowboxruby_pr)
  end
end 
# FlowBox Rails ----------------------------------------------------------------
namespace :flowboxrails do
  flowboxrails_pr = 'flowbox-rails'
  desc 'clone flowbox-rails from github'
  task :clone do
    puts `git clone https://github.com/FlowBox/flowbox-rails.git`
    Dir.chdir(flowboxrails_pr) do
      puts `git config branch.autosetuprebase always`
      puts `git checkout -b develop origin/develop`
      puts `git flow init -d`
      puts `git checkout master`
    end
  end
  desc 'git status'
  task :git_status do
    Dir.chdir("flowbox-ruby") do
      puts "-- FlowBox Rails --"+'-'*61
      puts `git status`
    end
  end
end
# Applications  ----------------------------------------------------------------
namespace :applications do
# We will use this method to add the rake files of the different applications
  Dir[File.join(File.dirname(__FILE__), 'applications','*', 'Rakefile')].each do |path| 
    project_name =  File.basename(File.dirname(path)).gsub(/-/,'')
    namespace project_name.to_sym do
      load path
    end
  end
end

# ALL JOBS  --------------------------------------------------------------------
desc 'clone FlowBox'
task :clone => ['libflowbox:clone', 'flowboxruby:clone', 'flowboxrails:clone'] do
end

desc 'clean FlowBox'
task :clean => ['libflowbox:clean','flowboxruby:clean'] do 
end
desc 'build FlowBox'
task :build => ['libflowbox:build', 'flowboxruby:build'] do 
end

desc 'install required dev libs for FlowBox'
task :dependencies => ['libflowbox:dependencies', 'flowboxruby:dependencies'] do 
end

desc 'git status'
task :git_status => ['libflowbox:git_status', 'flowboxruby:git_status', 'flowboxrails:git_status'] do 
  Dir.chdir("flowbox-ruby") do
    puts "-- FlowBox DEV --"+'-'*63
    puts `git status`
  end
end

task :default => [:build] do
end

