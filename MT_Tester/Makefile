#
.EXPORT_ALL_VARIABLES:

VER = 1.2.0

all:
$(MAKE) -f ./makefiles/${DEV}.mk all
backup:
tar -czvf MT_Tester-${VER}-$(shell date +%F).tar.gz projects/Output/pc-linux/MT_Tester* /mt_conf

.PHONY: all backup