#!/bin/bash
if [ "$1" = 1 ]; then
	if [ -e "/usr/lib/riptide" ]; then
		echo "Removing old /usr/lib/riptide"
			rm -rf "/usr/lib/riptide"
	else
		exit 0
	fi
fi