#include <ScriptPCH.h>
#include <ChatCommand.h>
#include <Chat.h>
#include <RBAC.h>
#include <Pet.h>
#include <Creature.h>
#include <PetAI.h>

std::vector<Unit*> GetNearbyPlayers(UnitAI* source, uint32 scanCount = 8)
{
    std::vector<Unit*> units;

    for (uint32 scanner = 0; scanner < scanCount; scanner++)
    {
        if (Unit* validate_target = source->SelectTarget(SELECT_TARGET_MINDISTANCE, scanner))
        {
            units.insert(units.end(), validate_target);
        }
    }

    return units;
}

class test_pet_script : public CreatureScript
{
public:
    test_pet_script() : CreatureScript("test_pet_script") {}

    struct test_pet_scriptAI : public PetAI // PetAI inherits from CreatureAI and UnitAI
    {
        test_pet_scriptAI(Creature* crt) : PetAI(crt) {
            me->AddUnitState(UnitState::UNIT_STATE_UNATTACKABLE);
            me->RemoveUnitMovementFlag(MovementFlags::MOVEMENTFLAG_MASK_MOVING);
            me->AddUnitMovementFlag(MovementFlags::MOVEMENTFLAG_ROOT); 
        }

        uint32 dmgtimer = 250;

        void Reset() override
        {
            dmgtimer = 250;
        }

        void UpdateAI(uint32 diff) override
        {
            if (dmgtimer <= diff)
            {
                for (Unit* un : GetNearbyPlayers(this, 10))
                {
                    me->DealDamage(me, un, un->GetMaxHealth()/15, (const CleanDamage*)nullptr, DOT);
                }

                dmgtimer = 250;
            }
            else {
                dmgtimer -= diff;
            }
        }
    };

    CreatureAI* GetAI(Creature* crt) const override
    {
        return new test_pet_scriptAI(crt);
    }
};

class test_pet_script_unitcvr : public CreatureScript
{
public:
    test_pet_script_unitcvr() : CreatureScript("test_pet_script_unitcvr") {}

    struct test_pet_script_unitcvrAI : public CreatureAI
    {
        test_pet_script_unitcvrAI(Creature* crt) : CreatureAI(crt) {
            me->AddUnitState(UnitState::UNIT_STATE_UNATTACKABLE);
            me->StopMoving();
            me->AddUnitMovementFlag(MovementFlags::MOVEMENTFLAG_ROOT);
        }

        uint32 dmgtimer = 250;

        void Reset() override
        {
            dmgtimer = 250;
        }

        void UpdateAI(uint32 diff) override
        {
            if (dmgtimer <= diff)
            {
                for (Unit* un : GetNearbyPlayers(this, 10))
                {
                    me->DealDamage(me, un, un->GetMaxHealth() / 15, (const CleanDamage*)nullptr, DOT);
                }

                dmgtimer = 250;
            }
            else {
                dmgtimer -= diff;
            }
        }
    };

    CreatureAI* GetAI(Creature* crt) const override
    {
        return new test_pet_script_unitcvrAI(crt);
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
        handler->extractQuotedArg((char*)args);
        trg->SetGameMaster(false);
        Pet *plpet = trg->SummonPet(0, trg->GetPositionX(), trg->GetPositionY(), trg->GetPositionZ(), trg->GetOrientation(), PetType::MAX_PET_TYPE, 2500);
        plpet->SetAI(new test_pet_script::test_pet_scriptAI(plpet));
       

        return true;
    }
};

void AddSC_custom_command_scripts()
{
    new test_cmd_script;
    new test_pet_script;
    new test_pet_script_unitcvr;
}
