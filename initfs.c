#include <stdio.h>
#include <stdlib.h>
#include "super_block.h"
#include "inode.h"
#include <time.h>

struct file_def{
    int inum;
    char filename[28];
};
struct inode empty_inode;
int zeroes[150],i;
struct super_block sb;
int fd;

void initfs(char *path, int fsize, int isize){
    if(path==NULL || fsize == 0 || isize == 0){
        printf("Incorrect usage. Correct usage is: initfs /path/to/file disksize isize\n");
        return;
    }
    printf("Initializing FS with parameters: path: %s dsize: %d isize: %d\n", path, fsize, isize);

    if(fsize-get_inode_block(isize)<0) {
        printf("disksize not enough for the given isize\n");
        return;
    }

    fd = open(path,2);
    if(fd<=0){
        printf("File not found.");
        exit(-1);
    }
    
    sb.fsize = fsize;
    sb.isize = isize;
    sb.nfree = 0;
    sb.ninode = 0;
    sb.fmod = 1;
    sb.time = (unsigned)time(NULL);

    load_freeblock_initial();
    load_inode_initial();
    create_root();
}
//load_freeblock_initial is used to fill the free[] in superblock with free data blocks
//Note: If the free[] becomes full then all the values in it are stored in new data block 
//      and that block is stored in the data block number in free[0]
int load_freeblock_initial()
{
    int i;
    add_a_free_block(0);
    for (i=(sb.fsize)-1;i>get_inode_block(sb.isize)+1;i--)
    {
        add_a_free_block(i);
    }
    
    return 0;
}
//get_inode_block returns the block number in which inode inum is located
int get_inode_block(int inum)
{
    if(inum % 16 == 0)
    {
        return (inum/16)-1+2;
    }
    else
        return (inum/16)+2;
}
// loads the inode[] with the free inodes
int load_inode_initial()
{
    int i;
    for(i=2;i<=sb.isize;i++)
    {
        add_a_free_inode(i);
    }
    return 0;
}

// writes the i number inode into inode block
void write_inode (int i,struct inode ip)
{
    if(lseek(fd,2048+((i-1)*64),0)<=0){
        printf("Some unknown error occurred!");
        exit(-1);
    }
    if(write(fd,&ip.flags,sizeof(ip))<=0){
        printf("Some unknown error occurred!");
        exit(-1);
    }
}
// get_inode_position helps us to find the exact position of the inode inum
int get_inode_position(int inum)
{
    return 2048+((inum-1)*sizeof(struct inode));
}
// add_a_free_block method adds a free data block to the free[] of the super block.
// This method is called recursively in the initfs method to load all the available free data blocks into free[]
int add_a_free_block(int free_block_number)
{
    if (sb.nfree == 150)
    {
        if(lseek(fd, free_block_number*1024, SEEK_SET)<=0){
            printf("Some unknown error occurred!");
            exit(-1);
        }
        if(write(fd, &sb.nfree, sizeof(sb.nfree) + sizeof(sb.free))<=0){
            printf("Some unknown error occurred!");
            exit(-1);
        }
        for(i=1;i<151;i++)
        {
            sb.free[i-1] = 0;
        }
        sb.nfree = 0;
        //*sb.free=  *zeroes;
        
    }

    sb.free[sb.nfree] = free_block_number;
    sb.nfree++;
    sb.fmod = 1;
    sb.time = (unsigned)time(NULL);
    return 0;
}
// add_a_free_inode method is called after a file is deleted and 
// it clears the inode corresponding to that file adds it to inode[] in superblock
int add_a_free_inode(int inum)
{
    if(sb.ninode <100)
    {
        sb.inode[sb.ninode]=inum;
        sb.ninode++;
        sb.fmod = 1;
        sb.time = (unsigned)time(NULL);
    }
    clear_inode(inum);
    
    return 0;
}
//clear_inode method is used to clear all the information(data about file)
int clear_inode(int inum)
{
    if(lseek(fd,get_inode_position(inum),0) <=0 ){
        printf("Some unknown error occurred!");
        exit(-1);
    }
    if(write(fd,&empty_inode,sizeof(struct inode)) <=0 ){
        printf("Some unknown error occurred!");
        exit(-1);
    }
    return 0;
}
//this method prints all the available inodes by traversing through 
//all the inodes and checking if it is free
int get_free_inodes()
{
    
    struct inode i_temp;
    unsigned short c; 
    printf("Free inodes: ");
    for(i=0;i<sb.isize;i++)
    {
            if(lseek(fd,2048+(i*64),SEEK_SET)<=0){
                printf("Some unknown error occurred!");
                exit(-1);
            }
            if(read(fd,&i_temp,sizeof(struct inode))<=0){
                printf("Some unknown error occurred!");
                exit(-1);
            }
            c = i_temp.flags;
            if(c>>15 == 0)//then it is a free inode
            {
                printf("%d, ",i+1);
            }
    }

    printf("\n");

    return 0;
}

