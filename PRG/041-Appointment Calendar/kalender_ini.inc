<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : kalender_ini.inc                                             //
// Version     : 21.2                                                         //
// Begin       : 2020-11-25                                                   //
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
 * PF 41: Appointment Calendar
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

//fxDebug('_GET'); fxDebug('_POST');

// Calendar view definitions
define('CV_D1',	693);	// one day
define('CV_D5',	694);	// five days
define('CV_D7',	695);	// seven days
define('CV_M1',	696);	// one month

// Selection mask
$maske106_sf=true;
$cpo41=true;
$err=false;

// ...Convert GET to POST
if(fxIsArray($_GET) && isset($_GET['ndate']) && strlen($_GET['ndate']))
{
	if(fxIsArray($_POST) && isset($_POST['Button_heute']))
		unset($_POST['Button_heute']);
	$_POST['Button_Anzeigen']=true;
	$_POST['Datumsangabe']=get_entry($_GET['ndate'],'date');
	if(isset($_GET['ncv']) && strlen($_GET['ncv']))
		$_POST['Ansicht']=(int)$_GET['ncv'];
	$_POST['KW']='';
	extract($_POST);
}

// ...Date Entry + CW
$adt='';
if(fxIsArray($_POST) && !isset($_POST['Button_heute'])  && isset($_POST['Datumsangabe']) && strlen($_POST['Datumsangabe']))
{
	// Validate date
	$adt=validate_entry($_POST['Datumsangabe'], $err_date, 'datum');
	if($err_date)
	{
		if(fxIsArray($_GET) && isset($_GET['tr']))
			$adt='';
		else
		{
			$err=true;
			error_msg($err_date, false, 'Datumsangabe');
		}
	}
	else
		$adt=substr($adt,0,8);
}
if(!$err && !strlen($adt))
	$adt=$GLOBALS['date'];	// today

// ...CW
$acw=0;
if(fxIsArray($_POST) && !isset($_POST['Button_heute']) && isset($_POST['KW']) && strlen($_POST['KW']))
{
	// Validate integer
	$acw=validate_entry($_POST['KW'], $err_cw, 'integer', '-1');
	if($err_cw)
	{
		if(fxIsArray($_GET) && isset($_GET['tr']))
			$acw=0;
		else
		{
			$err=true;
			error_msg($err_cw, false, 'KW');
		}
	}
	else
		$acw=(int)$acw;
}
if(!$err)
{
	if($acw <= 0)
		$acw=kw($adt);
	else
		$acw=min($acw, kw(substr($adt,0,4).'1231'));
}

// Date has changed, then change cw  or  CW has changed, then change date
if(!$err && fxIsArray($GLOBALS['fxptdata']['backup']))
{
	if(($GLOBALS['fxptdata']['backup']['adt'] != $adt) || (fxIsArray($_POST) && isset($_POST['Button_heute'])))
		$acw=kw($adt);
	else if($GLOBALS['fxptdata']['backup']['acw'] != $acw)
	{
		$fwd=1;
		if(fxIsArray($GLOBALS['fxpglobals']['settings']) && isset($GLOBALS['fxpglobals']['settings']['ersterwt']))
			$fwd=(int)$GLOBALS['fxpglobals']['settings']['ersterwt'];
		$adt=date('Ymd', strtotime(substr($adt,0,4).'W'.substr('0'.$acw,-2).$fwd));
	}
}

$maskenfeldwerte['datumsangabe']=$adt;
$maskenfeldwerte['kw']=$acw;
//fxDebug(array('$adt'=>$adt, '$acw'=>$acw),'Values', 0);

// ...View
$cv=CV_D5;	// five days
if(fxIsArray($_POST) && isset($_POST['Ansicht']))
{
	$cv=(int)$_POST['Ansicht'];
	if(($cv != CV_D1) && ($cv != CV_D5) && ($cv != CV_D7) && ($cv != CV_M1))
		$cv=CV_D5;
}
$Ansicht_ds=0;
$maskenfeldwerte['ansicht']=$cv;

// ...Employee Appointment
$pid=0;
if((strtolower(substr($GLOBALS['fxpglobals']['settings']['alle_termine'], 0, 1)) == 'n') || !$GLOBALS['fxpglobals']['persdat']['role'])
{
	$Mitarbeitertermine_nv=true;
	$pid=$GLOBALS['fxpglobals']['person'];
}
else
{
	$Mitarbeitertermine_ds=meldung(1422);
	if(fxIsArray($_POST) && isset($_POST['Mitarbeitertermine']))
		$pid=(int)$_POST['Mitarbeitertermine'];
	else if(fxIsArray($GLOBALS['fxptdata']['backup']) && isset($GLOBALS['fxptdata']['backup']['pid']))
		$pid=(int)$GLOBALS['fxptdata']['backup']['pid'];
}
$maskenfeldwerte['mitarbeitertermine']=$pid;
$prsid=$pid;
if(!$pid)
	$prsid=$GLOBALS['fxpglobals']['person'];

// ...Checkboxes
$get_apps=false;
$cba=array('prj'=>'AuswahlProjekte', 'tsk'=>'AuswahlAufgaben', 'apb'=>'Termine_geschaeftl', 'app'=>'Termine_privat', 'abl'=>'Urlaub', 'abt'=>'Schulung', 'abs'=>'Krank', 'abf'=>'Gleitzeit');
foreach($cba as $cbn => $cbf)
{
	${'cb_'.$cbn}=0;
	if(fxIsArray($_POST) && isset($_POST[$cbf]) && $_POST[$cbf])
	{
		${'cb_'.$cbn}=1;
		$maskenfeldwerte[strtolower($cbf)]=1;
		if(($cbn != 'prj') && ($cbn != 'tsk'))
			$get_apps=true;
	}
}

// Backup values
if(!$err)
	$GLOBALS['fxptdata']['backup']=array('adt'=>$adt, 'acw'=>$acw, 'pid'=>$pid);

// Display selection mask
echo(maske(106));
$maske106_nv=true;


