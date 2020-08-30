// BOSS HERALD OF DESTRUCTION ENTRY 400000
#include "ScriptPCH.h"
#include "SpellAuras.h"
#include "SpellAuraEffects.h"

//SPELL_SHADOW_CLEAVE = 72494
enum boss_spells
{
	SPELL_SHADOW_CLEAVE_PERIODIC = 900041,
	SPELL_NECROTIC_STRIKE = 72492,
	SPELL_BLAZING_DECOMPOSITION = 900042,
	SPELL_METEOR = 900053
};


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

class spell_magmatic_eruption : public SpellScript
{
	PrepareSpellScript(spell_magmatic_eruption);
	bool Validate(SpellInfo const*)
	{
		return ValidateSpellInfo({ 900044 });
	}

	void handleHit()
	{
		Unit* hunit = SpellScript::GetHitUnit();
		Unit* cunit = SpellScript::GetCaster();


		// Remove flame shield
		if (cunit->HasAura(900043))
		{
			cunit->RemoveAura(900043);
		}

		for (int i = 0; i < 90; i++)
		{

			if (hunit != SpellScript::GetCaster())
			{
				//CastSpellExtraArgs carg;
				//carg.SetTriggerFlags(TriggerCastFlags::TRIGGERED_FULL_MASK);
				//cunit->CastSpell(hunit, boss_spells::SPELL_BLAZING_DECOMPOSITION, carg);
				Aura* dec = cunit->AddAura(boss_spells::SPELL_BLAZING_DECOMPOSITION, hunit);
				dec->SetStackAmount(100);

				//cunit->AddAura(boss_spells::SPELL_BLAZING_DECOMPOSITION, hunit);
			}
			
		}
	}

	void Register() override
	{
		OnHit += SpellHitFn(spell_magmatic_eruption::handleHit);
	}
};

class herald_of_destruction_add : public CreatureScript
{
public:
	herald_of_destruction_add() : CreatureScript("herald_of_destruction_add") {}

	struct herald_of_destruction_addAI : public ScriptedAI
	{
		herald_of_destruction_addAI(Creature* creature) : ScriptedAI(creature) { }


        uint32 despawntimer = 12000;
        uint32 _despawntimer = 12000;

		void JustEngagedWith(Unit* who)
		{
			if (me->HasAura(900049))
			{
				return;
			}
			else {
				me->AddAura(900049, me);
			}

		}

        void Reset()
        {
            despawntimer = _despawntimer;
        }

		void UpdateAI(const uint32 uiDiff)
		{
            if (!me->IsInCombat())
            {
                if (uiDiff <= despawntimer)
                {
                    me->DespawnOrUnsummon();
                }
                else {
                    despawntimer -= uiDiff;
                }
            }

			if (!UpdateVictim()) {
				return;
			}



			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const override
	{
		return new herald_of_destruction_addAI(creature);
	}
};


class herald_of_destruction : public CreatureScript
{
public:
	herald_of_destruction() : CreatureScript("herald_of_destruction") {}



	struct herald_of_destructionAI : public ScriptedAI
	{
		herald_of_destructionAI(Creature* creature) : ScriptedAI(creature) {}

		uint32 phase = 0;

		uint32 base_display_id = 18036;
		uint32 ice_display_id = 21145;

		uint32 intermission_dot_timer = 3000;
		uint32 _intermission_dot_timer = 3000;

		uint32 checktimer = 500;
		uint32 _checktimer = 500;

		uint32 ctimer = 12000;
		uint32 bctimer = 12000;

		uint32 decompTimer = 5000;
		uint32 _decompTimer = 5000;

		uint32 necroticTimer = 9000;
		uint32 _necroticTimer = 9000;

		// P2 Timers
		uint32 summonTimer = 26000;
		uint32 _summonTimer = 26000;


		void Reset()
		{
			me->SetPowerType(Powers::POWER_MANA);
			me->SetPower(me->GetPowerType(), 120000);

			phase = 0;
			
			for (Unit* u : GetNearbyPlayers(this, 40))
			{
				if (u->GetEntry() == 400001)
				{	
					if (Creature* crt = u->ToCreature())
					{
						u->ToCreature()->DespawnOrUnsummon();
					}
				}
			}

			checktimer = _checktimer;
			ctimer = bctimer;
			necroticTimer = _necroticTimer;
			decompTimer = _decompTimer;

			intermission_dot_timer = _intermission_dot_timer;

			summonTimer = _summonTimer;

			me->SetNativeDisplayId(base_display_id);
			me->SetDisplayId(base_display_id);
			me->SetObjectScale(1);

			me->SetHealth(me->GetMaxHealth());
		}

		void TriggerPhaseTwo()
		{
			phase = 2;
			me->Yell("Phase 2 triggered!", Language::LANG_UNIVERSAL);

			//me->SetNativeDisplayId(ice_display_id);
			//me->SetDisplayId(ice_display_id);
			//me->SetObjectScale(0.2);
		}

