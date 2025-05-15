<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : termine_abw_pre.inc                                          //
// Version     : 24.1                                                         //
// Begin       : 2020-11-02                                                   //
// Last Change : 2024-03-14                                                   //
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
 * PF 222: Absence Planner
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

if($Termin_ID && !$err)
{
	$sql  = "SELECT personen_id, terminart AS fehltagsart, startzeit AS soll_beg_dtm, endzeit AS soll_end_dtm, soll_st_tag, soll_beginn,";
	$sql .= " beschreibung, bestaetigung_anmerk AS anmerkung, urlaub, stunden FROM termine WHERE termin_id=".$Termin_ID." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$maskenfeldwerte=db_values($sql);
//fxDebug($maskenfeldwerte, __LINE__.': '.$sql);

	$persid=$maskenfeldwerte['personen_id'];

	// Prüfen, ob ganz- oder halbtägig
	$tss=get_double($maskenfeldwerte['soll_st_tag']);
	$tsz=(int)substr((string)$maskenfeldwerte['soll_beg_dtm'], 8, 2)*3600 + (int)substr((string)$maskenfeldwerte['soll_beg_dtm'], 10, 2)*60 + (int)substr((string)$maskenfeldwerte['soll_beg_dtm'], 12, 2);
	$tez=(int)substr((string)$maskenfeldwerte['soll_end_dtm'], 8, 2)*3600 + (int)substr((string)$maskenfeldwerte['soll_end_dtm'], 10, 2)*60 + (int)substr((string)$maskenfeldwerte['soll_end_dtm'], 12, 2);
	$tdz=$tez-$tsz;
	if(substr((string)$maskenfeldwerte['soll_end_dtm'], 0, 8) > substr((string)$maskenfeldwerte['soll_beg_dtm'], 0, 8))
		$maskenfeldwerte['ganztaegig']=2571;	// ganztägig, da Termindauer > 1 Tag
	else if(($tss > 0.00) && ($tdz >= ($tss * 3600)))
		$maskenfeldwerte['ganztaegig']=2571;	// ganztägig, da Terminzeitraum >= Sollstunden pro Tag
	else if(strlen((string)$maskenfeldwerte['soll_beginn']))
	{
		if(substr((string)$maskenfeldwerte['soll_beginn'], 8, 6) == substr((string)$maskenfeldwerte['soll_beg_dtm'], 8, 6))
			$maskenfeldwerte['ganztaegig']=2572;	// halbtägig (1. Hälfte)
		else
			$maskenfeldwerte['ganztaegig']=2573;	// halbtägig (2. Hälfte)
	}
	else
	{
		$pdsql ="SELECT wochenarbeitstd, pause, arbeitstage, soll_beginn FROM madaten WHERE personen_id=".$persid;
		$pdata=db_values($pdsql);
//fxDebug($pdata, $pdsql);
		if(is_array($pdata) && $pdata['arbeitstage'])
		{
			$asc=validate_entry(get_entry($pdata['soll_beginn'], 'zeit'), $ierr, 'zeitspanne');
			$ast=($pdata['wochenarbeitstd']*3600)/$pdata['arbeitstage'];
			// Halbtägige Startzeit errechnen
			$ssc=$asc+($ast/2)+$pdata['pause'];
			$h=(int)($ssc/3600); $ssc=$ssc-$h*3600;
			$m=(int)($ssc/60);
			$s=$ssc-$m*60;
			$db_startzeit=substr('0'.$h, -2).substr('0'.$m, -2).substr('0'.$s, -2);
			if($db_startzeit == substr((string)$maskenfeldwerte['soll_beg_dtm'], 8))
				$maskenfeldwerte['ganztaegig']=2573;	// halbtägig (2. Hälfte)
			else if($db_startzeit >= substr((string)$maskenfeldwerte['soll_end_dtm'], 8))
				$maskenfeldwerte['ganztaegig']=2572;	// halbtägig (1. Hälfte)
		}
	}
	if(!$maskenfeldwerte['ganztaegig'])
		$maskenfeldwerte['ganztaegig']=2571;	// ganztägig
}
else if($err && ($aktion == FXP_DELETE))
{
	$maskenfeldwerte=$mfw_backup;
//fxDebug($maskenfeldwerte, __LINE__.': $mfw_backup');
}

// {Termin_ID}
$maskenfeldwerte['termin_id']		= $Termin_ID;
$maskenfeldwerte['mastertermin_id']	= $Termin_ID;

// Person:
$maskenfeldwerte['personen_id']=$persid;
if(isset($_GET['att']) && strlen((string)$_GET['att']))	// Aufruf über "Kalender"
{
	$Options=explode('.', $_GET['att']);
	$up=strpos($Options[10], '_');
	if($up !== false)
		$persid=(int)substr((string)$Options[10], $up+1);
	else
		$persid=(int)$Options[10];
	$maskenfeldwerte['personen_id']=$persid;
}
if($aktion == FXP_CREATE)
	$Personen_ID_ds=0;
if($aktion != FXP_CREATE)
	$Personen_ID_sf=-1;

