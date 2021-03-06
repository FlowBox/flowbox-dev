# Welcome to the FlowBox Project

We provide several scripts and config files to simplify the development 
of new FlowBox applications or to improve the core libraries of FlowBox.
All those files are maintained in the 'flowbox-dev' repository. 

Interested to start coding with FlowBox? 

If yes, then jump to the section called 'Installation Guide'. This howto 
helps you to prepare you system to code with FlowBox. In Section 
'My Fist Application' we discuss how you can build your own FlowBox
Application using the FlowBox libs.


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

    $ sudo apt-get install git git-flow

As a next step you should get familiar with the standard commands 
of 'git' and 'git flow'. The following readers are recommended:

* http://git-scm.com/documentation
* http://git-scm.com/book
* http://nvie.com/posts/a-successful-git-branching-model/

Often multiple persons commit code to a projects. To distinguish
the different commiters, git should be configured with meaningful
contact information: 

    $ git config --global user.name "Your Name"
    $ git config --global user.email yourmail@thisdomain.com

Now its time to checkout the helper scripts, change into the folder, and
ensure that git uses rebase by default.

    $ git clone https://github.com/FlowBox/flowbox-dev.git
    $ cd flowbox-dev
    $ git checkout -b develop origin/develop
    $ git config branch.autosetuprebase always

After that, we have to configure 'git-flow' (accept all default proposals) and switch back to master

    $ git flow init
    $ git checkout master

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
folder. Now you are asked to turn on rvm for this project folder. Accept it. 

Install the proposed ruby version by first install the required dependencies

    $ rvm requirements

Most likely you have to install the following packages (Ubuntu 12.10)

    $ sudo apt-get install build-essential openssl libreadline6 libreadline6-dev curl git-core zlib1g zlib1g-dev libssl-dev libyaml-dev libsqlite3-dev sqlite3 libxml2-dev libxslt-dev autoconf libc6-dev ncurses-dev automake libtool bison subversion pkg-config

Now install the propose ruby and prepare the gemset. 
ATTENTION: Replace the X.X.X with the proposed version.
Such as 

    $ rvm install ruby-X.X.X
    $ rvm use ruby-X.X.X
    $ rvm gemset create flowbox
    $ gem install rake

### The Rake File
Now we are ready to use Rake to build the core libs of FlowBox. 

    $ rake clone
    $ rake dependencies
    $ rake build



## My Fist Application
Probably you have recognized, that 'flowbox-dev' provides a folder called 
'application'. Thats exactly the place to seed our application.




