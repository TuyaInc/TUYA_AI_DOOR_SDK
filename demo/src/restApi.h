//
// Created by Rqg on 2019-12-27.
//

#ifndef AI_PAD_SDK_RESTAPI_H
#define AI_PAD_SDK_RESTAPI_H


#include "mongoose.h"

class restApi {

public:
    restApi();

    bool handleHttpRequest(mg_connection *nc, struct http_message *hm);

    void setWebsocketsConnection(mg_connection *nc);

    mg_connection * getWebsocketsConnection();

    void sendWsMsg(const char *msg);


    ActivateEnv acs_env{0};
private:
    mg_connection *ws_nc;

};


#endif //AI_PAD_SDK_RESTAPI_H