		void UpdateAI(const uint32 uiDiff)
		{
			if (!me->GetVictim() || !me->IsInCombat())
			{
				// OOC Timers
			}

			if (!UpdateVictim()) {
				return;
			}

			// Intermission Phase
			if (phase == 1)
			{
				// While casting with shield up
				if (me->HasAura(900043) && (me->GetCurrentSpellCastTime(900044) > 0))
				{
					if (intermission_dot_timer <= uiDiff)
					{
						intermission_dot_timer = _intermission_dot_timer;
						for (Unit* u : GetNearbyPlayers(this, 30))
						{
							if (u != me)
							{
								//me->AddAura(boss_spells::SPELL_BLAZING_DECOMPOSITION, u);
								CastSpellExtraArgs carg;
								carg.SetTriggerFlags(TriggerCastFlags::TRIGGERED_FULL_MASK);
								me->CastSpell(u, boss_spells::SPELL_BLAZING_DECOMPOSITION, carg);

							}


						}
						
					}
					else {
						intermission_dot_timer -= uiDiff;
					}
				}

				if (checktimer <= uiDiff)
				{
					// If shield ended but cast is up (destroyed shield)
					if (!me->HasAura(900043) && me->GetCurrentSpellCastTime(900044) > 0)
					{
						me->CastStop();
						// Trigger Phase 2
						TriggerPhaseTwo();

						for (Unit* u : GetNearbyPlayers(this, 30))
						{
							if (u->HasAura(boss_spells::SPELL_BLAZING_DECOMPOSITION))
							{
								u->RemoveAura(boss_spells::SPELL_BLAZING_DECOMPOSITION);
							}
						}
					}

					// If cast ended but shield is up (Immuned or healed through)
					if (me->HasAura(900043) && (me->GetCurrentSpellCastTime(900044) <= 0))
					{
						me->CastStop();
						// Trigger Phase 2
						TriggerPhaseTwo();

						for (Unit* u : GetNearbyPlayers(this, 30))
						{
							if (u->HasAura(boss_spells::SPELL_BLAZING_DECOMPOSITION))
							{
								u->RemoveAura(boss_spells::SPELL_BLAZING_DECOMPOSITION);
							}
						}
					}

					

					// If no aura and no cast time (delay bugged boss)
					if (!me->HasAura(900043) && (me->GetCurrentSpellCastTime(900044) <= 0))
					{
						TriggerPhaseTwo();
					}

					checktimer = _checktimer;
				}
				else {
					checktimer -= uiDiff;
					
				}

				

			}

			// 1st Phase
			if (phase == 0)
			{
				if (me->GetHealthPct() <= 75 && !me->HasAura(900043))
				{
					me->AddAura(900043, me);
					DoCast(900044);

					phase = 1;
					return;
				}

				if (decompTimer <= uiDiff)
				{
					for (Unit* u : GetNearbyPlayers(this, 40))
					{
						//me->AddAura(900042, u);
						CastSpellExtraArgs carg;
						carg.AddSpellBP0(irand(11531, 19051));
						carg.SetTriggerFlags(TriggerCastFlags::TRIGGERED_FULL_MASK);
						me->CastSpell(u, boss_spells::SPELL_BLAZING_DECOMPOSITION, carg);

					}

					decompTimer = _decompTimer;
				}
				else {
					decompTimer -= uiDiff;
				}

				if (ctimer <= uiDiff)
				{
					if (Unit* target = SelectTarget(SELECT_TARGET_MAXTHREAT, 0))
					{
						CastSpellExtraArgs earg;
						earg.SetTriggerFlags(TriggerCastFlags::TRIGGERED_FULL_MASK);
						me->CastSpell(target, SPELL_SHADOW_CLEAVE_PERIODIC, earg);
						ctimer = bctimer;
					}
				}
				else {
					ctimer -= uiDiff;
				}

				if (necroticTimer <= uiDiff)
				{
					if (Unit* target = SelectTarget(SELECT_TARGET_MAXTHREAT, 0))
					{
						CastSpellExtraArgs earg = new CastSpellExtraArgs();
						earg.AddSpellMod(SpellValueMod::SPELLVALUE_BASE_POINT0, 1235);
						earg.AddSpellMod(SpellValueMod::SPELLVALUE_BASE_POINT1, 691);

						DoCast(target, SPELL_NECROTIC_STRIKE, earg);
						necroticTimer = _necroticTimer;
					}

				}
				else {
					necroticTimer -= uiDiff;
				}


				DoMeleeAttackIfReady();
			}

			// 2nd Phase
			if (phase == 2)
			{
				if (summonTimer <= uiDiff)
				{
					me->SummonCreature(400001, me->GetPositionWithOffset(Position(2.5F, 0.1F, 1.5F, 0.0F)), TempSummonType::TEMPSUMMON_CORPSE_DESPAWN);

					summonTimer = _summonTimer;
				}
				else {
					summonTimer -= uiDiff;
				}


				DoMeleeAttackIfReady();
			}
		}

	};

	CreatureAI* GetAI(Creature* creature) const override
	{
		return new herald_of_destructionAI(creature);
	}

};

void AddSC_boss_herald_destr()
{
	new herald_of_destruction();
	new herald_of_destruction_add();
	RegisterSpellScript(spell_magmatic_eruption);
}
