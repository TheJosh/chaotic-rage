// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <stdint.h>
#include <stddef.h>
#include "cmd.h"
#include "../util/thread_safe_queue.h"

struct MHD_Daemon;
class GameState;


class CmdManager
{
    public:
        /**
        * Start the daemon
        **/
        CmdManager(uint16_t port);

        /**
        * Stop the daemon
        **/
        ~CmdManager();

        /**
        * Handle incoming HTTP requests
        **/
        int AccessHandlerCallback(
            struct MHD_Connection* connection,
            const char* url, const char* method,
            const char* upload_data, size_t* upload_data_size,
            void** ptr
        );

        /**
        * Create a command object based on incoming request details
        **/
        Cmd* dispatch(const char* url, const char* method);

        /**
        * Execute the queued commands
        **/
        void exec(GameState* st);

    protected:
        struct MHD_Daemon* daemon;
        ThreadSafeQueue<Cmd*> work;

    private:
        /**
        * Prevent copying
        **/
        CmdManager(const CmdManager& that);
};
