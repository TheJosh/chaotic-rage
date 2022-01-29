// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <microhttpd.h>
#include <string.h>
#include "cmd_manager.h"
#include "commands/echo.h"
#include "commands/stats.h"
#include "commands/day_night_animate.h"


/**
* Wrapper for the actual AccessHandlerCallback method in the CmdManager class
**/
MHD_Result ahc(
    void* cls, struct MHD_Connection* connection, const char* url,
    const char* method, const char* version, const char* upload_data,
    long unsigned int* upload_data_size, void** ptr
) {
    return static_cast<CmdManager*>(cls)->AccessHandlerCallback(
        connection, url, method, upload_data, upload_data_size, ptr
    );
}


/**
* Start the daemon
**/
CmdManager::CmdManager(uint16_t port)
{
    daemon = MHD_start_daemon(
        MHD_USE_THREAD_PER_CONNECTION,
        port,
        NULL,
        NULL,
        &ahc,
        (void*) this,
        MHD_OPTION_END
    );

    // TODO: Handle error
}


/**
* Stop the daemon
**/
CmdManager::~CmdManager()
{
    Cmd* c;
    while (work.trypop(c)) {
        c->cancel();
    }

    MHD_stop_daemon(daemon);
}


/**
* Handle incoming HTTP requests
**/
MHD_Result CmdManager::AccessHandlerCallback(
    struct MHD_Connection* connection,
    const char* url, const char* method,
    const char* upload_data, size_t* upload_data_size,
    void** ptr
) {
    static int dummy;
    struct MHD_Response* response;
    MHD_Result ret;

    if (0 != strcmp(method, "GET")) {
        return MHD_NO; /* unexpected method */
    }

    // The first time only the headers are valid, so store data but don't respond
    if (&dummy != *ptr) {
        *ptr = &dummy;
        return MHD_YES;
    }

    if (0 != *upload_data_size) {
        return MHD_NO; /* upload data in a GET!? */
    }

    // clear context pointer
    *ptr = NULL;

    Cmd* c = this->dispatch(connection, url, method);
    if (c == NULL) {
        return MHD_NO;
    }
    work.push(c);
    std::string resp = c->waitDone();
    delete c;

    response = MHD_create_response_from_buffer(
        resp.length(),
        (void*) resp.c_str(),
        MHD_RESPMEM_MUST_COPY
    );

    // Allows other origins to access data via AJAX
    MHD_add_response_header(response, "Access-Control-Allow-Origin", "*");

    ret = MHD_queue_response(
        connection,
        MHD_HTTP_OK,
        response
    );
    MHD_destroy_response(response);

    return ret;
}


/**
* Create a command object based on incoming request details
**/
Cmd* CmdManager::dispatch(
    struct MHD_Connection* connection,
    const char* url, const char* method
) {
    if (strcmp(url, "/echo") == 0) {
        const char *val = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "msg");
        if (val) {
            return new CmdEcho(std::string(val));
        }
    }

    if (strcmp(url, "/stats") == 0) {
        return new CmdStats();
    }

    if (strcmp(url, "/env/day-night") == 0) {
        float target_val = atof(MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "val"));
        float total_time = atof(MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "time"));
        return new CmdDayNightAnimate(target_val, total_time);
    }

    return NULL;
}


/**
* Execute the queued commands
**/
void CmdManager::exec(GameState* st)
{
    Cmd* c;
    while (work.trypop(c)) {
        c->exec(st);
    }
}
