own_tests/mini_serv.c -> My own "Exam Ready" Version of the Code



Explanation of important code parts:

- count: id for each user, updated in 'register_client()' (line 117)
- max_fd: the highest fd activly use for a client, updated in 'register_client()' (line 116) and 'disconnect_client()' (line 105)
- ids: a vector like ptr, holding id for each fd, updated in 'register_client()' (line 117)
- msgs: double ptr, holding the last msg for each active client, updated via 'extract_message()' (line 87), in disconnect_client() (line 99), in 'register_client()' (line 118) and 'main()' (line 178)
- afds: a fd_set (like a vector) holding 'all' open fds
- rfds: a fd_set (like a vector) holding 'all' open fds for reading
- wfds: a fd_set (like a vector) holding 'all' open fds for writing
- fd_sets: updated mainly in 'main()' (line 156) to make all available to read and write
- buf_read: The buffer to read msgs, fixed to read up to 1000 char at the same time and set the \0 at the 1001 char in the end, updated in 'main()' (line 172, 177, 178)
- buf_write: A buffer to write specific msgs, like the one from a client, to the other clients, updated in 'register_client()' (line 120, 121), 'disconnect_client()' (line 97, 98) and 'send_msg()' (line 88, 89)



FD Variables / Funcs:

- FD_SETSIZE: Fixed size from system, can be modifyed, defines how many fds a fd_set can hold up to
- FD_ZERO: Set the given fd_set to zero / null
- FD_SET: Sets a given fd into a given fd_set
- FD_ISSET: Searches if a given fd is set inside a given fd_set
- FD_CLR: Removes a given fd from a given fd_set



Funcs:

- fatal_error: Prints the default error
- notify: prints given msg to all clients inside the write fd_set excluding the author client
- send_msg: while extracting client msg line by line from given buffer into strings, sending them via notify to every other client
- disconnect_client: disconnecting a client, notify other clients, freeing msg space, remove fd and update max_fd
- register_client: add a client, give him a id, reset the msgs ptr to null (in case it's been use before), adding fd to 'all' fd_set (later updated to 'read' / 'write' fd_set), notify other clients
- create_socket: creating the socket fd and adding it to 'all' fd_set