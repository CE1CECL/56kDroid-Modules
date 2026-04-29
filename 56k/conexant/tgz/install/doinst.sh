#!/bin/bash
#%if ! 1
RIP_AUTOCONFIG=true
#%endif
if [ -n "${RIP_AUTOCONFIG}" -a -z "${RIP_NOAUTOCONFIG}" ]; then
	/usr/sbin/ripconfig --auto
	exit $?
else
	echo "To complete the installation and configuration of your RIPTIDE board,"
	echo "please run \"ripconfig\" (or \"/usr/sbin/ripconfig\")"
	exit 0
fi