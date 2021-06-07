<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : std_rep_datum_pre.inc                                        //
// Version     : 21.1                                                         //
// Begin       : 2020-11-11                                                   //
// Last Change : 2020-11-11                                                   //
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
 * 57: Time Sheets
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.1
 */

if(!isset($GLOBALS['app_wd']))
	$GLOBALS['app_wd']=0;

// Maskennummer
if(!isset($GLOBALS['srd_mnr']))
	$GLOBALS['srd_mnr']=193;

// Personen in Array einlesen
$number_persons=0;
if(!$err && $Button_Anzeigen)
{
	$pids='';
	if(!$Personen_ID && is_array($Pers_Arr))
		$pids=array_to_csv($Pers_Arr);
	else if($Personen_ID)
		$pids=$Personen_ID;

	if(strlen($pids))
	{
		$sql  = "SELECT";
		$sql .= " p.personen_id, p.pname, p.vorname, p.geschlecht, p.personentyp,";
		$sql .= " m.maeintrittsdatum, m.maaustrittsdatum, m.wochenarbeitstd, m.arbeitstage, m.soll_beginn, m.pause,";
		$sql .= " m.sonntag as w0, m.montag AS w1, m.dienstag AS w2, m.mittwoch AS w3, m.donnerstag AS w4, m.freitag AS w5, m.samstag AS w6 ";
		$sql .= "FROM";
		$sql .= " personen p, madaten m ";
		$sql .= "WHERE";
		$sql .= " p.personen_id IN (".$pids.") AND m.personen_id=p.personen_id AND p.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND m.mandanten_id=".$GLOBALS['fxpglobals']['client']." ";
		$sql .= "ORDER BY";
		$sql .= " p.pname, p.vorname";
		$pers_datas=db_values($sql, '*');
//fxDebug($pers_datas, $sql);
		if(fxIsArray($pers_datas))
			$number_persons=sizeof($pers_datas);
	}
}

