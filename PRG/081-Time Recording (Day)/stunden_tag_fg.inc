<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : stunden_tag_fg.inc                                           //
// Version     : 21.2                                                         //
// Begin       : 2020-11-09                                                   //
// Last Change : 2020-11-09                                                   //
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
 * PF 81: Time Recording (Day) - AJAX
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

// Was program call by JavaScript/AJAX or included
$callAJAX=false;
if(!isset($fg_pid))
	$fg_pid=0;
if(fxIsArray($_GET) && isset($_GET['fg_pid']))
{
	$fg_pid=$_GET['fg_pid'];
	$callAJAX=true;
}
$fg_pid=(int)$fg_pid;

// Fortschrittsermittlungsmethode aus DB ermitteln
if($fg_pid > 0)
{
	$fg_array=get_projekt_methode($fg_pid);

	$fg_methode=(int)$fg_array['methode'];
	$fg_zeit=(int)$fg_array['zeitpunkt'];
	$fg_mtext=meldung($fg_methode, true);
	$ppos=strpos($fg_mtext,'(');
	if($ppos)
		$fg_mtext=trim(substr($fg_mtext,0,$ppos));

	if($fg_zeit == 1790)	// Auf Anforderung (Standard)
		$cls='grey';
	else
		$cls='blue';

	$onclick=' '.fxf_jsFunction('fxLink', false, 'loader.php?url=progress_methods.inc&m='.$fg_methode.'&s='.$GLOBALS['fxpglobals']['lang']);
	$fg_methode='<font class="s2 '.$cls.'" style="cursor:pointer;border-bottom:1px dotted #bbbbbb;"'.$onclick.'>('.$fg_mtext.')</font>';
}
else
{
	$fg_methode='<font class="s2 lightred">('.meldung(388,true).')</font>';
}

// Ergebnis im AJAX-Fall am Bildschirm ausgeben
if($callAJAX)
{
	// Auftraggeber ebenfalls ermitteln und übermitteln
	$cust=0;
	if($fg_pid > 0)
		$cust=(int)db_value("SELECT kunde FROM projekte WHERE projekt_id=".$fg_pid." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
	echo($fg_methode.'|'.$cust);
}
?>