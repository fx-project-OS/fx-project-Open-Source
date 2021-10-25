<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : std_rep_datum_strg_ini.inc                                   //
// Version     : 21.2                                                         //
// Begin       : 2020-08-27                                                   //
// Last Change : 2020-07-22                                                   //
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
 * ???FILE???
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

$err	= false;		// Fehlervariable
$cpo57	= true;			// Kalenderpopup-Icons anzeigen

// "Person" (SELECT: Personen_ID)
$Personen_ID=0;
if(isset($_POST['Personen_ID']))
	$Personen_ID=(int)$_POST['Personen_ID'];
$Pers_Arr=auswahl_Personen_ID(1);
if(sizeof($Pers_Arr) > 1)
	$Personen_ID_ds=1410;	// Alle Personen
else
	$Personen_ID_ds=0;
$Personen_ID_arg=array('override'=>$Pers_Arr, 'Personen_ID'=>$GLOBALS['fxpglobals']['person']);

// "Anzeigeart" (SELECT: Anzeigeart)
$Anzeigeart=1260;	// Automatisch
if(isset($_POST['Anzeigeart']))
	$Anzeigeart=(int)$_POST['Anzeigeart'];

// "Schätzung" (SELECT: schaetzung)
$schaetzung=0;	// Ohne
if(isset($_POST['schaetzung']))
	$schaetzung=(int)$_POST['schaetzung'];
//fxDebug('_POST'); fxDebug($schaetzung, '$schaetzung');

// Monatsnavigation?
if(fxIsArray($_POST) && isset($_POST['nav']) && fxIsArray($_GET) && isset($_GET['navy']) && isset($_GET['navm']))
{
	$aym=substr('0000'.$_GET['navy'],-4).substr('00'.$_GET['navm'],-2);

	$_POST['Zeitspanne_von']	= get_entry($aym.'01', 'datum');
	$_POST['Zeitspanne_bis']	= get_entry(ultimo($aym.'01'), 'datum');
	$_POST['Button_Anzeigen']	= true;

	$Zeitspanne_von				= $_POST['Zeitspanne_von'];
	$Zeitspanne_bis				= $_POST['Zeitspanne_bis'];
	$Button_Anzeigen			= true;
}

// "Zeitraum" (INPUT: Zeitspanne_von + Zeitspanne_bis)
// ...Von: Keine Benutzereingaben, dann als Startdatum den ersten Wochentag der akt. Kalenderwoche
if(!$Zeitspanne_von)
	$von=substr($GLOBALS['datetime'],0,6).'01000000';
else
	$von=checkdatum('Zeitspanne_von');
// ...Bis: Keine Benutzereingaben, dann Enddatum=Heute
if(!$Zeitspanne_bis)
	$bis=$GLOBALS['datetime'];
else
	$bis=checkdatum('Zeitspanne_bis');
// ...Fehlerprüfung, ob Enddatum vor Startdatum liegt
if(!$err)
{
	if($bis < $von)
	{
		error_msg(1065, false); // Endzeit vor Startzeit
		$err=true;
	}
	else
	{
		$von=substr($von,0,8).'000000';
		$bis=substr($bis,0,8).'240000';
	}
}

// "Leereinträge", "Stunden", "Arb.Tage", "Urlaub", "Sonderurl.", "Schulung", "Krank", "Abwesend" + "Gleittag"
// (CHECKBOX: leer_anzeigen, Stunden, Arbeitstage, Urlaub, sonderurlaub, Schulung, Krank, abwesenheit + Gleitzeit)
$cb_checked=0;
$cb_fields=array(
	'Arbeitstage'=>"Arbeitstage", 'Urlaub'=>"Urlaub", 'sonderurlaub'=>"sonderurlaub", 'Schulung'=>"Schulungstage", 'Krank'=>"Krankheitstage", 'abwesenheit'=>"abwesenheit", 'Gleitzeit'=>"GleitzeitStunden",
	'leer_anzeigen'=>"leer_anzeigen", 'stunden'=>"SOLLStunden"
);
foreach($cb_fields as $cb_field => $dummy)
{
	if(isset($_POST[$cb_field]) && $_POST[$cb_field])
	{
		${'cb_'.strtolower($cb_field)}=true;
		$cb_checked++;
	}
	else
		${'cb_'.strtolower($cb_field)}=false;
}
if(!$cb_checked)
{
	if($_POST['Button_Anzeigen'])
		error_msg(1646, false);	// Es wurde keine notwendige Checkbox markiert!
	$err=true;
}
?>