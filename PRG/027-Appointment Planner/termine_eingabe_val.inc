<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : termine_eingabe_val.inc                                      //
// Version     : 24.1                                                         //
// Begin       : 2020-11-02                                                   //
// Last Change : 2024-07-04                                                   //
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
 * PF 27: Appointment Planner
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

$Termin_ID=$GLOBALS['fxptdata']['terminid'];
$dbwerte['termin_id']=$Termin_ID;

// Personendaten ermitteln
$pdata = get_persondata($persid);
if(!is_array($pdata))
{
	$err = true;
	error_msg(550, false);	// 550: Die gewählte Person besitzt noch keine Zusatzdaten!
}

// Datumsprüfung
if(!$err)
{
	$db_startdatum	= substr((string)$dbwerte['soll_beg_dtm'], 1, 8);
	if($dbwerte['terminende'] != 'NULL')
	{
		$db_enddatum = substr((string)$dbwerte['terminende'], 1, 8);
		if($db_enddatum == '00000000')
			$db_enddatum = $db_startdatum;
		else if($db_startdatum != $db_enddatum)
		{
			$err = true;
			error_msg(1864, array('Terminende'));	// 1864: Das Termin-Enddatum ist ungleich dem Termin-Startdatum! In fx-project können Termine nicht über mehrere Tage gehen.
		}
	}
	else
		$db_enddatum = $db_startdatum;
}

// Datumsprüfung für Synchronisation
if(!$err && ($dbwerte['terminende'] != 'NULL'))
{
	$db_startdatum	= substr((string)$dbwerte['soll_beg_dtm'], 1, 8);
}

// Start- + Endzeit
if(!$err)
{
	$asc = validate_entry(get_entry($pdata['soll_beginn'], 'zeit'), $ierr, 'zeitspanne');
	$ast = ($pdata['wochenarbeitstd']*3600)/$pdata['arbeitstage'];

	// Startzeit
	if($ganztaegig == 2573)									// halbtägig (2. Hälfte)
	{
		$ssc = $asc+($ast/2)+$pdata['pause'];
		$h = (int)($ssc/3600); $ssc = $ssc-$h*3600;
		$m = (int)($ssc/60);
		$s = $ssc-$m*60;
		$db_startzeit = substr('0'.$h, -2).substr('0'.$m, -2).substr('0'.$s, -2);
	}
	else if(($ganztaegig == 2571) || ($ganztaegig == 2572)) // ganztägig oder halbtägig (1. Hälfte)
		$db_startzeit = substr((string)$pdata['soll_beginn'], 8);
	else
		$db_startzeit = substr((string)$dbwerte['terminstart'], 9, 6);

	// Endzeit
	if($ganztaegig)
	{
		if($ganztaegig == 2572)								// halbtägig (1. Hälfte)
 			$ssc = $asc+($ast/2);
 		else												// ganztägig oder halbtägig (2. Hälfte)
 			$ssc = $asc+$ast+$pdata['pause'];
		$h = (int)($ssc/3600); $ssc = $ssc-$h*3600;
		$m = (int)($ssc/60);
		$s = $ssc-$m*60;
		$db_endzeit = substr('0'.$h, -2).substr('0'.$m, -2).substr('0'.$s, -2);
	}
	else if($dbwerte['terminende'] != 'NULL')
		$db_endzeit = substr((string)$dbwerte['terminende'], 9, 6);
	else
		$db_endzeit = $db_startzeit;

	$db_start = $db_startdatum.$db_startzeit;
	$db_end = $db_enddatum.$db_endzeit;

	$maskenfeldwerte['soll_beg_dtm']	= $db_start;
	$maskenfeldwerte['terminstart']		= $db_start;
	$maskenfeldwerte['terminende']		= $db_end;

	if($db_end < $db_start)
	{
		$err = true;
		error_msg(177, array('Terminende'));	// 177: Das Terminende liegt vor dem Terminstart?
	}
}

