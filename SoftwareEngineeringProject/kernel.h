#ifndef KERNEL_H
#define KERNEL_H

#include<QMainWindow>
#include<QMessageBox>
#include<qfileinfo.h>
#include<QString>
#include<string>
#include<vector>
#include<queue>
#include<iostream>          //console
#include<dirent.h>          //search file
#include<sys/stat.h>        //get file stat
#include<sys/types.h>       //get file stat
#include<unistd.h>          //get file stat
#include<pwd.h>             //get username
#include<unistd.h>          //get username
#include<grp.h>             //get groupname
#include<openssl/modes.h>   //encrypt
#include<openssl/aes.h>     //encrypt
#include<utime.h>           //change last modified time
#include<fcntl.h>           //open files
#include<time.h>            //get last modified time

using namespace std;

const int BuffSize = 512;
const int ZipLen = 8;

int fromlen;
FILE *in, *out;
unsigned char buff[BuffSize + 1];
unsigned char outbuff[BuffSize + 1];
int pwd[(1 << ZipLen)+ 1], pwdlen[(1 << ZipLen) +1];

struct UstarHeader
{
    char name[100];         //0
    char mode[8];           //100
    char uid[8];            //108
    char gid[8];            //116
    char size[12];          //124
    char mtime[12];         //136
    char cksum[8];          //148
    char typeflag;          //156
    char linkname[100];     //157
    char magic[8];          //257
    char uname[32];         //265
    char gname[32];         //297
    char devmajor[8];       //329
    char devminor[8];       //337
    char prefix[155];       //345
    char fill[12];          //5001
    void clear()
    {
        memset(this, 0, sizeof(struct UstarHeader));
        strcpy(name, "./");
        strcpy(magic, "ustar  ");
    }
    void init(struct stat st, char *from)
    {
        strcat(name+2, from+fromlen+1);
        for(int i = 0; i <= 6; i++)
            mode[i] = '0';
        mode[3] += 4 * (bool)(st.st_mode & S_ISUID) + 2 * (bool)(st.st_mode & S_ISGID) + (bool)(st.st_mode & S_ISVTX);
        mode[4] += 4 * (bool)(st.st_mode & S_IRUSR) + 2 * (bool)(st.st_mode & S_IWUSR) + (bool)(st.st_mode & S_IXUSR);
        mode[5] += 4 * (bool)(st.st_mode & S_IRGRP) + 2 * (bool)(st.st_mode & S_IWGRP) + (bool)(st.st_mode & S_IXGRP);
        mode[6] += 4 * (bool)(st.st_mode & S_IROTH) + 2 * (bool)(st.st_mode & S_IWOTH) + (bool)(st.st_mode & S_IXOTH);
        struct passwd *pw = getpwuid(st.st_uid);
        struct group *grp = getgrgid(st.st_gid);
        int tmp = st.st_uid;
        strcpy(uname,pw->pw_name);
        strcpy(gname,grp->gr_name);
        printf("uid:%d\n", tmp);
        for(int i = 6; i >=0; i--)
        {
            uid[i] = '0' + tmp % 8;
            tmp /= 8;
        }
        tmp = st.st_gid;
        printf("gid:%d\n", tmp);
        for(int i = 6; i >= 0; i--)
        {
            gid[i] = '0' + tmp % 8;
            tmp /= 8;
        }
        tmp = st.st_size;
        if((st.st_mode & S_IFMT) == S_IFDIR)
        {
            for(int i = 10 ; i >= 0; i--)
            {
                size[i] = '0';
            }
        }
        else
        {
            for(int i = 10; i >= 0; i--)
            {
                size[i] = '0' + tmp % 8;
                tmp /= 8;
            }
        }
        tmp = st.st_mtime;
        for(int i = 10; i >= 0; i--)
        {
            mtime[i] = '0' + tmp % 8;
            tmp /= 8;
        }
        switch(st.st_mode & S_IFMT)
        {
            case S_IFREG:
                typeflag = '0';
                break;
            //IFLNK ISLNK
            case S_IFCHR:
                typeflag = '3';
                break;
            case S_IFBLK:
                typeflag = '4';
                break;
            case S_IFDIR:
                typeflag = '5';
                if(name[strlen(name) - 1] != '/')
                    strcat(name + strlen(name) , "/");
                break;
            case S_IFIFO:
                typeflag = '6';
                break;
            default:
                typeflag = 'A';
                break;
        }
        tmp = 256;
        for(int i = 0; i < 100; i++)tmp += name[i];
        for(int i = 0; i < 8; i++)tmp += mode[i];
        for(int i = 0; i < 8; i++)tmp += uid[i];
        for(int i = 0; i < 8; i++)tmp += gid[i];
        for(int i = 0; i < 12; i++)tmp += size[i];
        for(int i = 0; i < 12; i++)tmp += mtime[i];
        tmp += typeflag;
        for(int i = 0; i < 100; i++)tmp += linkname[i];
        for(int i = 0; i < 8; i++)tmp += magic[i];
        for(int i = 0; i < 32; i++)tmp += uname[i];
        for(int i = 0; i < 32; i++)tmp += gname[i];
        for(int i = 0; i < 8; i++)tmp += devmajor[i];
        for(int i = 0; i < 8; i++)tmp += devminor[i];
        for(int i = 0; i < 155; i++)tmp += prefix[i];
//        printf("%d\n", tmp);
        for(int i = 5;i >= 0; i--)
        {
            cksum[i] = '0' + tmp % 8;
            tmp /= 8;
        }
        cksum[7] = ' ';
    }
    void write()
    {
        fwrite(name, sizeof(char), 100, out);
        fwrite(mode, sizeof(char), 8, out);
        fwrite(uid, sizeof(char), 8, out);
        fwrite(gid, sizeof(char), 8, out);
        fwrite(size, sizeof(char), 12, out);
        fwrite(mtime, sizeof(char), 12, out);
        fwrite(cksum, sizeof(char), 8, out);
        fwrite(&typeflag, sizeof(char), 1, out);
        fwrite(linkname, sizeof(char), 100, out);
        fwrite(magic, sizeof(char), 8, out);
        fwrite(uname, sizeof(char), 32, out);
        fwrite(gname, sizeof(char), 32, out);
        fwrite(devmajor, sizeof(char), 8, out);
        fwrite(devminor, sizeof(char), 8, out);
        fwrite(prefix, sizeof(char), 155, out);
        fwrite(fill, sizeof(char), 12, out);
    }
}UH;

