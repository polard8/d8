
all:
	echo "Building Gramado OS ..."

	make -C games/
	make -C de/
	make -C kernel/

