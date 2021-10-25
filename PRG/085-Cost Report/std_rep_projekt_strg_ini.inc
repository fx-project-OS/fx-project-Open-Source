<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : std_rep_projekt_strg_ini.inc                                 //
// Version     : 21.2                                                         //
// Begin       : 2020-11-18                                                   //
// Last Change : 2020-11-18                                                   //
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
 * PF 85: Cost Report
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

${'cpo'.$GLOBALS['fxpglobals']['tr']}=true;
$jetzt=$GLOBALS['datetime'];

$Personen_ID=$GLOBALS['fxpglobals']['person'];
if(fxIsArray($_POST) && isset($_POST['Personen_ID']))
	$Personen_ID=(int)$_POST['Personen_ID'];

$Projekt_ID=$GLOBALS['fxpglobals']['project'];
if(fxIsArray($_POST) && isset($_POST['Projekt_ID']))
	$Projekt_ID=(int)$_POST['Projekt_ID'];

// Startdatum: Keine Benutzereingaben, dann als Startdatum den ersten Tag des aktuellen Monats
if(!strlen($Zeitspanne_von) && !$_POST['Button_Anzeigen'])
	$von=substr($jetzt, 0, 6).'01000000';
// Startdatum: Benutzereingaben vorhanden, d.h. Datum auf Fehler prüfen
else
	$von=checkdatum('Zeitspanne_von');

// Enddatum: Keine Benutzereingaben, dann Enddatum=Heute
if(!strlen($Zeitspanne_bis) && !$_POST['Button_Anzeigen'])
	$bis=$jetzt;
// Enddatum: Benutzereingaben vorhanden, d.h. Datum auf Fehler prüfen
else
	$bis=checkdatum('Zeitspanne_bis');

// Fehlerprüfung, ob Enddatum vor Beginndatum liegt
if(!$fehler)
{
	if(!strlen($von) || !strlen($bis))
	{
		error_msg(1120, array('Zeitspanne_von', 'Zeitspanne_bis')); // Keine gültigen Zeitdaten
		$fehler=true;
	}
	else
	{
		$von=substr($von, 0, 8).'000000';
		$bis=substr($bis, 0, 8).'240000';
		if($bis < $von)
		{
			error_msg(1065, array('Zeitspanne_von', 'Zeitspanne_bis')); // Endzeit vor Startzeit
			$fehler=true;
		}
	}
}
?>