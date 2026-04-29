#!/bin/bash
/usr/sbin/hsfstop
if [ "$1" = 0 ]; then

	if [ -z "${CNXT_NOAUTOCONFIG}" ]; then
		/usr/sbin/hsfconfig --remove
	fi

	if [ -f /etc/hsfmodem/nvm.tar.gz ]; then
		( cd /etc/hsfmodem && rm -rf `tar tzf /etc/hsfmodem/nvm.tar.gz | egrep '^nvm/[^/]+/?$'` )
	fi
else
	exit 0
fi


# This must be last since the file CHANGES is automatically appended