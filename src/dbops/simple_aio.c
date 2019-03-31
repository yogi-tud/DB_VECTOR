#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <veaio.h>

#define BUF_SIZE (1024*1024)
ssize_t do_something_and_reading(int fd, uint64_t *rbuf,
                                        size_t size) {
        struct ve_aio_ctx *ctx;
        int errnoval, ret, i;
        ssize_t retval;
        /* Initialize and get VE AIO context */
        ctx = ve_aio_init();
        if (ctx == NULL) {
                perror("ve_aio_init");
                return -1;
        } 
        /* Start asynchronous read for context */
        ret = ve_aio_write(ctx, fd, size, rbuf, 0);
        if (ret == -1) {
                perror("ve_aio_read");
                retval = -1;
                goto completed;
        }
        for (i = 0; i < 10; i++) {
                /* Do something */
                /* Check state of asynchronous read */
                ret = ve_aio_query(ctx, &retval, &errnoval);
                if (ret == 0) {
                        goto completed;
                } else if (ret == -1) {
                        perror("ve_aio_query");
                        retval = -1;
                        goto completed;
                }
        }
        /* Wait completion of asynchronous read if it is not completed yet */
        ret = ve_aio_wait(ctx, &retval, &errnoval);
        if (ret == -1) {
                perror("ve_aio_wait");
                retval = -1;
        }
completed:
        ve_aio_fini(ctx);
        return retval;
}
int main (void) {
        int i;
        int fd;
        int ret;
        ssize_t retval;
        uint64_t buf[BUF_SIZE];
        uint64_t rbuf[BUF_SIZE];
        fd = open("/tmp/test_file", O_RDWR|O_CREAT, S_IRWXU);
        if (fd == -1) {
                perror("open");
                exit(1);
        }
        ret = ftruncate(fd, BUF_SIZE);
        if (ret == -1) {
                perror("ftruncate");
                exit(1);
        }
        for (i = 0; i < BUF_SIZE/8; i++)
                buf[i] = i;
        ret = pwrite(fd, buf, BUF_SIZE, 0);
        if (ret == -1) {
                perror("pwrite");
                exit(1);
        }
        retval = do_something_and_reading(fd, rbuf, BUF_SIZE);
        if (retval != BUF_SIZE) {
                fprintf(stderr, "do_something_and_reading fails: retval=%zd\n",
                        retval);
                goto err;
        }
        for (i = 0; i < BUF_SIZE / 8; i++) {
                if (buf[i] != rbuf[i]) {
                        fprintf(stderr, "diff buf[%d] = %lx, rbuf[%d] = %lx\n",
                                i, buf[i], i, rbuf[i]);
                        goto err;
                }
        }
        printf("OK\n");
err:
        close(fd);
        return 0;
}