struct hufnode
{
    int cnt, l, r, id;
    hufnode(int cnt = 0, int l = -1, int r = -1, int id = 0): cnt(cnt), l(l), r(r), id(id){}
    bool operator > (const hufnode &rhs)const
    {
        return cnt > rhs.cnt;
    }
}p[513];

struct trienode
{
    int ch[2], val;
    bool isval;

}t[513];

struct filter
{
    int type;//0 null 1 path, 2 name, 3type, 4time
    bool isblack;//
    QVector<QString>path, name, suff;
    time_t dayl, dayr;
    void clear()
    {
        type = 0;
        isblack = 1;
        path.clear();
        name.clear();
        suff.clear();
        dayl = dayr = 0;
    }
    filter()
    {
        clear();
    }
}FT;

inline int pack(char *from, char *to, bool jump = 0, bool start = 1)
{
    struct stat st;
    stat(from, &st);
    in = fopen(from, "rb");
    if(in == NULL)
    {
        return 5;
    }
    if(start == 0)out = fopen(to, "ab");
    else
    {
        if(FT.type == 1)
        {
            jump = !FT.isblack;
        }
        fromlen = strlen(from);
        out = fopen(to, "wb");
    }
    if(FT.type == 0)
    {
    }
    else if(FT.type == 1)
    {
        if((st.st_mode & S_IFMT) == S_IFDIR)
        {
            QString pathstr(from);
            bool ok = 0;
            for(auto u: FT.path)
            {
                if(from == u)
                {
                    ok = 1;
                    break;
                }
            }
            if(FT.isblack && ok)
            {
                fclose(in);
                fclose(out);
                return 0;
            }
            if(!FT.isblack)
            {
                jump &= !ok;
            }
        }
    }
    else if(FT.type == 2)
    {
        if((st.st_mode & S_IFMT) == S_IFREG)
        {
            QString pathstr(from);
            QFileInfo file(pathstr);
            bool ok = 0;
            for(auto u: FT.name)
            {
                if(u == file.baseName())
                {
                    ok = 1;
                    break;
                }
            }
            if(ok ^ (!FT.isblack))
            {
                return 0;
            }
        }
    }
    else if(FT.type == 3)
    {
//        puts("!");
        if((st.st_mode & S_IFMT) == S_IFREG)
        {
//            puts("@");
            QString pathstr(from);
            QFileInfo file(pathstr);
            bool ok = 0;
            for(auto u: FT.suff)
            {
                qDebug() << pathstr << " " << file.suffix() << " " << u << endl;
                if(u == file.suffix())
                {
                    ok = 1;
                    break;
                }
            }
            if(ok ^ (!FT.isblack))
            {
                return 0;
            }
        }
    }
    else if(FT.type == 4)
    {
        if((st.st_mode & S_IFMT) != S_IFDIR)
        {
//            printf("%lld %lld %lld\n", FT.dayl, st.st_mtime, FT.dayr, (FT.dayl <= st.st_mtime && st.st_mtime <= FT.dayr));
            if((FT.dayl <= st.st_mtime && st.st_mtime <= FT.dayr) ^ (!FT.isblack))
            {
                return 0;
            }
        }
    }
//    printf("%s %d\n", from, jump);
    if((st.st_mode & S_IFMT) == S_IFDIR)
    {
        UH.clear();
        UH.init(st, from);
        UH.write();
    }
    else if(jump == 0)
    {
        UH.clear();
        UH.init(st, from);
        UH.write();
        if((st.st_mode & S_IFMT) == S_IFREG)
        {
            while(!feof(in))
            {
                fread(buff, sizeof(char), BuffSize, in);
                fwrite(buff, sizeof(char), BuffSize, out);
            }
        }
    }
    fclose(in);
    fclose(out);
    if((st.st_mode & S_IFMT) == S_IFDIR)
    {
        DIR *dirp;
        dirent *dp;
        dirp = opendir(from);
        while((dp = readdir(dirp)) != NULL)
        {
            int lend = strlen(dp->d_name);
            if((lend == 1 && (dp->d_name)[0] == '.') || (lend == 2 && (dp->d_name)[0] == '.' && (dp->d_name)[1] == '.'))
                continue;
//            printf("%s\n", dp->d_name);

            char *path = (char*) malloc(strlen(from) + strlen(dp->d_name) + 2);
            strcpy(path, from);
            strcat(path, "/");
            strcat(path, dp->d_name);
            int ret = 0;
            if(ret = pack(path, to, jump, 0))
            {
                return ret;
            }
            free(path);
        }
        closedir(dirp);
    }
    if(start == 1)
    {
        out = fopen(to, "ab");
        memset(buff, 0, sizeof buff);
        fseek(out, 0, SEEK_END);
        for(int i = 1; i <= 2; i++)
        {
            fwrite(buff, sizeof(char), BuffSize, out);
        }
        fclose(out);
    }
    return 0;
}