// Abwesen.-Art:
$Fehltagsart_ds=0;
if((!isset($_POST['Fehltagsart']) || isset($_GET['tr'])) && ($aktion == FXP_CREATE))
{
	// Externe Mitarbeiter -> Abwesend (2879)
	if($GLOBALS['fxpglobals']['persdat']['personentyp'] == FXP_PT_CONTRACTOR)
		$maskenfeldwerte['fehltagsart']=2879;
	// Person mit erweiterten Befugnissen -> Urlaub (1039)
	else if($allrights)
		$maskenfeldwerte['fehltagsart']=1039;
	// Rest -> Urlaubsantrag (1178)
	else
		$maskenfeldwerte['fehltagsart']=1178;
}
$Fehltagsart_arg=array('tart'=>$tart);

// Startdatum:
if((!isset($_POST['Soll_Beg_Dtm']) || isset($_GET['tr'])) && ($aktion == FXP_CREATE))
{
	$maskenfeldwerte['soll_beg_dtm']=fxNow();
	$maskenfeldwerte['soll_end_dtm']='';
}

// ganztägig:
$ganztaegig_ds=0;

// Logikzuweisungen
if(($maskenfeldwerte['fehltagsart'] == FXP_AT_SLIDINGDAYREQUEST) || ($maskenfeldwerte['fehltagsart'] == FXP_AT_SLIDINGDAY))
{
	if(($aktion == FXP_CREATE) || ($aktion == FXP_CHANGE))
	{
		$maskenfeldwerte['ganztaegig']		= 2571;
		$maskenfeldwerte['stunden']			= '';

		if($aktion == FXP_CHANGE)
			$maskenfeldwerte['soll_end_dtm']='';
	}
	else
	{
		$ganztaegig_nv=true;
		$stunden_nv=true;

		if(substr((string)$maskenfeldwerte['soll_beg_dtm'], 0, 8) === substr((string)$maskenfeldwerte['soll_end_dtm'], 0, 8))
			$Soll_End_Dtm_nv=true;
	}
}
else if(($maskenfeldwerte['fehltagsart'] == FXP_AT_OVERTIMEOUTPAYREQUEST) || ($maskenfeldwerte['fehltagsart'] == FXP_AT_OVERTIMEOUTPAY))
{
	if(($aktion == FXP_CREATE) || ($aktion == FXP_CHANGE))
	{
		$maskenfeldwerte['soll_end_dtm']	= '';
		$maskenfeldwerte['ganztaegig']		= 2571;
	}
	else
	{
		$Soll_End_Dtm_lit='';
		$ganztaegig_lit=get_text(439,'15');
		$maskenfeldwerte['ganztaegig']		= '~';
		$maskenfeldwerte['soll_end_dtm']	= '~';
	}
}
else if(($aktion == FXP_CREATE) || ($aktion == FXP_CHANGE))
	$maskenfeldwerte['stunden']='';
else
	$stunden_nv=true;

// Anmerkung:
if($aktion == FXP_CREATE)
{
	$hr1_nv=true;
	$Anmerkung_nv=true;
}
else
{
	// Erstellt
	$anm='';
	$txt=trim((string)$maskenfeldwerte['anmerkung']);
	$aca=termin_ersteller(array('termin_id'=>$Termin_ID, 'terminart'=>$maskenfeldwerte['fehltagsart'], 'startzeit'=>$maskenfeldwerte['soll_beg_dtm'], 'anmerkung'=>$txt));
	if(fxIsArray($aca))
	{
//fxDebug($aca,$termin['termin_id']);
		$anm=$aca['text'];
	}
	// ff.
	if(strlen((string)$txt))
	{
		$aap='<img src='.$GLOBALS['gfxpath'].'tl_';
		if(substr((string)$txt,1,1) == '-')
		{
			if(substr((string)$txt,0,1) == '1')
				$aap .= 'green';
			else
				$aap .= 'red';
			$txt=substr((string)$txt,2);
		}
		else
			$aap .= 'yellow';
		$aap .= '.png>&nbsp;&nbsp;';
		$txt = $aap.$txt;
		if(strlen((string)$anm))
			$anm .= '<br />';
		$anm .= $txt;
	}
	if(strlen((string)$anm))
		$maskenfeldwerte['anmerkung']=$anm;
	else
	{
		$hr1_nv=true;
		$Anmerkung_nv=true;
	}
}

// Termininfos ausblenden
$dontshow=false;
// ja, falls kein eigener Termin
if($maskenfeldwerte['personen_id'] != $GLOBALS['fxpglobals']['person'])
{
	$dontshow=true;
	// ausser man hat Recht auf Ändern
	if(test_bit($gpa['par'][$maskenfeldwerte['personen_id']], FXP_CHANGE))
		$dontshow=false;
}
if($dontshow)
{
	$maskenfeldwerte['fehltagsart']		= '~'.$notext;
	$maskenfeldwerte['beschreibung']	= '~'.$notext;
	$maskenfeldwerte['name_termin']		= '~'.$notext;
	$maskenfeldwerte['anmerkung']		= '~'.$notext;
}

// Buttonleiste
if($aktion != FXP_CHANGE)
	$Button_Reset_nv=true;
if(($aktion == FXP_DISPLAY) && $Termin_ID)
{
	$GLOBALS['mail_attachments']=array();
	$ics_filename=iCalendar($Termin_ID);
	$ica=iCalendarCombine(1);
}

if(isset($mfw_backup['urlaub']))
	$maskenfeldwerte['urlaub']=$mfw_backup['urlaub'];

protectVar($maskenfeldwerte, 'mfw_backup');
?>