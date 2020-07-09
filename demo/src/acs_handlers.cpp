//
// Created by Rqg on 2020/1/4.
//


#include <tuya_ai_pad_sdk.h>

#ifdef MEDIA_STREAM
#include "media_test.h"
#endif

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/document.h>

#include <thread>
#include <iostream>


#include "acs_handlers.h"
#include "restApi.h"
#include "ctpl.h"
#include "acs_callback.h"


ctpl::thread_pool pool(3);

char *buildStr(std::string str) {
    char *cstr = (char *) malloc(str.length() * sizeof(char) + 1);
    strcpy(cstr, str.data());
    return cstr;
}


inline std::string getString(rapidjson::Value &d, const char *key, const std::string &defaultValue = "") {
    if (!d.HasMember(key)) {
        return defaultValue;
    }
    auto &v = d[key];
    if (!v.IsString()) {
        return defaultValue;
    }
    return v.GetString();
}

inline int getInt(rapidjson::Value &d, const char *key, const int defaultValue = 0) {
    if (!d.HasMember(key)) {
        return defaultValue;
    }
    auto &v = d[key];
    if (!v.IsInt()) {
        return defaultValue;
    }
    return v.GetInt();
}

inline int64_t getInt64(rapidjson::Value &d, const char *key, const int64_t defaultValue = 0L) {
    if (!d.HasMember(key)) {
        return defaultValue;
    }
    auto &v = d[key];
    if (!v.IsInt64()) {
        return defaultValue;
    }
    return v.GetInt64();
}


inline uint64_t getUint64(rapidjson::Value &d, const char *key, const uint64_t defaultValue = 0L) {
    if (!d.HasMember(key)) {
        return defaultValue;
    }
    auto &v = d[key];
    if (!v.IsUint64()) {
        return defaultValue;
    }
    return v.GetUint64();
}

inline int64_t getBool(rapidjson::Value &d, const char *key, const bool defaultValue = false) {
    if (!d.HasMember(key)) {
        return defaultValue;
    }
    auto &v = d[key];
    if (!v.IsBool()) {
        return defaultValue;
    }
    return v.GetBool();
}

inline float getFloat(rapidjson::Value &d, const char *key, const float defaultValue = 0.0f) {
    if (!d.HasMember(key)) {
        return defaultValue;
    }
    auto &v = d[key];

    if (v.IsInt()) {
        return v.GetInt();
    }

    if (!v.IsFloat()) {
        return defaultValue;
    }
    return v.GetFloat();
}

inline bool isObject(rapidjson::Value &d, const char *key) {
    if (!d.HasMember(key)) {
        return false;
    }

    return d[key].IsObject();
}

inline bool isArray(rapidjson::Value &d, const char *key) {
    if (!d.HasMember(key)) {
        return false;
    }
    return d[key].IsArray();
}

void handle_check_activated(restApi *thiz, struct mg_connection *nc, struct http_message *hm) {
    SEND_HEADER

    if (ty_is_device_activated()) {
        mg_printf_http_chunk(nc, "{ \"result\": %s }", "true");
    } else {
        mg_printf_http_chunk(nc, "{ \"result\": %s }", "false");
    }
    END_SEND
}

void handle_info(restApi *thiz, struct mg_connection *nc, struct http_message *hm) {
    SEND_HEADER
    rapidjson::StringBuffer s;
    rapidjson::Writer<rapidjson::StringBuffer> writer(s);

    writer.StartObject();

    writer.Key("roles");
    writer.StartArray();
    if (ty_is_device_activated() == 0) {
        writer.String("acs");
    } else {
        writer.String("no");
    }
    writer.EndArray();
    writer.EndObject();

    mg_printf_http_chunk(nc, "%s", s.GetString());
    END_SEND
}


