#include <iostream>
#include "ScriptPCH.h"

#define POWERTYPE POWER_MANA


enum timers
{
    voidbolt_base_timer = 7000,
    voidbolt_alt_base = 16000,
    voidstrike_base_timer = 22000,
    voidadd_base_timer = 5000,
    BaseAttTime = 2200,
    AttDecrement = 14,
    HealerDespawnBase = 10000,
    ManifestationDespawnBase = 6000,
    IntermissionDespawnBase = 6000,
    sfuryBaseTimer = 22000,
    fearBaseTimer = 30000,
    boltBaseTimer = 4000,
    zoneBaseTimer = 11000
};
enum misc
{
	MECHANIC_ENTRY_MANIFEST = 70032, // manifestation entry id
	MECHANIC_ENTRY_INTERMISSION = 70033, // intermission add entry id
	MECHANIC_ENTRY_HEALER = 70031, // 50% hp healers entry id
    MECHANIC_ENTRY_VZONE = 70034, // void zone entry id
    MECHANIC_ENTRY_MAIN = 70030, // voidmaster entry id
    INSTANCE_PLAYER_HEALTHMULT = 5 // Health multiplier (GetMaxHealth() * INSTANCE_PLAYER_HEALTHMULT)
};
enum spells
{
	SPELL_VOIDBOLT = 39329, // top threat
	SPELL_VOIDBOLT_NOINT = 33047, // random target
	SPELL_VOIDSTRIKE = 52633, // top threat
    SPELL_HEAL = 900001,
    SPELL_CBOLT = 71108,
    SPELL_FEAR = 65809,
    SPELL_SPEEDBUFF = 900005,
    SPELL_SHADOWFURY = 45270
};


void CustomCast(Unit* source, Unit* target, uint32 spellId, uint32 bp0Mod = 0, TriggerCastFlags trigger = TRIGGERED_NONE)
{
    CastSpellExtraArgs eArg;
    eArg.AddSpellMod(SPELLVALUE_BASE_POINT0, bp0Mod);
    eArg.SetTriggerFlags(trigger);

    source->CastSpell(target, spellId, eArg);
}
uint32 GetNearbyPlayerCount(UnitAI* source, uint32 scanCount = 8)
{
    uint32 plCount = 0;

    for (uint32 scanner = 0; scanner < scanCount; scanner++)
    {
        if (Unit* validate_target = source->SelectTarget(SELECT_TARGET_MINDISTANCE, scanner))
        {
            plCount += 1;
        }
    }

    return plCount;
}


class boss_jhuun : public CreatureScript
{
public:
	boss_jhuun() : CreatureScript("boss_jhuun") {}

	struct boss_jhuunAI : public ScriptedAI
	{
		boss_jhuunAI(Creature* creature) : ScriptedAI(creature) {}
        Map* parentMap = me->GetMap();


        

        uint32 voidbolt_timer;
        uint32 voidbolt_alt_timer;
        uint32 voidstrike_timer;
        uint32 voidadd_timer;
		uint32 curPhase = 0;
        uint32 AttTime;
		bool hasRunSummon = false;

        uint32 CalculateStat(Map* temp_map)
        {
            return ( temp_map->GetPlayers().getSize() ) * me->GetMaxHealth();
        }

        bool ValidateStat(Unit* crtur, Map* curmap)
        {
            if (crtur->GetMaxHealth() != CalculateStat(curmap))
            {
                return false;
            }
            else {
                return true;
            }
        }

        void DoScaleStat()
        {
            uint32 newHealth = CalculateStat(parentMap);
            me->SetMaxHealth(newHealth);
        }

        void JustAppeared()
        {
            DoScaleStat();
            me->SetPowerType(POWERTYPE);
            me->SetPower(me->GetPowerType(), me->GetMaxPower(me->GetPowerType()));
        }

		void Reset()
		{
            DoScaleStat();
			me->SetPowerType(POWERTYPE);
			hasRunSummon = false;
			curPhase = 1;
			me->SetPower(me->GetPowerType(), 0);
            AttTime = BaseAttTime;
            voidbolt_timer = voidbolt_base_timer;
            voidbolt_alt_timer = voidbolt_alt_base;
            voidstrike_timer = voidstrike_base_timer;
            voidadd_timer = voidadd_base_timer;
		}

		void EnterCombat(Unit* who)
		{
            DoScaleStat();
            me->Talk("Entered combat.", CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, 533, me);
            curPhase = 1;
			me->SetPowerType(POWER_MANA);
			me->SetPower(me->GetPowerType(), me->GetMaxPower(me->GetPowerType()));
		
		};

		//void AttackStart(Unit* who) override
		//{
           // AttackStart(who);
		//}

        

		void JustDied(Unit* killer)
		{
			me->SummonCreature(MECHANIC_ENTRY_MANIFEST, me->GetPosition(), TEMPSUMMON_CORPSE_DESPAWN);
		}

        void SpellHit(Unit* caster, SpellInfo const* spell)
        {
            if (spell->Id == 900003)
            {
                me->KillSelf();
            }
        }

