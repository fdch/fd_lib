#!/bin/bash
#
# Just open <main.command> by clicking or CMD+O or  ./main.command
#
#
# This shell script allows the user to work with 2 interconnected instances of PD
# via netsend and netreceive, the former handling audio (main-audio.pd) and the 
# latter handling VIDEO (main-video.pd). Both files are created in the "/bin" 
# directory. Once the "/bin" directory has been created, it only opens the files in
# different instances of Puredata. 
#
#
#
#
#
#
#
#
# report anything here camarafede[at]gmail[dot]com
#
#
#
#
# NOTE: you might need to change the PATH to Pd's bin if different than the 
# one provided here
#
PD='/Applications/Pd-0.47-1.app/Contents/Resources/bin'
echo "
current path to Pd's *bin* is
$PD
"
# 
#
# ...SHOULDN'T DO ANYTHING AFTER THIS IF WORKING CORRECTLY...
#
#
MAIN="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
if [ ! -d "$MAIN/bin" ]; then
mkdir $MAIN/bin
echo 
"
***************************************************************
	creating main-audio.pd

***************************************************************
"
echo "#N canvas 19 474 315 62 10;
#X declare -path ~/Documents/fd_lib;
#X obj 7 6 fd_netsend;
#N canvas 1048 240 163 168 AUDIO 1;
#N canvas 0 22 454 473 dependencies 0;
#X text 18 17 declare all dependencies here;
#X obj 19 43 cnv 15 400 400 empty empty empty 20 12 0 14 -233017 -66577
0;
#X obj 33 55 declare -path ~/Documents/fd_lib;
#X restore 7 6 pd dependencies _______;
#N canvas 15 45 236 169 tables 0;
#X text 12 30 place all [array] objects here;
#X obj 19 56 array define example;
#X restore 7 69 pd tables _____________;
#N canvas 828 456 339 250 gui-glue 0;
#X text 19 20 place all gui related glue-type messages;
#X obj 27 55 r oscillator-freq;
#X obj 27 99 s report;
#X text 87 104 <-- sending to <report> will print to console, f 24
;
#X obj 27 77 list prepend frequency;
#X connect 1 0 4 0;
#X connect 4 0 2 0;
#X restore 7 90 pd gui-glue ___________;
#N canvas 0 22 254 322 presets 0;
#X obj 29 13 r pd-dsp-started;
#X obj 29 35 spigot 1;
#X obj 29 57 t b b;
#X msg 67 59 0;
#X obj 29 123 bng 15 250 50 0 empty empty empty 17 7 0 10 -262144 -1
-1;
#X text 50 123 Edit the following message with your on-load presets
, f 27;
#X msg 29 182 \; \; pd-AUDIO_GUI vis 1 \; \; oscillator-freq-set 246
\; main-vol-set 0 \;;
#X connect 0 0 1 0;
#X connect 1 0 2 0;
#X connect 2 0 4 0;
#X connect 2 1 3 0;
#X connect 3 0 1 1;
#X connect 4 0 6 0;
#X restore 7 111 pd presets ____________;
#N canvas 0 22 420 225 mixer 0;
#X obj 34 64 catch~ dacout;
#X obj 34 137 *~;
#X obj 34 170 dac~;
#N canvas 0 22 231 296 main-vol 0;
#X obj 30 198 pack f 10;
#X obj 30 220 line~;
#X obj 30 11 r main-vol;
#X obj 109 19 r mute;
#X obj 30 93 t f f;
#X obj 57 118 sel 0;
#X msg 48 150 0;
#X obj 98 194 s main-vol-set;
#X msg 98 167 0;
#X obj 109 41 tgl 15 0 empty empty empty 17 7 0 10 -262144 -1 -1 0
1;
#X obj 109 61 sel 1 0;
#X obj 30 242 outlet~;
#X obj 98 117 list prepend set;
#X obj 98 139 list trim;
#X connect 0 0 1 0;
#X connect 1 0 11 0;
#X connect 2 0 4 0;
#X connect 3 0 9 0;
#X connect 4 0 0 0;
#X connect 4 1 5 0;
#X connect 5 1 12 0;
#X connect 6 0 0 0;
#X connect 6 0 7 0;
#X connect 8 0 7 0;
#X connect 9 0 10 0;
#X connect 10 0 6 0;
#X connect 10 1 8 0;
#X connect 12 0 13 0;
#X connect 13 0 8 0;
#X restore 71 96 pd main-vol;
#X text 25 20 change this according to your needs;
#X text 71 173 <-- default <dacout> is copied to dac 1 and 2;
#X text 150 97 <-- the <main-vol> fader. See <pd-controls>;
#X connect 0 0 1 0;
#X connect 1 0 2 0;
#X connect 1 0 2 1;
#X connect 3 0 1 1;
#X restore 7 132 pd mixer ______________;
#N canvas 27 59 505 242 signals 0;
#X obj 28 157 throw~ dacout;
#X text 204 171 (controlled by <main-vol> amount);
#N canvas 0 22 450 300 oscillator 0;
#X obj 38 139 *~;
#X obj 87 100 r oscillator;
#X obj 37 25 r oscillator-freq;
#X obj 87 122 fd_switch;
#X obj 138 149 switch~;
#X obj 38 47 phasor~;
#X obj 38 67 cos~;
#X obj 38 260 outlet~;
#X text 202 115 <-- use this <switch> setup to reduce computation of
idle elements, f 29;
#X obj 146 194 env~;
#X floatatom 146 216 5 0 0 0 - - -, f 5;
#X obj 146 259 s to-video-pd;
#X text 184 196 <-- tap to get amplitude info;
#X text 247 250 <-- use this message dedicated to send data to <MAIN_VIDEO_PD>
, f 23;
#X obj 38 89 *~;
#X obj 156 28 r oscillator-gain;
#X obj 156 50 pack f 10;
#X obj 156 72 line~;
#X obj 146 235 list prepend frequency-1;
#X connect 0 0 7 0;
#X connect 0 0 9 0;
#X connect 1 0 3 0;
#X connect 2 0 5 0;
#X connect 3 0 0 1;
#X connect 3 1 4 0;
#X connect 5 0 6 0;
#X connect 6 0 14 0;
#X connect 9 0 10 0;
#X connect 10 0 18 0;
#X connect 14 0 0 0;
#X connect 15 0 16 0;
#X connect 16 0 17 0;
#X connect 17 0 14 1;
#X connect 18 0 11 0;
#X restore 28 27 pd oscillator;
#N canvas 0 22 430 310 oscillator-2 0;
#X obj 38 159 *~;
#X obj 87 120 r oscillator;
#X obj 38 5 r oscillator-freq;
#X obj 87 142 fd_switch;
#X obj 138 169 switch~;
#X obj 38 57 phasor~;
#X obj 38 77 cos~;
#X obj 38 266 outlet~;
#X obj 38 27 * 1.5;
#X text 202 135 <-- use this <switch> setup to reduce computation of
idle elements, f 29;
#X text 82 28 <-- a bit different;
#X obj 146 194 env~;
#X floatatom 146 216 5 0 0 0 - - -, f 5;
#X obj 146 259 s to-video-pd;
#X text 184 196 <-- tap to get amplitude info;
#X text 247 250 <-- use this message dedicated to send data to <MAIN_VIDEO_PD>
, f 23;
#X obj 38 109 *~;
#X obj 156 48 r oscillator-gain;
#X obj 156 70 pack f 10;
#X obj 156 92 line~;
#X obj 146 235 list prepend frequency-2;
#X connect 0 0 7 0;
#X connect 0 0 11 0;
#X connect 1 0 3 0;
#X connect 2 0 8 0;
#X connect 3 0 0 1;
#X connect 3 1 4 0;
#X connect 5 0 6 0;
#X connect 6 0 16 0;
#X connect 8 0 5 0;
#X connect 11 0 12 0;
#X connect 12 0 20 0;
#X connect 16 0 0 0;
#X connect 17 0 18 0;
#X connect 18 0 19 0;
#X connect 19 0 16 1;
#X connect 20 0 13 0;
#X restore 94 54 pd oscillator-2;
#X text 117 157 <-- <dacout> is the next-to-last element in the audio
chain, f 65;
#X text 173 27 ... as many signals as you can imagine ...;
#X text 191 52 ... and connect them to a [throw~ dacout];
#X text 6 7 place all signals in subpatches or cloned abstractions
here ...;
#X connect 2 0 0 0;
#X connect 3 0 0 0;
#X restore 7 48 pd signals ____________;
#N canvas 0 22 450 300 from-video-pd 0;
#X obj 20 77 t b a;
#X obj 20 254 bng 15 250 50 0 empty empty empty 17 7 0 10 -262144 -1
-1;
#X text 46 256 <-- use this if you want to bang things <after> each
incoming data stream is passed;
#X text 233 103 <-- route this message at will;
#X obj 161 136 s blabla1;
#X obj 104 163 s blabla2;
#X obj 47 190 s blabla3;
#X obj 47 103 route blabla3 blabla2 blabla1;
#X obj 20 17 r from-video-pd;
#X text 125 18 <-- this receive is dedicated to the incoming data from
<MAIN_VIDEO_PD>, f 39;
#X connect 0 0 1 0;
#X connect 0 1 7 0;
#X connect 7 0 6 0;
#X connect 7 1 5 0;
#X connect 7 2 4 0;
#X connect 8 0 0 0;
#X restore 7 27 pd from-video-pd ______;
#X restore 86 39 pd AUDIO;
#N canvas 337 473 276 249 AUDIO_GUI 1;
#X obj 17 30 cnv 15 250 200 empty empty empty 20 12 0 14 -233017 -66577
0;
#X obj 207 52 vsl 15 128 0.01 1 1 0 main-vol main-vol-set main-vol
0 -9 0 10 -262144 -1 -1 0 1;
#X obj 225 51 bng 15 250 50 0 mute mute-set mute 17 7 0 10 -262144
-1 -1;
#X obj 23 56 tgl 15 0 oscillator oscillator-set oscillator 17 7 0 10
-262144 -1 -1 0 1;
#X obj 113 57 nbx 5 14 -1e+37 1e+37 0 0 oscillator-freq oscillator-freq-set
oscillator-freq 0 -8 0 10 -262144 -1 -1 0 256;
#X obj 115 90 hsl 50 10 0.04 4 1 0 oscillator-gain oscillator-gain-set
oscillator-gain -2 -8 0 10 -262144 -1 -1 0 1;
#X text 99 104 move the gain;
#X text 35 192 amplitude of the signal controls scale;
#X text 23 8 place your AUDIO GUI here;
#X restore 7 39 pd AUDIO_GUI;" > $MAIN/bin/main-audio.pd
wait
echo 
"
***************************************************************
	creating main-video.pd

