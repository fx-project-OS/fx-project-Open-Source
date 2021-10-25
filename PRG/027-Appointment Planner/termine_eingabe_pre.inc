<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : termine_eingabe_pre.inc                                      //
// Version     : 21.2                                                         //
// Begin       : 2020-11-02                                                   //
// Last Change : 2020-11-02                                                   //
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
 * @version 21.2
 */

$jetzt=$GLOBALS['datetime'];

// Daten holen
if($Termin_ID && !$err)
{
	$sql  = "SELECT t.personen_id, t.mastertermin_id, t.zeit_id, t.projekt_id,";
	$sql .= " t.startzeit AS soll_beg_dtm, t.endzeit AS soll_end_dtm, t.startzeit AS terminstart, t.endzeit AS terminende, t.soll_st_tag, t.soll_beginn,";
	$sql .= " t.beschreibung, t.terminart, t.treffpunkt, t.wiederholung, t.endedatum_db AS endedatum_wdh,";
	$sql .= " t.erledigt, t.wiedervorlagedb AS wiedervorlage, t.name_termin, t.bestaetigung_anmerk,";
	$sql .= " m.projekt_id AS masterprojekt_id, m.terminart AS masterterminart ";
	$sql .= "FROM termine t, termine m ";
	$sql .= "WHERE m.termin_id=t.mastertermin_id AND t.termin_id=".$Termin_ID." AND t.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND m.mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$maskenfeldwerte = db_values($sql);
	if(fxIsArray($maskenfeldwerte))
	{
		$_POST['Personen_ID'] = (int)$maskenfeldwerte['personen_id'];
		$maskenfeldwerte['wiederholung_anz'] = (int)substr($maskenfeldwerte['endedatum_wdh'], 8);
		if(!$maskenfeldwerte['wiederholung_anz'] || ($maskenfeldwerte['wiederholung_anz'] > 999))
			$maskenfeldwerte['wiederholung_anz'] = '';
		if(substr($maskenfeldwerte['endedatum_wdh'], 0, 8) == '99999999')
			$maskenfeldwerte['endedatum_wdh'] = '';

		// Prüfen, ob ganz- oder halbtägig
		$pdsql = "SELECT wochenarbeitstd, pause, arbeitstage, soll_beginn FROM madaten WHERE personen_id=".$_POST['Personen_ID']." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$pdata = db_values($pdsql);
		if(fxIsArray($pdata) && $pdata['arbeitstage'])
		{
			$tss = get_double($maskenfeldwerte['soll_st_tag']);
			if($tss > 0.00)
			{
				$sbd = $maskenfeldwerte['soll_beginn'];
				$asc = validate_entry(get_entry($sbd, 'zeit'), $ierr, 'zeitspanne');
				$ast = $tss*3600;
			}
			else
			{
				$sbd = $pdata['soll_beginn'];
				$asc = validate_entry(get_entry($sbd, 'zeit'), $ierr, 'zeitspanne');
				$ast = ($pdata['wochenarbeitstd']*3600)/$pdata['arbeitstage'];
			}
			// Halbtägige 1. Endzeit errechnen
			$ssc = $asc+($ast/2);
			$h = (int)($ssc/3600); $ssc = $ssc-$h*3600;
			$m = (int)($ssc/60);
			$s = $ssc-$m*60;
			$db_endzeit_1 = substr('0'.$h, -2).substr('0'.$m, -2).substr('0'.$s, -2);
			// Halbtägige 2. Startzeit errechnen
			$ssc = $asc+($ast/2)+$pdata['pause'];
			$h = (int)($ssc/3600); $ssc = $ssc-$h*3600;
			$m = (int)($ssc/60);
			$s = $ssc-$m*60;
			$db_startzeit_2 = substr('0'.$h, -2).substr('0'.$m, -2).substr('0'.$s, -2);
			// Halbtägige 2. Endzeit errechnen
			$ssc = $asc+$ast+$pdata['pause'];
			$h = (int)($ssc/3600); $ssc = $ssc-$h*3600;
			$m = (int)($ssc/60);
			$s = $ssc-$m*60;
			$db_endzeit_2 = substr('0'.$h, -2).substr('0'.$m, -2).substr('0'.$s, -2);
			if((substr($sbd, 8) == substr($maskenfeldwerte['terminstart'], 8)) && ($db_endzeit_1 == substr($maskenfeldwerte['terminende'], 8)))
				$maskenfeldwerte['ganztaegig'] = 2572;	// halbtägig (1. Hälfte)
			else if(($db_startzeit_2 == substr($maskenfeldwerte['terminstart'], 8)) && ($db_endzeit_2 == substr($maskenfeldwerte['terminende'], 8)))
				$maskenfeldwerte['ganztaegig'] = 2573;	// halbtägig (2. Hälfte)
			else if((substr($sbd, 8) == substr($maskenfeldwerte['terminstart'], 8)) && ($db_endzeit_2 == substr($maskenfeldwerte['terminende'], 8)))
				$maskenfeldwerte['ganztaegig'] = 2571;	// ganztägig
		}
	}

	if(!$maskenfeldwerte['ganztaegig'])
		$maskenfeldwerte['ganztaegig'] = 0;
}
else if($err && ($aktion == FXP_DELETE))
	$maskenfeldwerte = $mfw_backup;

