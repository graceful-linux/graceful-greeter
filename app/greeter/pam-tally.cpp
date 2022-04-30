//
// Created by dingjing on 4/5/22.
//

#include "pam-tally.h"

#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include <sys/mman.h>
#include <sys/stat.h>

#include <QDebug>

char shm_tally_real [128];

#define FILE_MODE (S_IRUSR | S_IWUSR)

#define CONFIG_FILE         "/usr/share/lightdm/lightdm.conf.d/graceful-setting.conf"

struct tallyLog
{
    char            fail_line[52];  /* rhost or tty of last failure */
    uint16_t        reserved;       /* reserved for future use */
    uint16_t        fail_cnt;       /* failures since last success */
    uint64_t        fail_time;      /* time of last failure */
};

static int get_is_open_other_authentication()
{
    char buf[128];
    FILE *config_file;

    if ((config_file = fopen(CONFIG_FILE, "r")) == nullptr) {
        return 0;
    }

    int open_other_authentication = 0;
    while (fgets(buf, sizeof(buf), config_file)) {
        if (strlen(buf) == 0 || buf[0] == '#') {
            memset((void*) buf, 0, sizeof(buf));
            continue;
        }
        if (buf[strlen(buf)-1] == '\n')
            buf[strlen(buf)-1] = '\0';

        char *p = strchr(buf, '=');
        if(!p)
            continue;
        *p = '\0';

        size_t len = strlen(buf);
        if (len == 0)
            continue;
        //去掉=之前的空格
        while (len--)
            if (buf[len] == ' ' || buf[len] == '\t')
                buf[len] = '\0';
        if (strcmp(buf, "open-other-authentication") != 0)
            continue;

        p++;
        len = strlen(p);
        if (len == 0)
            break;
        //去掉等号之后的空格
        while (*p == ' ' || *p == '\t') {
            p++;
            len--;
        }
        //去掉尾部空格
        while (len--)
            if (*(p+len) == ' ' || *(p+len) == '\t')
                *(p+len) = '\0';

        if (*p == '0')
            break;
        if (*p == '1') {
            open_other_authentication = 1;
            break;
        }
    }

    fclose (config_file);
    return open_other_authentication;
}

static int get_pam_tally (int *deny, int *unlock_time , int *root_unlock_time)
{
    char buf[128];
    FILE *auth_file;

    if ((auth_file = fopen("/etc/pam.d/common-auth", "r")) == nullptr)
        return -1;

    while (fgets(buf, sizeof(buf), auth_file)) {
        if (strlen(buf) == 0 || buf[0] == '#')
            continue;
        if (!strstr(buf, "deny"))
            continue;

        char *ptr = strtok(buf, " \t");
        while (ptr) {
            if (strncmp(ptr, "deny=", 5)==0){
                sscanf(ptr, "deny=%d", deny);
            }
            if (strncmp(ptr, "unlock_time=", 12)==0){
                sscanf(ptr, "unlock_time=%d", unlock_time);
            }
            if (strncmp(ptr, "root_unlock_time=", 17)==0){
                sscanf(ptr, "root_unlock_time=%d", root_unlock_time);
            }
            ptr = strtok(nullptr, " \t");
        }
        return 1;
    }
    return 0;
}

static void set_shm_tally_real()
{
    sprintf (shm_tally_real, "%s_%d", SHM_TALLY, getuid());
}

