#!/usr/bin/env bash
set -euo pipefail

# Default Values
DEFAULT_MAX_FD=256
DEFAULT_PG_SZ=4096
DEFAULT_LN_SZ=64
DEFAULT_CPU_CNT=1
DEFAULT_ICACHE_SZ=32768
DEFAULT_DCACHE_SZ=32768

##
# Returns the maximum amount of file descriptors allowed.
# Params:
#   - $1: String containing digit
#
# TODO: Find scalable way to detect if the digit is a power of 2.
is_power_of_2()
{
        POWERS_OF_2="32 64 128 256 512 1024 2048 4096 8192 16384 32768 65536 131072"

        if [ "$POWERS_OF_2" != "${POWERS_OF_2%"$1"*}" ] && [ "$1" -gt "31" ]; then
                echo "true"
        else
                echo ""
        fi
}

##
# Returns the maximum amount of file descriptors allowed.
# Params:
#   - $1: String with the OS being used
get_max_fd()
{
        if [ "$1" = "Darwin" ] || [ "$1" = "FreeBSD" ]; then
                MAX_FD=`getconf OPEN_MAX | tr -dc '[:digit:]'`
	elif [ "$1" = "Linux" ] && [ "$ARCH" != "arm" ]; then
                MAX_FD=`getconf -a | grep -i OPEN_MAX | tr -dc '[:digit:]'`
        else
                MAX_FD=""
        fi

        if [ $(is_power_of_2 "$MAX_FD") ]; then
                echo "$MAX_FD"
        else
                echo "$DEFAULT_MAX_FD"
        fi
}

##
# Returns the page size used by the OS.
# Params:
#   - $1: String with the OS being used
get_page_sz()
{
        if [ "$1" = "Darwin" ] || [ "$1" = "FreeBSD" ]; then
                PG_SZ=`sysctl -a | grep -i hw.pagesize: | cut -d' ' -f2 | tr -dc '[:digit:]'`
	elif [ "$1" = "Linux" ] && [ "$ARCH" != "arm" ]; then
                PG_SZ=`getconf -a | grep -i PAGE_SIZE | tr -dc '[:digit:]'`
        else
                PG_SZ=""
        fi

        if [ $(is_power_of_2 "$PG_SZ") ]; then
                echo "$PG_SZ"
        else
                echo "$DEFAULT_PG_SZ"
        fi
}

##
# Returns the cache line size of the architecture.
# Params:
#   - $1: String with the OS being used
get_cache_line_sz()
{
        if [ "$1" = "Darwin" ] || [ "$1" = "FreeBSD" ]; then
                LN_SZ=`sysctl -a | grep -i hw.cachelinesize | cut -d' ' -f2 | tr -dc '[:digit:]'`
	elif [ "$1" = "Linux" ] && [ "$ARCH" != "arm" ]; then
                LN_SZ=`getconf -a | grep -i LEVEL1_DCACHE_LINESIZE | tr -dc '[:digit:]'`
        else
                LN_SZ=""
        fi

        if [ $(is_power_of_2 "$LN_SZ") ]; then
                echo "$LN_SZ"
        else
                echo "$DEFAULT_LN_SZ"
        fi
}

# Returns the amount of CPUs in the machine.
# Params:
#   - $1: String with the OS being used
get_cpu_count()
{
        if [ "$1" = "Darwin" ] || [ "$1" = "FreeBSD" ]; then
                CPU_CNT=`sysctl -a | grep -i hw.physicalcpu: | cut -d' ' -f2 | tr -dc '[:digit:]'`
	elif [ "$1" = "Linux" ] && [ "$ARCH" != "arm" ]; then
                SOCKETS=`lscpu | grep -i "Socket(s):" | tr -d [[:blank:]] | cut -d ':' -f 2`
                CORES=`lscpu | grep -i "Core(s) per socket:" | tr -d [[:blank:]] | cut -d ':' -f 2`
                CPU_CNT=`expr "$SOCKETS" \* "$CORES"`
        else
                CPU_CNT="$DEFAULT_CPU_CNT"
        fi

        echo "$CPU_CNT"
}

# Returns the size of the instruction cache in bytes.
# Params:
#   - $1: String with the OS being used
get_icache_sz()
{
        if [ "$1" = "Darwin" ] || [ "$1" = "FreeBSD" ]; then
                ICACHE_SZ=`sysctl -a | grep -i hw.l1icachesize | cut -d' ' -f2 | tr -dc '[:digit:]'`
	elif [ "$1" = "Linux" ] && [ "$ARCH" != "arm" ]; then
                ICACHE_SZ=`getconf -a | grep -i LEVEL1_ICACHE_SIZE | tr -dc '[:digit:]'`
        else
                ICACHE_SZ=""
        fi

        if [ $(is_power_of_2 "$ICACHE_SZ") ]; then
                echo "$ICACHE_SZ"
        else
                echo "$DEFAULT_ICACHE_SZ"
        fi

}

# Returns the size of the data cache in bytes.
# Params:
#   - $1: String with the OS being used
get_dcache_sz()
{
        if [ "$1" = "Darwin" ] || [ "$1" = "FreeBSD" ]; then
                DCACHE_SZ=`sysctl -a | grep -i hw.l1dcachesize | cut -d' ' -f2 | tr -dc '[:digit:]'`
	elif [ "$1" = "Linux" ] && [ "$ARCH" != "arm" ]; then
                DCACHE_SZ=`getconf -a | grep -i LEVEL1_DCACHE_SIZE | tr -dc '[:digit:]'`
        else
                DCACHE_SZ=""
        fi

        if [ $(is_power_of_2 "$DCACHE_SZ") ]; then
                echo "$DCACHE_SZ"
        else
                echo "$DEFAULT_DCACHE_SZ"
        fi

}

# Get OS independent variables
OS=`uname -s`
ARCH=`uname -m`
if [ "$ARCH" != "${ARCH%"arm"*}" ]; then
        ARCH="arm"
fi

# Provide OS dependent variables
case "$1" in
        "MAX_FD")
                get_max_fd "$OS"
                ;;
        "PAGE_SZ")
                get_page_sz "$OS"
                ;;
        "CACHE_LINE_SZ")
                get_cache_line_sz "$OS"
                ;;
        "NUM_CPUS")
                get_cpu_count "$OS"
                ;;
        "ICACHE_SZ")
                get_icache_sz "$OS"
                ;;
        "DCACHE_SZ")
                get_dcache_sz "$OS"
                ;;
        *)
                echo ""
                ;;
esac
