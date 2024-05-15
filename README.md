# operating-system

Developed the core of an operating system.
Starter code: boots us into protected mode, sets up the GDT, LDT, and initial TSS, and maps a read-only file system image into physical memory. Credit: ece391 course staff, UIUC

#### Development:
- Set up the interrupt descriptor table (IDT)
- Basic paging support for tasks
- Separate 4 MB pages for the kernel and applications
- Initialized devices
- Designed file system. See [File System Design](/file_sys_design.png). Simultaneous opening of upto 8 files including `stdin` and `stdout` supported
- Wrote the system call interface along with ten system calls ([sys_calls.c](/student-distrib/sys_calls.c)): `halt`, `execute`, `read`, `write`, `open`, `close`, `getargs`, `vidmap`, `set_handler`, `sigreturn`
- Provided support for six tasks from program images in the file system which interface with the kernel via system calls
- Multiple terminals (3). Up to 6 processes are supported.
- Round-robin scheduling for processes
- Playing audio with SoundBlaster
- Signals: DIV_ZERO, SEGFAULT, INTERRUPT, ALARM, USER1

#### Running Code:
- Run in Linux Emulator ([QEMU](https://www.qemu.org/download/))
- `cd` into [student-distrib](/student-distrib/)
- Make sure [mp3.img](/student-distrib/mp3.img) is in [student-distrib](/student-distrib/)
- Run `make dep` the first time. Save a copy of test machine to desktop with preferred name
- After any changes to code, run `sudo make` to compile and `gdb linux` to run.
- Inside GDB, run `target remote 10.0.2.2:1234` to connect to test machine. Run `c` to continue running machine.

#### Some supported commands:
| Command | Description |
| --- | --- |
| ALT+F# | Switch terminals. # is terminal number. Terminals 1--3 are supported |
| Control+C | Terminate current process |
| `shell` | Start a new shell |
| `exit` | Exit shell program |
| `ls` | Standard ls behavior. List all files in our file system [fsdir](/fsdir/) |
| `grep ..` | Standard grep behavior |
| `counter` | Start a counter (OS will prompt user to choose from 3 upper bound options with 0/1/2) |
| `cat <filename>` | Print contents of file |
| `fish` | Run [fish](/fsdir/fish) animation |
| `pingpong` | Run [pingpong](/fsdir/pingpong) animation |
| `play <audiofile>` | Play audio file. Note: This does not work with a Docker setup on Mac. Will need QEMU on Windows or a Windows emulator/double-boot on Mac |

#### Useful Links
- [OSDev](https://wiki.osdev.org/Expanded_Main_Page)
- [QEMU](https://www.qemu.org/download/)
- [SoundBlaster](https://wiki.osdev.org/Sound_Blaster_16)