int pam_tally_init ()
{
    int fd;
    int deny = 0, unlock_time = 0 , root_unlock_time = 0;
    pam_tally *tally_ptr;

    set_shm_tally_real();

    qInfo() << "shm path: " << shm_tally_real;

    shm_unlink(shm_tally_real);

    if (get_is_open_other_authentication()) {
        return 0;
    }

    if (!get_pam_tally(&deny, &unlock_time,&root_unlock_time)) {
        return 0;
    }

    if (deny <= 0)
        deny = 0;
    if (unlock_time <= 0)
        unlock_time = 0;

    if ((fd = shm_open(shm_tally_real, O_RDWR | O_CREAT, FILE_MODE)) == -1) {
        qWarning() << "shm_open 错误: " << strerror(errno);
        return -1;
    }

    ftruncate(fd, sizeof(pam_tally));

    if ((tally_ptr = static_cast<pam_tally *>(mmap (nullptr, sizeof (pam_tally), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0))) == MAP_FAILED) {
        close(fd);
        return -1;
    }

    close(fd);
    tally_ptr->deny = deny;
    tally_ptr->unlock_time = unlock_time;
    tally_ptr->failed = 0;
    tally_ptr->lock_start_time = 0;
    tally_ptr->root_unlock_time = root_unlock_time;

    return 1;
}

static pam_tally* pam_tally_memory()
{
    int fd;
    pam_tally *tally_ptr;

    set_shm_tally_real();

    if ((fd = shm_open(shm_tally_real, O_RDWR, FILE_MODE)) == -1) {
        return nullptr;
    }

    if ((tally_ptr = static_cast<pam_tally *>(mmap (nullptr, sizeof (pam_tally), PROT_READ | PROT_WRITE, MAP_SHARED, fd,0))) == MAP_FAILED) {
        close(fd);
        return nullptr;
    }
    close(fd);
    return tally_ptr;
}

int pam_tally_is_enabled ()
{
    int fd;

    set_shm_tally_real();

    if ((fd = shm_open(shm_tally_real, O_RDONLY, FILE_MODE)) == -1) {
        qDebug() << "shm_open 错误: " << strerror(errno);
        close(fd);
        return 0;
    }

    pam_tally *tally_ptr;

    if ((tally_ptr = pam_tally_memory()) == nullptr) {
        close(fd);
        return -1;
    }

    if (tally_ptr->deny == 0 || tally_ptr->unlock_time == 0) {
        close(fd);
        return 0;
    }

    close(fd);

    return 1;
}

int pam_tally_add_failed ()
{
    pam_tally *tally_ptr;

    if ((tally_ptr = pam_tally_memory()) == nullptr)
        return -1;

    tally_ptr->failed++;

    //如果失败次数达到上限，开始计时
    if (tally_ptr->failed >= tally_ptr->deny)
        tally_ptr->lock_start_time = time(nullptr);

    return 0;
}

int pam_tally_clear_failed ()
{
    pam_tally *tally_ptr;

    if ((tally_ptr = pam_tally_memory()) == nullptr)
        return -1;

    tally_ptr->failed = 0;
    tally_ptr->lock_start_time = 0;
    return 0;
}

int pam_tally_failure_is_out()
{
    pam_tally *tally_ptr;

    if ((tally_ptr = pam_tally_memory()) == nullptr)
        return -1;

    return (tally_ptr->failed >= tally_ptr->deny ? 1 : 0);
}

int pam_tally_deny()
{
    pam_tally *tally_ptr;

    if ((tally_ptr = pam_tally_memory()) == nullptr)
        return -1;

    return tally_ptr->deny;
}

int pam_tally_failed_count()
{
    pam_tally *tally_ptr;

    if ((tally_ptr = pam_tally_memory()) == nullptr)
        return -1;

    return tally_ptr->failed;
}

int pam_tally_unlock_time()
{
    pam_tally *tally_ptr;

    if ((tally_ptr = pam_tally_memory()) == nullptr)
        return -1;

    qInfo() << "unlock time = " << tally_ptr->unlock_time;
    return tally_ptr->unlock_time;
}

int pam_tally_is_canUnlock()
{
    pam_tally *tally_ptr;

    if ((tally_ptr = pam_tally_memory()) == nullptr)
        return -1;

    if (tally_ptr->failed >= tally_ptr->deny && time(nullptr) - tally_ptr->lock_start_time < tally_ptr->unlock_time)
        return 0;
    return 1;
}

int pam_modutil_read(int fd, char *buffer, int count)
{
    int block, offset = 0;

    while (count > 0) {
        block = (int) read (fd, &buffer[offset], count);

        if (block < 0) {
            if (errno == EINTR) continue;
            return block;
        }
        if (block == 0) return offset;

        offset += block;
        count -= block;
    }

    return offset;
}

