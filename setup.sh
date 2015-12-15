#!/bin/bash

function all
{
	clone
	toolchain
}

function clone
{
	cd /$HOME
	apt-get install git
	git clone https://github.com/JohannMassyn/CometOS.git
	chmod -R 755 CometOS
	cd CometOS
	chmod -R 755 *
}

function toolchain_download
{
	#Download apt-get stuff
	echo -e "\e[33m Downloading Packages ...\e[37m"
	apt-get install libgmp3-dev libmpfr-dev libisl-dev libmpc-dev texinfo qemu gcc g++ genext2fs make

	echo -e "\e[33m Downloading Toolchain ...\e[37m"
	cd /$HOME
	sudo wget -O cometos-toolchain.tar.gz https://googledrive.com/host/0B5_PgYahYySfbXFfQ3N4cC1RbW8
	tar -zxvf cometos-toolchain.tar.gz
	rm cometos-toolchain.tar.gz
}

function toolchain
{
	#Download apt-get stuff
	echo -e "\e[33m Downloading Packages ...\e[37m"
	apt-get install libgmp3-dev libmpfr-dev libisl-dev libmpc-dev texinfo qemu gcc g++ genext2fs make

	echo -e "\e[33m Making Dir's \e[37m"
	cd HOME
	mkdir opt
	chmod -R 755 opt
	cd opt
	mkdir cross
	chmod -R 755 cross
	cd /$HOME
	mkdir src
	chmod -R 755 src

	export PREFIX="$HOME/opt/cross"
	export TARGET=i686-elf
	export PATH="$PREFIX/bin:$PATH"

	# ======= Binutils ========
	echo -e "\e[33m Making Binutils \e[37m"
	cd $HOME/src
	echo -e "\e[33m Downloading ... \e[37m"
	wget http://ftpmirror.gnu.org/binutils/binutils-2.24.tar.gz
	echo -e "\e[33m Extracting ... \e[37m"
	tar -zxvf binutils-2.24.tar.gz
	mkdir build-binutils
	chmod -R 755 build-binutils
	cd build-binutils
	echo -e "\e[33m Configure Binutils \e[37m"
	../binutils-2.24/configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror
	echo -e "\e[33m Making Binutils\e[37m"
	make
	echo -e "\e[33m Installing Binutils\e[37m"
	make install
	echo -e "\e[32m Binutils Complete \e[37m"

	# ======= gcc ========
	echo -e "\e[33m Making gcc \e[37m"
	cd $HOME/src
	echo -e "\e[33m Downloading ... \e[37m"
	wget http://ftpmirror.gnu.org/gcc/gcc-4.9.1/gcc-4.9.1.tar.gz
	echo -e "\e[33m Extracting ... \e[37m"
	tar -zxvf gcc-4.9.1.tar.gz
	which -- $TARGET-as || echo $TARGET-as is not in the PATH
	mkdir build-gcc
	chmod -R 755 build-gcc
	cd build-gcc
	echo -e "\e[33m Configure gcc \e[37m"
	../gcc-4.9.1/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --without-headers
	echo -e "\e[33m Making gcc-all\e[37m"
	make all-gcc
	echo -e "\e[33m Making target-libgcc\e[37m"
	make all-target-libgcc
	echo -e "\e[33m Installing gcc\e[37m"
	make install-gcc
	echo -e "\e[33m Installing target-libgcc\e[37m"
	make install-target-libgcc
	echo -e "\e[32m gcc Complete \e[37m"
	echo -e "\e[32m enjoy crunching bits ;) \e[37m"

}

# ====================== Start ========================
clear
echo -e "\e[37m"
cat << "EOF"

	 ██████╗ ██████╗ ███╗   ███╗███████╗████████╗ ██████╗ ███████╗
	██╔════╝██╔═══██╗████╗ ████║██╔════╝╚══██╔══╝██╔═══██╗██╔════╝
	██║     ██║   ██║██╔████╔██║█████╗     ██║   ██║   ██║███████╗
	██║     ██║   ██║██║╚██╔╝██║██╔══╝     ██║   ██║   ██║╚════██║
	╚██████╗╚██████╔╝██║ ╚═╝ ██║███████╗   ██║   ╚██████╔╝███████║
	 ╚═════╝ ╚═════╝ ╚═╝     ╚═╝╚══════╝   ╚═╝    ╚═════╝ ╚══════╝
	 +================================== Tool Chain Builder =====+


EOF

echo -e "\e[33m Warning: this script will not work without apt and wget.\e[37m"
echo -e "\e[33mYou can down load the following source code manually:\e[37m"
echo -e "\e[1;33m- GCC, GNU Make, GNU Bison, GNU GMP, GNU MPFR, GNU MPC, Flex, g++\e[37m"

#Main Menu
PS3='Please enter your choice: '
options=("All" "Clone CometOS" "Download toolchain" "Build & install toolchain" "Quit")
select opt in "${options[@]}"
do
    case $opt in
    	"All")
            echo -e "\e[91m Running All \e[37m"
            all
            echo -e "\e[32m Complete - 100% \e[37m"
            exit
            ;;
        "Clone CometOS")
            echo -e "\e[91m Cloneing CometOS \e[37m"
            clone
            echo -e "\e[32m Complete - 100% \e[37m"
            exit
            ;;
        "Download toolchain")
            echo -e "\e[91m Downloading toolchain \e[37m"
            toolchain_download
            echo -e "\e[32m Complete - 100% \e[37m"
            exit
            ;;
        "Build & install toolchain")
            echo -e "\e[91m Build and install toolchain \e[37m"
            toolchain
            echo -e "\e[32m Complete - 100% \e[37m"
            exit
            ;;
        "Quit")
            exit
            ;;
        *) echo invalid option;;
    esac
done