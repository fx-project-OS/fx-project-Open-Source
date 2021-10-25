<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : termine_liste_pre.inc                                        //
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

setForm(true);

$maske541_sf=true;
$index=true;
if(isset($GLOBALS['fxptdata']['lvalues']) && isset($GLOBALS['fxptdata']['lvalues'][$GLOBALS['_maskcounter']]))
	unset($GLOBALS['fxptdata']['lvalues'][$GLOBALS['_maskcounter']]);

if(isset($ganztaegig_nv))
	unset($ganztaegig_nv);

// Termin-ID's der ausgewählten Person ermitteln (bereits in Auswahlarray vorhanden)
$tids='';
if(fxIsArray($Termin_Arr))
{
//fxDebug($Termin_Arr, '$Termin_Arr', 0);
	$strt=false;
	$srch='_n'.$spersid;
	foreach($Termin_Arr as $tid => $dmy)
	{
		if(substr($tid, 0,1) == '_')
		{
			if($strt)
				break;
			else if($tid === $srch)
				$strt=true;
		}
		else if($strt)
			$tids .= ','.$tid;
	}
	$tids=substr($tids, 1);
fxDebug($tids, '$tids', 3);
}

// Termine aus Datenbank holen
$varr=array();
if(strlen($tids))
{
	$sql  = "SELECT termin_id, mastertermin_id, personen_id, projekt_id, startzeit, endzeit, soll_st_tag, soll_beginn, terminart, terminart AS masterterminart, name_termin, beschreibung";
	$sql .= " FROM termine";
	$sql .= " WHERE termin_id IN (".$tids.") AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$sql .= " ORDER BY startzeit DESC";
	$varr=db_values($sql, '*');
fxDebug($varr, $sql, 3);
}

