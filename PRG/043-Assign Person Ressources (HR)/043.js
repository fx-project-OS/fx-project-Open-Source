////////////////////////////////////////////////////////////////////////////////
// File name   : 043.js                                                       //
// Version     : 21.2                                                         //
// Begin       : 2020-11-03                                                   //
// Last Change : 2020-11-03                                                   //
// Author      : FeRox Management Consulting GmbH & Co. KG                    //
//               Adolf-Langer-Weg 11a, D-94036 Passau (Germany)               //
//               https://www.ferox.de - info@ferox.de                         //
// License     : GNU-GPL v3 (https://opensource.org/licenses/GPL-3.0)         //
// -------------------------------------------------------------------------- //
// fx-project - An open source PHP Project Managament Software                //
// Copyright  © FeRox Management Consulting GmbH & Co. KG                     //
// -------------------------------------------------------------------------- //
// This program is free software: you can redistribute it and/or modify       //
// it under the terms of the GNU General Public License as published by       //
// the Free Software Foundation, either version 3 of the License, or          //
// (at your option) any later version.                                        //
//                                                                            //
// This program is distributed in the hope that it will be useful,            //
// but WITHOUT ANY WARRANTY; without even the implied warranty of             //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              //
// GNU General Public License for more details.                               //
//                                                                            //
// You should have received a copy of the GNU General Public License          //
// along with this program.  If not, see <https://www.gnu.org/licenses/>.     //
//                                                                            //
// See ../LICENSE.TXT file for more information.                              //
// -------------------------------------------------------------------------- //
// LICENSING ADDENDUM:                                                        //
// Programs in the SPP (Special Programs) subfolder are coded extensions of   //
// the open source software fx-project. These programs are offered for sale   //
// by the manufacturer FeRox Management Consulting GmbH & Co. KG and require  //
// a valid key for execution. It is forbidden to resell these programs        //
// and/or keys or to pass them on free of charge or use them without the      //
// express written permission of FeRox Management Consulting GmbH & Co. KG.   //
////////////////////////////////////////////////////////////////////////////////

/**
 * @file
 * PF 43: Assign Person Ressources
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

function fxf_fn_init43()
{
//alert('Init Start: '+oFXP.tr);
	tSet.task_skills_array	= [];
	tSet.call_counter		= 0;
	tSet.sel_persid			= 0;
	tSet.ntasks				= 0;
	tSet.oassigned			= '';

	var id_task_skills=fxf_fn_getElement('task_skills');
	if(id_task_skills)
	{
		var ta=id_task_skills.innerHTML.split(',');
		if(ta && ta.length)
		{
			var tc=0;
			var i=0;
			while(i<ta.length)
			{
				tSet.task_skills_array[tc] = {'project':ta[i], 'skillgroup':ta[i+1], 'skill':ta[i+2]};
				tc++;
				i += 3;
			}

			var id_first_assign=fxf_fn_getElement('first_assign');
			if(id_first_assign)
				fxf_fn_updateSkills();
			else
				tSet.call_counter=1;
		}
	}

	var id_ntasks=fxf_fn_getElement('ntasks');
	if(id_ntasks)
	{
		tSet.ntasks=parseInt(id_ntasks.innerHTML);
		if(tSet.ntasks > 1)
			tSet.oassigned=fxf_fn_getAssignedResources();
	}
//alert('tSet.task_skills_array='+tSet.task_skills_array+', tSet.call_counter='+tSet.call_counter+', tSet.sel_persid='+tSet.sel_persid+', tSet.ntasks='+tSet.ntasks+', tSet.oassigned='+tSet.ntasks);
}

function fxf_fn_saveElement43(element)
{
//alert('JS: saveElement'+oFXP.tr);
	fxf_fn_ajaxSaveElement(element);

	if(element.id == 'Projekt_ID')
		fxf_fn_updateSkills.delay(oFXP.ldelay);

	return 0;
}
