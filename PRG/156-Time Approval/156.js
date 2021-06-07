////////////////////////////////////////////////////////////////////////////////
// File name   : 156.js                                                       //
// Version     : 21.1                                                         //
// Begin       : 2020-11-12                                                   //
// Last Change : 2020-11-12                                                   //
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
 * PF 156: Time Approval
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.1
 */

function fxf_fn_init156()
{
//alert('JS: init'+oFXP.tr);
}

function fxf_fn_saveElement156(element)
{
//alert('JS: saveElement'+oFXP.tr);
	if((element.id == 'Zeitspanne_von') || (element.id == 'Zeitspanne_bis') || (element.id == 'Kunde') || (element.id == 'Personen_ID'))
	{
		var sde=fxf_fn_getElement('Zeitspanne_von');
		var ede=fxf_fn_getElement('Zeitspanne_bis');
		var ce=fxf_fn_getElement('Kunde');
		var pe=fxf_fn_getElement('Personen_ID');
		fxf_fn_updateSelectedProjects('Projekt_ID', 0, 1, sde.value, ede.value, fxf_fn_getSelectedValue(ce).value, fxf_fn_getSelectedValue(pe).value, 1, 1);

		return -1;
	}

	return 1;
}
