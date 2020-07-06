# HTTPServerLoadBalancer

This load balancer will channel all requests to the least loaded server, i.e., the server that has serviced the least requests.

To spin up load balancer:

Simply type "make" into terminal.  This will create a binary "loadbalancer2" executable file.

- If the option for number of requests before a health check poll is triggered, siimply provide the "-R (number of requests)" flag as an argument.
- If a particular number of threads is to be used, sipmly provide the "-N (threadCountNumber)" flag as an argument.

- The binary "loadbalancer2" executable file can be run by simply typing "./loadbalancer portnumber arg2 arg3 ..." where "portnumber" is the esired port number for client connection.  The argument after "portnumber" must also be a port number, but it will be the first port number of the srever that the loadbalancer will be capable of connecting to.  These two portnumbers are required non-optional arguments.  Other port numbers may be added if one desires the loadbalancer to connect to more servers.
