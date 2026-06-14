#! /bin/bash

# Default build and run script

# Go into the script's directory
cd "$(dirname "$0")"

# Create build dir if it does not exists
if [ ! -d "build" ]; then
	mkdir build
fi

cd build && cmake .. && make

# if [ $? = 0 ]; then
# 	./moc
# fi
