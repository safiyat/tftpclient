#Trivial File Transfer Protocol

TFTP is a simple, lockstep, file transfer protocol which allows a client to get from or put a file onto a remote host. Specification for the protocol can be found in RFC 1350.

*TFTP is virtually unused for Internet transfers.*

TFTP is implemented on top of UDP/IP protocols using the port number 69 for communication.

**TFTP only reads and writes files to or from the remote server. It cannot list, delete or rename files or directories and has no provision for user authentication.**


## Details

In TFTP, a transfer is initiated by the client issuing a request to read or write a particular file on the server.
The request can optionally include a set of negotiated transfer parameters proposed by the client under the terms specified by RFC 2347.

If the server grants the request, the **file is sent in fixed length blocks of 512 bytes by default** or the number specified in the blocksize negotiated option defined by RFC 2348.

Each block of transferred data must be acknowledged by an acknowledgment packet before the next block can be sent. A data packet of less than 512 bytes or the agreed blocksize option signals termination of a transfer.

*If a packet gets lost in the network, the intended recipient will timeout and may retransmit their last packet (which may be data or an acknowledgment), thus causing the sender of the lost packet to retransmit that lost packet.*

**The sender has to keep just one packet on hand for retransmission**, since the lock step acknowledgment guarantees that all older packets have been correctly received.

A time-out is setup when a data packet Data(k) or an acknowledgement ACK(j) is sent – if the corresponding ACK(k) or Data(j+1) is not received within the time-out, the same Data(k) or ACK(j) is retransmitted.

A transfer request is always initiated targeting port 69, but the data transfer ports are chosen independently by the sender and receiver during the transfer initialization.

When using TFTP, you should know the exact file name you want to receive.

TFTP Opcodes:<br>
 1 : Read Request (RRQ)<br>
 2 : Write Request (WRQ)<br>
 3 : Data (DATA)<br>
 4 : Acknowledgment (ACK)<br>
 5 : Error (ERROR)<br>
 6 : Option Acknowledgement


#### Flow
- The initiating host A sends an `RRQ` (read request) or `WRQ` (write request) packet to host S at port number 69, containing the filename, transfer mode.
- S replies with an  ACK (acknowledgement) packet to `WRQ` and directly with a DATA packet to `RRQ`. Packet is sent from a randomly allocated ephemeral port, and *all future packets to host S should be directed to this port*.
- The sender host sends numbered DATA packets to the receiver host, all but the last containing a full-sized block of data (512 bytes default). The destination host replies with numbered ACK packets for all DATA packets.
- The final DATA packet must contain less than a full-sized block of data to signal that it is the last. If the size of the transferred file is an exact multiple of the block-size, the sender sends a final DATA packet containing 0 bytes of data.
- Receiver responds to each DATA with associated numbered ACK. Sender responds to the first received ACK of a block with DATA of the next block.
- If an ACK is not eventually received, a retransmit timer re-sends DATA packet.

**Errors can be caused by**:
- Not being able to satisfy request (eg. file not found).
- Receiving a packet which cannot be explained by delay or duplication (incorrectly formed packet).
- Losing access to a necessary resource (eg. disk full or access denied).

These error cause in termination of connection.

## TFTP Packet Structure


## Custom extension

#### Directory Listing Request

**Custom opcode**: 6
**Opcode name**: Directory Listing Request (*LSQ*)

The TFTP header of a packet contains the opcode associated with that packet, in this case the value `6`, in two bytes.

Message flow could be depicted as in the following diagram.
```
-----------------------------------------------------------------------------------------------------------
|                                                                                                         |
|                 ----------                                                     ----------               |
|                 | Client |                                                     | Server |               |
|                 ----------                                                     ----------               |
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
-----------------------------------------------------------------------------------------------------------
```


## Implementation Details

#### Main class data members
 - TFTP Server IP
 - TFTP Server port and negotiated port
 - UDP Packet Count
 - Timeout


#### Commands
- GET
- PUT
- LIST
