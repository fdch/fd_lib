#!/bin/bash
FILENAME="!an_overview/an_overview-help-$1.pd"
SCREENX=1280
SCREENY=700
INC=25
#below are just fixed stuff
XPOS=$INC
YPOS=$INC
XDIM=0
YDIM=0
YINC=0
XINC=0
LIM=$(($SCREENY-($INC*2)))
OBJ=""
LLINE=""
#this line gets all files without "-help", "-test", "_dummy" or "_" as names
OBJECTS=$(find . \! -name "*~-help*" \! -name "*-help*" \! -name "*~-test*" \! -name "*-test*" -name "*.pd" \! -name "_dummy.pd" \! -name "_*.pd" -print)
echo "#N canvas 0 22 $SCREENX $SCREENY 12;" > "$FILENAME" #root canvas

#loop through all objects found
for i in $OBJECTS; do
OBJ=$(basename "$i" ".pd") #get basename of the abstraction
LLINE=$(tail -n 1 "$i") #get the last line of the abstraction
if [ "$OBJ" == "fd_plot" ] || [ "$OBJ" == "fd_getfiles" ] || [ "$OBJ" == "fd_colorpicker" ]
then
echo "#X text $XPOS $YPOS See help for \"$OBJ\";" >> "$FILENAME" #text it
else
echo "#X obj $XPOS $YPOS $OBJ;" >> "$FILENAME" #print the object
fi
if grep -q "coords" <<< "$LLINE" #check if it's a GOP abstraction
then #we get the dimensions and adjust Yinc and Xinc accordingly
XDIM=$(echo "$LLINE" | cut -d ' ' -f 7) #get xpos
YDIM=$(echo "$LLINE" | cut -d ' ' -f 8) #get ypos
  if [ "$YDIM" -gt "10" ] #check if it's a relatively valid ypos...
  then
  ((YINC+=$YDIM)) #increment ypos by gop ypix
    if [ "$XDIM" -gt "100" ]
    then
    ((XINC+=$XDIM)) #increment xpos by gop xpix
    fi
  fi
fi
if [ "$YINC" -gt "$INC" ] #take care of margins
then #there was a gop before us
((YPOS += $YINC))
else #there wasn't one
((YPOS += $INC))
fi
if [ "$YPOS" -gt "$LIM" ]
then #make a new column
((YPOS = $INC)) #reset ypos
  if [ "$XINC" -gt "$INC" ]
  then #there was a gop before us
  ((XPOS += $XINC))
  XINC=0
  else #there wasn't one
  ((XPOS += 120))
  fi
fi
YINC=0
done
exit
