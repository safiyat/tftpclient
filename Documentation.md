# Documentation

The code has been organized in the conventional OOP paradigm.

There are three classes, each with a defined role.

## FileHandler

This is a helper class, that helps reading from and writing into files (text and binary).

Writing to file is fairly simple. In **netascii** mode, the data received is written in text format, porting the line endings to the current platform and written. In **octet** mode, the data is written as is, in binary format.

Reading from files is slightly complex in case of **netascii** mode.
**netascii** mode, as defined in RFC764, specifies that the line endings have to be in CRLF (Carriage Return, Line Feed) format, irrespective of the source file line endings or platform specifications. Hence, the files to be sent (or received) in **netascii** need to be translated to local platform line endings.
Line Endings:
- Windows: CRLF
- Unix-family: LF
- Macintosh: CR

The data to be sent per block (after reading) *has to be* 512 bytes in total, which would mean that data translated before sending in **netascii** mode has to be truncated to 512 bytes, if the size increased after porting to CRLF format (addition of CR, '\r').
The function `readBlock` maintains a marker (`quint64 position`) to track the position the file has been read to. It also reads the data from the file and translates it to CRLF format in case of **netascii** mode.
The function `writeBlock` writes the data received from the server into files, converting from CRLF format when necessary.

## tftpDatagram
This too is a helper class that helps in forming and parsing tftp packets that are to be sent to and received from the tftp server respectively.
The private functions read from, and write to tftp packets.
The public functions set and get the data members of the class, that are used by the helper functions.
The public functions also provide means to send and receive packets in all the formats.
**This class has been derived from the QUdpSocket class, building on the functionalities of UDP sockets**.

## tftpClientWindow
This is the main class that provides the UI, and the flow of the TFTP client.

The UI has been created using Qt Designer (tftpclientwindow.ui), which is in XML format and parsed by Qt.

The objects of helper classes mentioned above are created and used here.

There are setters and getters for the variables required for tftp.

The main functions in the class are:
- `onBrowseFile`
- `onExecuteClick`
These two functions are called *slots* in Qt parlance. Slots are analogous to listeners in Java.
These functions are executed whenever the **Browse** button or the **Execute** button is clicked.

- `getMethod`
This function implements the GET method of tftp, by sending an RRQ and then corresponding ACKs to the DATA packets received.
It first loops over sending RRQ packets, until one is successfully sent.
Then it waits for the first DATA packet to arrive. Once the data packet has arrived, the data is saved, the size of data block is noted and the ACK for that block is sent.
Then the infinite loop to receive data and send acknowledgments is started, until the DATA block received is less than the size of the first data block.
The received data is then written to file using the fileHandler class.

- `putMethod`
This function implements the PUT method of tftp, by sending a WRQ and then corresponding DATA packets after the ACKs are received.

It first loops over sending WRQ packets, until one is successfully sent.
Then it waits for the first ACK packet to arrive.
Once the first ACK packet has arrived, the data is then sent block by block with the size of 512 bytes, until the file is completely sent.
The last data block is generally less than 512 bytes, which signifies the last packet. In case the last data block too is 512 bytes in size, an additional data packet with a data block size of 0 bytes is sent to signify it as the last packet.
All the data packets being sent are sent repeatedly until their corresponding ACKs are received.

- `listMethod`
This function implements the LIST method, by sending an LSQ and then corresponding ACKs to the DATA packets received.
It first loops over sending LSQ packets, until one is successfully sent.
Then it waits for the first DATA packet to arrive. Once the data packet containing directory listing has arrived, the data is saved, the size of data block is noted and the ACK for that block is sent.
Then the infinite loop to receive data and send acknowledgments is started, until the DATA block received is less than the size of the first data block.
The received data is then printed in the output text browser.


- `bindUdpSocket`
This function creates a UDP Socket to listen to on a random free port available on the local computer. It returns the port number the UDP socket is listening on.

- `disconnectSocket`
This function disconnects the UDP socket and frees up the port.

- `printDatagram`
This function takes as input the a tftp datagram and prints the contents in human readable form. It can read all the packet formats and print them.

- `opcodeToString`
This function returns the operation from the opcode as a string. Used in `printDatagram` function.
