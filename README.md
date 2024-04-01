# WEBSERV Project

here is the full subject description: [en.subject.pdf](https://github.com/yannCardona/webserv/blob/d86a6c6284fc223ff8c0fed014e0cb8c7fec7fc4/en.subject.pdf)

This project is coded in c++. It's a simple http-server.
1. 
* you can serve a fully static website
* clients can upload files
* it has GET, POST, and DELETE methods implemented
* it can listen to multiple ports
* in the config file you can setup routes with one or multiple of the following rules/configuration:
  - turn on or off directory listing
        * define a directory or a file from where the file should be searched
        * define a list of accepted HTTP methods for the route
        * set a default file to answer if the request is a directory
        * execute CGI based on .py file extension
  

