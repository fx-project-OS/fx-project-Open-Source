////////////////////////////////////////////////////////////////////////////////
// File name   : 007.js                                                       //
// Version     : 21.2                                                         //
// Begin       : 2020-10-21                                                   //
// Last Change : 2021-04-08                                                   //
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
 * PF 7: Employee's Contract Data - JavaScript function collection
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

function fxf_fn_init7()
{
//alert('Init Start: '+oFXP.tr);
	fxf_fn_updateContractData(null);
}

function fxf_fn_updateContractData(element)
{
	var ei='';
	if(element)
		ei=element.id;

	// Working Hours/Week
	var id_whw=fxf_fn_getElement('WochenArbeitStd');
	if(!id_whw)
		return;
	var whw=fxf_fn_string2float(id_whw.value);
	if(whw < 0.0)
		whw=whw*-1.0;
	id_whw.value=fxf_fn_float2string(whw);
	id_whw.fxv=id_whw.value;

	// Count and update workdays
	var wdc=0;
	var id_wdc=$('td_Arbeitstage');
	if(id_wdc)
	{
		for(var w=29; w<36; w++)
		{
			var idw=$(fxf_fn_getText(w));
			if(idw)
			{
				if(idw.checked || ((idw.type == 'hidden') && (idw.value == '1')))
					wdc++;
			}
		}
		id_wdc.innerHTML=wdc;
	}

	// Working Start Time
	var id_wst=fxf_fn_getElement('Arbeitsbeginn');
	var wst=fxf_fn_time2sec(id_wst.value);
	id_wst.value=fxf_fn_sec2time(wst);
	id_wst.fxv=id_wst.value;

	// Break
	var id_brk=fxf_fn_getElement('Pause');
	var brk=fxf_fn_timespan2sec(id_brk.value, 'Pause', true);
	if(brk < 0.0)
		brk=brk*-1.0;
	id_brk.value=fxf_fn_sec2timespan(brk);
	id_brk.fxv=id_brk.value;

	// Info
	// ...Working Hours/Day
	var id_whd=$('arbstdtag');
	var whd=0.0;
	if((whw > 0.0) && (wdc > 0))
		whd=Math.round(whw*100/wdc)/100;
	id_whd.innerHTML=fxf_fn_float2string(whd);
	// ...Working Time
	var id_wkt=$('arbzeit');
	var wet=Math.round(wst+brk+whd*3600);
	var wkt=fxf_fn_sec2time(wst);
	if((wet != wst) && (whd > 0.0))
		wkt=wkt+'-'+fxf_fn_sec2time(wet);
	id_wkt.innerHTML=wkt;
//alert('fxf_fn_updateContractData\nelement='+element+', ei='+ei+'\n\nwhw='+whw+' ('+fxf_fn_getType(whw)+'), wdc='+wdc+' ('+fxf_fn_getType(wdc)+'), wst='+wst+' ('+fxf_fn_getType(wst)+'), brk='+brk+' ('+fxf_fn_getType(brk)+')\n-> whd='+whd+' ('+fxf_fn_getType(whd)+'), wet='+wet+' ('+fxf_fn_getType(wet)+')\n\n-> wkt='+wkt);

	if(element)
		fxf_fn_saveElement(element);
}