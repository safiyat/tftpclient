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


## TFTP Packet Structure


## Custom extension

### Directory Listing Request

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
