////////////////////////////////////////////////////////////////////////////////
// File name   : 218.js                                                       //
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
 * PF 218: Weekly Hours and Overtimes
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

function fxf_fn_init218()
{
//alert('JS: init'+oFXP.tr);
}

function fxf_fn_saveElement218(element)
{
//alert('JS: saveElement'+oFXP.tr);
	fxf_fn_ajaxSaveElement(element);

	var ei4=element.id.substr(0,4);
	if((ei4 == 'Mona') || (ei4 == 'Jahr'))
		fxf_fn_recalcCarry(element);

	return 0;
}

function fxf_fn_recalcCarry(element)
{
	fxf_fn_waitScreen('#');
	var eid=element.id;
	var eind='';
	var ipos=eid.indexOf('[');
	if(ipos >= 0)
	{
		eind=eid.substr(ipos);
		eid=eid.substr(0,ipos);
	}
//alert('recalcCarry: Changed element='+element.id+' -- eid:'+eid+', eind:'+eind+', fxv='+element.fxv);

	var src=fxf_fn_getElement('hcalc'+eind);
	var pid=parseInt(fxf_fn_getElement('Personen_ID'+eind).value);
	var year=parseInt(fxf_fn_getElement('Jahr'+eind).value);
	var month=parseInt(fxf_fn_getSelectedValue(fxf_fn_getElement('Monat'+eind)).value)-17;
//alert('pid='+pid+', year='+year+', month='+month);

	var url=fxf_fn_gProgram('recalc_carry', 'pid='+pid+'&year='+year+'&month='+month+fxf_fn_gParam());
	new Ajax.Updater(src, url,
	{
		method:'get',
		onSuccess:function(transport)
		{
			src.value=transport.responseText;
			src.fxv=src.value;
			fxf_fn_waitScreen('');
		}
	});
}

function fxf_fn_copyCarry(element)
{
	var eid=element.id;
	var eind='';
	var ipos=eid.indexOf('[');
	if(ipos >= 0)
	{
		eind=eid.substr(ipos);
		ename=eid.substr(0, ipos);
	}
//alert('copyCarry: Clicked element='+element.id+' -- eid:'+eid+', eind:'+eind);

	var src=fxf_fn_getElement('hcalc'+eind);
	var dst=fxf_fn_getElement('Ueberstunden_rest'+eind);
	if(src && dst)
	{
		dst.value=src.value;
		dst.fxv=dst.value;
		fxf_fn_saveElement.delay(oFXP.sdelay, dst);
	}
}
