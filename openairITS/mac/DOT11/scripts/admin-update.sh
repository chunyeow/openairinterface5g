#!/usr/bin/env bash
# 
# Copyright 2007, 2008, 2010	Luis R. Rodriguez <mcgrof@winlab.rutgers.edu>
#
# Use this to update compat-wireless-2.6 to the latest
# wireless-testing.git tree you have.
#
# Usage: you should have the latest pull of wireless-2.6.git
# git://git.kernel.org/pub/scm/linux/kernel/git/linville/wireless-testing.git
# We assume you have it on your ~/devel/wireless-testing/ directory. If you do,
# just run this script from the compat-wireless-2.6 directory.
# You can specify where your GIT_TREE is by doing:
#
# export GIT_TREE=/home/mcgrof/wireless-testing/
# 
# for example
#
GIT_URL="git://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git"
GIT_COMPAT_URL="git://git.kernel.org/pub/scm/linux/kernel/git/mcgrof/compat.git"

INCLUDE_NET_BT="hci_core.h l2cap.h bluetooth.h rfcomm.h hci.h mgmt.h smp.h"
NET_BT_DIRS="bluetooth bluetooth/bnep bluetooth/cmtp bluetooth/rfcomm bluetooth/hidp"

INCLUDE_LINUX="ieee80211.h nl80211.h"
INCLUDE_LINUX="$INCLUDE_LINUX pci_ids.h eeprom_93cx6.h"
INCLUDE_LINUX="$INCLUDE_LINUX ath9k_platform.h"
INCLUDE_LINUX="$INCLUDE_LINUX wl12xx.h"

# For rndis_wext
INCLUDE_LINUX_USB="usbnet.h rndis_host.h"

INCLUDE_LINUX_SPI="libertas_spi.h"

# The good new yummy stuff
INCLUDE_NET="cfg80211.h ieee80211_radiotap.h cfg80211-wext.h"
INCLUDE_NET="$INCLUDE_NET mac80211.h lib80211.h regulatory.h"

# Pretty colors
GREEN="\033[01;32m"
YELLOW="\033[01;33m"
NORMAL="\033[00m"
BLUE="\033[34m"
RED="\033[31m"
PURPLE="\033[35m"
CYAN="\033[36m"
UNDERLINE="\033[02m"

NET_DIRS="wireless mac80211 rfkill"
CODE_METRICS=code-metrics.txt

usage() {
	printf "Usage: $0 [ refresh] [ --help | -h | -s | -n | -p | -c ]\n"

	printf "${GREEN}%10s${NORMAL} - will update your all your patch offsets using quilt\n" "refresh"
	printf "${GREEN}%10s${NORMAL} - get and apply pending-stable/ fixes purging old files there\n" "-s"
	printf "${GREEN}%10s${NORMAL} - apply the patches linux-next-cherry-picks directory\n" "-n"
	printf "${GREEN}%10s${NORMAL} - apply the patches on the linux-next-pending directory\n" "-p"
	printf "${GREEN}%10s${NORMAL} - apply the patches on the crap directory\n" "-c"
}

brag_backport() {
	COMPAT_FILES_CODE=$(find ./ -type f -name  \*.[ch] | egrep  "^./compat/|include/linux/compat" |
		xargs wc -l | tail -1 | awk '{print $1}')
	let COMPAT_ALL_CHANGES=$2+$COMPAT_FILES_CODE
	printf "${GREEN}%10s${NORMAL} - backport code changes\n" $2
	printf "${GREEN}%10s${NORMAL} - backport code additions\n" $3
	printf "${GREEN}%10s${NORMAL} - backport code deletions\n" $4
	printf "${GREEN}%10s${NORMAL} - backport from compat module\n" $COMPAT_FILES_CODE
	printf "${GREEN}%10s${NORMAL} - total backport code\n" $COMPAT_ALL_CHANGES
	printf "${RED}%10s${NORMAL} - %% of code consists of backport work\n" \
		$(perl -e 'printf("%.4f", 100 * '$COMPAT_ALL_CHANGES' / '$1');')
}

nag_pending_stable() {
	printf "${YELLOW}%10s${NORMAL} - Code changes brought in from pending-stable\n" $2
	printf "${YELLOW}%10s${NORMAL} - Code additions brought in from pending-stable\n" $3
	printf "${YELLOW}%10s${NORMAL} - Code deletions brought in from pending-stable\n" $4
	printf "${RED}%10s${NORMAL} - %% of code being cherry picked from pending-stable\n" $(perl -e 'printf("%.4f", 100 * '$2' / '$1');')
}

