#define DISCORDPP_IMPLEMENTATION
#include "Discord.hpp"
#include <iostream>
#include <ctime>

Discord::Discord() = default;

Discord::~Discord() = default;

void Discord::init() {
    client = std::make_unique<discordpp::Client>();

    // Callback for debugging connection issues
    // client->AddLogCallback([](std::string message, discordpp::LoggingSeverity severity){
    //     if (severity >= discordpp::LoggingSeverity::Warning) {
    //         std::cerr << "[Discord] " << message << std::endl;
    //     } else {
    //         std::cout << "[Discord] " << message << std::endl;
    //     }
    // }, discordpp::LoggingSeverity::Info);

    uint64_t clientId = 1395908581886394419;
    client->SetApplicationId(clientId);

    // Create the Activity
    discordpp::Activity activity{};
    activity.SetState("Developing");
    activity.SetType(discordpp::ActivityTypes::Playing);

    // Set Assets
    discordpp::ActivityAssets assets{};
    assets.SetLargeImage("placeholder_icon");
    assets.SetLargeText("Iro Engine");
    activity.SetAssets(assets);

    // Set Timestamps
    discordpp::ActivityTimestamps timestamps{};
    timestamps.SetStart(time(nullptr));
    activity.SetTimestamps(timestamps);

    client->UpdateRichPresence(activity, [](discordpp::ClientResult result) {
        if (result.Successful()) {
            std::cout << "Discord activity updated successfully.\n";
        } else {
            std::cerr << "Failed to update Discord activity: " << result.Error() << "\n";
        }
    });

    // Asynchronously connect to Discord
    client->Connect();
    std::cout << "Connecting to Discord...\n";
}

void Discord::update() {
    discordpp::RunCallbacks();
}
