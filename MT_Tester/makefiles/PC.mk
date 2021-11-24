###############################################################################
#可以在make命令行指定变量
#   变量名         取值                              默认值
#   VALGRIND      yes/no                           no
#   RAM_SIZE      wlan ramdisk size(M)             256
###############################################################################
.EXPORT_ALL_VARIABLES:

VALGRIND = no
RAM_SIZE = 512

#其它变量不建议在命令行下修改
RAMDISK_FILE=/tmp/MT_Tester-ramdisk
KERNEL_VERSION=2.6.18.6
TEMP_ROOTFS_PATH=rootfs
KERNEL_SRC_PATH=kernel-${KERNEL_VERSION}
ROOT=$(shell pwd)/${OUTPUT}/${TEMP_ROOTFS_PATH}
STORAGE=$(shell pwd)/patch/x86/embedded_rootfs/storage
OCTEON_ROOT=$(shell pwd)/${OUTPUT}
DIR_WLAN_STORAGE = $(shell pwd)/${OUTPUT}/wlan-storage
DIR_WLAN_SRC_HTML = $(shell pwd)/projects/linux/src/code/om/wmc
DIR_WLAN_DST_LINUX = ${DIR_WLAN_STORAGE}/ACSubSystem/CPS
DIR_WLAN_DST_HTML = ${DIR_WLAN_STORAGE}/ACSubSystem/OM
DIR_WLAN_SRC_LINUX = $(shell pwd)/projects/linux/projects

