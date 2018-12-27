# socket_code
client-server programming

###### simple compile 
    gcc client_server.c -o prog
###### run
    Usage: <HOST> <PORT> <PATH> <QUERY>
    ./prog (run with default values)
    ./prog localhost 3001 /temp ?sensorId=01\&value=20 (notice the escape of '&')
    
A small c program to send/receive requests/responses to a JS express webserver running locally
using socket programming

