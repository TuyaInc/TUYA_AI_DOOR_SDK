//
// Created by Rqg on 2020/1/4.
//

#ifndef AI_PAD_SDK_ACS_HANDLERS_H
#define AI_PAD_SDK_ACS_HANDLERS_H

#define SEND_HEADER mg_printf(nc, "%s", "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\nAccess-Control-Allow-Methods: GET, POST\r\nAccess-Control-Allow-Headers:*\r\nTransfer-Encoding: chunked\r\n\r\n");
#define END_SEND mg_send_http_chunk(nc, "", 0); /* Send empty chunk, the end of response */

class restApi;

void handle_deactivate(restApi *thiz, struct mg_connection *nc, struct http_message *hm);

void handle_activate(restApi *thiz, struct mg_connection *nc, struct http_message *hm);

void handle_info(restApi *thiz, struct mg_connection *nc, struct http_message *hm);

void handle_check_activated(restApi *thiz, struct mg_connection *nc, struct http_message *hm);

void handle_trigger_face_sync(restApi *thiz, struct mg_connection *nc, struct http_message *hm);

void handle_trigger_rule_sync(restApi *thiz, struct mg_connection *nc, struct http_message *hm);

void handle_get_device_detail(restApi *thiz, struct mg_connection *nc, struct http_message *hm);

void handle_get_all_member(restApi *thiz, struct mg_connection *nc, struct http_message *hm);

void handle_get_all_visitor(restApi *thiz, struct mg_connection *nc, struct http_message *hm);

void handle_get_visitor_by_id(restApi *thiz, struct mg_connection *nc, struct http_message *hm);

void handle_get_member_by_id(restApi *thiz, struct mg_connection *nc, struct http_message *hm);

void handle_get_all_rule(restApi *thiz, struct mg_connection *nc, struct http_message *hm);

void handle_get_rule_by_id(restApi *thiz, struct mg_connection *nc, struct http_message *hm);

void handle_get_image(restApi *thiz, struct mg_connection *nc, struct http_message *hm);

void handle_report_access(restApi *thiz, struct mg_connection *nc, struct http_message *hm);
void handle_startmedia(restApi *thiz, struct mg_connection *nc, struct http_message *hm);
void handle_stopmedia(restApi *thiz, struct mg_connection *nc, struct http_message *hm);
void handle_getmediastate(restApi *thiz, struct mg_connection *nc, struct http_message *hm);

char *buildStr(std::string str);


void init_acs_after_activated(restApi *thiz, int ret);

void stopHandle();


#endif //AI_PAD_SDK_ACS_HANDLERS_H
