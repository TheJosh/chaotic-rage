#!/bin/bash


sudo /etc/init.d/mysql stop
sudo /etc/init.d/postgresql stop
/bin/sync


if [ "$PLATFORM" == "linux" ]; then
	make -j2 || exit 1


else
	echo 'Unknown $PLATFORM variable'
	exit 1
fi
