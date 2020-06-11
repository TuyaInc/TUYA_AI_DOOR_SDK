//
// Created by Rqg on 2019-12-25.
//

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include <tuya_ai_pad_sdk.h>
#include <cstring>
#include <sys/utsname.h>
#include <ifaddrs.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <iostream>

#include "mongoose.h"
#include "restApi.h"


static sig_atomic_t s_signal_received = 0;
static const char *s_http_port = "8800";
static struct mg_serve_http_opts s_http_server_opts;


static void signal_handler(int sig_num) {
    signal(sig_num, signal_handler);  // Reinstantiate signal handler
    s_signal_received = sig_num;
}


restApi *apiHandler;

static void ev_handler(struct mg_connection *nc, int ev, void *ev_data) {
    switch (ev) {
        case MG_EV_WEBSOCKET_HANDSHAKE_DONE: {
            /* New websocket connection. Tell everybody. */
            apiHandler->setWebsocketsConnection(nc);
            printf("acs_ws connected socket:%d\n", nc->sock);
            break;
        }
        case MG_EV_WEBSOCKET_FRAME: {

        }
        case MG_EV_HTTP_REQUEST: {
            struct http_message *hm = (struct http_message *) ev_data;

            if (!apiHandler->handleHttpRequest(nc, hm)) {
                mg_serve_http(nc, hm, s_http_server_opts); /* Serve static content */
            }
            break;
        }
        case MG_EV_CLOSE: {
            /* Disconnect. Tell everybody. */
            if (nc == apiHandler->getWebsocketsConnection()) {
                apiHandler->setWebsocketsConnection(nullptr);
                printf("acs_ws disconnected socket:%d\n", nc->sock);
            }
            break;
        }
    }
}

struct file_writer_data {
    FILE *fp;
    size_t bytes_written;
};

static void handle_upload(struct mg_connection *nc, int ev, void *p) {
    struct file_writer_data *data = (struct file_writer_data *) nc->user_data;
    struct mg_http_multipart_part *mp = (struct mg_http_multipart_part *) p;

    switch (ev) {
        case MG_EV_HTTP_REQUEST:
            mg_printf(nc, "%s", "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\nAccess-Control-Allow-Methods: GET, POST\r\nAccess-Control-Allow-Headers:*\r\nTransfer-Encoding: chunked\r\n\r\n");
            mg_send_http_chunk(nc, "", 0); /* Send empty chunk, the end of response */
            break;
        case MG_EV_HTTP_PART_BEGIN: {
            if (data == NULL) {
                data = (struct file_writer_data *) calloc(1, sizeof(struct file_writer_data));
                data->fp = fopen("acs_report", "wb");
                data->bytes_written = 0;

                if (data->fp == NULL) {
                    mg_printf(nc, "%s",
                              "HTTP/1.1 500 Failed to open a file\r\n"
                              "Content-Length: 0\r\n\r\n");
                    nc->flags |= MG_F_SEND_AND_CLOSE;
                    free(data);
                    return;
                }
                nc->user_data = (void *) data;
            }
            break;
        }
        case MG_EV_HTTP_PART_DATA: {
            if (fwrite(mp->data.p, 1, mp->data.len, data->fp) != mp->data.len) {
                mg_printf(nc, "%s",
                          "HTTP/1.1 500 Failed to write to a file\r\n"
                          "Content-Length: 0\r\n\r\n");
                nc->flags |= MG_F_SEND_AND_CLOSE;
                return;
            }
            data->bytes_written += mp->data.len;
            break;
        }
        case MG_EV_HTTP_PART_END: {
            mg_printf(nc,
                      "HTTP/1.1 200 OK\r\n"
                      "Content-Type: text/plain\r\n"
                      "Access-Control-Allow-Origin: *\r\nAccess-Control-Allow-Methods: GET, POST\r\nAccess-Control-Allow-Headers:*\r\n"
                      "Connection: close\r\n\r\n"
                      "Written %ld of POST data to a temp file\n\n",
                      (long) ftell(data->fp));
            nc->flags |= MG_F_SEND_AND_CLOSE;
            fclose(data->fp);
            free(data);
            nc->user_data = NULL;
            break;
        }
    }
}


int main(void) {
    struct mg_mgr mgr;
    struct mg_connection *nc;

    apiHandler = new restApi();

    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);
    setvbuf(stdout, NULL, _IOLBF, 0);
    setvbuf(stderr, NULL, _IOLBF, 0);

    mg_mgr_init(&mgr, NULL);

    nc = mg_bind(&mgr, s_http_port, ev_handler);
    if (nc == nullptr) {
        printf("mg_bind fail\n");
        return -1;
    }
    mg_set_protocol_http_websocket(nc);

    mg_register_http_endpoint(nc, "/upload", handle_upload MG_UD_ARG(NULL));

//    s_http_server_opts.document_root = "../demo/web/dist";  // Serve current directory
    s_http_server_opts.document_root = "./dist/";  // Serve current directory
    s_http_server_opts.enable_directory_listing = "yes";

    printf("Started on port %s\n", s_http_port);
    while (s_signal_received == 0) {
        mg_mgr_poll(&mgr, 200);
    }
    mg_mgr_free(&mgr);

    exit(0);

    ty_free_sdk();

    return 0;
}



