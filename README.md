# Blitz P2P Bridge - Tunnel TCP for p2p streams

This server provides a means for two, or more, app to communicate continuously over TCP connection.

Basically when app A write on channel the app B receive the data and vice versa of course.

The paper [Peer-to-Peer Communication Across Network Address Translators](http://www.brynosaurus.com/pub/net/p2pnat/) inspired this project.

## Characteristics

* Each client needs an ID. Currently this ID is composed of 15 charcacters defined by the client
* The first message sent to the server, by the client, is your ID
* The second is the expected id of your partner
* If the parter is not online, the client is moved to the waiting list
* And if the is online, the client is linked with his partner they and can comunicate
* When two clients is linked, both are removed from the waiting list

## TODO

- [ ] P2P working:
    - [x] Connection between clients, with waiting list
    - [x] Send/Receing content from partner
    - [ ] Logs/Output with datetime, ID, sizes and IP
- [ ] UnitTests
- [ ] Encrypted communication
- [ ] Pre-registered ID's
- [ ] Real-time information:
    - [ ] Current ID's in use
    - [ ] Number of connections
- [ ] Services:
    - [ ] Systemd: stop, start and restart
    - [ ] SystemV: stop, start and restart
- [ ] Packages:
    - [ ] RPM
    - [ ] DEB
- [ ] Reports, by log analysis:
    - [ ] Bytes per ID and IP at time
