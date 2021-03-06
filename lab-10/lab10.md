---
title: 'The FAT-12 Filesystem'
---

# Introduction
In this lab, you will gain hands-on experience reading a FAT-12 filesystem.  Using information we provide, you will decode the boot sector by hand.  You will then write a program to do this automatically.  [You may findthis website very useful as you work on this lab.](http://wiki.osdev.org/FAT)

## Terms

### Sector
The smallest unit of transfer; It's also called a block. There are 512 bytes / block on a floppy disk.

### Boot Sector
Stores the vital information about the filesystem.  The boot sector is laid out in the following way, starting at the beginning of the disk(logical sector 0, byte 0):
All values are stored as unsigned little-endian numbers unless otherwise specified.

| Offset | Length | Contents				| Display Format |
|:------:|--------|-------------------------------------|----------------|
|  0x00	 |3	  |Binary offset of boot loader		|hex		 |
|  0x03	 |8	  |Volume Label (ASCII, null padded)	|ASCII		 |
|  0x0B  |2	  |Bytes / sector			|decimal	 |
|  0x0D  |1	  |Sectors / cluster			|decimal	 |
|  0x0E	 |2	  |Reserved sectors			|decimal	 |
|  0x10  |1	  |Number of FATs (generally 2)		|decimal	 |
|  0x11  |2	  |Number of Root Directory entries	|decimal	 |
|  0x13  |2	  |Number of logical sectors		|decimal	 |
|  0x15  |1	  |Medium descriptor			|hex		 |
|  0x16  |2	  |Sectors per FAT			|decimal	 |
|  0x18  |2	  |Sectors per track			|decimal	 |
|  0x1A  |2	  |Number of heads			|decimal	 |
|  0x1C  |2	  |Number of hidden sectors		|decimal	 |

## Note on Endianess
All data on a computer is stored as a string of 0s and 1s.  Every eight of these binary numbers are grouped together to for a byte, and every byte is given an address in the computers memory.  
This leads to the question for how the bytes should be arranged for a multibyte number.  For example, the decimal number 305,419,896.  
This number as a `uint32_t` in hexadecimal is 0x12345678.  There are two ways that computers normally store this value in memory: little endian or big endian.  
Normally you as the programmer never need to concern yourself with how numbers are stored in the computers memory, but when you are working directly with that physical medium it because a necessarry
consideration.

### Little Endian
Little endian is stored with the least significant byte (LSB) at the lowest memory address.  Therefore if the example number is stored at memory location 0x1000 the resulting memory map would be:

| Address | Value |
|:-------:|:-----:|
| 0x1000  | 0x78  |
| 0x1001  | 0x56  |
| 0x1002  | 0x34  |
| 0x1003  | 0x12  |

### Big Endian
Big endian is stored with the most significant byte (MSB) at the lowest memory address.  Therefore if the example number is stored at memory location 0x1000 in big endian format the resulting memory map would be:

| Address | Value |
|:-------:|:-----:|
| 0x1000  | 0x12  |
| 0x1001  | 0x34  |
| 0x1002  | 0x56  |
| 0x1003  | 0x78  |

### Testing Endianess
You can find online information about a given architecture to find if it is big or little endian.  You can also write a quick program that will test the endianess for you.  
Compile and run this program to find the endianess of your architecture.  Include the result in your lab report.

~~~C
#include <stdio.h>
#include <stdint.h>
int main(int argc, const char * argv[])
{
 uint32_t a = 0x12345678;
 uint8_t *c = (uint8_t*)(&a);
  if (*c == 0x78) {
   printf("little-endian\n");
  } else {
   printf("big-endian\n");
  }
 return 0;
}
~~~

### Endian Swap
Sometimes you may find it neccessary to switch from one endianess to another.  The following functions will do just that.

~~~C
#include <stdint.h>

inline uint16_t swp_end_16(uint16_t n)
{
 return (n << 8) | (n >> 8);
}

inline uint32_t swp_end_32(uint32_t n)
{
 return ((n << 24)  & 0xFF000000) | ((n << 8) & 0x00FF0000) | ((n >> 8) & 0x0000FF00) | ((n >> 24) & 0x000000FF);
}
~~~

## Useful system calls
More information on these system calls can be found in `man 2 open`, `man 2 read`, and `man 2 lseek`.

~~~c
int open(const char* path, int flags) 
~~~

Opens a file at `path` and returns a file descriptor. For example, `open("/tmp/myfile", ORDONLY)` opens an existing file called `myfile` in the `tmp` directory in read-only mode.  
It returns a file descriptor that can be used like a handle to the open file.

~~~c
ssize_t read(int fd, void *buf, size_t count)
~~~

Reads `count` bytes from the file descriptor `fd` into the memory location starting at `buf`.  It starts reading from the current offset into the file.  
The offset is set to zero if the file has just been opened.

~~~c
off_t lseek(int fd, off_t offset, int whence)
~~~

Sets the offset of the file descriptor `fd` to `offset`, depending on whence.  If `whence` is `SEEK_SET`, then the offset is figured from the start of the file.  
If `whence` is `SEEK_CUR`, then the offset is relative to the current offset.

## Inspecting binary files
You should have a few floppy disk images available in the lab folder, along with skeleton code to decode the boot sector.  In Unix, devices are treated as files.  
For example, the floppy drive is normally found at a device file such as `/dev/fd0`.  We can use a file image as a substitute for an actual floppy disk.  
The floppy images contain a real FAT-12, FAT-16, and FAT-32 filesystems. If you know how the byts are laid out, then you can decode the structures that describe the filesystem and work with the files.  
To help you get started, we're going to decode a few fields by hand.

There is a utility in Unix called `hexdump` that will show the binary contents of a file.  This is useful to inspect a file containing binary data.  If you use hexdump -C on one of the suppiled images, 
the output will look something like this(use `hexdump -C image.fat12 | less to more easily read it)

~~~bash
00000000  eb 3c 90 6d 6b 66 73 2e  66 61 74 00 02 04 01 00  |.<.mkfs.fat.....|
00000010  02 00 02 00 08 f8 02 00  20 00 40 00 00 00 00 00  |........ .@.....|
00000020  00 00 00 00 80 00 29 1e  71 c8 d0 43 50 52 45 33  |......).q..CPRE3|
00000030  30 38 2d 31 32 20 46 41  54 31 32 20 20 20 0e 1f  |08-12 FAT12   ..|
00000040  be 5b 7c ac 22 c0 74 0b  56 b4 0e bb 07 00 cd 10  |.[|.".t.V.......|
00000050  5e eb f0 32 e4 cd 16 cd  19 eb fe 54 68 69 73 20  |^..2.......This |
...[there should be a lot more after this]
~~~

The first column is the hexadecimal offset into the file.  Since each line contains 16 bytes, this will always end in a 0.  The middle 16 columns are the hexadecimal bytes of the file.  
The third column contains the ASCII representation of the bytes, or a `.` if the byte is not a printable character.  
For example, the second byte, having an offset of 0x01, has the value 0x3c corresponding to the character `<`, while the third byte, with a value of 0x90, is not printable, so it is shown as a dot.

# Exercises

## Decoding the boot sector by hand
Using `hexdump` and the offsets in the tables above, find and decode the values for each of the following fields in the boot sector(remember that it starts at offset 0):

|			|Hex		|Decimal	|
|:----------------------|:--------------|:--------------|
|Bytes / sector 	|	0200	|	512	|
|Sectors / cluster	|	04	|	4	|
|Root directory entries |	0200	|	512	|
|Sectors / FAT		|	0002	|	2	|

Have the lab TA check your answers.  You will use these values for debugging the next part.

# Task for this lab
Write code to decode and print the boot sector of the FAT-12 filesystem.  The starting offset and size of each field can be found in the table at the beginning of the lab handout.  
All of the information is stored as binary values on the disk.  When the values are to be printed, use the same format as the table at the beginning of the handout.  The program should:
 - Take the name of the file to read as an argument
 - decode the boot sector of the file given
 - print out the values in the boot sector
 - no segmentation faults should occur
  - meaning all system calls should be checked if they were successful
  - if an error did occur, it should print out an error message detailing what caused it, and exit gracefully

# What to submit
Please submit **only** your well-formatted and commented source code and a copy of the output.

# Extra credit
Extra credit can be had for extending the boot sector parser to handle the FAT-16 and FAT-32 filesystems.  Consult the OS Dev website regarding the boot sector for the other two filesystems, [which can be found here.](http://wiki.osdev.org/FAT)

# License
This lab write up and all accompany materials are distributed under the MIT License.  For more information, read the accompaning LICENSE file distributed with the source code.
