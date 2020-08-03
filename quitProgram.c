
void quitProgram(){
     
    if(sb.fmod==1){
        sb.fmod = 0;
        printf("Saving\n");
        if(lseek(fd,1024,SEEK_SET)<=0){
            printf("Some unknown error occurred!");
            exit(-1);
        }
        if(write(fd,&sb,sizeof(sb))<=0){
            printf("Some unknown error occurred!");
            exit(-1);
        }
    }
    printf("Quitting\n");
    exit(0);
}