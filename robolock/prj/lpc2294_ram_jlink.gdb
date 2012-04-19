#
# This config file was tested with J-Link GDB Server v4.10i
#

# Listening for commands on this PC's tcp port 2331
target remote localhost:2331

# Set gdb server to little endian
monitor endian little

# Set JTAG speed to adaptive
monitor speed adaptive

# Reset the chip to get to a known state.
monitor reset 0

# Setup target, remap first 64 bytes
# to the internal RAM
monitor writeu16 0xE01FC040 = 0x0002
monitor writeu16 0xE01FC040

# Setup GDB for faster downloads
set remote memory-write-packet-size 1024
set remote memory-write-packet-size fixed

#load
#break main
#continue
