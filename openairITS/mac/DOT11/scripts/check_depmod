#!/bin/bash
# Copyright 2009	Luis R. Rodriguez <mcgrof@gmail.com>
# 
# Ensures your distribution likes to prefer updates/ over the kernel/
# search updates built-in

# Seems Mandriva has an $DEPMOD_DIR but it doesn't have any files,
# so lets deal with those distributions.
DEPMOD_CONF="/etc/depmod.conf"
DEPMOD_CONF_TMP="$DEPMOD_CONF.compat-wireless.old"
DEPMOD_DIR="/etc/depmod.d/"
COMPAT_DEPMOD_FILE=compat-wireless.conf
GREP_REGEX_UPDATES="^[[:space:]]*search.*[[:space:]]updates\([[:space:]]\|$\)"
GREP_REGEX_SEARCH="^[[:space:]]*search[[:space:]].\+$"
DEPMOD_CMD="depmod"

function add_compat_depmod_conf {
	echo "NOTE: Your distribution lacks an $DEPMOD_DIR directory with "
	echo "updates/ directory being prioritized for modules, we're adding "
	echo "one for you."
	mkdir -p $DEPMOD_DIR
	FIRST_FILE=$(ls $DEPMOD_DIR|head -1)
	[ -n "$FIRST_FILE" ] && while [[ $FIRST_FILE < $COMPAT_DEPMOD_FILE ]]; do
		COMPAT_DEPMOD_FILE="0$COMPAT_DEPMOD_FILE"
	done
	echo "search updates" > $DEPMOD_DIR/$COMPAT_DEPMOD_FILE
}

function add_global_depmod_conf {
	echo "NOTE: Your distribution lacks updates/ directory being"
 	echo "prioritized for modules, we're adding it to $DEPMOD_CONF."
	rm -f $DEPMOD_CONF_TMP
	[ -f $DEPMOD_CONF ] && cp -f $DEPMOD_CONF $DEPMOD_CONF_TMP
	echo "search updates" > $DEPMOD_CONF
	[ -f $DEPMOD_CONF_TMP ] && cat $DEPMOD_CONF_TMP >> $DEPMOD_CONF
}

function depmod_updates_ok {
	echo "depmod will prefer updates/ over kernel/ -- OK!"
}

function add_depmod_conf {
	if [ -f "$DEPMOD_CONF" ]; then
		add_global_depmod_conf
	else
		DEPMOD_VERSION=$($DEPMOD_CMD --version | cut -d" " -f2 | sed "s/\.//")
		if [[ $DEPMOD_VERSION -gt 36 ]]; then
			add_compat_depmod_conf
		else
			add_global_depmod_conf
		fi
	fi
}

# =============================================================================
# === MAIN ====================================================================
# =============================================================================

GREP_FILES=""
[ -f $DEPMOD_CONF ] && GREP_FILES="$DEPMOD_CONF"
if [ -d $DEPMOD_DIR ]; then
	DEPMOD_FILE_COUNT=$(ls $DEPMOD_DIR | wc -l)
	[[ $DEPMOD_FILE_COUNT -gt 0 ]] && GREP_FILES="$GREP_FILES $DEPMOD_DIR/*"
fi

if [ -n "$GREP_FILES" ]; then
	grep -q "$GREP_REGEX_SEARCH" $GREP_FILES
	if [[ $? -eq 0 ]]; then
		grep -q "$GREP_REGEX_UPDATES" $GREP_FILES
		if [[ $? -eq 0 ]]; then
			depmod_updates_ok
		else
			add_depmod_conf
		fi
	else
		depmod_updates_ok
	fi
else
	depmod_updates_ok
fi

exit 0