void get_tally (uid_t uid, int *tfile, struct tallyLog *tally)
{
    char filename[50]={0};
    sprintf (filename,"%s","/tmp/.tallylog");
    void* void_tally = tally;
    if ((*tfile = open(filename, O_RDONLY)) == -1) {
        qWarning() << "open tallyLog failed: " << strerror(errno);
        return ;
    }

    if (lseek(*tfile, (off_t)uid*(off_t)sizeof(*tally), SEEK_SET) == (off_t)-1) {
        qWarning() << "lseek tallyLog failed";
        close(*tfile);
        return ;
    }

    if (pam_modutil_read(*tfile, (char*) void_tally, sizeof(*tally)) != sizeof(*tally)) {
        qWarning() << "read tallyLog failed";
        memset(tally, 0, sizeof(*tally));
    }
    close(*tfile);
    tally->fail_line[sizeof(tally->fail_line)-1] = '\0';
}

int pam_tally_unlock_time_left(uid_t userID, int *fail_cnt,int *left_time, int *deny, int *fail_time, int *unlock_time1)
{
    pam_tally *tally_ptr;

    if((tally_ptr = pam_tally_memory()) == nullptr)
        return 0;

    int unlock_time = tally_ptr->unlock_time;
    *deny = tally_ptr->deny;

    if(unlock_time == 0)
        return 0;

    int tfile = -1;



    struct tallyLog tally{};
    tally.reserved = 0;
    tally.fail_cnt = 0;
    tally.fail_time = 0;

    get_tally (userID,&tfile,&tally);
    if(tally.fail_cnt<*deny)
        return 0;

    //连续输错，时间累加
    if (tally.fail_cnt >= (*deny)*2 && tally.fail_cnt < (*deny)*3){
        unlock_time = unlock_time*2;
    } else if(tally.fail_cnt >= (*deny)*3 && tally.fail_cnt < (*deny)*4){
        unlock_time = unlock_time*8;
    } else if (tally.fail_cnt >= (*deny)*4){
        *fail_cnt = 0xFFFF;//永久锁定
        return 0;
    }



    unsigned long unlock_time_left = unlock_time - ((long)time(nullptr) - tally.fail_time);
    *fail_time = (int) tally.fail_time;
    *unlock_time1 = unlock_time;

    *left_time = unlock_time_left > 0 ? (int) unlock_time_left : 0;
    *fail_cnt = tally.fail_cnt;

    return *left_time;
}

int pam_tally_root_unlock_time_left(uid_t userID, int *fail_cnt,int *left_time, int *deny1)
{

    pam_tally *tally_ptr;
    if((tally_ptr = pam_tally_memory()) == nullptr){
        return 0;
    }
    int root_unlock_time = tally_ptr->root_unlock_time;
    int deny = tally_ptr->deny;
    if(root_unlock_time == 0)
        return 0;

    int tfile = -1;
    struct tallyLog tally{};
    tally.reserved = 0;
    tally.fail_cnt = 0;
    tally.fail_time = 0;

    get_tally(userID,&tfile,&tally);
    if(tally.fail_cnt<deny)
        return 0;

    //连续输错，时间累加
    if (tally.fail_cnt >= (deny)*2 && tally.fail_cnt < (deny)*3){
        root_unlock_time = root_unlock_time*2;
    }
    else if(tally.fail_cnt >= (deny)*3 && tally.fail_cnt < (deny)*4){
        root_unlock_time = root_unlock_time*8;
    }
    else if (tally.fail_cnt >= (deny)*4){
        *fail_cnt = 0xFFFF;//永久锁定
        return 0;
    }

    unsigned long root_unlock_time_left = root_unlock_time - ((long)time(nullptr) - tally.fail_time);

    *left_time = root_unlock_time_left > 0 ? (int) root_unlock_time_left : 0;

    *fail_cnt = tally.fail_cnt;

    return *left_time;
}


