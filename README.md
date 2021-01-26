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
  - Run the command "run.sh <server_port_number_1> <server_port_number_2> <server_port_number_3> ... "   
  
    - Note that you can add as many server port numbers as you wish as command line arguments to "run.sh".  These server port numbers will be the ports of the servers that are         spun up for the load balancer to interface with.  The default port number for the load balancer itself is hard-coded in the "run.sh" file as port 8080, but you can change       it if you so desire.  Simply open the "run.sh" file and switch out the the "8080" port number for any other port number you wish.  The -R and -N values are also hard coded       as 4 and 4, respectively, but again, you may change those values if you wish by simply opening the "run.sh" file and altering them.
    
    - The "run.sh" file will handle everything for you.  It will "cd" to the MTS sub-folder and build a docker image of the multi-threaded http server.  It will then run an           instance of the multi-threaded http server for every port number that you entered as an argument to "run.sh", each within its own docker container running ubuntu 18.04.         You may throw requests at these port numbers directly, or you may use the load balancer, which again, can be contacted at port 8080 by default.  Upon issuing a "ctrl-C"         (SIGINT) to the "run.sh" script, the script will kill the load balancer process, run 'make clean' to clean the local directory, "cd" to the MTS sub-directory, and call the       "stop_remove.sh" script to stop and remove all of the docker containers running instances of the multi-threaded http server.  Nice and tidy.
 
 - EXAMPLE OF HOW TO RUN:
     
   Assuming you have already cloned the repo;  
   Run:  
   $./run.sh 8082 8083 8084  
   At this point you should see print messages from the load balancer.."[+] Load balancer is waiting" should be the first message displayed.  
   Remember, above we have told "run.sh" to open three instances of the multi-threaded http server because we have given it three port numbers. 
     
   You can throw a request at the load balancer like so;  
     
   curl -v localhost:8080/<file_name>  
     
   Or you can throw a request at any one of the servers like so;  
     
   curl -v localhost:8082/<file_name>  
     
   When you're ready to stop using the load balancer and servers, simply throw a SIGINT (ctrl-C) at your "run.sh" script, and voila, you're done using the load balancer and        servers.  Wait a few moments and everything will be cleaned.  
   
   Happy load balancing!!  


