OVERVIEW:
    We are creating a file system in this project.
    We have to Run the fsaccess.c , after which we will be asked to enter a command.
    The first command to be entered is " initfs path fsize isize"
    All the commands we call are handled in util.c by the handleCommand(command) method.
    The available commands are:
        - initfs path fsize isize
        - testinit
        - listfree 
        - listifree 
        - q     (save and quit)
        - loadfs
    To compile and run the program:
        1. Execute > cc fsaccess.c
        2. A new file a.out would be created
        3. Execute > ./a.out
        4. Next you need to either call initfs to initialize a new FS or loadfs to load the FS from an existing file.
        5. If using initfs, make sure to create a new file by using the touch command and pass the path to this file to initfs. The instructions to do this are mentioned below in the description of initfs.
About the file system:
    - SuperBlock
        The size of the superblock in out file system has been increased to 1024 bytes

        isize ( 4bytes), fsize ( 4bytes) , nfree( 2bytes), free[150] (600bytes), ninode(2bytes)
        inode[100](400bytes), flock (1byte), ilock(1byte), fmod (1byte), time (4bytes)

        isize is the number of inodes present in the file system.

        data block #0 is the first data block.

        inode #1 is the first inode

        free[] and inode[] are being used similar to their use in v6 file system.

    - inodes
        Each inode in our file system has been increased to 64 bytes.

        We can store 16 inodes in each block.

        flags (2bytes), nlinks (1bytes),uid(1bytes) , gid(1bytes) , size0(2bytes) , size1(2bytes) 
        addr[11](44bytes) , actime(4bytes) , modtime(4bytes)  

        The addr[11] has been updated in this file system.
        If the file is a small file, then we direct data block numbers will be stored in addr[]
        If it is a large file, then the first 10 indexes are used to store direct data block address and the last index will used to store address of a triple indirect block which will enable us to save upto 256*256*256 = 16777216 data blocks.
        In each block we can store upto  256 data block numbers .

All the commands available:
    initfs
        Before running initfs, you need to create a new file using touch command from the terminal.
        Then path to this file can be passed as an argument to initfs command.
        - Usage: initfs path fsize isize   
            the initfs command takes in 3 arguments
            path - the path to the file 
            fsize - the size of the disk(no.of blocks)
            isize - number of inodes
        - This command initializes the file system by leaving 0th block for boot loader
          1st block for super block
          2 - x blocks will be dedicated for inodes
          x - fsize blocks will be data blocks
        - inode[] will be loaded will 100 free inodes and ninode will be given value of 100.
        - We will iterate through all the data blocks and add them to free[].
            when the free[] is full , then we store values of the data blocks in the new data block 
            and store the value of new data block in free[0]. Note: the value of nfree is changed to 1.
        - creates a root directory and assigns inode-1 and datblock-0 to it.
            In the data block the files . , .. are stored for each of which inode values are 1.
    testinit 
        - This command is used to check if initfs worked correctly
        - it displays:
                isize
                fsize
                nfree
                free[]      
                ninode
                inode[]
                fmod
                time
                the root inode values and the values in the data block of the root
    listfree    
        - prints all the free data block numbers
    listifree   
        - prints all the free inode numbers
    q
        - saves the state of the disk and quits the program.
    loadfs
        - load the file system from an existing file on disk
        - Usage: loadfs path
            - path is the path to file from which to load the FS

    
