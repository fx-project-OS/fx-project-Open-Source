////////////////////////////////////////////////////////////////////////////////
// File name   : 268.js                                                       //
// Version     : 21.2                                                         //
// Begin       : 2020-03-31                                                   //
// Last Change : 2021-10-06                                                   //
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
 * PF 268: Hourly Rates - JavaScript function collection
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

function fxf_fn_init268()
{
//alert('Init Start: '+oFXP.tr);
	var cne=fxf_fn_getElement('Kategorie');
	if(cne)
		fxf_fn_disableSkill(cne);
}

function fxf_fn_saveElement268(element)
{
//alert('JS: saveElement'+oFXP.tr);
	fxf_fn_ajaxSaveElement(element);

	if(element.id == 'Kategorie')
		fxf_fn_updatePriceCategory(element);

	return 0;
}

function fxf_fn_disableSkill(element)
{
	// Disable select if skill selected
	var sne=fxf_fn_getElement('MaArt_ID');
	if(sne)
	{
		var spe=sne.up(3);
		if(spe)
		{
			if(parseInt(fxf_fn_getSelectedValue(element).value) == 375)
				spe.style.display='none';
			else
				spe.style.display='';
		}
	}
}

function fxf_fn_updatePriceCategory(element)
{
	var selected=fxf_fn_getSelectedValue(element);

	// Change select
	var sne=fxf_fn_getElement('AuswahlFeldtyp');
	if(sne)
	{
		var url=fxf_fn_gProgram('ekvk_select', 'kat='+parseInt(selected.value)+fxf_fn_gParam());
//alert('url='+url);
		new Ajax.Request(url,
		{
			method:'get', asynchronous:true,
			onSuccess: function(transport)
			{
				fxf_fn_setNewSelects(sne,transport.responseText,true);
				fxf_fn_ajaxSaveElement.delay(oFXP.ldelay, sne);
			}
		});
	}

	// Change literal
	literal_id_field=$('catlit');
	if(literal_id_field)
		literal_id_field.innerHTML=selected.text;

	// Disable select if skill selected
	fxf_fn_disableSkill(element);
}