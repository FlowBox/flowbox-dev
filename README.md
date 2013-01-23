# Welcome to the FlowBox Project

To simplify the development of new FlowBox applications or to improve
the core libraries of FlowBox we provide several scripts and configuration 
files. You can find those files in the 'flowbox-dev' repository. 

Ready to start? 

If yes, then jump to the next section called 'Installation Guide' to 
prepare your system and follow the instructions of the section 
'Fist Application' to build your first application using FlowBox.

Happy coding and enjoy life!

Best, 
Dominik

http://daringfireball.net/projects/markdown/basics

## Installation Guide

The installation guide helps you to setup the basic programs required 
to effectively develop new FlowBox component or to extend the functionality
of the core libaries of FlowBox. As first step, we will install a version 
controlling system called 'git' and checkout the basic FlowBox code 'flowbox-dev'. 
Then we install the Ruby Version Manager (rvm) and install the ruby 
interpreter.

This installation guide is tested for:

* Ubuntu 12.04
* Ubuntu 12.10

### Version Controlling System: GIT
As a first step we have to install git and git-flow. Open a console (CTRL-L) 
and install the following packets using apt-get.

  sudo apt-get install git git-flow

As a next step you should get familiar with the standard commands 
of 'git' and 'git flow'. The following readers are recommended:

* http://git-scm.com/documentation
* http://git-scm.com/book
* http://nvie.com/posts/a-successful-git-branching-model/

Now its time to checkout the helper scripts and change into the top folder.

    $ git clone git://github.com/FlowBox/flowbox-dev.git
    $ git clone https://github.com/FlowBox/flowbox-dev.git
    $ cd flowbox-dev
    $ git checkout -b develop origin/develop

Since multiple persons will commit code to the project, we should 
configure git with the right personal contact data and rebase settings:

    $ git config user.name "Your Name"
    $ git config user.email yourmail@thisdomain.com
    $ git config branch.autosetuprebase always

Lest init the git-flow (accept all default proposals) and switch back to master
    $ git flow init
    $ git checkout master

Now we are ready for the next step.

### Ruby Version Manager: RVM
The FlowBox Project is based on Ruby. This allows us to assure that all users 
have a similar Ruby setup -- simplify troubleshooting. A good Howto about 
installing rvm can be found on the [rvm project](http://https://rvm.io)
side. Typically we use a rvm user installation. 

This includes the following steps:

    $ sudo apt-get install curl subversion
    $ \curl -L https://get.rvm.io | bash -s stable --ruby 

Now we have to fix some gnome-terminal problems related to Ubuntu. Just follow 
the instructions of [rvm integration gnome terminal](https://rvm.io/integration/gnome-terminal/).
Close the genome-terminal and open a new one. Switch back into the flobox-dev
folder. Now you are asked to tun on rvm on this folder. Accept. 

Install the proposed ruby version by first install the required dependencies
    $ rvm requirements

Most likely you have to install the following packages (Ubuntu 12.10)
    $ sudo apt-get install build-essential openssl libreadline6 libreadline6-dev curl git-core zlib1g zlib1g-dev libssl-dev libyaml-dev libsqlite3-dev sqlite3 libxml2-dev libxslt-dev autoconf libc6-dev ncurses-dev automake libtool bison subversion pkg-config

Now install the propose ruby and prepare the gemset
    $ rvm install ruby-1.9.3-p286
    $ rvm current ruby-1.9.3-p286
    $ rvm gemset create flowbox
    $ gem install rake

### The Rake File
Now we are ready to use Rake to build the core libs of FlowBox. 
    $ rake dependencies
    $ rake build





