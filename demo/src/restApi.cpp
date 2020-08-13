//
// Created by Rqg on 2019-12-27.
//

#include <string>
#include <tuya_ai_pad_sdk.h>
#include <mutex>
#include <queue>
#include <condition_variable>

#include <map>
#include <ifaddrs.h>
#include <sys/utsname.h>

#include "restApi.h"
#include "acs_handlers.h"


typedef void(*requestFn)(restApi *thiz, struct mg_connection *nc, struct http_message *hm);

std::map<std::string, requestFn> requestApis = {
        {"/api/v1/check_activated",   handle_check_activated},
        {"/api/v1/info",              handle_info},
        {"/api/v1/activate",          handle_activate},
        {"/api/v1/deactivate",        handle_deactivate},
        {"/api/v1/trigger_face",      handle_trigger_face_sync},
        {"/api/v1/trigger_rule",      handle_trigger_rule_sync},
        {"/api/v1/get_device_detail", handle_get_device_detail},
        {"/api/v1/get_all_member",    handle_get_all_member},
        {"/api/v1/get_all_visitor",   handle_get_all_visitor},
        {"/api/v1/get_visitor_by_id", handle_get_visitor_by_id},
        {"/api/v1/get_member_by_id",  handle_get_member_by_id},
        {"/api/v1/get_all_rule",      handle_get_all_rule},
        {"/api/v1/get_rule_by_id",    handle_get_rule_by_id},
        {"/api/v1/get_image",         handle_get_image},
        {"/api/v1/report_access",     handle_report_access},
        {"/api/v1/startmedia",        handle_startmedia},
        {"/api/v1/stopmedia",         handle_stopmedia},
        {"/api/v1/getmediastate",     handle_getmediastate},
};


bool restApi::handleHttpRequest(mg_connection *nc, struct http_message *hm) {
    std::string api(hm->uri.p, hm->uri.len);
    if (mg_vcasecmp(&hm->method, "OPTIONS") == 0) {
        SEND_HEADER
        END_SEND
        printf("%s option\n", api.data());
        return true;
    }

    auto fn = requestApis[api];

    if (fn == nullptr) {
        printf("%s, func not found\n", api.data());
        return false;
    }

    fn(this, nc, hm);
    return true;
}


std::string getIPAddress() {
    std::string ipAddress = "Unable to get IP Address";
    struct ifaddrs *interfaces = NULL;
    struct ifaddrs *temp_addr = NULL;
    int success = 0;
    // retrieve the current interfaces - returns 0 on success
    success = getifaddrs(&interfaces);
    if (success == 0) {
        // Loop through linked list of interfaces
        temp_addr = interfaces;
        while (temp_addr != NULL) {
            if (temp_addr->ifa_addr && temp_addr->ifa_addr->sa_family == AF_INET) {
                // Check if interface is en0 which is the wifi connection on the iPhone
                if (strcmp(temp_addr->ifa_name, "eth0") == 0) {
                    ipAddress = inet_ntoa(((struct sockaddr_in *) temp_addr->ifa_addr)->sin_addr);
                }
            }
            temp_addr = temp_addr->ifa_next;
        }
    }
    // Free memory
    freeifaddrs(interfaces);
    return ipAddress;
}


char *getMacAddress() {
    char *mac = (char *) malloc(19 * sizeof(char));
    memset(mac, 0, 19 * sizeof(char));

    auto f = fopen("/sys/class/net/eth0/address", "r");

    if (f == nullptr) {
        printf("read mac address fail");
        strcpy(mac, "unknown");
        return mac;
    }

    auto count = fscanf(f, "%s", mac);

    return mac;
}

void restApi::stopServer() {
    stopHandle();
}

void queryDeviceInfo(ActivateEnv *env) {
    env->firmwareVersion = buildStr("1.0.0");

    struct utsname name{};
    uname(&name);

    env->kernelVersion = buildStr(name.release);
    env->model = buildStr(name.machine);
    env->sn = buildStr("1231212321");
    printf("try get ip\n");
    env->ip = buildStr(getIPAddress());
    printf("try get mac\n");
    env->mac = getMacAddress();


}

void logMsg(int level, const char *msg) {
    printf("logMsg:%d -- %s", level, msg);
}

restApi::restApi() : ws_nc(nullptr) {

    char basePath[20] = "./";

    auto *apath = (char *) malloc(sizeof(char) * 1024);
    realpath(basePath, apath);
    queryDeviceInfo(&acs_env);
    acs_env.basePath = apath;

    acs_env.pid = buildStr("8krgasdila00tmqx");
    acs_env.uuid = buildStr("tuya9d446f3fd5c3e210");
    acs_env.pkey = buildStr("wCMVbVvvXEDlLpBDcWv6Htsg831hlec0");
     
    acs_env.dbKey = buildStr("aflajdsfj");
    acs_env.dbKdfIter = 1000;

    acs_env.activateTimeoutMills = 15000; // 15 seconds
//    printEnv(&acs_env);


    printf("\nip: %s \n", acs_env.ip);
    char kvPwd[] = "fjalkdsfljadsfkaf";
    ty_set_tuya_log_callback(logMsg);
    ty_tuya_ai_pad_initSDK(acs_env.basePath, kvPwd);
}

void restApi::setWebsocketsConnection(mg_connection *nc) {
    ws_nc = nc;
}


std::mutex d_mutex;
std::condition_variable d_condition;
std::queue<std::string> d_queue;

void restApi::sendWsMsg(const char *msg) {
    if (ws_nc == nullptr || msg == nullptr)
        return;

    std::unique_lock<std::mutex> lock(d_mutex);
    d_queue.push(msg);
    d_condition.notify_one();
}

void restApi::handleWebSocketMsg() {
    std::unique_lock<std::mutex> lock(d_mutex);
    d_condition.wait_for(lock, std::chrono::milliseconds(200), [] { return !d_queue.empty(); });
    if (d_queue.empty()) {
        return;
    }
    auto msg = d_queue.front();
    d_queue.pop();
    mg_send_websocket_frame(ws_nc, WEBSOCKET_OP_TEXT, msg.data(), msg.size());

}

mg_connection *restApi::getWebsocketsConnection() {
    return ws_nc;
}