// Einladungs-Termine nicht bearbeitbar
if(($aktion == FXP_CHANGE) && ($maskenfeldwerte['terminart'] == FXP_AT_INVITATION))
{
	$maske99_sf = 2;
	$Wiedervorlage_sf = 1;
}

// {ID}
$maskenfeldwerte['id'] = $Termin_ID;
// {MasterTermin_ID}
if(!$maskenfeldwerte['mastertermin_id'])
{
	if($MasterTermin_ID)
		$maskenfeldwerte['mastertermin_id']	= $MasterTermin_ID;
	else
		$maskenfeldwerte['mastertermin_id']	= $Termin_ID;
}

// Person:
if(isset($_GET['att']) && strlen($_GET['att']))	// Aufruf über "Kalender"
{
	$Options = explode('.', $_GET['att']);
	$up = strpos($Options[10], '_');
	if($up !== false)
		$Personen_ID = (int)substr($Options[10], $up+1);
	else
		$Personen_ID = (int)$Options[10];
	$_POST['Personen_ID'] = $Personen_ID;
}
else if((!isset($_POST['Personen_ID']) || isset($_GET['tr'])) && ($aktion == FXP_CREATE))
	$_POST['Personen_ID'] = $GLOBALS['fxpglobals']['person'];
if($aktion == FXP_CREATE)
	$Personen_ID_ds = 0;
$maskenfeldwerte['personen_id'] = $_POST['Personen_ID'];

// Terminart:
$Terminart_ds = 0;
if((!isset($_POST['Terminart']) || isset($_GET['tr'])) && ($aktion == FXP_CREATE))
	$maskenfeldwerte['terminart'] = FXP_AT_PRIVATE;
$Terminart_arg = array('tart'=>$tart);

// Gep. Startdatum:
if((!isset($_POST['Soll_Beg_Dtm']) || isset($_GET['tr'])) && ($aktion == FXP_CREATE))
	$maskenfeldwerte['soll_beg_dtm'] = $jetzt;

// Terminstart:
if((!isset($_POST['Terminstart']) || isset($_GET['tr'])) && ($aktion == FXP_CREATE))
{
	if(strlen($_GET['Zeitpunkt']) > 0)
		$jetzt = $_GET['Zeitpunkt'];
	$akth = (int)substr($jetzt, 8, 2);
	$aktm = (int)substr($jetzt, 10, 2);
	// Uhrzeit auf nächste halbe Stunde aufrunden wenn Minuten ungleich "0" oder "30"
	if($aktm < 30 && $aktm != 0)
		$aktm = 30;
	else if($aktm > 30)
	{
		$aktm = 0;
		$akth++;
		if($akth > 23)
			$akth = 0;
	}
	$maskenfeldwerte['terminstart'] = substr($jetzt, 0, 8).substr('00'.$akth, -2).substr('00'.$aktm, -2).'00';
}

// Widerholungstermin:
if(($maskenfeldwerte['wiederholung'] || ($err && ($dbwerte['wiederholung'] == 'NULL'))) && ($maskenfeldwerte['mastertermin_id'] != $Termin_ID))
{
	if($err)
		$maskenfeldwerte['bestaetigung_anmerk'] = $mfw_backup['bestaetigung_anmerk'];
	$maskenfeldwerte['wiederholung'] = '~<b class="blue">'.$maskenfeldwerte['bestaetigung_anmerk'].'</b>';
	$Endedatum_WDH_nv		= true;
	$wiederholung_anz_nv	= true;
	if($aktion != FXP_CREATE)
	{
		$Soll_Beg_Dtm_sf	= 1;
		$Terminart_sf		= 1;
	}
}

