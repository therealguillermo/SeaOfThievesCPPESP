#include "config.h"
#include "json.h"
#include <filesystem>

nlohmann::json json;

namespace fs = std::filesystem;

void assign_item(item& item) {
    if (json[item.name.c_str()].empty())
        return;

    if (item.type == "float")
        *reinterpret_cast<float*>(item.value) = json[item.name.c_str()].get< float >();
    if (item.type == "int")
        *reinterpret_cast<int*>(item.value) = json[item.name.c_str()].get< int >();
    if (item.type == "bool")
        *reinterpret_cast<bool*>(item.value) = json[item.name.c_str()].get< bool >();
    if (item.type == "int [4]")
        for (int i = 0; i < 4; i++)
            reinterpret_cast<int*>(item.value)[i] = json[item.name.c_str()][std::to_string(i).c_str()].get< float >();
}

void save_item(item& item) {
    if (item.type == "float")
        json[item.name.c_str()] = *reinterpret_cast<float*>(item.value);
    if (item.type == "int")
        json[item.name.c_str()] = *reinterpret_cast<int*>(item.value);
    if (item.type == "bool")
        json[item.name.c_str()] = *reinterpret_cast<bool*>(item.value);
    if (item.type == "int [4]")
        for (int i = 0; i < 4; i++)
            json[item.name.c_str()][std::to_string(i).c_str()] = reinterpret_cast<int*>(item.value)[i];
}

void reset_item(item& item) {
    if (json[item.name.c_str()].empty())
        return;

    if (json[item.name.c_str()].is_array())
        for (int i = 0; i < 4; i++)
            json[item.name.c_str()][i].empty();

    json[item.name.c_str()].clear();
}

bool c_config::init() {

    m_directory = std::filesystem::current_path().string();

    if (!std::filesystem::exists(m_directory)) {
        if (!std::filesystem::create_directory(m_directory)) {
            _RPT1(_CRT_WARN, "Failed to create profile directory. Ignoring this error will result in not being able to create or save profiles.\n\n%s", m_directory);
            return false;
        }
    }

    item(Vars.GUI, "GUI", m_items);

    item(Vars.ESP.Player.bActive, "ESP.Player.bActive", m_items);
    item(Vars.ESP.Player.bHealth, "ESP.Player.bHealth", m_items);
    item(Vars.ESP.Player.bName, "ESP.Player.bName", m_items);
    item(Vars.ESP.Player.bTeam, "ESP.Player.bTeam", m_items);
    item(Vars.ESP.Player.bWeapon, "ESP.Player.bWeapon", m_items);
    item(Vars.ESP.Player.colorEnemy, "ESP.Player.colorEnemy", m_items);
    item(Vars.ESP.Player.colorTeam, "ESP.Player.colorTeam", m_items);

    item(Vars.ESP.Ships.bActive, "ESP.Ships.bActive", m_items);
    item(Vars.ESP.Ships.colorEnemy, "ESP.Ships.colorEnemy", m_items);
    item(Vars.ESP.Ships.colorTeam, "ESP.Ships.colorTeam", m_items);

    item(Vars.ESP.Animals.bActive, "ESP.Animals.bActive", m_items);
    item(Vars.ESP.Animals.bChicken, "ESP.Animals.bChicken", m_items);
    item(Vars.ESP.Animals.bPig, "ESP.Animals.bPig", m_items);
    item(Vars.ESP.Animals.bSnake, "ESP.Animals.bSnake", m_items);

    item(Vars.ESP.World.bFort, "ESP.World.bFort", m_items);
    item(Vars.ESP.World.bIslands, "ESP.World.bIslands", m_items);
    item(Vars.ESP.World.bMapPins, "ESP.World.bMapPins", m_items);
    item(Vars.ESP.World.bShipWreck, "ESP.World.bShipWreck", m_items);
    item(Vars.ESP.World.bMermaid, "ESP.World.bMermaid", m_items);
    item(Vars.ESP.World.colorWorld, "ESP.World.colorWorld", m_items);

    item(Vars.ESP.colorCommon, "ESP.colorCommon", m_items);
    item(Vars.ESP.colorRare, "ESP.colorRare", m_items);
    item(Vars.ESP.colorLegendary, "ESP.colorLegendary", m_items);
    item(Vars.ESP.colorMythical, "ESP.colorMythical", m_items);
    item(Vars.ESP.colorSpecial, "ESP.colorSpecial", m_items);
    item(Vars.ESP.colorOther, "ESP.colorOther", m_items);


    item(Vars.ESP.Skeleton.bActive, "ESP.Skeleton.bActive", m_items);
    item(Vars.ESP.Skeleton.bWeapon, "ESP.Skeleton.bWeapon", m_items);
    item(Vars.ESP.Skeleton.colorSkeleton, "ESP.Skeleton.colorSkeleton", m_items);

    item(Vars.ESP.Treasure.bActive, "ESP.Treasure.bActive", m_items);


    return true;
}