bool isdirempty(char *path)
{
    DIR *dirp;
    dirent *dp;
    dirp = opendir(path);
    bool isempty = 1;
    while((dp = readdir(dirp)) != NULL)
    {
        int lend = strlen(dp->d_name);
        if((lend == 1 && (dp->d_name)[0] == '.') || (lend == 2 && (dp->d_name)[0] == '.' && (dp->d_name)[1] == '.'))
            continue;
        char *next = (char*) malloc(strlen(path) + strlen(dp->d_name) + 2);
        strcpy(next, path);
        strcat(next, "/");
        strcat(next, dp->d_name);
        if(dp->d_type == 4)
        {
            if(isdirempty(next))
            {
                rmdir(next);
            }
            else
            {
                isempty = 0;
            }
        }
        else
        {
            isempty = 0;
        }
        free(next);
    }
    closedir(dirp);
    printf("%d %s\n",isempty, path);
    return isempty;
}

inline int unpack(char *from, char *to, bool clr)
{
//    puts("!");
    in = fopen(from, "rb");
    if(in == NULL)
    {
        return 5;
    }
    while(!feof(in))
    {
//        printf("{%d}\n",ftell(in));
        int size = 0, tmp = 256;
        uid_t uid = 0;
        gid_t gid = 0;
        memset(&UH, 0, sizeof UH);
        fread(UH.name, 1, 100, in);
        fread(UH.mode, 1, 8, in);
        fread(UH.uid, 1, 8, in);
        fread(UH.gid, 1, 8, in);
        fread(UH.size, 1, 12, in);
        fread(UH.mtime, 1, 12, in);
        fread(UH.cksum, 1, 8, in);
        fread(&UH.typeflag, 1, 1, in);
        fread(UH.linkname, 1, 100, in);
        fread(UH.magic, 1, 8, in);
        fread(UH.uname, 1, 32, in);
        fread(UH.gname, 1, 32, in);
        fread(UH.devmajor, 1, 8, in);
        fread(UH.devminor, 1, 8, in);
        fread(UH.prefix, 1, 155, in);
        fread(UH.fill, 1, 12, in);
        if(UH.name[0] == 0)
        {
//            puts("JUMP");
            continue;
        }
//        puts(UH.magic);
//        for(int i = 0; i < strlen(UH.name); i++)
//        {
//            printf("!%d",UH.name[i]);
//        }
//        printf("%lld\n", ftell(in));
        if(strcmp(UH.magic, "ustar  ") != 0)
        {
            fclose(in);
            return 1;//Doesn't correspound to Ustar Format.
        }
        for(int i = 0; i < 100; i++)tmp += UH.name[i];
        for(int i = 0; i < 8; i++)tmp += UH.mode[i];
        for(int i = 0; i < 8; i++)tmp += UH.uid[i];
        for(int i = 0; i < 8; i++)tmp += UH.gid[i];
        for(int i = 0; i < 12; i++)tmp += UH.size[i];
        for(int i = 0; i < 12; i++)tmp += UH.mtime[i];
        tmp += UH.typeflag;
        for(int i = 0; i < 100; i++)tmp += UH.linkname[i];
        for(int i = 0; i < 8; i++)tmp += UH.magic[i];
        for(int i = 0; i < 32; i++)tmp += UH.uname[i];
        for(int i = 0; i < 32; i++)tmp += UH.gname[i];
        for(int i = 0; i < 8; i++)tmp += UH.devmajor[i];
        for(int i = 0; i < 8; i++)tmp += UH.devminor[i];
        for(int i = 0; i < 155; i++)tmp += UH.prefix[i];
        for(int i = 5; i >= 0; i--)
        {
            tmp -= UH.cksum[i] - '0';
            tmp /= 8;
        }
//        printf("(%lld)\n",ftell(in));
        if(tmp != 0)
        {
            fclose(in);
            return 2;//Checksum is wrong or data was modified.
        }
        for(int i = 0; i <= 10; i++)
        {
            size *= 8;
            size += UH.size[i] - '0';
        }
        for(int i = 0; i <= 6; i++)
        {
            uid *= 8;
            gid *= 8;
            uid += UH.uid[i] - '0';
            gid += UH.gid[i] - '0';
        }
        mode_t mode = 0;
        UH.mode[4] -= '0';
        UH.mode[5] -= '0';
        UH.mode[6] -= '0';
//        printf("%d %d %d\n",UH.mode[4], UH.mode[5], UH.mode[6]);
        if(UH.mode[3] & 4) mode |= S_ISUID;
        if(UH.mode[3] & 2) mode |= S_ISGID;
//        mode |= S_ISUID;
//        mode |= S_ISGID;
        if(UH.mode[3] & 1) mode |= S_ISVTX;
        if(UH.mode[4] & 4) mode |= S_IRUSR;
        if(UH.mode[4] & 2) mode |= S_IWUSR;
        if(UH.mode[4] & 1) mode |= S_IXUSR;
        if(UH.mode[5] & 4) mode |= S_IRGRP;
        if(UH.mode[5] & 2) mode |= S_IWGRP;
        if(UH.mode[5] & 1) mode |= S_IXGRP;
        if(UH.mode[6] & 4) mode |= S_IROTH;
        if(UH.mode[6] & 2) mode |= S_IWOTH;
        if(UH.mode[6] & 1) mode |= S_IXOTH;
        char *path = (char *)malloc(strlen(to) + strlen(UH.name) + 2);
        strcpy(path, to);
        strcat(path, UH.name + 1);
//        printf("%s\n", path);
        puts("!");
        if(UH.typeflag == '5')
        {
            if(NULL == opendir(path))
            {
                if(mkdir(path, mode) == -1)
                {
                    fclose(in);
                    return 6;
                }
            }
            else return 3;
            chmod(path, mode);
            chown(path, uid, gid);
//            printf("%d\n", feof(in));
//            time_t mtime = 0, atime = 0;
//            for(int i = 0; i <= 10; i++)
//            {
//                mtime = mtime * 8 + UH.mtime[i] - '0';
//            }
//            struct utimbuf timebuf;
//            timebuf.actime = atime;
//            timebuf.modtime = mtime;
//            utime(path, &timebuf);
        }
        else//if(UH.typeflag == 0)
        {
            out = fopen(path, "wb");
            int blk = (size + 511) / 512;
//            printf("blk = %d %s\n", blk, to);
            for(int i = 0; i < blk; i++)
            {
                fread(buff, 1, BuffSize, in);
                if(i == blk - 1)fwrite(buff, 1, size - 512 * (blk - 1), out);
                else fwrite(buff, 1, BuffSize, out);
            }
            fclose(out);
            printf("%s %d %d\n", path, uid, gid);
            chmod(path, mode);
            chown(path, uid, gid);
            time_t mtime = 0, atime = 0;
            for(int i = 0; i <= 10; i++)
            {
                mtime = mtime * 8 + UH.mtime[i] - '0';
            }
            struct utimbuf timebuf;
            timebuf.actime = atime;
            timebuf.modtime = mtime;
            utime(path, &timebuf);
        }
    }
    fclose(in);
    if(clr)
    {
        if(isdirempty(to))
        {
            rmdir(to);
        }
    }
    return 0;
}

