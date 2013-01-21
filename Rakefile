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

# SYSTEM -----------------------------------------------------------------------
namespace :system do 
  task :requirements do 
    throw "TODO"
  end
end

# RVM --------------------------------------------------------------------------
# FlowBox relies on ruby version manager (https://rvm.io)
namespace :rvm do
  desc 'rvm requirements'
  task :requirements do 
    req_a = ['bash', 'awk', 'sed', 'grep']
    req_a << ['which', 'ls', 'cp', 'tar']
    req_a << ['curl', 'gunzip', 'bunzip2']
    req_a << ['git', 'subversion']
    req_a.each do |req|
      req_p = `which #{req}`.strip()
      throw 'rvm requires #{req}. Install it first (see https://rvm.io)' if req_p == nil
    end
  end
  desc 'install rvm'
  task :installation => [:requirements] do 
    # 1) check rvm is not already installed
    rvm_p = `which rvm`.strip()
    if !(rvm_p =~ /home*rvm/)
      throw 'rvm is already installed'
    end
    # 2) check we are NOT root (user-land installation)
    throw 'rvm should be installed as non-root users' if Process.uid == 0
    # 3) fetch and install rvm
    `\curl -L https://get.rvm.io | bash -s stable --ruby`
  end
  desc 'build the required ruby'
  task :ruby do 
    puts `rvm install $FLOWBOX_RUBY_CURRENT`
  end
  desc 'create required gemset'
  task :gemset do 
    puts `rvm use $FLOWBOX_RUBY_CURRENT`
    puts `rvm gemset create $FLOWBOX_GEMSET_CURRENT`
  end
end

# FlowBox C++ ------------------------------------------------------------------
namespace :libflowbox do
  libflowbox_pr = "libflowbox"
  libflowbox_lib_pr = "#{libflowbox_pr}/lib"

  directory libflowbox_lib_pr
  desc 'clone libflowbox from github (over ssh)'
  task :clone do 
    puts "fetch the source from github"
    system "git clone git@github.com:FlowBox/libflowbox.git"
  end
  desc 'prepare the Makefile using cmake'
  task :cmake => [libflowbox_lib_pr] do 
    puts "call cmake"
    Dir.chdir(libflowbox_lib_pr) do
      puts `cmake -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug ../`
    end
  end
  desc 'build the libflowbox'
  task :build => [libflowbox_lib_pr] do
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
    puts "fetch the source from github"
    `git clone git@github.com:FlowBox/flowbox-ruby.git`
  end
  desc 'get dependencies (bundler)'
  task :bundle do 
    Dir.chdir(flowboxruby_pr) do
      `bundle install`
    end
  end
  task :clean do
    Dir.chdir(flowboxruby_pr) do
      `rake clean`
    end
  end
  task :clobber do
    Dir.chdir(flowboxruby_pr) do
      `rake clobber`
    end
  end
  desc 'build flowbox-ruby'
  task :build do 
    Dir.chdir(flowboxruby_pr) do
      `rake`
    end
  end
  desc 'git status'
  task :git_status do
    git_status(flowboxruby_pr)
  end
end 
# FlowBox Rails ----------------------------------------------------------------
namespace :flowboxrails do
  desc 'clone flowbox-rails from github (over ssh)'
  task :clone do 
    puts "fetch the source from github"
    system "git clone git@github.com:FlowBox/flowbox-rails.git"
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
desc 'clean FlowBox'
task :clean => ['libflowbox:clean','flowboxruby:clean'] do 
end
desc 'build FlowBox'
task :build => ['libflowbox:build', 'flowboxruby:build'] do 
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