void printEnv(ActivateEnv *env) {
    if (env->token != nullptr) {
        std::cout << "token:" << env->token;
    }
    std::cout << ", " << "basePath:" << env->basePath << ", " << "pid:" << env->pid << ", " << "uuid:" << env->uuid
              << ", " << "pkey:" << env->pkey << ", " << "dbKey:" << env->dbKey << ", " << "dbKdfIter:"
              << env->dbKdfIter << ", " << "model:" << env->model << ", " << "sn:" << env->sn << ", " << "mac:"
              << env->mac << ", " << "ip:" << env->ip << ", " << "kernelVersion:" << env->kernelVersion << ", "
              << "firmwareVersion:"
              << env->firmwareVersion << ", activateTimeoutMills:" << env->activateTimeoutMills << ", activateMode: "
              << env->activateMode << std::endl;
}


bool activating = false;
bool acs_running = false;

static void send_activated_msg(restApi *thiz, bool ret) {
    printf("send_activated_msg %d\n", ret);
    rapidjson::StringBuffer s;
    rapidjson::Writer<rapidjson::StringBuffer> writer(s);

    writer.StartObject();
    writer.Key("api");
    writer.String("activate");
    writer.Key("success");
    writer.Bool(ret);
    writer.EndObject();

    thiz->sendWsMsg(s.GetString());
}


void init_acs_after_activated(restApi *thiz, int ret) {
    printf("init_acs_after_activated %d\n", ret);
    send_activated_msg(thiz, ret == 0);

    if (ret != 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        exit(0);
    } else {
        uint64_t ts = 0;
        int tz = 0;
        ty_get_server_time(&ts);

        printf("ty_get_server_time %lld\n", ts);

        char cmd[256]{0};
        sprintf(cmd, "date +%%s -s @%lld", ts);
        printf("run cmd:%s\n", cmd);
        system(cmd);
        system("hwclock -w");
    }

#ifdef MEDIA_STREAM
    printf("tuya::MediaTest::getInstance()->start()\n");
    tuya::MediaTest::getInstance()->start();
#endif

    activating = false;
}

static void activate_device(restApi *thiz, bool remoteActivate = false) {
    printf("activate_device %d\n", remoteActivate);
    auto ret = ty_activate_sdk(&thiz->acs_env);

    acs_running = ret == 0;

    if (remoteActivate) {
        return;
    }

    init_acs_after_activated(thiz, ret);

}


void handle_activate(restApi *thiz, struct mg_connection *nc, struct http_message *hm) {
    rapidjson::StringBuffer s;
    rapidjson::Writer<rapidjson::StringBuffer> writer(s);


    if (acs_running || activating) {
        writer.StartObject();
        writer.Key("success");
        writer.Bool(true);
        writer.EndObject();

        SEND_HEADER
        mg_printf_http_chunk(nc, "%s", s.GetString());
        END_SEND


        if (acs_running && !activating) {
            printf("activate already running send_activated_msg \n");
            send_activated_msg(thiz, true);
        }

        return;
    }

    activating = true;
    bind_acs_callbacks(thiz);

    if (ty_is_device_activated() == 0) {
        thiz->acs_env.token = ty_activated_token();
        thiz->acs_env.activateTimeoutMills = 15000; // 15 seconds
        thiz->acs_env.activateMode = ACTIVATE_MODE_WIFI;
        printf("acs token: %s\n", thiz->acs_env.token);
    } else {
        rapidjson::Document d;
        d.Parse(hm->body.p, hm->body.len);
        if (d.HasParseError()) {
            printf("parse error, body %s \n", hm->body.p);
            return;
        }

        auto token = getString(d, "token");
        if (token.empty()) {
            printf("activate acs wired mode \n");
            thiz->acs_env.token = nullptr;
            thiz->acs_env.activateMode = ACTIVATE_MODE_REMOTE;
            thiz->acs_env.remoteRegion = TY_REGION_CN;
        } else {
            printf("activate acs wifi mode \n");
            thiz->acs_env.token = buildStr(token);
            thiz->acs_env.activateTimeoutMills = 15000; // 15 seconds
            thiz->acs_env.activateMode = ACTIVATE_MODE_WIFI;

        }
    }

    printEnv(&thiz->acs_env);

    pool.push([thiz](int id) {
        activate_device(thiz, thiz->acs_env.activateMode == ACTIVATE_MODE_REMOTE);
    });

    writer.StartObject();
    writer.Key("success");
    writer.Bool(true);
    writer.EndObject();

    SEND_HEADER
    mg_printf_http_chunk(nc, "%s", s.GetString());
    END_SEND
}