if(!$err)
{
	// Termine können logischerweise nur in dem Zeitraum stattfinden, in dem der Mitarbeiter in der Firma ist
	if(substr((string)$db_start, 0, 8) < substr((string)$pdata['maeintrittsdatum'], 0, 8))
	{
		$err = true;
		error_msg(473, array('Soll_Beg_Dtm', 'Terminstart'), get_entry($pdata['maeintrittsdatum'], 'datum'), get_entry($db_start, 'datum'));	// 473: Das Eintrittsdatum ($var2) des Mitarbeiters liegt nach dem geplanten Startdatum ($var3)!
	}
	if($pdata['maaustrittsdatum'] && (substr((string)$db_end, 0, 8) > substr((string)$pdata['maaustrittsdatum'], 0, 8)))
	{
		$err = true;
		error_msg(474, array('Terminende'), get_entry($pdata['maaustrittsdatum'], 'datum'), get_entry($db_end, 'datum'));	// 474: Das Austrittsdatum ($var2) des Mitarbeiters liegt vor dem geplanten Enddatum ($var3)!
	}

	// Prüfen, ob beim Anlegen von Terminen bereits Termine in diesem Zeitraum eingetragen sind (falls Terminüberschneidung nicht erlaubt ist)
	if((strtolower(substr((string)$GLOBALS['fxpglobals']['settings']['termin_uebers'], 0, 1)) == 'n') && ($dbwerte['terminart'] != FXP_AT_REMINDER) && ($dbwerte['terminart'] != FXP_AT_MESSAGE) && ($dbwerte['terminart'] != FXP_AT_PROJECTMESSAGE))
	{
		$termine_no = FXP_AT_MEETING.",".FXP_AT_CONFERENCE.",".FXP_AT_BUSINESSDINNER.",".FXP_AT_CONTACT.",".FXP_AT_TODO;

		$stsql  = "SELECT termin_id, startzeit, endzeit, terminart, name_termin FROM termine";
		$stsql .= " WHERE (archiv=0 OR archiv IS NULL) AND personen_ID=".$persid;
		$stsql .= " AND terminart IN (".$termine_no.")";
		$stsql .= " AND (bestaetigung_anmerk IS NULL OR bestaetigung_anmerk NOT LIKE '0-%') AND startzeit<'".$db_end."' AND endzeit>'".$db_start."'";
		if($dbwerte['termin_id'])
			$stsql .= " AND termin_id<>".$dbwerte['termin_id'];
		$stsql .= " ORDER BY startzeit";
		$starr = db_values($stsql, '*');
//fxDebug($starr, $stsql);
		if(is_array($starr))
		{
			$txt = '<br><ul>';
			foreach($starr as $stv)
				$txt .= '<li><font color=#0000aa>['.$tarten[$stv['terminart']].']</font><br>'.get_entry($stv['startzeit'], 'datetime').' - '.get_entry($stv['endzeit'], 'datetime').': <b>'.$stv['name_termin'].'</b></li>';
			$txt .= '</ul>';

			// 178: In diesem Zeitraum ($var1) können für $var2 keine Termine eingetragen werden, da bereits andere Einträge vorhanden sind!$var3
			error_msg(178, false, get_entry($db_start, 'datetime').' - '.get_entry($db_end, 'zeit'), '<b>'.$pdata['name'].'</b>', $txt);
			$err = true;
		}
	}

	// 2340: Das Erinnerungsdatum an einen Termin darf nicht gleich dem Datum des Termins sein!
	if(($dbwerte['wiedervorlage'] != 'NULL') && (substr((string)$dbwerte['wiedervorlage'], 1, 8) == substr((string)$db_start, 0, 8)))
	{
		$err = true;
		error_msg(2340, array('Soll_Beg_Dtm', 'Wiedervorlage'));
	}

	// Wiederholung
	if($dbwerte['wiederholung'] && ($dbwerte['wiederholung'] != 'NULL'))
	{
		// 181: Beim Anlegen von Wiederholungs-Terminen muss eine Begrenzung eingegeben werden!
		if(($dbwerte['endedatum_wdh'] == 'NULL') && (($dbwerte['wiederholung_anz'] == 'NULL') || ($dbwerte['wiederholung_anz'] == 0)))
		{
			$err = true;
			error_msg(181, array('Endedatum_WDH', 'wiederholung_anz'));
		}

		// 182: Das Ende einer Wiederholung$var1 kann nicht vor dem Starttermin$var2 liegen!
		if(!$err && (substr((string)$dbwerte['endedatum_wdh'], 1, 8) <= substr((string)$dbwerte['soll_beg_dtm'], 1, 8)))
		{
			$err = true;
			error_msg(182, array('Endedatum_WDH'), ' <I><FONT COLOR="#882233">('.get_entry(substr((string)$dbwerte['endedatum_wdh'], 1, 14), 'datum').')</FONT></I>', ' <I><FONT COLOR="#223388">('.get_entry(substr((string)$dbwerte['soll_beg_dtm'], 1, 14), 'datum').')</FONT></I>');
		}

		// 188: Bitte beachten! Bei Wiederholungsterminen werden nur zum Starttermin$var1 Einladungen verschickt.
		if(!$err && isset($_POST['einladen']) && is_array($_POST['einladen']))
		{
			error_msg(188, false, ' <I>('.get_entry($db_start, 'datetime').' - '.get_entry($db_end, 'zeit').')</I>');
		}
	}

	// 742: Es wurde noch kein Projekt ausgewählt!
	if(isset($_POST['terminpr']) && $_POST['terminpr'] && (!isset($_POST['Projekt_ID']) || !$_POST['Projekt_ID']))
	{
		$err = true;
		error_msg(742, array('Projekt_ID'));
	}
}

// Klick auf "Prüfen" behandeln wie Fehler
if($checkit)
{
	error_msg(2992,false);	// 2992: Die Wochenvorschau in der Einladungsliste wurde entsprechend dem eingegebenem Terminstartdatum neu geladen.
	$err=true;
}
//$err=true;
?>