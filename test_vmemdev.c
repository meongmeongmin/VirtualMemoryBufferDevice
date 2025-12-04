#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(void)
{
    const char *path = "/dev/vmemdev";  // 가상 디바이스 위치
    int fd = open(path, O_RDWR);
    if (fd < 0) 
    { 
        perror("open");
        return 1;
    }

    char cmd[256];
    char buf[1024];

    while (1)
    {
        printf("==========================================================================\n");
        printf("데이터 쓰기 → w 안녕하세요.\n");
        printf("데이터 읽기 → r\n");
        printf("데이터 읽기 → r 1000\n");
        printf("현재 오프셋 확인 → p\n");
        printf("오프셋 → sk 0 set\n");
        printf("오프셋 → sk 0 cur\n");
        printf("오프셋 → sk 0 end\n");
        printf("나가기 → quit\n");
        printf("==========================================================================\n");

        if (scanf("%s", cmd) <= 0)
            continue;

        // quit
        if (!strcmp(cmd, "quit"))
            break;

        // write
        else if (!strcmp(cmd, "w"))
        {
            getchar();
            fgets(buf, sizeof(buf), stdin);
            buf[strcspn(buf, "\n")] = '\0';

            ssize_t w = write(fd, buf, strlen(buf));
            printf("→ write %zd bytes\n", w);
        }

        // read
        else if (!strcmp(cmd, "r"))
        {
            int size = 0;
            char line[128];
            fgets(line, sizeof(line), stdin);

            if (sscanf(line, "%d", &size) != 1 || size > sizeof(buf) - 1)
                size = sizeof(buf) - 1;

            ssize_t r = read(fd, buf, size);
            buf[r] = '\0';

            printf("→ read %zd bytes:\n\"%s\"\n", r, buf);
        }

        // lseek
        else if (!strcmp(cmd, "sk"))
        {
            char mode[10];
            long val;
            scanf("%ld %s", &val, mode);

            int wh = SEEK_SET;
            if (!strcmp(mode, "set"))
                wh = SEEK_SET;
            else if (!strcmp(mode, "cur")) 
                wh = SEEK_CUR;
            else if (!strcmp(mode, "end")) 
                wh = SEEK_END;
            else
            {
                printf("→ SEEK 모드 오류 (SET/CUR/END)\n");
                continue;
            }

            off_t pos = lseek(fd, val, wh);
            if (pos < 0)
                perror("lseek");
            else
                printf("→ 새로운 오프셋: %ld\n", pos);
        }

        // 현재 오프셋 확인
        else if (!strcmp(cmd, "p"))
        {
            off_t pos = lseek(fd, 0, SEEK_CUR);
            printf("→ 현재 오프셋: %ld\n", pos);
        }

        else
            printf("→ 알 수 없는 명령: %s\n", cmd);
    }

    close(fd);
    return 0;
}