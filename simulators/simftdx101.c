// can run this using rigctl/rigctld and socat pty devices
#define _XOPEN_SOURCE 700
// since we are POSIX here we need this
#if 0
struct ip_mreq
{
    int dummy;
};
#endif

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include "../include/hamlib/rig.h"

#define BUFSIZE 256

float freqA = 14074000;
float freqB = 14074500;
int vfo = 0;
int ft = 0;
int modeA = 0xc;
int modeB = 0xc;
int vs = 0;
int tx = 0;
int ai = 0;
int sh = 19;
int na = 0;
int ex039 = 0;
int keyspd = 20;
int split = 0;
int power = 50;
int rport_gain_ssb = 50;
int rport_gain_am = 50;
int rport_gain_fm = 50;
int rport_gain_psk = 50;
int syncvfo = 0;
int ant = 1;

// ID 0310 == 310, Must drop leading zero
typedef enum nc_rigid_e
{
    NC_RIGID_NONE            = 0,
    NC_RIGID_FT450           = 241,
    NC_RIGID_FT450D          = 244,
    NC_RIGID_FT950           = 310,
    NC_RIGID_FT891           = 135,
    NC_RIGID_FT991           = 135,
    NC_RIGID_FT2000          = 251,
    NC_RIGID_FT2000D         = 252,
    NC_RIGID_FTDX1200        = 583,
    NC_RIGID_FTDX9000D       = 101,
    NC_RIGID_FTDX9000Contest = 102,
    NC_RIGID_FTDX9000MP      = 103,
    NC_RIGID_FTDX5000        = 362,
    NC_RIGID_FTDX3000        = 460,
    NC_RIGID_FTDX101D        = 681,
    NC_RIGID_FTDX101MP       = 682
} nc_rigid_t;

int
getmyline(int fd, char *buf)
{
    char c;
    int i = 0;
    memset(buf, 0, BUFSIZE);

    while (read(fd, &c, 1) > 0)
    {
        buf[i++] = c;

        if (c == ';') { return strlen(buf); }
    }

    if (strlen(buf) == 0) { hl_usleep(10 * 1000); }

    return strlen(buf);
}

#if defined(WIN32) || defined(_WIN32)
int openPort(char *comport) // doesn't matter for using pts devices
{
    int fd;
    fd = open(comport, O_RDWR);

    if (fd < 0)
    {
        perror(comport);
    }

    return fd;
}

#else
int openPort(char *comport) // doesn't matter for using pts devices
{
    int fd = posix_openpt(O_RDWR);
    char *name = ptsname(fd);

    if (name == NULL)
    {
        perror("ptsname");
        return -1;
    }

    printf("name=%s\n", name);

    if (fd == -1 || grantpt(fd) == -1 || unlockpt(fd) == -1)
    {
        perror("posix_openpt");
        return -1;
    }

    return fd;
}
#endif



