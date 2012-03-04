File and Directory Commands
=============================

Finding Where You Are
----------------------

You can check what directory you're in using the <code>pwd</code> command. <code>pwd</code> stands for print working directory.

e.g.

```bash

$ pwd
/Users/robert/Dropbox/Documents/My Code/Git_WA/Tutorials/linux

```

Listing the Directory's Contents
-------------------------------

You can list the contents of the directory using the <code>ls</code> command.

e.g.

```bash

$ ls
README.rdoc                           basic_jquery_jquery_ui_setup      processing_markers_via_json_and_ajax
basic_google_maps_api_interactions    linux                             using_the_users_location_with_google_maps

```

You can view extra details by passing specific flags to the command. I often use the l, A, h, r and t flags when listing the contents of the directory.

* The l flag means to list the contents in long format.
* The A flag means to show hidden files.
* The h flag means to display the file information in human readable format.
* The t flag means to sort by file modification time.
* The r flag means to reverse the order of the listed files.

You can learn more about the <code>ls</code> command by looking at the man page.

```bash

$ man ls

```

e.g. Using extra flags

```bash

$ pwd
/Users/robert/Dropbox/Documents/My Code/Git_WA/Tutorials

$ ls -lAhrt
total 8
-rw-r--r--   1 robert  staff   109B 21 Jan 11:48 README.rdoc
drwxr-xr-x   4 robert  staff   136B  5 Feb 14:31 basic_jquery_jquery_ui_setup
drwxr-xr-x   4 robert  staff   136B 25 Feb 17:46 basic_google_maps_api_interactions
drwxr-xr-x   4 robert  staff   136B 25 Feb 18:07 using_the_users_location_with_google_maps
drwxr-xr-x  16 robert  staff   544B 25 Feb 18:08 .git
drwxr-xr-x   3 robert  staff   102B 25 Feb 18:28 processing_markers_via_json_and_ajax
drwxr-xr-x   4 robert  staff   136B  4 Mar 21:40 linux

```


Changing Directory
-------------------

You can change directory using the <code>cd</code> command. <code>cd</code> stands for change directory.

The <code>cd</code> command takes 0 or 1 arguments.

The <code>cd</code> command takes your destination directory as an argument. This is the directory you want to change to. It can either be provided as a relative path, or a full path. A relative path is one relative to your current location. It doesn't have a leading slash (e.g. linux/tools). A full path is one with a leading slash (e.g. /tmp/linux/tools). To go up a directory level, specify the directory as <code>../</code>.

If you don't supply the command an argument, you will change directory to your home folder.

```bash

$ pwd
/Users/robert/Dropbox/Documents/My Code/Git_WA/Tutorials/linux

$ cd ../

$ pwd
/Users/robert/Dropbox/Documents/My Code/Git_WA/Tutorials

$ ls
README.rdoc                           basic_jquery_jquery_ui_setup      processing_markers_via_json_and_ajax
basic_google_maps_api_interactions    linux                             using_the_users_location_with_google_maps

$ cd linux

$ pwd
/Users/robert/Dropbox/Documents/My Code/Git_WA/Tutorials/linux

$ ls
basic_commands.markdown

```
