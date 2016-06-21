#Trivial File Transfer Protocol

TFTP is a simple, lockstep, file transfer protocol which allows a client to get from or put a file onto a remote host. Specification for the protocol can be found in RFC 1350.

*TFTP is virtually unused for Internet transfers.*

TFTP is implemented on top of UDP/IP protocols using the port number 69 for communication.

**TFTP only reads and writes files to or from the remote server. It cannot list, delete or rename files or directories and has no provision for user authentication.**


## Details

In TFTP, a transfer is initiated by the client issuing a request to read or write a particular file on the server.
The request can optionally include a set of negotiated transfer parameters proposed by the client under the terms specified by RFC 2347.

If the server grants the request, the **file is sent in fixed length blocks of 512 bytes of data by default** or the number specified in the blocksize negotiated option defined by RFC 2348.

Each block of transferred data must be acknowledged by an acknowledgment packet before the next block can be sent. A packet with data of size less than 512 bytes or the agreed blocksize option signals termination of a transfer.

*If a packet gets lost in the network, the intended recipient will timeout and may retransmit their last packet (which may be data or an acknowledgment), thus causing the sender of the lost packet to retransmit that lost packet.*

**The sender has to keep just one packet on hand for retransmission**, since the lock step acknowledgment guarantees that all older packets have been correctly received.

A time-out is setup when a data packet Data(k) or an acknowledgement ACK(j) is sent – if the corresponding ACK(k) or Data(j+1) is not received within the time-out, the same Data(k) or ACK(j) is retransmitted.

A transfer request is always initiated targeting port 69, but the data transfer ports are chosen independently by the sender and receiver during the transfer initialization.

When using TFTP, you should know the exact file name you want to receive.

#### Flow

> Any transfer begins with a request to read or write a file, which also serves to request a connection.
> If the server grants the request, the connection is opened and the file is sent in fixed length blocks of data of 512 bytes. Each data packet contains one block of data, and must be acknowledged by an acknowledgment packet before the next packet can be sent. A data packet with data less than 512 bytes signals termination of a transfer.
> If a packet gets lost in the network, the intended recipient will timeout and may retransmit his last packet (which may be data or an acknowledgment), thus causing the sender of the lost packet to retransmit that lost packet.


- The initiating host A sends an `RRQ` (read request) or `WRQ` (write request) packet to host S at port number 69, containing the filename, transfer mode.
- S replies with an  ACK (acknowledgement) packet to `WRQ` and directly with a DATA packet to `RRQ`. Packet is sent from a randomly allocated ephemeral port, and *all future packets to host S should be directed to this port*.
- The sender host sends numbered DATA packets to the receiver host, all but the last containing a full-sized block of data (512 bytes default). The destination host replies with numbered ACK packets for all DATA packets.
- The final DATA packet must contain less than a full-sized block of data to signal that it is the last. If the size of the transferred file is an exact multiple of the block-size, the sender sends a final DATA packet containing 0 bytes of data.
- Receiver responds to each DATA with associated numbered ACK. Sender responds to the first received ACK of a block with DATA of the next block.
- If an ACK is not eventually received, a retransmit timer re-sends DATA packet.


> Most errors cause termination of the connection. An error is signalled by sending an error packet. This packet is not acknowledged, and not retransmitted (i.e., a TFTP server or user may terminate after sending an error message), so the other end of the connection may not get it.

**Errors can be caused by**:
- Not being able to satisfy request (eg. file not found).
- Receiving a packet which cannot be explained by delay or duplication (incorrectly formed packet).
- Losing access to a necessary resource (eg. disk full or access denied).

These error cause in termination of connection.

> TFTP recognizes only one error condition that does not cause termination, the source port of a received packet being incorrect. In this case, an error packet is sent to the originating host.


## TFTP Packet Format
> **Extra**
>
> TFTP is designed to be implemented on top of the Datagram protocol (UDP). Since Datagram is implemented on the Internet protocol, packets will have an Internet header, a Datagram header, and a TFTP header. Additionally, the packets may have a header (LNI, ARPA header, etc.) to allow them through the local transport medium.
>
> TFTP does not specify any of the values in the Internet header. On the other hand, the source and destination port fields of the Datagram header (its format is given in the appendix) are used by TFTP and the length field reflects the size of the TFTP packet. The transfer identifiers (TID’s) used by TFTP are passed to the Datagram layer to be used as ports; therefore they must be between 0 and 65,535.
>

The TFTP header consists of a 2 byte opcode field which indicates the packet’s type (e.g., DATA, ERROR, etc.).

