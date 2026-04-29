#!/bin/bash
if [ "$1" = 0 -a -z "${RIP_NOAUTOCONFIG}" ]; then
	/usr/sbin/ripconfig --remove
else
	exit 0
fi

# This must be last since the file CHANGES is automatically appended