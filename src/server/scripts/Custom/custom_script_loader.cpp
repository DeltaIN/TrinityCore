/*
 * This file is part of the TrinityCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

// This is where scripts' loading functions should be declared:
void AddSC_boss_jhuun();
void AddSC_jhuun_healer();
void AddSC_jhuun_intermission_add();
void AddSC_spell_items();
void AddSC_boss_manifest();
void AddSC_instance_pos();
void AddSC_boss_herald_destr();
void AddSC_itemset_battlelord_script();
void AddSC_custom_ply_scripts();
void AddSC_custom_command_scripts();

void AddSC_Custom_Instances();

// The name of this function should match:
// void Add${NameOfDirectory}Scripts()
void AddCustomScripts()
{
    AddSC_boss_jhuun();
    AddSC_jhuun_healer();
    AddSC_spell_items();
    AddSC_jhuun_intermission_add();
    AddSC_boss_manifest();
    AddSC_instance_pos();

    AddSC_itemset_battlelord_script();
    AddSC_boss_herald_destr();
    AddSC_custom_ply_scripts();
    AddSC_custom_command_scripts();
    AddSC_Custom_Instances();
}