//create_root is used to create root directory.(inode = 1,datablock = 1)
//It contains two files with filenames . and ..,both pointing to root, i.e., their inodes are 1.
int create_root(){
    struct file_def this_file_def;
    this_file_def.inum = 1;
    strcpy(this_file_def.filename, ".");

    struct file_def parent_file_def;
    parent_file_def.inum = 1;
    strcpy(parent_file_def.filename,"..");

    struct file_def buff[2];
    buff[0] = this_file_def;
    buff[1] = parent_file_def;

    int data_block_position = get_inode_block(sb.isize)+1;

    if(lseek(fd, data_block_position*1024, SEEK_SET)<=0){
        printf("Some unknown error occurred!");
        exit(-1);
    }
    if(write(fd, buff, 2*sizeof(struct file_def))<=0){
        printf("Some unknown error occurred!");
        exit(-1);
    }

    struct inode root_inode;
    root_inode.flags = 49663;
    root_inode.addr[0] = data_block_position;
    root_inode.size1 = 64;
    root_inode.actime = (unsigned) time(NULL);
    root_inode.modtime = (unsigned) time(NULL);

    write_inode(1, root_inode);
}
// This method prints all the available data blocks
void get_all_free_blocks(){
    int allocated_block = allocate_a_data_block();
    while(allocated_block!=0){
        printf("%d ", allocated_block);
        allocated_block = allocate_a_data_block();
    }
    printf("Out of disk space\n");

}
//This method is used to allocate a data block whenever a file is to be extended
int allocate_a_data_block(){
    if(sb.nfree==0) return sb.free[sb.nfree];
    sb.nfree--;
    int free_block_num = sb.free[sb.nfree];
    sb.free[sb.nfree] = 0;
    sb.fmod = 1;
    sb.time = (unsigned)time(NULL);
    if(sb.nfree==0 && free_block_num!=0){
        printf("free array empty, going to: %d\n", free_block_num);
        if(lseek(fd, free_block_num*1024, SEEK_SET)<=0){
            printf("Some unknown error occurred!");
            exit(-1);
        }
        if(read(fd, &sb.nfree, sizeof(sb.nfree) + sizeof(sb.free))<=0){
            printf("Some unknown error occurred!");
            exit(-1);
        }
        //printf("read: %d", bytes_read);

    }
    return free_block_num;
}
//This is a test method to test that initfs worked properly
void test_init(){
    printf("isize: %d\n", sb.isize);
    printf("fsize: %d\n", sb.fsize);
    printf("nfree: %d\n", sb.nfree);
    printf("Superblock modified: %d\n", sb.fmod);
    printf("Superblock modified time: %d\n\n", sb.time);
    printf("free[]: ");
    for(i=sb.nfree-1; i>=0; i--){
        printf("%d, ", sb.free[i]);
    }
    printf("\n\n");

    printf("ninode: %d\n", sb.ninode);
    printf("inode[]: ");
    for(i = sb.ninode-1; i>=0; i--){
        printf("%d, ", sb.inode[i]);
    }
    printf("\n\n");

    struct inode root_inode;
    if(lseek(fd, get_inode_position(1), SEEK_SET)<=0){
        printf("Some unknown error occurred!");
        exit(-1);
    }
    if(read(fd, &root_inode, sizeof(root_inode))<=0){
        printf("Some unknown error occurred!");
        exit(-1);
    }

    printf("Root inode values: \n");
    printf("Flags: %d\n", root_inode.flags);
    printf("Root inode last access time: %d\n", root_inode.actime);
    printf("Root inode last modified time: %d\n", root_inode.modtime);
    printf("Block number pointed by root inode: %d\n",root_inode.addr[0]);

    printf("Contents of block number %d:\n", root_inode.addr[0]);

    if(lseek(fd, root_inode.addr[0]*1024, SEEK_SET)<=0){
        printf("Some unknown error occurred!");
        exit(-1);
    }

    struct file_def filedef;
    if(read(fd, &filedef, sizeof(filedef))<=0){
        printf("Some unknown error occurred!");
        exit(-1);
    }
    i = 1;
    while(filedef.inum!=0){
        printf("File %d: inode: %d name: %s\n", i++, filedef.inum, filedef.filename);
        int rs = read(fd, &filedef, sizeof(filedef));
        if(rs<=0){
            printf("Some unknown error occurred!");
            exit(-1);
        }
    }
}

//this method is used to load the file system from an existing file
int loadfs(char *path){
    if(path==NULL){
        printf("Incorrect usage. Correct usage is: loadfs /path/to/file\n");
        return;
    }
    printf("Loading FS with parameters: path: %s\n", path);

    //open the file
    fd = open(path,2);
    if(fd==-1){
        printf("File not found.");
        exit(-1);
    }

    int ls = lseek(fd,1024,SEEK_SET);
    if(ls<=0) {
        printf("Some unknown error occurred");
        exit(-1);
    }
    
    int rs = read(fd,&sb,sizeof(sb));
    if(rs<=0){
        printf("Some unknown error occurred");
        exit(-1);
    }

    sb.time = (unsigned)time(NULL);

    printf("loaded the FS. Calling testinit\n");

    test_init();

}