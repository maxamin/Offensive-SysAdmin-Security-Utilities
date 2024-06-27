#!/bin/bash
# make our output look nice...
function print_good () {
	echo -e "\x1B[01;32m[+]\x1B[0m $1"
}

function print_error () {
	echo -e "\x1B[01;31m[-]\x1B[0m $1"
}

function print_info () {
	echo -e "\x1B[01;34m[*]\x1B[0m $1"
}

#
# Compile Artifacts to Deliver an X64 Payload
#
function build_artifacts64() {
	# compile our 64-bit DLL
	print_info "Recompile artifact64.x64.dll with ${1}"
	${CCx64}-gcc -m64 $options $common $1 src-main/dllmain.c -Wall -shared -DDATA_SIZE=1024 src-main/dllmain.def -s -o ${2}/artifact64.x64.dll

	# compile our 64-bit EXE
	print_info "Recompile artifact64.exe with ${1}"
	${CCx64}-gcc -m64 $options $common $1 src-main/main.c -Wall -mwindows -DDATA_SIZE=1024 -s -o ${2}/artifact64.exe

	# compile our 64-bit Service EXE (auto-migrate to allow cleanup)
	print_info "Recompile artifact64svc.exe with ${1}"
	${CCx64}-gcc -m64 $options $common $1 src-main/svcmain.c -D_MIGRATE_ -Wall -mwindows -DDATA_SIZE=1024 -s -o ${2}/artifact64svc.exe

	#
	# Compile Large Artifacts for Fully-Staged Beacon Payloads
	#

	# compile our 64-bit DLL
	print_info "Recompile artifact64big.x64.dll with ${1}"
	${CCx64}-gcc -m64 $options $common $1 src-main/dllmain.c -Wall -shared -DDATA_SIZE=${stagesize} src-main/dllmain.def -s -o ${2}/artifact64big.x64.dll

	# compile our 64-bit EXE
	if [[ "$3" = "DEBUG" ]]; then
	    print_info "Recompile artifact64big.exe with ${1} DEBUG MODE"
	    ${CCx64}-gcc -m64 $options $common $1 src-main/main.c -Wall -mconsole -DDATA_SIZE=${stagesize} -s -o ${2}/artifact64big.exe -DDEBUG
	else
	    print_info "Recompile artifact64big.exe with ${1}"
	    ${CCx64}-gcc -m64 $options $common $1 src-main/main.c -Wall -mwindows -DDATA_SIZE=${stagesize} -s -o ${2}/artifact64big.exe
	fi
	    
	# compile our 64-bit Service EXE (auto-migrate to allow cleanup)
	print_info "Recompile artifact64svcbig.exe with ${1}"
	${CCx64}-gcc -m64 $options $common $1 src-main/svcmain.c -D_MIGRATE_ -Wall -mwindows -DDATA_SIZE=${stagesize} -s -o ${2}/artifact64svcbig.exe

	# cleanup
	rm -f *.o temp.exe temp.dll
}

#
# Compile Artifacts (x86/x64) to Deliver an x86 Payload
#
function build_artifacts() {
	rm -rf $2
	mkdir $2

	# A few notes on optional defines...
	# -D_MIGRATE_   = automatically spawn a process and inject shellcode into it
	#                 .. this is a requirement for 64-bit artifacts.
	# -DDATA_SIZE   = number of bytes to allocate for hot-patched data. For Beacon's
	#                 this should be 280KB. For stagers 1KB is fine.

	# compile our 32-bit DLL
	print_info "Recompile artifact32.dll with ${1}"
	${CCx86}-gcc -Wl,--enable-stdcall-fixup $options $common $1 src-main/dllmain.c -Wall -shared -DDATA_SIZE=1024 src-main/dllmain.def -s -o ${2}/artifact32.dll

	# compile our 32-bit EXE
	print_info "Recompile artifact32.exe with ${1}"
	${CCx86}-gcc $options $common $1 src-main/main.c -Wall -mwindows -DDATA_SIZE=1024 -s -o ${2}/artifact32.exe

	# compile our 32-bit Service EXE (auto-migrate to allow cleanup)
	print_info "Recompile artifact32svc.exe with ${1}"
	${CCx86}-gcc $options $common $1 src-main/svcmain.c -D_MIGRATE_ -Wall -mwindows -DDATA_SIZE=1024 -s -o ${2}/artifact32svc.exe

	#
	# Compile Large Artifacts for Fully-Staged Beacon Payloads
	#

	# compile our 32-bit DLL
	print_info "Recompile artifact32big.dll with ${1}"
	${CCx86}-gcc -Wl,--enable-stdcall-fixup $options $common $1 src-main/dllmain.c -Wall -shared -DDATA_SIZE=${stagesize} src-main/dllmain.def -s -o ${2}/artifact32big.dll

	# compile our 32-bit EXE
	print_info "Recompile artifact32big.exe with ${1}"
	${CCx86}-gcc $options $common $1 src-main/main.c -Wall -mwindows -DDATA_SIZE=${stagesize} -s -o ${2}/artifact32big.exe

	# compile our 32-bit Service EXE (auto-migrate to allow cleanup)
	print_info "Recompile artifact32svcbig.exe with ${1}"
	${CCx86}-gcc $options $common $1 src-main/svcmain.c -D_MIGRATE_ -Wall -mwindows -DDATA_SIZE=${stagesize} -s -o ${2}/artifact32svcbig.exe

	# cleanup
	rm -f *.o temp.exe temp.dll

	# copy our script over
	cp script.example ${2}/artifact.cna
}

# compiler flags to pass to all builds. Use this to set optimization level or tweak other fun things.
export options="-Os"

# our common files... goal is to stay very light
export common="src-common/patch.c src-common/unhook.c"

# change up the compiler if you need to
export CCx86="i686-w64-mingw32"
export CCx64="x86_64-w64-mingw32"

# check for a cross-compiler
if [ $(command -v ${CCx64}-gcc) ]; then
	print_good "You have a x86_64 mingw--I will recompile the artifacts"
else
	print_error "No cross-compiler detected. Try: apt-get install mingw-w64"
	exit
fi

# ----------------------------------------------------------------------------
# 'stagesize' can be changed to accommodate the larger required space for beacons that
# have used the 'BEACON_RDLL_SIZE' hook for large custom reflective loaders.
# The size specified must be equal/larger than the size of the payload being
# patched into the selected artifact.
# ----------------------------------------------------------------------------
# See: "Final Payload Buffer Size" in 'script.example'
# Default/Legacy stage size = 271360
# 100KB User Defined Reflective Loader size = 412256
# ----------------------------------------------------------------------------
export stagesize="271360"
# export stagesize="412256"

export stagesize=$(($stagesize*3))

echo Building artifact kit with Stage Data Size: ${stagesize}

#
# build our artifacts with different bypass techniques
#
build_artifacts "src-common/bypass-an0n.c" "dist-an0n" $1
build_artifacts64 "src-common/bypass-an0n.c" "dist-an0n" $1


