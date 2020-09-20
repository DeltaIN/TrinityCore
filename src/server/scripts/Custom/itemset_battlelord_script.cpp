#include "ScriptPCH.h"
#include <SpellAuraEffects.h>
#include <Unit.h>
#include <Player.h>
#include <thread>
#include <Item.h>
#include <Pet.h>
#include <PetAI.h>

/* See item_battlelord_helmet::HandlePeriodic
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
*/

class item_battlelord_helmet : public AuraScript
{
	PrepareAuraScript(item_battlelord_helmet);

    //std::thread* heal_thread;

    void handleRemove(AuraEffect const* aureff, AuraEffectHandleModes mode)
    {
        //heal_thread->join();
        //heal_thread->detach();
    }

    void HandlePeriodic(AuraEffect* aureff)
    {
        int32 current_amount = aureff->GetAmount();
        int32 new_amount = current_amount * frand(0.7, 1.7);
        aureff->SetAmount(new_amount);
    }

	void handleCalc(AuraEffect const* aureff, int32& amount, bool& c)
	{
		AuraScript::GetAura()->SetCritChance(7.0F);
		uint32 plyhp = GetUnitOwner()->GetMaxHealth();

        uint32 desr = plyhp * 0.04;
		amount = (desr / 10);


        
        //heal_thread = new std::thread(battlelord_grow_heal, GetAura());
		
	}


	void Register() override
	{
        AfterEffectRemove += AuraEffectRemoveFn(item_battlelord_helmet::handleRemove, EFFECT_0, SPELL_AURA_ANY, AuraEffectHandleModes::AURA_EFFECT_HANDLE_DEFAULT);
		DoEffectCalcAmount += AuraEffectCalcAmountFn(item_battlelord_helmet::handleCalc, EFFECT_0, SPELL_AURA_ANY);
        OnEffectUpdatePeriodic += AuraEffectUpdatePeriodicFn(item_battlelord_helmet::HandlePeriodic, EFFECT_0, SPELL_AURA_ANY);
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

class item_battlelord_legs_pet : public CreatureScript
{
public:
    item_battlelord_legs_pet() : CreatureScript("item_battlelord_legs_pet") {}
    struct item_battlelord_legs_petAI : public PetAI
    {
        item_battlelord_legs_petAI(Creature* crt) : PetAI(crt) {}

        uint32 dmgtimer;

        void Reset() override
        {
            dmgtimer = 500;
        }

        void UpdateAI(uint32 diff) override
        {
            me->DealDamage(me, me->GetVictim(), 40000, (const CleanDamage*)nullptr, DamageEffectType::DOT);
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new item_battlelord_legs_petAI(creature);
    }
};

class item_battlelord_legs : public AuraScript
{
    PrepareAuraScript(item_battlelord_legs);

    void onProc(ProcEventInfo& pinfo)
    {
        if (Player* ply = AuraScript::GetUnitOwner()->ToPlayer())
        {
            Pet* plypet = ply->SummonPet(0, ply->GetPositionX(), ply->GetPositionY(), ply->GetPositionZ(), ply->GetOrientation(), PetType::SUMMON_PET, 4000);
        }
    }

    void Register() override
    {
        OnProc += AuraProcFn(onProc);
    }
};


void AddSC_itemset_battlelord_script()
{
	RegisterAuraScript(item_battlelord_helmet);
	RegisterAuraScript(item_battlelord_chest);
	RegisterAuraScript(item_battlelord_hands_dummy);
    RegisterAuraScript(item_battlelord_hands);
}