inline void getpwd(int u, int pre = 0, int pos = 1, int dep = 0)
{
    if(p[u].l == -1 && p[u].r == -1)
    {
        pwd[u] = pre;
        pwdlen[u] = dep;
        return;
    }
    if(p[u].l != -1)
    {
        getpwd(p[u].l, pre << 1, pos, dep + 1);
    }
    if(p[u].r != -1)
    {
        getpwd(p[u].r, pre << 1 | 1, pos, dep + 1);
    }
}

inline void ecb(unsigned char *from, unsigned char *to, char *key, bool enc)
{
    int keylen = strlen(key);
    for(int i = keylen; i < 32; i++)
    {
        key[i] = '#';
    }
    AES_KEY aes_key;
    if(enc)
    {
        AES_set_encrypt_key((unsigned char*)key, 32 * 8, &aes_key);
        AES_ecb_encrypt((unsigned char*)from, (unsigned char*)to, &aes_key, AES_ENCRYPT);
    }
    else
    {
        AES_set_decrypt_key((unsigned char*)key, 32 * 8, &aes_key);
        AES_ecb_encrypt((unsigned char*)from, (unsigned char*)to, &aes_key, AES_DECRYPT);
    }
}

inline int hufzip(char *from, char *to, char *key, bool keeptmp)
{
    priority_queue<hufnode, vector<hufnode>, greater<hufnode>>Q;
    strcat(to, ".tmp");
    in = fopen(from, "rb");
    out = fopen(to, "wb");
    memset(p, 0, sizeof p);
    memset(pwd, 0, sizeof pwd);
    memset(pwdlen, 0, sizeof pwdlen);
    memset(outbuff, 0, sizeof outbuff);
    long long tot = 256;
    while(!feof(in))
    {
        fread(buff, sizeof(char), BuffSize, in);
        for(int i = 0; i < BuffSize; i++)
        {
            p[(unsigned char)buff[i]].cnt++;
        }
    }
    for(int i = 0; i < tot; i++)
    {
        p[i].id = i;
        p[i].l = -1;
        p[i].r = -1;
        if(!p[i].cnt)continue;
        Q.push(p[i]);
    }
    while(!Q.empty())
    {
        hufnode u = Q.top();Q.pop();
        if(Q.empty())break;
        hufnode v = Q.top();Q.pop();
        p[tot] = hufnode(u.cnt + v.cnt, u.id, v.id, tot);
        Q.push(p[tot++]);
    }
    getpwd(tot - 1);
//    for(int i = 0; i < (1 << ZipLen); i++)
//    {
//        printf("%d: %d %d\n", i, pwd[i], pwdlen[i]);
//    }
    char magic[10] = "huffman";
    fwrite(magic, 1, 8, out);
    fseek(in, 0, SEEK_SET);
    fseek(out, 12, SEEK_SET);
    tot = 0;
    for(int i = 0; i < 256; i++)tot += (p[i].cnt != 0);
    fwrite(&tot, 4, 1, out);
    for(int i = 0; i < 256; i++)
    {
        if(p[i].cnt == 0)continue;
        buff[0] = i;
        buff[1] = pwdlen[i];
        fwrite(buff, 1, 2, out);
        int tmp = pwd[i];
        for(int j = (pwdlen[i] - 1) / 8; j >= 0 ; j--)
        {
            buff[j] = tmp % 256;
            tmp /= 256;
        }
        fwrite(buff, (pwdlen[i] - 1) / 8 + 1, 1, out);
    }
    tot = 0;
    int pos = 0, postmp = 7;
    memset(outbuff, 0 ,sizeof outbuff);
    while(!feof(in))
    {
        fread(buff, 1, BuffSize, in);
        for(int i = 0; i < BuffSize; i++)
        {
            tot += pwdlen[buff[i]];
            for(int j = pwdlen[buff[i]] - 1; j >= 0; j--)
            {
                if(pos == BuffSize && postmp == 7)
                {
                    fwrite(outbuff, BuffSize, 1, out);
                    memset(outbuff, 0, sizeof outbuff);
                    pos = 0;
                    postmp = 7;
                }
                if((pwd[buff[i]] >> j) & 1)
                {
                    outbuff[pos] |= (1 << (postmp));
                }
                if(postmp == 0)pos++, postmp = 7;
                else postmp--;
            }
        }
    }
    if(!(pos == BuffSize && postmp == 7))
    {
        fwrite(outbuff, BuffSize, 1, out);
    }
//    printf("%lld\n", tot);
    fseek(out, 8, SEEK_SET);
    fwrite(&tot, 4, 1, out);
    struct stat st;
    stat(to, &st);
    fseek(out, 0, SEEK_END);
    if(st.st_size % 16 == 0)
    {
        memset(buff, 0, sizeof(char) * 16);
        fwrite(buff, 1, 16, out);
    }
    else
    {
        int a = 16 - st.st_size % 16;
        for(int i = 0; i < a; i++)buff[i] = a;
        fwrite(buff, 1, a, out);
    }
    fclose(in);
    fclose(out);
    if(!keeptmp)remove(from);
    strcpy(from, to);
    to[strlen(to) - 4] = 0;
    in = fopen(from, "rb");
    out = fopen(to, "wb");
    while(!feof(in))
    {
        fread(buff, 1, 16, in);
        ecb(buff, buff + 16, key, 1);
        fwrite(buff + 16, 1, 16, out);
    }
    fclose(in);
    fclose(out);
    if(!keeptmp)remove(from);
    return 0;
}
//        /home/stevenzhang/Desktop/files/from.tar
inline void buildtrie(unsigned char v, int &tot)
{
//    puts("--------------------------");
    int u = 0;
    for(int i = pwdlen[v] - 1; i >= 0; i--)
    {
        if(!t[u].ch[((pwd[v]) >> i) & 1])
            t[u].ch[((pwd[v]) >> i) & 1] = ++tot;
        u = t[u].ch[((pwd[v]) >> i) & 1];
    }
    t[u].isval = 1;
    t[u].val = v;
}

