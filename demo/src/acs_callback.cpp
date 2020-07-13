//
// Created by Rqg on 2020/1/4.
//

#include <tuya_ai_pad_sdk.h>

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/document.h>


#include "acs_callback.h"
#include "restApi.h"
#include "acs_handlers.h"

restApi *acs_ctx = nullptr;

void cb_device_detail(DeviceDetail *d) {
    if (acs_ctx == nullptr) return;

    rapidjson::StringBuffer s;
    rapidjson::Writer<rapidjson::StringBuffer> writer(s);

    writer.StartObject();
    writer.Key("api");
    writer.String("device_detail");

    writer.Key("devId");
    writer.String(d->devId);
    writer.Key("name");
    writer.String(d->name);
    writer.Key("companyName");
    writer.String(d->companyName);
    writer.Key("companyLogo");
    writer.String(d->companyLogo);

    writer.EndObject();

    acs_ctx->sendWsMsg(s.GetString());
}


void cb_face_status(int status, char *msg) {
    if (acs_ctx == nullptr) return;

    rapidjson::StringBuffer s;
    rapidjson::Writer<rapidjson::StringBuffer> writer(s);

    writer.StartObject();
    writer.Key("api");
    writer.String("face_status");

    writer.Key("status");
    writer.Int(status);
    writer.Key("status_name");
    switch (status) {
        case SYNC_STATUS_START:
            writer.String("开始同步");
            break;
        case SYNC_STATUS_PROGRESS:
            writer.String("正在同步");
            break;
        case SYNC_STATUS_ERROR:
            writer.String("同步出错");
            break;
        case SYNC_STATUS_FINISH:
            writer.String("同步完成");
            break;
        case SYNC_STATUS_ALL_REMOVED:
            writer.String("移除全部数据");
            break;
        default:
            writer.String("未知同步状态");
    }
    writer.Key("msg");
    writer.String(msg);
    writer.EndObject();

    acs_ctx->sendWsMsg(s.GetString());
}

void cb_rule_status(int status, char *msg) {
    if (acs_ctx == nullptr) return;

    rapidjson::StringBuffer s;
    rapidjson::Writer<rapidjson::StringBuffer> writer(s);

    writer.StartObject();
    writer.Key("api");
    writer.String("rule_status");

    writer.Key("status");
    writer.Int(status);
    writer.Key("status_name");
    switch (status) {
        case SYNC_STATUS_START:
            writer.String("开始同步");
            break;
        case SYNC_STATUS_PROGRESS:
            writer.String("正在同步");
            break;
        case SYNC_STATUS_ERROR:
            writer.String("同步出错");
            break;
        case SYNC_STATUS_FINISH:
            writer.String("同步完成");
            break;
        case SYNC_STATUS_ALL_REMOVED:
            writer.String("移除全部数据");
            break;
        default:
            writer.String("未知同步状态");
    }

    writer.Key("msg");
    writer.String(msg);
    writer.EndObject();

    acs_ctx->sendWsMsg(s.GetString());
}

int cb_face_data_change(char *uid, char *name, char *avatarPath, int type, int status, void *data) {
    if (acs_ctx == nullptr) return FACE_DATA_ERR_REGISTER;

    rapidjson::StringBuffer s;
    rapidjson::Writer<rapidjson::StringBuffer> writer(s);

    writer.StartObject();
    writer.Key("api");
    writer.String("face_data");

    writer.Key("uid");
    writer.String(uid);
    writer.Key("name");
    writer.String(name);
    writer.Key("avatarPath");
    writer.String(avatarPath);
    writer.Key("type");
    writer.Int(type);
    writer.Key("status");
    writer.Int(status);
    writer.Key("status_name");
    switch (status) {
        case USER_STATUS_ADD:
            writer.String("add user");
            break;
        case USER_STATUS_MODIFY:
            writer.String("modify user");
            break;
        case USER_STATUS_REMOVE:
            writer.String("remove user");
            break;
        default:
            writer.String("unknown user status");
    }

    if (type == USER_TYPE_VISITOR) {
        writer.Key("user_type");
        writer.String("Visitor");

        if (data) {
            auto v = (Visitor *) data;
            writer.Key("allowedDate");
            writer.String(v->allowedDate);
        }
    } else if (type == USER_TYPE_MEMBER) {
        writer.Key("user_type");
        writer.String("Member");

        if (data) {
            auto m = (Member *) data;
            writer.Key("gender");
            writer.Int(m->gender);
        }
    }

    writer.EndObject();

    acs_ctx->sendWsMsg(s.GetString());

    return FACE_DATA_ERR_SUCCESS;
}


