////////////////////////////////////////////////////////////////////////////////
// File name   : 058.js                                                       //
// Version     : 24.1                                                         //
// Begin       : 2025-01-29                                                   //
// Last Change : 2025-01-30                                                   //
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
 * PF 58: Reference Values - JavaScript function collection
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

var ls58='';
var sid=['63', '75', '1054'];

function fxf_fn_init58()
{
//alert('Init Start: '+oFXP.tr);
}

function fxf_fn_saveElement58(element)
{
	var e3=element.id.substr(0,3);
	if(e3 == 'fcx')
	{
		var svt=fxf_fn_getSelectedValue(element);
//alert('JS: fxf_fn_saveElement'+oFXP.tr+'\nelement.id='+element.id+'\n\ne3='+e3+'\nval='+svt.value+', txt='+svt.text);
		fxf_fn_closeElement58();

		var lss=ls58.split('_');
		var eid='CodeWert['+lss[0]+']['+lss[1]+']';
		var iel=$(eid);
		var ovl=iel.value;
		iel.value=svt.value;
		var stx=eid+'=tx'+svt.value;

		var etd='TabWert['+lss[0]+']['+lss[1]+']';
		var tel=$(etd);
		var tvl=tel.value;
		if(!tvl.length)
		{
			tel.value=svt.text;
			stx += '&'+etd+'=tx'+svt.text;
		}

		if(svt.value != ovl)
		{
			var frm=iel.attributes['fxform'].value;
//alert('fxf_fn_saveFields('+stx+', frm='+frm+')');
			fxf_fn_saveFields(stx, 'frm='+frm);
			return 0;
		}
		return 1;
	}
	return 1;
}

function fxf_fn_openElement58(mc, i, id, l)
{
	fxf_fn_closeElement58();
	ls58=mc+'_'+i+'_'+id;
//alert('JS: fxf_fn_openElement'+oFXP.tr+'\nmc='+mc+', i='+i+', id='+id+'\n-> ls58='+ls58);

	var did=$('sel'+id);
	if(did)
	{
		var eid='CodeWert['+mc+']['+i+']';
		var iel=$(eid);
		var val=iel.value;
//alert('val='+val);

//		fxf_fn_changeSelectedValue(spe,val,false);

		var iel=$('td_icon['+mc+']['+i+']');
		var ied=fxf_fn_getBCR(iel);
//alert('ied:\n\nleft='+ied.left+', right='+ied.right+', top='+ied.top+', bottom='+ied.bottom+', width='+ied.width+', height='+ied.height+'\n\ndim:\n\n.sd: s='+dim.sd.swidth+'x'+dim.sd.sheight+', a='+dim.sd.awidth+'x'+dim.sd.aheight+', p='+dim.sd.pwidth+'x'+dim.sd.pheight+'\n\n.wd: left='+dim.wd.left+', top='+dim.wd.top+', right='+dim.wd.right+', bottom='+dim.wd.bottom+' -- width='+dim.wd.width+', height='+dim.wd.height);

		l=parseInt(l);

		did.style.left=(ied.right - dim.wd.left - 9 + oID.fxwork.scrollLeft + l)+'px';
		did.style.top=(ied.top - dim.wd.top - 28 + oID.fxwork.scrollTop)+'px';
		did.style.display='block';
	}
}

function fxf_fn_closeElement58()
{
//alert('JS: fxf_fn_closeElement'+oFXP.tr);
	// 63=Länderkennzeichen, 75=Währung + 1054=Verkaufseinheit
	for(var i=0; i<sid.length; i++)
	{
		var did=$('sel'+sid[i]);
		if(did)
			did.style.display='none';
	}
}
