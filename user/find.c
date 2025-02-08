#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
char *
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), '\0', DIRSIZ-strlen(p));
  return buf;
}
int find(char * pt, char *filename)
{
    
    char *path = pt;
    char *fn = filename;
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;
    if ((fd = open(path, 0)) < 0)
    {
        printf("can not open %s", path);
        return 1;
    }
    if (fstat(fd, &st) < 0)
    {
        //fprintf(2, "ls: cannot stat %s\n", path);
        close(fd);
        return 1;
    }

    switch (st.type)
    {
    case T_DEVICE:
    case T_FILE:
        // 如果是文件 or 设备则检查是否与提供的名字一致
        char *name = fmtname(path);
        if (!strcmp(name, filename))
        {
            printf("%s\n",path);
        }
        break;

    case T_DIR:
        strcpy(buf, path);
        p = buf+strlen(buf);
        *p++ = '/';
        while (read(fd, &de, sizeof(de)) == sizeof(de))
        {
            // 屏蔽.  ..两个目录 
            if(!strcmp(de.name, ".") || !strcmp(de.name, ".."))   
                continue;
            if (de.inum == 0)
                continue;
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;
            if (stat(buf, &st) < 0)
            {   
                printf("ls: cannot stat %s\n", buf);
                continue;
            }
            find(buf, fn);
            //printf("%s %d %d %d\n", fmtname(buf), st.type, st.ino, st.size);
        }
        break;
    }
    close(fd);
    return 0;

}
int main(int argc, char *argv[])
{
    if (argc <= 1)
    {
        printf("wrong arg num");
        exit(1);
    }
    char *pt = argv[1];
    char *fn = argv[2];
    find(pt, fn);
    exit(0);
}