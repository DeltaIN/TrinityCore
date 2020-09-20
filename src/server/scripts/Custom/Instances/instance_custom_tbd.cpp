#include "ScriptPCH.h"
#include <Unit.h>
#include <Map.h>
#include <MapRefManager.h>
#include <SpellAuras.h>
#include <SpellAuraEffects.h>
#include <Spell.h>

// ----- GLOBALS -----
namespace custom_tbd_constants
{
    const uint32 map_id = 0;
    const std::string script_name = "instance_custom_tbd";

    enum eIDs
    {
        SPELL_BLEED_HIT = 900057,
        SPELL_BLEED_HIT_DAMAGE = 900057,

        ENTRY_DREKAR_CREATURE = 400002
    };
}

// ----- TYPES -----

// ----- INSTANCE/MAP/WORLD SCRIPTS -----
class instance_custom_tbd : public InstanceMapScript
{
public:
    instance_custom_tbd() : InstanceMapScript(custom_tbd_constants::script_name.c_str(), custom_tbd_constants::map_id) {}

    struct instance_custom_tbd_Instance : public InstanceScript
    {
        const Map* map_ref; // current map object
        instance_custom_tbd_Instance(Map* map) : InstanceScript(map), map_ref(map) {}
        
        void Update(uint32 diff) override
        {

        }
    };
};

// ----- CREATURE SCRIPTS -----
class boss_drekar : public CreatureScript
{
public:
    boss_drekar() : CreatureScript("boss_drekar") {}

    struct boss_drekar_script : public BossAI
    {
    public:
        boss_drekar_script(Creature* crt) : BossAI(crt, 0) {}

        void Reset() override
        {
            if (Aura* aurcheck = me->GetAura(custom_tbd_constants::SPELL_BLEED_HIT, me->GetGUID()))
                me->RemoveAura(aurcheck);

            
        }

        void JustEngagedWith(Unit* crt) override
        {
            me->AddAura(custom_tbd_constants::SPELL_BLEED_HIT, me);

            DoZoneInCombat();
        }
    };

    CreatureAI* GetAI(Creature* crt) const override
    {
        return new boss_drekar_script(crt);
    }
};

// ----- SPELL/AURA SCRIPTS -----
class boss_drekar_aura_bleedHit : public AuraScript
{
    PrepareAuraScript(boss_drekar_aura_bleedHit);

    uint32 damage_spell_id = custom_tbd_constants::SPELL_BLEED_HIT_DAMAGE;

    void HandleEffectProc(AuraEffect const* aureff, ProcEventInfo& procinfo)
    {
        PreventDefaultAction();

        Unit* procsource = procinfo.GetActor();
        Unit* proctarget = procinfo.GetActionTarget();

        uint32 damage_amount = proctarget->GetMaxHealth() / 5;
        if (uint32 proc_hit_damage = procinfo.GetDamageInfo()->GetDamage())
            damage_amount += proc_hit_damage;


        // Check if proc target has an existing aura and update it
        if (Aura* exist_aura = proctarget->GetAura(damage_spell_id, procsource->GetGUID()))
        {
            damage_amount = damage_amount * 2;

            int32 cur_amount = exist_aura->GetEffect(0)->GetAmount();
            int32 cur_duration = exist_aura->GetDuration();
            int32 cur_max_duration = exist_aura->GetMaxDuration();

            exist_aura->SetMaxDuration(cur_max_duration * 1.5);
            exist_aura->SetDuration(exist_aura->GetMaxDuration());
            exist_aura->GetEffect(0)->SetAmount(cur_amount + (damage_amount * 2) );

            if (Player* ply = proctarget->ToPlayer())
            {
                ply->SendPlaySpellImpact(ply->GetGUID(), 19280);
            }
        }
        else { // apply new aura
            //procsource->CastSpell(proctarget, damage_spell_id);
            Aura* new_aura = procsource->AddAura(damage_spell_id, proctarget);
            new_aura->GetEffect(0)->SetAmount(damage_amount);
        }
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(boss_drekar_aura_bleedHit::HandleEffectProc, EFFECT_0, SPELL_AURA_ANY);
    }
};

void AddSC_instance_custom_tbd()
{
    new instance_custom_tbd;
    new boss_drekar;
    RegisterAuraScript(boss_drekar_aura_bleedHit);
}
