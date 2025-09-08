# YALI-OS
YALI-OS is an interactive, multitasking operating system with a custom kernel and shell. 
You must have the C cross compiler, be using Ubuntu (or a compatible debian system) and have the necessary tools like GDB or QEMU to test the operating system. 

To build the project type the following in the root dir: 
./build.sh

To clean the bin and o files in the build dir type the following: 
make clean 

For intermediate commits and for testing purposes please type the following lines to test the machine with qemu and gdb: 
cd bin 
gdb 
add-symbol-file ../build/kernelfull.o
target-remote | qemu-system-i386 -hda ./os.bin -S -gdb stdio 

After opening the terminal with QEMU you can add breakpoints with: 
break /filename/.c: /line/



