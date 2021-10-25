<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : helptext.inc                                                 //
// Version     : 21.2                                                         //
// Begin       : 2020-09-22                                                   //
// Last Change : 2020-09-22                                                   //
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
 * Display help text in a popup
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

// Search for and include "basics.inc" to set all definitions, variables and necessary dynamic paths
$__pnm='basics.inc';
if(!isset($GLOBALS['__loaded_'.$__pnm]))
{
	$__prg=$__pnm;
	$__pfc=0;
	while(($__pfc < 10) && !file_exists($__prg))
	{
		$__pfc++;
		$__prg='../'.$__prg;
	}
	if(file_exists($__prg))
	{
		require($__prg);
	}
	else
		die('<div style="padding:8px;"><h1 style="padding:8px;color:red;background:#fff;border:2px solid red;box-shadow:4px 4px 4px rgba(0,0,0, 0.5);">fx-project - ERROR: Necessary file &quot;'.$__pnm.'&quot; not found!</h1></div>');
}


////////////////////////////////////////////////////////////////////////////////
// SUBMIT VARIABLES
////////////////////////////////////////////////////////////////////////////////
$hid=$_GET['hid'];

$spr=$_GET['spr'];
if(!$spr)
	$spr=$GLOBALS['fxpglobals']['lang'];
if(!$spr)
	$spr=7;
if($spr != $GLOBALS['fxpglobals']['lang'])
	$GLOBALS['fxpglobals']['lang']=$spr;

$nhid=false;
if(strtolower(substr($hid, 0, 2)) == 'ew')
	$nhid='Einst';
else if(strtolower(substr($hid, 0, 2)) == 'op')
	$nhid='SOP';
else if(strtolower(substr($hid, 0, 2)) == 'up')
	$nhid='SUP';

// Get help message
$mld='';
if($nhid)
{
	$hid=(int)substr($hid, 2);
	if($nhid == 'Einst')
	{
		$esql  = "SELECT ed.einstellungs_id, ed.einstellungs_index, ed.einstellungs_name, ed.id_feld, w.tabwert, ev.vergaberecht, ew.einstellungswert";
		$esql .= " FROM s_einstellungsdef ed, s_einstellungsrecht ev, s_einstellungswerte ew, wertetab w";
		$esql .= " WHERE ed.einstellungs_id=".$hid;
		$esql .= " AND ev.einstellungs_id=ed.einstellungs_id AND ev.mandanten_id=0";
		$esql .= " AND ew.einstellungs_id=ed.einstellungs_id AND ew.mandanten_id=0";
		$esql .= " AND w.wertetabid=ed.einstellungs_name AND w.id_sprache=".$spr;
		$earr=db_values($esql);
//fxDebug($earr, $esql, 0);
		if(fxIsArray($earr))
			$mld=$earr['tabwert'];
	}

	$hid_text=$nhid.substr('0000'.$hid, -4);
}

// Wrong call?
else if($hid < 1)
	$mld='';

// Graphic mask
else
{
	$hp=strpos($hid, '|');
	if($hp)
	{
		$hid_text=(int)substr($hid, $hp+1);
		$hid=(int)substr($hid, 0, $hp);
	}
	else
		$hid_text=$hid;
	$mld=@meldung($hid, false);
}

$extr=true;

