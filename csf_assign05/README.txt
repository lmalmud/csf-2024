Brady Bock
Lucy Malmud

NOTES ON RUNNING REFERENCE CLIENTS
chmod +x ref_server
nc localhost 5000
./ref_server 5000
LOGIN alice
OK
CREATE fruit
OK
PUSH 42
OK
SET fruit apples
OK
BYE
OK
./get_value localhost 5000 alice fruit apples

NOTES ON RUNNING SERVER
./server 5000
other window: nc localhost 5000