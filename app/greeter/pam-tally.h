//
// Created by dingjing on 4/5/22.
//

#ifndef GRACEFUL_GREETER_PAM_TALLY_H
#define GRACEFUL_GREETER_PAM_TALLY_H

#include <time.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SHM_TALLY   "/shm_tally"

struct _pam_tally
{
    int deny;                       // 失败次数上限
    int unlock_time;                // 普通用户失败次数达到上限后，多少秒之后才能解锁
    int root_unlock_time;           // root用户失败次数达到上限后，多少秒之后才能解锁
    int failed;                     // 当前失败的次数
    time_t lock_start_time;         // 失败次数达到上限后，开始计时
};
typedef struct _pam_tally pam_tally;

int pam_tally_init ();
int pam_tally_deny ();
int pam_tally_is_enabled ();
int pam_tally_add_failed ();
int pam_tally_unlock_time ();
int pam_tally_failed_count ();
int pam_tally_clear_failed ();
int pam_tally_is_canUnlock ();
int pam_tally_failure_is_out ();
int pam_tally_unlock_time_left (uid_t userID, int *fail_cnt, int *left_time, int *deny, int *fail_time, int *unlock_time);
int pam_tally_root_unlock_time_left (uid_t userID, int *fail_cnt, int *left_time, int *deny);

#ifdef __cplusplus
}
#endif


#endif //GRACEFUL_GREETER_PAM_TALLY_H
