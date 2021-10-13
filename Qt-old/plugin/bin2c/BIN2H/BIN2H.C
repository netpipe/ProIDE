#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <memory.h>
#include <fstream>
#include <malloc.h>

int output_filename(char *);

char outfile[]="xxxxxxxx.h";

int main(int argc, char *argv[])
{
   unsigned char *buffer;
   int  fcount,handle,handle1;
   unsigned long fsize;
   unsigned int i,j,k,loop;
   char odata[]="0xXX,";
   char start[]="unsigned char data[";
   char start1[16];
   char start2[]="]=\x0D\x0A{\x0D\x0A ";

   fprintf(stderr,"\nSequoia Binary File to 'C' Header File Conversion Utility Version 1.0\n");
   fprintf(stderr,"Copyright 1995 by Sequoia Advanced Technologies, Inc.\n");
   fprintf(stderr,"All Rights Reserved\n\n");
   fprintf(stderr,"May be freely distributed provided the above copyright\n");
   fprintf(stderr,"message is left intact.\n\n");
   if(argc==1)
   {
    fprintf(stderr,"\ausage: convert [filename]\n\n\n");
    return 1;
   }
   if((buffer=(char *)malloc(16384)) == NULL)
   {
    fprintf(stderr,"\aERROR: Cannot allocate 16k of memory needed to operate.\n");
    return 1;
   }
   if((handle = open(argv[1],O_BINARY | O_RDONLY)) == -1) 
   {
    fprintf(stderr,"\aERROR: Cannot open file \"%s\" for conversion.\n",argv[1]);
    free(buffer);
    return 1;
   }

   fsize=filelength(handle);
   fprintf(stderr,"Input filename: %s\n",argv[1]);
   fprintf(stderr,"Input filesize: %ld\n",fsize);

   handle1=output_filename(argv[1]);
   if(handle1==-1)
   {
    if(errno==EEXIST) 
    {
     fprintf(stderr,"\aERROR: Output file \"%s\" already exists!\n",outfile);
     fprintf(stderr,"       As a precaution, this utility will not overwrite.\n");
     fprintf(stderr,"       If you wish to use the output filename \"%s\",\n",outfile);
     fprintf(stderr,"       you must delete \"%s\", and run the utility again.\n",outfile);
    }
    else
     fprintf(stderr,"\aERROR: Cannot create output file.\n");
 
    free(buffer);
    close(handle);
    return 1;
   }


   loop=fsize/16380;
   
   sprintf(start1,"%ld",fsize);

   write(handle1,start,strlen(start));
   write(handle1,start1,strlen(start1));
   write(handle1,start2,strlen(start2));

   for(i=0;i<loop;i++)
   {
    read(handle,buffer,16380);
    for(j=0;j<1365;j++)
    {
     for(k=0;k<12;k++)
     {
      sprintf(odata,"0x%.2hX,",buffer[((j*12)+k)]);
      write(handle1,odata,5);
     }
     write(handle1,"\x0D\x0A ",3);
    }
    fsize-=16380;
   }

   read(handle,buffer,fsize);

   i=1;
   while(fsize)
   {
    sprintf(odata,"0x%.2hX,",buffer[i-1]);
    write(handle1,odata,5);
    if(i%12==0)
     write(handle1,"\x0D\x0A ",3);
    i++;
    fsize--;
   }
   lseek(handle1,-1,SEEK_END);
   write(handle1," \x0D\x0A};",5);
   close(handle);
   fsize=filelength(handle1);
   close(handle1);
   free(buffer);
   fprintf(stderr,"\nOutput filename: %s\n",outfile);
   fprintf(stderr,"Output filesize: %ld\n",fsize);
   return 0;
}

int output_filename(char *filename)
{
   char *ptr,*ptr1;
   int i;


   ptr=strrchr(filename,'\\');

   if(ptr!=0)
    ptr++;
   else
    ptr=filename;
   
   ptr1=strchr(ptr,'.');
   if(ptr1!=0)
    *ptr1=0;
   stpcpy(outfile,ptr);
   ptr=strchr(outfile,0);
   ptr[0]='.';
   ptr[1]='h';
   ptr[2]=0;
   return(open(outfile,O_BINARY | O_RDWR | O_CREAT | O_EXCL,S_IREAD | S_IWRITE));
}





