#encoding: utf-8
###############################################################################
# FLOWBOX
################################################################################
#
# Here we define some common task that will simplify your job
#
# http://jasonseifer.com/2010/04/06/rake-tutorial
# http://en.wikipedia.org/wiki/Rake_%28software%29
# http://docs.rubyrake.org/user_guide/chapter03.html
# http://ruby.about.com/od/gosugme/ss/Embedding-Images-In-Gosu-Programs_3.htm
#

namespace :rvm do
  desc 'build required ruby'
  task :build do 
    puts `rvm install $FLOWBOX_RUBY_CURRENT`
  end
  desc 'create required gemset'
  task :build do 
    puts `rvm gemset $FLOWBOX_GEMSET_CURRENT`
  end

end

namespace :libflowbox do

  directory "libflowbox/lib"

  desc 'clone libflowbox from github (over ssh)'
  task :clone do 
    puts "fetch the source from github"
    system "git clone git@github.com:FlowBox/libflowbox.git"
  end

  desc 'prepare the Makefile using cmake'
  task :cmake => ["libflowbox/lib"] do 
    puts "call cmake"
    puts `cd libflowbox/lib && cmake -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug ../`
  end

  desc 'build the libflowbox'
  task :build => ["libflowbox/lib", :clean, :cmake] do
    puts "build libflowbox"
    sh 'cd libflowbox/lib; make all'
  end

  task :clean do
    puts "clean up libflowbox -- call make clean"
    if File.exists?("libflowbox/lib/Makefile") == true
      sh 'cd libflowbox/lib; make clean'
    end
  end
  task :clobber do
    puts "clobber libflowbox -- remove all cmake stuff"
    if File.("libflowbox/lib").exists?()
    end
  end
end

namespace :flowboxruby do

  desc 'clone flowbox-ruby from github (over ssh)'
  task :clone do 
    puts "fetch the source from github"
    system "git clone git@github.com:FlowBox/flowbox-ruby.git"
  end

  desc 'get dependencies (bundler)'
  task :bundle do 
    puts 'install dependencies using bundler'
    puts `cd flowbox-ruby && bundle install`
  end

  desc 'build flowbox-ruby'
  task :build do 
    puts 'build flowbox-ruby'
    system "cd flowbox-ruby && rake clean && rake clobber && rake"
  end
end 
namespace :applications do 

end


# We will use this method to add the rake files of the different applications
# Dir.glob(File.join(File.dirname(__FILE__), '**', 'Rakefile')).each do |tasks|
#  load tasks
# end
