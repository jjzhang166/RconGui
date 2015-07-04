RCon GUI
========

This is a graphical front end to the darkplaces remote console protocol.

Notable Features
----------------

* Server management
   * Save server connections for quick access
   * Manage/view multiple servers at the same time
* Server status overview
   * Customizable player actions (default: kick, kickban and mute)
   * Customizable quick commands (default: fs_rescan, restart, endmatch)
   * Map selection
* Interactive console
   * Cvar autocompletion
   * Cvar expansion
   * Option to attach to the server run-time log
   * History of recent commands
* Cvar viewer
   * View/edit values
   * Advanced filtering
   * Cvar description
   * Unset cvar
   * Reset cvars to their default value
* Many configuration options to tweak the tool to fit specific needs

License
-------

GPLv3+ see COPYING

Getting the latest sources
--------------------------

A web frontend to the repository is available at
https://github.com/mbasaglia/RconGui

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
