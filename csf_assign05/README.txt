Brady Bock
Lucy Malmud

Every table required synchronization which we implemented with mutexes.
We knew that tables would require synchronization since it is possible
for multiple clients to access and modify the same table, and we wanted
to ensure that the data stored was consistent. We did not need the value
stacks to be synchronized, since each client has their own value stack. We
implemented the synchronizaiton with mutexes by locking the mutex corresponding
to the table. This is because the table modification functions are not atomic,
so we hold the lock until the operation is completed.