static void deactivate_device(restApi *thiz) {
    auto ret = ty_deactivate_sdk();

    rapidjson::StringBuffer s;
    rapidjson::Writer<rapidjson::StringBuffer> writer(s);

    writer.StartObject();
    writer.Key("api");
    writer.String("activate");
    writer.Key("success");
    writer.Bool(ret == 0);
    writer.EndObject();

    thiz->sendWsMsg(s.GetString());

    std::this_thread::sleep_for(std::chrono::milliseconds(200));
}

void stopHandle() {
    pool.stop(true);
}

void handle_deactivate(restApi *thiz, struct mg_connection *nc, struct http_message *hm) {
    rapidjson::StringBuffer s;
    rapidjson::Writer<rapidjson::StringBuffer> writer(s);

    pool.push([thiz](int id) {
        deactivate_device(thiz);
    });

    writer.StartObject();
    writer.Key("success");
    writer.Bool(true);
    writer.EndObject();

    SEND_HEADER
    mg_printf_http_chunk(nc, "%s", s.GetString());
    END_SEND
}

void handle_trigger_face_sync(restApi *thiz, struct mg_connection *nc, struct http_message *hm) {
    rapidjson::StringBuffer s;
    rapidjson::Writer<rapidjson::StringBuffer> writer(s);

    int full = 0;
    int remove = 0;

    rapidjson::Document d;
    d.Parse(hm->body.p, hm->body.len);
    if (d.HasParseError()) {
        printf("parse error, body %s \n", hm->body.p);
        return;
    }

    full = getInt(d, "full", 0);
    remove = getInt(d, "remove", 0);

    pool.push([full, remove](int) {
        ty_trigger_face_sync(full, remove);
    });

    writer.StartObject();
    writer.Key("success");
    writer.Bool(true);
    writer.EndObject();

    SEND_HEADER
    mg_printf_http_chunk(nc, "%s", s.GetString());
    END_SEND
}

void handle_trigger_rule_sync(restApi *thiz, struct mg_connection *nc, struct http_message *hm) {
    rapidjson::StringBuffer s;
    rapidjson::Writer<rapidjson::StringBuffer> writer(s);

    int full = 0;
    int remove = 0;

    rapidjson::Document d;
    d.Parse(hm->body.p, hm->body.len);
    if (d.HasParseError()) {
        printf("parse error, body %s \n", hm->body.p);
        return;
    }

    full = getInt(d, "full", 0);
    remove = getInt(d, "remove", 0);

    pool.push([full, remove](int) {
        ty_trigger_rule_sync(full, remove);
    });

    writer.StartObject();
    writer.Key("success");
    writer.Bool(true);
    writer.EndObject();

    SEND_HEADER
    mg_printf_http_chunk(nc, "%s", s.GetString());
    END_SEND
}

void handle_get_device_detail(restApi *thiz, struct mg_connection *nc, struct http_message *hm) {
    rapidjson::StringBuffer s;
    rapidjson::Writer<rapidjson::StringBuffer> writer(s);

    auto d = ty_get_device_detail();

    writer.StartObject();
    writer.Key("devId");
    writer.String(d.devId);
    writer.Key("name");
    writer.String(d.name);
    writer.Key("companyName");
    writer.String(d.companyName);
    writer.Key("companyLogo");
    writer.String(d.companyLogo);
    writer.EndObject();

    SEND_HEADER
    mg_printf_http_chunk(nc, "%s", s.GetString());
    END_SEND

    //in case of memory clean before assemble json
    free(d.devId);
    free(d.name);
    free(d.companyLogo);
    free(d.companyName);
}


