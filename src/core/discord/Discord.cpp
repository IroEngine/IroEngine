#define DISCORDPP_IMPLEMENTATION
#include "Discord.hpp"
#include <ctime>
#include <iostream>

Discord::Discord() = default;
Discord::~Discord() = default;

void Discord::init() {
    client = std::make_unique<discordpp::Client>();

    // Useful for debugging connection issues.
    // client->AddLogCallback([](std::string message, discordpp::LoggingSeverity severity){
    //     if (severity >= discordpp::LoggingSeverity::Warning) {
    //         std::cerr << "[Discord] " << message << std::endl;
    //     } else {
    //         std::cout << "[Discord] " << message << std::endl;
    //     }
    // }, discordpp::LoggingSeverity::Info);

    client->SetApplicationId(1395908581886394419);

    discordpp::Activity activity {};
    activity.SetType(discordpp::ActivityTypes::Playing);
    activity.SetState("Developing");

    discordpp::ActivityAssets assets {};
    assets.SetLargeImage("iroengine_logo");
    assets.SetLargeText("Iro Engine");
    activity.SetAssets(assets);

    discordpp::ActivityTimestamps timestamps {};
    timestamps.SetStart(time(nullptr));
    activity.SetTimestamps(timestamps);

    client->UpdateRichPresence(activity, [](discordpp::ClientResult result) {
        if (result.Successful()) {
            std::cout << "Discord activity updated successfully.\n";
        } else {
            std::cerr << "Failed to update Discord activity: " << result.Error() << "\n";
        }
    });

    // Connection is asynchronous.
    client->Connect();
    std::cout << "Connecting to Discord...\n";
}

void Discord::update() {
    discordpp::RunCallbacks();
}