nag_next_cherry_pick() {
	printf "${YELLOW}%10s${NORMAL} - Code changes brought in from linux-next\n" $2
	printf "${YELLOW}%10s${NORMAL} - Code additions brought in from linux-next\n" $3
	printf "${YELLOW}%10s${NORMAL} - Code deletions brought in from linux-next\n" $4
	printf "${RED}%10s${NORMAL} - %% of code being cherry picked from linux-next\n" $(perl -e 'printf("%.4f", 100 * '$2' / '$1');')
}

nag_pending() {
	printf "${YELLOW}%10s${NORMAL} - Code changes posted but not yet merged\n" $2
	printf "${YELLOW}%10s${NORMAL} - Code additions posted but not yet merged\n" $3
	printf "${YELLOW}%10s${NORMAL} - Code deletions posted but not yet merged\n" $4
	printf "${RED}%10s${NORMAL} - %% of code not yet merged\n" $(perl -e 'printf("%.4f", 100 * '$2' / '$1');')
}

nag_crap() {
	printf "${RED}%10s${NORMAL} - Crap changes not yet posted\n" $2
	printf "${RED}%10s${NORMAL} - Crap additions not yet posted\n" $3
	printf "${RED}%10s${NORMAL} - Crap deletions not yet posted\n" $4
	printf "${RED}%10s${NORMAL} - %% of crap code\n" $(perl -e 'printf("%.4f", 100 * '$2' / '$1');')
}

