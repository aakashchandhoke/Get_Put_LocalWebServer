# Get_Put_LocalWebServer
We have implemented a HTTP client and server running a paired down version of HTTP/1.0. We have extended the client and server to make use of some of the application level headers. In this project we have GET the request from the local server in iterative manner as well as concurrent manner. In addition to these, we have GET the request from the Web Server too. We have PUT the request in case of local server in an iterative manner as well as concurrent manner. 


Run_Client <br>
./httpclient <Host_name> <Port_No> <GET/PUT> <Path_Of_File>

Two Modes are there :-<br>

A.> For GET/PUT in LocalServer :-<br>
For eg.) <br>
./httpclient localhost 50000 GET file.txt<br>
              or<br>
./httpclient localhost 50000 PUT file.txt<br>
<br>
B.> FOR GET from WebServer :-<br>
For eg.)<br>
./httpclient www.google.co.in 80 GET index.html<br>

Run_Server (LOCAL) :- <br>
<br>
./httpclient <Port_No><br>


# Made by:-
# Aakash Chandhoke