bool c_config::save(const std::string& file) {
    try {
        std::ofstream output_file = std::ofstream(m_directory + "/" + file);

        if (!output_file.good())
            return false;

        for (auto& item : m_items)
            save_item(item);

        output_file << std::setw(4) << json << std::endl;
        output_file.close();

        return true;
    } catch (std::ofstream::failure& ex) {
        _RPT1(_CRT_WARN, "Failed to save the default profile. Ignoring this warning will most likely result in future profiles not being saved correctly.\n\n%s", ex.what());
        return false;
    }
}

bool c_config::reset(const std::string& file) {
    std::ofstream output_file = std::ofstream(m_directory + "/" + file);

    if (!output_file.good())
        return false;

    for (auto& item : m_items)
        reset_item(item);

    output_file << std::setw(4) << json << std::endl;
    output_file.close();

    return true;
}

bool c_config::load(const std::string& file) {
    std::ifstream input_file = std::ifstream(m_directory + "/" + file);
    if (!input_file.good())
        return false;

    try {
        json << input_file;
    } catch (const std::exception& ex) {
        UNREFERENCED_PARAMETER(ex);
        _RPT2(_CRT_ERROR, "Failed to %s profile. Ignoring this error may prevent you from loading profiles.\n\n%s", file.c_str(), ex.what());

        input_file.close();
        return false;
    }

    for (auto& item : m_items)
        assign_item(item);

    input_file.close();

    return true;
}

void c_config::remove(const std::string& file) const {
    std::string path = m_directory + "/" + file;
    std::remove(path.c_str());
}

std::vector< std::string > c_config::get_configs() const {
    std::vector< std::string > output{ };

    for (auto& file_path : std::filesystem::directory_iterator(m_directory)) {
        if (file_path.path().string().empty())
            continue;

        if (file_path.path().string().find('.') != std::string::npos)
            continue;

        output.emplace_back(file_path.path().string().erase(0, m_directory.length() + 1));
    }

    return output;
}

bool c_config::import_from_clipboard(const std::string& file) {
    const auto get_clipboard_data = []() -> std::string {
        OpenClipboard(nullptr);

        HANDLE handle = GetClipboardData(CF_TEXT);
        const auto data = static_cast<char*>(GlobalLock(handle));

        std::string text(data);
        GlobalUnlock(handle);
        CloseClipboard();

        return text;
    };

    const std::string clipboard = get_clipboard_data();

    std::ofstream output = std::ofstream(m_directory + "/" + file);
    if (!output.good())
        return false;

    // clear our config
    output.clear();

    // write clipboard data to the file
    output << clipboard;

    // close the file after writing
    output.close();

    // re-load the imported config
    load(file);

    return true;
}

void c_config::export_to_clipboard(const std::string& file) const {
    std::ifstream input_file = std::ifstream(m_directory + "/" + file);
    std::string str((std::istreambuf_iterator< char >(input_file)), std::istreambuf_iterator< char >());

    const char* output = str.c_str();
    const size_t len = strlen(output) + 1;

    HGLOBAL mem = GlobalAlloc(GMEM_MOVEABLE, len);
    memcpy(GlobalLock(mem), output, len);

    GlobalUnlock(mem);
    OpenClipboard(nullptr);
    EmptyClipboard();
    SetClipboardData(CF_TEXT, mem);
    CloseClipboard();
}


bool c_offsets::init() {

    m_directory = std::filesystem::current_path().string();

    if (!std::filesystem::exists(m_directory)) {
        if (!std::filesystem::create_directory(m_directory)) {
            _RPT1(_CRT_WARN, "Failed to create profile directory. Ignoring this error will result in not being able to create or save profiles.\n\n%s", m_directory);
            return false;
        }
    }

    item(Offsets.UWorld.OwningGameInstance, "UWorld.OwningGameInstance", m_items);
    item(Offsets.UWorld.PersistentLevel, "UWorld.PersistentLevel", m_items);

    item(Offsets.UGameInstance.LocalPlayers, "UGameInstance.LocalPlayers", m_items);

    item(Offsets.ULocalPlayer.PlayerController, "ULocalPlayer.PlayerController", m_items);

    item(Offsets.APlayerController.CameraManager, "APlayerController.CameraManager", m_items);
    item(Offsets.APlayerController.Pawn, "APlayerController.Pawn", m_items);
    item(Offsets.APlayerController.ControlRotation, "APlayerController.ControlRotation", m_items);

    item(Offsets.AActor.actorId, "AActor.actorId", m_items);
    item(Offsets.AActor.HealthComponent, "AActor.HealthComponent", m_items);
    item(Offsets.AActor.WieldedItemComponent, "AActor.WieldedItemComponent", m_items);
    item(Offsets.AActor.rootComponent, "AActor.rootComponent", m_items);
    item(Offsets.AActor.PlayerState, "AActor.PlayerState", m_items);


    item(Offsets.APlayerCameraManager.CameraCache, "APlayerCameraManager.CameraCache", m_items);

    item(Offsets.APlayerState.PlayerName, "APlayerState.PlayerName", m_items);
    item(Offsets.AItemProxy.AItemInfo, "AItemProxy.AItemInfo", m_items);

    item(Offsets.AItemInfo.UItemDesc, "AItemInfo.UItemDesc", m_items);

    item(Offsets.ABootyItemInfo.BootyType, "ABootyItemInfo.BootyType", m_items);
    item(Offsets.ABootyItemInfo.Rarity, "ABootyItemInfo.Rarity", m_items);



    item(Offsets.AShip.CrewOwnershipComponent, "AShip.CrewOwnershipComponent", m_items);
    item(Offsets.UCrewOwnershipComponent.CrewId, "UCrewOwnershipComponent.CrewId", m_items);

    item(Offsets.ACrewService.Crews, "ACrewService.Crews", m_items);

    item(Offsets.AMapTable.MapPins, "AMapTable.MapPins", m_items);
    item(Offsets.AMapTable.ServerCenter, "AMapTable.ServerCenter", m_items);
    item(Offsets.AMapTable.TrackedShips, "AMapTable.TrackedShips", m_items);

    item(Offsets.UWieldedItemComponent.WieldedItem, "UWieldedItemComponent.WieldedItem", m_items);
    item(Offsets.AWieldableItem.ItemInfo, "AWieldableItem.ItemInfo", m_items);

    item(Offsets.AFauna.Name, "AFauna.Name", m_items);


    return true;
}

