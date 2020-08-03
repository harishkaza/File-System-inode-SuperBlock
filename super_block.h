struct super_block{

    unsigned int isize;
    unsigned int fsize;
    unsigned short nfree;
    unsigned int free[150];
    unsigned short ninode;
    unsigned int inode[100];
    char flock;
    char ilock;
    char fmod;
    unsigned int time;
};