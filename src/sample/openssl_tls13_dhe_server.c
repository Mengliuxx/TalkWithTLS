#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

#include "openssl/crypto.h"
#include "openssl/ssl.h"

#include "test_common.h"

#define SERVER_CERT_FILE "./certs/ECC_Prime256_Certs/serv_cert.pem"
#define SERVER_KEY_FILE "./certs/ECC_Prime256_Certs/serv_key.der"

int g_kexch_groups[] = {
    NID_ffdhe2048,
    NID_ffdhe3072,
    NID_ffdhe4096,
    NID_ffdhe6144,
    NID_ffdhe8192
};

SSL_CTX *create_context()
{
    SSL_CTX *ctx;

    ctx = SSL_CTX_new(TLS_server_method());
    if (!ctx) {
        printf("SSL ctx new failed\n");
        return NULL;
    }

    printf("SSL context created\n");

    if (SSL_CTX_use_certificate_file(ctx, SERVER_CERT_FILE, SSL_FILETYPE_PEM) != 1) {
        printf("Load Server cert %s failed\n", SERVER_CERT_FILE);
        goto err_handler;
    }

    printf("Loaded server cert %s on context\n", SERVER_CERT_FILE);

    if (SSL_CTX_use_PrivateKey_file(ctx, SERVER_KEY_FILE, SSL_FILETYPE_ASN1) != 1) {
        printf("Load Server key %s failed\n", SERVER_KEY_FILE);
        goto err_handler;
    }

    printf("Loaded server key %s on context\n", SERVER_KEY_FILE);

    printf("SSL context configurations completed\n");

    return ctx;
err_handler:
    SSL_CTX_free(ctx);
    return NULL;
}

SSL *create_ssl_object(SSL_CTX *ctx, int lfd)
{
    SSL *ssl;
    int fd;

    fd = do_tcp_accept(lfd);
    if (fd < 0) {
        printf("TCP connection establishment failed\n");
        return NULL;
    }

    ssl = SSL_new(ctx);
    if (!ssl) {
        printf("SSL object creation failed\n");
        return NULL;
    }

    SSL_set_fd(ssl, fd);

    if (SSL_set1_groups(ssl, g_kexch_groups, sizeof(g_kexch_groups)/sizeof(g_kexch_groups[0])) != 1) {
        printf("Set Groups failed\n");
        goto err_handler;
    }

    printf("SSL object creation finished\n");

    return ssl;
err_handler:
    SSL_free(ssl);
    return NULL;
}

int do_data_transfer(SSL *ssl)
{
    const char *msg_res[] = {MSG1_RES, MSG2_RES};
    const char *res;
    char buf[MAX_BUF_SIZE] = {0};
    int ret, i;
    for (i = 0; i < sizeof(msg_res)/sizeof(msg_res[0]); i++) {
        res = msg_res[i];
        ret = SSL_read(ssl, buf, sizeof(buf) - 1);
        if (ret <= 0) {
            printf("SSL_read failed ret=%d\n", ret);
            return -1;
        }
        printf("SSL_read[%d] %s\n", ret, buf);

        ret = SSL_write(ssl, res, strlen(res));
        if (ret <= 0) {
            printf("SSL_write failed ret=%d\n", ret);
            return -1;
        }
        printf("SSL_write[%d] sent %s\n", ret, res);
    }
    return 0;
}

int tls13_server()
{
    SSL_CTX *ctx;
    SSL *ssl = NULL;
    int ret_val = -1;
    int lfd = -1;
    int fd = -1;
    int ret;

    ctx = create_context();
    if (!ctx) {
        return -1;
    }

    lfd = do_tcp_listen(SERVER_IP, SERVER_PORT);
    if (lfd < 0) {
        goto err_handler;
    }

    ssl = create_ssl_object(ctx, lfd);
    if (!ssl) {
        goto err_handler;
    }

    fd = SSL_get_fd(ssl);

    ret = SSL_accept(ssl); 
    if (ret != 1) {
        printf("SSL accept failed%d\n", ret);
        goto err_handler;
    }

    printf("SSL accept succeeded\n");

    if (do_data_transfer(ssl)) {
        printf("Data transfer over TLS failed\n");
        goto err_handler;
    }
    printf("Data transfer over TLS succeeded\n");
    SSL_shutdown(ssl);
    ret_val = 0;
err_handler:
    if (ssl) {
        SSL_free(ssl);
    }
    SSL_CTX_free(ctx);
    close(lfd);
    close(fd);
    return ret_val;
}

int main()
{
    printf("OpenSSL version: %s, %s\n", OpenSSL_version(OPENSSL_VERSION), OpenSSL_version(OPENSSL_BUILT_ON));
    if (tls13_server()) {
        printf("TLS12 server connection failed\n");
        return -1;
    }
    return 0;
}

