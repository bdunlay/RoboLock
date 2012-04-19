#
# This config file was tested with J-Link GDB Server v4.10i
#

# Listening for commands on this PC's tcp port 2331
target remote localhost:2331

# Enable flash download and flash breakpoints.
# Flash download and flash breakpoints are features of
# the J-Link software which require separate licenses 
# from SEGGER.

# Select flash device
monitor flash device = LPC2294

# Enable FlashDL and FlashBPs
monitor flash download = 1
monitor flash breakpoints = 1

# Set gdb server to little endian
monitor endian little

# Set JTAG speed to adaptive
monitor speed adaptive

# Reset the chip to get to a known state
monitor reset 0

# Setup target, 
# remap first 64 bytes to the internal flash
monitor writeu16 0xE01FC040 = 0x0001
monitor writeu16 0xE01FC040

#load
#break main
#continue