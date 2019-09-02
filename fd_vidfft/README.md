# fd_vidfft 



 

 

Inlets:
Arguments:
Inlet 1: gemlist
Outlets:
Outlet 1: gemlist
Example:
pix_texture
Help>Find Externals> SEARCH:Gem >> DOWNLOAD
add the object [declare -stdlib Gem] to the parent patch
Needs Gem
Synopsis: [fd_vidfft]
Description: Renders an ffifft image of incoming pixes
The [fd_vidfft] abstraction calculates the fft of the
Inlet 1: message: size N. N must be 2^n
Inlet 2: message: window (hann float| gauss float| triang
Create window:
TURN DSP ON!
GEM abstraction 
github.cofdcfd_lib
size N must be 2^n
Inlet 1: float: 1|0 display fft or ifft (default 1)
Inlet 2: message: smooth float (-1..1) (default -1)
Inlet 2: message: coef float (default 2)
Inlet 2: message: crop float float (x , y) pairs for
-1..1
shift drag these:


 