all:taget-programs target-wlan-storage taget-config
ifneq (${FLASH}, no)
	mkdir -p ${ROOT}/mnt/primary && cp -rf flash/mnt/* ${ROOT}/mnt/primary
	mkdir -p ${ROOT}/mnt/secondary && cp -rf flash/mnt/* ${ROOT}/mnt/secondary
endif
	cp -f config/wlan/version/install.x86 ${OUTPUT}/install
#增加重新根据PCI号设置网络接口名称的功能
	mkdir -p ${OUTPUT}/${TEMP_ROOTFS_PATH}/devinfo && cp -f config/netif/network-scripts.${DEV} ${OUTPUT}/${TEMP_ROOTFS_PATH}/devinfo/network-scripts
ifneq (${DEV}, PC)
	mkdir -p ${OUTPUT}/${TEMP_ROOTFS_PATH}/bin && cp -f config/netif/recfg-netif ${OUTPUT}/${TEMP_ROOTFS_PATH}/bin/recfg-netif
else
	-rm -f ${OUTPUT}/${TEMP_ROOTFS_PATH}/bin/recfg-netif
endif
	cd ${OUTPUT} && tar -czvf ${TARGET_PACKET} install ${TEMP_ROOTFS_PATH}

target-kernel:
	cd ${OUTPUT} && tar -xzvf ../patch/x86/kernel_2.6/${KERNEL_SRC_PATH}.tar.gz 
	cd ${OUTPUT} && cp ../patch/x86/kernel_2.6/${DEV}.config ${KERNEL_SRC_PATH}/.config
	mkdir -p ${OUTPUT}/${TEMP_ROOTFS_PATH}/boot
	${MAKE} -C ${OUTPUT}/${KERNEL_SRC_PATH} INSTALL_PATH=../${TEMP_ROOTFS_PATH}/boot INSTALL_MOD_PATH=../${TEMP_ROOTFS_PATH} all
	${MAKE} -C ${OUTPUT}/${KERNEL_SRC_PATH} INSTALL_PATH=../${TEMP_ROOTFS_PATH}/boot INSTALL_MOD_PATH=../${TEMP_ROOTFS_PATH} modules_install
	${MAKE} -C ${OUTPUT}/${KERNEL_SRC_PATH} INSTALL_PATH=../${TEMP_ROOTFS_PATH}/boot INSTALL_MOD_PATH=../${TEMP_ROOTFS_PATH} install
	
taget-programs:
	mkdir -p ${OCTEON_ROOT}/linux
	cp -rf config ${OCTEON_ROOT}/linux
	make -C ${OUTPUT} -f ../patch/x86/embedded_rootfs/pkg_makefiles/dhcp-4.1.2.mk all install
	make -C ${OUTPUT} -f ../patch/x86/embedded_rootfs/pkg_makefiles/ac-resource.mk all install
	make -C ${OUTPUT} -f ../patch/x86/embedded_rootfs/pkg_makefiles/keepalived-1.1.20.mk all install
	make -C ${OUTPUT} -f ../patch/x86/embedded_rootfs/pkg_makefiles/quagga.mk all install

target-wlan-storage:
	mkdir -p ${DIR_WLAN_STORAGE}
	cd ${DIR_WLAN_STORAGE} && mkdir -p ACConfigData ACLog ACSubSystem ACVersion WAPICertificate
	cd ${DIR_WLAN_STORAGE}/ACConfigData && mkdir -p omc && touch omc/incrdata.dat && cp ${DIR_WLAN_SRC_HTML}/ac-init-data.tar.gz ./
	cd ${DIR_WLAN_STORAGE}/ACLog && mkdir -p ALARM EVENT OPERAT RUN
	cd ${DIR_WLAN_STORAGE}/ACSubSystem && mkdir -p CPM CPS OM UPM
	chmod a+r ${DIR_WLAN_STORAGE} && chmod a+w ${DIR_WLAN_STORAGE} && chmod a+x ${DIR_WLAN_STORAGE}
	cp -f config/8021x/hostapd.conf ${DIR_WLAN_STORAGE}/ACSubSystem/CPS
	cp -f config/8021x/hostapd.conf ${DIR_WLAN_STORAGE}/ACSubSystem/CPM

#为了防止更新版本时修改update，将update脚本放在ramdisk里，初始化时拷贝到/bin/
	chmod 755 config/wlan/version/update && cp -f config/wlan/version/update ${DIR_WLAN_STORAGE}

ifneq (${APP}, no)
	cd ${DIR_WLAN_SRC_LINUX} && make TARGET=pc SYSTEM=${SYSTEM} VALGRIND=${VALGRIND} all && make TARGET=pc SYSTEM=${SYSTEM} VALGRIND=${VALGRIND} PROJECT_INSTALL_DIR=${DIR_WLAN_DST_LINUX} install
endif
	cd ${DIR_WLAN_DST_HTML} && unzip -o ${DIR_WLAN_SRC_HTML}/conf.zip && unzip -o ${DIR_WLAN_SRC_HTML}/servlets.zip

	echo "software-name:MT_Tester" > ${DIR_WLAN_STORAGE}/ACVersion/version.config
	echo "software-verno:${VER}" >> ${DIR_WLAN_STORAGE}/ACVersion/version.config
	echo "software-vendor:MyCompany" >> ${DIR_WLAN_STORAGE}/ACVersion/version.config
	echo "software-file:$(notdir ${TARGET_PACKET})" >> ${DIR_WLAN_STORAGE}/ACVersion/version.config
	echo "software-build:$(shell date "+%Y-%m-%d %H-%M-%S")" >> ${DIR_WLAN_STORAGE}/ACVersion/version.config
	echo "software-description:${SOFTWARE_DESCRIPTION}" >> ${DIR_WLAN_STORAGE}/ACVersion/version.config

	chmod -R 777 ${DIR_WLAN_STORAGE}
	chmod 644 ${DIR_WLAN_STORAGE}/ACVersion/version.config
ifneq (${APP}, no)
	chmod 644 ${DIR_WLAN_STORAGE}/ACSubSystem/CPS/MT_Tester.map
endif

	rm -f ${RAMDISK_FILE} && mktemp ${RAMDISK_FILE}
	dd if=/dev/zero of=${RAMDISK_FILE} bs=1M count=${RAM_SIZE}
	/sbin/losetup /dev/loop0 ${RAMDISK_FILE} && /sbin/mkfs.ext3 -m 0 /dev/loop0
	rm -rf ${OUTPUT}/wlan-ramdisk && mkdir -p ${OUTPUT}/wlan-ramdisk
	mount -t ext3 /dev/loop0 ${OUTPUT}/wlan-ramdisk
	cp -rf ${DIR_WLAN_STORAGE}/* ${OUTPUT}/wlan-ramdisk && sync
	gzip -cfv9 ${RAMDISK_FILE} > ${ROOT}/usr/local/wlan-ramdisk.gz
	umount ${OUTPUT}/wlan-ramdisk && /sbin/losetup -d /dev/loop0
	rm -f ${RAMDISK_FILE}
	
taget-config:
	mkdir -p ${ROOT}/etc/rc.d
	chmod 777 patch/x86/embedded_rootfs/etc-files/profile && cp -f patch/x86/embedded_rootfs/etc-files/profile ${ROOT}/etc
	chmod 777 patch/x86/embedded_rootfs/etc-files/rc.local && cp -f patch/x86/embedded_rootfs/etc-files/rc.local ${ROOT}/etc/rc.d

	mkdir -p ${ROOT}/etc/vsftpd
	chmod 644 config/vsftpd/vsftpd.conf && cp -f config/vsftpd/vsftpd.conf ${ROOT}/etc/vsftpd

clean:
	rm -rf ${OUTPUT}/*

install:
	cp -f ${OUTPUT}/${TARGET_PACKET} ${INSTALL_PATH}
