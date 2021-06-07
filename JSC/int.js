////////////////////////////////////////////////////////////////////////////////
// File name   : int.js                                                       //
// Version     : 21.1                                                         //
// Begin       : 2020-07-30                                                   //
// Last Change : 2020-08-20                                                   //
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
 * Javascript functions and routines for all internal programs, i.e. programs in the subdirectory ../INT
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.1
 */

var startup_error='';
var status_check=true;
var status_timer=0.1;

var mcx=0, mcy=0;

// Get all necessary objects by id
var id_status=$('status');		// Surrounding div of progress bar and loading icon
var id_statusd=$('statusd');	// Surrounding div of progress bar
var id_statush=$('statush');	// Progress bar: Headline
var id_statusb=$('statusb');	// Progress bar: Fill bar
var id_statust=$('statust');	// Progress bar: Bar text
var id_statusm=$('statusm');	// Loading icon: Message
if(!id_status || !id_statusd || !id_statush || !id_statusb || !id_statust || !id_statusm)
	startup_error += 'Necessary status objects not found!<br />';

var id_work=$('work');			// Surrounding div of main work area
var id_runtime=$('runtime');	// Surrounding div of runtime (optional)
if(!id_work)
	startup_error += 'Necessary work object not found!<br />';

var id_info=$('info');			// Info passed from source file
if(!id_info)
	startup_error += 'Necessary info object not found!\n';

// Get necessary info
if(id_info)
{
	var prgname='';
	var datetime='';
	var locseskey='';
	var emode='';

	var ia=id_info.innerHTML.split('|');
	if(ia.length > 0)
		prgname=ia[0];
	if(ia.length > 1)
		datetime=ia[1];
	if(ia.length > 2)
		locseskey=ia[2];
	if(ia.length > 3)
		emode=ia[3];
	if(!prgname.length)
		startup_error += 'Necessary info [program name] not submitted!\n';
}

if(startup_error)
{
	alert(startup_error);
	throw new Error(startup_error);
}

var el_tooltip=null;
var id_tooltip=$('tooltip');
if(id_tooltip)
	id_tooltip.style.display='none';

document.observe('mousemove', fxf_eh_mouseMove);
document.observe('mouseover', fxf_eh_mouseOver);
document.observe('mouseout', fxf_eh_mouseOut);

var locstoid=localStorage.getItem('locstoid');
var sesstoid=sessionStorage.getItem('sesstoid');
var urladd='&prgname='+prgname+'&locstoid='+locstoid+'&sesstoid='+sesstoid+'&locseskey='+locseskey+'&emode='+emode;
//alert('urladd='+urladd);

// Everything OK, load start
var lid=0;
fxf_fn_checkStatus();
fxf_fn_load(lid);

// Mouse Move
function fxf_eh_mouseMove(event)
{
	// Get mouse position
	if(event.pageX != null)
	{
		mcx=event.pageX;
		mcy=event.pageY;
	}
	else
	{
		mcx=0;
		mcy=0;
		if(event.clientX != null)
		{
			mcx=event.clientX;
			mcy=event.clientY;
		}
		mcx=mcx + (document.documentElement.scrollLeft ? document.documentElement.scrollLeft : document.body.scrollLeft);
		mcy=mcy + (document.documentElement.scrollTop ? document.documentElement.scrollTop : document.body.scrollTop);
	}

	// Tooltip?
	if(id_tooltip && (id_tooltip.style.display != 'none'))
	{
		id_tooltip.style.left=(mcx+26)+'px';
		id_tooltip.style.top=(mcy+4)+'px';
	}
}

// Mouse Over
function fxf_eh_mouseOver(event)
{
	var element=$(Event.element(event));
	// Tooltip?
	if(id_tooltip && element && element.attributes && (typeof element.attributes['tooltip'] != 'undefined'))
	{
		if(element.attributes['tooltip'].value.length)
		{
			id_tooltip.innerHTML=element.attributes['tooltip'].value;
			id_tooltip.style.display='';
		}
		else if(id_tooltip.style.display != 'none')
			id_tooltip.style.display='none';
	}
}

// Mouse Out
function fxf_eh_mouseOut(event)
{
	var element=$(Event.element(event));
	// Tooltip?
	if(id_tooltip && (id_tooltip.style.display != 'none'))
		id_tooltip.style.display='none';
}

