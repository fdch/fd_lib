## Pd-messages helper files

This is an overview on how to interact with pd (create objects, connect lines, open patches, etc.) using messages. 

Basically everything in pd is a message. However, since some messages are only used by pd's gui, there is no documentation on how these work. There are some tutorials out there but some might be outdated. This overview is an attempt to bring this messages to the surface.

This tutorial is still on the makings, so any suggestions will be more than welcomed. 

### Useful resources:
* [PdWiki](http://puredata.info/community/pdwiki/PdInternalMessages/): a list of all pd-messages and a tutorial (Pd-0.34) by Damien Henry
* [iemguts](https://git.iem.at/pd/iemguts.git) is really convenient for canvas-related stuff
* [Barknecht](http://puredata.info/community/conventions/convention04/lectures/tk-barknecht): a tutorial on data structures