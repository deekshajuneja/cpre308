
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
 #include <sys/types.h>
       #include <sys/stat.h>
       #include <fcntl.h>

typedef struct fat_extBS_32
{
	//extended fat32 stuff
	uint32_t		table_size_32;
	uint16_t		extended_flags;
	uint16_t		fat_version;
	uint32_t		root_cluster;
	uint16_t		fat_info;
	uint16_t		backup_BS_sector;
	uint8_t 		reserved_0[12];
	uint8_t		drive_number;
	uint8_t 		reserved_1;
	uint8_t		boot_signature;
	uint32_t 		volume_id;
	uint8_t		volume_label[11];
	uint8_t		fat_type_label[8];
	uint8_t		bote_code[420];
	uint16_t	signature;
 
}__attribute__((packed)) fat_extBS_32_t;
 
typedef struct fat_extBS_16
{
	//extended fat12 and fat16 stuff
	uint8_t		bios_drive_num;
	uint8_t		reserved1;
	uint8_t		boot_signature;
	uint32_t		volume_id;
	uint8_t		volume_label[11];
	uint8_t		fat_type_label[8];
	uint8_t		boot_code[448];
	uint16_t	signature;
 
}__attribute__((packed)) fat_extBS_16_t;
 
typedef struct fat_BPM
{
	uint8_t 		bootjmp[3];
	uint8_t 		oem_name[8];
	uint16_t 	    bytes_per_sector;
	uint8_t		sectors_per_cluster;
	uint16_t		reserved_sector_count;
	uint8_t		table_count;
	uint16_t		root_entry_count;
	uint16_t		total_sectors_16;
	uint8_t		media_type;
	uint16_t		table_size_16;
	uint16_t		sectors_per_track;
	uint16_t		head_side_count;
	uint32_t 		hidden_sector_count;
	uint32_t 		total_sectors_32;
 
}__attribute__((packed)) fat_BPM_t;

// A function that takes two bytes as little endian and reorders the bytes as one
// big endian short
// LSB is the first byte that is read
// MSB is the second byte read
uint16_t swap_endian(uint8_t LSB, uint8_t MSB){
	uint16_t ret = (uint16_t)MSB;
	ret = ret << 8;
	ret = ret | (uint16_t)LSB;
	return ret;
}

typedef struct fat_BS
{
	fat_BPM_t bpm;
	union
	{
		fat_extBS_16_t fat12_16;
		fat_extBS_32_t fat32;
	}bs;
}__attribute__((packed)) fat_BS_t;

int main(int argc, const char * argv[])
{
	fat_BS_t fat12;
	char buf[16];
	int f1;
	// 1. Open the file
	if(argc < 2)
	{
	printf("No file given");
	return -1;
	}
	int i;
	int n_char=0;
	f1 = open(argv[1], O_RDONLY);
	
	ssize_t read_things;
	printf("Binary offset: ");
	read_things = read(f1, buf, 3);
	for(i =0; i<3; i++)
	{
		fat12.bpm.bootjmp[i] = buf[i];
		printf("%x \t", fat12.bpm.bootjmp[i]);
	}
	printf("\n");
	printf("Volume Label: ");
	read_things = read(f1, buf, 8);
	for(i =0; i<8; i++)
	{
		fat12.bpm.oem_name[i] = buf[i];
		printf("%c \t", fat12.bpm.oem_name[i]);
	}
	
	printf("\n");
	printf("Bytes/Sector: ");
	read_things = read(f1, buf, 2);
	uint8_t a = buf[0];
	uint8_t b = buf[1];
	uint16_t c = swap_endian(a, b);
	fat12.bpm.bytes_per_sector = c;
	printf("%d \t", fat12.bpm.bytes_per_sector);

	printf("\n");
	printf("Sector/Cluster: ");
	read_things = read(f1, buf, 1);
	fat12.bpm.sectors_per_cluster = buf[0];
	printf("%d \t", fat12.bpm.sectors_per_cluster);
	
	printf("\n");
	printf("Reserved Sector: ");
	read_things = read(f1, buf, 2);
	a = buf[0];
	b = buf[1];
	c = swap_endian(a, b);
	fat12.bpm.reserved_sector_count = c;
	printf("%d \t", fat12.bpm.reserved_sector_count);
	
	printf("\n");
	printf("No. of Fats: ");
	read_things = read(f1, buf, 1);
	fat12.bpm.table_count = buf[0];
	printf("%d \t", fat12.bpm.table_count);
	
	printf("\n");
	printf("No. of Root Directory Entries: ");
	read_things = read(f1, buf, 2);
	a = buf[0];
	b = buf[1];
	c = swap_endian(a, b);
	fat12.bpm.root_entry_count = c;
	printf("%d \t", fat12.bpm.root_entry_count);
	
	printf("\n");
	printf("No. of logical sector: ");
	read_things = read(f1, buf, 2);
	a = buf[0];
	b = buf[1];
	c = swap_endian(a, b);
	fat12.bpm.total_sectors_16 = c;
	printf("%d \t", fat12.bpm.total_sectors_16);
	
	printf("\n");
	printf("Medium Descriptor: ");
	read_things = read(f1, buf, 1);
	fat12.bpm.media_type = buf[0];
	printf("%x \t", fat12.bpm.media_type);
	
	printf("\n");
	printf("Sectors per fat: ");
	read_things = read(f1, buf, 2);
	a = buf[0];
	b = buf[1];
	c = swap_endian(a, b);
	fat12.bpm.table_size_16 = c;
	printf("%d \t", fat12.bpm.table_size_16);
	
	printf("\n");
	printf("Sectors per track: ");
	read_things = read(f1, buf, 2);
	a = buf[0];
	b = buf[1];
	c = swap_endian(a, b);
	fat12.bpm.sectors_per_track = c;
	printf("%d \t", fat12.bpm.sectors_per_track);
	
	printf("\n");
	printf("No. of heads: ");
	read_things = read(f1, buf, 2);
	a = buf[0];
	b = buf[1];
	c = swap_endian(a, b);
	fat12.bpm.head_side_count = c;
	printf("%d \t", fat12.bpm.head_side_count);
	
	printf("\n");
	printf("No. of hidden Sectors: ");
	read_things = read(f1, buf, 2);
	a = buf[0];
	b = buf[1];
	c = swap_endian(a, b);
	fat12.bpm.hidden_sector_count = c;
	printf("%d \t", fat12.bpm.hidden_sector_count);
	uint8_t d = buf[2];
	uint8_t e = buf[3];
	uint16_t f = swap_endian(d, e);
	//even though in the lab report the length is given as 4, but on the website, the length is 4. Moreover, the struct also has uint32_t.
	fat12.bpm.total_sectors_32 = f;
	printf("%d \t", fat12.bpm.hidden_sector_count); 
	
	printf("\n");
	printf("No. of total sectors: ");
	read_things = read(f1, buf, 4);
	a = buf[0];
	b = buf[1];
	c = swap_endian(a, b);
	fat12.bpm.total_sectors_32 = c;
	printf("%d \t", fat12.bpm.total_sectors_32);
	d = buf[2];
	e = buf[3];
	f = swap_endian(d, e);
	fat12.bpm.total_sectors_32 = f;
	printf("%d \t", fat12.bpm.total_sectors_32);
	printf("\n");	
	close(f1);
	// 2. read the boot sector and save the values into the struct
	// 3. print out the elements of the struct
	// Good luck!
//	uint8_t a = 0x95;
//	uint8_t b = 0x22;
//	uint16_t c = swap_endian(a, b);
//	printf("\nc = %x\n", c);
	return 0;
}


