#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#ifndef MAJOR
    #define MAJOR(a) (((unsigned)(a))>>8)
#endif
#ifndef MINOR
    #define MINOR(a) ((a)&0xff) //&是将两个二进制的数逐位相与
#endif

#define MINIX_HEADER 32

#define DEFAULT_MAJOR_ROOT 3
#define DEFAULT_MINOR_ROOT 1

#define SETUP_SECTS 4

#define STRINGIFY(x) #x

void die(char *str)
{
    fprintf(stderr,"%s\n",str);
    exit(1);
}

int main(int argc, char **argv)
{
    int i,c,id;
    char buf[1024];
    char *baby = "baby";
    int major_root, minor_root;
    struct stat file;
//    echo_paras(argc,argv);

    /*
    参数如果不是4个或5个，直接报错退出
    参数如果是4个，根文件号用默认值
    参数如果是5个，第4个参数若为“FLOPPY”，则为0-0；若不是，则取该设备文件状态信息
    */
    if((argc != 4) && (argc != 5))
        die("Need more parameters");

    if(argc == 5)
    {
        if(strcmp(argv[4],"FLOPPY"))
        {
            if(stat(argv[4], &file))
            {
                perror(argv[4]);
                die("Couldn't stat root device");
            }
            // 分别得到该文件系统的 major minor
            major_root = MAJOR(file.st_rdev);
            minor_root = MINOR(file.st_rdev);
        }
        else
        {
            major_root = 0;
            minor_root = 0;
        }
    }
    else
    {
        major_root = DEFAULT_MAJOR_ROOT;
        minor_root = DEFAULT_MINOR_ROOT;
    }
    
    fprintf(stderr,"Root device is (%d,%d)\n", major_root, minor_root);

    // 若主设备号不是默认0，或软盘2，或硬盘3，则报错退出
    if( (major_root != 0) && (major_root != 2) && (major_root != 3) )
    {
        fprintf (stderr, "Illegal root device (major = %d)\n", major_root);
        die ("Bad root device --- major #");
    }


    // 检查boot

    for(i=0;i<sizeof(buf);i++) buf[i]=0;
    // 返回int型文件描述符，出错返回-1,错误代码存入errno中
    // 标准输入是0，标准输出是1，标准出错是2
    if((id=open(argv[1],O_RDONLY,0))<0)
        die("Unable to open Boot");
    // 返回实际读到的字节数
    if(read(id,buf,MINIX_HEADER) != MINIX_HEADER)
        die("Unable to read header of Boot");
    // 0x0301-执行文件魔数;0x10-可执行文件标志;0x04-cpu标识号
    if(((long *)buf)[0] != 0x04100301)
        die("Non-Minix header of Boot");
    // 判断头部字段后3字节是否正确
    if(((long *)buf)[1] != MINIX_HEADER)
        die("Non-Minix header of Boot");
    // 判断数据段长a_data 字段(long)内容是否为0。
    if(((long *) buf)[3] != 0)
        die ("Illegal data segment in Boot");
    // 判断堆a_bss 字段(long)内容是否为0。
    if(((long *) buf)[4] != 0)
        die ("Illegal bss in 'boot'");
    // 判断执行点a_entry 字段(long)内容是否为0。
    if(((long *) buf)[5] != 0)
        die ("Non-Minix header of 'boot'");
    // 判断符号表长字段a_sym 的内容是否为0。
    if(((long *) buf)[7] != 0)
        die ("Illegal symbol table in 'boot'");
    
    i=read(id,buf,sizeof(buf));
    fprintf(stderr,"Boot sector %d bytes.\n",i);

    if(i != 512)
        die("Boot sector must be exactly 512 bytes");
    if((*(unsigned short *)(buf+510)) != 0xAA55)
        die("Boot block hasn't got boot flag (0xAA55)");
    
    buf[508] = minor_root;
    buf[509] = major_root;

    // 将buf内存所指的512个字节写出到stdout,返回实际写入字符数
    i=write(1,buf,512);
    if(i != 512)
        die("Write call failed");
    
    close(id);
    fprintf(stderr,"========================\nboot/bootsect.s ALL OK !\n========================\n");

/*
    // 检查setup

	if ((id=open(argv[2],O_RDONLY,0))<0)
		die("Unable to open 'setup'");
	if (read(id,buf,MINIX_HEADER) != MINIX_HEADER)
		die("Unable to read header of 'setup'");
	if (((long *) buf)[0]!=0x04100301)
		die("Non-Minix header of 'setup'");
	if (((long *) buf)[1]!=MINIX_HEADER)
		die("Non-Minix header of 'setup'");
	if (((long *) buf)[3]!=0)
		die("Illegal data segment in 'setup'");
	if (((long *) buf)[4]!=0)
		die("Illegal bss in 'setup'");
	if (((long *) buf)[5] != 0)
		die("Non-Minix header of 'setup'");
	if (((long *) buf)[7] != 0)
		die("Illegal symbol table in 'setup'");
    for(i=0;(c=read(id,buf,sizeof(buf))>0); i+=c)
        if(write(1,buf,c)!=c)
            die("Write call failed");
    close(id);

    if (i > SETUP_SECTS*512)
		die("Setup exceeds " STRINGIFY(SETUP_SECTS)" sectors - rewrite build/boot/setup");

    fprintf(stderr,"Setup is %d bytes.\n",i);

    // 缓冲区 buf 清0
    for(c=0; c<sizeof(buf); c++)
        buf[c] = '\0';

    while(i < SETUP_SECTS*512)
    {
        c = SETUP_SECTS *512 - i;
        if(c > sizeof(buf))
            c = sizeof(buf);
        if(write(1,buf,c) != c)
            die("Write call failed");
        i += c;
    }
*/
    return 0;
}


