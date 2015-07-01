RCon GUI
========

This is a graphical front end to the darkplaces remote console protocol.


License
-------

GPLv3+ see COPYING


Getting the latest sources
--------------------------

A web frontend to the repository is available at https://github.com/mbasaglia/RconGui
The git URL is https://github.com/mbasaglia/RconGui.git

Since this project uses git submodules,
you need to make sure you clone them as well the main repository.

Eg:
    git clone --recursive https://github.com/mbasaglia/RconGui.git

Or
    git clone https://github.com/mbasaglia/RconGui.git
    cd RconGui
    git submodule init
    git submodule update

Dependencies
------------

* CMake
* Any C++11 compliant compiler
* Boost Asio
* Qt 5

Compiling
---------

This project uses the standard CMake build conventions:

    mkdir build && cd build && cmake .. && make


Contacts
--------

Mattia Basaglia <mattia.basaglia@gmail.com>
