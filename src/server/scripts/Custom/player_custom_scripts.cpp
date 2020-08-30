#include <ScriptPCH.h>
#include <Unit.h>
#include <Player.h>

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



void AddSC_custom_ply_scripts()
{
    new first_join_script;
}
