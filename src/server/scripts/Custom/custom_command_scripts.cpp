#include <ScriptPCH.h>
#include <ChatCommand.h>
#include <Chat.h>
#include <RBAC.h>

class test_cmd_script : public CommandScript
{
public:
    test_cmd_script() : CommandScript("test_cmd_script") {}

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> cmds =
        {
            {"g_items", rbac::RBAC_PERM_COMMAND_CHEAT, true, &HandleTestCommand, ""}, // name, perm, allow console, handler reference, help
        };


        return cmds;
    }

    static bool HandleTestCommand(ChatHandler* handler, char const* args)
    {
        Player* trg;
        handler->extractPlayerTarget((char*)args, &trg);
        trg->SetGameMaster(false);
        return true;
    }
};

void AddSC_custom_command_scripts()
{
    new test_cmd_script;
}
