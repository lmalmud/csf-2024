Brady Bock
Lucy Malmud

NOTES:
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