<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : termine_auswahl_ini.inc                                      //
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

$ptyp=(int)$GLOBALS['fxpglobals']['persdat']['personentyp'];

// Im Popupkalender die ausführliche Version anzeigen inkl. Sondertage etc.
echo('<div id="datepopup_quick" style="display:none;">0</div>');

// Normale Befugnisse
$allrights=0;
// Komplette Befugnisse haben: Systemadministrator und Geschäftsleitung
if(!$GLOBALS['fxpglobals']['person'] || $GLOBALS['fxpglobals']['persdat']['roles'][2])
	$allrights=2;
// Erweiterte Befugnisse haben:	Administratoren, Verwaltung bzw. Ansprechpartner
else if(($GLOBALS['fxpglobals']['user'] < 2) || $GLOBALS['fxpglobals']['useradmin'] || $GLOBALS['fxpglobals']['persdat']['roles'][-1] || ($ptyp == FXP_PT_CUSTOMER_CP) || ($ptyp == FXP_PT_CONTACT_CP) || ($ptyp == FXP_PT_PARTNER_CP))
	$allrights=1;
$GLOBALS['fxptdata']['allrights']=$allrights;
echo('<div id="allrights" style="display:none;">'.$allrights.'</div>');

// Einstellungswert, ob Ext. Mitarbeiter die gleichen Abwesenheitstage haben können wie Int. Mitarbeiter
$cont_like_emp=0;
if(strtolower(substr($GLOBALS['fxpglobals']['settings']['ext_darf_url'], 0, 1)) == 'j')
	$cont_like_emp=1;
$GLOBALS['fxptdata']['cont_like_emp']=$cont_like_emp;
echo('<div id="cont_like_emp" style="display:none;">'.$cont_like_emp.'</div>');

// Einstellungswert, ob Verwaltung auch Abwesenheitsanträge genehmigen darf von Personen, welche einen anderen Vorgesetzten haben
$ap_can_appr_all=0;
if(strtolower(substr($GLOBALS['fxpglobals']['settings']['vw_alle_gen'], 0, 1)) == 'j')
	$ap_can_appr_all=1;
$GLOBALS['fxptdata']['ap_can_appr_all']=$ap_can_appr_all;
echo('<div id="ap_can_appr_all" style="display:none;">'.$ap_can_appr_all.'</div>');

// Alle externen Mitarbeiter einlesen
$contractors_csv='';
$contractors_arr=array();
$sql="SELECT personen_id FROM personen WHERE mandanten_id=".$GLOBALS['fxpglobals']['client']." AND personentyp=".FXP_PT_CONTRACTOR;
$tmp=db_values($sql, '*');
if(fxIsArray($tmp))
{
	foreach($tmp as $t)
	{
		$contractors_arr[$t['personen_id']]=true;
		if(strlen($contractors_csv))
			$contractors_csv .= ',';
		$contractors_csv .= $t['personen_id'];
	}
}
$GLOBALS['fxptdata']['contractors']=$contractors_csv;
echo('<div id="contractors" style="display:none;">'.$contractors_csv.'</div>');

// Alle untergeordneten Personen einlesen
$elt_csv=$GLOBALS['fxpglobals']['person'];
// ...Elterdaten von Ansprechpartnern ermitteln: 620->276=Auftraggeber, 841->1372=Kontakt, 2092->2091=Lieferant
if(($ptyp == FXP_PT_CUSTOMER_CP) || ($ptyp == FXP_PT_CONTACT_CP) || ($ptyp == FXP_PT_PARTNER_CP))
	$elt_csv .= ",".(int)$GLOBALS['fxpglobals']['persdat']['elter'];
$subordinates_csv='';
$subordinates_arr=array();
$sql  = "SELECT personen_id FROM personen WHERE elter IN (".$elt_csv.")";
$sql .= " AND personentyp IN (".FXP_PT_EMPLOYEE.",".FXP_PT_CONTRACTOR.") AND personen_id>0 AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
$tmp=db_values($sql, '*');
//fxDebug($tmp, $sql);
if(fxIsArray($tmp))
{
	foreach($tmp as $t)
	{
		$subordinates_arr[$t['personen_id']]=true;
		if(strlen($subordinates_csv))
			$subordinates_csv .= ',';
		$subordinates_csv .= $t['personen_id'];
	}
}
$GLOBALS['fxptdata']['subordinates']=$subordinates_csv;
echo('<div id="subordinates" style="display:none;">'.$subordinates_csv.'</div>');