$hid='Hilfe_'.$hid_text;
if(($nhid == 'Einst') && is_array($earr))
{
	$msg=@get_message($hid);
//fxDebug($msg, $hid, 0);

	$ki='';
	$be='';
	$bl=false;
	if(strlen($msg))
	{
		// Compact info
		$spos=strpos($msg, '<ki>');
		if($spos !== false)
		{
			$epos=strpos($msg, '</ki>', $spos);
			if($epos !== false)
			{
				$ki=extract_meldung(substr($msg, $spos+4, $epos-4-$spos));
				$msg=substr($msg, 0, $spos).substr($msg, $epos+5);
			}
			else
			{
				$ki=extract_meldung(substr($msg, $spos+4));
				$msg=substr($msg, 0, $spos);
			}
		}

		$spos=strpos($msg, '<be>');
		if($spos !== false)
			$bl=true;

		$be=extract_meldung($msg);
	}
//fxDebug($ki, 'Compact info');
//fxDebug($be, 'Description');

	// Setting
	$msg='<b class="s6 blue tshadowlight">'.$earr['tabwert'].'</b><br /><br /><hr class="fxhrg" /><br />';

	// Compact info
	if($ki)
		$msg .= $ki.'<br /><br />';

	// Setting type
	if($earr['id_feld'])
		$ea=meldung(1892, true);	// Select field
	else
		$ea=meldung(1893, true);	// Input field

	// Possible setting values
	$msg .= '<div class="manual_std"><b class="s4">'.meldung(1891, false).':</b>&nbsp;&nbsp;&nbsp;<font class="s2 italic grey">['.$ea.']</font></div>';
	if($earr['id_feld'])	// Select field
	{
		$dwsql="SELECT wertetabid, tabwert FROM wertetab WHERE id_feld=".$earr['id_feld']." AND id_sprache=".$spr." AND mandanten_id=0 ORDER BY position, tabwert";
		$dwarr=db_values($dwsql, '*');
		if(fxIsArray($dwarr))
		{
			$msg .= '<ul style="padding-left:12px;">';
			foreach($dwarr as $dw)
			{
				$msg .= '<li>'.$dw['tabwert'];
				if($dw['wertetabid'] == $earr['einstellungswert'])
					$msg .= '&nbsp;&nbsp;<i class="grey">('.meldung(1894, false).')</i>';	// Standard
				$msg .= '</li>';
			}
			$msg .= '</ul>';
		}
	}
	else					// Various, depends on setting frame
		$msg .= '<ul style="padding-left:12px;"><li>'.meldung(1895, false).'&nbsp;&nbsp;<i class="grey">('.meldung(1894, false).': <b>'.$earr['einstellungswert'].'</b>)</i></li></ul>';	// Standard
	$msg .= '<br />';

	// Setting rights (Systemadministrator, Main administrator (Client 1), Client administrator, User)
	$vra=explode(',', meldung(1897, false));
	if(fxIsArray($vra))
	{
		$msg .= '<div class="manual_std"><b class="s4">'.meldung(1896, false).':</b></div>';
		$msg .= '<table border="0" cellpadding="0" cellspacing="0">';
		foreach($vra as $vi => $vv)
		{
			if($vi <= $earr['vergaberecht'])
				$img='1';
			else
				$img='x';

			$msg .= '<tr>';
			$msg .= '<td><ul style="padding-left:12px;"><li>'.$vv.'</li></ul></td>';
			$msg .= '<td align="center" style="padding:0 12px;"><img src="'.$GLOBALS['gfxpath'].$img.'.png"></td>';
			$msg .= '</tr>';
		}
		$msg .= '</table><br />';
	}

	// Current setting value
	$usr_settings=benutzereinstellungen($GLOBALS['fxpglobals']['user'], false, false);
	$tv=$usr_settings[$earr['einstellungs_index']];
	if($earr['id_feld'])	// Select field
		$tv=meldung($tv, true);
	else if($earr['einstellungs_index'] == 'sel_max_cl')
	{
		$ra=getSettingMaxCL($tv);
		$rc='C';
		$rl='L';
		if($GLOBALS['fxpglobals']['lang'] == 1)
			$rc='Z';
		$tv .= '&nbsp;&nbsp;<span class="s3 lightgrey">&rarr;</span>&nbsp;&nbsp;<i class="s3 lightblue">{'.$rc.'} = '.$ra['chars'].'</i>&nbsp;&nbsp;<span class="s3 lightgrey">&rarr;</span>&nbsp;&nbsp;<i class="s3 lightblue">{'.$rl.'} = '.$ra['lines'].'</i>';
	}
	$msg .= '<div class="s4b blue" style="padding-left:18px;"><span class="lightgrey">&rarr;</span>&nbsp;&nbsp;'.$tv.'</div><br />';

	// Description
	if(strlen($be))
	{
		if(!$bl)
		{
			$msg .= '<hr class="fxhrg"><br />';
			$msg .= '<div class="manual_std"><b class="s4">'.meldung(1559, false).':</b></div>';
		}
		$msg .= '<p>'.$be.'</p>';
	}

	$extr=false;
}
else
	$msg=@get_message($hid);

$print_icon=true;
if(!strlen($msg))
{
	$print_icon=false;
	$msg='<table width=100% height=100% cellspacing=32><tr><td width=38% align=right><b style="font-size:128px;color:#c50016;text-shadow:2px 2px rgba(0,0,0, 0.5);">!</b></td><td width=62% align=left><b class="s5" style="color:#c50016;">'.meldung(966,false).'</b></td></tr></table>'; // Help text not available!
}

// Popup mask: Setting help text
$html=popupMask(meldung(2443,false), '', $msg, 'dcbpu', 880, 640, true, $print_icon);
echo($html);
?>