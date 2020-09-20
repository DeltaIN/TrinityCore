#include <ScriptPCH.h>
#include <SpellAuras.h>
#include <Spell.h>
#include <SpellScript.h>
#include <SpellMgr.h>

enum Events
{
    EVENT_SHADOW_BOLT = 1
};

class test_boss_script : public CreatureScript
{
    test_boss_script() : CreatureScript("test_boss_script") {}

    struct test_boss_scriptAI : public ScriptedAI
    {
        test_boss_scriptAI(Creature* crt) : ScriptedAI(crt) {
            
        }

        void Reset() override
        {
            me->m_Events.KillAllEvents(true);
        }

        void JustEnteredCombat(Unit* who) override
        {
            me->m_Events.AddEvent(EVENT_SHADOW_BOLT, 4000);
        }

        void UpdateAI(uint32 diff) override
        {
            if (!UpdateVictim())
                return;

            me->m_Events.Update(diff);

            
        }
    };

    CreatureAI* GetAI(Creature* crt) const override
    {

    }
};

class test_world_script : public WorldScript
{
public:
    test_world_script() : WorldScript("test_world_script") {}
};


class test_aura_script : public AuraScript
{
public:
    PrepareAuraScript(test_aura_script);   

    void Register() override
    {

    }
};

class test_spell_script : public SpellScriptLoader
{
public:
    test_spell_script() : SpellScriptLoader("test_spell_script") {}

    class test_spell_script_spscript : public SpellScript
    {
    public:
        PrepareSpellScript(test_spell_script_spscript);

        void HandleEffectHit(SpellEffIndex effIndex)
        {
            Spell* spl = SpellScript::GetSpell();
            const SpellInfo* splinf = GetSpellInfo();
            if (Player* cst = GetCaster()->ToPlayer())
            {
                SpellModifier* spmod = new SpellModifier(NULL);
                spmod->op = SpellModOp::SPELLMOD_DAMAGE_MULTIPLIER;
                spmod->value = 1.5;
                spmod->type = SpellModType::SPELLMOD_FLAT;
                spmod->spellId = splinf->Id;
                

                cst->AddSpellMod(spmod, true);
            }
        }

        void Register() override
        {
            OnEffectHit += SpellEffectFn(test_spell_script_spscript::HandleEffectHit, EFFECT_0, SPELL_EFFECT_ANY);
        }
    };
};

void AddSC_test_scripts()
{
    new test_world_script;
}
