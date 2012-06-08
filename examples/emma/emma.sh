#!/bin/bash
ACTION=$1
TARGET=minimal-net
NB_INSTANCES=1

BRIDGE="br1";

BIN=bin
LOG=log
BUILD_PATH=builds
SRC_PATH=src

NODE=emma-node
ROUTER=$NODE-border
SHELL=$NODE-shell
#########################################################################
# Builder
#########################################################################
function _build {
	if [ ! -d $BUILD_PATH ]
		then
		mkdir $BUILD_PATH
	fi
	cd $SRC_PATH 
	
	# Build router node		
	if test -e $1 || [ "$1" = "$ROUTER" ]
		then
			echo "Build border router node"
			if test -e $1; then
				../$0 clean
			fi
			make $ROUTER TARGET=$TARGET BORDER=1
			mv $ROUTER.$TARGET ../$BUILD_PATH/$ROUTER.$TARGET
	fi	
		
	# Build shell node
	if test -e $1 || [ "$1" = "$SHELL" ]
		then
			echo "Build shell node"
			if test -e $1; then
				../$0 clean
			fi
			make $SHELL TARGET=$TARGET EMMA_SHELL=1 NODE_ID=1
			mv $SHELL.$TARGET ../$BUILD_PATH/$SHELL.$TARGET
	fi
	
	# Build N nodes
	if test -e $1 || [ "$1" = "$NODE" ]
		then
			echo "Build process of $NB_INSTANCES instance(s)" 
			for ((i=2; i<$NB_INSTANCES+2;i++))
				do
					echo "Build instance $i"
					../$0 clean
					make $NODE TARGET=$TARGET NODE_ID=$i
					mv $NODE.$TARGET ../$BUILD_PATH/$NODE.$TARGET.$i
				done
		fi
}
#########################################################################
# Clean
#########################################################################
function _clean {
		echo "Clean"
		cd $SRC_PATH
		make clean
		rm -rf obj*
		cd ..
}
#########################################################################
# Clean
#########################################################################
function _clean_force {
		_clean
		rm $BUILD_PATH/*
}
#########################################################################
# Runner
#########################################################################
function _run {
	if [ ! -d $LOG ]
		then
		mkdir $LOG
	fi

	if test -e $1 || [ "$1" = "$ROUTER" ]
		then
			echo "Run border router node"
			./$BUILD_PATH/$ROUTER.$TARGET >> $LOG/$ROUTER.$TARGET.log &
	fi
	
	if test -e $1 || [ "$1" = "$NODE" ]
		then
			cd $BUILD_PATH
			for instance in ./$NODE.$TARGET.*;
				do
					echo "Run $instance"
					./$instance >> ../$LOG/$instance.log &
				done
			cd ..
	fi
		
	if test -e $1 || [ "$1" = "$SHELL" ]
		then
			echo "Run shell node"
			./$BUILD_PATH/$SHELL.$TARGET
	fi
	
}
#########################################################################
# Kill
#########################################################################
function _killall {
		pkill $NODE*
}
#########################################################################
# Network
#########################################################################
function _network {
		if [ "$1" = "del" ] 
			then
			echo "Delete bridge $BRIDGE";
			ifconfig $BRIDGE down
			brctl delbr $BRIDGE
	
		elif [ "$1" = "add" ] 
			then
			echo "Add bridge $BRIDGE";
			brctl addbr $BRIDGE
			ip tuntap add dev $BRIDGE mode tap
			ifconfig $BRIDGE up
	
		elif [ "$1" = "run" ]
			then
			echo "Configure bridge $BRIDGE";
			TAPS=`ifconfig -a | awk '/tap/ { print $0 }' | sed -e 's/ /\n/' | sed -e '/ .*/d'`
			for tap in $TAPS
				do
					brctl addif $BRIDGE $tap
				done
	
		elif [ "$1" = "show" ]
			then
			brctl show
	
		else 
			echo "make_bridge [add | del | run | show]"
		fi
}

#avrdude -c arduino -p atmega128rfa1 -P /dev/ttyUSB0 -b 57600 -U flash:w:test_atmega128rfa1.hex
#########################################################################
function _usage {
echo "USAGE:"
echo "emma.sh   build   [[target]]"
echo "          run [[target]]"
echo "          net [add | del | run | show]"
echo "          killall"
echo "          clean"
echo "          cleanall"
}

case $ACTION in 

	build)
	_build $2
	;; 
 
	run) 
	_run $2
	;; 
	
	net)
	_network $2
	;;
	
	killall)
	_killall
	;; 
	
	clean)
	_clean
	;;
	
	cleanall)
	_clean_force
	;;
	
	*)
		_usage $2

	;;
esac