> In order to create a connection, each end of the connection chooses a TID for itself, to be used for the duration of that connection. The TID’s chosen for a connection should be randomly chosen, so that the probability that the same number is chosen twice in immediate succession is very low. Every packet has associated with it the two TID’s of the ends of the connection, the source TID and the destination TID. These TID’s are handed to the supporting UDP (or other datagram protocol) as the source and destination ports. A requesting host chooses its source TID as described above, and sends its initial request to the known TID 69 decimal (105 octal) on the serving host. The response to the request, under normal operation, uses a TID chosen by the server as its source TID and the TID chosen for the previous message by the requestor as its destination TID. The two chosen TID’s are then used for the remainder of the transfer.

> In the next step, and in all succeeding steps, the hosts should make sure that the source TID matches the value that was agreed on. If a source TID does not match, the packet should be discarded as erroneously sent from somewhere else. An error packet should be sent to the source of the incorrect packet, while not disturbing the transfer.


| Opcode  | Operation           |
|:-------:|:--------------------|
| 1       | Read request (RRQ)  |
| 2       | Write request (WRQ) |
| 3       | Data (DATA)         |
| 4       | Acknowledgment (ACK)|
| 5       | Error (ERROR)       |


```
 2 bytes    string    1 byte    string   1 byte
 ------------------------------------------------
| Opcode | Filename |    0    |  Mode  |    0    |
 ------------------------------------------------

                * RRQ/WRQ Packets *
Opcode = 1, 2
Mode = netascii, octet, mail
```

```
 2 bytes      2 bytes            n bytes
 ------------------------------------------------
| Opcode |    Block #    |         Data         |
 ------------------------------------------------

                  * DATA Packet *
Opcode = 3
1 <= Block #
0 <= n <= 512
```


#### Acknowledgement
An acknowledgment packet will contain the block number of the data packet being acknowledged. Each data packet has associated with it a block number; block numbers are consecutive and begin with one. Acknowledgement block number (in response to a WRQ) will be 0.

All packets other than duplicate ACK's and those used for termination are acknowledged unless a timeout occurs.

```
 2 bytes      2 bytes
 ------------------------
| Opcode |    Block #    |
 ------------------------

      * ACK Packet *
      
Opcode = 4
1 <= Block #
    Block # == 0 when WRQ is acknowledged.
```

#### Termination & Error

**Normal Termination**

The end of a transfer is marked by a DATA packet that contains between 0 and 511 bytes of data (i.e., Datagram length < 516). This packet is acknowledged by an ACK packet like all other DATA packets. The host acknowledging the final DATA packet may terminate its side of the connection on sending the final ACK.

On the other hand, dallying is encouraged. This means that the host sending the final ACK will wait for a while before terminating in order to retransmit the final ACK if it has been lost. The acknowledger will know that the ACK has been lost if it receives the final DATA packet again.


**Error: Premature Termination**

If a request can not be granted, or some error occurs during the transfer, then an ERROR packet (opcode 5) is sent. This is only a courtesy since it will not be retransmitted or acknowledged, so it may never be received. Timeouts must also be used to detect errors.

```
 2 bytes      2 bytes         string      1 byte
 ------------------------------------------------
| Opcode |   ErrorCode   |    ErrMsg    |    0   |
 ------------------------------------------------
 
                  * ERROR Packet * 
Opcode = 4
1 <= Block #
    Block # == 0 when WRQ is acknowledged. 
```

## Custom extension

#### Directory Listing Request

**Custom opcode**: 6

**Opcode name**: Directory Listing Request (*LSQ*)

The TFTP header of a packet contains the opcode associated with that packet, in this case the value `6`, in two bytes.

Message flow could be depicted as in the following diagram.
```
 ---------------------------------------------------------------------------------------------------------
|                                                                                                         |
|                  --------                                                       --------                |
|                 | Client |                                                     | Server |               |
|                  --------                                                       --------                |
|                     |                                                               |                   |
|                     |------------------------------LSQ----------------------------->|                   |
|                     |                                                               |                   |
|                     |<------------------------DATA #0 (512B)------------------------|                   |
|                     |                                                               |                   |
|                     |-----------------------------ACK #0--------------------------->|                   |
|                     |                                                               |                   |
|                     |<------------------------DATA #1 (137B)------------------------|                   |
|                     |                                                               |                   |
|                     |-----------------------------ACK #1--------------------------->|                   |
|                     |                                                               |                   |
|                                                                                                         |
 ---------------------------------------------------------------------------------------------------------
```


## Implementation Details

#### Classes
- Main class
  - TFTP Server IP `<QHostAddress>`
  - TFTP Server port and TIDs (negotiated port and own port). `<quint16>`
  - UDP Packet Count `<quint32>`
  - Timeout `<quint8>`
- Datagram class
  - source IP `<QHostAddress>`
  - source port `<quint16>`
  - destination IP `<QHostAddress>`
  - destination port `<quint16>`
  - block number `<quint32>`
  - opcode `<quint16>`
  - filename `<QByteArray>`
  - mode `<QByteArray>`
  - body `<QByteArray>`
  - prettyPrint()
  - read()       # the received datagram


#### Commands
- GET
- PUT
- LIST
