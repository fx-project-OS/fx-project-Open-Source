<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : ekvk_select.inc                                              //
// Version     : 24.1                                                         //
// Begin       : 2020-11-17                                                   //
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
 * 268: Hourly Rates
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
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
if(!isset($kat))
	$kat=0;
if(fxIsArray($_GET) && isset($_GET['kat']))
{
	$kat=$_GET['kat'];
	$callAJAX=true;
}
$kat=(int)$kat;
$s_name='AuswahlFeldtyp';

// Auswahlwerte rücksetzen
${$s_name.'_jf'} = false;
if(isset($GLOBALS['fxpselects']) && is_array($GLOBALS['fxpselects']) && isset($GLOBALS['fxpselects'][$s_name]))
	unset($GLOBALS['fxpselects'][$s_name]);

// Kein Projekttypfilter
$GLOBALS['noselpopptype']=true;

// Auswahlarray entsprechend der Kategorie füllen
$rarr=array();
switch($kat)
{
	case 21:	// 21-Personen: Mandant, Auftraggeber, Int. Mitarbeiter + Ext. Mitarbeiter
		${$s_name.'_arg'}=array(
			'modus'=>2,
			'ptpa'=>array(
				'persontypes'=>array(FXP_PT_CLIENT, FXP_PT_CUSTOMER, FXP_PT_EMPLOYEE, FXP_PT_CONTRACTOR),
				'all_clients'=>false
			)
		);

		$rarr=auswahl_Personen_ID(1, false, $s_name);
	break;

	default:	// 66-Vorgänge: Mutter- und Teilprojekte
		${$s_name.'_arg'}=array
		(
			'projects_only'=>true
		);

		$rarr=auswahl_Projekt_ID(1, false, $s_name);
	break;
}

// Ergebnis im AJAX-Fall am Bildschirm ausgeben
if($callAJAX)
{
	// Load selects
	fxSession('s','l');

	// Create new select
	$GLOBALS['fxpselects'][$s_name]=array();
	if(fxIsArray($rarr))
	{
		foreach($rarr as $value => $text)
			$GLOBALS['fxpselects'][$s_name][$value]=$text;
	}

	// Save selects
	fxSession('s','s');
	fxSession('t','p',$s_name,0);

	$result=selEntriesJS($s_name, $GLOBALS['fxpselects'][$s_name], false, false, true);
	echo($result);
}
else
{
	${$s_name.'_ds'}=0;
	${$s_name.'_array'}=$rarr;
}
?>