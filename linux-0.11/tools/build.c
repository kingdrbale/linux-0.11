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
    #define MINOR(a) ((a)&0xff) //&�ǽ����������Ƶ�����λ����
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
    �����������4����5����ֱ�ӱ����˳�
    ���������4�������ļ�����Ĭ��ֵ
    ���������5������4��������Ϊ��FLOPPY������Ϊ0-0�������ǣ���ȡ���豸�ļ�״̬��Ϣ
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
            // �ֱ�õ����ļ�ϵͳ�� major minor
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

    // �����豸�Ų���Ĭ��0��������2����Ӳ��3���򱨴��˳�
    if( (major_root != 0) && (major_root != 2) && (major_root != 3) )
    {
        fprintf (stderr, "Illegal root device (major = %d)\n", major_root);
        die ("Bad root device --- major #");
    }


    // ���boot

    for(i=0;i<sizeof(buf);i++) buf[i]=0;
    // ����int���ļ���������������-1,����������errno��
    // ��׼������0����׼�����1����׼������2
    if((id=open(argv[1],O_RDONLY,0))<0)
        die("Unable to open Boot");
    // ����ʵ�ʶ������ֽ���
    if(read(id,buf,MINIX_HEADER) != MINIX_HEADER)
        die("Unable to read header of Boot");
    // 0x0301-ִ���ļ�ħ��;0x10-��ִ���ļ���־;0x04-cpu��ʶ��
    if(((long *)buf)[0] != 0x04100301)
        die("Non-Minix header of Boot");
    // �ж�ͷ���ֶκ�3�ֽ��Ƿ���ȷ
    if(((long *)buf)[1] != MINIX_HEADER)
        die("Non-Minix header of Boot");
    // �ж����ݶγ�a_data �ֶ�(long)�����Ƿ�Ϊ0��
    if(((long *) buf)[3] != 0)
        die ("Illegal data segment in Boot");
    // �ж϶�a_bss �ֶ�(long)�����Ƿ�Ϊ0��
    if(((long *) buf)[4] != 0)
        die ("Illegal bss in 'boot'");
    // �ж�ִ�е�a_entry �ֶ�(long)�����Ƿ�Ϊ0��
    if(((long *) buf)[5] != 0)
        die ("Non-Minix header of 'boot'");
    // �жϷ��ű��ֶ�a_sym �������Ƿ�Ϊ0��
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

    // ��buf�ڴ���ָ��512���ֽ�д����stdout,����ʵ��д���ַ���
    i=write(1,buf,512);
    if(i != 512)
        die("Write call failed");
    
    close(id);
    fprintf(stderr,"========================\nboot/bootsect.s ALL OK !\n========================\n");

/*
    // ���setup

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

    // ������ buf ��0
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