bool c_offsets::save(const std::string& file) {
    try {
        std::ofstream output_file = std::ofstream(m_directory + "/" + file);

        if (!output_file.good())
            return false;

        for (auto& item : m_items)
            save_item(item);

        output_file << std::setw(4) << json << std::endl;
        output_file.close();

        return true;
    } catch (std::ofstream::failure& ex) {
        _RPT1(_CRT_WARN, "Failed to save the default profile. Ignoring this warning will most likely result in future profiles not being saved correctly.\n\n%s", ex.what());
        return false;
    }
}

bool c_offsets::reset(const std::string& file) {
    std::ofstream output_file = std::ofstream(m_directory + "/" + file);

    if (!output_file.good())
        return false;

    for (auto& item : m_items)
        reset_item(item);

    output_file << std::setw(4) << json << std::endl;
    output_file.close();

    return true;
}

bool c_offsets::load(const std::string& file) {
    std::ifstream input_file = std::ifstream(m_directory + "/" + file);
    if (!input_file.good())
        return false;

    try {
        json << input_file;
    } catch (const std::exception& ex) {
        UNREFERENCED_PARAMETER(ex);
        _RPT2(_CRT_ERROR, "Failed to %s profile. Ignoring this error may prevent you from loading profiles.\n\n%s", file.c_str(), ex.what());

        input_file.close();
        return false;
    }

    for (auto& item : m_items)
        assign_item(item);

    input_file.close();

    return true;
}

void c_offsets::remove(const std::string& file) const {
    std::string path = m_directory + "/" + file;
    std::remove(path.c_str());
}

std::vector< std::string > c_offsets::get_configs() const {
    std::vector< std::string > output{ };

    for (auto& file_path : std::filesystem::directory_iterator(m_directory)) {
        if (file_path.path().string().empty())
            continue;

        if (file_path.path().string().find('.') != std::string::npos)
            continue;

        output.emplace_back(file_path.path().string().erase(0, m_directory.length() + 1));
    }

    return output;
}

bool c_offsets::import_from_clipboard(const std::string& file) {
    const auto get_clipboard_data = []() -> std::string {
        OpenClipboard(nullptr);

        HANDLE handle = GetClipboardData(CF_TEXT);
        const auto data = static_cast<char*>(GlobalLock(handle));

        std::string text(data);
        GlobalUnlock(handle);
        CloseClipboard();

        return text;
    };

    const std::string clipboard = get_clipboard_data();

    std::ofstream output = std::ofstream(m_directory + "/" + file);
    if (!output.good())
        return false;

    // clear our config
    output.clear();

    // write clipboard data to the file
    output << clipboard;

    // close the file after writing
    output.close();

    // re-load the imported config
    load(file);

    return true;
}

void c_offsets::export_to_clipboard(const std::string& file) const {
    std::ifstream input_file = std::ifstream(m_directory + "/" + file);
    std::string str((std::istreambuf_iterator< char >(input_file)), std::istreambuf_iterator< char >());

    const char* output = str.c_str();
    const size_t len = strlen(output) + 1;

    HGLOBAL mem = GlobalAlloc(GMEM_MOVEABLE, len);
    memcpy(GlobalLock(mem), output, len);

    GlobalUnlock(mem);
    OpenClipboard(nullptr);
    EmptyClipboard();
    SetClipboardData(CF_TEXT, mem);
    CloseClipboard();
}
