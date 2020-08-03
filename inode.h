struct inode{
    unsigned short flags;
    char nlinks;
    char uid;
    char gid;
    unsigned short size0;
    unsigned short size1;
    unsigned int addr[11];
    unsigned int actime;
    unsigned int modtime;
};