		void UpdateAI(const uint32 diff)
		{
            if (!ValidateStat(me, parentMap))
            {
                DoScaleStat();
            }

			if (!me->GetVictim())
			{
				// if no target - OOC Timers
			}

			if (!UpdateVictim()) {
				return; // If no target
			}

			if (me->GetHealthPct() <= 5)
			{
				// intermission phase
				curPhase = 2;
			}

			// Basic spell timers in phase 0
			if (curPhase == 1)
			{
                DoMeleeAttackIfReady();
                
                
                
                


				if (voidbolt_timer <= diff)
				{
                    if (Unit* target = me->GetVictim())
                    {
                        
                        DoCastVictim(SPELL_VOIDBOLT);
                       
                        voidbolt_timer = 7000;
                    }
                    
				}
				else {
					voidbolt_timer -= diff;
				}

				if (voidbolt_alt_timer <= diff)
				{
                    if (Unit* target = me->GetVictim())
                    {
                        

                        DoCast(SelectTarget(SELECT_TARGET_RANDOM, 0), SPELL_VOIDBOLT_NOINT);

                        voidbolt_alt_timer = 16000;
                    }
					
					
				}
				else {
					voidbolt_alt_timer -= diff;
				}

				if (voidstrike_timer <= diff)
				{
                    if (Unit* target = me->GetVictim())
                    {
                        DoCastVictim(SPELL_VOIDSTRIKE, TRIGGERED_CAST_DIRECTLY);
                       
                        voidstrike_timer = 22000;
                    }
					
				}
				else {
					voidstrike_timer -= diff;
				}

				// Health mechanics
				if (me->GetHealthPct() <= 50)
				{
					if (hasRunSummon == false)
					{
						Position Spawn_A = { 341.4f, -31.5f, -2.3f };
						Position Spawn_B = { 329.3f, 28.9f, -2.3f };
						me->SummonCreature(MECHANIC_ENTRY_HEALER, Spawn_A, TEMPSUMMON_CORPSE_DESPAWN);
                        me->SummonCreature(MECHANIC_ENTRY_HEALER, Spawn_B, TEMPSUMMON_CORPSE_DESPAWN);
						hasRunSummon = true;
					}
				}
			}
			else if (curPhase == 2)
			{
				// Intermission
                me->StopMoving();
                if (!me->GetCurrentSpellCastTime(900003) > 0)
                {
                    me->AddAura(63710, me);
                    me->StopMoving();
                    DoCast(900003);
                }
				

				// Summon timer
				if (voidadd_timer <= diff)
				{
					me->SummonCreature(MECHANIC_ENTRY_INTERMISSION, SelectTarget(SELECT_TARGET_RANDOM, 0)->GetPosition(), TEMPSUMMON_CORPSE_DESPAWN);
                    voidadd_timer = voidadd_base_timer;
				}
				else {
					voidadd_timer -= diff;
				}

                
			}

            
		}



	};

	CreatureAI* GetAI(Creature* creature) const override
	{
		return new boss_jhuunAI(creature);
	}


	private:
};


class jhuun_intermission_add : public CreatureScript
{
public:
    jhuun_intermission_add() : CreatureScript("jhuun_intermission_add") { }

    struct jhuun_intermission_addAI : public ScriptedAI
    {
        jhuun_intermission_addAI(Creature* creature) : ScriptedAI(creature) {}

        uint32 despawntimer;
        uint32 cboltTimer = 4000;

        void JustAppeared()
        {
            despawntimer = IntermissionDespawnBase;
            me->m_CombatDistance = 5000;
            cboltTimer = 4000;
        }

        void Reset()
        {
            despawntimer = IntermissionDespawnBase;
            cboltTimer = 4000;
        }


        void UpdateAI(const uint32 diff)
        {
            
            if (!me->IsInCombat())
            {
                // ooc
                if (despawntimer <= diff)
                {
                    me->KillSelf();
                }
                else {
                    despawntimer -= diff;
                }


                return;
            }

            if (cboltTimer <= diff)
            {
                DoCastVictim(SPELL_CBOLT);
                
            }
            else {
                cboltTimer -= diff;
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new jhuun_intermission_addAI(creature);
    }
};

class boss_manifest : public CreatureScript
{
public:
    boss_manifest() : CreatureScript("boss_manifest") {}

    struct boss_manifestAI : public ScriptedAI
    {
        boss_manifestAI(Creature* creature) : ScriptedAI(creature) {}

        uint32 despawntimer;
        uint32 sfuryTimer;
        uint32 fearTimer;
        uint32 boltTimer;
        uint32 zoneTimer;
        bool hasSummoned;
        bool hasBarriered;
        uint32 z_phase;

        void ReInit()
        {
            fearTimer = fearBaseTimer;
            sfuryTimer = sfuryBaseTimer;
            boltTimer = boltBaseTimer;
            zoneTimer = zoneBaseTimer;
            despawntimer = ManifestationDespawnBase;
            hasSummoned = false;
            hasBarriered = false;
            z_phase = 0;
        }

        void JustAppeared()
        {
            ReInit();
            despawntimer = ManifestationDespawnBase;
            //me->m_CombatDistance = 5000;
        }

