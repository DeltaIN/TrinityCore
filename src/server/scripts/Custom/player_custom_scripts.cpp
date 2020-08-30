#include <ScriptPCH.h>
#include <Unit.h>
#include <Player.h>
#include <ChatCommand.h>
#include <Chat.h>
#include <RBAC.h>

// METHODS
// SCRIPT CLASSES

// first join script (add default custom items, set level, skill, etc.)
class first_join_script : public PlayerScript
{
public:
    first_join_script() : PlayerScript("first_join_script") {}

    void OnLogin(Player* ply, bool firstlogin) override
    {
        if (!firstlogin)
            return;

        // Variables
        uint8 classid = ply->GetClass();
        uint32 specid = ply->GetActiveSpec();

        if (specid == 71 || specid == 72)
        {
            for (int I = 99900; I <= 99907; I++)
            {
                ply->AddItem(I, 1);
            }
        }

    }


};

class test_cmd_script : public CommandScript
{
public:
    test_cmd_script() : CommandScript("test_cmd_script") {}

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> cmds =
        {
            {"test", rbac::RBAC_PERM_COMMAND_CHEAT, true, &HandleTestCommand, ""}, // name, perm, allow console, handler reference, help
        };


        return cmds;
    }

    static bool HandleTestCommand(ChatHandler* handler, char const* args)
    {
        Player* trg;
        handler->extractPlayerTarget((char*)args, &trg);
        trg->Kill(trg, trg, false);
        return true;
    }
};

void AddSC_custom_ply_scripts()
{
    new first_join_script;
    new test_cmd_script;
}