void cb_device_event_call(DeviceEvent event) {
    printf("cb_device_event_call %d\n", event);
    if (acs_ctx == nullptr) return;

    switch (event) {
        case ACTIVATED: {
            init_acs_after_activated(acs_ctx, 0);
        }
            break;
        case UNBIND: {
            rapidjson::StringBuffer s;
            rapidjson::Writer<rapidjson::StringBuffer> writer(s);

            writer.StartObject();
            writer.Key("device_event");
            writer.String(event == UNBIND ? "unbind_event" : "reset_event");

            writer.EndObject();
            acs_ctx->sendWsMsg(s.GetString());
        }
            break;
        case RESET:
            printf("\n************************ device reset ***************************\n");
            acs_ctx->stopServer();
            exit(0);
            break;
    }


}


int cb_dp_report_call(DPEvent *event) {
    if (acs_ctx == nullptr) return false;

    rapidjson::StringBuffer s;
    rapidjson::Writer<rapidjson::StringBuffer> writer(s);

    writer.StartObject();
    writer.Key("dp_id");
    writer.Int(event->dpId);

    writer.Key("dp_type");
    writer.Int(event->dpType);

    writer.Key("dpValue");
    switch (event->dpType) {
        case PROP_STR:
            writer.String(event->dpValue.strValue);
            break;
        case PROP_VALUE:
            writer.Int(event->dpValue.iValue);
            break;
        case PROP_ENUM:
            writer.Int(event->dpValue.enumValue);
            break;
        case PROP_BOOL:
            writer.Int(event->dpValue.bValue);
            break;
    }

    writer.EndObject();

    acs_ctx->sendWsMsg(s.GetString());

    return 0;
}

int cb_upgrade_available(const TUYA_FW_INFO *info) {
    if (acs_ctx == nullptr) return 0;

    rapidjson::StringBuffer s;
    rapidjson::Writer<rapidjson::StringBuffer> writer(s);
    char buf[1024];
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "cb_upgrade_available %s, %s, %s, %d, %c\n", info->fw_url, info->sw_ver, info->fw_md5, info->file_size,
            info->tp);
    writer.StartObject();
    writer.Key("msg");
    writer.String(buf);
    writer.EndObject();
    acs_ctx->sendWsMsg(s.GetString());
    return 0;
}

void cb_upgrade_download_finish(const TUYA_FW_INFO *, int downloadResult) {

    if (acs_ctx == nullptr) return;

    rapidjson::StringBuffer s;
    rapidjson::Writer<rapidjson::StringBuffer> writer(s);
    char buf[1024];
    memset(buf, 0, sizeof(buf));
    if (downloadResult == 0) {
        sprintf(buf, "cb_upgrade_download_finish upgrade file download success\n");
    } else {
        sprintf(buf, "cb_upgrade_download_finish upgrade file download file\n");
    }
    writer.StartObject();
    writer.Key("msg");
    writer.String(buf);
    writer.EndObject();
    acs_ctx->sendWsMsg(s.GetString());

}

void cb_upgrade_download_progress(const TUYA_FW_INFO *, uint64_t totalSize, uint64_t dataSize) {
    if (acs_ctx == nullptr) return;

    rapidjson::StringBuffer s;
    rapidjson::Writer<rapidjson::StringBuffer> writer(s);
    char buf[1024];
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "cb_upgrade_download_progress %lld/%lld", dataSize, totalSize);
    writer.StartObject();
    writer.Key("msg");
    writer.String(buf);
    writer.EndObject();
    acs_ctx->sendWsMsg(s.GetString());
}

long timezoneOffsetCb() {
    return 8 * 3600 * 1000;
}

const char update_file_path[] = "./update.bin";

void bind_acs_callbacks(restApi *thiz) {
    acs_ctx = thiz;
    ty_set_device_detail_cb(cb_device_detail);
    ty_set_face_sync_status_change_cb(cb_face_status);
    ty_set_rule_sync_status_change_cb(cb_rule_status);
    ty_set_face_data_change_cb(cb_face_data_change);
    //设置设备时间回调
    ty_set_device_callback(cb_device_event_call);

    //设置dp上报回调
    ty_set_dp_callback(cb_dp_report_call);
//    char * update_file_path;
    ty_add_upgrade_callback(update_file_path, cb_upgrade_available, cb_upgrade_download_finish,
                            cb_upgrade_download_progress);


    ty_set_get_timezone_offset_millis_callback(timezoneOffsetCb);
}
