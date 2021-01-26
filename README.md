# HTTPServerLoadBalancer

This load balancer will channel all requests to the least loaded server, i.e., the server that has serviced the least requests.

To spin up load balancer:

Simply type "make" into terminal.  This will create a binary "loadbalancer2" executable file.

- If the option for number of requests before a health check poll is triggered, siimply provide the "-R (number of requests)" flag as an argument.
- If a particular number of threads is to be used, sipmly provide the "-N (threadCountNumber)" flag as an argument.

- The binary "loadbalancer2" executable file can be run by simply typing "./loadbalancer portnumber arg2 arg3 ..." where "portnumber" is the esired port number for client connection.  The argument after "portnumber" must also be a port number, but it will be the first port number of the srever that the loadbalancer will be capable of connecting to.  These two portnumbers are required non-optional arguments.  Other port numbers may be added if one desires the loadbalancer to connect to more servers.
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

This is a load balancer for the HTTP server that is also featured on this local Github account in its own repository.  
This load balancer will route requests to the "least loaded" server (meaning the server that has serviced the lowest number of requests).  
This load balancer accepts two optional integer command line arguments that have an effect on its operations.  These optional command line arguments are prefaced with the flags -R and -N, respectively.
  
- The -R flag indicates the threshold for sending a "health check" request to each of the servers.  The value returned from the "health check" request is the total number of requests that the server has received over the total number of "missed" requests (a miss is any request that resulted in a response code in the 400's or 500's ).  It is the total number of requests that is used to determine the "least loaded" server, and so the -R argument really just translates to "the number of requests that the load balancer must see before assessing (or re-assessing) which server is the 'least loaded'".  
  
- The -N flag indicates the number of threads that the load balancer will use as "workers". The more threads the load balancer has to work with, the more requests the load balancer can concurrently handle (until increasing the thread count starts to meet a point of diminishing returns due to limitations of the hardware, of course).  
  
- HOW TO RUN:  
  
  - Clone the repository.  


