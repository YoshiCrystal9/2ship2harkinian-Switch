#ifndef RANDO_SPOILER_H
#define RANDO_SPOILER_H

#include <vector>
#include <string>
#include "nlohmann/json.hpp"

namespace Rando {

namespace Spoiler {

extern std::vector<std::string> spoilerOptions;
void RefreshOptions();
nlohmann::json GenerateFromSaveContext();
void SaveToFile(std::string fileName, nlohmann::json spoiler);
nlohmann::json LoadFromFile(std::string filePath);
void ApplyToSaveContext(nlohmann::json spoiler);
bool HandleFileDropped(std::string filePath);

} // namespace Spoiler

} // namespace Rando

#endif