void handle_get_all_member(restApi *thiz, struct mg_connection *nc, struct http_message *hm) {
    Member *p = nullptr;
    uint32_t size = 0;
    ty_get_all_member_info(&p, &size);

    rapidjson::StringBuffer s;
    rapidjson::Writer<rapidjson::StringBuffer> writer(s);

    writer.StartObject();
    writer.Key("members");
    writer.StartArray();

    for (int i = 0; i < size; ++i) {
        auto m = p + i;

        writer.StartObject();
        writer.Key("uid");
        writer.String(m->uid);
        writer.Key("name");
        writer.String(m->name);
        writer.Key("workNo");
        writer.String(m->workNo);
        writer.Key("ruleIds");
        writer.String(m->ruleIds);
        writer.Key("avatarPath");
        writer.String(m->avatarPath);
        writer.Key("gender");
        writer.Int(m->gender);
        writer.EndObject();
    }

    writer.EndArray();
    writer.EndObject();


    SEND_HEADER
    mg_printf_http_chunk(nc, "%s", s.GetString());
    END_SEND

    //in case of memory clean before assemble json
    ty_free_member_info(p, size);
}

void handle_get_all_visitor(restApi *thiz, struct mg_connection *nc, struct http_message *hm) {
    Visitor *p = nullptr;
    uint32_t size = 0;
    ty_get_all_visitor_info(&p, &size);

    rapidjson::StringBuffer s;
    rapidjson::Writer<rapidjson::StringBuffer> writer(s);

    writer.StartObject();
    writer.Key("visitors");
    writer.StartArray();

    for (int i = 0; i < size; ++i) {
        auto m = p + i;
        writer.StartObject();
        writer.Key("uid");
        writer.String(m->uid);
        writer.Key("name");
        writer.String(m->name);
        writer.Key("inviteReason");
        writer.String(m->inviteReason);
        writer.Key("allowedDate");
        writer.String(m->allowedDate);
        writer.Key("avatarPath");
        writer.String(m->avatarPath);
        writer.EndObject();
    }

    writer.EndArray();
    writer.EndObject();


    SEND_HEADER
    mg_printf_http_chunk(nc, "%s", s.GetString());
    END_SEND

    //in case of memory clean before assemble json
    ty_free_visitor_info(p, size);
}

void handle_get_visitor_by_id(restApi *thiz, struct mg_connection *nc, struct http_message *hm) {
    rapidjson::Document d;

    rapidjson::StringBuffer s;
    rapidjson::Writer<rapidjson::StringBuffer> writer(s);

    d.Parse(hm->body.p, hm->body.len);
    if (d.HasParseError()) {
        printf("parse error, body %s \n", hm->body.p);
        return;
    }

    auto uid = getString(d, "uid", "");

    if (uid.length() == 0) {
        writer.StartObject();
        writer.Key("err");
        writer.String("params err");
        writer.EndObject();

        SEND_HEADER
        mg_printf_http_chunk(nc, "%s", s.GetString());
        END_SEND

        return;
    }

    auto m = ty_get_visitor_by_uid(uid.data());

    writer.StartObject();
    writer.Key("visitor");
    writer.StartObject();

    writer.Key("uid");
    writer.String(m->uid);
    writer.Key("name");
    writer.String(m->name);
    writer.Key("inviteReason");
    writer.String(m->inviteReason);
    writer.Key("allowedDate");
    writer.String(m->allowedDate);
    writer.Key("avatarPath");
    writer.String(m->avatarPath);

    writer.EndObject();
    writer.EndObject();

    SEND_HEADER
    mg_printf_http_chunk(nc, "%s", s.GetString());
    END_SEND

    //in case of memory clean before assemble json
    ty_free_visitor_info(m, 1);
}

