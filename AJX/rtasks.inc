<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : rtasks.inc                                                   //
// Version     : 21.1                                                         //
// Begin       : 2020-11-11                                                   //
// Last Change : 2020-11-11                                                   //
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
 * AJAX: Update ressource tasks
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.1
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

$callAJAX=false;
if(!isset($pid))
	$pid=0;
if(isset($_GET) && isset($_GET['pid']))
{
	$pid=(int)$_GET['pid'];
	$callAJAX=true;
}
$pid=(int)$pid;

if(isset($_GET) && isset($_GET['s_name']))
	$s_name=$_GET['s_name'];
else if(!isset($s_name))
	$s_name='Projekt_ID';

$Personen_ID=$pid;

// Date
if(strtolower($GLOBALS['fxpglobals']['settings']['ze_alle_aktiv']) == 'ja')		// Soll die Zeit- und Materialverbrauchserfassung generell auf alle aktiven Aufgaben ohne Startzeitbegrenzung erlaubt sein?
	$pdatum='99999999';
else
{
	$today=substr($GLOBALS['fxpglobals']['datetime'],0,8);
	if(isset($_GET) && isset($_GET['pdatum']))
		$pdatum=substr($_GET['pdatum'],0,8);
	else if(!isset($pdatum))
		$pdatum=$today;
	else
		$pdatum=substr($pdatum,0,8);
//echo('START: $pdatum='.$pdatum.'<br />');

	if(isset($_GET) && isset($_GET['ansicht']))
		$ansicht=(int)$_GET['ansicht'];
	else if(!isset($ansicht))
		$ansicht=0;
	else
		$ansicht=(int)$ansicht;
	if($ansicht)
	{
		if($ansicht == 696)	// Monatsletzter
			$pdatum=ultimo($pdatum,false);
		else				// Wochenletzer
			$pdatum=getLastWeekday($pdatum);
	}
//echo('- ANSICHT='.$ansicht.' -> $pdatum='.$pdatum.'<br />');

	if(strtolower($GLOBALS['fxpglobals']['settings']['zeit_zukunft']) == 'ja')	// Zeit- und Materialverbrauchserfassung für ein zukünftiges Datum erlauben?
		$pdatum=max($today, $pdatum);
//echo('- NO FUTURE? -> $pdatum='.$pdatum.'<hr />');
}
$pdatum=substr($pdatum,0,8).'240000';

// Set variables
$va=array(
	'multi'=>0, 'with_empty'=>0,
	'timereg'=>1, 'with_unknown'=>1, 'nocost'=>1, 'notodos'=>1
);
foreach($va as $vn => $vd)
{
	if(isset($GLOBALS[$vn]))
	{
		if($GLOBALS[$vn])
			$GLOBALS[$vn]=1;
		else
			$GLOBALS[$vn]=0;
	}
	else if(isset($_GET) && isset($_GET[$vn]))
	{
		if($_GET[$vn])
			$GLOBALS[$vn]=1;
		else
			$GLOBALS[$vn]=0;
	}
	else
		$GLOBALS[$vn]=$vd;
}

// Auswahlarray füllen
$rarr=array();
if($Personen_ID)
{
	$Projekt_ID_arg=array('person'=>$Personen_ID, 'timereg'=>$timereg, 'pdatum'=>$pdatum, 'with_unknown'=>$with_unknown, 'nocost'=>$nocost, 'notodos'=>$notodos);
	$rarr=auswahl_Projekt_ID(1);
}
else
	$rarr[0]=meldung(1449,false);	// [Unknown]
//fxDebug($rarr, '$rarr');

// Ergebnis im AJAX-Fall am Bildschirm ausgeben
if($callAJAX)
{
	// Load selects and transaction values
	$frm='';
	if(is_array($_GET) && isset($_GET['frm']))
		$frm=trim($_GET['frm']);
	fxSession('s','l');
	fxSession('t','l','ret');
	$GLOBALS['_form']=$frm;

	// Create new select
	if($with_empty)
	{
		if(isset($rarr[0]))
		{
			$arr[-1]=meldung(794,true);	// -leer-
			$arr[-2]='<d>';
		}
		else
		{
			$arr[0]=meldung(794,true);	// -leer-
			if(fxIsArray($rarr))
				$arr[-1]='<d>';
		}
	}
	if(fxIsArray($rarr))
	{
		foreach($rarr as $value => $text)
			$arr[$value]=$text;
	}
	$GLOBALS['fxpselects'][$s_name]=array();
	foreach($arr as $value => $text)
		$GLOBALS['fxpselects'][$s_name][$value]=$text;

	// Save selects
	fxSession('s','s');
	fxSession('t','w',$s_name,0);

	if($multi)
		$result=multiSelEntries($s_name,$arr);
	else
		$result=selEntriesJS($s_name,$arr,false,false,true);
//echo('<pre>'.fxHtmlEncode($result).'</pre>');
	echo($result);
}
else
{
	$GLOBALS[$s_name.'_array']=$rarr;
	echo('<div id="rtaskparam" style="display:none;">'.$Personen_ID.','.$pdatum.','.$multi.','.$with_empty.','.$timereg.','.$with_unknown.','.$nocost.','.$notodos.'</div>');
}
?>