if($number_persons > 0)
{
	$mc=$GLOBALS['_maskcounter'];

	${'maske'.$GLOBALS['srd_mnr'].'_nw'}=true;
	if(isset($GLOBALS['fxptdata']['lvalues']) && isset($GLOBALS['fxptdata']['lvalues'][$mc]))
		unset($GLOBALS['fxptdata']['lvalues'][$mc]);

	// Columns
	$fa=fxf_getMaskFields($GLOBALS['srd_mnr'],false,array('icon_aendern'=>0));
//fxDebug($fa,'$fa', 0);

	// No right border in the following column
	fxf_maskColumnBorder(array('icon_aendern'), 0, -1, $mc);

	// Berechnungs-Startdatum muss immer ein Monatserster sein wegen korrektem Übertrag
	$mvon=substr($von,0,6).'01'.substr($von,8);

	// Monatstage
	$mo_days=array(1=>31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31);
	$fart=array(FXP_HL_FULL=>'&nbsp;<font class=s1>[1]</font>', FXP_HL_HALF=>'&nbsp;<font class=s1>[½]</font>', FXP_HL_DISPLAY=>'&nbsp;<font class=s1>[0]</font>');
	$sart=array(1=>'&nbsp;<font class=s1>[1]</font>', 0.5=>'&nbsp;<font class=s1>[½]</font>', 0=>'&nbsp;<font class=s1>[0]</font>');
	$today=substr($GLOBALS['datetime'],0,8);
	$ctime=substr($GLOBALS['datetime'],8,6);
	$yesterday=substr(time_add($today,-1,'D'), 0,8);
	$tomorrow=substr(time_add($today,1,'D'), 0,8);
//echo('['.__LINE__.'] $yesterday='.$yesterday.', $today='.$today.', $tomorrow='.$tomorrow.' -- $ctime='.$ctime.'<br />');
	$nr_days=fxp_date_diff($von, $bis);
	// Monatsnamen
	$month_names=fxf_getReferenceValues('monat');
//fxDebug($month_names, '$month_names');
	// Wochentagsnamen
	$weekday_names=fxf_getWeekdayNames();
	$wda=array('so', 'mo', 'di', 'mi', 'do', 'fr', 'sa', 'so');
//fxDebug($weekday_names, '$weekday_names');
	// Texte
	$t_ext=get_text(1229, '5');				// Ext.
	$t_int=get_text(1225, '5');				// Int.
	$t_uebertrag='&nbsp;<b>'.meldung(1575).'</b>';	// Übertrag
	$t_suebertrag='&nbsp;<b>'.meldung(166).'</b>';	// Zeitkonto-Übertrags-Startwert
	$t_zwischen='<b>'.meldung(1436).'</b>';	// Zwischensumme
	$t_gesamt='<b>'.meldung(1555).'</b>';	// Gesamtsumme
	$t_auszahlung='&nbsp;&middot;&nbsp;<b>'.meldung(1943).'</b>';	// Überstundenauszahlung

	$htsf_array=get_uskg_array();
	$htsf_var_array=array(
		FXP_AT_VACATIONREQUEST=>'urlaub',				FXP_AT_VACATION=>'urlaub',
		FXP_AT_SPECIALVACATIONREQUEST=>'sonderurlaub',	FXP_AT_SPECIALVACATION=>'sonderurlaub',
		FXP_AT_TRAININGREQUEST=>'schulungstage',		FXP_AT_TRAINING=>'schulungstage',
		FXP_AT_SICKCALL=>'krankheitstage',				FXP_AT_SICK=>'krankheitstage',
		FXP_AT_ABSENT=>'abwesenheit',
		FXP_AT_SLIDINGDAYREQUEST=>'gleitzeitstunden',	FXP_AT_SLIDINGDAY=>'gleitzeitstunden'
	);
//fxDebug($pers_datas, $number_persons.' Personen im Zeitraum '.$von.' - '.$bis.' ['.$nr_days.' Tage]');

	$dg_rep=array('sdatum'=>'ist_beginn', 'edatum'=>'ist_ende');

	// Anzeigemodus ermitteln -> $zm: 1=Tag, 2=Monat, 3=Jahr
	if(!$Anzeigeart) // Automatisch
	{
		if($nr_days < 40)			// Tag(e)
			$zm=1;
		else if($nr_days < 1000)	// Monat(e)
			$zm=2;
		else						// Jahr(e)
			$zm=3;
	}
	else
		$zm=$Anzeigeart-916;		// je nach Auswahl

	// Colspan
	$cs=13;
	if(($zm > 1) || !$cb_stunden)
	{
		$lcs=2;
		$cs--;
		$Zeit_ID_nv=true;
	}
	else
		$lcs=3;

	$efield='';
	foreach($cb_fields as $cb_field=> $list_field)
	{
		$field=strtolower($cb_field);
		if($field != 'leer_anzeigen')
		{
			if(!${'cb_'.$field})
			{
				$cs--;
				${$list_field.'_nv'}=true;
				if($field == 'stunden')
				{
					$cs -= 2;
					$ISTStunden_nv=true;
					$Delta_nv=true;
				}
			}
			else if($field == 'stunden')
				$efield='Delta';
			else
				$efield=$list_field;
		}
	}
	$lfield=strtolower($efield);
//echo('$cs='.$cs.', $efield='.$efield.', $lfield='.$lfield.'<hr />');

	// Farben
	$ca=array('summe'=>'col_c2c2c2', 'uebertrag'=>'col_c2c2ca', 'auszahlung'=>'col_d2dad2');

	// Feiertage ermitteln
	$feiertage=get_feiertage($von,$bis);
//fxDebug($feiertage,'$feiertage');

	// Berechtigte Projekte ermitteln
	// ...Admin, GF und VW
	if(!$GLOBALS['fxpglobals']['person'] || ($GLOBALS['fxpglobals']['user'] == 1) || $GLOBALS['fxpglobals']['persdat']['roles'][2] || $GLOBALS['fxpglobals']['persdat']['roles'][-1])
		$prjsql="z.projekt_id>=0";
	// ...Rest
	else
	{
		$prjsql  = "z.personen_id=".$GLOBALS['fxpglobals']['person'];
		$prjsql .= " OR z.projekt_id IN (".prights_get(0, 0, true).")";
	}
//fxDebug($prjsql,'$prjsql');

	// IST-Stunden aller Personen zu berechtigten Aufgaben (ohne Gleittage, da dort projekt_id=-1)...
	$ia=array();
	$istsql  = " FROM zeitdaten z, personen p";
	$istsql .= " WHERE p.personen_id=z.personen_id AND z.personen_id IN (".$pids.") AND ((".$prjsql.") AND z.projekt_id>=0)";
	$istsql .= " AND z.zeitart IN (".FXP_TE_TIMEREC.",".FXP_TE_TIMEREC_SUM.",".FXP_TE_TRAVELEXP_TIMEREC.") AND z.zeitaufwand IS NOT NULL AND z.zeitaufwand>0 AND (z.ist_beginn<='".$bis."' AND z.ist_ende>='".$von."')";
	$istsql .= " AND z.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND p.mandanten_id=".$GLOBALS['fxpglobals']['client'];

	// ...ermitteln
	$sql="SELECT z.personen_id, z.projekt_id, z.ist_beginn, z.ist_ende, z.zeitaufwand, z.zeitart".$istsql." ORDER BY p.pname, p.vorname, z.ist_beginn";
	$ist=db_values($sql,'*');
//fxDebug($ist, $sql);
	if(fxIsArray($ist))
	{
		foreach($ist as $isa)
		{
			$ip=$isa['personen_id'];
			$is=substr($isa['ist_beginn'],0,8);
			if(!isset($ia[$ip]))
				$ia[$ip]=array();
			$ia[$ip][$is]=true;
		}
	}
//fxDebug($ia,'$ia');

	// Startwochentag
	$fd=(int)$GLOBALS['fxpglobals']['settings']['ersterwt'];

	// Gesamtsummen aller Personen [evtl. je nach dummymode (0=Aktuell, 1=Vergangenheit + 2=Zukunft)]
	$a_disp=false;
	$a_arbeitstage0=0.0; $a_arbeitstage1=0.0; $a_arbeitstage2=0.0;
	$a_urlaub=0.0;
	$a_sonderurlaub=0.0;
	$a_schulungstage=0.0;
	$a_krankheitstage=0.0;
	$a_abwesenheit=0.0;
	$a_gleitzeitstunden=0;
	$a_sollstunden0=0; $a_sollstunden1=0; $a_sollstunden2=0;
	$a_iststunden0=0; $a_iststunden1=0; $a_iststunden2=0;
	$a_delta0=0; $a_delta1=0; $a_delta2=0;

	$ln=0;
	// Alle Personen durchgehen
	foreach($pers_datas as $pers)
	{
//fxDebug($pers,'['.__LINE__.'] $pers');
		$persid=(int)$pers['personen_id'];
		if(!isset($ia[$persid]))
			$ia[$persid]=array();
		$fullname=$pers['pname'];
		if(strlen(trim($pers['vorname'])))
			$fullname .= ', '.trim($pers['vorname']);

		$pset=false;

		// Maskentitel
		if(!$ln)
		{
			if($number_persons > 1)
				${'maske'.$GLOBALS['srd_mnr'].'_head'}=meldung(1415, false, meldung(1410, false), get_entry($von, 'datum'), get_entry($bis, 'datum'), (1+$nr_days));
			else
				${'maske'.$GLOBALS['srd_mnr'].'_head'}=meldung(1415, false, $fullname, get_entry($von, 'datum'), get_entry($bis, 'datum'), (1+$nr_days));

			// Monatsanzeige in Anzeigeart Tag(e)?
			if(($zm == 1) && (substr($von,0,6) == substr($bis,0,6)) && (substr($von,6,2) == '01') && (substr($bis,6,2) == substr(ultimo($bis),6,2)))
			{
				$ay=(int)substr($von,0,4);
				$am=(int)substr($von,4,2);
				$ad=(int)substr($von,6,2);

//function addNavigation($y, $m=0, $w=0, $d=0, $f='right', $trlnk=false)
				${'maske'.$GLOBALS['srd_mnr'].'_head'} .= addNavigation($ay,$am,0,0,'right',true);
			}
		}

		// Ausgetretene oder noch nicht eingetretene Personen nicht anzeigen
		if(($number_persons > 1) && (($pers['maeintrittsdatum'] > $bis) || ($pers['maaustrittsdatum'] && ($pers['maaustrittsdatum'] < $von))))
			continue;

		// Soll-Sekunden, Arbeitsbeginn und -ende
		$soll_sekunden_tag=0;
		if($pers['arbeitstage'] > 0)
			$soll_sekunden_tag=($pers['wochenarbeitstd']/$pers['arbeitstage'])*3600;
		$wday_start=substr($pers['soll_beginn'],8,6);
		$wday_end=((int)substr($wday_start,0,2)*3600)+((int)substr($wday_start,2,2)*60)+(int)substr($wday_start,4) + (int)$pers['pause'] + $soll_sekunden_tag;
//sec2time($sec, $alz=true, $ds=false)
		$wday_end=sec2time($wday_end,true,true,'');
//echo('['.__LINE__.'] $soll_sekunden_tag='.$soll_sekunden_tag.', $wday_start='.$wday_start.', $wday_end='.$wday_end.'<br />'.$nl);

		// Zusatzdaten ermitteln
		$psdate=substr($pers['maeintrittsdatum'], 0, 8);
		$pedate=substr($pers['maaustrittsdatum'], 0, 8);
		if(!strlen($pedate))
			$pedate='99999999';
		$rsdate=$psdate;

		$startsum=summe_zeitkonto($persid,$von);

		// Startbilanz
		$rstartsum='';
		$sql="SELECT zeitaufwand, ist_beginn FROM zeitdaten WHERE zeitart=105 AND personen_id=".$persid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$sbil=db_values($sql);
//fxDebug($sbil,'['.__LINE__.'] $sbil: $sql='.$sql);
		if(fxIsArray($sbil))
		{
			$sj=substr($sbil['ist_beginn'], 0, 4);
			$sm=substr($sbil['ist_beginn'], 4, 2);
			$sm++;
			if($sm > 12)
			{
				$sm=1;
				$sj++;
			}

			$rsdate=substr('0000'.$sj, -4).substr('00'.$sm, -2).'01';
			$rstartsum=(int)$sbil['zeitaufwand'];
		}
		else if(strlen($GLOBALS['fxpglobals']['settings']['zk_ueb_jahr']))
		{
			$tmpdate=substr('0000'.$GLOBALS['fxpglobals']['settings']['zk_ueb_jahr'], -4). substr('00'.$GLOBALS['fxpglobals']['settings']['zk_ueb_monat'], -2).'01';
			if(!$rsdate || ($tmpdate > $rsdate))
				$rsdate=$tmpdate;
			$rstartsum=0;
		}
		if($ctime > $wday_end)
			$redate=max($rsdate, min($today, $pedate));
		else
			$redate=max($rsdate, min($yesterday, $pedate));
//echo('['.__LINE__.'] $psdate: '.$psdate.', $pedate: '.$pedate.' -- $rsdate: '.$rsdate.', $redate: '.$redate.' ($rstartsum: '.$rstartsum.', $startsum: '.$startsum.')<br />');
		// Sondertage
		$htsf_days=get_sondertage($von, $bis, $persid, 0, false, false);
//fxDebug($htsf_days, '$htsf_days');
//function getPersonWorkdata($pid=0, $ssd='', $sed='', $extended=false, $ret_array=false, $set_debug=0)
		// Überstundenauszahlung
		$oop=overtime_outpayment($persid, $von,$bis);

		// Starttag aufsplitten
		$year=(int)substr($von,0,4);
		$month=(int)substr($von,4,2);
		$day=(int)substr($von,6,2);

		$lightyear=fx_date('L', fx_mktime(12, 0, 0, 1, 1, $year));
		if($lightyear)
			$mo_days[2]=29;

		// Startwochentag ermitteln
		$weekday=fx_date('w', fx_mktime(12, 0, 0, $month, $day, $year));
//echo('['.__LINE__.'] <b>START:</b> $year='.$year.', $month='.$month.', $day='.$day.' - $weekday='.$weekday.'<br />'.$nl);

		$dummymoder=-1;

		// Zwischensummen einer Person [evtl. je nach dummymode (0=Aktuell, 1=Vergangenheit + 2=Zukunft)]
		$s_disp=false;
		$s_mindate=''; $s_maxdate='';
		$s_arbeitstage0=0.0; $s_arbeitstage1=0.0; $s_arbeitstage2=0.0;
		$s_urlaub=0.0;
		$s_sonderurlaub=0.0;
		$s_schulungstage=0.0;
		$s_krankheitstage=0.0;
		$s_abwesenheit=0.0;
		$s_gleitzeitstunden=0;
		$s_sollstunden0=0; $s_sollstunden1=0; $s_sollstunden2=0;
		$s_iststunden0=0; $s_iststunden1=0; $s_iststunden2=0;
		$s_delta0=0; $s_delta1=0; $s_delta2=0;

		// Gesamtsummen einer Person [evtl. je nach dummymode (0=Aktuell, 1=Vergangenheit + 2=Zukunft)]
		$t_disp=false;
		$t_arbeitstage0=0.0; $t_arbeitstage1=0.0; $t_arbeitstage2=0.0;
		$t_urlaub=0.0;
		$t_sonderurlaub=0.0;
		$t_schulungstage=0.0;
		$t_krankheitstage=0.0;
		$t_abwesenheit=0.0;
		$t_gleitzeitstunden=0;
		$t_sollstunden0=0; $t_sollstunden1=0; $t_sollstunden2=0;
		$t_iststunden0=0; $t_iststunden1=0; $t_iststunden2=0;
		$t_delta0=0; $t_delta1=0; $t_delta2=0;

		$sdate=substr($von,0,8);
		$edate=substr($bis,0,8);
		$vdate=substr($von,0,8);

		$show_kapp=false;
		$k_sollstunden=0;
		$k_iststunden=0;
		$ksdate=$sdate;

		$pln=$ln;

		// Leerzeile
		leerzeile();
		$lset=false;

		// Name
		if($number_persons > 1)
		{
			$txt='&nbsp;'.person_pic($persid, 20, $pers['geschlecht'], $GLOBALS['fxpglobals']['client'], false).'&nbsp;<font class="s4b">'.$fullname.'</font>&nbsp;<i class="s2 grey">[';
			if($pers['personentyp'] == FXP_PT_CONTRACTOR)
				$txt .= $t_ext;
			else
				$txt .= $t_int;
			$txt .= ']</i>';
			fxf_listSectHeader($mc, $ln, $txt, 'tage', $lfield, $cs);
			$ln++;
		}
		$sln=$ln;

		// Übertrag
		$uln='';
		if(($sdate >= $psdate) && ($sdate >= $rsdate) && !isset($Delta_nv))
		{
			if(($sdate == $rsdate) && strlen($rstartsum))
				$startsum=0.0;
			else
			{
				leerzeile();
				$pset=false;

				$uln=$ln;
				$utxt=$t_uebertrag.' <i>(- '.get_entry($vdate, 'datum').')</i>';
				$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['tage']='^'.$ca['uebertrag'].'" nowrap colspan="'.($cs-1).'"><!--|~'.$utxt;
				$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['iststunden']='~-->'.$utxt;
				$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['delta']='^'.$ca['uebertrag'].'|~'.gVal($startsum,'','','h');
				$ln++;

				$s_delta0=$startsum;
				$t_delta0=$startsum;
			}
		}

		// Jeden Tag durchgehen
		$dc=0;
		$sc=false;
		while($sdate <= $edate)
		{
			$asdate=$sdate;
			$wsdate=$asdate;
			if($zm == 2)
				$aedate=substr('0000'.$year, -4).substr('00'.$month, -2).substr('00'.$mo_days[$month], -2);
			else if($zm == 3)
				$aedate=substr('0000'.$year, -4).'1231';
			else
				$aedate=$asdate;
			$aedate=min($aedate, $edate);
//echo('['.__LINE__.'] <b>ACT DAY:</b> $year='.$year.', $month='.$month.', $day='.$day.' - $weekday='.$weekday.' -- $asdate='.$asdate.', $aedate='.$aedate.'<br />'.$nl);

			// Zeilensummen je nach Modus (Tag/Monat/Jahr) einer Person [evtl. je nach dummymode (0=Aktuell, 1=Vergangenheit + 2=Zukunft)]
			$l_arbeitstage0=0.0; $l_arbeitstage1=0.0; $l_arbeitstage2=0.0;
			$l_urlaub=0.0;
			$l_sonderurlaub=0.0;
			$l_schulungstage=0.0;
			$l_krankheitstage=0.0;
			$l_abwesenheit=0.0;
			$l_gleitzeitstunden=0;
			$l_sollstunden0=0; $l_sollstunden1=0; $l_sollstunden2=0;
			$l_iststunden0=0; $l_iststunden1=0; $l_iststunden2=0;
			$l_delta0=0; $l_delta1=0; $l_delta2=0;

			$l_txt='';
			$l_soll_sekunden_tag=array();
			$bgcol='';
			$tooltip='';

			$ayear=$year;
			$amonth=$month;
			$aday=$day;
			$aweekday=$weekday;

			while($wsdate <= $aedate)
			{
//echo('['.__LINE__.'] <b>'.$month.'</b>: $wsdate='.$wsdate.' -- $asdate='.$asdate.'-$aedate='.$aedate.' -- $psdate='.$psdate.'-$pedate='.$pedate.' -- $rsdate='.$rsdate.'-$redate='.$redate.'<br />'.$nl);
				if($weekday == $fd)
					$sc=true;

				// Dummywerte
				if((($wsdate < $psdate) || ($wsdate > $pedate)) && !isset($ia[$persid][$wsdate]))
					$dummymode=3;
				else if(($wsdate > $redate) && !isset($ia[$persid][$wsdate]))
					$dummymode=2;
				else if($wsdate < $rsdate)
					$dummymode=1;
				else
					$dummymode=0;
				if(($dummymode < 3) && ($schaetzung == 2))	// Schätzwerte als reale Werte behandeln
					$dummymode=0;
//if($dummymode != $dummymoder) echo('['.__LINE__.'] $wsdate='.$wsdate.': $dummymode='.$dummymode.'<br>');
				$dummymoder=$dummymode;

				// Startübertrag
				if(($wsdate == $rsdate) && strlen($rstartsum) && !isset($Delta_nv))
				{
					if($asdate == $rsdate)
					{
						leerzeile();
						$pset=false;

						$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['tage']='^'.$ca['uebertrag'].'" nowrap colspan="'.($cs-1).'"><!--|~'.$t_suebertrag;
						$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['iststunden']='~-->'.$t_suebertrag;
						$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['delta']='^'.$ca['uebertrag'].'|~'.gVal($rstartsum,'','','h');
						$ln++;
					}

					// ...evtl. je nach dummymode (0=Aktuell + 2=Zukunft)
					$l_sollstunden0=0; $l_sollstunden2=0;
					$l_iststunden0=0; $l_iststunden2=0;
					$l_delta0=0; $l_delta1=0; $l_delta2=0;

					$s_sollstunden0=0; $s_sollstunden2=0;
					$s_iststunden0=0; $s_iststunden2=0;
					$s_delta0=$rstartsum; $s_delta1=0; $s_delta2=0;

					$t_sollstunden0=0; $t_sollstunden2=0;
					$t_iststunden0=0; $t_iststunden2=0;
					$t_delta0=$rstartsum; $t_delta1=0; $t_delta2=0;

					$rstartsum='';
				}

				// Arbeitstage
				$swa=getPersonWorkdata($persid,$wsdate,$wsdate,$wda[$weekday]);
//fxDebug($swa,'['.__LINE__.'] $swa=getPersonWorkdata($persid='.$persid.',$wsdate='.$wsdate.',$wsdate='.$wsdate.',$wda[$weekday='.$weekday.']='.$wda[$weekday].')');
				$workday=$swa['wd'];
				$soll_sekunden_tag=$swa['st']*3600;
				$l_soll_sekunden_tag[$soll_sekunden_tag]=true;
				// ...Feiertag
				if(fxIsArray($feiertage[$wsdate]) && ($feiertage[$wsdate]['typ'] > 0.0))
				{
					if($feiertage[$wsdate]['art'] == FXP_HL_HALF)		// Halber Feiertag
						$workday=max(0.0, $workday-0.50);
					else if($feiertage[$wsdate]['art'] == FXP_HL_FULL)	// Ganzer Feiertag
						$workday=0.0;
				}
				// ...Sondertage
				if(($workday > 0.0) && fxIsArray($htsf_days[$wsdate]) && ($htsf_days[$wsdate][-1]['typ'] != FXP_AT_SLIDINGDAYREQUEST) && ($htsf_days[$wsdate][-1]['typ'] != FXP_AT_SLIDINGDAY))
					$workday=max(0.0, $workday-$htsf_days[$wsdate][-1]['dauer']);

				${'l_arbeitstage'.$dummymode} += $workday;
				${'l_sollstunden'.$dummymode} += ($workday*$soll_sekunden_tag);
//echo('['.__LINE__.'] $workday='.$workday.', $soll_sekunden_tag='.$soll_sekunden_tag.' -- $l_arbeitstage'.$dummymode.'='.${'l_arbeitstage'.$dummymode}.', $l_sollstunden'.$dummymode.'='.${'l_sollstunden'.$dummymode}.'<br />'.$nl);

				// ...Farbcodierung für Tagesanzeige
				if($zm == 1)
				{
					// ...Sondertage
					if(fxIsArray($htsf_days[$wsdate]))
					{
						$img='';
						if(fxIsArray($feiertage[$wsdate]) && ($feiertage[$wsdate]['typ'] > 0.0))
						{
							$img='&nbsp;<img src="'.$GLOBALS['gfxpath'].'tts.png" align="top">';
							if($feiertage[$wsdate]['art'] == FXP_HL_HALF)	// Halber Feiertag
								$tooltip=' tooltip="+ '.$feiertage[$wsdate]['name'].$fart[$feiertage[$wsdate]['art']].'" style="cursor:help;"';
							else
								$tooltip=' tooltip="'.$feiertage[$wsdate]['name'].'" style="cursor:help;"';
						}
						$ac=getAbsentClass($htsf_days[$wsdate][-1]['typ']);
						$cls=$ac['cls'];
						$bgcol='~<span class="'.$cls.'"'.$tooltip.'>'.$htsf_array[$htsf_days[$wsdate][-1]['typ']].$sart[$htsf_days[$wsdate][-1]['dauer']].$img.'</span>';
					}
					// ...Feiertage
					else if(fxIsArray($feiertage[$wsdate]) && ($feiertage[$wsdate]['typ'] > 0.0))
						$bgcol='~<span class="blue">'.$feiertage[$wsdate]['name'].$fart[$feiertage[$wsdate]['art']].'</span>';
					else
					{
						$bo='';
						$be='';
						if($dummymode)
						{
							$bo='<i class="lightgrey">(';
							$be=')</i>';
						}

						if(!$workday)
							$bgcol='~<span class="grey" style="border:1px solid #cccccc;border-radius:6px;">&nbsp;'.$htsf_array['frei'].'&nbsp;</span>';
						else
							$bgcol='~'.$bo.$htsf_array['arbeit'].$be;
					}
				}

				// Urlaub/Sonderurlaub/Schulung/Krank/Abwesend/Gleittag
				if(fxIsArray($htsf_days[$wsdate]))
				{
					foreach($htsf_var_array as $htsf_typ=> $htsf_var)
					{
						if(is_array($htsf_days[$wsdate][$htsf_typ]))
						{
							if(($htsf_typ == FXP_AT_SLIDINGDAYREQUEST) || ($htsf_typ == FXP_AT_SLIDINGDAY))
								${'l_'.$htsf_var} += $htsf_days[$wsdate][$htsf_typ][-1]['stunden_termin']*3600.0;
							else
								${'l_'.$htsf_var} += $htsf_days[$wsdate][$htsf_typ][-1]['dauer'];
						}
					}
				}

				// IST-Stunden
				$aist=0;
				$r_zvon='';
				$r_zbis='';
				$r_sum=false;
				if(fxIsArray($ist))
				{
					foreach($ist as $iln => $id)
					{
						$daydate=substr($id['ist_beginn'],0,8);
						if(($daydate < $wsdate) || ($id['personen_id'] != $persid))
							continue;
						else if($daydate > $wsdate)
							break;

						$ibz=substr($id['ist_beginn'], 8);
						$iez=substr($id['ist_ende'], 8);
						if($ibz != $iez)
						{
							if($iez == '000000')
								$iez='240000';
							if(!$r_zvon || ($ibz < $r_zvon))
								$r_zvon=$ibz;
							if(!$r_zbis || ($iez > $r_zbis))
								$r_zbis=$iez;
						}
						$aist += $id['zeitaufwand'];
						if($id['zeitart'] == FXP_TE_TIMEREC_SUM)
							$r_sum=true;

						unset($ist[$iln]);
					}
				}
				$k_iststunden += $aist;
				$k_sollstunden += $workday*$soll_sekunden_tag;
				if($dummymode == 2)
					$l_iststunden2 += max($aist,$workday*$soll_sekunden_tag);
				else
					${'l_iststunden'.$dummymode} += $aist;
//echo('['.__LINE__.'] &rarr; $aist='.$aist.', $k_iststunden='.$k_iststunden.' ('.get_entry($k_iststunden/3600,'dezimal').' h), $k_sollstunden='.$k_sollstunden.' ('.get_entry($k_sollstunden/3600,'dezimal').' h), $_iststunden'.$dummymode.'='.${'l_iststunden'.$dummymode}.' ('.get_entry(${'l_iststunden'.$dummymode}/3600,'dezimal').' h)<br />'.$nl);

				// Nächster Tag
				$weekday++;
				if($weekday > 6)
					$weekday=0;

				$day++;
				if($day > $mo_days[$month])
				{
//echo('['.__LINE__.'] <b>'.$month.'</b>: $ksdate='.$ksdate.', $k_iststunden='.$k_iststunden.' ('.get_entry($k_iststunden/3600,'dezimal').' h), $k_sollstunden='.$k_sollstunden.' ('.get_entry($k_sollstunden/3600,'dezimal').' h)<br />'.$nl);
					if(!$dummymode && is_array($karr) && sizeof($karr))
					{
						foreach($karr as $kn=> $ka)
						{
							if((substr($ka['soll_beginn'],0,8) <= $asdate) && (substr($ka['soll_ende'],0,8) >= $asdate))
							{
								if(substr($ksdate,6,2) != '01')
								{
									$epia=getExtPersInfo($persid, substr($ksdate,0,6).'01', substr($ksdate,0,6).($day-1), array(), true);
//fxDebug($epia,'['.__LINE__.'] getExtPersInfo('.$persid.', '.substr($ksdate,0,6).'01, '.substr($ksdate,0,6).$day.')');
									$k_sollstunden=0;
									$k_iststunden=0;
									if(fxIsArray($epia) && fxIsArray($epia['days']))
									{
										foreach($epia['days'] as $epi)
										{
											$k_sollstunden += (int)($epi['h']*3600);
											$k_iststunden += $epi['ws'];
										}
									}
								}
								$show_kapp=true;
								$k_delta=$k_iststunden-$k_sollstunden;
//echo('['.__LINE__.'] <b>'.$month.'</b>: $k_delta='.$k_delta.' (= '.get_entry($k_delta/3600,'dezimal').' h), $k_month='.$k_month.' (= '.get_entry($k_month/3600,'dezimal').' h)<hr />'.$nl);
							}
						}
					}
					$k_sollstunden=0;
					$k_iststunden=0;

					$day=1;
					$month++;
					if($month > 12)
					{
						$month=1;
						$year++;
						$lightyear=fx_date('L', fx_mktime(12, 0, 0, 1, 1, $year));
						if($lightyear)
							$mo_days[2]=29;
						else
							$mo_days[2]=28;
						$ksdate=substr('0000'.$year, -4).substr('00'.$month, -2).substr('00'.$day, -2);
					}
				}

				if($wsdate < $vdate)
				{
					if(($l_delta0 || strlen($uln)) && !isset($Delta_nv))
					{
						$startsum += $l_delta0;
						if(!strlen($uln))
						{
							leerzeile();
							$pset=false;

							$uln=$ln;
							$utxt=$t_uebertrag.' <i>(- '.get_entry($vdate, 'datum').')</i>';
							$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['tage']='^'.$ca['uebertrag'].'" nowrap colspan="'.($cs-1).'"><!--|~'.$utxt;
							$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['iststunden']='~-->'.$utxt;
							$ln++;
						}
						$GLOBALS['fxptdata']['lvalues'][$mc][$uln]['delta']='^'.$ca['uebertrag'].'|~'.gVal($startsum,'','','h');

						$s_delta0=$startsum;
						$t_delta0=$startsum;
					}

					$l_delta0=0;
					$l_delta1=0;
					$l_delta2=0;
				}

				$wsdate=substr('0000'.$year, -4).substr('00'.$month, -2).substr('00'.$day, -2);
			}

			// Werte anzeigen
			if($wsdate > $vdate)
			{
				$l_disp=false;
				if($cb_leer_anzeigen && (!$Delta_nv || ($zm == 1)))
					$l_disp=true;
				else
				{
//echo('['.__LINE__.'] <b>'.$wsdate.'</b><br />- arbeitstage: 0='.${'l_arbeitstage0'}.', 1='.${'l_arbeitstage1'}.', 2='.${'l_arbeitstage2'}.'<br />- sollstunden: 0='.${'l_sollstunden0'}.', 1='.${'l_sollstunden1'}.', 2='.${'l_sollstunden2'}.'<br />- iststunden: 0='.${'l_iststunden0'}.', 1='.${'l_iststunden1'}.', 2='.${'l_iststunden2'}.'<hr />');
					foreach($cb_fields as $cb_field=> $list_field)
					{
						if(($cb_field != 'leer_anzeigen') && !${$list_field.'_nv'})
						{
							$field=strtolower($list_field);
							if(($field == 'arbeitstage') && (${'l_arbeitstage0'} || ($schaetzung && (${'l_arbeitstage1'} || ${'l_arbeitstage2'}))))
								$l_disp=true;
							else if(($field == 'sollstunden') && (${'l_sollstunden0'} || ${'l_iststunden0'} || ($schaetzung && (${'l_sollstunden1'} || ${'l_sollstunden2'} || ${'l_iststunden1'} || ${'l_iststunden2'}))))
								$l_disp=true;
							else if(${'l_'.$field})
								$l_disp=true;

							if($l_disp)
								break;
						}
					}
				}
//fxDebug($l_disp, '['.__LINE__.'] '.$aedate.': $l_disp');

				if($l_disp)
				{
					$s_disp=true;
					$t_disp=true;
					$a_disp=true;

					$s_maxdate=$aedate;
					if(!strlen($s_mindate))
						$s_mindate=$s_maxdate;

					if((($zm == 1) && ($asdate == $today)) || (($zm == 2) && (substr($asdate,0,6) == substr($today,0,6))) || (($zm == 3) && (substr($asdate,0,4) == substr($today,0,4))))	// Aktueller Tag/Monat/Jahr
						$cls=' class="green"';
					else if($dummymode > 1)
						$cls=' class="lightergrey"';
					else if($dummymode)
						$cls=' class="lightgrey"';
					else if(($zm == 1) && (($aweekday == 0) || ($aweekday == 6)))	// Wochenende
						$cls=' class="grey"';
					else
						$cls=' class="black"';

					if($zm == 1)
					{
						if(($ln > $sln) && (!$dc || ($dc && $sc)))
						{
							leerzeile();
							$sc=false;
						}

						$GLOBALS['maske'.$GLOBALS['srd_mnr'].'_trheight_'.$ln]=18;
//						if(!isset($GLOBALS['no_efield']))
//							$GLOBALS[$efield.$GLOBALS['srd_mnr'].'_'.$ln.'_ffs']=10;

						$dc++;
						$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['tage']='~<table cellpadding=0 cellspacing=0><tr><td width=56><b'.$cls.'>'.substr($weekday_names[$aweekday],0,2).'</b></td><td>&nbsp;<span'.$cls.'>'.get_entry($asdate,'datum').'</span></td></tr></table>';
						if(${'l_iststunden'.$dummymode} && (strlen($r_zvon) || strlen($r_zbis) || $r_sum))
						{
							if($r_sum)
								$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['zeit_id']='~&sum;';
							else
							{
								if(!strlen($r_zvon))
									$r_zvon='000000';
								if(!strlen($r_zbis))
									$r_zbis='240000';
								$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['zeit_id']='~'.get_entry($asdate.$r_zvon, 'zeit').'-'.get_entry($asdate.$r_zbis, 'zeit');
							}
						}
						else
							$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['zeit_id']='~';
						$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['datum']=$bgcol;
					}
					else
					{
						if(($ln > $sln) && $lset)
						{
							leerzeile();
							$lset=false;
						}

						$GLOBALS['maske'.$GLOBALS['srd_mnr'].'_trheight_'.$ln]=18;
//						if(!isset($GLOBALS['no_efield']))
//							$GLOBALS[$efield.$GLOBALS['srd_mnr'].'_'.$ln.'_ffs']=10;

						if($zm == 2)
							$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['tage']='~<b'.$cls.'>'.$month_names[$amonth].'</b>&nbsp;<span'.$cls.'>'.$ayear.'</span>';
						else
							$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['tage']='~<b'.$cls.'>'.$ayear.'</b>';
						$fdate=$asdate;
						if($fdate < $vdate)
							$fdate=$vdate;
						$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['datum']='~<font class="grey">'.get_entry($fdate, 'datum');
						if($aedate != $fdate)
							$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['datum'] .= '-'.get_entry($aedate, 'datum');
						$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['datum'] .= '</font>';
					}
					$pset=true;

					for($d=0; $d<3; $d++)
						${'l_delta'.$d} = ${'l_iststunden'.$d}-${'l_sollstunden'.$d};

					$GLOBALS['maske'.$GLOBALS['srd_mnr'].'_trheight_'.$ln]=20;
					fillSum57('l', '', '', '');
					$ln++;
				}
			}

			// Zwischen- und Gesamtsummen einer Person aufaddieren
			for($d=2; $d>=0; $d--)
			{
				${'s_arbeitstage'.$d} += ${'l_arbeitstage'.$d};
				${'t_arbeitstage'.$d} += ${'l_arbeitstage'.$d};

				${'s_sollstunden'.$d} += ${'l_sollstunden'.$d};
				${'t_sollstunden'.$d} += ${'l_sollstunden'.$d};

				${'s_iststunden'.$d} += ${'l_iststunden'.$d};
				${'t_iststunden'.$d} += ${'l_iststunden'.$d};

//				${'s_delta'.$d} += ${'l_delta'.$d};
				${'s_delta'.$d}=${'s_iststunden'.$d}-${'s_sollstunden'.$d};
				${'t_delta'.$d} += ${'l_delta'.$d};
			}
			$s_urlaub			+= $l_urlaub;
			$s_sonderurlaub		+= $l_sonderurlaub;
			$s_schulungstage	+= $l_schulungstage;
			$s_krankheitstage	+= $l_krankheitstage;
			$s_abwesenheit		+= $l_abwesenheit;
			$s_gleitzeitstunden	+= $l_gleitzeitstunden;

			$t_urlaub			+= $l_urlaub;
			$t_sonderurlaub		+= $l_sonderurlaub;
			$t_schulungstage	+= $l_schulungstage;
			$t_krankheitstage	+= $l_krankheitstage;
			$t_abwesenheit		+= $l_abwesenheit;
			$t_gleitzeitstunden	+= $l_gleitzeitstunden;

			// Zischensummen
			$show_sum='';
			if(($zm == 1) && (($month != $amonth) || ($wsdate > $edate)))
			{
				if($s_mindate < $vdate)
					$s_mindate=$vdate;
				$show_sum='<b>'.$ayear.',&nbsp;'.$month_names[$amonth].'</b> <i>('.get_entry($s_mindate, 'datum');
				if($s_maxdate != $s_mindate)
					$show_sum .= ' - '.get_entry($s_maxdate, 'datum');
				$show_sum .= ')</i>';
			}
			else if(($zm == 2) && ($year != $ayear))
				$show_sum='<b>'.$ayear.'</b>';

			// Zwischensummen einer Person anzeigen...
			if(strlen($show_sum))
			{
				if($s_disp || $cb_leer_anzeigen)
				{
					$lset=true;
					$GLOBALS['maske'.$GLOBALS['srd_mnr'].'_trheight_'.$ln]=20;
					$GLOBALS['msk_nbcol_'.$GLOBALS['srd_mnr'].'_'.$ln]=$GLOBALS['msk_tbcol'];
					fillSum57('s', $ca['summe'], $ca['summe'], $show_sum);
					fxf_maskColumnBorder($fa, 1, -1, $mc, $ln);
					$ln++;
				}

				// ...und wieder leeren [evtl. je nach dummymode (0=Aktuell, 1=Vergangenheit + 2=Zukunft)]
				$s_disp=false;
				$s_mindate=''; $s_maxdate='';
				$s_arbeitstage0=0.0; $s_arbeitstage1=0.0; $s_arbeitstage2=0.0;
				$s_urlaub=0.0;
				$s_sonderurlaub=0.0;
				$s_schulungstage=0.0;
				$s_krankheitstage=0.0;
				$s_abwesenheit=0.0;
				$s_gleitzeitstunden=0;
				$s_sollstunden0=0; $s_sollstunden1=0; $s_sollstunden2=0;
				$s_iststunden0=0; $s_iststunden1=0; $s_iststunden2=0;
				$s_delta0=0; $s_delta1=0; $s_delta2=0;

				$dc=0;
			}

			$ncs=$cs-1;

			// Überstundenauszahlungen
			if(sizeof($oop) && !$Delta_nv)
			{
				foreach($oop as $odate=> $odata)
				{
					if(($odate >= $asdate) && ($odate <= $aedate))
					{
//fxDebug($odata,'$odata: $odate='.$odate.' ($asdate='.$asdate.', $aedate='.$aedate.')');
						$val=$odata['sec'];
						$uut=$t_auszahlung.'&nbsp;<b>'.$month_names[(int)substr($odate, 4, 2)].'&nbsp;'.$ayear.'</b>';

						$val=-$odata['sec'];
						$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['tage']='^'.$ca['auszahlung'].'" nowrap colspan="'.$ncs.'"><!--|~'.$uut;
						$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['iststunden']='~-->'.$uut;
						if(($odate >= $rsdate) && ($odate >= $psdate) && ($odate <= $redate))
						{
							$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['delta']='^'.$ca['auszahlung'].'|~'.gVal($val,'','','h');

							$t_delta0 += $val;
						}
						else
							$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['delta']='^'.$ca['auszahlung'].'|~'.gVal($val,'','i','h');

						unset($oop[$odate]);
						$ln++;
					}
				}
			}

			$sdate=$wsdate;
		}

		// Gesamtsummen einer Person anzeigen...
		if($t_disp || $cb_leer_anzeigen)
		{
			if($number_persons > 1)
			{
				$GLOBALS['maske'.$GLOBALS['srd_mnr'].'_trheight_'.$ln]=22;
				$GLOBALS['msk_nbcol_'.$GLOBALS['srd_mnr'].'_'.$ln]=$GLOBALS['msk_tbcol'];
				fillSum57('t', 'class_bcr1', 'class_bcr2', $t_zwischen.': '.$fullname);
				fxf_maskColumnBorder($fa, 1, -1, $mc, $ln);
				$ln++;
			}
		}
		else
		{
			while($ln >= $pln)
			{
				unset($GLOBALS['fxptdata']['lvalues'][$mc][$ln]);
				$ln--;
			}
			$ln=$pln;
		}
		// ...und zur Gesamtsummer aller Personen hinzuaddieren
		for($d=2; $d>=0; $d--)
		{
			${'a_arbeitstage'.$d} += ${'t_arbeitstage'.$d};
			${'a_sollstunden'.$d} += ${'t_sollstunden'.$d};
			${'a_iststunden'.$d} += ${'t_iststunden'.$d};
			${'a_delta'.$d} += ${'t_delta'.$d};
		}
		$a_urlaub			+= $t_urlaub;
		$a_sonderurlaub		+= $t_sonderurlaub;
		$a_schulungstage	+= $t_schulungstage;
		$a_krankheitstage	+= $t_krankheitstage;
		$a_abwesenheit		+= $t_abwesenheit;
		$a_gleitzeitstunden	+= $t_gleitzeitstunden;
	}

	// Gesamtsumme aller Personen anzeigen
	if($a_disp || $cb_leer_anzeigen)
	{
		$txt=$t_gesamt;
		if(($number_persons == 1) && !$GLOBALS['app_wd'])
			$txt .= ': '.$fullname;
		$GLOBALS['maske'.$GLOBALS['srd_mnr'].'_trheight_'.$ln]=24;
		$GLOBALS['msk_nbcol_'.$GLOBALS['srd_mnr'].'_'.$ln]=$GLOBALS['msk_tbcol'];
		fillSum57('a', 'class_bcr3 bold', 'class_bcr3 bold', $txt);
		fxf_maskColumnBorder($fa, 1, -1, $mc, $ln);
		$ln++;

		$GLOBALS['msk_nbcol_'.$GLOBALS['srd_mnr'].'_'.$ln]=$GLOBALS['msk_sbcol'];
		fxf_maskColumnBorder($fa, 1, -1, $mc, $ln, true);
		$ln++;
	}
	else
		$GLOBALS['fxptdata']['lvalues'][$mc]=array();

//fxDebug($GLOBALS['fxptdata']['lvalues'][$mc], '$GLOBALS[\'_maskcounter\']='.$mc);
}