void handle_get_member_by_id(restApi *thiz, struct mg_connection *nc, struct http_message *hm) {
    rapidjson::Document d;

    rapidjson::StringBuffer s;
    rapidjson::Writer<rapidjson::StringBuffer> writer(s);

    d.Parse(hm->body.p, hm->body.len);
    if (d.HasParseError()) {
        printf("parse error, body %s \n", hm->body.p);
        return;
    }

    auto uid = getString(d, "uid", "");

    if (uid.length() == 0) {
        writer.StartObject();
        writer.Key("err");
        writer.String("params err");
        writer.EndObject();

        SEND_HEADER
        mg_printf_http_chunk(nc, "%s", s.GetString());
        END_SEND

        return;
    }

    auto m = ty_get_member_by_uid(uid.data());

    writer.StartObject();
    writer.Key("member");
    writer.StartObject();
    writer.Key("uid");
    writer.String(m->uid);
    writer.Key("name");
    writer.String(m->name);
    writer.Key("workNo");
    writer.String(m->workNo);
    writer.Key("ruleIds");
    writer.String(m->ruleIds);
    writer.Key("avatarPath");
    writer.String(m->avatarPath);
    writer.Key("gender");
    writer.Int(m->gender);
    writer.EndObject();
    writer.EndObject();

    SEND_HEADER
    mg_printf_http_chunk(nc, "%s", s.GetString());
    END_SEND

    //in case of memory clean before assemble json
    ty_free_member_info(m, 1);
}


void handle_get_all_rule(restApi *thiz, struct mg_connection *nc, struct http_message *hm) {
    Rule *p = nullptr;
    uint32_t size = 0;
    ty_get_all_rule_info(&p, &size);

    rapidjson::StringBuffer s;
    rapidjson::Writer<rapidjson::StringBuffer> writer(s);

    writer.StartObject();
    writer.Key("rules");
    writer.StartArray();

    for (int i = 0; i < size; ++i) {
        auto m = p + i;
        writer.StartObject();

        writer.Key("id");
        writer.Int(m->id);
        writer.Key("name");
        writer.String(m->name);
        writer.Key("period");
        writer.String(m->period);
        writer.Key("allowedDate");
        writer.String(m->allowedDate);
        writer.Key("deniedDate");
        writer.String(m->deniedDate);

        writer.EndObject();
    }

    writer.EndArray();
    writer.EndObject();


    SEND_HEADER
    mg_printf_http_chunk(nc, "%s", s.GetString());
    END_SEND

    //in case of memory clean before assemble json
    ty_free_rule_info(p, size);
}

void handle_get_rule_by_id(restApi *thiz, struct mg_connection *nc, struct http_message *hm) {
    rapidjson::Document d;

    rapidjson::StringBuffer s;
    rapidjson::Writer<rapidjson::StringBuffer> writer(s);

    d.Parse(hm->body.p, hm->body.len);
    if (d.HasParseError()) {
        printf("parse error, body %s \n", hm->body.p);
        return;
    }

    auto id = getUint64(d, "id", 0);

    if (id == 0) {
        writer.StartObject();
        writer.Key("err");
        writer.String("params err");
        writer.EndObject();

        SEND_HEADER
        mg_printf_http_chunk(nc, "%s", s.GetString());
        END_SEND

        return;
    }

    auto m = ty_get_rule_by_id(id);

    writer.StartObject();
    writer.Key("rule");
    writer.StartObject();

    writer.Key("id");
    writer.Int(m->id);
    writer.Key("name");
    writer.String(m->name);
    writer.Key("period");
    writer.String(m->period);
    writer.Key("allowedDate");
    writer.String(m->allowedDate);
    writer.Key("deniedDate");
    writer.String(m->deniedDate);

    writer.EndObject();
    writer.EndObject();

    SEND_HEADER
    mg_printf_http_chunk(nc, "%s", s.GetString());
    END_SEND

    //in case of memory clean before assemble json
    ty_free_rule_info(m, 1);
}


void handle_get_image(restApi *thiz, struct mg_connection *nc, struct http_message *hm) {
    char path[256]{0};
    mg_get_http_var(&hm->query_string, "path", path, sizeof(path));

    if (strlen(path) == 0) {
        mg_http_send_error(nc, 400, "path params is null");
        return;
    }

    mg_str mime{"image/jpeg", strlen("image/jpeg")};
    mg_str options{};

    mg_http_serve_file(nc, hm, path, mime, options);
}


