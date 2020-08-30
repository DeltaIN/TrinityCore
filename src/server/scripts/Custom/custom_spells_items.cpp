#include "ScriptPCH.h"
#include "ScriptMgr.h"
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "SpellHistory.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "SpellMgr.h"


class spell_prydaz : public AuraScript
{
    PrepareAuraScript(spell_prydaz);
    bool Validate(SpellInfo const* /*SpellInfo*/)
    {
        return ValidateSpellInfo({ 900031 });
    }

    void HandleCalc(AuraEffect const* auraEff, int32& amount, bool& /*canBeRecalculated*/)
    {
        //PreventDefaultAction();
        amount = (auraEff->GetCaster()->GetMaxHealth()) * 3;
    }



    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_prydaz::HandleCalc, EFFECT_0, SPELL_AURA_ANY);

    }
};

void _CustomCast(Unit* source, Unit* target, uint32 spellId, uint32 bp0Mod = 0, TriggerCastFlags trigger = TRIGGERED_NONE)
{
    CastSpellExtraArgs eArg;
    eArg.AddSpellMod(SPELLVALUE_BASE_POINT0, bp0Mod);
    eArg.SetTriggerFlags(trigger);

    source->CastSpell(target, spellId, eArg);
}

enum spellEmp {
    SPELL_DUMMY_EMP = 900032,
    SPELL_METEOR = 57467,
    SPELL_CBOLT = 71108,
    SPELL_CONFLAGRATE = 17962,
    SPELL_INCINERATE = 69973
};
class spell_emp_destruction : public SpellScript
{
    PrepareSpellScript(spell_emp_destruction);
    bool Validate(SpellInfo const*)
    {
        return ValidateSpellInfo({ SPELL_DUMMY_EMP });
    }

    void HandleDummy(SpellEffIndex effind)
    {
        Unit* caster = GetCaster();
        Player* _caster = caster->ToPlayer();

        Unit* target = GetHitUnit();
        Player* _target = target->ToPlayer();
        

        int32 randnum = irand(1,100);
        bool debug = true;

        if (debug) {
            int32 db_dmg = 100000;
            CastSpellExtraArgs _DbArg;
            _DbArg.AddSpellMod(SPELLVALUE_BASE_POINT0, db_dmg);
            _DbArg.SetTriggerFlags(TRIGGERED_FULL_MASK);
            GetCaster()->CastSpell(_target->GetPosition(), SPELL_CBOLT, _DbArg);
        }
        if (randnum == 100)
        {
            // Config
            int32 dmg = 12000000;



            // meteor (aoe cast)
            CastSpellExtraArgs _SpellArg;
            _SpellArg.AddSpellMod(SPELLVALUE_BASE_POINT0, dmg);
            _SpellArg.SetTriggerFlags(TRIGGERED_FULL_MASK);

            caster->CastSpell(_target->GetPosition(), SPELL_METEOR, _SpellArg);
        }
        else if (randnum >= 66 && randnum != 100)
        {
            // Config
            int32 dmg = 6600000;


            // cbolt
            CastSpellExtraArgs _SpellArg;
            _SpellArg.AddSpellMod(SPELLVALUE_BASE_POINT0, 3000000);
            _SpellArg.SetTriggerFlags(TRIGGERED_FULL_MASK);

            caster->CastSpell(_target->GetPosition(), SPELL_CBOLT, _SpellArg);
        }
        else if (randnum < 66 && randnum > 33)
        {
            // Config
            int32 dmg = 970000;


            // conflagrate
            CastSpellExtraArgs _SpellArg;
            _SpellArg.AddSpellMod(SPELLVALUE_BASE_POINT0, 3000000);
            _SpellArg.SetTriggerFlags(TRIGGERED_FULL_MASK);

            caster->CastSpell(_target->GetPosition(), SPELL_CONFLAGRATE, _SpellArg);
        }
        else {
            // Config
            int32 dmg = 1000000;

            // incinerate
            CastSpellExtraArgs _SpellArg;
            _SpellArg.AddSpellMod(SPELLVALUE_BASE_POINT0, dmg);
            _SpellArg.SetTriggerFlags(TRIGGERED_FULL_MASK);

            caster->CastSpell(_target->GetPosition(), SPELL_INCINERATE, _SpellArg);
        }
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_emp_destruction::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }     
};



void AddSC_spell_items()
{
    RegisterSpellScript(spell_emp_destruction);
    RegisterAuraScript(spell_prydaz);
}


