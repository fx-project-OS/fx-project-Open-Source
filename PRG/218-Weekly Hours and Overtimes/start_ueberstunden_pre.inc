<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : start_ueberstunden_pre.inc                                   //
// Version     : 24.1                                                         //
// Begin       : 2020-11-09                                                   //
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
 * PF 218: Weekly Hours and Overtimes
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

$index=true;
setForm(true);
$Monat_ds=false;

$Mitarbeiter_nw=true;
$nv_text1_nw=true;
$Ueberstunden_rest_nw=true;
$Ueberstunden_breite=60;
$Anmerkung_len=128;
$pcol1='#000000';
$pcol2='#7a7a7a';
$ncol1='#e1001a';
$ncol2='#ee7e8c';

// Texte
$t_ext=get_text(1229, '5');	// Ext.
$t_int=get_text(1225, '5');	// Int.
$t_ohne=meldung(388,false);	// Ohne
$t_alte=meldung(1233,false);	// <b>&gt; $var1 ältere Einträge vorhanden &lt;</b><br /><i class=lightgrey>(Klicken zum Anzeigen der kompletten Liste in einem neuen Popup)</i>
$otcva=explode('|', meldung(2959, false));	// Überstundenübertrag gesetzt für|Gesetzt am/um|Ab Monat|Von wem|Wert|Anmerkung|Kalkulierter Übertragswert bei aktuellen Wochenarbeitsstunden von $var1 h|Kalkulierten Übertragswert kopieren
//fxDebug($otcva,'$otcva', 0);

$ochka=explode('|', meldung(3131, false));	// Es wurden noch keine Monatsüberträge gespeichert.|Alle Monatsüberträge sind korrekt.|Die Monatsüberträge ab $var1 müssen neu berechnet werden!
//fxDebug($ochka,'$ochka', 0);

$achka=explode('|', meldung(3354, false));	// Es wurden noch keine Abwesenheitstermine ab $var1 angelegt.|Alle Abwesenheitstermine ab $var1 haben die korrekten Tagesarbeitsstunden von $var2 Std./Tag.|Die Abwesenheitstermine ab $var1 müssen angepasst werden bzgl. der Tagesarbeitsstunden!
//fxDebug($achka,'$achka', 0);

$heute=substr((string)$GLOBALS['datetime'],0,8);

// Alle internen (275) und externen (840) Mitarbeiter ermitteln mit eingestellten Auswahlkriterien
//	$GLOBALS['ignore_personentyp']=true;
$sql  = "SELECT p.personen_id, p.pname, p.vorname, p.geschlecht, p.personentyp, p.inaktiv, m.maeintrittsdatum, m.maaustrittsdatum, m.wochenarbeitstd, m.arbeitstage FROM personen p, madaten m";
$sql .= " WHERE p.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND p.personentyp IN (275,840) AND m.personen_id=p.personen_id AND m.mandanten_id=".$GLOBALS['fxpglobals']['client'];
// ...Auswahlkriterium "Suchtext"
if(strlen((string)$suchtext))
	$sql .= " AND (".db_like('p.pname','%'.$suchtext.'%')." OR ".db_like('p.vorname','%'.$suchtext.'%').")";
// ...Auswahlkriterium "Abteilung"
if($abt_id > 0)
{
	$acsv=getDepartments($GLOBALS['fxpglobals']['client'], $abt_id);
	$sql .= " AND p.abt_id IN (".$acsv.")";
}
// ...Auswahlkriterien "Zeitspanne von/bis"
if(strlen((string)$zeitspanne_von) && strlen((string)$zeitspanne_bis))
	$sql .= " AND m.maeintrittsdatum<='".$zeitspanne_bis."240000' AND (m.maaustrittsdatum IS NULL OR m.maaustrittsdatum>='".$zeitspanne_von."000000')";
else if(strlen((string)$zeitspanne_von))
	$sql .= " AND m.maeintrittsdatum>='".$zeitspanne_von."000000'";
