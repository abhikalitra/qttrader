#!/bin/sh

printf "\nGenerating source code API documentation ...\n"
dxCmd=`command -v doxygen`
if [ -z "$dxCmd" ] ; then
  printf "WARNING: Cannot find 'doxygen'.\n"
else
  $dxCmd misc/doxygen/config.txt
  printf "Done.\n"
  printf "Now do './configure; sudo make install' to merge the API docs with the Qt help system.\n"
fi