int main(int argc, char *argv[])
{
    char buf[256];
    char *pbuf;
    int n;
    int fd = openPort(argv[1]);

    while (1)
    {
        if (getmyline(fd, buf))
        {
            printf("Cmd:%s\n", buf);
        }
        else { continue; }

        if (strcmp(buf, "RM5;") == 0)
        {
            printf("%s\n", buf);
            hl_usleep(50 * 1000);
            pbuf = "RM5100000;";
            n = write(fd, pbuf, strlen(pbuf));
            printf("n=%d\n", n);

            if (n <= 0) { perror("RM5"); }
        }

        if (strcmp(buf, "AN0;") == 0)
        {
            hl_usleep(50 * 1000);
            SNPRINTF(buf, sizeof(buf), "AN0%d;", ant);
            n = write(fd, buf, strlen(buf));

            if (n <= 0) { perror("AN"); }
        }
        else if (strncmp(buf, "AN", 2) == 0)
        {
            sscanf(buf,"AN%d",&ant);
            printf("Ant set to %d\n", ant);
        }
        else if (strcmp(buf, "IF;") == 0)
        {
            printf("%s\n", buf);
            hl_usleep(50 * 1000);
            pbuf = "IF059014200000+000000700000;";
            n = write(fd, pbuf, strlen(pbuf));
            printf("n=%d\n", n);

            if (n <= 0) { perror("IF"); }
        }
        else if (strcmp(buf, "ID;") == 0)
        {
            printf("%s\n", buf);
            hl_usleep(50 * 1000);
            int id = NC_RIGID_FTDX3000;
            SNPRINTF(buf, sizeof(buf), "ID%03d;", id);
            n = write(fd, buf, strlen(buf));
            printf("n=%d\n", n);

            if (n <= 0) { perror("ID"); }
        }

#if 0
        else if (strncmp(buf, "AI", 2) == 0)
        {
            if (strcmp(buf, "AI;"))
            {
                printf("%s\n", buf);
                hl_usleep(50 * 1000);
                n = fprintf(fp, "%s", "AI0;");
                printf("n=%d\n", n);

                if (n <= 0) { perror("AI"); }
            }
        }

#endif
        else if (strcmp(buf, "EX032;") == 0)
        {
            ant = (ant + 1) % 3;
            printf("%s\n", buf);
            hl_usleep(50 * 1000);
            SNPRINTF(buf, sizeof(buf), "EX032%1d;", ant);
            n = write(fd, buf, strlen(buf));
            printf("n=%d\n", n);

            if (n < 0) { perror("EX032"); }
        }

        else if (strcmp(buf, "FA;") == 0)
        {
            SNPRINTF(buf, sizeof(buf), "FA%08.0f;", freqA);
            n = write(fd, buf, strlen(buf));
        }
        else if (strncmp(buf, "FA", 2) == 0)
        {
            sscanf(buf, "FA%f", &freqA);
        }
        else if (strcmp(buf, "FB;") == 0)
        {
            SNPRINTF(buf, sizeof(buf), "FB%08.0f;", freqB);
            n = write(fd, buf, strlen(buf));
        }
        else if (strncmp(buf, "FB", 2) == 0)
        {
            sscanf(buf, "FB%f", &freqB);
        }
        else if (strcmp(buf, "FT;") == 0)
        {
            SNPRINTF(buf, sizeof(buf), "FT%d;", ft);
            n = write(fd, buf, strlen(buf));
        }
        else if (strcmp(buf, "MD0;") == 0)
        {
            SNPRINTF(buf, sizeof(buf), "MD0%X;", modeA);
            n = write(fd, buf, strlen(buf));
        }
        else if (strncmp(buf, "MD0", 3) == 0)
        {
            sscanf(buf, "MD0%d", &modeA);
        }
        else if (strcmp(buf, "MD1;") == 0)
        {
            SNPRINTF(buf, sizeof(buf), "MD1%X;", modeB);
            n = write(fd, buf, strlen(buf));
        }
        else if (strncmp(buf, "MD1", 3) == 0)
        {
            sscanf(buf, "MD1%d", &modeB);
        }
        else if (strcmp(buf, "VS;") == 0)
        {
            SNPRINTF(buf, sizeof(buf), "VS%d;", vs);
            n = write(fd, buf, strlen(buf));
        }
        else if (strncmp(buf, "VS", 2) == 0)
        {
            sscanf(buf, "VS%d", &vs);
        }
        else if (strcmp(buf, "TX;") == 0)
        {
            SNPRINTF(buf, sizeof(buf), "TX%d;", tx);
            n = write(fd, buf, strlen(buf));
        }
        else if (strncmp(buf, "TX", 2) == 0)
        {
            sscanf(buf, "TX%d", &tx);
        }
        else if (strcmp(buf, "AI;") == 0)
        {
            SNPRINTF(buf, sizeof(buf), "AI%d;", ai);
            n = write(fd, buf, strlen(buf));
        }
        else if (strncmp(buf, "AI", 2) == 0)
        {
            sscanf(buf, "AI%d", &ai);
        }
        else if (strcmp(buf, "PC;") == 0)
        {
            SNPRINTF(buf, sizeof(buf), "PC%d;", power);
            n = write(fd, buf, strlen(buf));
        }
        else if (strncmp(buf, "PC", 2) == 0)
        {
            sscanf(buf, "PC%d", &power);
        }
        else if (strcmp(buf, "SH0;") == 0)
        {
            SNPRINTF(buf, sizeof(buf), "SH0%d;", sh);
            n = write(fd, buf, strlen(buf));
        }
        else if (strncmp(buf, "SH0", 3) == 0)
        {
            sscanf(buf, "SH0%d", &sh);
        }
        else if (strcmp(buf, "NA0;") == 0)
        {
            SNPRINTF(buf, sizeof(buf), "NA0%d;", na);
            n = write(fd, buf, strlen(buf));
        }
        else if (strncmp(buf, "NA0", 3) == 0)
        {
            sscanf(buf, "NA0%d", &na);
        }
        else if (strcmp(buf, "EX039;") == 0)
        {
            SNPRINTF(buf, sizeof(buf), "EX039%d;", ex039);
            n = write(fd, buf, strlen(buf));
        }
        else if (strncmp(buf, "EX039", 5) == 0)
        {
            sscanf(buf, "EX039%d", &ex039);
        }
        else if (strcmp(buf, "PS;") == 0)
        {
            SNPRINTF(buf, sizeof(buf), "PS1;");
            n = write(fd, buf, strlen(buf));
        }
        else if (strncmp(buf, "KS;", 3) == 0)
        {
            sprintf(buf, "KS%d;", keyspd);
            n = write(fd, buf, strlen(buf));
        }
        else if (strncmp(buf, "KS", 2) == 0)
        {
            sscanf(buf, "KS%03d", &keyspd);
        }
        else if (strncmp(buf, "ST;", 3) == 0)
        {
            sprintf(buf, "ST%d;", split);
            n = write(fd, buf, strlen(buf));
        }
        else if (strncmp(buf, "ST", 2) == 0)
        {
            sscanf(buf, "ST%d", &split);
        }
        else if (strcmp(buf, "EX010415;") == 0)
        {
            sprintf(buf, "EX010415%03d;", rport_gain_psk);
            n = write(fd, buf, strlen(buf));
        }
        else if (strncmp(buf, "EX010415", 8) == 0)
        {
            printf("Here#1");
            sscanf(buf, "EX010415%d", &rport_gain_psk);
        }
        else if (strcmp(buf, "SY;") == 0)
        {
            sprintf(buf, "SY%d;", syncvfo);
            n = write(fd, buf, strlen(buf));
        }
        else if (strncmp(buf, "SY", 2) == 0)
        {
            sscanf(buf, "SY%d", &syncvfo);
        }

        else if (strlen(buf) > 0)
        {
            fprintf(stderr, "Unknown command: %s\n", buf);
        }

    }

    return 0;
}