// Alle Personen einlesen, welche keinen Vorgesetzten haben
if(($ptyp == FXP_PT_CUSTOMER_CP) || ($ptyp == FXP_PT_CONTACT_CP) || ($ptyp == FXP_PT_PARTNER_CP))
	$mpid=0;
else
	$mpid=(int)db_value("SELECT personen_id FROM personen WHERE personentyp=".FXP_PT_CLIENT." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
$nosupervisors_csv='';
$nosupervisors_arr=array();
$sql  = "SELECT personen_id FROM personen WHERE (elter=".$mpid." OR elter=0 OR elter IS NULL)";
$sql .= " AND personentyp IN (".FXP_PT_EMPLOYEE.",".FXP_PT_CONTRACTOR.") AND personen_id>0 AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
$tmp=db_values($sql, '*');
//fxDebug($tmp, $sql);
if(fxIsArray($tmp))
{
	foreach($tmp as $t)
	{
		$nosupervisors_arr[$t['personen_id']]=true;
		if(strlen($nosupervisors_csv))
			$nosupervisors_csv .= ',';
		$nosupervisors_csv .= $t['personen_id'];
	}
}
$GLOBALS['fxptdata']['nosupervisors']=$nosupervisors_csv;
echo('<div id="nosupervisors" style="display:none;">'.$nosupervisors_csv.'</div>');

// Auswahlmaske immer bearbeitbar
$maske452_sf=true;

// Wer darf welche Terminarten bearbeiten?
$taar=array();
$tart='';
// Abwesenheitsplaner
if($GLOBALS['fxpglobals']['tr'] == 222)
{
	// Urlaubsantrag, Sonderurlaubsantrag, Krankmeldung + Schulungsantrag
	$taar[FXP_AT_VACATIONREQUEST]		= true;	// Urlaubsantrag				(1178)
	$taar[FXP_AT_SPECIALVACATIONREQUEST]	= true;	// Sonderurlaubsantrag			(2877)
	$taar[FXP_AT_SICKCALL]		= true;	// Krankmeldung					(2105)
	$taar[FXP_AT_TRAININGREQUEST]		= true;	// Schulungsantrag				(2106)
	$taar[FXP_AT_SLIDINGDAYREQUEST]		= true;	// Gleittagantrag				(1940)
	$taar[FXP_AT_OVERTIMEOUTPAYREQUEST]	= true;	// Überstundenauszahlungsantrag	(1942)

	$tart=FXP_AT_VACATIONREQUEST.','.FXP_AT_SPECIALVACATIONREQUEST.','.FXP_AT_SICKCALL.','.FXP_AT_TRAININGREQUEST.','.FXP_AT_SLIDINGDAYREQUEST.','.FXP_AT_OVERTIMEOUTPAYREQUEST;

	// Abwesend, Urlaub, Sonderurlaub, Krankheit + Schulung
	$taar[FXP_AT_ABSENT]		= true;	// Abwesend						(2879)
	$taar[FXP_AT_VACATION]			= true;	// Urlaub						(1039)
	$taar[FXP_AT_SPECIALVACATION]	= true;	// Sonderurlaub					(2878)
	$taar[FXP_AT_SICK]			= true;	// Krankheit					(1040)
	$taar[FXP_AT_TRAINING]		= true;	// Schulung						(1468)
	$taar[FXP_AT_SLIDINGDAY]		= true;	// Gleittagantrag				(1941)
	$taar[FXP_AT_OVERTIMEOUTPAY]	= true;	// Überstundenauszahlung		(1943)

	$tart .= ','.FXP_AT_ABSENT.','.FXP_AT_VACATION.','.FXP_AT_SPECIALVACATION.','.FXP_AT_SICK.','.FXP_AT_TRAINING.','.FXP_AT_SLIDINGDAY.','.FXP_AT_OVERTIMEOUTPAY;
}
// Terminplaner
else
{
	$taar[FXP_AT_INVITATION]		= true;	// Einladungen		(187)
	$taar[FXP_AT_MEETING]			= true;	// Meeting			(661)
	$taar[FXP_AT_CONFERENCE]		= true;	// Konferenz		(662)
	$taar[FXP_AT_BUSINESSDINNER]	= true;	// Geschäftsessen	(718)
	$taar[FXP_AT_CONTACT]			= true;	// Kontakt			(719)
	$taar[FXP_AT_PRIVATE]	= true;	// Privattermin		(720)
	$taar[FXP_AT_MESSAGE]		= true;	// Mitteilung		(971)
	$taar[FXP_AT_VARIOUS]	= true;	// Verschiedenes	(2297)

	$tart .= FXP_AT_INVITATION.','.FXP_AT_MEETING.','.FXP_AT_CONFERENCE.','.FXP_AT_BUSINESSDINNER.','.FXP_AT_CONTACT.','.FXP_AT_PRIVATE.','.FXP_AT_MESSAGE.','.FXP_AT_VARIOUS;
}
$absenttypes_csv=array_to_csv($taar);
$GLOBALS['fxptdata']['absenttypes']=$absenttypes_csv;
echo('<div id="absenttypes" style="display:none;">'.$absenttypes_csv.'</div>');

$notext='<i class="grey">***</i>';

// Falls auf "Prüfen" geklickt wurde
$checkit=false;
if($_POST['Button_Check'])
{
	$checkit=true;
	$_POST['Button_Multi']=true;
	unset($_POST['Button_Check']);
}

$spersid=0;

// Wurde auf ein Submit-Image geklickt?
$simg='';
$siid=0;
if(fxIsArray($_POST))
{
	$chks='akt'.$GLOBALS['fxpglobals']['action'].'_';
	$chkl=strlen($chks);
	foreach($_POST as $pkey => $pval)
	{
		if(substr($pkey,0,$chkl) == $chks)
		{
			$simg=substr($pkey, $chkl);
			break;
		}
	}
}
if(strlen($simg))
{
	$sie=explode('_', $simg);
	$spersid=(int)$sie[0];
	$siid=(int)$sie[1];

	$_POST['Termin_ID']=$siid;
	if($GLOBALS['fxpglobals']['action'] == FXP_DELETE)
		$_POST['Button_Multi']=true;
	else
		$_POST['Button_Submit']=true;
}

// Wurde die Liste bereits vorher angezeigt?
$lkey=-1;
if(fxIsArray($_POST['nv_text1']) && !$_POST['Button_Submit'])
{
	reset($_POST['nv_text1']);
	$lkey=key($_POST['nv_text1']);

	if(!$spersid)
	{
		$sie=explode('_', $_POST['nv_text1'][$lkey][0]);
		$spersid=(int)$sie[0];
	}
}

// Termin-Auswahlmaske anzeigen?
$stid=0;
$Termin_ID=0;
$disp_select=true;
if($GLOBALS['fxpglobals']['action'] == FXP_CREATE)
	$disp_select=false;
else
{
	if($spersid && ($GLOBALS['fxpglobals']['action'] == FXP_DELETE) && $_POST['Button_Multi'])
		$Termin_ID=-1;	// Dummy
	else if(isset($_GET['Termin_ID']) && $_GET['Termin_ID'])
	{
		$Termin_ID=(int)$_GET['Termin_ID'];
		$stid=$Termin_ID;
		$_GET['Personen_ID']=(int)db_value("SELECT personen_id FROM termine WHERE termin_id=".$stid." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
		$_POST=array();
		$spersid=0;
		$disp_select=false;
	}
	else if(isset($_POST['Termin_ID']))
	{
		$Termin_ID=(int)$_POST['Termin_ID'];
		$stid=$Termin_ID;
	}
	else if(fxIsArray($mfw_backup) && isset($mfw_backup['termin_id']))
		$Termin_ID=(int)$mfw_backup['termin_id'];
	else if(isset($_POST['ID']))
		$Termin_ID=(int)$_POST['ID'];
	else if(fxIsArray($mfw_backup) && isset($mfw_backup['id']))
		$Termin_ID=(int)$mfw_backup['id'];
	if(($Termin_ID >= 0) && isset($_POST['Termin_ID']) && (substr($_POST['Termin_ID'], 0, 2) == '_n'))
	{
		$spersid=(int)substr($_POST['Termin_ID'], 2);
		if(fxIsArray($mfw_backup))
		{
			if(isset($mfw_backup['termin_id']))
				$Termin_ID=(int)$mfw_backup['termin_id'];
			else if(isset($mfw_backup['id']))
				$Termin_ID=(int)$mfw_backup['id'];
		}
//fxDebug($mfw_backup, '$mfw_backup');
	}
}
$GLOBALS['fxptdata']['terminid']=$Termin_ID;

if(!$siid && $stid)
	$siid=$stid;
if(!$siid && $_POST['Button_Multi'] && ($Termin_ID > 0))
{
	$siid=$Termin_ID;
	$stid=$Termin_ID;
}

// Bestandsdaten
$avt=0;
if($Termin_ID > 0)
{
	$sql="SELECT * FROM termine WHERE termin_id=".$Termin_ID." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$old=db_values($sql);
	if(fxIsArray($old))
		$avt=$old['terminart'];
}
else
	$old=false;
fxDebug($old, $sql, 3);
echo('<div id="avt" style="display:none;">'.$avt.'</div>');
$GLOBALS['fxptdata']['avt']=$avt;

// Personen-ID
$persid=$GLOBALS['fxpglobals']['person'];
if($spersid > 0)
	$persid=$spersid;
else if(fxIsArray($old))
	$persid=(int)$old['personen_id'];
else if(isset($_GET['att']) && strlen($_GET['att']))	// Aufruf über "Kalender"
{
	$Options=explode('.', $_GET['att']);
	$up=strpos($Options[10], '_');
	if($up !== false)
		$persid=(int)substr($Options[10], $up+1);
	else
		$persid=(int)$Options[10];
}
else if($_GET['Personen_ID'])
	$persid=(int)$_GET['Personen_ID'];
else if($_POST['Personen_ID'])
	$persid=(int)$_POST['Personen_ID'];
else if($dbwerte['personen_id'])
	$persid=(int)$dbwerte['personen_id'];
else if($mfw_backup['personen_id'] && ($GLOBALS['fxpglobals']['action'] != FXP_CREATE))
	$persid=(int)$mfw_backup['personen_id'];

// Auswahlfeld-Argument füllen
$tid='';
if($spersid)
	$tid='_n'.$spersid;
if(!strlen($tid))
	$tid=$Termin_ID;
if(!strlen($tid))
	$tid='_n'.$GLOBALS['fxpglobals']['person'];
//echo('<b>$tid</b>='.$tid.'<br>');
$maskenfeldwerte['termin_id']=$tid;

$gpa=get_gpa($GLOBALS['fxpglobals']['person'], true, true, $tart);

$Termin_ID_ds=0;
$Termin_ID_arg=array('tart'=>$tart, 'gpa'=>$gpa);
$Termin_Arr=auswahl_Termin_ID(1);
//fxDebug($Termin_Arr,'$Termin_Arr', 0);

// Prüfen ob Person Rechte für diesen Termin hat
if(($Termin_ID > 0) && ((substr($Termin_ID, 0, 1) == '_') || !$Termin_Arr[$Termin_ID]))
	$Termin_ID=0;

// Mastertermin-ID
if($Termin_ID <= 0)
	$MasterTermin_ID=0;
else
	$MasterTermin_ID=db_value("SELECT mastertermin_id FROM termine WHERE termin_id=".$Termin_ID." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);

// Zeiterfassungstermin
if($_POST['terminpr'])
{
	if(fxIsArray($old) && ($old['zeit_id'] > 0))
		$zeit_id=(int)$old['zeit_id'];
	else
		$zeit_id=-1;
}
else
	$zeit_id=0;

// Vorgang
if($_POST['Projekt_ID'])
	$sel_projekt_id=(int)$_POST['Projekt_ID'];
else if($zeit_id != 0)
{
	if(fxIsArray($old) && ($old['projekt_id'] > 0))
		$sel_projekt_id=(int)$old['projekt_id'];
	else
		$sel_projekt_id=-1;
}
else
	$sel_projekt_id=0;

// Klick auf "Auswählen"-Button simulieren
if($Termin_ID && ($_GET['aktion'] || $_GET['start']))
{
	$_POST['Button_Submit']	= true;
	$Button_Submit			= true;
}

// Wurde auf "Auswählen" oder "Prüfen" geklickt, dann keine Validierung bzw. DB-Zugriffe durchführen
if($_POST['Button_Submit'] || $_POST['Button_Check'])
	$db_update=false;

// Terminarten
$tarten=array();
$tarray=db_values("SELECT tabwert, wertetabid FROM wertetab WHERE id_feld=434 AND id_sprache=".$GLOBALS['fxpglobals']['lang']." AND mandanten_id IN (0,".$GLOBALS['fxpglobals']['client'].")", '*');
if(fxIsArray($tarray))
{
	foreach($tarray as $ttext)
		$tarten[$ttext['wertetabid']]=$ttext['tabwert'];
}
//fxDebug($tarten,'$tarten');

// Wurde Datum übermittelt aus Kalender
if($_GET['Zeitpunkt'])
{
	$_POST['Soll_Beg_Dtm']=get_entry($_GET['Zeitpunkt'], 'datum');

	$maskenfeldwerte['soll_beg_dtm']=$_GET['Zeitpunkt'];
	$maskenfeldwerte['soll_end_dtm']='';
}

// Prüfen, für welche Jahre bereits Feiertage eingetragen wurden
$valid_years=array();
$sub="substring";
$sql="SELECT ".$sub."(datum, 1, 4) AS datum FROM feiertage_mand WHERE mandanten_id=".$GLOBALS['fxpglobals']['client']." GROUP BY ".$sub."(datum, 1, 4) ORDER BY datum";
$tmp=db_values($sql, '*');
//fxDebug($tmp, $sql);
if(fxIsArray($tmp))
{
	foreach($tmp as $t)
		$valid_years[(int)$t['datum']]=true;
}
//fxDebug($valid_years, '$valid_years');

// Warnmeldungen bei Transaktionsaufruf 222: "Zeitgenehmigungsverfahren"
if(!$err && ($_GET['tr'] == 222) && (($GLOBALS['fxpglobals']['action'] == FXP_CREATE) || ($GLOBALS['fxpglobals']['action'] == FXP_CHANGE)))
{
	$today=substr(fxNow(), 0, 8);

	// 2801: Es können noch keine Termine für das Jahr $var1 erfasst werden, da für dieses Jahr noch keine Feiertage hinterlegt wurden! Bitte legen Sie zuerst Feiertage an oder wenden Sie sich an Ihren Administrator.
	$year=(int)substr($today, 0, 4);
	if(!isset($valid_years[$year]))
	{
		error_msg(2801, false, '<b>'.$year.'</b>');
		$err=true;
	}

	if(!$err)
	{
		$month=(int)substr($today, 4, 2);
		if(!isset($valid_years[$year+1]) && ($month > 9))
		{
			error_msg(2801, false, '<b>'.($year+1).'</b>');
			$err=true;
		}
	}

	if($err)
		$db_update=false;
}

//echo('<b>$Termin_ID</b>='.$Termin_ID.', <b>$stid</b>='.$stid.', <b>$spersid</b>='.$spersid.', <b>$persid</b>='.$persid.', <b>$siid</b>='.$siid.', <b>$simg</b>='.$simg.'<br>');
// Termineingabe anzeigen?
$disp_input=false;
if((($stid > 0) && (!$spersid || $GLOBALS['fxpglobals']['action'] != FXP_DELETE)) || ($GLOBALS['fxpglobals']['action'] == FXP_CREATE))
	$disp_input=true;

// Einladungsliste anzeigen?
$msk112_simplemessage=true;
$disp_invitation=false;
if(($GLOBALS['fxpglobals']['tr'] == 27) && $disp_input)
	$disp_invitation=true;

// Terminliste anzeigen?
$msk541_simplemessage=true;
$disp_list=false;
if($spersid && ($GLOBALS['fxpglobals']['action'] != FXP_CREATE))
	$disp_list=true;
?>