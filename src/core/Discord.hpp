#pragma once

#include "lib/discord/discordpp.h"
#include <memory>

// Manages the connection and Rich Presence updates with the Discord client.
class Discord {

private:
    std::unique_ptr<discordpp::Client> client;


public:
    Discord();
    ~Discord();

    void init();
    void update();

};
