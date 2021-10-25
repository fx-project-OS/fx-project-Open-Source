////////////////////////////////////////////////////////////////////////////////
// File name   : 056.js                                                       //
// Version     : 21.2                                                         //
// Begin       : 2020-10-15                                                   //
// Last Change : 2020-10-15                                                   //
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
 * PF 56: Profile Program Assignment - JavaScript function collection
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

function fxf_fn_init56()
{
//alert('Init Start: '+oFXP.tr);
}

function fxf_fn_cbToggleDetail(ln,detail)
{
//alert('fxf_fn_cbToggleDetail(ln='+ln+',detail='+detail+')');

	// Change click icon
	var sia=$$('[id^="shrcb_"]');
	if(sia.length)
	{
		var ic=0;
		var cc=0;
		for(var i=0; i<sia.length; i++)
		{
			ic++;
			var il=parseInt(sia[i].id.substr(6));
			if((ln < 0) || (il == ln))
			{
				var ei=$('enlcb_'+il);
				if(detail)
				{
					ei.style.display='none';
					sia[i].style.display='block';
				}
				else
				{
					ei.style.display='block';
					sia[i].style.display='none';
				}
			}
			if(sia[i].style.display != 'none')
				cc++;
		}

		var aei=$$('[id="aenlcb"]');
		for(i=0; i<aei.length; i++)
		{
			if(cc >= ic/2)
				aei[i].style.display='none';
			else
				aei[i].style.display='block';
		}

		var asi=$$('[id="ashrcb"]');
		for(i=0; i<asi.length; i++)
		{
			if(cc >= ic/2)
				asi[i].style.display='block';
			else
				asi[i].style.display='none';
		}
//alert('ic='+ic+', cc='+cc);
	}

	// Toggle checkboxes
	var dcbs=$$('[id^="detcb_"]');
	if(dcbs.length)
	{
		for(var c=0; c<dcbs.length; c++)
		{
			var io=dcbs[c].id.indexOf('[');
			var pid=dcbs[c].id.substr(6,io-6);
			var mc=dcbs[c].id.substr(io+1);
			var io=mc.indexOf('[');
			var cl=parseInt(mc.substr(io+1,mc.length-io-2));
			var mc=parseInt(mc.substr(0,io-1));
			if((ln < 0) || (cl == ln))
			{
				var cs='';
				var cb0=$('cb0_'+pid+'['+mc+']['+cl+']');
				if(cb0.checked)
					cs='1111';
				else
				{
					var cb1=$('cb1_'+pid+'['+mc+']['+cl+']');
					if(cb1 && cb1.checked)
						cs += '1';
					else
						cs += '0';
					var cb2=$('cb2_'+pid+'['+mc+']['+cl+']');
					if(cb2 && cb2.checked)
						cs += '1';
					else
						cs += '0';
					var cb3=$('cb3_'+pid+'['+mc+']['+cl+']');
					if(cb3 && cb3.checked)
						cs += '1';
					else
						cs += '0';
					var cb4=$('cb4_'+pid+'['+mc+']['+cl+']');
					if(cb4 && cb4.checked)
						cs += '1';
					else
						cs += '0';
				}
				if((cs == '0000') || (cs == '1111'))
				{
					var acb=$('allcb_'+pid+'['+mc+']['+cl+']');
					if(detail)
					{
						acb.style.display='none';
						dcbs[c].style.display='block';
					}
					else
					{
						acb.style.display='block';
						dcbs[c].style.display='none';
					}
				}
//alert('pid='+pid+', mc='+mc+', cl='+cl+' -- cs='+cs);
			}
		}
	}
}

function fxf_fn_cbClick(ci,pid,mc,ln,set)
{
//alert('fxf_fn_cbClick(ci='+ci+',pid='+pid+',mc='+mc+',ln='+ln+',set='+set+')');
	var frm='';
	var ccs='';
	var cb0s=$$('[id^="cb0_"]');
	if(cb0s.length)
	{
		for(var c=0; c<cb0s.length; c++)
		{
			if(!frm.length)
				frm=cb0s[c].attributes['fxform'].value;
			var io=cb0s[c].id.indexOf('[');
			var cpid=cb0s[c].id.substr(4,io-4);
			var cmc=cb0s[c].id.substr(io+1);
			var io=cmc.indexOf('[');
			var cln=parseInt(cmc.substr(io+1,cmc.length-io-2));
			var cmc=parseInt(cmc.substr(0,io-1));
			if((!pid.length || (cpid == pid)) && ((mc < 0) || (cmc == mc)) && ((ln < 0) || (cln == ln)))
			{
				if(!ci)
				{
					if(set > 0)
						cb0s[c].checked=true;
					else if(!set)
						cb0s[c].checked=false;
					if(ccs.length)
						ccs += '&';
					ccs += cb0s[c].name+'=cb';
					if(cb0s[c].checked)
						ccs += '1';
					for(var ac=1; ac<5; ac++)
					{
						var acb=$('cb'+ac+'_'+cpid+'['+cmc+']['+cln+']');
						if(acb)
						{
							acb.checked=cb0s[c].checked;
							if(ccs.length)
								ccs += '&';
							ccs += acb.name+'=cb';
							if(acb.checked)
								ccs += '1';
						}
					}
				}
				else
				{
					var cs='';
					for(var ac=1; ac<5; ac++)
					{
						var acb=$('cb'+ac+'_'+cpid+'['+cmc+']['+cln+']');
						if(acb)
						{
							if(set > 0)
								acb.checked=true;
							else if(!set)
								acb.checked=false;
						}
						if(!acb || (acb && acb.checked))
							cs += '1';
						else
							cs += '0';
						if(acb)
						{
							if(ccs.length)
								ccs += '&';
							ccs += acb.name+'=cb';
							if(acb.checked)
								ccs += '1';
						}
					}
					if(cs == '1111')
						cb0s[c].checked=true;
					else
						cb0s[c].checked=false;
					if(ccs.length)
						ccs += '&';
					ccs += cb0s[c].name+'=cb';
					if(cb0s[c].checked)
						ccs += '1';
				}
			}
		}
	}

	// Save checkboxes
	if(ccs.length)
	{
//alert('frm='+frm+'\nccs='+ccs);
		fxf_fn_saveFields(ccs, 'frm='+frm);
	}
}