inline int hufunzip(char *from, char *to, char *key, bool keeptmp)
{
    strcat(to, ".tmp");
    in = fopen(from, "rb");
    out = fopen(to, "wb");
    while(!feof(in))
    {
        fread(buff, 1, 16, in);
//        for(int i = 0; i < 16; i++)
//        {
//            printf("!%d ", buff[i]);
//        }puts("");
        ecb(buff, buff + 16, key, 0);
        fwrite(buff + 16, 1, 16, out);
//        for(int i = 0; i < 16; i++)
//        {
//            printf("%d ", buff[i + 16]);
//        }puts("");
    }
    fclose(in);
    fclose(out);
    memset(t, 0, sizeof t);
    memset(pwd, 0 ,sizeof pwd);
    memset(pwdlen, 0 ,sizeof pwdlen);
    int size, alpha, tot = 0;
    strcpy(from, to);
    to[strlen(to) - 4] = 0;
    in = fopen(from, "rb");
    out = fopen(to, "wb");
    char magic[10] = {0};
    fread(magic, 1, 8, in);
    if(strcmp(magic, "huffman") != 0)
    {
        if(!keeptmp)remove(from), remove(to);
        return 4;
    }
    fread(&size, 4, 1, in);
    fread(&alpha, 4, 1, in);
    for(int i = 0; i < alpha; i++)
    {
        fread(&buff, 1, 2, in);
        int x = buff[0], y = buff[1];
        pwdlen[x] = y;
        int bytes = (y - 1) / 8 + 1;
        fread(&buff, 1, bytes, in);
        for(int j = 0; j <= (y - 1) / 8; j++)
        {
            pwd[x] = pwd[x] * 256 + buff[j];
        }
        buildtrie(x, tot);
    }
//    for(int i = 0; i < 256; i++)
//    {
//        printf("%d %d %d\n", i, pwd[i], pwdlen[i]);
//    }
    int pos = BuffSize, postmp = 7;
    int cor = 0;
    int u = 0;
//    printf("%d %d\n", cor, size);
    while(cor < size)
    {
        if(pos == BuffSize)
        {
            fread(&buff, 1, BuffSize, in);
            pos = 0;
        }
        u = t[u].ch[(buff[pos] >> postmp) & 1];
        if(t[u].isval)
        {
            fwrite(&t[u].val, 1, 1, out);
            u = 0;
        }
        if(postmp == 0)
        {
            postmp = 7;
            pos++;
        }
        else postmp--;
        cor++;
    }
    fclose(in);
    fclose(out);
    if(!keeptmp)remove(from);
    return 0;
}

#endif // KERNEL_H
