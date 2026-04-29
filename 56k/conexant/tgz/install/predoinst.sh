#!/bin/bash
if [ "$1" = 1 ]; then
	if [ -e "/etc/hsfmodem" ]; then
		echo "Removing old /etc/hsfmodem"
		rm -rf "/etc/hsfmodem"
	fi
	if [ -e "/usr/lib/hsfmodem" ]; then
		echo "Removing old /usr/lib/hsfmodem"
		rm -rf "/usr/lib/hsfmodem"
	fi
fi
exit 0