export ARCH=mppa
export POK_PATH="$(pwd)"
VALID_EXAMPLES="$(find $POK_PATH/examples -maxdepth 1 -mindepth 1 -type d  | awk -F/ '{print $NF}')"
VALID_ARGS=$VALID_EXAMPLES+" clean all"

echo "[SH] Reading input data"
ARG=$1

if [[ $ARG = "" ]]; then
	echo "[SH] No build target provided!"
	exit 1
fi

#Check arguments and prepare variables
if [[ $VALID_ARGS =~ $ARG ]]; then
	BUILD_TARGET="build-${ARG}"
	if [[ $ARG = "all" ]]; then
		ARG=$VALID_EXAMPLES
		NEED_SUBDIR=1
	elif  [[ $ARG = "clean" ]]; then #in case of cleaning, perform the action and exit
		for EXAMPLE in $VALID_EXAMPLES
		do
			echo "[SH] Cleaning example ${EXAMPLE}"
			cd ./examples/${EXAMPLE}/generated-code
			make clean
			cd $POK_PATH
		done
		rm -rf build-*
		exit 0
	fi	
else	
	echo "[SH] Invalid argument. Check examples folder or compile using the parameter 'all'."
	exit 1
fi

echo "[SH] Creating build directory"
mkdir -p $BUILD_TARGET

for EXAMPLE in $ARG
do
	if [[ $NEED_SUBDIR = 1 ]]; then
		SUBDIR="build-${EXAMPLE}"
		mkdir -p $BUILD_TARGET/$SUBDIR
	fi
	
	echo "[SH] Building example ${EXAMPLE}"
	# Moving to sample application and building it
	cd ./examples/${EXAMPLE}/generated-code
	make
	cd $POK_PATH

	cp ./examples/${EXAMPLE}/generated-code/cpu/pok.elf ./$BUILD_TARGET/$SUBDIR/pok.elf
	cp ./examples/${EXAMPLE}/generated-code/cpu/pok.elf.map ./$BUILD_TARGET/$SUBDIR/pok.elf.map
done
