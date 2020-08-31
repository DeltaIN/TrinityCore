#include "ScriptPCH.h"
#include <Unit.h>

// Instance Script
class instance_custom_pos : public InstanceMapScript
{
public:
    instance_custom_pos() : InstanceMapScript("instance_custom_pos", 602) {}

    struct instance_custom_pos_iScript : public InstanceScript
    {
        instance_custom_pos_iScript(InstanceMap* mp) : InstanceScript(mp) {}

        int numplayers = 0;
        std::vector<Creature*> mobs;
        std::map<Creature*, uint32> mxhp;
        uint32 check_timer = 7000;
        uint32 _check_timer = 7000;

        void OnCreatureCreate(Creature* crt) override
        {
            mobs.push_back(crt);
            mxhp.insert(mxhp.end(), { crt, crt->GetMaxHealth()});
            crt->SetRespawnTime(2^30);
        }

        void OnCreatureRemove(Creature* crt) override
        {
            for (size_t i = 0; i < mobs.size(); i++)
            {
                if (mobs.at(i) == crt)
                {
                    mobs.erase(mobs.begin()+i);
                }
            }

            for (auto const& [key, val] : mxhp)
            {
                if (key == crt)
                {
                    mxhp.erase(key);
                }
            }
        }

        void OnPlayerEnter(Player* pl) override
        {
            numplayers += 1;
        }

        void OnPlayerLeave(Player* pl) override
        {
            numplayers -= 1;
        }

        void Update(const uint32 diff) override
        {
            // add diff timer?
            if (check_timer <= diff)
            {
                for (Creature* crt : mobs)
                {
                    uint32 base_max_hp = mxhp.at(crt);
                    uint32 correct_max_hp = base_max_hp * (1 + (numplayers * 0.5));
                    uint32 current_max_hp = crt->GetMaxHealth();

                    if (current_max_hp != correct_max_hp)
                    {
                        crt->SetMaxHealth(correct_max_hp);
                        current_max_hp = correct_max_hp;
                    }
                }

                check_timer = _check_timer;
            }
            else {
                check_timer -= diff;
            }

           
        }
        
    };

    InstanceScript* GetInstanceScript(InstanceMap* mp) const override
    {
        return new instance_custom_pos_iScript(mp);
    }
};

// Boss #1 (Njekar)
class boss_njekar : public CreatureScript
{
public:
    boss_njekar() : CreatureScript("boss_njekar") {}

    struct boss_njekarAI : public BossAI
    {
        boss_njekarAI(Creature* crt) : BossAI(crt, 0) {
            me->SetReactState(ReactStates::REACT_DEFENSIVE);
            me->ClearUnitState(UnitState::UNIT_STATE_UNATTACKABLE);
        }

        uint32 summon_timer = 0;
        uint32 ms_timer = 0;
        uint32 bonestorm_timer = 0;
        bool bladestorm = false;

        bool special_triggered_event = false;

        void Reset() override
        {
            me->ClearUnitState(UnitState::UNIT_STATE_UNATTACKABLE);

            me->RemoveAura(58808);

            summon_timer = 21000;
            ms_timer = 18000;
            bonestorm_timer = 15500;
            bladestorm = false;
            special_triggered_event = false;

            me->SetPowerType(Powers::POWER_RAGE, true);
            me->SetPower(Powers::POWER_RAGE, 4000000);
        }

        void JustEngagedWith(Unit* vct) override
        {
            CastSpellExtraArgs cerg;
            cerg.SetTriggerFlags(TriggerCastFlags::TRIGGERED_FULL_MASK);
            SpellCastResult scr = me->CastSpell(me, 58808, cerg);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (me->GetHealth() <= 5 && !special_triggered_event)
            {
                special_triggered_event = true;
                me->AddUnitState(UnitState::UNIT_STATE_UNATTACKABLE);
            }


            // Bladestorm
            if (me->GetHealth() <= 50 && !bladestorm)
            {
                DoCast(69652);
                bladestorm = !bladestorm;
            }

            // Necromancer Summon
            if (summon_timer <= diff)
            {
                me->SummonCreature(70009, me->GetVictim()->GetPosition(), TempSummonType::TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 27000);
                summon_timer = 21000;
            }
            else
            {
                summon_timer -= diff;
            }

            // Bone Storm Passive
            if (bonestorm_timer <= diff)
            {
                DoCastVictim(69075);
                bonestorm_timer = 15500;
            }
            else
            {
                bonestorm_timer -= diff;
            }

            // MS Cast
            if (ms_timer <= diff)
            {
                DoCastVictim(47486);
                ms_timer = 18000;
            }
            else {
                    ms_timer -= diff;
            }
                
            
        }
    };

    CreatureAI* GetAI(Creature* crt) const override
    {
        return new boss_njekarAI(crt);
    }
};

// Necromancer Mobs/Adds
class mob_pos_necromancer : public CreatureScript
{
public:
    mob_pos_necromancer() : CreatureScript("mob_pos_necromancer") {}

    struct boss_njekar_necromancerAI : public ScriptedAI
    {
        boss_njekar_necromancerAI(Creature* crt) : ScriptedAI(crt) {}

        double hellfire_chance = 40.0;
        uint32 hellfire_timer = 6000;

        CastSpellExtraArgs csea;
        

        void Reset()
        {
            csea.SetTriggerFlags(TriggerCastFlags::TRIGGERED_IGNORE_CAST_IN_PROGRESS);
            hellfire_timer = 6000;
        }

        void UpdateAI(const uint32 diff) override
        {
            if (!UpdateVictim())
                return;

            //test
            // Hellfire highest priority
            if (me->GetCurrentSpellCastTime(70283) > 0)
                return;

            if (hellfire_timer <= diff)
            {
                if (rand_chance() <= hellfire_chance)
                {
                    DoCast(me, 70283, csea);
                }

                hellfire_timer = 6000;
            }
            else {
                hellfire_timer -= diff;
            }

            // Heal 2nd priority
            if (me->GetCurrentSpellCastTime(71783) > 0)
                return;

            Unit* slc = me->SelectNearbyTarget(me->GetVictim(), 10);
            if (slc->GetMaxHealth() - slc->GetHealth() > 50000)
            {
                DoCast(slc, 71783, csea);
            }
                


            // Shadow bolt last priority
            if (me->GetCurrentSpellCastTime(70270) <= 0)
            {
                DoCastVictim(70270);
            }
        }
    };
    
    CreatureAI* GetAI(Creature* crt) const override
    {
        return new boss_njekar_necromancerAI(crt);
    }
};

void AddSC_instance_pos()
{
    new instance_custom_pos;
    new boss_njekar;
    new mob_pos_necromancer;
}
