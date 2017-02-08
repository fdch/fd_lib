# fdpd
The purpose of <fdpd> is to allow for a single double-click on <fdpd.command> to open a Pureadata workspace with 2 interconnected instances(OSX), one for audio and one for video (Gem). All the trouble of connecting two instances of pd is taken care of by <fdpd>, the rest is up to you.

# Puredata
get Pd here: http://msp.ucsd.edu

# Gem
get Gem & manuals via deken way 
	open pd
	Help > Find Externals
	Type "Gem"
	find your version and DOWNLOAD
	restar Pd

or here http://gem.iem.at 

# Once fdpd is up and running
Just opening 'fdpd.command' by clicking (or CMD+O or  ./fdpd.command) works like a charm

# Installation (First time only) 
(requires basic copy-pasting on Terminal):

1. Copy this directory <fdpd> in <your-working-directory>.

2. Open Terminal:
	$ cd your-working-directory
	$ chmod u+x fdpd.command
	*** See below before continuing ***
	$ ./fdpd.command
	$ exit
(IF PD OPENED: quit the terminal after this, you won't need it again)

# VERY IMPORTANT
Mantra: "know thy pd path"

Before doing <./fdpd.command> you need to check that the PATH to Pd's bin IS THE SAME than this one: '/Applications/Pd-0.47-1.app/Contents/Resources/bin'. If it isn't, you need to edit (with TextEdit or any text editor) <fdpd.command> in line 36, replacing PD='/Applications/Pd-0.47-1.app/Contents/Resources/bin' with PD='/<your-path>/Pd-<version>.app/Contents/Resources/bin', filling in with your path and your Pd version accordingly.

# Information
 This shell script allows the user to work with 2 interconnected instances of PD via netsend and netreceive, the former handling audio (main-audio.pd) and the latter handling VIDEO (main-video.pd). Both files are created in the "/bin" directory. Once the "/bin" directory has been created, it only opens the files in different instances of Puredata. 

# How it works
You will see two Pd-s opening, one called MAIN_AUDIO_PATCH and the other MAIN_VIDEO_PATCH. The first thing that happens is the propt for PD's Audio Properties. You are required to choose your devices on every load (can be disabled). Choose your interface and click "OK". After you click "OK", the DSP will turn on for MAIN_AUDIO_PATCH. Once DSP is on, the color of each patch will turn to light green, indicating that MAIN_AUDIO_PATCH is connected to MAIN_VIDEO_PATCH. Right-click on the black canvases to see the workings of such connection. If colors did not change, change port number to be the same on both patches, then click "1" bang and "2" bang buttons (order is important). If colors turned GREEN-ish, you will see the [gemwin] created. This means you are ready to go.

# What is in the default patches?
Move the faders up and down and click on some toggles to get this default setup working. What it does is the following: the main audio is generating a very simple signal of 2 oscillators. The amplitude of the oscillators is analyzed and their dB information is passed to the video patch to control the width of the video and border rectangle sizes. The default frequency is 237 Hz, which is rather fast to see, but good enough to hear. Bring it down to 3 Hz, and you'll see it instead. 

# What can you do?
You can put as many signals as you want and connect them to as many parameters of the video as you want, and viceversa. There are a few examples of this in use, that I plan to share in the future. Basically, this is simply a workspace to use Gem with Pd, which are fantastic for working with visuals (OpenGL, GLSL) and audio. 

The basic element of a connection between the two instances, in this case, is a [list] to be sent between the patches. The pd-objects behind this are [netsend] and [netreceive]. This connection is solved for you. Below are instructions on adding new variables in between the instances.

# Sending lists between AUDIO and VIDEO (and viceversa)
As of now, when you create a new message-route from one pd instance to the other, you need to use [lists]:
	[VARIABLE] <-- whatever incoming list of x lenght
	|
	[list prepend VARIABLE-NAME] <-- assign it a name
	[s to-audio-pd] (or [s to-video-pd] depending where you are)

Then, to use this VARIABLE-NAME on the other instance, you need to go to the other instance, enter [pd from-video-pd] (or [pd from-audio-pd], depending where you are], and edit the [route] object so that:
	[route x y ... VARIABLE-NAME] <-- put the name here
	|	|	|
	[s x]	[s y]	[s VARIABLE-NAME] <-- and here again

# To do
Automated routing of messages between instances. This process described above works very well. But, it can be rather tedious and one could imagine it happening automatically once you create a send and receive pair. 

# Issues
report anything here camarafede[at]gmail[dot]com