else if(strlen((string)$zeitspanne_bis))
	$sql .= " AND (m.maaustrittsdatum IS NOT NULL AND m.maaustrittsdatum<='".$zeitspanne_bis."240000')";
// ...Auswahlkriterium "Aktive Mitarbeiter"
if($akt_ma)
	$sql .= " AND (p.inaktiv IS NULL OR p.inaktiv=0) AND (m.maaustrittsdatum IS NULL OR m.maaustrittsdatum>='".$heute."000000')";
//	$sql .= filter_person();
$sql .= " ORDER BY p.pname, p.vorname";
$personen=db_values($sql, '*');
fxDebug($personen,$sql, 2);

if(fxIsArray($personen))
{
	$pids=array_to_csv($personen,'personen_id');

	// Aktuelle Daten ermitteln
	$ada=array();
	$sql  = "SELECT personen_id, ist_beginn, zeitaufwand, rk_ort1 FROM zeitdaten";
	$sql .= " WHERE zeitart=105 AND personen_id IN (".$pids.") AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$tmp=db_values($sql, '*');
fxDebug($tmp, $sql, 3);
	if(fxIsArray($tmp))
	{
		foreach($tmp as $t)
			$ada[(int)$t['personen_id']]=array('date'=>substr((string)$t['ist_beginn'],0,6), 'shours'=>$t['zeitaufwand'], 'note'=>$t['rk_ort1']);
	}
fxDebug($ada, '$ada: Aktuelle Werte', 2);

	// Historiendaten ermitteln
	$hda=array();
	if($historie)
	{
		$sql  = "SELECT autoid, personen_id, ist_beginn, zeitaufwand, rk_ort1, aenderungs_id, zeitstempel FROM zeitdaten_history";
		$sql .= " WHERE zeitart=105 AND aktion=2 AND personen_id IN (".$pids.") AND mandanten_id=".$GLOBALS['fxpglobals']['client']." ORDER BY autoid";
		$tmp=db_values($sql, '*');
fxDebug($tmp, $sql, 3);
		if(fxIsArray($tmp))
		{
			$hla=array();
			$haa=array();
			foreach($tmp as $t)
			{
				$pid=(int)$t['personen_id'];
				$dtm=substr((string)$t['ist_beginn'],0,6);
				if(!array_key_exists($pid, $hda))
				{
					$hda[$pid]=array();
					$hla[$pid]='';
					$haa[$pid]='';
				}
				$ck=$dtm.'_'.$t['zeitaufwand'].'_'.$t['rk_ort1'];
				$hpid=0;
				if(!strlen((string)$hla[$pid]) || ($hla[$pid] != $ck))
				{
					$hpid=$pid;
					$hla[$pid]=$ck;
				}
				else if(strlen((string)$haa[$pid]) && ($hla[$pid] == $ck))
				{
					$hpid=$pid;
					unset($hda[$pid][$haa[$pid]]);
				}
				if($hpid)
				{
					$hda[$hpid][$t['autoid']]=array('date'=>$dtm, 'shours'=>$t['zeitaufwand'], 'note'=>$t['rk_ort1'], 'creator'=>$t['aenderungs_id'], 'created'=>$t['zeitstempel']);
					$haa[$hpid]=$t['autoid'];
				}
			}
		}
	}
fxDebug($hda, '$hda: Historien-Werte', 2);

	// Überstundenberechnungswerte ermitteln
	$oca=array();
	$sql  = "SELECT personen_id, ist_beginn, zeitaufwand FROM zeitdaten";
	$sql .= " WHERE zeitart=104 AND personen_id IN (".$pids.") AND mandanten_id=".$GLOBALS['fxpglobals']['client']." ORDER BY personen_id, ist_beginn";
	$tmp=db_values($sql,'*');
fxDebug($tmp, $sql, 3);
	if(fxIsArray($tmp))
	{
		foreach($tmp as $t)
		{
			$pid=(int)$t['personen_id'];
			$dtm=substr((string)$t['ist_beginn'],0,6);
			if(!isset($oca[$pid]))
				$oca[$pid]=array();
			if(!isset($oca[$pid][$dtm]))
				$oca[$pid][$dtm]=$t['zeitaufwand'];
		}
	}
fxDebug($oca, '$oca: Überstundenberechnungswerte', 2);

	$tr_zint=tr_rechte(7);
	$tr_zext=tr_rechte(10);

	$mc=$GLOBALS['_maskcounter'];
	$mn=445;
	$ln=0;
	foreach($personen as $pers)
	{
		// Gesamtinfo
		if($GLOBALS['fxpglobals']['settings']['zk_ueb_jahr'] && !$ln)
		{
			$mdat=substr('0000'.$GLOBALS['fxpglobals']['settings']['zk_ueb_jahr'], -4).substr('0000'.$GLOBALS['fxpglobals']['settings']['zk_ueb_monat'], -2).'01000000';
			$kut='&nbsp;&nbsp;'.meldung(3162).' '.get_entry($mdat, 'datum');	// <b>Globale Mandanten-Einstellung:</b> Zeitdaten-Überträge berechnen ab
			fxf_listSectHeader($mc, $ln, $kut, 'mitarbeiter', 'daten', 8, -2, 1);
			$ln++;
		}

		$pid=(int)$pers['personen_id'];

		// Aktuelle Werte ermitteln
		$dat=substr((string)$pers['maeintrittsdatum'],0,6);
		if($GLOBALS['fxpglobals']['settings']['zk_ueb_jahr'])
		{
			$d=substr('0000'.$GLOBALS['fxpglobals']['settings']['zk_ueb_jahr'], -4).substr('0000'.$GLOBALS['fxpglobals']['settings']['zk_ueb_monat'], -2);
			if($d > $dat)
				$dat=$d;
		}
		$shours=0;
		$year=(int)substr((string)$dat,0,4);
		$month=(int)substr((string)$dat,4,2);
		$note='';
		if(fxIsArray($ada) && array_key_exists($pid, $ada))
		{
			$dat=$ada[$pid]['date'];
			$shours=$ada[$pid]['shours'];
			$note=$ada[$pid]['note'];

			$year=(int)substr((string)$dat,0,4);
			$month=1+(int)substr((string)$dat,4,2);
			if($month > 12)
			{
				$month=1;
				$year++;
			}
		}
//echo('<b>'.$pid.'</b>: $month='.$month.', $year='.$year.', $shours='.$shours.', $note='.$note.'<br />');

		// Mitarbeiter
		$cls='black';
		if($pers['personentyp'] == 840)
			$cls='blue';
		if($pers['inaktiv'])
			$cls='grey italic';
		$name='<font class="'.$cls.'">'.$pers['pname'];
		if(strlen((string)$pers['vorname']))
			$name .= ',&nbsp;'.$pers['vorname'];
		$name .= '</font>&nbsp;<i class="lightgrey">[';
		if($pers['personentyp'] == 840)
			$name .= $t_ext;
		else
			$name .= $t_int;
		$name .= ']</i>';

		// Historien-Tooltip
		$him='';
		$hst='';
		$htt='';
		$ctt='';
		$hzm=10;
		$hpa=array();
		if(fxIsArray($hda) && array_key_exists($pid, $hda))
		{
			$him='<img src="'.$GLOBALS['gfxpath'].'tts.png" align="right" style="margin-top:2px;">';
			$hst='cursor:pointer;';

			$tt  = '<table width=100% border=0 align=center bgcolor=#cccccc cellpadding=4 cellspacing=1>';
			$tt .= '<tr style=display:table-row>';
			for($o=1; $o<6; $o++)
				$tt .= '<td bgcolor=#dddddd class=bold>'.$otcva[$o].'</td>';
			$tt .= '</tr>';

			$soh=sizeof($hda[$pid]);
			$sc=1;
			foreach($hda[$pid] as $arr)
			{
				$hy=(int)substr((string)$arr['date'], 0, 4);
				$hm=1+(int)substr((string)$arr['date'], 4, 2);
				if($hm > 12)
				{
					$hm=1;
					$hy++;
				}
				$bc1='f8f8f8';
				$bc2='ffffff';
				if($sc == $soh)
				{
					$bc1='e7f8e7';
					$bc2='eeffee';
				}

				$trd='none';
				if(($soh < $hzm) || ($sc > $soh-$hzm))
					$trd='table-row';
				else if($sc == 1)
				{
					$tt .= '<tr note style=display:table-row><td colspan=5 align=center bgcolor=#ffffff class=red>'.str_replace('$var1', $soh-$hzm, $t_alte).'</td></tr>';
					$hst='cursor:pointer;';
				}

				$tt .= '<tr style=display:'.$trd.'>';
				// ...Gesetzt am/um
				$tt .= '<td bgcolor=#'.$bc1.' class=grey nowrap>'.get_entry($arr['created'], 'datetime').'</td>';
				// ...Von wem
				if(!isset($hpa[$arr['creator']]))
					$hpa[$arr['creator']]=get_persondata($arr['creator'], 0, false, true);
//fxDebug($hpa[$arr['creator']], '$hpa['.$arr['creator'].']');
				$tt .= '<td bgcolor=#'.$bc1.' class=grey nowrap>'.$hpa[$arr['creator']]['name2'].'</td>';
				// ...Ab Monat
				$tt .= '<td bgcolor=#'.$bc2.' nowrap>'.$hy.',&nbsp;'.meldung(17+$hm, false).'</td>';
				// ...Wert
				$tt .= '<td bgcolor=#'.$bc2.' align=right nowrap>'.get_entry($arr['shours'], 'zeitspanne').' h</td>';
				// ...Anmerkung
				$hnote=$arr['note'];
				if(($hnote === NULL) || !strlen((string)$hnote))
					$hnote='<i class=lightergrey>('.$t_ohne.')</i>';	// Ohne
				$tt .= '<td bgcolor=#'.$bc2.' class=grey>'.$hnote.'</td>';
				$tt .= '</tr>';

				$sc++;
			}
			$tt .= '</table><br />';

			$htt=' tooltip="'.$otcva[0].': <b>'.strtr(strip_tags($name), $GLOBALS['caption_strtr']).'</b><br /><br />'.$tt.'"';
			$ctt=' '.fxf_jsFunction('tooltipPopup', 'event');

		}
		$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['mitarbeiter']='~<div'.$ctt.' style="padding:0 2px;'.$hst.'"'.$htt.'>'.$him.person_pic($pers['personen_id'], 20, $pers['geschlecht'], $GLOBALS['fxpglobals']['client'], false).'&nbsp;&nbsp;'.$name.'&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</div>';

		// Ein-/Austrittsdatum
		$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['maeintrittsdatum']='~&nbsp;'.get_entry($pers['maeintrittsdatum'],'datum');
		if($pers['maaustrittsdatum'])
			$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['maeintrittsdatum'] .= '-'.get_entry($pers['maaustrittsdatum'],'datum');
		$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['maeintrittsdatum'] .= '&nbsp;';

		// Std./Woche
		$val=$pers['wochenarbeitstd'];
		$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['nv_text2']=get_entry($val, 'dezimal');
		if($err && fxIsArray($_POST) && array_key_exists('WochenArbeitStd', $_POST))
			$val=$_POST['WochenArbeitStd'][$mc][$ln];
		$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['wochenarbeitstd']=$val;
		if((($pers['personentyp'] == 275) && !$tr_zint) || (($pers['personentyp'] == 840) && !$tr_zext))
			${'WochenArbeitStd'.$ln.'_dis'}=true;

		// Monat
		$Monat_ds=0;
		$val=17+$month;	// Wertetab 18=Januar, 19=Februar, ...
		$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['nv_text2'] .= '|'.$val;
		if($err && fxIsArray($_POST) && array_key_exists('Monat', $_POST))
			$val=$_POST['Monat'][$mc][$ln];
		$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['monat']=$val;

		// Jahr
		$val=$year;
		$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['nv_text2'] .= '|'.$val;
		if($err && fxIsArray($_POST) && array_key_exists('Jahr', $_POST))
			$val=$_POST['Jahr'][$mc][$ln];
		$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['jahr']=$val;

		// Rest-Überstunden
		$cval=$shours;
		$val=$shours;
		$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['nv_text2'] .= '|'.$val;
		if($err && fxIsArray($_POST) && array_key_exists('Ueberstunden_rest', $_POST))
		{
			$cval=summe_zeitkonto($pid, $GLOBALS['fxptdata']['lvalues'][$mc][$ln]['jahr'].substr('00'.($GLOBALS['fxptdata']['lvalues'][$mc][$ln]['monat']-17), -2));
			$val=$_POST['Ueberstunden_rest'][$mc][$ln];
		}
		$ccol=$pcol2;
		if($cval < 0)
			$ccol=$ncol2;
		$vcol=$pcol1;
		if($val < 0)
			$vcol=$ncol1;
		$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['nv_text1']='~'.fieldTimespan('hcalc['.$mc.']['.$ln.']', $cval, $Ueberstunden_breite, false, false, true, '', '', '', true).'<span style="color:'.$ccol.';">h</span><img id="icopy['.$mc.']['.$ln.']" src="'.$GLOBALS['gfxpath'].'npan.png" style="margin:0 4px;cursor:pointer;" tooltip="'.$otcva[7].'">';
		$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['ueberstunden_rest']='~'.fieldTimespan('Ueberstunden_rest['.$mc.']['.$ln.']', $val, $Ueberstunden_breite, false, false, false, '', '', '', true).'<span style="color:'.$vcol.';">h</span>';

		// Anmerkung
		$val=$note;
		$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['nv_text2'] .= '|'.$note;
		if($err && fxIsArray($_POST) && array_key_exists('Anmerkung', $_POST))
			$val=$_POST['Anmerkung'][$mc][$ln];
		$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['anmerkung']=$val;

		// Daten
		// ...Monatsüberträge
		$fdd='';
		$scm=substr('0000'.$year,-4).substr('00'.$month,-2);
		if(fxIsArray($oca[$pid]) && isset($oca[$pid][$scm]))
		{
//fxDebug($oca[$pid], 'Überstundenberechnungswerte von ['.$pid.'] ab '.$scm.' bis '.substr((string)$GLOBALS['datetime'],0,6));
			$poa=summe_zeitkonto($pid,substr((string)$GLOBALS['datetime'],0,6),true);
//fxDebug($poa,'$poa');

			$dc=0;
			if(fxIsArray($poa))
			{
				foreach($poa as $cd => $cv)
				{
					if(isset($oca[$pid][$cd]) && ($oca[$pid][$cd] != $cv))
					{
						if(!strlen((string)$fdd))
							$fdd=$cd;
						$dc++;
					}
				}
			}
//echo('$pid='.$pid.', $dc='.$dc.', $fdd='.$fdd.'<br />');

			if(strlen((string)$fdd))
			{
				// Die Monatsüberträge ab $var1 müssen neu berechnet werden!
				$dc='<img src="'.$GLOBALS['gfxpath'].'tl_red.png" tooltip="<font class=red>'.str_replace('$var1', '&quot;'.meldung(17+(int)substr((string)$fdd,4,2),true).' '.substr((string)$fdd,0,4).'&quot;', $ochka[2]).'</font>" style="cursor:help;">';
			}
			else
			{
				// Alle Monatsüberträge sind korrekt.
				$dc='<img src="'.$GLOBALS['gfxpath'].'tl_green.png" tooltip="<font class=green>'.$ochka[1].'</font>" style="cursor:help;">';
			}
		}
		else
		{
			// Es wurden noch keine Monatsüberträge gespeichert.
			$dc='<img src="'.$GLOBALS['gfxpath'].'tl_grey.png" tooltip="<font class=lightgrey>'.$ochka[0].'</font>" style="cursor:help;">';
		}
		// ...Abwesenheitstermine
		$val=0.0;
		if($pers['arbeitstage'] > 0)
			$val=round($pers['wochenarbeitstd']*100.0/$pers['arbeitstage'])/100.0;
		$aah=get_entry($val,'decimal');
		$aad='&quot;'.meldung(17+(int)substr((string)$scm,4,2),true).' '.substr((string)$scm,0,4).'&quot;';
		$sql="SELECT COUNT(termin_id) AS aok FROM termine WHERE personen_id=".$pid." AND mandanten_id=".$GLOBALS['fxpglobals']['client']." AND endzeit>='".$scm."01000000' AND soll_st_tag=".$val." AND terminart IN (".$_tca[FXP_AC_ABSENT].")";
		$aok=(int)db_value($sql);
//fxDebug($aok, '$scm='.$scm.'<br />'.$sql);
		$sql="SELECT COUNT(termin_id) AS ako FROM termine WHERE personen_id=".$pid." AND mandanten_id=".$GLOBALS['fxpglobals']['client']." AND endzeit>='".$scm."01000000' AND (soll_st_tag IS NULL OR (soll_st_tag IS NOT NULL AND soll_st_tag<>".$val.")) AND terminart IN (".$_tca[FXP_AC_ABSENT].")";
		$ako=(int)db_value($sql);
//fxDebug($ako, '$scm='.$scm.'<br />'.$sql);
		if(!$aok && !$ako)
		{
			// Es wurden noch keine Abwesenheitstermine ab $var1 angelegt.
			$dc .= '<img src="'.$GLOBALS['gfxpath'].'tl_grey.png" tooltip="<font class=lightgrey>'.strtr($achka[0], array('$var1'=>$aad)).'</font>" style="cursor:help;margin-left:5px;">';
		}
		else if($aok)
		{
			// 	Alle Abwesenheitstermine ab $var1 haben die korrekten Tagesarbeitsstunden von $var2 Std./Tag.
			$dc .= '<img src="'.$GLOBALS['gfxpath'].'tl_green.png" tooltip="<font class=green>'.strtr($achka[1], array('$var1'=>$aad, '$var2'=>$aah)).'</font>" style="cursor:help;margin-left:5px;">';
		}
		else
		{
			// 	Die Abwesenheitstermine ab $var1 müssen angepasst werden bzgl. der Tagesarbeitsstunden!
			$dc .= '<img src="'.$GLOBALS['gfxpath'].'tl_red.png" tooltip="<font class=red>'.strtr($achka[2], array('$var1'=>$aad, '$var2'=>$aah)).'</font>" style="cursor:help;margin-left:5px;">';
		}
		$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['daten']='~'.$dc;

		// Versteckt
		$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['personen_id']=$pers['personen_id'];
		$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['nv_text2'] .= '^'.(int)$pers['arbeitstage'].'^'.$fdd.'^'.$ako;

		$ln++;
	}

	if($ln)
	{
		fxf_maskColumnBorder(array('Mitarbeiter','MaEintrittsDatum','WochenArbeitStd','Monat','Jahr','nv_text1','Anmerkung','daten'), 1, -1, $mc, $ln, true);
		$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['ueberstunden_rest']='~';
		$ln++;
	}
}

if(!$GLOBALS['fxptdata']['lvalues'][$mc])
	$GLOBALS['fxptdata']['lvalues'][$mc]=true;
?>