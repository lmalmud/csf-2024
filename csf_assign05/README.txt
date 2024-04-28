Brady Bock
- Structure of handling client connections
- SET and GET invalid accesses
- Error handling
- Writing responses
Lucy Malmud
- Body of main operations
- Refactored code
- Initial synchronization code

Every table required synchronization which we implemented with mutexes.
We knew that tables would require synchronization since it is possible
for multiple clients to access and modify the same table, and we wanted
to ensure that the data stored was consistent. We did not need the value
stacks to be synchronized, since each client has their own value stack. We
implemented the synchronizaiton with mutexes by locking the mutex corresponding
to the table. This is because the table modification functions are not atomic,
so we hold the lock until the operation is completed.

We are confident that the server is free of deadlocks because we released all
locks in a timely manner. We did so by tracking which tables a client
had modified in transaction mode and freeing them all before leaving
transaction mode. Similarly, when not in transaction mode, we were
sure to immediately lock and unlock the mutex surrounding the table modification operation.