        void Reset()
        {
            ReInit();
            despawntimer = ManifestationDespawnBase;
        }

        void EnterCombat(Unit* who)
        {
            if (me->GetVictim() == who)
            {
                me->Attack(who, true);
            }
        }

        void UpdateAI(const uint32 diff)
        {


            
            
            if (!me->IsInCombat())
            {
                if (despawntimer <= diff)
                {
                    me->KillSelf();
                    me->SummonCreature(70030, me->GetPosition(), TEMPSUMMON_CORPSE_DESPAWN);
                    me->DisappearAndDie();
                }
                else {
                    despawntimer -= diff;
                }
            }

            if (!UpdateVictim()) {
                return; // If no target
            }

            if (z_phase == 1)
            {
                if (!hasBarriered)
                {
                    DoCast(me, 900006, TRIGGERED_CAST_DIRECTLY);
                    DoCast(900007);
                    hasBarriered = true;
                }
                return; // prevent cast overlap
            }

            // PHASE 0 MECHANICS
            DoMeleeAttackIfReady();
            // FEAR
            if (fearTimer <= diff)
            {
                DoCast(SelectTarget(SELECT_TARGET_RANDOM, 0), SPELL_FEAR);
                fearTimer = fearBaseTimer;
            }
            else {
                fearTimer -= diff;
            }

            // SHADOWFURY
            if (sfuryTimer <= diff)
            {
                DoCastAOE(SPELL_SHADOWFURY);
                sfuryTimer = sfuryBaseTimer;
            }
            else {
                sfuryTimer -= diff;
            }

            // CHAOS BOLT
            if (boltTimer <= diff)
            {
                DoCastVictim(SPELL_CBOLT);
                boltTimer = boltBaseTimer;
            }
            else {
                boltTimer -= diff;
            }

            // VOID ZONE
            if (zoneTimer <= diff)
            {
                //me->SummonCreature(MECHANIC_ENTRY_VZONE , SelectTarget(SELECT_TARGET_RANDOM, 0)->GetPosition(), TEMPSUMMON_CORPSE_DESPAWN);
                zoneTimer = zoneBaseTimer;
            }
            else {
                zoneTimer -= diff;
            }

            // HEALER
            if (me->GetHealthPct() <= 50)
            {
                if (!hasSummoned)
                {
                    // SUMMON (remember to script healer add)
                    Position HealerPos = {390.18f, -34.08f, 20.18f, 2.61f};
                    me->SummonCreature(MECHANIC_ENTRY_HEALER, HealerPos, TEMPSUMMON_CORPSE_DESPAWN);

                    // SPEED
                    Unit* speedTarget = SelectTarget(SELECT_TARGET_RANDOM, 0);
                    me->AddAura(SPELL_SPEEDBUFF, speedTarget);

                    hasSummoned = true;
                }
            }

            if (me->GetHealthPct() <= 30)
            {
                z_phase = 1;
            }

        }


    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_manifestAI(creature);
    }
};

class jhuun_healer : public CreatureScript
{
public:
    jhuun_healer() : CreatureScript("jhuun_healer") {}

    struct jhuun_healerAI : public ScriptedAI
    {
        jhuun_healerAI(Creature* creature) : ScriptedAI(creature) {}

        uint32 despawntimer;

        uint32 numPlayers;
        uint32 bp0_add_base;
        uint32 bp0_plScale;
        uint32 bp0_calc = 0;

        uint32 bp0_worldScale;


        void JustAppeared()
        {
            despawntimer = HealerDespawnBase;
            me->m_CombatDistance = 5000;
            me->m_SightDistance = 5000;
            CombatManager& cbM = me->GetCombatManager();
            cbM.SetInCombatWith(me->SelectNearestPlayer(53003));
        }

        void Reset()
        {
            despawntimer = HealerDespawnBase;
        }

        void UpdateAI(const uint32 diff)
        {
            if (!me->IsInCombat())
            {
                // if ooc
                if (despawntimer <= diff)
                {
                    me->KillSelf();
                }
                else {
                    despawntimer -= diff;
                }
            }


            if (!me->GetCurrentSpellCastTime(SPELL_HEAL))
            {
                if (Unit* target = me->FindNearestCreature(MECHANIC_ENTRY_MAIN, 533, true))
                {
                    //DoCast(target, SPELL_HEAL);
                    CustomCast(me, target, SPELL_HEAL, target->GetMaxHealth() / 20);
                }
                else if (Unit* targetManifest = me->FindNearestCreature(70032, 6000, true))
                {
                    //DoCast(targetManifest, SPELL_HEAL);
                    CustomCast(me, target, SPELL_HEAL, target->GetMaxHealth() / 15);
                }
                
            }

        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new jhuun_healerAI(creature);
    }
};

void AddSC_boss_jhuun()
{
	new boss_jhuun();
}

void AddSC_jhuun_intermission_add()
{
    new jhuun_intermission_add();
}

void AddSC_jhuun_healer()
{
    new jhuun_healer();
}
void AddSC_boss_manifest()
{
    new boss_manifest();
}
