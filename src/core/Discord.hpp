#pragma once

#include "lib/discord/discordpp.h"
#include <memory>

class Discord {
private:
    std::unique_ptr<discordpp::Client> client;

public:
    Discord();
    ~Discord();

    void init();
    void update();
};
