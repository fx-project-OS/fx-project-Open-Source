////////////////////////////////////////////////////////////////////////////////
// File name   : 040.js                                                       //
// Version     : 24.1                                                         //
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
 * PF 40: Design
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

function fxf_fn_init40()
{
//alert('JS: init'+oFXP.tr);
}

function fxf_fn_saveElement40(element)
{
	var e4=element.id.substr(0,4);
	var val='';
	if(e4 == 'Feld')
		val=fxf_fn_getSelectedValue(element).value;
	else if((e4 == 'set_') || (e4 == 'mset') || (e4 == 'msec'))
		val=element.value;
//alert('JS: saveElement'+oFXP.tr+'\nelement.id='+element.id+'\n\ne4='+e4+'\nval='+val);
	if(val.length)
		fxf_fn_designField(element.id,val);

	return 1;
}

function fxf_fn_designChangeColorGradient(id,ndir,event)
{
	ndir=parseInt(ndir);
//alert('fxf_fn_designChangeColorGradient(id='+id+', ndir='+ndir+')');

	// Get direction element and curren direction
	var el=null;
	if(id.length)
		el=$(id+'_dir');
//alert('el='+el);
	if(!el)
		return;

	var cdir=parseInt(el.attributes['cdir'].value);
//alert('cdir='+cdir);

	// Direction selection
	if((ndir <= 0) || (ndir == 5) || (ndir > 9))
	{
//alert('Direction selection');
		var da=[
			{'dir':7, 'arrow':'&nwarr;', 'text':'top left'},
			{'dir':8, 'arrow':'&uarr;', 'text':'top'},
			{'dir':9, 'arrow':'&nearr;', 'text':'top right'},
			{'dir':4, 'arrow':'&larr;', 'text':'left'},
			{'dir':5, 'arrow':'&nbsp;', 'text':'dummy'},
			{'dir':6, 'arrow':'&rarr;', 'text':'right'},
			{'dir':1, 'arrow':'&swarr;', 'text':'bottom left'},
			{'dir':2, 'arrow':'&darr;', 'text':'bottom'},
			{'dir':3, 'arrow':'&searr;', 'text':'bottom right'}
		];
		var data='';
		for(var d=0; d<da.length; d++)
		{
			var bc='fcfcfc';
			var ac='000';
			var cr='pointer';
			var oc=' onmouseover="$(this).style.color=\'#006b9f\';$(this).style.background=\'#b5cfdc\';" onmouseout="$(this).style.color=\'#'+ac+'\';$(this).style.background=\'#'+bc+'\';" onclick="fxf_fn_fxLinkClose();fxf_fn_designChangeColorGradient(\''+id+'\','+da[d].dir+',event);"';
			if(da[d].dir == cdir)
			{
				bc='b5dccf';
				ac='009f6b';
				cr='default';
				oc='';
			}
			if(d%3 == 0)
				data += '<div style="position:relative;left:0;top:0;">';
			if(d == 4)
				data += '<div style="position:relative;width:128px;height:1px;margin:4px;border-right:1px solid transparent;border-bottom:1px solid transparent;display:inline-block;"></div>';
			else
				data += '<div style="position:relative;width:128px;height:128px;margin:4px;background:#'+bc+';color:#'+ac+';font-size:64px;font-weight:bolder;text-align:center;border-radius:8px;border-right:1px solid #000;border-bottom:1px solid #000;box-shadow:4px 4px 4px rgba(0,0,0, 0.25);cursor:'+cr+';display:inline-block;"'+oc+'>'+da[d].arrow+'<div style="position:absolute;left:0;bottom:8px;width:128px;text-align:center;"><i class="s2 grey" style="font-weight:normal;">'+da[d].text+'</i></div></div>';
			if(d%3 == 2)
				data += '</div>';
		}
		fxf_fn_fxLinkDisplay(data, el.attributes['tooltip'].value, false);
		return;
	}

	// Direction set
	fxf_fn_designField(id,'dir'+ndir,event);
}

