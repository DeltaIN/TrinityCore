#include <ScriptPCH.h>
#include <Unit.h>
#include <Player.h>
#define EnableFJS true
#define EnableAAS false

// first join script (add default custom items, set level, skill, etc.)
class first_join_script : public PlayerScript
{
public:
    first_join_script() : PlayerScript("first_join_script") {}

    void OnLogin(Player* ply, bool firstlogin) override
    {
        if (!firstlogin)
            return;

        ply->SetLevel(80);
        
    }


};

class player_chat_antiadvert : public PlayerScript
{
public:
    player_chat_antiadvert() : PlayerScript("player_chat_antiadvert") {}

    std::vector<std::string> blockedmessages = {"abcdefghijklmnopqrstuvwxyz"};

    void OnChat(Player* sender, uint32 type, uint32 lang, std::string& msg)
    {
        std::string sendername = sender->GetName();
        for (std::string blk : blockedmessages)
        {
            std::string lwr = msg;
            std::transform(lwr.begin(), lwr.end(), lwr.begin(), ::tolower);

            if (lwr.compare(blk) != std::string::npos)
            {
                msg = "";
                break;
            }
        }
    }
};



void AddSC_custom_ply_scripts()
{
    if (EnableFJS)
        new first_join_script;

    if (EnableAAS)
        new player_chat_antiadvert;
    
}
