#include <ScriptPCH.h>

class test_boss_script : public CreatureScript
{
    test_boss_script() : CreatureScript("test_boss_script") {}

    struct test_boss_scriptAI : public BossAI
    {
        test_boss_scriptAI(Creature* crt) : BossAI(crt, 0) {
            
        }
    };

    CreatureAI* GetAI(Creature* crt) const override
    {

    }
};
