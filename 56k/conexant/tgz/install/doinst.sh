#!/bin/bash

if [ -f /etc/hsfmodem/nvm.tar.gz ]; then
	( cd /etc/hsfmodem && tar xzf nvm.tar.gz )
fi

#%if ! 1
CNXT_AUTOCONFIG=true
#%endif
if [ -n "${CNXT_AUTOCONFIG}" -a -z "${CNXT_NOAUTOCONFIG}" ]; then
	/usr/sbin/hsfconfig --auto
	ret=$?
	if [ "${ret}" -eq 123 ]; then
		ret=0
	fi
	if [ "${ret}" -eq 124 -a -n "${CNXT_NOWRONGKERNELFAIL}" ]; then
		ret=0
	fi
	exit ${ret}
else
	echo "To complete the installation and configuration of your modem,"
	echo "please run \"hsfconfig\" (or \"/usr/sbin/hsfconfig\")"
	exit 0
fi