void reportAccessToServer(restApi *thiz, bool online, const char *uid, const char *picPath, int user_type, float temp,
                          bool tempOk) {
    using namespace std::chrono;
    milliseconds ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());

    auto ret = ty_report_access(uid, picPath, user_type, ms.count(), temp, tempOk);

    if (online) {
        rapidjson::StringBuffer s;
        rapidjson::Writer<rapidjson::StringBuffer> writer(s);

        writer.StartObject();
        writer.Key("api");
        writer.String("report_access");
        writer.Key("canAccess");
        writer.Bool(ret == 0);
        writer.EndObject();

        thiz->sendWsMsg(s.GetString());

        printf("reportAccessToServer send result: %s\n", s.GetString());
    }
}


bool offlinePermission(const char *uid, const char *picPath, int user_type, float temp, bool tempOk) {
    using namespace std::chrono;
    auto ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

    bool canAccess = false;

    switch (user_type) {
        case FACE_TYPE_MEMBER: {
            auto member = ty_get_member_by_uid(uid);
            canAccess = ty_member_access_permission_by_rules(member->ruleIds, ms) == 0;
            ty_free_member_info(member, 1);
        }
            break;
        case FACE_TYPE_VISITOR: {
            auto visitor = ty_get_visitor_by_uid(uid);
            canAccess = ty_visitor_access_permission_by_allowed_date(visitor->allowedDate, ms) == 0;
            ty_free_visitor_info(visitor, 1);
        }
            break;
        case FACE_TYPE_UNKNOWN:
            break;
        default:
            printf("unknown user type:%d\n", user_type);
            user_type = FACE_TYPE_UNKNOWN;
    }

    pool.push([uid, picPath, user_type, ms, temp, tempOk](int) {
        ty_report_access(uid, picPath, user_type, ms, temp, tempOk);
    });

    return canAccess;
}


void handle_report_access(restApi *thiz, struct mg_connection *nc, struct http_message *hm) {
    rapidjson::Document d;

    rapidjson::StringBuffer s;
    rapidjson::Writer<rapidjson::StringBuffer> writer(s);

    d.Parse(hm->body.p, hm->body.len);
    if (d.HasParseError()) {
        printf("parse error, body %s \n", hm->body.p);
        return;
    }

    auto uid = getString(d, "uid", "");
    if (uid.length() == 0) {
        mg_http_send_error(nc, 400, "uid is invalid");
        return;
    }

    auto picPath = getString(d, "picPath", "");
    if (picPath.length() == 0) {
        mg_http_send_error(nc, 400, "picPath is invalid");
        return;
    }

    auto userType = getInt(d, "userType", -1);
    if (userType == -1) {
        mg_http_send_error(nc, 400, "userType is invalid");
        return;
    }
    auto temp = getFloat(d, "temp", -1.0f);
    if (userType == -1) {
        mg_http_send_error(nc, 400, "temp is invalid");
        return;
    }

    auto tempOK = getBool(d, "tempOK", false);
    if (userType == -1) {
        mg_http_send_error(nc, 400, "tempOK is invalid");
        return;
    }

    auto online = getBool(d, "online", true);

    printf("handle_report_access %s, %s, %d, %d\n", uid.data(), picPath.data(), userType, online ? 1 : 0);

    int m = false;

    if (online) {
        pool.push([thiz, online, uid, picPath, userType, temp, tempOK](int) {
            reportAccessToServer(thiz, online, uid.data(), picPath.data(), userType, temp, tempOK);
        });
    } else {
        m = offlinePermission(uid.data(), picPath.data(), userType, temp, tempOK);
    }


    writer.StartObject();
    writer.Key("canAccess");
    writer.Bool(m);
    writer.EndObject();

    SEND_HEADER
    mg_printf_http_chunk(nc, "%s", s.GetString());
    END_SEND
}