// Display calendar?
if(!$err && fxIsArray($_POST) && (isset($_POST['Button_Anzeigen']) || isset($_POST['Button_heute'])))
{
	// Texte aus DB die immer benötigt werden
	$meldung_43=meldung(43, true);		// Vorgänge
	$meldung_68=meldung(68, true);		// Neuer Termin
	$meldung_69=meldung(69, true);		// Neue Abwesenheit
	$meldung_793=meldung(793, true);	// Diese Woche anzeigen
	$meldung_1036=meldung(1036, true);	// KW
	$meldung_1917=meldung(1917, true);	// Neue Zeiterfassung

	$text_38=get_text(38, '15');		// Beschr.
	$text_349=get_text(349, '10');		// Ansprechp.
	$text_434=get_text(434, '15');		// Terminart
	$text_541=get_text(541, '15');		// Treffpunkt
	$text_1092=get_text(1092, '15');	// Betreff
	$text_1325=get_text(1325, '15');	// Zeitraum
	$text_1370=get_text(1370, '15');	// Anmerkung

	$start_text=get_text(130, '5');		// (Start)
	$ende_text=get_text(76, '5');		// (Ende)

	$nav_3018=explode('|', meldung(3018));	// Previous Week|Next Week|Back|Forward|Previous|Next

	// Alle Grafiken in ein Array speichern -> Code übersichtlicher
	$TerminGrafiken=array
	(
		FXP_AT_REMINDER => 'c_rem',
		FXP_AT_INVITATION => 'c_inv',
		FXP_AT_MEETING => 'c_meet',
		FXP_AT_CONFERENCE => 'c_conf',
		FXP_AT_BUSINESSDINNER => 'c_eat',
		FXP_AT_CONTACT => 'c_cont',
		FXP_AT_MESSAGE => 'c_msg',
		FXP_AT_PROJECTMESSAGE => 'c_pmsg',
		FXP_AT_PRIVATE => 'c_priv',
		FXP_AT_TODO => 'c_todo',

		FXP_AT_ABSENT => 'c_abs',
		FXP_AT_VACATION => 'c_hday',
		FXP_AT_VACATIONREQUEST => 'c_hday',
		FXP_AT_SPECIALVACATION => 'c_shday',
		FXP_AT_SPECIALVACATIONREQUEST => 'c_shday',
		FXP_AT_SICK => 'c_sick',
		FXP_AT_SICKCALL => 'c_sick',
		FXP_AT_TRAINING => 'c_trn',
		FXP_AT_TRAININGREQUEST => 'c_trn',
		FXP_AT_SLIDINGDAY => 'c_flex',
		FXP_AT_SLIDINGDAYREQUEST => 'c_flex',
		FXP_AT_OVERTIMEOUTPAY => 'c_otpay',
		FXP_AT_OVERTIMEOUTPAYREQUEST => 'c_otpay'
	);

	// Fehltagsarrays
	$fehltagsantrag	= array(						FXP_AT_VACATIONREQUEST=>true,	FXP_AT_SPECIALVACATIONREQUEST=>true,	FXP_AT_SICKCALL=>true,	FXP_AT_TRAININGREQUEST=>true,		FXP_AT_SLIDINGDAYREQUEST=>true);
	$fehltag		= array(FXP_AT_ABSENT=>true,	FXP_AT_VACATION=>true,			FXP_AT_SPECIALVACATION=>true,		FXP_AT_SICK=>true,			FXP_AT_TRAINING=>true,			FXP_AT_SLIDINGDAY=>true);
	$fehltage=$fehltag;
	foreach($fehltagsantrag as $ftid => $dmy)
		$fehltage[$ftid]=true;

	// Calendar variables: startdate, enddate, days per line and width per day
	switch($cv)
	{
		case CV_D1:	// one day
			$sdt=$adt;
			$edt=$adt;
			$days=1;
		break;

		case CV_D5:	// five days = monday to friday
			$sdt=getFirstWeekday($adt);
			$d=getWeekday($sdt);
			if(!$d || ($d > 6))	// If "Sunday" we need next day
				$sdt=time_add($sdt, 1, 'D');
			$edt=time_add($sdt, 4, 'D');
			$days=5;
		break;

		case CV_D7:	// seven days = week
			$sdt=getFirstWeekday($adt);
			$edt=getLastWeekday($adt);
			$days=7;
		break;

		case CV_M1:	// one month
			$sdt=getFirstWeekday(substr($adt,0,6).'01');
			$edt=getLastWeekday(ultimo($adt,false));
			$days=fxp_date_diff($sdt,$edt);
		break;
	}
	if($cv == CV_M1)
	{
		$rsdt=substr($adt,0,6).'01';
		$redt=ultimo($adt,false);
		$wd=170;
		if($pid)
			$wd=200;
		$width=floor(($GLOBALS['fxpglobals']['swidth']-$wd)/7);
	}
	else
	{
		$rsdt=$sdt;
		$redt=$edt;
		$wd=150;
		if($cv != CV_D1)
		{
			if($pid || ($cv == CV_D7))
				$wd=180;
			else
				$wd=170;
		}
		$width=floor(($GLOBALS['fxpglobals']['swidth']-$wd)/$days);
	}

fxDebug(array('$rsdt'=>$rsdt, '$redt'=>$redt, '$sdt'=>$sdt, '$edt'=>$edt, '$days'=>$days, '$width'=>$width),'Calendar variables', 2);

	$datearr=array();
	for($d=1; $d<=$days; $d++)
		$datearr[$d]=time_add($sdt,$d-1,'D');
//fxDebug($datearr,'$datearr', 0);

	$enddays=$days-1;
	$date_END=time_add($datearr[1], $enddays, 'D');

	$Feiertag=get_feiertage($datearr[1], $date_END);
//fxDebug($Feiertag,'$Feiertag', 0);

	$fomatdate=formatdatetime($GLOBALS['fxpglobals']['settings']['formatdatum'], $datearr[1]);

	$lttj='<b>'.meldung(1697,false).'</b>: ';	// Sprung zu

	// Transaktionsrechteprüfung
	$lnk_width=0;
	// Recht auf: Terminplaner (27)
	$lnk_27='';
	if(tr_rechte(27))
	{
		$lnk_27=link_img($GLOBALS['gfxpath'].'ktp.png', $meldung_68, 16, 16);
		$lnk_width +=16;
	}
	$ltt_27=$lttj.trPath(27);
	// Recht auf: ZE (TAG) (81)
	$lnk_81='';
	if(tr_rechte(81) && $pid && (($pid == $GLOBALS['fxpglobals']['person']) || ($cv != CV_M1)))
	{
		$lnk_81=$meldung_1917;
		$lnk_width +=16;
	}
	$ltt_81=$lttj.trPath(81);
	// Recht auf: ZE (SUMME) (34)
	$lnk_34='';
	if(tr_rechte(34) && $pid && (($pid == $GLOBALS['fxpglobals']['person']) || ($cv != CV_M1)))
	{
		$lnk_34=$meldung_1917;
		if(!strlen($lnk_81))
			$lnk_width +=16;
	}
	$ltt_34=$lttj.trPath(34);
	// Recht auf: RK (150)
	$lnk_150='';
	if(tr_rechte(150) && $pid && (($pid == $GLOBALS['fxpglobals']['person']) || ($cv != CV_M1)))
		$lnk_150=$meldung_1917;
	$ltt_150=$lttj.trPath(150);
	// Recht auf: Abwesenheitsplaner (222)
	$lnk_222='';
	if(tr_rechte(222))
	{
		$lnk_222=link_img($GLOBALS['gfxpath'].'kap.png', $meldung_69, 16, 16);
		$lnk_width +=16;
	}
	$ltt_222=$lttj.trPath(222);

	// Hier werden die Wochentage ermittelt, für die Tagesansicht
	$W=1;
	if($cv == CV_D1)
	{
		$maxzeichen=200;
		if($DBDatumsangabe)
		{
			$Tag=$weekday['wday'];
			$datearr[1]=fx_date("YmdHis", $DBDatumsangabe);
		}
		else
		{
			$dayofweek=fx_getdate(time());
			$Tag=$dayofweek['wday'];
		}
	}
	else
		$maxzeichen=30;

	// Calendar mask header with navigation arrows
//fxDebug($nav_3018,'$nav_3018', 0);
	$msktitle=trim($GLOBALS['fx_mfields']['mentry']);
	if(strlen($msktitle))
		$msktitle .= ':&nbsp;';
	$msktitle .= get_entry($rsdt,'datum');
	if($redt != $rsdt)
		$msktitle .= '&nbsp;-&nbsp;'.get_entry($redt,'datum');
	$msktitle .= '<div style="position:absolute;top:4px;right:12px;">';
	// ...tooltips
	if($cv == CV_D1)
	{
		$ldt=time_add($sdt, -1, 'D');
		$rdt=time_add($sdt, 1, 'D');
		$ttl=$nav_3018[2];	// Back
		$ttr=$nav_3018[3];	// Forward
		$ttdl=get_entry($ldt,'date');
		$ttdr=get_entry($rdt,'date');
	}
	else if($cv == CV_M1)
	{
		$ldt=time_add(substr($adt,0,6).'01', -1, 'M');
		$rdt=time_add(substr($adt,0,6).'01', 1, 'M');
		$ttl=$nav_3018[4];	// Previous
		$ttr=$nav_3018[5];	// Next
		$ttdl=get_entry($ldt,'date').' - '.get_entry(ultimo($ldt,false),'date');
		$ttdr=get_entry($rdt,'date').' - '.get_entry(ultimo($rdt,false),'date');
	}
	else
	{
		$ldt=time_add($sdt, -7, 'D');
		$rdt=time_add($sdt, 7, 'D');
		$ttl=$nav_3018[0];	// Previous Week
		$ttr=$nav_3018[1];	// Next Week
		$ttdl=get_entry($ldt,'date').' - '.get_entry(time_add($ldt,$days-1,'D'),'date');
		$ttdr=get_entry($rdt,'date').' - '.get_entry(time_add($rdt,$days-1,'D'),'date');
	}
	// ...Back/Previous
	$onclick=' '.fxf_jsFunction('loadTR', '>1', '^loading', $GLOBALS['fxpglobals']['tr'], 'navprev&form=_fxform_selmask_1&keep_get=1&ndate='.$ldt);
	$msktitle .= '<div class="btn" style="float:left;font-size:18px;" tooltip="<b>'.$ttl.'</b>: '.$ttdl.'"'.$onclick.'>&larr;</div>';
	// ...Next/Forward
	$onclick=' '.fxf_jsFunction('loadTR', '>1', '^loading', $GLOBALS['fxpglobals']['tr'], 'navnext&form=_fxform_selmask_1&keep_get=1&ndate='.$rdt);
	$msktitle .= '<div class="btn" style="float:left;margin-left:4px;font-size:18px;" tooltip="<b>'.$ttr.'</b>: '.$ttdr.'"'.$onclick.'>&rarr;</div>';
	$msktitle .= '</div>';
	msk_window($msktitle);

	// Kalenderdisplay
	echo('<div id="scheduler" style="border:1px solid #888888;border-left:0;box-shadow:2px 2px 8px #888888;">'.$nl);
	echo('<table border="0" cellpadding="0" cellspacing="0">'.$nl);

	// $Art übernimmt, jenachdem was der User sehen will, die Werte, die private oder geschäftliche
	// Termine spezifizieren. Dieser Teil findet auch weiter unten, in einigen SQL-Anweisungen nochmals Verwendung
	// - für keinen; Terminart wird mit einem UNMÖGLICHEN Wert gesucht --> es gibt KEINE Treffer
	$gesch='';
	$priv='';
	$url='';
	$schul='';
	$kra='';
	$gleit='';
	$sonst='';
	$acat=appcategory();
	if(fxIsArray($acat))
	{
		if($Termine_geschaeftl)
			$gesch="OR terminart IN (".$acat[FXP_AC_BUSINESS]['csv'].") ";
		if($Termine_privat)
			$priv="OR terminart IN (".$acat[FXP_AC_PRIVATE]['csv'].") ";
		if($Abwesnd)
			$url="OR terminart IN (".FXP_AT_ABSENT.") ";
		if($Urlaub)
			$url="OR terminart IN (".FXP_AT_VACATIONREQUEST.",".FXP_AT_VACATION.",".FXP_AT_SPECIALVACATIONREQUEST.",".FXP_AT_SPECIALVACATION.") ";
		if($Schulung)
			$schul="OR terminart IN (".FXP_AT_TRAININGREQUEST.",".FXP_AT_TRAINING.") ";
		if($Krank)
			$kra="OR terminart IN (".FXP_AT_SICKCALL.",".FXP_AT_SICK.") ";
		if($Gleitzeit)
			$gleit="OR terminart IN (".FXP_AT_SLIDINGDAYREQUEST.",".FXP_AT_SLIDINGDAY.") ";
		$sonst="OR terminart IN (".$acat[FXP_AC_VARIOUS]['csv'].") ";
	}
	else
		$sonst="OR terminart IN (".FXP_AT_REMINDER.",".FXP_AT_INVITATION.",".FXP_AT_VARIOUS.") ";

	// Erinnerung (180), Einladung (187) + Verschiedenes (2297) immer anzeigen
	$selected_types=substr($gesch.$priv.$url.$schul.$kra.$gleit.$sonst, 3);
	$Art=" AND (".$selected_types.")";

	// Spaltenüberschrift - Uhrzeit
	$hstyle='background:#cccccc;font-weight:bold;color:#444444;border-left:1px solid #aaaaaa;border-bottom:1px solid #aaaaaa;box-shadow:4px 4px 8px #ffffff inset;padding:4px;';
	if($cv != CV_M1)
	{
		echo('	<tr>'.$nl);
		echo('		<td align="center" nowrap style="'.$hstyle.'">Zeit</td>'.$nl);	// 1.Spalte
	}

	$Wochentage=fxf_getWeekdayNames();
	if(fxIsArray($Wochentage))
	{
		for($wtc=0; $wtc<8; $wtc++)
		{
			$nwtc=$wtc;
			if($nwtc > 6)
				$nwtc -=7;
			if($cv == CV_D1)
				$Wochentag[$wtc]=$Wochentage[$nwtc].',';
			else
				$Wochentag[$wtc]=substr($Wochentage[$nwtc], 0, 2).',';
		}
	}

	// Personeninfos des Users ermitteln
	if($pid)
		$pinfo=get_persondata($pid);
	// User=(Sys-)Admin => fixe Kalenderanzeige
	else
		$pinfo=array(0=>1,1,1,1,1,1,1, 'soll_beginn'=>"00000000000000", 'sollstunden_tag'=>23.5, 'pause'=>0);

	// Hier wird das jeweilige Datum, das schon zuvor in das Array $datearr geschrieben wurde
	// in die dafür vorgesehene Spalte geschrieben
	$akt_tag_ini=-1;
	for($d=1; $d<=$days; $d++)
	{
		$datearr[$d]=formatdatetime($GLOBALS['fxpglobals']['settings']['formatdatum'], $datearr[$d]);
		$Tag=zeitsek(strtodatetime($GLOBALS['fxpglobals']['settings']['formatdatum'], $datearr[$d]));
		$TAG_arr=fx_getdate($Tag);
		if($akt_tag_ini < 0)
			$akt_tag_ini=$TAG_arr['wday'];
		if($TAG_arr['wday'])
			$day=$Wochentag[$TAG_arr['wday']];
		else
			$day=$Wochentag[7];

		// Der Feiertagname wird zum Datum geschrieben
		$Feiertagname='';
		$cclass='black';
		if(sizeof($Feiertag))
		{
			$db_day=strtodatetime($GLOBALS['fxpglobals']['settings']['formatdatum'], $datearr[$d]);
			if($Feiertag[substr($db_day,0,8)])
			{
				$cclass='blue';

				$Feiertagname=$Feiertag[substr($db_day, 0, 8)]['name'];
				if($Feiertag[substr($db_day, 0, 8)]['art'] == FXP_HL_HALF)
					$Feiertagname .='&nbsp;<sup>1/2</sup>';
				$Feiertagname='<br /><i class="'.$cclass.'">'.$Feiertagname.'</i>';
			}
		}
		$onclick='';
		if(($cv != CV_M1) && ($cv != CV_D1))	// mit Link zum Einzeltag
		{
			$onclick=' '.fxf_jsFunction('loadTR', '>1', '^loading', $GLOBALS['fxpglobals']['tr'], 'navday&form=_fxform_selmask_1&keep_get=1&ndate='.$datearr[$d].'&ncv='.CV_D1);
			if($cv != CV_M1)
				$hstyle .= 'cursor:pointer;';
		}
		if($cv != CV_M1)
			echo('		<td colspan="4" width="'.$width.'" align="center" valign="top" nowrap style="'.$hstyle.'"'.$onclick.'><font class="'.$cclass.' bold">'.$day.'&nbsp;'.$datearr[$d].'</font>'.$Feiertagname.'</td>'.$nl);
	}

	// Dieselbe Schleifen wie oben wird hier nochmal durchlaufen, dies ist nötig, da in der
	// obigen Schleife das Datum für die 7-Tage berechnet wird, und hier schon alle 7 Daten
	// benötigt werden, wäre das in 1 Schleife, wäre das nicht möglich
	// Zuerst muss das Datum ins DB-Format gebracht werden
	for($d=1; $d<=$days; $d++)
		$datearr[$d]=strtodatetime($GLOBALS['fxpglobals']['settings']['formatdatum'], $datearr[$d]);

	if($cv != CV_M1)
		echo('	</tr>'.$nl);

	$tistyle=' style="background:#ffffff;font-weight:normal;color:#000000;border-left:1px solid #aaaaaa;box-shadow:-4px -4px 8px #dfdfdf inset;padding:4px;"';
	$tostyle=' style="background:#eeeeee;font-weight:normal;color:#888888;border-left:1px solid #aaaaaa;box-shadow:-4px -4px 8px #cfcfcf inset;padding:4px;"';
	$pstyle=' style="background:#dddddd;font-weight:normal;color:#888888;border-left:1px solid #aaaaaa;border-bottom:1px solid #888888;box-shadow:-4px -4px 8px #bfbfbf inset;padding:4px;"';
	$licol=array('ffffff','f1f1f1');
	$locol=array('efefef','e1e1e1');
	$lhcol=array('e3f0f6','d3e0e6');

	// Soll nur einmal geöffnet werden, um Projekte anzuzeigen
	if($AuswahlProjekte || $AuswahlAufgaben)
	{
		// Initialisieren des Anhangs für die SQL-Abfrage, auf die anzuzeigenden Vorgänge
		$attachments='';
		$M=0;

		// Person != Geschäftsleitung oder Sysadmin => Projektrechte-Prüfung
		$projekte="";
		if($GLOBALS['fxpglobals']['person'] && ($GLOBALS['fxpglobals']['user'] > 1) &&!$GLOBALS['fxpglobals']['persdat']['roles'][2])
			$projekte=" AND p.projekt_id IN (".prights_get(0).")";

		// Aufbau des SQL-Befehls für Auswahl der Vorgänge die angezeigt werden sollen
		$Anhang="";
		if($AuswahlProjekte)
		{
			$Anhang=" AND p.projektart<";
			if($AuswahlAufgaben)
				$Anhang .="=";
			$Anhang .=FXP_PRJ_TASK;
		}
		else if($AuswahlAufgaben)
			$Anhang=" AND p.projektart=".FXP_PRJ_TASK;

		// Aufbau des SQL-Befehls
		$last_day=$datearr[$days];

		$ProjSQL  = "SELECT z.projekt_id, z.mutterprojekt_id, z.soll_beginn, z.soll_ende, p.vorgangsnummer, p.name_projekt, p.projektart, p.beschreibung";
		$ProjSQL .= " FROM zeitdaten z, projekte p";
		$ProjSQL .= " WHERE z.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND z.soll_beginn<='".$last_day."' AND z.soll_ende>='".$datearr[1]."'";
		$ProjSQL .= " AND z.projekt_id=p.projekt_id AND (p.projekt_status=".FXP_PS_PLANNED." OR p.projekt_status=".FXP_PS_ACTIVE.") AND p.mandanten_id=".$GLOBALS['fxpglobals']['client'].$projekte.$Anhang;
		$ProjSQL .= " ORDER BY z.soll_beginn, p.vorgangsnummer";
		$Projekt=db_values($ProjSQL, '*');
	}
	else
		$Projekt=false;

	// Aufbau der 2.Zeile, in der die laufenden Projekte eingetragen sind, wenn dannach verlangt wurde
	if(($cv != CV_M1) && ($AuswahlProjekte || $AuswahlAufgaben))
	{
		// Alle Aufgaben holen, denen die Person als Ressource zugeordnet ist
		$wd_lnk='20';
		$ResAufgaben=array();
		if(is_array($Projekt) && $AuswahlAufgaben && (strlen($lnk_81) || strlen($lnk_34)))
		{
			$ResSQL  = "SELECT m.projekt_id FROM maplanung m, projekte p";
			$ResSQL .= " WHERE m.personen_id=".$GLOBALS['fxpglobals']['person']." AND m.mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$ResSQL .= " AND m.soll_beginn<='".$last_day."' AND m.soll_ende>='".$datearr[1]."'";
			$ResSQL .= " AND p.projekt_id=m.projekt_id AND p.projekt_status=".FXP_PS_ACTIVE." AND p.mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$ResTmp=db_values($ResSQL, '*');
//fxDebug($ResTmp, $ResSQL, 0);
			if(is_array($ResTmp))
			{
				$wd_lnk='36';
				foreach($ResTmp as $ResData)
					$ResAufgaben[(int)$ResData['projekt_id']]=true;
			}
		}
//fxDebug($ResAufgaben, '$ResAufgaben', 0);

		echo('	<tr>'.$nl);
		echo('		<td align="center" valign="top" nowrap'.$pstyle.'>'.link_img($GLOBALS['gfxpath'].'c_proj.png', $meldung_43, 16, 16).'</td>'.$nl);	// 1.Spalte: Vorgänge
		for($d=1; $d<=$days; $d++)
		{
			$Projektbeschreibung='';
			if(is_array($Projekt))
			{
				$ad=substr($datearr[$d], 0, 8);
				foreach($Projekt as $pdat)
				{
					$psd=substr($pdat['soll_beginn'], 0, 8);
					$ped=substr($pdat['soll_ende'], 0, 8);
					if(($ad >=$psd) && ($ad <=$ped))
					{
						// Handelt es sich bei diesem Eintrag um den Start oder das Ende eines Vorgangs?
						if($ad == $psd)
						{
							$status='<font class="s2 lightgrey">'.$start_text;
							if($ad == $ped)
								$status .='+'.$ende_text;
							$status .= ':</font>&nbsp;';
						}
						else if($ad == $ped)
							$status='<font class="s2 grey">'.$ende_text.':</font>&nbsp;';
						else
							$status='<font class="s2 grey">&hellip;</font>&nbsp;';

						$pvn=no_null($pdat['vorgangsnummer']);

						$Projektbeschreibung .= '<div style="width:'.$width.'px;border-bottom:1px dotted #cccccc;padding:0 2px;text-align:left;white-space:nowrap;overflow:hidden;">'; 
						$Projektbeschreibung .= $status;
//						$Projektbeschreibung .= '<img src="'.$GLOBALS['gfxpath'].'pt'.$pdat['projektart'].'.png" align="top">&nbsp;';
						$Projektbeschreibung .= '<font class="s2 black">'.$pvn.'</font>';
						$Projektbeschreibung .= '&nbsp;<font class="s2 darkgrey">'.trim($pdat['name_projekt']).'</font>';
						$Projektbeschreibung .= '</div>'; 
					}
				}
			}
			if(!strlen($Projektbeschreibung))
				$Projektbeschreibung = '<font class="lightgrey">x</font>';
			echo('		<td align="center" valign="top" colspan="4" style="border-left:1px solid #aaaaaa;border-bottom:1px solid #888888;">'.$Projektbeschreibung.'</td>'.$nl);
		}
		echo('	</tr>'.$nl);
	}

	// $Startzeit=Beginn der Kernarbeitszeit; $Endzeit=Ende der Kernarbeitszeit
	// $Startzeit/$Endzeit beinhalten die Werte, die der User in die Textfelder für den Anzeigeraum eingetragen hat.
	// Formatierung der Zeiten in DB-Format und im Folgenden Berechnung der Sekundenanzahl zu dieser Uhrzeit,
	// über kleinen, nötigen Umweg
	if(!$Startzeit)
		$Startzeit='00:00';
	if(!$Endzeit)
		$Endzeit='23:30';
	$Startzeit=strtodatetime($GLOBALS['fxpglobals']['settings']['formatzeit'], $Startzeit);
	$Endzeit=strtodatetime($GLOBALS['fxpglobals']['settings']['formatzeit'], $Endzeit);

	$Soll_Beginn=$pinfo['soll_beginn'];	// Arbeitsbeginn des Users
	$Soll_Ende="00000000".substr(time_add(substr(fxNow(), 0, 8).substr($pinfo['soll_beginn'], 8), $pinfo['sollstunden_tag'], "H"), 8);
	$start_kern=(fx_mktime(substr($Soll_Beginn, 8, 2), substr($Soll_Beginn, 10, 2), 0, 1, 1, 2000) - fx_mktime(0, 0, 0, 1, 1, 2000)) / 1800;
	$ende_kern=(($start_kern * 1800) + ($pinfo['sollstunden_tag'] * 3600)) / 1800;

	// Errechnung der Sekundenanzahl von $Startzeit und $Endzeit, da es sich bei $Startzeit und
	// $Endzeit um  Zeitangaben handelt muss man ein Datum hinzuaddieren, und danach wieder subtrahieren
	if($Startzeit > 0)
		$Startzeit=fx_mktime(substr($Startzeit, 8, 2), substr($Startzeit, 10, 2), 0, 1, 1, 2000) - fx_mktime(0, 0, 0, 1, 1, 2000);
	$Endzeit=fx_mktime(substr($Endzeit, 8, 2), substr($Endzeit, 10, 2), 0, 1, 1, 2000) - fx_mktime(0, 0, 0, 1, 1, 2000);

	// Berechnung: Wieviele halbe Stunden waren in $Startzeit ursprünglich;
	//	$Startzeit=8:00 --> $Startzeit=16
	$Startzeit=$Startzeit/1800;
	$Endzeit=$Endzeit/1800;

	// Aufbau des SQL-Befehls für die Abfrage der Termine
	// Ist User Projektleiter ... und ist in den Einstellungen gespeichert, ja er darf alle Termine sehen ...
	if((($GLOBALS['fxpglobals']['persdat']['role'] != 0) && (strtolower(substr($GLOBALS['fxpglobals']['settings']['alle_termine'], 0, 1)) == 'j') && ($pid != $GLOBALS['fxpglobals']['person'])) ||!$GLOBALS['fxpglobals']['person'] || ($GLOBALS['fxpglobals']['user'] == 1))
	{
		// Für Anzeige, welcher Termin zu wem gehört
		// ... wenn ja, dann bekommt er alle Termine zu sehen, außer der Privattermine ...
		// ... und Termine der Geschäftsführung werden immer ausblendet (außer Verwaltung)
		$Name_Person=", p.pname, p.vorname";
		$Name_Tabelle=", personen p";

		$rp=get_persons();
		if(!$pid)
			$Ist_Projektleiter=" AND (t.personen_id=".$GLOBALS['fxpglobals']['person']." OR (t.personen_id IN (".$rp.") AND t.terminart<>720)) AND p.personen_id=t.personen_id";
		else
			$Ist_Projektleiter=" AND (t.personen_id=".$pid." AND t.terminart<>720) AND p.personen_id=t.personen_id";
		$Ist_Projektleiter .= " AND p.mandanten_id=".$GLOBALS['fxpglobals']['client'];
	}
	else
	{
		// ... ansonsten nur die eigenen
		$Name_Person='';
		$Name_Tabelle='';
		$Ist_Projektleiter=" AND t.personen_id=".$GLOBALS['fxpglobals']['person'];
	}

	// Ist MONAT gewählt ensteht eine andere Tabelle, wie bei den anderen Ansichten
	if($cv == CV_M1)
	{
		$Startzeit=0;
		$Endzeit=ceil($days/7)-1;
	}

	$min_time='2400';
	$max_time='0000';
	// SQL-Befehl
	if(($cv == CV_D1) && $DBDatumsangabe_bck)
	{
		$sql_startzeit=substr($DBDatumsangabe_bck, 0, 8).'000000';
		$sql_endzeit=substr($DBDatumsangabe_bck, 0, 8).'240000';
	}
	else
	{
		$sql_startzeit=substr($datearr[1], 0, 8).'000000';
		$sql_endzeit=substr($date_END, 0, 8).'240000';
	}
	$R=0;
	$sql  = "SELECT t.termin_id, t.name_termin, t.beschreibung, t.startzeit, t.endzeit, t.terminart, t.wiederholung, t.projekt_id,";
	$sql .= " t.treffpunkt, t.erledigt, t.ansprechpartner, t.personen_id, t.bestaetigung_anmerk AS anmerkung, t.aktiv_kz".$Name_Person;
	$sql .= " FROM termine t".$Name_Tabelle;
	$sql .= " WHERE (t.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND t.startzeit<'".$sql_endzeit."' AND t.endzeit>'".$sql_startzeit."'".$Ist_Projektleiter.$Art.")";
	$sql .= " AND (t.archiv=0 OR t.archiv IS NULL)";
	// abgelehnte Termine nie anzeigen
	$sql .= " AND (t.bestaetigung_anmerk IS NULL OR t.bestaetigung_anmerk NOT LIKE '0-%')";
	// erledigte Termine ausblenden?
	if($nur_unerledigte_anzeigen)
		$sql .= " AND (t.erledigt IS NULL OR t.erledigt=0)";
	$sql .= " ORDER BY t.startzeit, t.endzeit";
	// Ausführen des SQL-Befehls, nur wenn man auch Termine sehen will
	$GLOBALS['mail_attachments']=array();
	$Termin=array();
	if($Termine_geschaeftl || $Termine_privat || $Abwesend || $Urlaub || $Sonderurlaub || $Schulung || $Krank || $Gleitzeit)
	{
		$TArray=db_values($sql,'*');
//fxDebug($TArray, $sql, 0);
		if(fxIsArray($TArray))
		{
			fxDeleteTMP();
			$icta=explode('|',meldung(3144));	// Termin im iCalendar-Format downloaden|Alle Termine im iCalendar-Format downloaden
			foreach($TArray as $Termine)
			{
				$time_start=substr($Termine['startzeit'], 8, 4);
				$time_end=substr($Termine['endzeit'], 8, 4);
				$min_time=min($min_time, $time_start);
				$max_time=max($max_time, $time_end);
//echo('Termine: $time_start='.$time_start.', $time_end='.$time_end.', $min_time='.$min_time.', $max_time='.$max_time.'<br />');
				for($t=0; $t<=fxp_date_diff($Termine['startzeit'], $Termine['endzeit']); $t++)
				{
					// Dauert der Terminer wenig als einen Tag (z.B. 9h-15h) dann wird er nur einmal angezeigt
					if(time_add($Termine['startzeit'], $t, 'D') >=$Termine['endzeit'] && $t)
						continue;
					// Startzeiten
					$alle_startzeiten[$R]="00000000".substr($Termine['startzeit'], 8); // Alle verschiedenen Startzeiten werden in ein Array geschrieben
					$datum=time_add(substr($Termine['startzeit'], 0, 8)."000000", $t, 'D');
					$Termin[$R][$datum]=$Termine; // Mit der Zählervariablen $R und dem Datum als Indizes werden die Termin Daten in einem Array gesichert
					$R++;
				}
			}
		}
	}

	// Addieren einer weiteren Stunde, da diese als Pause benötigt wird
	if(($cv != CV_M1) && $pid)
		$ende_kern +=$pinfo['pause']/1800;

	// Zeiterfassung
	$time_entries=array();
	if((strlen($lnk_81) || strlen($lnk_34)) && $pid)
	{
		$sql  = "SELECT z.zeit_id, z.projekt_id, z.ist_beginn, z.ist_ende, z.zeitaufwand, z.pause, z.zeitart,";
		$sql .= " t.taetigkeit_id, t.taetigkeit_text, t.anmerkungintern";
		$sql .= " FROM zeitdaten z, taetigkeiten t";
		$sql .= " WHERE z.personen_id=".$pid." AND z.zeitart IN (".FXP_TE_TIMEREC.",".FXP_TE_TIMEREC_SUM.",".FXP_TE_TRAVELEXP_TIMEREC.") AND z.projekt_id>=0 AND z.ist_beginn<'".$sql_endzeit."' AND z.ist_ende>='".$sql_startzeit."' AND z.mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$sql .= " AND t.zeit_id=z.zeit_id AND t.mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$sql .= " ORDER BY z.ist_beginn, z.ist_ende";
		$tmp=db_values($sql, '*');
//fxDebug($tmp, $sql, 0);

		if(fxIsArray($tmp))
		{
			$empty_array=array();
			$time_hour=0;
			for($time_counter=0; $time_counter<48; $time_counter++)
			{
				$minutes=30*$time_counter;
				$time_string=substr('00'.floor($minutes/60), -2).substr('00'.($minutes%60), -2);

				$empty_array[$time_string]=0;
			}

			foreach($tmp as $t)
			{
				$time_date=substr($t['ist_beginn'], 0, 8);

				if(!is_array($time_entries[$time_date]))
					$time_entries[$time_date]=$empty_array;

				if(($t['zeitart'] == FXP_TE_TIMEREC_SUM) || ($t['ist_beginn'] == $t['ist_ende']))
				{
					$nstf='';
					foreach($time_entries[$time_date] as $st => $std)
					{
						if(fxIsArray($std))
							$nstf=$std['ist_ende'];
					}
					if(!strlen($nstf))
					{
						if($t['zeitart'] == FXP_TE_TIMEREC_SUM)
							$nstf=substr($t['ist_beginn'],0,8).'000000';
						else
							$nstf=substr($t['ist_beginn'],0,8).substr($Soll_Beginn,8);
					}

					$t['ist_beginn']=$nstf;
					$t['ist_ende']=time_add($t['ist_beginn'], $t['zeitaufwand']+$t['pause'], 'S');
					if(substr($t['ist_ende'],0,8) > substr($t['ist_beginn'],0,8))
						$t['ist_ende']=substr($t['ist_beginn'],0,8).'240000';
				}
				$time_start=substr($t['ist_beginn'], 8, 2).substr('00'.(string)(floor((int)substr($t['ist_beginn'], 10, 2)/30)*30), -2);
				$time_end=substr($t['ist_ende'], 8, 2).substr('00'.(string)(ceil((int)substr($t['ist_ende'], 10, 2)/30)*30), -2);
				if(substr($time_end, 2) == '60')
					$time_end=substr('00'.((int)substr($time_end, 0, 2)+1), -2).'00';
				$min_time=min($min_time, $time_start);
				$max_time=max($max_time, $time_end);
//echo('Zeiterfassung: $time_start='.$time_start.', $time_end='.$time_end.', $min_time='.$min_time.', $max_time='.$max_time.'<br />');
				$time_act=$time_start;
				$emc=0;
				while(($time_act < $time_end) && ($emc < 48))
				{
					if(!is_array($time_entries[$time_date][$time_act]))
					{
						if($time_act == $time_start)
							$time_entries[$time_date][$time_act]=$t;
						else if($t['zeitart'] == FXP_TE_TRAVELEXP_TIMEREC)
							$time_entries[$time_date][$time_act] |= 4;
						else if($t['zeitart'] == FXP_TE_TIMEREC_SUM)
							$time_entries[$time_date][$time_act] |= 2;
						else if($t['zeitart'] == FXP_TE_TIMEREC)
							$time_entries[$time_date][$time_act] |= 1;
					}

					if(substr($time_act, 2) == '00')
						$time_act=substr($time_act, 0, 2).'30';
					else
						$time_act=substr('00'.((int)substr($time_act, 0, 2)+1), -2).'00';

					$emc++;
				}
			}
		}
	}
//fxDebug($time_entries, '$time_entries', 0);

	$linecounter=0;
	for($x=$Startzeit; $x<=$Endzeit; $x++)
	{
//echo('$x='.$x.', $start_kern='.$start_kern.', $ende_kern='.$ende_kern.'<br />');
		$modlinecounter=$linecounter%2;
		if(!isset($akt_tag) || ($cv != CV_M1))
			$akt_tag=$akt_tag_ini;

		// Initialisieren, bzw. wieder löschen falls bei vorherigem Schleifendurchlauf gefüllt worden
		$disp=false;
		if($cv != CV_M1)
		{
			$minutes=30*$x;

			// Liefert die Uhrzeit für die 1.Spalte der Tabelle
			$time=fx_date('YmdHis', fx_mktime(0, $minutes, 0, 1, 1, 2000));
			$time=formatdatetime($GLOBALS['fxpglobals']['settings']['formatzeit'], $time);

			$check=substr('00'.floor($minutes/60), -2).substr('00'.($minutes%60), -2);
			$min_check=substr('00'.floor(($start_kern*30)/60), -2).substr('00'.(($start_kern*30)%60), -2);
			$max_check=substr('00'.floor(($ende_kern*30)/60), -2).substr('00'.(($ende_kern*30)%60), -2);
//echo('$check='.$check.', $min_time='.$min_time.', $max_time='.$max_time.', $min_check='.$min_check.', $max_check='.$max_check.'<br />');

			if(($check >=$min_time) && ($check < $min_check))
				$disp=true;
			if(($check < $max_time) && ($check >=$max_check))
				$disp=true;

			// aktuelle Uhrzeit im Kalender (z.B. 10:00 h oder 15:30 h)
			$Time00=strtodatetime($GLOBALS['fxpglobals']['settings']['formatzeit'], $time);
			// 29 Minuten und 59 Sekunden später (z.B. 10:29.59 oder 15:59:59)
			$Time30="00000000".fx_date("His", zeitsek($Time00)+1799);
		}

		// Festlegung der Stylesheets, und Anzeige auf "alles ist zu sehen" wenn zwischen 8h und 16h
		if(($cv != CV_M1) && (($x < $start_kern) || ($x > $ende_kern)))
		{
			$lcol=$licol[$modlinecounter];
		}
		else
		{
			$lcol=$locol[$modlinecounter];
			$disp=true;
		}
		if($disp || ($cv == CV_M1))
		{
			// Zeilendefinition
			echo(	'<tr>'.$nl);

			$tstyle=$tistyle;
			if(($x < $start_kern) || ($x > $ende_kern))
				$tstyle=$tostyle;

			// Uhrzeit in Spalte schreiben
			if($cv != CV_M1)
				echo(		'<td valign="top" align="center" nowrap'.$tstyle.'>'.$time.'</td>'.$nl);

			// Aufbau der Kalendertabelle
			for($d=1; $d<=$days; $d++)
			{
				// Kein Arbeitstag
				$kap=true;
				if(!$pinfo[$akt_tag])
					$kap=false;
				if($cv != CV_M1)
				{
					$lcol=$licol[$modlinecounter];
					if(!$kap || ($x < $start_kern) || ($x > $ende_kern))
						$lcol=$locol[$modlinecounter];
				}
				else if($kap)
					$lcol=$licol[0];
				else
					$lcol=$locol[0];

				if(($x == 0) && ($d == 1))
					$DatumMonat=$datearr[1];	// Wird nur ein einziges Mal (beim 1.Durchlauf) ausgeführt
				else if($d != 8)
					$DatumMonat=time_add($DatumMonat, 1, 'D'); // Ansonsten wird das Datum immer um 1 Tag erhöht

				// $Datum wird mit $datearr[$d] gefüllt und im Folgenden an das Termineingabe-Dokument geschickt
				$Datum=formatdatetime($GLOBALS['fxpglobals']['settings']['formatdatum'], $datearr[$d]);

				// Monatsansicht gewählt
				if($cv == CV_M1)
				{
					$height=floor($width/2);
					$DatumMonat=formatdatetime($GLOBALS['fxpglobals']['settings']['formatdatum'], $DatumMonat);
					$DatumMonat=strtodatetime($GLOBALS['fxpglobals']['settings']['formatdatum'], $DatumMonat);

					if($d == 8)
					{
/*
						$fwdt=substr(time_add($DatumMonat, -6, 'D'), 0,8);
						$onclick=' '.fxf_jsFunction('loadTR', '>1', '^loading', $GLOBALS['fxpglobals']['tr'], 'navweek&form=_fxform_selmask_1&keep_get=1&ndate='.max($fwdt, $rsdt).'&ncv='.CV_D7);
						echo('<td height="100%" nowrap align="center" class="psina" tooltip="'.$meldung_793.': '.get_entry($fwdt,'date').' - '.get_entry($DatumMonat,'date').'" style="padding:4px;border:groove;cursor:pointer;"'.$onclick.'><b>&uarr;</b><br /><br /><b class="s1">'.$meldung_1036.'&nbsp;'.kw($fwdt).'</b></td>'.$nl);
*/
						break;
					}
					else
					{
						if($d == 1)
						{
							$fwdt=substr($DatumMonat,0,8);
							$lwdt=substr(time_add($fdwt, 6, 'D'), 0,8);
							$onclick=' '.fxf_jsFunction('loadTR', '>1', '^loading', $GLOBALS['fxpglobals']['tr'], 'navweek&form=_fxform_selmask_1&keep_get=1&ndate='.min($fwdt, $rsdt).'&ncv='.CV_D7);
							echo('<td height="100%" nowrap align="center" class="psina" tooltip="'.$meldung_793.': '.get_entry($fwdt,'date').' - '.get_entry($lwdt,'date').'" style="padding:4px;border:groove;cursor:pointer;"'.$onclick.'><b class="s1">'.$meldung_1036.'&nbsp;'.kw($fwdt).'</b></td>'.$nl);
						}

						$project_entries='';
						$absent_entries='';
						$appoint_entries='';

						for($r=0; $r<=$R; $r++)
						{
							$appoint_date=$DatumMonat;

							// Krankheitstermine von anderen Mitarbeitern die bereits vorbei sind, sollen nicht angezeigt werden
							if((substr($Termin[$r][$appoint_date]['endzeit'], 0, 8) < substr(fxNow(), 0, 8)) && (($Termin[$r][$appoint_date]['terminart'] == FXP_AT_SICKCALL) || ($Termin[$r][$appoint_date]['terminart'] == FXP_AT_SICK)) && ($Termin[$r][$appoint_date]['personen_id'] != $GLOBALS['fxpglobals']['person']))
								continue;

							// aktuelle Uhrzeit im Kalender (z.B. 10:00 h oder 15:00 h)
							if($Termin[$r][$appoint_date])
							{
								$appoint_start=formatdatetime($GLOBALS['fxpglobals']['settings']['formatzeit'], $Termin[$r][$appoint_date]['startzeit']);
								$appoint_end=formatdatetime($GLOBALS['fxpglobals']['settings']['formatzeit'], $Termin[$r][$appoint_date]['endzeit']);

								// Ist diese Variable gefüllt, wird der Name des "Terminbesitzers" angezeigt
								$pers_name='';
								if($Termin[$r][$appoint_date]['pname'] && ($Termin[$r][$appoint_date]['personen_id'] != $GLOBALS['fxpglobals']['person']))
								{
									$pers_name='<br /><i class="s2 grey">(';
									if(strlen(trim($Termin[$r][$appoint_date]['vorname'])))
										$pers_name .= $Termin[$r][$appoint_date]['vorname'].' ';
									$pers_name .= $Termin[$r][$appoint_date]['pname'].')</i>';
								}

								$tart=$Termin[$r][$appoint_date]['terminart'];
								if($fehltage[$tart])
								{
									$ltr='222';
									$ac=getAbsentClass($tart,true);
								}
								else
								{
									$ltr='27';
									$ac='black';
								}

								// Bestimmung des zu benutzenden Icons
								$TerminIcon=$TerminGrafiken[$tart];
								if(!$TerminIcon)
									$TerminIcon='c_outl';
								// Wiederholungstermin ??
								if($Termin[$r][$appoint_date]['wiederholung'] != 0)
									$TerminIcon='c_rept';
								// Bereits erledigt ?
								if($Termin[$r][$appoint_date]['erledigt'] == 1)
									$TerminIcon='c_fin';
								$TerminIcon=$GLOBALS['gfxpath'].$TerminIcon.'.png';
								// Übertragen der Termin-ID in eine neue, sprechende Variable
								$Termin_ID=$Termin[$r][$appoint_date]['termin_id'];
								// Termininfo-Tooltip
								$info=termin_tooltip($Termin[$r][$appoint_date]);
								// Aufbereiten und Anzeige des Treffpunkts
								if($Termin[$r][$appoint_date]['treffpunkt'] && ($Termin[$r][$appoint_date]['treffpunkt'] != "-"))
									$Termin[$r][$appoint_date]['treffpunkt']=$text_541.": ".$Termin[$r][$appoint_date]['treffpunkt'];
								else
									$Termin[$r][$appoint_date]['treffpunkt']='';

								// Wenn Outlook-Termin => blauer Rand
								if($Termin[$r][$appoint_date]['aktiv_kz'])
									$bcol='006b9f';
								else
									$bcol='bbbbbb';

								// Eintrag
								// Verwaltung, Gl und PL können alle Termin öffnen (bearbeiten) die angezeigt werden ...
								$lnk=true;
								if($pers_name)
								{
									if($GLOBALS['fxpglobals']['persdat']['roles'][-1] || $GLOBALS['fxpglobals']['persdat']['roles'][1] || $GLOBALS['fxpglobals']['persdat']['roles'][2] ||!$GLOBALS['fxpglobals']['person'] || ($GLOBALS['fxpglobals']['user'] == 1))
									{
										// ... aber PL darf keine Fehltage ändern, nur VW und GL
										if($fehltage[$tart] && $GLOBALS['fxpglobals']['persdat']['roles'][1])
											$lnk=false;
									}
									else
										$lnk=false;
								}
								if($lnk)
									$img=fieldLinkImage('lnk', $TerminIcon, '?tr_alt='.$ltr.'&aktion=1&att='.$att.'&Termin_ID='.$Termin_ID.'&Datumsangabe='.$adt, $GLOBALS['ltt_'.$ltr], $ltr);
								else
									$img='<img src="'.$TerminIcon.'" align="top">';
								$ics_link='';
								$ics_filename=iCalendar($Termin[$r][$appoint_date]['termin_id']);
								if(strlen($ics_filename))
									$ics_link='<a href="'.$ics_filename.'"><img src="'.$GLOBALS['gfxpath'].'iccl.png" tooltip="'.$icta[0].'<br ><br /><i class=blue>'.$GLOBALS['mail_attachments'][$ics_filename]['summary'].'</i>" style="position:relative;cursor:pointer;vertical-align:middle;"></a>&nbsp;';

								$appoint  = '<div style="margin-bottom:2px;background:#fff;border:1px solid #'.$bcol.';border-radius:6px;box-shadow:4px 4px 4px rgba(0,0,0, 0.1);cursor:help;" tooltip="'.$info.'">';
								$appoint .= '	<div style="padding:2px;background:#ddd;border-top-left-radius:6px;border-top-right-radius:6px;border-bottom:1px solid #cccccc;">'.$img.'&nbsp;<font class="s2 darkgrey">'.$appoint_start.'-'.$appoint_end.'</font></div>';
								$appoint .= '	<div style="width:'.($width-3).'px;border-bottom-left-radius:6px;border-bottom-right-radius:6px;padding:2px;white-space:normal;overflow:hidden;">'.$ics_link.'<font class="'.$ac.'">'.$Termin[$r][$appoint_date]['name_termin'].'</font>'.$pers_name.'</div>';
								$appoint .= '</div>';

								if($fehltage[$tart])
									$absent_entries .= $appoint;
								else
									$appoint_entries .= $appoint;
							}
						}

						// Feiertag ?
						$Feiertagname='';
						$cclass='black';
						if(fxIsArray($Feiertag) && $Feiertag[substr($DatumMonat,0,8)])
						{
							$cclass='blue';

							$Feiertagname=$Feiertag[substr($DatumMonat,0,8)]['name'];
							if($Feiertag[substr($DatumMonat, 0, 8)]['art'] == FXP_HL_HALF)
								$Feiertagname .='&nbsp;<sup>1/2</sup>';
							$Feiertagname='<i class="'.$cclass.'">'.$Feiertagname.'</i>';
							$lcol=$lhcol[0];
							$kap=false;
						}

						$D=$akt_tag;
						if(!$D)
							$D=7;

						if(fxIsArray($Projekt))
						{
							$ad=substr($DatumMonat, 0, 8);
							foreach($Projekt as $pdat)
							{
								$psd=substr($pdat['soll_beginn'], 0, 8);
								$ped=substr($pdat['soll_ende'], 0, 8);
								if(($ad >=$psd) && ($ad <=$ped))
								{
									// Handelt es sich bei diesem Eintrag um den Start oder das Ende eines Vorgangs?
									if($ad == $psd)
									{
										$status='<font class="s2 lightgrey">'.$start_text;
										if($ad == $ped)
											$status .='+'.$ende_text;
										$status .= ':</font>&nbsp;';
									}
									else if($ad == $ped)
										$status='<font class="s2 grey">'.$ende_text.':</font>&nbsp;';
									else
										$status='<font class="s2 grey">&hellip;</font>&nbsp;';

									$pvn=no_null($pdat['vorgangsnummer']);

									$project_entries .= '<div style="width:'.$width.'px;border-bottom:1px dotted #cccccc;padding:0 2px;text-align:left;white-space:nowrap;overflow:hidden;">'; 
									$project_entries .= $status;
//									$project_entries .= '<img src="'.$GLOBALS['gfxpath'].'pt'.$pdat['projektart'].'.png" align="top">&nbsp;';
									$project_entries .= '<font class="s2 black">'.$pvn.'</font>';
									$project_entries .= '&nbsp;<font class="s2 darkgrey">'.trim($pdat['name_projekt']).'</font>';
									$project_entries .= '</div>'.$nl;
								}
							}
						}

						// Ist nötig für das Vorbelegen der Felder beim Zielformular
						// Es wird auf Mittag des jeweiligen Tages vorbelegt
						$inact='';
						$mlcol=$lcol;
						if((substr($appoint_date,0,8) < $rsdt) || (substr($appoint_date,0,8) > $redt))
						{
							$inact=' ainactive';
							$mlcol=$locol[0];
						}
						$DatumMonat=formatdatetime($GLOBALS['fxpglobals']['settings']['formatdatum'], $DatumMonat);
						$Zeitpunkt=substr(strtodatetime($GLOBALS['fxpglobals']['settings']['formatdatum'], $DatumMonat), 0, 8)."120000";
						echo('		<td valign="top" bgcolor="#'.$mlcol.'" style="border-left:1px solid #aaaaaa;border-bottom:1px solid #aaaaaa;">'.$nl);
						echo('			<table height="'.$height.'" border="0" cellspacing="0" cellpadding="2">'.$nl);

						// Tagesdatum
						$onclick=' '.fxf_jsFunction('loadTR', '>1', '^loading', $GLOBALS['fxpglobals']['tr'], 'navday&form=_fxform_selmask_1&keep_get=1&ndate='.$DatumMonat.'&ncv='.CV_D1);
						echo('				<tr>'.$nl);
						echo('					<td height="14" width="'.$width.'" align="center" nowrap style="'.$hstyle.'cursor:pointer;"'.$onclick.'>'.$nl);
						echo('							<b class="'.$cclass.$inact.'">'.$Wochentag[$D].'&nbsp;'.$DatumMonat.'</b>'.$nl);
						echo('					</td>'.$nl);
						echo('				</tr>'.$nl);

						$entry=false;
						// Feiertag
						if(strlen($Feiertagname))
						{
							echo('				<tr><td align="center" valign="top" style="border-bottom:1px solid #aaaaaa;">'.$Feiertagname.'</td></tr>'.$nl);
							$entry=true;
						}
						echo('			<tr>'.$nl);
						echo('				<td height="100%" align="left" valign="top">'.$nl);
						// Neuanlage
						if(strlen($lnk_81) || strlen($lnk_34) || strlen($lnk_27) || (strlen($lnk_222) && $kap))
						{
							$icons='';
							// ...Zeiterfassung
							if(strlen($lnk_81))
								$icons .= fieldLinkImage('lnk81', 'c_time', '?aktion=3&gd='.substr($Zeitpunkt,0,8).'&persid='.$pid, $lnk_81, 81, 'margin:1px;margin-right:8px;');
							else if(strlen($lnk_34))
								$icons .= fieldLinkImage('lnk34', 'c_time', '?aktion=3&gd='.substr($Zeitpunkt,0,8).'&persid='.$pid, $lnk_34, 34, 'margin:1px;margin-right:8px;');
							// ...Termin
							if(strlen($lnk_27))
								$icons .= fieldLinkImage('app', 'icne', '?tr_alt=27&aktion=2&att='.$att.'&Zeitpunkt='.$Zeitpunkt.'&Datumsangabe='.$adt, $meldung_68, 27, 'margin:0 1px 1px 0;');
							// ...Abwesenheitstermin
							if(strlen($lnk_222) && $kap)
								$icons .= fieldLinkImage('abs', 'icad', '?tr_alt=222&aktion=2&att='.$att.'&Zeitpunkt='.$Zeitpunkt.'&Datumsangabe='.$adt, $meldung_69, 222, 'margin:0 1px 1px 0;');
							echo('					'.$icons.'<br />'.$nl);
						}
						$show_array=array('project_entries', 'absent_entries', 'appoint_entries');
						foreach($show_array as $show_type)
						{
							$show_string=${$show_type};
							if(strlen($show_string))
							{
								echo('						<br />'.$show_string.$nl);
								$entry=true;
							}
						}
						echo('						<br />'.$nl);
						echo('					</td>'.$nl);
						echo('				</tr>'.$nl);
						echo('			</table>'.$nl);
						echo('		</td>'.$nl);

						$DatumMonat=strtodatetime($GLOBALS['fxpglobals']['settings']['formatdatum'], $DatumMonat);
					}
				}
				// Keine Monatsanzeige
				else
				{
					if($x == $start_kern)
						$kap=true;
					else
						$kap=false;

					$absent_entries='';
					$appoint_entries='';

					$wdiff=15;
					if($pid == $GLOBALS['fxpglobals']['person'])
						$wdiff += 19;

					for($r=0; $r<=$R; $r++)
					{
						// Für das Vorbelegen der Felder des Zielformulars
						$Zeitpunkt=substr($datearr[$d],0, 8).substr(strtodatetime($GLOBALS['fxpglobals']['settings']['formatzeit'], $time),8);
						$appoint_date=$datearr[$d];

						// Krankheitstermine von anderen Mitarbeitern die in bereits vorbei sind, sollen nicht angezeigt werden
						if((substr($Termin[$r][$appoint_date]['endzeit'], 0, 8) < substr(fxNow(), 0, 8)) && ($Termin[$r][$appoint_date]['terminart'] == 1040) && ($Termin[$r][$appoint_date]['personen_id'] != $GLOBALS['fxpglobals']['person']))
							continue;

						if($Termin[$r][$appoint_date] && ($Time00 <=substr($Termin[$r][$appoint_date]['startzeit'], 8)) && ($Time30 >=substr($Termin[$r][$appoint_date]['startzeit'], 8)))
						{
							if($Termin[$r][$appoint_date]['termin_id'] > 0)
							{
								$appoint_start=formatdatetime($GLOBALS['fxpglobals']['settings']['formatzeit'], $Termin[$r][$appoint_date]['startzeit']);
								$appoint_end=formatdatetime($GLOBALS['fxpglobals']['settings']['formatzeit'], $Termin[$r][$appoint_date]['endzeit']);

								// Füllen der Termin-ID
								$Termin_ID=$Termin[$r][$appoint_date]['termin_id'];
								$tart=$Termin[$r][$appoint_date]['terminart'];
								if($fehltage[$tart])
								{
									$ltr='222';
									$ac=getAbsentClass($tart,true);
								}
								else
								{
									$ltr='27';
									$ac='black';
								}

								// Anzeige des Namens des "Terminbesitzers", wenn Variable gefüllt ist
								$pers_name='';
								if($Termin[$r][$appoint_date]['pname'] && ($Termin[$r][$appoint_date]['personen_id'] != $GLOBALS['fxpglobals']['person']))
								{
									$pers_name='<br /><i class="s2 grey">(';
									if(strlen(trim($Termin[$r][$appoint_date]['vorname'])))
										$pers_name .= $Termin[$r][$appoint_date]['vorname'].' ';
									$pers_name .= $Termin[$r][$appoint_date]['pname'].')</i>';
								}

								// Bestimmung des zu benutzenden Icons
								$TerminIcon=$TerminGrafiken[$Termin[$r][$appoint_date]['terminart']];
								if(!$TerminIcon)
									$TerminIcon='c_outl';
								// Wiederholungstermin ??
								if($Termin[$r][$appoint_date]['wiederholung'] != 0)
									$TerminIcon='c_rept';
								// Bereits erledigt ?
								if($Termin[$r][$appoint_date]['erledigt'] == 1)
									$TerminIcon='c_fin';
								$TerminIcon=$GLOBALS['gfxpath'].$TerminIcon.'.png';

								// Termininfo-Tooltip
								$info=termin_tooltip($Termin[$r][$appoint_date]);
							}

							// Wenn Outlook-Termin => blauer Rand
							if($Termin[$r][$appoint_date]['aktiv_kz'])
								$bcol='006b9f';
							else
								$bcol='bbbbbb';

							// Eintrag
							// Verwaltung, Gl und PL können alle Termin öffnen (bearbeiten) die angezeigt werden ...
							$lnk=true;
							if($pers_name)
							{
								if($GLOBALS['fxpglobals']['persdat']['roles'][-1] || $GLOBALS['fxpglobals']['persdat']['roles'][1] || $GLOBALS['fxpglobals']['persdat']['roles'][2] ||!$GLOBALS['fxpglobals']['person'] || ($GLOBALS['fxpglobals']['user'] == 1))
								{
									// ... aber PL darf keine Fehltage ändern, nur VW und GL
									if($fehltage[$tart] && $GLOBALS['fxpglobals']['persdat']['roles'][1])
										$lnk=false;
								}
								else
									$lnk=false;
							}
							if($lnk)
								$img=fieldLinkImage('lnk', $TerminIcon, '?tr_alt='.$ltr.'&aktion=1&att='.$att.'&Termin_ID='.$Termin_ID.'&Datumsangabe='.$adt, $GLOBALS['ltt_'.$ltr], $ltr);
							else
								$img='<img src="'.$TerminIcon.'" align="top">';

							$appoint  = '<div style="margin-bottom:2px;background:#fff;border:1px solid #'.$bcol.';border-radius:6px;box-shadow:4px 4px 4px rgba(0,0,0, 0.1);cursor:help;" tooltip="'.$info.'">';
							$appoint .= '	<div style="padding:2px;background:#ddd;border-top-left-radius:6px;border-top-right-radius:6px;border-bottom:1px solid #cccccc;">'.$img.'&nbsp;<font class="s2 darkgrey">'.$appoint_start.'-'.$appoint_end.'</font></div>';
							$appoint .= '	<div style="width:'.($width-$wdiff).'px;border-bottom-left-radius:6px;border-bottom-right-radius:6px;padding:2px;white-space:normal;overflow:hidden;">'.$ics_link.'<font class="'.$ac.'">'.$Termin[$r][$appoint_date]['name_termin'].'</font>'.$pers_name.'</div>';
							$appoint .= '</div>';

							if($fehltage[$tart])
								$absent_entries .= $appoint;
							else
								$appoint_entries .= $appoint;
						}
					}
					$lnk=$lnk_27;
					// Feiertag ?
					if(sizeof($Feiertag) && $Feiertag[substr($datearr[$d], 0, 8)])
					{
						$lcol=$lhcol[$modlinecounter];
						$kap=false;
					}
					// Check ob in der aktuellen halben Stunde bereits ein Termin für die Person hinterlegt ist
					$st=substr($appoint_date, 0, 8).substr($Time00, 8);
					$en=substr($appoint_date, 0, 8).substr($Time30, 8);
					if($pid)
					{
						$csql  = "SELECT termin_id FROM termine";
						$csql .=" WHERE personen_id=".$pid." AND (startzeit<'".$en."' AND endzeit>'".$st."') AND (archiv IS NULL OR archiv=0)";
						$csql .=" AND (bestaetigung_anmerk IS NULL OR bestaetigung_anmerk NOT LIKE '0%') AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
						$Ter=db_values($csql, '*');
					}
					if(!fxIsArray($Ter))
						$colorcode='d2d2d2';
					else
					{
						// Kein "Neu-Anlegen"-Icon wenn der Zeitraum bereits belegt ist
						$lnk='';
						if(sizeof($Ter) == 1)
							$colorcode='79c7ae';
						else
							$colorcode='e879a6';
					}

					// Zeiterfassungsbalken
					$time_date=substr($Zeitpunkt, 0, 8);
					$time_key=substr($Zeitpunkt, 8, 4);
					if(fxIsArray($time_entries) && fxIsArray($time_entries[$time_date]) && $time_entries[$time_date][$time_key])
					{
						$ze_colorcode1='79aec7';
						$ze_colorcode2='d3e0e6';
						if(fxIsArray($time_entries[$time_date][$time_key]))
						{
							if(($time_entries[$time_date][$time_key]['zeitart'] == FXP_TE_TIMEREC_SUM) || ($time_entries[$time_date][$time_key]['zeitart'] == FXP_TE_TRAVELEXP_TIMEREC))
							{
								$ze_colorcode1='79c7ae';
								$ze_colorcode2='d3e6e0';
							}

							$tt=get_entry($time_entries[$time_date][$time_key]['ist_beginn'], 'zeit').'-'.get_entry($time_entries[$time_date][$time_key]['ist_ende'], 'zeit').': '.get_entry($time_entries[$time_date][$time_key]['zeitaufwand']/3600, 'dezimal').'&nbsp;h';
							if($time_entries[$time_date][$time_key]['zeitart'] == FXP_TE_TIMEREC_SUM)
							{
								$tt='&sum;&nbsp;'.$tt;
								$time_lnk=fieldLinkImage('lnk34', 'c_time', '?aktion=3&gd='.$Zeitpunkt.'&persid='.$pid, $tt, 34, 'margin-top:2px;');
							}
							else if($time_entries[$time_date][$time_key]['zeitart'] == FXP_TE_TRAVELEXP_TIMEREC)
								$time_lnk=fieldLinkImage('lnk150', 'c_time', '?aktion=3&gd='.$Zeitpunkt.'&persid='.$pid, $tt, 150, 'margin-top:2px;');
							else
								$time_lnk=fieldLinkImage('lnk81', 'c_time', '?aktion=3&gd='.$Zeitpunkt.'&persid='.$pid, $tt, 81, 'margin-top:2px;');
						}
						else
						{
							if(!($time_entries[$time_date][$time_key]%2))
							{
								$ze_colorcode1='79c7ae';
								$ze_colorcode2='d3e6e0';
							}
							$time_lnk='';
						}
					}
					else
					{
						$ze_colorcode1='d2d2d2';
						$ze_colorcode2=$lcol;
						if(strlen($lnk_81))
							$time_lnk=fieldLinkImage('lnk81', 'c_time', '?aktion=3&gd='.$Zeitpunkt.'&persid='.$pid, $lnk_81, 81, 'margin-top:2px;');
						else
							$time_lnk=fieldLinkImage('lnk34', 'c_time', '?aktion=3&gd='.$Zeitpunkt.'&persid='.$pid, $lnk_34, 34, 'margin-top:2px;');
					}
					echo('		<td width="4" bgcolor="#'.$ze_colorcode1.'" style="border-left:1px solid #aaaaaa;"><font style="font-size:2px;">&nbsp;</font></td>'.$nl);

					// Neuanlage oder Änderung nur bei eigener Person
					if($pid == $GLOBALS['fxpglobals']['person'])
						echo('		<td width="20" align="center" valign="top" bgcolor="#'.$ze_colorcode2.'" style="border-left:1px solid #888888;">'.$time_lnk.'</td>'.$nl);
					else
						echo('		<td></td>'.$nl);

					// Terminbalken
					echo('		<td width="4" bgcolor="#'.$colorcode.'" style="border-left:1px solid #aaaaaa;"><font style="font-size:2px;">&nbsp;</font></td>'.$nl);

					// Termineintrag
					echo('		<td valign="top" bgcolor="#'.$lcol.'" style="border-left:1px solid #888888;padding:2px;">'.$nl);
					// ...Neuanlage
					$entry=false;
					if(strlen($lnk) || (strlen($lnk_222) && $kap))
					{
						$icons='';
						// ...Termin
						if(strlen($lnk))
							$icons .= fieldLinkImage('app', 'icne', '?tr_alt=27&aktion=2&att='.$att.'&Zeitpunkt='.$Zeitpunkt.'&Datumsangabe='.$adt, $meldung_68, 27, 'margin:0 1px 1px 0;');
						// ...Abwesenheitstermin
						if(strlen($lnk_222) && $kap)
						{
							$icons .= fieldLinkImage('abs', 'icad', '?tr_alt=222&aktion=2&att='.$att.'&Zeitpunkt='.$Zeitpunkt.'&Datumsangabe='.$adt, $meldung_69, 222, 'margin:0 1px 1px 0;');
							$kap=false;
						}
						echo($icons.'<br />'.$nl);
						$entry=true;
					}
					$show_array=array('absent_entries', 'appoint_entries');
					foreach($show_array as $show_type)
					{
						$show_string=${$show_type};
						if(strlen($show_string))
						{
							if($entry)
								echo('			<br />');
							echo($show_string.$nl);
							$entry=true;
						}
					}
					echo('			<br />'.$nl);
					echo('		</td>'.$nl);
					$colorcode='';
				}

				$akt_tag++;
				if($akt_tag > 6)
					$akt_tag=0;
			}
			echo('	</tr>'.$nl);
		}
		$linecounter++;
	}
	echo('</table>'.$nl);
	echo('</div>'.$nl);


	// Calendar mask footer
	msk_window('', '', true);

	// Print
	$ptitle=meldung(366).': '.get_entry($sdt,'datum');
	if($edt != $sdt)
		$ptitle .= ' - '.get_entry($edt,'datum');
	drawPrint('scheduler', $ptitle);

	// All appointment
	$ica=iCalendarCombine(2);
}
?>