////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/**
 * Draw time sheet sum line
 *
 * @param integer $md  - Mandatory parameter:                Mode
 * @param string  $cc1 - Optional parameter (default = ''):  Column color value #1
 * @param string  $cc2 - Optional parameter (default = ''):  Column color value #2
 * @param string  $txt - Optional parameter (default = ''):  Text
 */
function fillSum57($md, $cc1='', $cc2='', $txt='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!isset($GLOBALS['calvalfut']))
		$GLOBALS['calvalfut']=meldung(3025);	// Kalkulatorischer Wert (in der Zukunft)
	if(!isset($GLOBALS['calvalpst']))
		$GLOBALS['calvalpst']=meldung(3253);	// Kalkulatorischer Wert (mit $var1 Sollarbeitsstunden pro Tag in der Vergangenheit)

	$h='';
	if(isset($GLOBALS['maske'.$GLOBALS['srd_mnr'].'_trheight_'.$GLOBALS['ln']]))
		$h=' height="'.$GLOBALS['maske'.$GLOBALS['srd_mnr'].'_trheight_'.$GLOBALS['ln']].'"';

	// Farben
	if(strlen($cc1))
	{
		$cc1='^'.$cc1.'" nowrap colspan="'.$GLOBALS['lcs'].'"'.$h;
		if(isset($GLOBALS['msk_nbcol_'.$GLOBALS['srd_mnr'].'_'.$GLOBALS['ln']]))
			$cc1 .= ' style="padding:1px 4px;border-top:1px solid #'.$GLOBALS['msk_nbcol_'.$GLOBALS['srd_mnr'].'_'.$GLOBALS['ln']].';border-right:1px solid #'.$GLOBALS['msk_nbcol_'.$GLOBALS['srd_mnr'].'_'.$GLOBALS['ln']].'"';
		$cc1 .= '><!--|~';
	}
	else
		$cc1='~';
	if(strlen($cc2))
	{
		$cc2='^'.$cc2;
		if(isset($GLOBALS['msk_nbcol_'.$GLOBALS['srd_mnr'].'_'.$GLOBALS['ln']]))
			$cc2 .= '"'.$h.' style="padding:1px 4px;border-top:1px solid #'.$GLOBALS['msk_nbcol_'.$GLOBALS['srd_mnr'].'_'.$GLOBALS['ln']].';border-right:1px solid #'.$GLOBALS['msk_nbcol_'.$GLOBALS['srd_mnr'].'_'.$GLOBALS['ln']];
		$cc2 .= '|~';
	}
	else
		$cc2='~';

	// Text
	if(strlen($txt))
	{
		$txt='&nbsp;'.$txt.'&nbsp;';
		$GLOBALS['fxptdata']['lvalues'][$GLOBALS['mc']][$GLOBALS['ln']]['tage']=$cc1.$txt;
		$GLOBALS['fxptdata']['lvalues'][$GLOBALS['mc']][$GLOBALS['ln']]['datum']='~-->'.$txt;
	}

	// Normale + Mögliche Schätzwerte (*)
	$arr=array(
		'*arbeitstage'=>'ddezimal',
		'urlaub'=>'ddezimal', 'sonderurlaub'=>'ddezimal', 'schulungstage'=>'ddezimal', 'krankheitstage'=>'ddezimal', 'abwesenheit'=>'ddezimal',
		'gleitzeitstunden'=>'hzeitspanne',
		'*sollstunden'=>'hzeitspanne', '*iststunden'=>'hzeitspanne', '*delta'=>'hzeitspanne'
	);

	// Breite
	$atw=13;
	$siw=15;

	foreach($arr as $field => $uc)
	{
		$unit=substr($uc,0,1);
		$cmod=substr($uc,1);
		$npar='';

		$tw=$siw;
		if($field == '*arbeitstage')
			$tw=$atw;

		if($cmod == 'dezimal')
		{
			$evalue=0.0;
			$nvalue=0.0;
		}
		else
		{
			$evalue=0;
			$nvalue=0;
		}

		// Möglicher Schätzwert?
		$ptt='';
		if(substr($field,0,1) == '*')
		{
			$field=substr($field,1);
			if($GLOBALS[$md.'_'.$field.'2'] && $GLOBALS['schaetzung'])
				$evalue=$GLOBALS[$md.'_'.$field.'2'];
			$nvalue=$GLOBALS[$md.'_'.$field.'0'];
			if(!$nvalue && $GLOBALS[$md.'_'.$field.'1'])
			{
				$nvalue=$GLOBALS[$md.'_'.$field.'1'];
				if($field == 'arbeitstage')
					$npar='l';
				else if($field != 'iststunden')
				{
					$npar='i';
					if(($field == 'sollstunden') && fxIsArray($GLOBALS['l_soll_sekunden_tag']))
					{
						$sst='';
						foreach($GLOBALS['l_soll_sekunden_tag'] as $st => $dummy)
						{
							if(strlen($sst))
								$sst .= ' + ';
							$sst .= get_entry($st/3600,'dezimal');
						}
						$ptt=' style="cursor:help;" tooltip="'.str_replace('$var1',$sst,$GLOBALS['calvalpst']).'"';
					}
				}
			}
		}
		else
			$nvalue=$GLOBALS[$md.'_'.$field];

		$GLOBALS['fxptdata']['lvalues'][$GLOBALS['mc']][$GLOBALS['ln']][$field]=$cc2;
		$etab=false;
		// Schätzwert anzeigen?
		if($evalue)
		{
			$etab=true;
			$GLOBALS['fxptdata']['lvalues'][$GLOBALS['mc']][$GLOBALS['ln']][$field] .= '<table border="0"'.$h.' cellpadding="0" cellspacing="0" style="width:'.$tw.'em;"><tr><td width="55%" align="right" nowrap style="border-right:1px dotted #888888;cursor:help;" tooltip="'.$GLOBALS['calvalfut'].'">';
			$GLOBALS['fxptdata']['lvalues'][$GLOBALS['mc']][$GLOBALS['ln']][$field] .= gVal($evalue,$cmod,'i',$unit).'&nbsp;';
			$GLOBALS['fxptdata']['lvalues'][$GLOBALS['mc']][$GLOBALS['ln']][$field] .= '</td><td width="45%" align="right" nowrap'.$ptt.'>';
			$ptt='';
		}
		// Normalwert anzeigen?
		if(($md != 'l') || $nvalue)
		{
			if(!$etab)
			{
				$etab=true;
				if(strlen($ptt))
					$GLOBALS['fxptdata']['lvalues'][$GLOBALS['mc']][$GLOBALS['ln']][$field] .= '<table border="0"'.$h.' cellpadding="0" cellspacing="0" style="width:'.ceil($tw/2).'em;"><tr><td align="right" nowrap'.$ptt.'>';
				else
					$GLOBALS['fxptdata']['lvalues'][$GLOBALS['mc']][$GLOBALS['ln']][$field] .= '<table border="0"'.$h.' cellpadding="0" cellspacing="0"><tr><td align="right" nowrap>';
			}
			$GLOBALS['fxptdata']['lvalues'][$GLOBALS['mc']][$GLOBALS['ln']][$field] .= '&nbsp;'.gVal($nvalue,$cmod,$npar,$unit);
		}
		if($etab)
			$GLOBALS['fxptdata']['lvalues'][$GLOBALS['mc']][$GLOBALS['ln']][$field] .= '</td></tr></table>';
	}
}
?>