// Load program via ajax
function fxf_fn_load(id, pst)
{
	fxf_fn_progress('Loading &hellip;', '', -1);
	status_check=true;
	fxf_fn_checkStatus();

	var url=prgname;
	id=parseInt(id);
	if(typeof(pst) == 'undefined')
		pst='';
	else
		pst='&'+pst;

	var get=window.location.search;
	if(get.length && window.history.replaceState)
		window.history.replaceState({}, '', url);

	var frm=document.forms;
	if(frm && frm.length)
	{
//var fd=frm.length+' form(s) found...\n\n';
		for(var f=0; f<frm.length; f++)
		{
//fd += 'Form #'+f+': id='+frm[f].id+':\n';
			var elm=frm[f].elements;
			if(elm && elm.length)
			{
//fd += '- '+elm.length+' element(s) found...\n';
				for(var e=0; e<elm.length; e++)
				{
//fd += '---> Element #'+e+': id='+elm[e].id+', name='+elm[e].name+', type='+elm[e].type+', value='+elm[e].value+'\n';
					var ein='';
					if(elm[e].hasAttribute('id'))
						ein=elm[e].id;
					else if(elm[e].hasAttribute('name'))
						ein=elm[e].name;
					if(ein.length)
					{
						if(elm[e].type == 'checkbox')
						{
							var val=0;
							if(elm[e].checked)
								var val=1;
						}
						else
						{
							var val=elm[e].value;
							if(typeof val == 'number')
							{
								if(Math.ceil(parseFloat(val)) === val)
									val=parseInt(val);
							}
							else if(val.length)
								val=encodeURIComponent(val);
						}
						pst=pst+'&'+ein+'='+val;
					}
				}
			}
//fd += '\n';
		}
//alert(fd);
	}
//alert('url='+url+'\n\nid='+id+'\nget='+get+'\npst='+pst+'\nlocstoid='+locstoid+'&sesstoid='+sesstoid+'\nemode='+emode);

	new Ajax.Request(url+get,
	{
		method:'post',  postBody:'menu_set='+id+pst+urladd, asynchronous:true,
		onSuccess: function(transport)
		{
			id_work.innerHTML=transport.responseText;
			status_check=false;
			lid=id;
			fxf_fn_progress.delay(status_timer, '', '', -1);
		}
	});
}

// Check status
function fxf_fn_checkStatus()
{
	var url=prgname;
//alert('url='+url);

	new Ajax.Request(url,
	{
		method:'post',  postBody:'check_status=1&menu_set='+lid+urladd, asynchronous:true,
		onSuccess: function(transport)
		{
			var status=transport.responseText;
			if(status.length)
			{
				sa=status.split('|');
				var msg='';
				if(sa.length > 0)
					msg=sa[0];
				var hdl='';
				if(sa.length > 1)
					hdl=sa[1];
				var pct=-1;
				if(sa.length > 2)
					pct=parseInt(sa[2]);
//alert('CheckStatus (check='+status_check+', timer='+status_timer+'):\n\n'+status+'\n\nmsg: '+msg+'\nhdl: '+hdl+'\npct: '+pct);
				fxf_fn_progress(msg, hdl, pct);
			}
			if(status_check)
				fxf_fn_checkStatus.delay(status_timer);
		}
	});
}

// Display progress bar and/or loading icon
function fxf_fn_progress(message, headline, percent)
{
	var disp=false;

	// Message
	if(id_statusm.innerHTML != message)
		id_statusm.innerHTML=message;
	if(message.length)
		disp=true;

	// Progress
	if(id_statush.innerHTML != headline)
		id_statush.innerHTML=headline;
	percent=Math.min(100, parseInt(percent));
	if(percent >= 0)
	{
		disp=true;

		var pt=percent+'%';
		if(id_statust.innerHTML != pt)
			id_statust.innerHTML=pt;
		if(id_statusb.style.width != pt)
			id_statusb.style.width=pt;

		if(id_statusd.style.display == 'none')
			id_statusd.style.display='';
	}
	else if(id_statusd.style.display != 'none')
		id_statusd.style.display='none';

	// Display progress bar or loading icon?
	if(disp)
	{
		if(id_status.style.display == 'none')
			id_status.style.display='';
	}
	else if(id_status.style.display != 'none')
		id_status.style.display='none';
}

// Handler that is called if user clicks on a function name in the function list
function fxf_fn_selectListEntry(prefix, name)
{
//alert('fxf_fn_selectListEntry(prefix=\''+prefix+'\', name=\''+name+'\'');
	var cls='';
	var ia=['fct', 'pfl'];
	for(var i=0; i<ia.length; i++)
	{
		var lst=$$('[id^="'+ia[i]+'_"]');
		for(var l=0; l<lst.length; l++)
		{
			if(lst[l].id == prefix+'_'+name)
			{
				if(lst[l].className == 'fsls')
					lst[l].className='fsla';
				else
					lst[l].className='fsls';
				cls=lst[l].className;
			}
			else if(lst[l].className == 'fsls')
				lst[l].className='fsla';
		}
	}

	var url=prgname;
//alert('url='+url+'\nprefix='+prefix+', name='+name+', cls='+cls);

	new Ajax.Request(url,
	{
		method:'post',  postBody:'fetch_result=1&menu_set='+lid+'&prefix='+prefix+'&name='+name+'&cls='+cls+urladd, asynchronous:true,
		onSuccess: function(transport)
		{
			var result=transport.responseText;
			var pp=result.indexOf('|');
			if(pp > 0)
			{
				var rhd=result.substr(0,pp);
				var rct=result.substr(pp+1);
			}
			else
			{
				var rhd='';
				var rct=result;
			}
//alert('rhd:\n'+rhd+'\n\nrct:\n'+rct);

			var drhd=$('result_header');
			if(drhd)
				drhd.innerHTML=rhd;

			var drct=$('result_content');
			if(drct)
				drct.innerHTML=rct;
		}
	});
}

function fxf_fn_intScroll(src, fix)
{
	var sd=$(src);
	var fd=$(fix);
	if(sd && fd && fd.style)
		fd.style.left=sd.scrollLeft+'px';
}