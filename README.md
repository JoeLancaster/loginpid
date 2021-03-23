# loginpid
Follow the chain of parents of a given pid (or loginpid's pid) until: a process started with argv[0][0] == '-' i.e a login shell or a pid of 1 is found, prints the found pid
Returns 0 on success, 1 on error.
