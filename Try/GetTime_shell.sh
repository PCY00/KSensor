#!/bin/bash

while true
do
  Gtime=$(date "+%Y%m%d%H%M%S")             
  echo -e ${Gtime} >> system_time.txt   # >> : Earwriting, > : Overwrite, 
                                        # -e : Enable backslash and escape characters
  sleep 5                               #5sec delay
done
