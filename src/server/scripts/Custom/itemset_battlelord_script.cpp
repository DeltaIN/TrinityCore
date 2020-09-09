#include "ScriptPCH.h"
#include <SpellAuraEffects.h>
#include <Unit.h>
#include <Player.h>
#include <thread>
#include <Item.h>
#include <Pet.h>
#include <PetAI.h>


void battlelord_grow_heal(Aura* aur)
{
    while (true)
    {
        if (aur->IsExpired())
            break;

        AuraEffect* aureff = aur->GetEffect(EFFECT_0);

        aureff->SetAmount(aureff->GetAmount() * frand(0.7, 1.7));
        

        std::this_thread::sleep_for(std::chrono::milliseconds(aureff->GetAmplitude()));
        
    }

    return;
}

class item_battlelord_helmet : public AuraScript
{
	PrepareAuraScript(item_battlelord_helmet);

    std::thread* heal_thread;

    void handleRemove(AuraEffect const* aureff, AuraEffectHandleModes mode)
    {
        heal_thread->join();
        heal_thread->detach();
    }

	void handleCalc(AuraEffect const* aureff, int32& amount, bool& c)
	{
		AuraScript::GetAura()->SetCritChance(0);
		uint32 plyhp = GetUnitOwner()->GetMaxHealth();

        uint32 desr = plyhp * 0.04;
		amount = (desr / 10);

        
        heal_thread = new std::thread(battlelord_grow_heal, GetAura());
		
	}


	void Register() override
	{
        AfterEffectRemove += AuraEffectRemoveFn(item_battlelord_helmet::handleRemove, EFFECT_0, SPELL_AURA_ANY, AuraEffectHandleModes::AURA_EFFECT_HANDLE_DEFAULT);
		DoEffectCalcAmount += AuraEffectCalcAmountFn(item_battlelord_helmet::handleCalc, EFFECT_0, SPELL_AURA_ANY);
	}
};

class item_battlelord_chest : public AuraScript
{
	PrepareAuraScript(item_battlelord_chest);

	void h(AuraEffect const* e, int32& a, bool& c)
	{
		GetAura()->SetCritChance(50.0F);

		Unit* o = GetCaster();

		a = o->GetMaxHealth() * 0.09;
	}

	void Register() override
	{
		DoEffectCalcAmount += AuraEffectCalcAmountFn(item_battlelord_chest::h, EFFECT_0, SPELL_AURA_ANY);
	}
};

class item_battlelord_hands_dummy : public AuraScript
{
	PrepareAuraScript(item_battlelord_hands_dummy);

	void onproc(AuraEffect const* aureff, ProcEventInfo& pinfo)
	{
		uint32 dmg = pinfo.GetDamageInfo()->GetDamage();

		AuraScript::GetEffect(aureff->GetEffIndex())->SetAmount(dmg); // set amount of this effect to the amount of damage that procced the effect

		

	}

	void Register() override
	{
		OnEffectProc += AuraEffectProcFn(item_battlelord_hands_dummy::onproc, EFFECT_0, SPELL_AURA_ANY);
	}
};

class item_battlelord_hands : public AuraScript
{
	PrepareAuraScript(item_battlelord_hands);

	
	int32 old_dur;

	void h(AuraEffect const* e, int32& a, bool& c)
	{
        Unit* o = GetCaster();
		
		uint32 dmg = o->GetAura(900050, o->GetGUID())->GetEffect(0)->GetAmount(); // get the amount of damage that procced the dummy effect
		

        AuraScript::GetAura()->SetCritChance(25.0F);
        a = dmg * 0.2;
	}



	void Register() override
	{
		DoEffectCalcAmount += AuraEffectCalcAmountFn(item_battlelord_hands::h, EFFECT_0, SPELL_AURA_ANY);
	}
};

class creature_battlelord_pet : public CreatureScript
{
public:
    creature_battlelord_pet() : CreatureScript("creature_battlelord_pet") {}

    struct creature_battlelord_petAI : public PetAI
    {
        Player* owner;

        enum pet_spells
        {
            SPELL_HEAL_OWNER = 0,
            SPELL_DAMAGE_VICTIM = 1
        };

        std::vector<std::pair<pet_spells, uint32>> spell_timers;

        creature_battlelord_petAI(Creature* crt) : PetAI(crt) {
            if (Player* wne = me->GetOwner()->ToPlayer())
                owner = wne;

            spell_timers.push_back(std::make_pair(SPELL_DAMAGE_VICTIM, 4000));
        }

        

        void OwnerAttacked(Unit* who) override
        {
            AttackStart(who);
        }


        void OwnerAttackedBy(Unit* who) override
        {
            if (!me->GetReactState() == ReactStates::REACT_DEFENSIVE)
                AttackStart(who);
        }

        void UpdateAI(uint32 diff) override
        {
            if (!UpdateVictim())
                return;

            if (!owner)
                return;

            if (!me->GetCurrentSpell(CURRENT_GENERIC_SPELL) && owner->GetHealthPct() < 50)
                me->CastSpell(owner, SPELL_HEAL_OWNER);

            if (spell_timers[SPELL_DAMAGE_VICTIM].second <= diff && !me->GetCurrentSpell(CURRENT_GENERIC_SPELL))
            {
                me->CastSpell(me->GetVictim(), SPELL_DAMAGE_VICTIM);
                spell_timers[SPELL_DAMAGE_VICTIM].second = 4000;
            }
            else {
                spell_timers[SPELL_DAMAGE_VICTIM].second -= diff;
            }
        }
    };

    CreatureAI* GetAI(Creature* crt) const override
    {
        return new creature_battlelord_petAI(crt);
    }
};

class item_battlelord_undefined : public AuraScript
{
    PrepareAuraScript(item_battlelord_undefined);

    void effapply(const AuraEffect* aureff, AuraEffectHandleModes mode)
    {
        if (Player* owner = GetAura()->GetUnitOwner()->ToPlayer())
        {
            if (owner->GetPet())
                return;

            Pet* pt = owner->SummonPet(99900, 0, 0, 0, 0, PetType::SUMMON_PET, 0, true);
            pt->SetAI(new creature_battlelord_pet::creature_battlelord_petAI(pt));
        }
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(effapply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_DEFAULT);
    }
};


void AddSC_itemset_battlelord_script()
{
	RegisterAuraScript(item_battlelord_helmet);
	RegisterAuraScript(item_battlelord_chest);
	RegisterAuraScript(item_battlelord_hands_dummy);
    RegisterAuraScript(item_battlelord_hands);
}