***************************************************************
"
echo "#N canvas 18 563 316 67 10;
#X declare -path ~/Documents/fd_lib;
#X declare -stdlib Gem;
#X obj 6 6 fd_netreceive;
#N canvas 1048 49 161 167 VIDEO 1;
#N canvas 0 22 454 473 dependencies 0;
#X text 18 17 declare all dependencies here;
#X obj 19 43 cnv 15 400 400 empty empty empty 20 12 0 14 -233017 -66577
0;
#X obj 40 85 declare -path ~/Documents/fd_lib;
#X obj 40 62 declare -stdlib Gem;
#X restore 7 6 pd dependencies _______;
#N canvas 17 45 236 169 tables 0;
#X text 12 30 place all [array] objects here;
#X obj 19 56 array define example;
#X restore 7 68 pd tables _____________;
#N canvas 828 456 339 250 gui-glue 0;
#X text 19 20 place all gui related glue-type messages;
#X obj 27 99 s report;
#X text 87 104 <-- sending to <report> will print to console, f 24
;
#X obj 27 55 r video;
#X obj 27 77 list prepend video;
#X connect 3 0 4 0;
#X connect 4 0 1 0;
#X restore 7 87 pd gui-glue ___________;
#N canvas 0 22 254 322 presets 0;
#X obj 22 78 bng 15 250 50 0 empty empty empty 17 7 0 10 -262144 -1
-1;
#X text 48 69 Edit the following message with your on-load presets
, f 27;
#X obj 22 16 r video-loadbang;
#X obj 22 38 del 10;
#X msg 22 115 \; \; gemwin-set 1 \; \; borders-set 1 \; video-set 1
\; \; \; \; pd-VIDEO_GUI vis 1 \;;
#X connect 0 0 4 0;
#X connect 2 0 3 0;
#X connect 3 0 0 0;
#X restore 7 107 pd presets ____________;
#N canvas 28 27 479 401 gemheads 0;
#N canvas 869 455 450 300 borders 0;
#X obj 22 138 scale 0.75;
#X msg 96 183 draw line;
#X obj 22 207 rectangle 5.2 4;
#X obj 22 160 pix_texture;
#X obj 22 95 t a a;
#X obj 96 138 spigot 1;
#X obj 96 160 t b b;
#X msg 132 159 0;
#X obj 22 50 r borders;
#X obj 22 73 gemhead 2;
#X obj 122 24 r border-scale;
#X obj 130 73 spigot;
#X obj 166 50 r border-scale-switch;
#X obj 122 46 / 100;
#X connect 0 0 3 0;
#X connect 1 0 2 0;
#X connect 3 0 2 0;
#X connect 4 0 0 0;
#X connect 4 1 5 0;
#X connect 5 0 6 0;
#X connect 6 0 1 0;
#X connect 6 1 7 0;
#X connect 7 0 5 1;
#X connect 8 0 9 0;
#X connect 9 0 4 0;
#X connect 10 0 13 0;
#X connect 11 0 0 1;
#X connect 12 0 11 1;
#X connect 13 0 11 0;
#X restore 19 53 pd borders;
#X text 17 11 place [gemheads] here;
#N canvas 869 455 450 300 video 0;
#X obj 22 207 rectangle 5.2 4;
#X obj 22 160 pix_texture;
#X obj 22 50 r borders;
#X obj 22 73 gemhead 1;
#X obj 22 138 pix_video;
#X obj 22 104 scale 1;
#X obj 91 14 r video-scale;
#X obj 91 71 spigot;
#X obj 145 73 r video-scale-switch;
#X obj 91 36 / 100;
#X connect 1 0 0 0;
#X connect 2 0 3 0;
#X connect 3 0 5 0;
#X connect 4 0 1 0;
#X connect 5 0 4 0;
#X connect 6 0 9 0;
#X connect 7 0 5 1;
#X connect 8 0 7 1;
#X connect 9 0 7 0;
#X restore 19 33 pd video;
#X restore 7 47 pd gemheads ___________;
#N canvas 0 22 450 300 from-audio-pd 0;
#X obj 20 77 t b a;
#X obj 20 17 r from-audio-pd;
#X text 125 18 <-- this receive is dedicated to the incoming data from
<MAIN_AUDIO_PD>, f 39;
#X obj 20 254 bng 15 250 50 0 empty empty empty 17 7 0 10 -262144 -1
-1;
#X text 46 256 <-- use this if you want to bang things <after> each
incoming data stream is passed;
#X text 78 76 <-- route this message at will;
#X obj 161 136 s blabla1;
#X obj 104 163 s blabla2;
#X obj 47 190 s blabla3;
#X obj 47 103 route blabla3 blabla2 blabla1 frequency-2 frequency-1
;
#X obj 237 207 s border-scale;
#X obj 303 170 s video-scale;
#X connect 0 0 3 0;
#X connect 0 1 9 0;
#X connect 1 0 0 0;
#X connect 9 0 8 0;
#X connect 9 1 7 0;
#X connect 9 2 6 0;
#X connect 9 3 10 0;
#X connect 9 4 11 0;
#X restore 7 27 pd from-audio-pd ______;
#N canvas 0 22 420 225 gemwin 0;
#X obj 14 186 gemwin;
#X obj 14 7 r gemwin;
#X msg 33 73 0 \, destroy;
#X obj 14 29 route float;
#X obj 14 51 sel 1 0;
#X text 76 116 edit this message according to your needs;
#X msg 14 148 0 \, destroy \, FSAA 4 \, offset 525 22 \, title MAIN_VIDEO_PD
\, dimen 520 400 \, create \, 1 \,;
#X connect 1 0 3 0;
#X connect 2 0 0 0;
#X connect 3 0 4 0;
#X connect 3 1 0 0;
#X connect 4 0 6 0;
#X connect 4 1 2 0;
#X connect 6 0 0 0;
#X restore 7 127 pd gemwin _____________;
#X restore 85 39 pd VIDEO;
#N canvas 615 473 274 249 VIDEO_GUI 0;
#X obj 8 26 cnv 15 250 200 empty empty empty 20 12 0 14 -233017 -66577
0;
#X obj 22 45 tgl 15 0 gemwin gemwin-set gemwin 17 7 0 10 -262144 -1
-1 1 1;
#X obj 22 68 tgl 15 0 borders borders-set borders 17 7 0 10 -262144
-1 -1 1 1;
#X obj 22 88 tgl 15 0 video video-set video 17 7 0 10 -262144 -1 -1
1 1;
#X obj 115 88 tgl 15 0 video-scale-switch video-scale-switch-set video-scale-switch
17 7 0 10 -262144 -1 -1 1 1;
#X obj 115 68 tgl 15 0 border-scale-switch border-scale-switch-set
border-scale-switch 17 7 0 10 -262144 -1 -1 1 1;
#X text 121 50 toggle ON these;
#X text 25 193 amplitude of the signal controls scale;
#X text 6 7 place your VIDEO GUI here;
#X restore 6 39 pd VIDEO_GUI;" > $MAIN/bin/main-video.pd
wait
fi
echo 
"
***************************************************************
	opening all in $MAIN/bin with Puredata at $PD
***************************************************************
"
cd $PD
open $MAIN/bin/main-audio.pd  
open --new --background $MAIN/bin/main-video.pd
echo 
"
***************************************************************
	CLOSE THIS TERMINAL
***************************************************************
"
#exit
