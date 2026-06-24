#include "JungleGame.h"
#include "Modules/ModuleManager.h"

DEFINE_LOG_CATEGORY(LogJungleGame);

class FJungleGameModule final : public FDefaultGameModuleImpl
{
};

IMPLEMENT_PRIMARY_GAME_MODULE(FJungleGameModule, JungleGame, "JungleGame");