nagometer() {
	CHANGES=0

	ORIG_CODE=$2
	ADD=$(grep -Hc ^+ $1/*.patch| awk -F":" 'BEGIN {sum=0} {sum += $2} END { print sum}')
	DEL=$(grep -Hc ^- $1/*.patch| awk -F":" 'BEGIN {sum=0} {sum += $2} END { print sum}')
	# Total code is irrelevant unless you take into account each part,
	# easier to just compare against the original code.
	# let TOTAL_CODE=$ORIG_CODE+$ADD-$DEL

	let CHANGES=$ADD+$DEL

	case $1 in
	"patches")
		brag_backport $ORIG_CODE $CHANGES $ADD $DEL
		;;
	"pending-stable")
		nag_pending_stable $ORIG_CODE $CHANGES $ADD $DEL
		;;
	"linux-next-cherry-picks")
		nag_next_cherry_pick $ORIG_CODE $CHANGES $ADD $DEL
		;;
	"linux-next-pending")
		nag_pending $ORIG_CODE $CHANGES $ADD $DEL
		;;
	"crap")
		nag_crap $ORIG_CODE $CHANGES $ADD $DEL
		;;
	*)
		;;
	esac

}

EXTRA_PATCHES="patches"
REFRESH="n"
GET_STABLE_PENDING="n"
POSTFIX_RELEASE_TAG=""
if [ $# -ge 1 ]; then
	if [ $# -gt 4 ]; then
		usage $0
		exit
	fi
	if [[ $1 = "-h" || $1 = "--help" ]]; then
		usage $0
		exit
	fi
	while [ $# -ne 0 ]; do
		if [[ "$1" = "-s" ]]; then
			GET_STABLE_PENDING="y"
			EXTRA_PATCHES="${EXTRA_PATCHES} pending-stable" 
			EXTRA_PATCHES="${EXTRA_PATCHES} pending-stable/backports/"
			POSTFIX_RELEASE_TAG="${POSTFIX_RELEASE_TAG}s"
			shift; continue;
		fi
		if [[ "$1" = "-n" ]]; then
			EXTRA_PATCHES="${EXTRA_PATCHES} linux-next-cherry-picks"
			POSTFIX_RELEASE_TAG="${POSTFIX_RELEASE_TAG}n"
			shift; continue;
		fi
		if [[ "$1" = "-p" ]]; then
			EXTRA_PATCHES="${EXTRA_PATCHES} linux-next-pending"
			POSTFIX_RELEASE_TAG="${POSTFIX_RELEASE_TAG}p"
			shift; continue;
		fi
		if [[ "$1" = "-c" ]]; then
			EXTRA_PATCHES="${EXTRA_PATCHES} crap"
			POSTFIX_RELEASE_TAG="${POSTFIX_RELEASE_TAG}c"
			shift; continue;
		fi
		if [[ "$1" = "refresh" ]]; then
			REFRESH="y"
			shift; continue;
		fi

		echo "Unexpected argument passed: $1"
		usage $0
		exit
	done

fi

# User exported this variable
if [ -z $GIT_TREE ]; then
	GIT_TREE="/home/$USER/linux-next/"
	if [ ! -d $GIT_TREE ]; then
		echo "Please tell me where your linux-next git tree is."
		echo "You can do this by exporting its location as follows:"
		echo
		echo "  export GIT_TREE=/home/$USER/linux-next/"
		echo
		echo "If you do not have one you can clone the repository:"
		echo "  git clone $GIT_URL"
		exit 1
	fi
else
	echo "You said to use git tree at: $GIT_TREE for linux-next"
fi

if [ -z $GIT_COMPAT_TREE ]; then
	GIT_COMPAT_TREE="/home/$USER/compat/"
	if [ ! -d $GIT_COMPAT_TREE ]; then
		echo "Please tell me where your compat git tree is."
		echo "You can do this by exporting its location as follows:"
		echo
		echo "  export GIT_COMPAT_TREE=/home/$USER/compat/"
		echo
		echo "If you do not have one you can clone the repository:"
		echo "  git clone $GIT_COMPAT_URL"
		exit 1
	fi
else
	echo "You said to use git tree at: $GIT_COMPAT_TREE for compat"
fi

# Drivers that have their own directory
DRIVERS="drivers/net/wireless/ath"
DRIVERS="$DRIVERS drivers/net/wireless/ath/carl9170"
DRIVERS="$DRIVERS drivers/net/wireless/ath/ath5k"
DRIVERS="$DRIVERS drivers/net/wireless/ath/ath6kl"
DRIVERS="$DRIVERS drivers/net/wireless/ath/ath9k"
DRIVERS="$DRIVERS drivers/ssb"
DRIVERS="$DRIVERS drivers/bcma"
DRIVERS="$DRIVERS drivers/net/wireless/b43"
DRIVERS="$DRIVERS drivers/net/wireless/b43legacy"
DRIVERS="$DRIVERS drivers/net/wireless/brcm80211"
DRIVERS="$DRIVERS drivers/net/wireless/brcm80211/brcmfmac"
DRIVERS="$DRIVERS drivers/net/wireless/brcm80211/brcmsmac"
DRIVERS="$DRIVERS drivers/net/wireless/brcm80211/brcmsmac/phy"
DRIVERS="$DRIVERS drivers/net/wireless/brcm80211/brcmutil"
DRIVERS="$DRIVERS drivers/net/wireless/brcm80211/include"
DRIVERS="$DRIVERS drivers/net/wireless/iwlegacy"
DRIVERS="$DRIVERS drivers/net/wireless/iwlwifi"
DRIVERS="$DRIVERS drivers/net/wireless/rt2x00"
DRIVERS="$DRIVERS drivers/net/wireless/zd1211rw"
DRIVERS="$DRIVERS drivers/net/wireless/libertas"
DRIVERS="$DRIVERS drivers/net/wireless/p54"
DRIVERS="$DRIVERS drivers/net/wireless/rtl818x"
DRIVERS="$DRIVERS drivers/net/wireless/rtl818x/rtl8180"
DRIVERS="$DRIVERS drivers/net/wireless/rtl818x/rtl8187"
DRIVERS="$DRIVERS drivers/net/wireless/rtlwifi"
DRIVERS="$DRIVERS drivers/net/wireless/rtlwifi/rtl8192c"
DRIVERS="$DRIVERS drivers/net/wireless/rtlwifi/rtl8192ce"
DRIVERS="$DRIVERS drivers/net/wireless/rtlwifi/rtl8192cu"
DRIVERS="$DRIVERS drivers/net/wireless/rtlwifi/rtl8192se"
DRIVERS="$DRIVERS drivers/net/wireless/rtlwifi/rtl8192de"
DRIVERS="$DRIVERS drivers/net/wireless/libertas_tf"
DRIVERS="$DRIVERS drivers/net/wireless/ipw2x00"
DRIVERS="$DRIVERS drivers/net/wireless/wl12xx"
DRIVERS="$DRIVERS drivers/net/wireless/wl1251"
DRIVERS="$DRIVERS drivers/net/wireless/iwmc3200wifi"
DRIVERS="$DRIVERS drivers/net/wireless/orinoco"
DRIVERS="$DRIVERS drivers/net/wireless/mwifiex"

# Staging drivers
STAGING_DRIVERS=""

# Ethernet drivers
DRIVERS="$DRIVERS drivers/net/ethernet/atheros"
DRIVERS="$DRIVERS drivers/net/ethernet/atheros/atl1c"
DRIVERS="$DRIVERS drivers/net/ethernet/atheros/atl1e"
DRIVERS="$DRIVERS drivers/net/ethernet/atheros/atlx"
DRIVERS="$DRIVERS drivers/net/ethernet/atheros/alx"

# Bluetooth drivers
DRIVERS_BT="drivers/bluetooth"

# Drivers that belong the the wireless directory
DRIVER_FILES="adm8211.c  adm8211.h"
DRIVER_FILES="$DRIVER_FILES rndis_wlan.c"
DRIVER_FILES="$DRIVER_FILES mac80211_hwsim.c mac80211_hwsim.h"
DRIVER_FILES="$DRIVER_FILES at76c50x-usb.c at76c50x-usb.h"
DRIVER_FILES="$DRIVER_FILES mwl8k.c"

rm -rf drivers/

mkdir -p include/linux/ include/net/ include/linux/usb \
	include/linux/unaligned \
	include/linux/spi \
	include/trace \
	include/pcmcia \
	include/crypto \
	net/mac80211/ net/wireless/ \
	net/rfkill/ \
	drivers/ssb/ \
	drivers/bcma/ \
	drivers/net/usb/ \
	drivers/net/wireless/ \
	drivers/net/ethernet/atheros \
	drivers/net/ethernet/broadcom
mkdir -p include/net/bluetooth/

# include/linux
DIR="include/linux"
for i in $INCLUDE_LINUX; do
	echo "Copying $GIT_TREE/$DIR/$i"
	cp "$GIT_TREE/$DIR/$i" $DIR/
done

cp -a $GIT_TREE/include/linux/ssb include/linux/
cp -a $GIT_TREE/include/linux/bcma include/linux/
cp -a $GIT_TREE/include/linux/rfkill.h include/linux/rfkill_backport.h

# include/net
DIR="include/net"
for i in $INCLUDE_NET; do
	echo "Copying $GIT_TREE/$DIR/$i"
	cp "$GIT_TREE/$DIR/$i" $DIR/
done

DIR="include/net/bluetooth"
for i in $INCLUDE_NET_BT; do
  echo "Copying $GIT_TREE/$DIR/$i"
  cp $GIT_TREE/$DIR/$i $DIR/
done

DIR="include/linux/usb"
for i in $INCLUDE_LINUX_USB; do
	echo "Copying $GIT_TREE/$DIR/$i"
	cp $GIT_TREE/$DIR/$i $DIR/
done

DIR="include/linux/spi"
for i in $INCLUDE_LINUX_SPI; do
	echo "Copying $GIT_TREE/$DIR/$i"
	cp $GIT_TREE/$DIR/$i $DIR/
done

# net/wireless and net/mac80211
for i in $NET_DIRS; do
	echo "Copying $GIT_TREE/net/$i/*.[ch]"
	cp $GIT_TREE/net/$i/*.[ch] net/$i/
	cp $GIT_TREE/net/$i/Makefile net/$i/
	rm -f net/$i/*.mod.c
done

cp $GIT_TREE/MAINTAINERS ./

# Copy files needed for statically compiled regulatory rules database
cp $GIT_TREE/net/wireless/db.txt net/wireless/
cp $GIT_TREE/net/wireless/genregdb.awk net/wireless/

# net/bluetooth
for i in $NET_BT_DIRS; do
	mkdir -p net/$i
	echo "Copying $GIT_TREE/net/$i/*.[ch]"
	cp $GIT_TREE/net/$i/*.[ch] net/$i/
	cp $GIT_TREE/net/$i/Makefile net/$i/
	rm -f net/$i/*.mod.c
done

# Drivers in their own directory
for i in $DRIVERS; do
	mkdir -p $i

	# -print -quit will just print once, so we don't burden
	# this script with searching for further files if one is
	# found
	FILES_FOUND=$(find $GIT_TREE/$i/ -maxdepth 1 -type f -name \*.[ch] -print -quit | wc -l)
	if [ $FILES_FOUND -eq 1 ]; then
		echo "Copying $GIT_TREE/$i/*.[ch]"
		cp $GIT_TREE/$i/*.[ch] $i/
	fi
	if [ -f $GIT_TREE/$i/Makefile ]; then
		cp $GIT_TREE/$i/Makefile $i/
	fi
	if [ -f $GIT_TREE/$i/Kconfig ]; then
		cp $GIT_TREE/$i/Kconfig $i/
	fi
	rm -f $i/*.mod.c
done

# Staging drivers in their own directory
rm -rf drivers/staging/
mkdir -p drivers/staging/
for i in $STAGING_DRIVERS; do
	if [ ! -d $GIT_TREE/$i ]; then
		continue
	fi
	rm -rf $i
	echo -e "Copying ${RED}STAGING${NORMAL} $GIT_TREE/$i/*.[ch]"
	# staging drivers tend to have their own subdirs...
	cp -a $GIT_TREE/$i drivers/staging/
	rm -f $i/*.mod.c
done

for i in $DRIVERS_BT; do
	mkdir -p $i
	echo "Copying $GIT_TREE/$i/*.[ch]"
	cp $GIT_TREE/$i/*.[ch] $i/
	cp $GIT_TREE/$i/Makefile $i/
	rm -f $i/*.mod.c
done

# For rndis_wlan, we need a new rndis_host.ko, cdc_ether.ko and usbnet.ko
RNDIS_REQS="Makefile rndis_host.c cdc_ether.c usbnet.c"
DIR="drivers/net/usb"
for i in $RNDIS_REQS; do
	echo "Copying $GIT_TREE/$DIR/$i"
	cp $GIT_TREE/$DIR/$i $DIR/
done

DIR="drivers/net/ethernet/broadcom"
echo > $DIR/Makefile
cp $GIT_TREE/$DIR/b44.[ch] $DIR
# Not yet
echo "obj-\$(CONFIG_B44) += b44.o" >> $DIR/Makefile

# Misc
mkdir -p drivers/misc/eeprom/
cp $GIT_TREE/drivers/misc/eeprom/eeprom_93cx6.c drivers/misc/eeprom/
cp $GIT_TREE/drivers/misc/eeprom/Makefile drivers/misc/eeprom/

DIR="drivers/net/wireless"
# Drivers part of the wireless directory
for i in $DRIVER_FILES; do
	cp $GIT_TREE/$DIR/$i $DIR/
done

# Top level wireless driver Makefile
cp $GIT_TREE/$DIR/Makefile $DIR

# Compat stuff
COMPAT="compat"
mkdir -p $COMPAT
echo "Copying $GIT_COMPAT_TREE/ files..."
cp $GIT_COMPAT_TREE/compat/*.c $COMPAT/
cp $GIT_COMPAT_TREE/compat/Makefile $COMPAT/
cp -a $GIT_COMPAT_TREE/udev .
cp -a $GIT_COMPAT_TREE/scripts $COMPAT/
cp -a $GIT_COMPAT_TREE/include/linux/* include/linux/
cp -a $GIT_COMPAT_TREE/include/net/* include/net/
cp -a $GIT_COMPAT_TREE/include/trace/* include/trace/
cp -a $GIT_COMPAT_TREE/include/pcmcia/* include/pcmcia/
cp -a $GIT_COMPAT_TREE/include/crypto/* include/crypto/
rm -f $COMPAT/*.mod.c

# files we suck in for compat-wireless
export WSTABLE="
        net/wireless/
        net/wireless/
        net/mac80211/
        net/rfkill/
        drivers/net/wireless/
        net/bluetooth/
        drivers/bluetooth/
        drivers/net/atl1c/
        drivers/net/atl1e/
        drivers/net/atlx/
        include/linux/nl80211.h
        include/linux/rfkill.h
        include/net/cfg80211.h
	include/net/mac80211.h
        include/net/regulatory.h
        include/net/cfg80211.h"

# Stable pending, if -n was passed
if [[ "$GET_STABLE_PENDING" = y ]]; then

	if [ -z $NEXT_TREE ]; then
		NEXT_TREE="/home/$USER/linux-next/"
		if [ ! -d $NEXT_TREE ]; then
			echo "Please tell me where your linux-next git tree is."
			echo "You can do this by exporting its location as follows:"
			echo
			echo "  export NEXT_TREE=/home/$USER/linux-next/"
			echo
			echo "If you do not have one you can clone the repository:"
			echo "  git clone git://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git"
			exit 1
		fi
	else
		echo "You said to use git tree at: $NEXT_TREE for linux-next"
	fi

	LAST_DIR=$PWD
	cd $GIT_TREE
	if [ -f localversion* ]; then
		echo -e "You should be using a stable tree to use the -s option"
		exit 1
	fi

	# we now assume you are using a stable tree
	cd $GIT_TREE
	LAST_STABLE_UPDATE=$(git describe --abbrev=0)
	cd $NEXT_TREE
	PENDING_STABLE_DIR="pending-stable/"

	rm -rf $PENDING_STABLE_DIR

	git tag -l | grep $LAST_STABLE_UPDATE 2>&1 > /dev/null
	if [[ $? -ne 0 ]]; then
		echo -e "${BLUE}Tag $LAST_STABLE_UPDATE not found on $NEXT_TREE tree: bailing out${NORMAL}"
		exit 1
	fi
	echo -e "${GREEN}Generating stable cherry picks... ${NORMAL}"
	echo -e "\nUsing command on directory $PWD:"
	echo -e "\ngit format-patch --grep=\"stable@kernel.org\" -o $PENDING_STABLE_DIR ${LAST_STABLE_UPDATE}.. $WSTABLE"
	git format-patch --grep="stable@kernel.org" -o $PENDING_STABLE_DIR ${LAST_STABLE_UPDATE}.. $WSTABLE
	if [ ! -d ${LAST_DIR}/${PENDING_STABLE_DIR} ]; then
		echo -e "Assumption that ${LAST_DIR}/${PENDING_STABLE_DIR} directory exists failed"
		exit 1
	fi
	echo -e "${GREEN}Purging old stable cherry picks... ${NORMAL}"
	rm -f ${LAST_DIR}/${PENDING_STABLE_DIR}/*.patch
	cp ${PENDING_STABLE_DIR}/*.patch ${LAST_DIR}/${PENDING_STABLE_DIR}/
	if [ -f ${LAST_DIR}/${PENDING_STABLE_DIR}/.ignore ]; then
		for i in $(cat ${LAST_DIR}/${PENDING_STABLE_DIR}/.ignore) ; do
			echo -e "Skipping $i from generated stable patches..."
			rm -f ${LAST_DIR}/${PENDING_STABLE_DIR}/*$i*
		done
	fi
	echo -e "${GREEN}Updated stable cherry picks, review with git diff and update hunks with ./scripts/admin-update.sh -s refresh${NORMAL}"
	cd $LAST_DIR
fi

# Refresh patches using quilt
patchRefresh() {
	if [ -d patches.orig ] ; then
		rm -rf .pc patches/series
	else
		mkdir patches.orig
	fi

	export QUILT_PATCHES=$1

	mv -u $1/* patches.orig/

	for i in patches.orig/*.patch; do
		if [ ! -f "$i" ]; then
			echo -e "${RED}No patches found in $1${NORMAL}"
			break;
		fi
		echo -e "${GREEN}Refresh backport patch${NORMAL}: ${BLUE}$i${NORMAL}"
		quilt import $i
		quilt push -f
		RET=$?
		if [[ $RET -ne 0 ]]; then
			echo -e "${RED}Refreshing $i failed${NORMAL}, update it"
			echo -e "use ${CYAN}quilt edit [filename]${NORMAL} to apply the failed part manually"
			echo -e "use ${CYAN}quilt refresh${NORMAL} after the files are corrected and rerun this script"
			cp patches.orig/README $1/README
			exit $RET
		fi
		QUILT_DIFF_OPTS="-p" quilt refresh -p ab --no-index --no-timestamp
	done
	quilt pop -a

	cp patches.orig/README $1/README
	rm -rf patches.orig .pc $1/series
}

ORIG_CODE=$(find ./ -type f -name  \*.[ch] |
	egrep -v "^./compat/|include/linux/compat" |
	xargs wc -l | tail -1 | awk '{print $1}')
printf "\n${CYAN}compat-wireless code metrics${NORMAL}\n\n" > $CODE_METRICS
printf "${PURPLE}%10s${NORMAL} - Total upstream lines of code being pulled\n" $ORIG_CODE >> $CODE_METRICS

for dir in $EXTRA_PATCHES; do
	LAST_ELEM=$dir
done

for dir in $EXTRA_PATCHES; do
	if [[ ! -d $dir ]]; then
		echo -e "${RED}Patches: $dir empty, skipping...${NORMAL}\n"
		continue
	fi
	if [[ $LAST_ELEM = $dir && "$REFRESH" = y ]]; then
		patchRefresh $dir
	fi

	FOUND=$(find $dir/ -maxdepth 1 -name \*.patch | wc -l)
	if [ $FOUND -eq 0 ]; then
		continue
	fi
	for i in $dir/*.patch; do
		echo -e "${GREEN}Applying backport patch${NORMAL}: ${BLUE}$i${NORMAL}"
		patch -p1 -N -t < $i
		RET=$?
		if [[ $RET -ne 0 ]]; then
			echo -e "${RED}Patching $i failed${NORMAL}, update it"
			exit $RET
		fi
	done
	nagometer $dir $ORIG_CODE >> $CODE_METRICS
done

DIR="$PWD"
cd $GIT_TREE
GIT_DESCRIBE=$(git describe)
GIT_BRANCH=$(git branch --no-color |sed -n 's/^\* //p')
GIT_BRANCH=${GIT_BRANCH:-master}
GIT_REMOTE=$(git config branch.${GIT_BRANCH}.remote)
GIT_REMOTE=${GIT_REMOTE:-origin}
GIT_REMOTE_URL=$(git config remote.${GIT_REMOTE}.url)
GIT_REMOTE_URL=${GIT_REMOTE_URL:-unknown}
echo -e "${GREEN}Updated${NORMAL} from local tree: ${BLUE}${GIT_TREE}${NORMAL}"
echo -e "Origin remote URL: ${CYAN}${GIT_REMOTE_URL}${NORMAL}"
cd $DIR
if [ -d ./.git ]; then
	if [[ ${POSTFIX_RELEASE_TAG} != "" ]]; then
		echo -e "$(git describe)-${POSTFIX_RELEASE_TAG}" > compat_version
	else
		echo -e "$(git describe)" > compat_version
	fi

	cd $GIT_TREE
	TREE_NAME=${GIT_REMOTE_URL##*/}

	echo $TREE_NAME > $DIR/compat_base_tree
	echo $GIT_DESCRIBE > $DIR/compat_base_tree_version

	case $TREE_NAME in
	"wireless-testing.git") # John's wireless-testing
		# We override the compat_base_tree_version for wireless-testing
		# as john keeps the Linus' tags and does not write a tag for his
		# tree himself so git describe would yield a v2.6.3x.y-etc but
		# what is more useful is just the wireless-testing master tag
		MASTER_TAG=$(git tag -l| grep master | tail -1)
		echo $MASTER_TAG > $DIR/compat_base_tree_version
		echo -e "This is a ${RED}wireless-testing.git${NORMAL} compat-wireless release"
		;;
	"linux-next.git") # The linux-next integration testing tree
		MASTER_TAG=$(git tag -l| grep next | tail -1)
		echo $MASTER_TAG > $DIR/master-tag
		echo -e "This is a ${RED}linux-next.git${NORMAL} compat-wireless release"
		;;
	"linux-stable.git") # Greg's all stable tree
		echo -e "This is a ${GREEN}linux-stable.git${NORMAL} compat-wireless release"
		;;
	"linux-2.6.git") # Linus' 2.6 tree
		echo -e "This is a ${GREEN}linux-2.6.git${NORMAL} compat-wireless release"
		;;
	*)
		;;
	esac

	cd $DIR
	echo -e "\nBase tree: ${GREEN}$(cat compat_base_tree)${NORMAL}" >> $CODE_METRICS
	echo -e "Base tree version: ${PURPLE}$(cat compat_base_tree_version)${NORMAL}" >> $CODE_METRICS
	echo -e "compat-wireless release: ${YELLOW}$(cat compat_version)${NORMAL}" >> $CODE_METRICS

	cat $CODE_METRICS
fi

./scripts/driver-select restore