function fxf_fn_designField(id,value,event)
{
	var e4=id.substr(0,4);
	var v3=value.substr(0,3);
//alert('fxf_fn_designField(id='+id+', value='+value+')\n\ne4='+e4+'\nv3='+v3);

	if((e4 == 'Feld') && (v3 == 'dp_'))
	{
//alert('changeSelectedValue to '+value.substr(3));
		var se=fxf_fn_getField($(id),true);
		fxf_fn_changeSelectedValue(se,value.substr(3),true);
	}

	var n=$('design').value;
	var d=$('dsg').innerHTML;
	var f=$('tcss_filename').innerHTML;
	var m=parseInt($('ms').innerHTML);
	var s=document.querySelector('input[name="msec"]:checked').value;
//alert('design_feld:\n\nclient - c='+oSet.client+'\nperson - p='+oSet.person+'\nid - i='+id+'\nvalue - v='+value+'\n\n$design.value - n='+n+'\n\n$dsg.innerHTML - d='+d+'\n\n$tcss_filename.innerHTML - f='+f+'\n\n$ms.innerHTML - m='+m+'\n\n$msec:checked.value - s='+s);
	var url=fxf_fn_gProgram('design_feld', 'c='+oSet.client+'&p='+oSet.person+'&i='+fxf_fn_escapeText(id,false)+'&v='+fxf_fn_escapeText(value,false)+'&n='+fxf_fn_escapeText(n,false)+'&d='+fxf_fn_escapeText(d,false)+'&f='+fxf_fn_escapeText(f,false)+'&m='+fxf_fn_escapeText(m,false)+'&s='+fxf_fn_escapeText(s,false)+fxf_fn_gParam());
//alert('url='+url);

	new Ajax.Request(url,
	{
		method:'get', asynchronous:true,
		onSuccess: function(transport)
		{
			var rs=transport.responseText;
			if(rs.length)
			{
//fxf_fn_fxLinkDisplay(rs, 'Return string', false);
				var rs_menu='';
				var dp=rs.indexOf(divstr[2]);
				if(dp >= 0)
				{
					rs_menu=rs.substr(dp+5);
					rs=rs.substr(0,dp);
				}
				var rs_field='';
				var dp=rs.indexOf(divstr[1]);
				if(dp >= 0)
				{
					rs_field=rs.substr(dp+5);
					rs=rs.substr(0,dp);
				}
				var rs_css='';
				var dp=rs.indexOf(divstr[0]);
				if(dp >= 0)
				{
					rs_css=rs.substr(dp+5);
					rs=rs.substr(0,dp);
				}
//fxf_fn_fxLinkDisplay('<u>rs:</u><br />'+rs+'<hr size=1><br /><u>rs_css:</u><br />'+rs_css+'<hr size=1><br /><u>rs_field:</u><br />'+rs_field+'<hr size=1><br /><u>rs_menu:</u><br />'+rs_menu+'<hr size=1><br />', 'Return string', false);

				// CSS has changed?
				if(rs_css.length)
				{
					$('preview_style_sheet').innerHTML=rs_css;
				}

				// Field has changed?
				if(rs_field.length)
				{
					var rss=rs_field.split('|');

					// Design editor description
					var dd=$('de_dsc');
					if(dd)
						dd.innerHTML=rss[0];
					// Design editor setting
					var ds=$('de_set');
					if(ds && (rss.length > 1))
						ds.innerHTML=rss[1];
				}

				// Menu has changed?
				if(rs_menu.length)
				{
					var rss=rs_menu.split('!-m-!');

					// Menu preview
					var pm=$('dsg_menusection');
					if(pm)
						pm.innerHTML=rss[0];
					// Menu design description
					var md=$('mde_dsc');
					if(md && (rss.length > 1))
						md.innerHTML=rss[1];
					// Menu design colors
					var mc=$('mde_col');
					if(mc && (rss.length > 2))
						mc.innerHTML=rss[2];
					// Menu design preview
					var mp=$('mde_prv');
					if(mp && (rss.length > 3))
						mp.innerHTML=rss[3];
				}
			}
		}
	});
}