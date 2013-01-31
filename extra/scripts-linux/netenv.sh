#!/bin/bash

echo "" > client.log
echo "" > server.log
gnome-terminal --title "Client log" --geometry=80x60 --command "tail -f client.log"
gnome-terminal --title "Server log" --geometry=80x60 --command "tail -f server.log"

echo "Use 'tools/netrun.sh' to run a test."