// Vorgang:
$Projekt_ID_arg = array('projects_only'=>true);
if(!$err)
{
	if($maskenfeldwerte['projekt_id'] > 0)
	{
		$sql = "SELECT projektart, elter, vorgangsnummer, name_projekt FROM projekte WHERE projekt_id=".$maskenfeldwerte['projekt_id'];
		$vga = db_values($sql);
		if(is_array($vga) && ($vga['projektart'] == FXP_PRJ_TASK) && $maskenfeldwerte['zeit_id'])
		{
			$eid = (int)$vga['elter'];
			if(($aktion == FXP_CREATE) || ($aktion == FXP_CHANGE))
				$maskenfeldwerte['projekt_id'] = $eid;
			else
			{
				$sql = "SELECT vorgangsnummer, name_projekt, projektart FROM projekte WHERE projekt_id=".$eid;
				$ega = db_values($sql);
				$maskenfeldwerte['projekt_id'] = '~<b>'.no_null($ega['vorgangsnummer']).'&nbsp;-&nbsp;'.$ega['name_projekt'].'</b>&nbsp;{';
				if($ega['projektart'] == FXP_PRJ_MAIN)
					$maskenfeldwerte['projekt_id'] .= '*';
				$maskenfeldwerte['projekt_id'] .= '*}<br>-&gt;&nbsp;<font class="grey">'.no_null($vga['vorgangsnummer']).'&nbsp;-&nbsp;'.$vga['name_projekt'].'</font>';
			}
		}
	}
	else if(($maskenfeldwerte['projekt_id'] < 0) && (($aktion == FXP_DISPLAY) || ($aktion == FXP_DELETE)))
		$maskenfeldwerte['projekt_id'] = '~<b>'.meldung(681, false).'</b>';
}

// Zeiterf.-Termin:
if($maskenfeldwerte['zeit_id'] || ($err && $_POST['terminpr']))
	$maskenfeldwerte['terminpr'] = 1;
else
	$maskenfeldwerte['terminpr'] = '';

// erledigt:
if(($aktion == FXP_CREATE) || ($aktion == FXP_DELETE))
	$erledigt_nv = true;
else
{
	if(!$maskenfeldwerte['erledigt'])
		$maskenfeldwerte['erledigt'] = '';
	else
		$maskenfeldwerte['erledigt'] = 1;
}

// Termininfos ausblenden
$dontshow = false;
// ja, falls kein eigener Termin und man nicht im Neuanlage-Modus ist
if(!$err && ($aktion != FXP_CREATE) && ($maskenfeldwerte['personen_id'] != $GLOBALS['fxpglobals']['person']))
{
	$dontshow = true;
	// ausser man hat Recht auf Ändern und es ist kein Privattermin
	if(($maskenfeldwerte['terminart'] != FXP_AT_PRIVATE) && ($maskenfeldwerte['masterterminart'] != FXP_AT_PRIVATE) && test_bit($gpa['par'][$maskenfeldwerte['personen_id']], FXP_CHANGE))
		$dontshow = false;
}
if($dontshow)
{
	$maskenfeldwerte['terminart']		= '~&nbsp;'.$notext.'&nbsp;';
	$maskenfeldwerte['beschreibung']	= '~&nbsp;'.$notext.'&nbsp;';
	$maskenfeldwerte['treffpunkt']		= '~&nbsp;'.$notext.'&nbsp;';
	$maskenfeldwerte['name_termin']		= '~&nbsp;'.$notext.'&nbsp;';
}

// Link
if(strlen($_backlink))
	$maskenfeldwerte['defaultaktion'] = $_backlink;
else
{
	$defaultaktion_nv	= true;
	$hr_nv				= true;
}

protectVar($_backlink, '_backlink');

// Buttonleiste
if($GLOBALS['fxpglobals']['action'] == FXP_DISPLAY)
	$Button_Multi_nv = true;
if($GLOBALS['fxpglobals']['action'] != FXP_CHANGE)
	$Button_Reset_nv	= true;
if(($GLOBALS['fxpglobals']['action'] == FXP_DISPLAY) || ($GLOBALS['fxpglobals']['action'] == FXP_DELETE))
	$Button_Check_nv	= true;

protectVar($maskenfeldwerte, 'mfw_backup');
?>