#!/bin/bash
#
#  Runs the pound test over and over and over again
#  Any coredumps are saved in a directory and then we go again
#  Probably a good idea to run your tests on a clean build
#


# Check it's actually there
if [ ! -f chaoticrage ]; then
	echo "Chaotic Rage main binary 'chaoticrage' not found."
	exit
fi

# We need coredumps to be generated.
ulimit -c unlimited

# The log file name includes the PID so you can run multiple
# multipound tests at once. Like some sort of multimultipound.
LOGFILE="multipound$$.log"


while [ 1 ]; do
	if [ -f core ]; then rm core; fi
	if [ -f $LOGFILE ]; then rm $LOGFILE; fi
	
	# Run the test.
	./chaoticrage --no-mouse-grab --campaign=pound --mod=test $@ 2>&1 | tee $LOGFILE
	
	# If it coredumped, save the file somewhere useful
	if [ -f core ]; then
		mkdir -p tools/multipound/coredumps
		mv core tools/multipound/coredumps/core_`date +"%Y-%m-%d_%H-%M-%s"`
	fi
	
	# If it coredumped, save the file somewhere useful
	if [ -f $LOGFILE ]; then
		mkdir -p tools/multipound/logs
		mv $LOGFILE tools/multipound/logs/log_`date +"%Y-%m-%d_%H-%M-%s"`
	fi
	
	# Wait a moment to give the user a chance to quit.
	echo "Test finished. Waiting 5 secs. Press CTRL+C to exit."
	sleep 5
done

if [ -f $LOGFILE ]; then rm $LOGFILE; fi