// Termine aufbereiten
if(fxIsArray($varr))
{
	// Einträgezähler für bestimmte Spalten
	$cnt_ganztaegig=0;
	$cnt_projekt=0;

	// Personendaten ermitteln
	$sql="SELECT wochenarbeitstd, pause, arbeitstage, soll_beginn FROM madaten WHERE personen_id=".$spersid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$pdt=db_values($sql);
fxDebug($pdt, $sql, 3);

	// Projektdaten ermitteln
	$pids='';
	foreach($varr as $c => $v)
	{
		if($v['projekt_id'] > 0)
			$pids .= ','.$v['projekt_id'];

		if($v['termin_id'] != $v['mastertermin_id'])
			$varr[$c]['masterterminart']=db_value("SELECT terminart FROM termine WHERE termin_id=".$v['mastertermin_id']." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
	}
	$prja=array();
	if(strlen($pids))
	{
		$pids=substr($pids, 1);
fxDebug($pids, '$pids', 3);
		$sql="SELECT projekt_id, vorgangsnummer, name_projekt FROM projekte WHERE projekt_id IN (".$pids.") AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$tmp=db_values($sql, '*');
		if(fxIsArray($tmp))
		{
			foreach($tmp as $t)
				$prja[(int)$t['projekt_id']]=array('nr'=>no_null($t['vorgangsnummer']), 'name'=>$t['name_projekt']);
		}
	}
fxDebug($prja, '$prja', 3);

	// Maskenüberschrift
	$sql="SELECT pname, vorname FROM personen WHERE personen_id=".$spersid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$tmp=db_values($sql);
	if(fxIsArray($tmp))
	{
		$man=trim($tmp['pname']);
		$mav=trim($tmp['vorname']);
		if(strlen($mav))
			$man .= ',&nbsp;'.$mav;

		$maske541_head='+:&nbsp;&nbsp;'.$man;
	}

	// Aktionsicons und -tooltips
	$ic_acta=array(FXP_DISPLAY=>'icsh', FXP_CHANGE=>'icch', FXP_DELETE=>'icde');
	$icon='';
	if(isset($ic_acta[$GLOBALS['fxpglobals']['action']]))
		$icon=$ic_acta[$GLOBALS['fxpglobals']['action']];
	$tt_acta=explode(',',meldung(16,false));
	$tt_act=$tt_acta[$GLOBALS['fxpglobals']['action']-1];

	fxDeleteTMP();
	$GLOBALS['mail_attachments']=array();
	$icta=explode('|',meldung(3144));	// Termin im iCalendar-Format downloaden|Alle Termine im iCalendar-Format downloaden

	$mc=$GLOBALS['_maskcounter'];
	$GLOBALS['fxptdata']['lvalues'][$mc]=array();

	$fa=array('Checkbox', 'Datum', 'Zeitraum', 'ganztaegig', 'Terminart', 'Projekt', 'einladen', 'betreff', 'Beschreibung', 'icon|0');
	fxf_maskColumnBorder(array('icon'), 0, -1, $mc);

	foreach($varr as $l => $v)
	{
		// Aktuell gewählte Zeile hervorheben
		$cls='';
		if($siid && ($siid == $v['termin_id']))
			$cls='^class_rg'.(($l+1)%2).'|';

		// Termininfos ausblenden?
		$dontshow=false;
		// ja, falls kein eigener Termin
		if($spersid != $GLOBALS['fxpglobals']['person'])
		{
			$dontshow=true;
			// ausser man hat Recht auf Ändern und es ist kein Privattermin
			if(($v['terminart'] != FXP_AT_PRIVATE) && ($v['masterterminart'] != FXP_AT_PRIVATE) && test_bit($gpa['par'][$spersid], FXP_CHANGE))
				$dontshow=false;
		}

		$GLOBALS['fxptdata']['lvalues'][$mc][$l]=array();

		// ...Checkbox
		if($GLOBALS['fxpglobals']['action'] == FXP_DELETE)
		{
			$val='';
			if($err && $_POST['Checkbox'][$mc][$l])
				$val='1';
			$GLOBALS['fxptdata']['lvalues'][$mc][$l]['checkbox']=$cls.$val;
		}

		$dt1=get_entry($v['startzeit'], "datum");
		$dt2=get_entry($v['endzeit'], "datum");
		$tm1=get_entry($v['startzeit'], "zeit");
		$tm2=get_entry($v['endzeit'], "zeit");

		if($GLOBALS['fxpglobals']['tr'] == 222)	// Zeitgenehmigungsverfahren
		{
			// ...Datum
			$val=$dt1.',&nbsp;'.$tm1;
			if($dt2 != $dt1)
				$val .= '&nbsp;-&nbsp;'.$dt2.',&nbsp;'.$tm2;
			else if($tm2 != $tm1)
				$val .= '-'.$tm2;
			$GLOBALS['fxptdata']['lvalues'][$mc][$l]['datum']=$cls.'~<b>'.$val.'</b>';
		}
		else
		{
			// ...Datum
			$val=$dt1;
			if($dt2 != $dt1)
				$val .= '-'.$dt2;
			$GLOBALS['fxptdata']['lvalues'][$mc][$l]['datum']=$cls.'~<b>'.$val.'</b>';

			// ...Zeit
			$val=$tm1;
			if($tm2 != $tm1)
				$val .= '-'.$tm2;
			$GLOBALS['fxptdata']['lvalues'][$mc][$l]['zeitraum']=$cls.'~'.$val;
		}

		// ...Dauer
		$GLOBALS['fxptdata']['lvalues'][$mc][$l]['ganztaegig']=$cls.'~';
		if(fxIsArray($pdt) && $pdt['arbeitstage'])
		{
			$d=0;
			$tss=get_double($v['soll_st_tag']);
			if($tss > 0.00)
			{
				$sbd=$v['soll_beginn'];
				$asc=validate_entry(get_entry($sbd, 'zeit'), $ierr, 'zeitspanne');
				$ast=$tss*3600;
			}
			else
			{
				$sbd=$pdt['soll_beginn'];
				$asc=validate_entry(get_entry($sbd, 'zeit'), $ierr, 'zeitspanne');
				$ast=($pdt['wochenarbeitstd']*3600)/$pdt['arbeitstage'];
			}
			// Halbtägige 1. Endzeit errechnen
			$ssc=$asc+($ast/2);
			$h=(int)($ssc/3600); $ssc=$ssc-$h*3600;
			$m=(int)($ssc/60);
			$s=$ssc-$m*60;
			$db_endzeit_1=substr('0'.$h, -2).substr('0'.$m, -2).substr('0'.$s, -2);
			// Halbtägige 2. Startzeit errechnen
			$ssc=$asc+($ast/2)+$pdt['pause'];
			$h=(int)($ssc/3600); $ssc=$ssc-$h*3600;
			$m=(int)($ssc/60);
			$s=$ssc-$m*60;
			$db_startzeit_2=substr('0'.$h, -2).substr('0'.$m, -2).substr('0'.$s, -2);
			// Halbtägige 2. Endzeit errechnen
			$ssc=$asc+$ast+$pdt['pause'];
			$h=(int)($ssc/3600); $ssc=$ssc-$h*3600;
			$m=(int)($ssc/60);
			$s=$ssc-$m*60;
			$db_endzeit_2=substr('0'.$h, -2).substr('0'.$m, -2).substr('0'.$s, -2);
			if((substr($sbd, 8) == substr($v['startzeit'], 8)) && ($db_endzeit_1 == substr($v['endzeit'], 8)))
				$d=2572;	// halbtägig (1. Hälfte)
			else if(($db_startzeit_2 == substr($v['startzeit'], 8)) && ($db_endzeit_2 == substr($v['endzeit'], 8)))
				$d=2573;	// halbtägig (2. Hälfte)
			else if((substr($sbd, 8) == substr($v['startzeit'], 8)) && ($db_endzeit_2 == substr($v['endzeit'], 8)))
				$d=2571;	// ganztägig
			if($d > 0)
			{
				$GLOBALS['fxptdata']['lvalues'][$mc][$l]['ganztaegig'] .= meldung($d, false);
				$cnt_ganztaegig++;
			}
		}

		// ...Terminart (keine Privattermine anderer Personen anzeigen)
		if($dontshow)
			$val=$notext;
		else
		{
//function iCalendar($aid, $cid=0, $with_attachment=true)
			$val='';
			$ics_filename='';
			if($GLOBALS['fxpglobals']['action'] == FXP_DISPLAY)
				$ics_filename=iCalendar($v['termin_id']);
			if(strlen($ics_filename))
				$val .= '<a href="'.$ics_filename.'"><img src="'.$GLOBALS['gfxpath'].'iccl.png" tooltip="'.$icta[0].'<br ><br /><i class=blue>'.$GLOBALS['mail_attachments'][$ics_filename]['summary'].'</i>" align="right" style="position:relative;cursor:pointer;"></a>';
			$val .= $tarten[$v['terminart']];
			if(strlen($ics_filename))
				$val .= '&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;';
		}
		$GLOBALS['fxptdata']['lvalues'][$mc][$l]['terminart']=$cls.'~'.$val;

		// ...Projekt
		$val=(int)$v['projekt_id'];
		if(($val > 0) && fxIsArray($prja) && isset($prja[$val]))
		{
			$htt=' tooltip="'.$prja[$val]['name'].'"';
			$GLOBALS['fxptdata']['lvalues'][$mc][$l]['projekt']=$cls.'~<div style="margin:0 4px;"><span id="prjhtt'.$l.'" style="float:left;cursor:help;"'.$htt.'><b class="grey">&uarr;</b>&nbsp;<b class="blue">'.$prja[$val]['nr'].'</b>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</span><span style="float:right;margin-top:-21px;"><img src="'.$GLOBALS['gfxpath'].'tts.png"></span></div>';
			$cnt_projekt++;
		}
		else
			$GLOBALS['fxptdata']['lvalues'][$mc][$l]['projekt']=$cls.'~&nbsp;';

		// Einladungsliste (vorerst ausgeblendet)

		// ...Betreff (keine Privattermine anderer Personen anzeigen)
		$val=$v['name_termin'];
		if($dontshow)
			$val=$notext;
		$GLOBALS['fxptdata']['lvalues'][$mc][$l]['betreff']=$cls.'~'.text_wrap($val,true,48);

		// ...Beschreibung (keine Privattermine anderer Personen anzeigen)
		$val=$v['beschreibung'];
		if($dontshow)
			$val=$notext;
		$GLOBALS['fxptdata']['lvalues'][$mc][$l]['beschreibung']=$cls.'~<font class="grey">'.text_wrap($val,true,48).'</font>';

		// ...Icon
		$GLOBALS['fxptdata']['lvalues'][$mc][$l]['icon']='^class_transparent|~';
		if(strlen($icon))
			$GLOBALS['fxptdata']['lvalues'][$mc][$l]['icon'] .= fieldSubmitImage('akt'.$GLOBALS['fxpglobals']['action'].'_'.$spersid.'_'.$v['termin_id'], $icon, '', $tt_act);

		// ...Hidden
		$GLOBALS['fxptdata']['lvalues'][$mc][$l]['nv_text1']=$spersid.'_'.$v['termin_id'];
	}

	if($l)
	{
		$l++;
		fxf_maskColumnBorder($fa, 1, -1, $mc, $l, true);
		$l++;
	}

	// Alle Termine
	$ica=iCalendarCombine(2);

	// Spalten ausblenden?
	if(!$cnt_ganztaegig)
		$ganztaegig_nv=true;
	if(!$cnt_projekt)
		$Projekt_nv=true;
	$einladen_nv=true;	// (vorerst)
	if($GLOBALS['fxpglobals']['tr'] == 222)	// Zeitgenehmigungsverfahren
	{
		$Zeitraum_nv=true;
		$einladen_nv=true;
		$betreff_nv=true;
	}
	if($GLOBALS['fxpglobals']['action'] != FXP_DELETE)
	{
		$Checkbox_nv=true;
		$Button_Multi_nv=true;
	}
}
?>