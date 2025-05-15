////////////////////////////////////////////////////////////////////////////////
// File name   : 166.js                                                       //
// Version     : 24.1                                                         //
// Begin       : 2020-11-03                                                   //
// Last Change : 2022-09-05                                                   //
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
 * PF 166: Client
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

function fxf_fn_init166()
{
//alert('JS: init'+oFXP.tr);
}

function fxf_fn_saveElement166(element)
{
//alert('JS: saveElement'+oFXP.tr);
	fxf_fn_ajaxSaveElement(element);

	// Sex has changed - change salutation
	if(element.id == 'Geschlecht')
	{
		var delement=fxf_fn_getField($('Anrede'),true);
		if(delement)
		{
			var sex=parseInt(fxf_fn_getSelectedValue(element).value);
			var salutation=parseInt(fxf_fn_getSelectedValue(delement).value);
			var nsalutation=salutation;
			if(sex == 8)			// male
				nsalutation=311;	// ...Mr.
			else if(sex == 9)		// female
				nsalutation=312;	// ...Mrs.
			else if(sex == 2802)	// diverse
				nsalutation=2803;	// ...Mr./Mrs.
//alert('Sex has changed: sex='+sex+' (salutation='+salutation+', nsalutation='+nsalutation+')');
			if(nsalutation != salutation)
				fxf_fn_changeSelectedValue(delement,nsalutation,true);
		}
	}
	// Salutation has changed - change sex
	else if(element.id == 'Anrede')
	{
		var delement=fxf_fn_getField($('Geschlecht'),true);
		if(delement)
		{
			var salutation=parseInt(fxf_fn_getSelectedValue(element).value);
			var sex=parseInt(fxf_fn_getSelectedValue(delement).value);
			var nsex=sex;
			if(salutation == 310)		// Company
				nsex=0;					// ...-empty-
			else if(salutation == 311)	// Mr.
				nsex=8;					// ...male
			else if(salutation == 312)	// Mrs.
				nsex=9;					// ...female
			else if(salutation == 2803)	// Mr./Mrs.
				nsex=2802;				// ...diverse
//alert('Salutation has changed: salutation='+salutation+' (sex='+sex+', nsex='+nsex+')');
			if(nsex != sex)
				fxf_fn_changeSelectedValue(delement,nsex,true);
		}
	}

	return 0;
}
