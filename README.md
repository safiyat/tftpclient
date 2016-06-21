# Custom TFTP Client

Implemented a TFTP Client Application protocol layer, capable of sending and receiving files according to the TFTP RFC 1350 with the following features:

a) Implemented the upload mechanism (write-request WRQ). Also the write-request and acknowledgment messages is displayed (ASCII).

b) Implemented the download mechanism (read-request RRQ). Also the Read-request and acknowledgment messages is displayed (ASCII).

c) Implemented the directory listing mechanism (directory-listing request). This is a custom TFTP extension, not available in the standard protocol description (RFC).