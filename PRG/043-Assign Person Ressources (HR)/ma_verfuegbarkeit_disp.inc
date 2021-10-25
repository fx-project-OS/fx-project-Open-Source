<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : ma_verfuegbarkeit_disp.inc                                   //
// Version     : 21.2                                                         //
// Begin       : 2020-12-10                                                   //
// Last Change : 2020-12-10                                                   //
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
 * PF 43: Assign Person Ressources (HR)
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

$maske181_sf=true;

$vba=$GLOBALS['fxptdata']['backup']['projekt_array'];	// Backup der alte Einstellung
$GLOBALS['fxptdata']['backup']['projekt_array']['aufgabe']=1;
echo(maske(181));
$GLOBALS['fxptdata']['backup']['projekt_array']=$vba;	// Rücksetzen der alten Einstellung

// Kalender darstellen
if($_POST['ZAuslastung'] && $_POST['ZProjekte'])
	$mhead=1250;
else if($_POST['ZProjekte'])
	$mhead=1249;
else if($_POST['ZAuslastung'])
	$mhead=1248;
else
	$mhead=1146;
$mhead_text=meldung($mhead).' (';
if($ke == 'D')
	$mhead_text .= meldung(1261);
else if($ke == 'W')
	$mhead_text .= meldung(1262);
else if($ke == 'M')
	$mhead_text .= meldung(1263);
else
	$mhead_text .= meldung(1264);
if($MaFaehigkeit_ID)
	$mhead_text .= ', '.meldung(1698);	//Sortiert nach Fähigkeiten
$mhead_text .= ')';

if(fxIsArray($personen) && fxIsArray($GLOBALS['fxptdata']['cal'][3]))
{
	// Legende zeichnen
	if(!$_POST['ZAuslastung'] && !$_POST['ZProjekte'])
		$legende='';
	else
		$legende=farblegende('get');

	// Hidden-Felder angeben, falls auf "Mitarbeiter zuordnen" geklickt wird
	kalender($mhead, $legende, '', $mhead_text, '', 1248);
}

// Fehlermeldung, falls keine Daten gefunden wurden
else if($_POST['Button_Anzeigen'] && !$fehler && ($awerr < 2))
{
	$mldtxt=meldung(1066);
	if(is_array($mldtxt))
		$mldtxt=$mldtxt['tabwert'];
	kalender($mhead, '?'.$mldtxt, '', $mhead_text, '', 1248);
}
?>