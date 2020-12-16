

Description:
build: make all
run:   ./concur or ./concur sem (to use semphores)
repo: https://github.com/latenite4/concur_c.git

This program demonstrates multiple threads started by one process writing to
the same file.  because there may be interference when writing to the 
common file, semapphores are used.

Dependencies:  /usr/local/lib/libutilz.so
Code for this library comes from repo: https://github.com/latenite4/utilz_c.git

12/15/20 still can't cause race condition in shared memory, with or without semaphores.
