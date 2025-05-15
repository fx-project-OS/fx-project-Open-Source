<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : tools_pers.inc                                               //
// Version     : 24.1                                                         //
// Begin       : 2020-08-20                                                   //
// Last Change : 2024-11-05                                                   //
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
 * Collection of person functions
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

$GLOBALS['__loaded_'.basename(__FILE__)]=true;


////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/**
 * Liefert die Auslastung einer Person für eine Aufgabe zurück
 *
 * @param various $task        - Mandatory parameter:                 ???PARAMETER???
 * @param various $pers        - Mandatory parameter:                 ???PARAMETER???
 * @param various $eff         - Mandatory parameter:                 ???PARAMETER???
 * @param string  $sdate       - Optional parameter (default = ''):   ???PARAMETER???
 * @param string  $edate       - Optional parameter (default = ''):   ???PARAMETER???
 * @param string  $workdays    - Optional parameter (default = ''):   ???PARAMETER???
 * @param double  $hoursperday - Optional parameter (default = 0.0):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auslastung($task, $pers, $eff, $sdate='', $edate='', $workdays='', $hoursperday=0.0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$mid	= $GLOBALS['fxpglobals']['client'];
	$task	= (int)$task;
	$pers	= (int)$pers;
	$eff	= (double)$eff;

	// Fehler, falls keine Projekt-ID oder keine Personen-ID übergeben wurde
	if(($task < 1) || ($pers < 1))
		return 0.0;

	// Person hat keinen Aufwand, deshalb auch keine Auslastung
	if($eff <= 0.0)
		return 0.0;

	// Zeiten der Aufgabe
	$sdate=substr(trim((string)$sdate),0,8);
	$edate=substr(trim((string)$edate),0,8);
	if(!strlen((string)$sdate) || !strlen((string)$edate))
	{
		$sql="SELECT soll_beginn, soll_ende FROM zeitdaten WHERE projekt_id=".$task." AND mandanten_id=".$mid." AND zeitart=102";
		$tdat=db_values($sql);
		if(!is_array($tdat))
			return 0.0;

		$sdate=substr((string)$tdat['soll_beginn'],0,8);
		$edate=substr((string)$tdat['soll_ende'],0,8);
	}

	// Grunddaten der Person
	$workdays=trim((string)$workdays);
	$hoursperday=(double)$hoursperday;
	if(!strlen((string)$workdays) || (strlen((string)$workdays) != 7))
	{
		$pers_array=getExtPersInfo($pers, $sdate, $edate);
		$day_array	= $pers_array['days'];
//fxDebug($day_array, 'auslastung('.$task.', '.$pers.', '.$eff.')');

		// Auslastung ermitteln
		if(!is_array($day_array) || !sizeof($day_array))
			return 1.0;

		$eff_free=0.0;
		foreach($day_array as $day => $eff_day)
		{
			if(($day >= $sdate) && ($day <= $edate))
				$eff_free += $eff_day['h'];
		}
		$eff_free *= 3600.0;	// Stunden in Sekunden umrechnen
	}
	else
	{
		$wd=countWeekdays($sdate, $edate, (int)substr((string)$workdays, 0, 1), (int)substr((string)$workdays, 1, 1), (int)substr((string)$workdays, 2, 1), (int)substr((string)$workdays, 3, 1), (int)substr((string)$workdays, 4, 1), (int)substr((string)$workdays, 5, 1), (int)substr((string)$workdays, 6, 1));
		$eff_free=($wd * $hoursperday)*3600.0;
//echo($pers.': $task='.$task.'; $eff='.$eff.'; $wd('.$sdate.' - '.$edate.' ['.$workdays.'])='.$wd.'; $hoursperday='.$hoursperday.'; $eff_free * 3600.0='.($wd*$hoursperday).' * 3600.0='.$eff_free.'<br />');
	}

	if($eff_free <= 0.0)
		return 1.0;

	$eff_task=round($eff/$eff_free, 4);
//echo('-> Berechnete Auslastung: '.$eff_task.'<br />');

	return $eff_task;
}

/**
 * ???FUNCTION???
 *
 * @param string  $von - Optional parameter (default = ''):    ???PARAMETER???
 * @param string  $bis - Optional parameter (default = ''):    ???PARAMETER???
 * @param integer $pid - Optional parameter (default = 0):     ???PARAMETER???
 * @param boolean $md  - Optional parameter (default = false): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function personen_auslastung($von='', $bis='', $pid=0, $md=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Rückgabe-Array
	$ausl_array['ausl']		= 0.0;
	$ausl_array['summe']	= 0.0;
	$ausl_array['arbeit']	= 0.0;
	$ausl_array['frei']		= 0.0;

	$mid=$GLOBALS['fxpglobals']['client'];
	if(!$pid)
		$pid=$GLOBALS['fxpglobals']['person'];

	if(!$von)
	{
		// Startdatum=Starttag der aktuellen Woche
		$von=fxNow();

		$tag 	= (int)substr((string)$von,6,2);
		$monat	= (int)substr((string)$von,4,2);
		$jahr 	= (int)substr((string)$von,0,4);
		$wtg=fx_date('w', fx_mktime(0, 0, 0, $monat, $tag, $jahr));
		$stg=(int)($GLOBALS['fxpglobals']['settings']['ersterwt']);
		$von=fx_date('Ymd000000', fx_mktime(0, 0, 0, $monat, $tag-$wtg+$stg, $jahr));
	}

	if(!$bis)
	{
		// Enddatum=Endtag nach der Standard-Zeitspanne (4 Wochen)
		$jahr	= (int)(substr((string)$von,0,4));
		$monat	= (int)(substr((string)$von,4,2));
		$tag	= (int)(substr((string)$von,6,2));
		$bis=fx_date('Ymd000000', fx_mktime(0, 0, 0, $monat, $tag+$GLOBALS['fxpglobals']['settings']['stdzeitspanne']-1, $jahr));
	}
	$von=substr((string)$von,0,8).'000000';
	$bis=substr((string)$bis,0,8).'240000';

	// Personeninfos holen
	$sql  = "SELECT p.pname, p.vorname, p.geschlecht, p.personentyp,";
	$sql .= " m.maeintrittsdatum, m.maaustrittsdatum, m.wochenarbeitstd, m.arbeitstage, m.montag AS b, m.dienstag AS c, m.mittwoch AS d, m.donnerstag AS e, m.freitag AS f, m.samstag AS g, m.sonntag AS a";
	$sql .= " FROM personen p, madaten m";
	$sql .= " WHERE m.personen_id=p.personen_id AND p.personen_id=".$pid." AND p.mandanten_id=".$mid." AND m.mandanten_id=".$mid;
	$pinf=db_values($sql);
//fxDebug($pinf, $pid);
	$ausl_array['name']=trim((string)$pinf['pname']);
	if(strlen((string)trim((string)$pinf['vorname'])))
		$ausl_array['name'] .= ', '.trim((string)$pinf['vorname']);
	$ausl_array['geschlecht']	= $pinf['geschlecht'];
	$ausl_array['typ']			= $pinf['personentyp'];
	$ausl_array['wochenstd']	= $pinf['wochenarbeitstd'];
	$ausl_array['arbeitstage']	= $pinf['arbeitstage'];
	if(($pinf['maeintrittsdatum'] !== NULL) && strlen((string)$pinf['maeintrittsdatum']))
		$ausl_array['eintritt']=substr((string)$pinf['maeintrittsdatum'], 0,8).'000000';
	else
		$ausl_array['eintritt']='00000000000000';
	if(($pinf['maaustrittsdatum'] !== NULL) && strlen((string)$pinf['maaustrittsdatum']))
		$ausl_array['austritt']=substr((string)$pinf['maaustrittsdatum'], 0,8).'240000';
	else
		$ausl_array['austritt']='99999999999999';

	// Projekt-Status
	$pstat='';
	if($GLOBALS['fxpglobals']['setfilter'] && !$GLOBALS['fxpglobals']['filter']['fprojekt_inaktiv'] && is_array($GLOBALS['fxpglobals']['filter']))
	{
		if($GLOBALS['fxpglobals']['filter']['projektstatus_297'])
			$pstat .= ',297';
		if($GLOBALS['fxpglobals']['filter']['projektstatus_300'])
			$pstat .= ',300';
	}
	if(strlen((string)$pstat))
		$pstat=substr((string)$pstat, 1);
	else
		$pstat='297,300';
	// Auslastungsdaten holen
	$_ad='';
	$sql  = "SELECT m.projekt_id, m.soll_beginn, m.soll_ende, m.auslastung, m.aufwand_std FROM maplanung m, projekte p";
	$sql .= " WHERE p.projekt_id=m.projekt_id AND m.personen_id=".$pid." AND m.auslastung>0 AND (m.soll_beginn<='".$bis."' AND m.soll_ende>='".$von."') AND p.projekt_status IN (".$pstat.")";
	$sql .= " AND m.mandanten_id=".$mid." AND p.mandanten_id=".$mid;
	$pausl=db_values($sql, '*');
	if(!is_array($pausl))
		return $ausl_array;
//fxDebug($pausl, 'Projektauslastung ('.$pid.')');

	// Feiertage in dieser Zeitspanne ermitteln
	$ftv='_ft'.$von.$bis;
	if(!isset($GLOBALS[$ftv]))
	{
		$GLOBALS[$ftv]=get_feiertage($von, $bis);
//fxDebug($GLOBALS[$ftv], 'Feiertage: '.$ftv);
	}

	// Sondertage ermitteln
	$_st=get_sondertage($von, $bis, $pid);
//fxDebug($_st, 'Sondertage ('.$pid.')');

	// Gleitzeit ermitteln
	$_gz='';
	$sql="SELECT ist_beginn FROM zeitdaten WHERE projekt_id=-1 AND personen_id=".$pid." AND (ist_beginn<='".$bis."' AND ist_ende>='".$von."') AND mandanten_id=".$mid." ORDER BY ist_beginn";
	$sar=db_values($sql, '*');
	if(is_array($sar))
	{
		foreach($sar as $se)
			$_gz[substr((string)$se['ist_beginn'],0,8)]=true;
	}
//fxDebug($_gz, 'Gleitzeit ('.$pid.')');


	// Monatstage
	$mo_tage=array(1 => 31, 28, 31, 30,  31,  30,  31,  31,  30,  31,  30,  31);

	// Datum aufsplitten
	$stag 	= (int) substr((string)$von,6,2);
	$smonat=(int) substr((string)$von,4,2);
	$sjahr 	= (int) substr((string)$von,0,4);
	// Startwochentag
	$stg=fx_date('w', fx_mktime(0, 0, 0, $smonat, $stag, $sjahr));
	// Auf Schaltjahr prüfen
	if(strlen((string)$sjahr) == 4)
		$lj=fx_is_leap_year($sjahr);
	else
		$lj=false;
	if($lj)
		$mo_tage[2]=29;
	// Starttag + Endtag
	$atag=substr((string)$von,0,8);
	$ltag=substr((string)$bis,0,8);

	while($atag <= $ltag)
	{
		$wday=0.0;
		// Arbeitstag
		if($pinf[chr($stg+97)])
		{
			$wday=1.0;
			// Feiertag ?
			$fadd=0;
			if(($wday > 0.0) && sizeof($GLOBALS[$ftv]) && ($GLOBALS[$ftv][$atag]['art'] == FXP_HL_FULL))	// Ganzer Feiertag
			{
				$wday=0.0;
				$fadd=1;
			}
			else if(($wday > 0.0) && sizeof($GLOBALS[$ftv]) && ($GLOBALS[$ftv][$atag]['art'] == FXP_HL_HALF))	// Halber Feiertag
			{
				$wday -= 0.5;
				$fadd=1;
			}
			// Gleittag ?
			if(($wday > 0.0) && is_array($_gz) && $_gz[$atag])	// Gleittag
				$wday=0.0;
			// Sondertag ?
			if(($wday > 0.0) && sizeof($_st) && is_array($_st[$atag]))
				$wday=max(0.0, $wday-$_st[$atag][-1]['dauer']);
		}
		$ausl_array['arbeit'] += $wday;
		$ausl_array['frei'] += (1.0-$wday);

		// Auslastung für diesen Tag holen
		$ta=0.0;
		if($wday > 0.0)
		{
			$vt=$atag.'120000';
			foreach($pausl as $pa)
			{
				if(($vt < $pa['soll_ende']) && ($vt > $pa['soll_beginn']))
					$ta += $pa['auslastung'];
			}
			$ausl_array['summe'] += $ta;
		}
		if($md == 't')
			$ausl_array['t'][$atag]['ausl']=$ta;

		$stg++;
		if($stg > 6)
			$stg=0;
		$stag++;
		if($stag > $mo_tage[$smonat])
		{
			$stag=1;
			$smonat++;
			if($smonat > 12)
			{
				$smonat=1;
				$sjahr++;
				if(strlen((string)$sjahr) == 4)
					$lj=fx_is_leap_year($sjahr);
				else
					$lj=false;
				if($lj)
					$mo_tage[2]=29;
				else
					$mo_tage[2]=28;
			}
		}
		$atag=substr('0000'.$sjahr, -4).substr('00'.$smonat, -2).substr('00'.$stag, -2);
	}
	$ausl_array['summe']=$ausl_array['summe']*100.0;
	$ausl_array['ausl']=$ausl_array['summe']/$ausl_array['arbeit'];

	return $ausl_array;
}

/**
 * Berechnet die Auslastung für alle nicht abgeschlossenen Vorgänge, zu denen eine übergebene
 * Person zugeteilt ist, neu. Es kann ein kommaseparierter String und ein Datum übergeben
 * werden, dannn werden nur die übergebenen Vorgänge, die diesen Tag beinhalten, berücksichtigt
 *
 * @param various $pid    - Mandatory parameter:                ???PARAMETER???
 * @param string  $sdatum - Optional parameter (default = ''):  ???PARAMETER???
 * @param string  $edatum - Optional parameter (default = ''):  ???PARAMETER???
 * @param string  $prj    - Optional parameter (default = ''):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function update_auslastung($pid, $sdatum='', $edatum='', $prj='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$pid=(int)$pid;
	$mid=$GLOBALS['fxpglobals']['client'];

	// Nur Projekte die einen bestimmten (Fehl-)Tag enthalten
	$datum="";
	$sdatum=trim((string)$sdatum);
	if(strlen((string)$sdatum))
		$datum .= " AND z.soll_ende>='".substr((string)$sdatum,0,8)."000000'";
	$edatum=trim((string)$edatum);
	if(strlen((string)$edatum))
		$datum .= " AND z.soll_beginn<='".substr((string)$edatum, 0,8)."240000'";

	// Nur bestimmte Projekte
	$prj=trim((string)$prj);
	if(strlen((string)$prj))
		$prj=" AND m.projekt_id IN (".$prj.")";

	$uerr=false;
	// Alle nicht abgeschlossenen Vorgänge (Aufgaben!) die auch die evtl. oben genannte Kriterien erfüllen und denen der Mitarbeiter zugeordnet ist
	$sql  = "SELECT m.maplanung_id, m.personen_id, m.projekt_id, m.aufwand_std";
	$sql .= " FROM maplanung m, projekte p, zeitdaten z";
	$sql .= " WHERE ";
	if($pid > 0)
		$sql .= "m.personen_id=".$pid." AND ";
	$sql .= "p.projekt_status<>".FXP_PS_COMPLETED.$datum.$prj." AND m.projekt_id=p.projekt_id AND p.zeit_id=z.zeit_id AND m.mandanten_id=".$mid." AND p.mandanten_id=".$mid." AND z.mandanten_id=".$mid;
	$pa=db_values($sql, '*');
	if(fxIsArray($pa))
	{
		foreach($pa as $p)
		{
			$ausl=auslastung((int)$p['projekt_id'], (int)$p['personen_id'], $p['aufwand_std']);
			$sql="UPDATE maplanung SET auslastung=".$ausl." WHERE maplanung_id=".(int)$p['maplanung_id']." AND mandanten_id=".$mid;
			$uerr=db_x($sql);
			if($uerr)
			{
				$uerr=846;	// Fehler bei der Projektauslastungsberechnung!
				break;
			}
		}
	}

	return $uerr;
}

/**
 * ???FUNCTION???
 *
 * @param string  $typ       - Optional parameter (default = 'per'): ???PARAMETER???
 * @param integer $mid       - Optional parameter (default = 0):     ???PARAMETER???
 * @param integer $prid      - Optional parameter (default = 0):     ???PARAMETER???
 * @param string  $psid      - Optional parameter (default = ''):    ???PARAMETER???
 * @param string  $sdatum    - Optional parameter (default = ''):    ???PARAMETER???
 * @param boolean $force     - Optional parameter (default = false): ???PARAMETER???
 * @param boolean $dbaccess  - Optional parameter (default = true):  ???PARAMETER???
 * @param boolean $write_log - Optional parameter (default = false): ???PARAMETER???
 * @param boolean $sendmail  - Optional parameter (default = true):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function update_maplanung($typ='per', $mid=0, $prid=0, $psid='', $sdatum='', $force=false, $dbaccess=true, $write_log=false, $sendmail=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$mid=(int)$mid;
	if(!$mid)
		$mid=$GLOBALS['fxpglobals']['client'];

	$typ=substr((string)strtolower((string)$typ),0,3);
	switch($typ)
	{
		case 'sac':	// Material
			$macol='sachmittel_id';
			$maart=1450;
			$sendmail=false;	// Don't send emails
		break;

		default:	// Employee
			$macol='personen_id';
			$maart=1094;
		break;
	}

	// Read all fields from "maplanung"
	$sqlz=" FROM maplanung WHERE mandanten_id=".$mid." AND ";
	if(strlen((string)$sdatum))
		$sqlz .= "soll_ende>='".substr((string)$sdatum,0,8)."000000' AND (";
	if(strlen((string)$psid))
	{
		$sqlz .= $macol." IN (".$psid.")";
		$sqlz .= " OR ((".$macol." IS NOT NULL AND ".$macol.">0) AND projekt_id IN (SELECT projekt_id FROM maplanung WHERE ".$macol." IN (".$psid.") AND mandanten_id=".$mid."))";
	}
	else
	{
		$sqlz .= "(".$macol." IS NOT NULL AND ".$macol.">0)";
		if($prid > 0)
			$sqlz .= " AND projekt_id IN (".inferior_vn($prid).")";
	}
	if(strlen((string)$sdatum))
		$sqlz .= ")";
	$sql="SELECT *".$sqlz." ORDER BY projekt_id";
	$mpd=db_values($sql,'*');

	// No data, leave function
	if(!fxIsArray($mpd))
		return false;

//fxDebug($mpd, $sql);

	// Extract project id's from array and add planned effort
	$pids="SELECT projekt_id".$sqlz;
	$aufw=array();
	foreach($mpd as $m)
	{
		$pid=$m['projekt_id'];
		$pz=round(get_double($m['prozent']),4);
		if($pz <= 0.0)
			$pz=0.0;
		$ms=substr((string)$m['soll_beginn'],0,8).'000000';
		$me=substr((string)$m['soll_ende'],0,8).'240000';
		if(!fxIsArray($aufw[$pid]))
			$aufw[$pid]=array('r'=>(double)$m['aufwand_std'], 'p'=>0.0, 'z'=>$pz, 'm'=>1, 'a'=>0, 'ms'=>$ms, 'me'=>$me, 's'=>'', 'e'=>'', 'as'=>'', 'os'=>1, 'ap'=>'', 'op'=>1, 'n'=>'');
		else
		{
			if($ms < $aufw[$pid]['ms'])
				$aufw[$pid]['ms']=$ms;
			if($me > $aufw[$pid]['me'])
				$aufw[$pid]['me']=$me;
			$aufw[$pid]['r'] += (double)$m['aufwand_std'];
			$aufw[$pid]['z'] += $pz;
			$aufw[$pid]['m']++;
		}
	}

	// Get actual planned effort for each project
	$sql  = "SELECT p.projekt_id, p.vorgangsnummer, p.name_projekt, p.projektart, p.aufwand_soll, p.projekt_status, p.prioritaet, z.soll_beginn, z.soll_ende";
	$sql .= " FROM projekte p, zeitdaten z WHERE p.projekt_id IN (".$pids.") AND z.projekt_id=p.projekt_id AND z.zeitart=102 AND p.mandanten_id=".$mid." AND z.mandanten_id=".$mid;
	$paw=db_values($sql,'*');

	// No data, leave function
	if(!fxIsArray($paw))
		return false;

	foreach($paw as $p)
	{
		$apid=(int)$p['projekt_id'];

		$aufw[$apid]['p']	= (double)$p['aufwand_soll'];
		$aufw[$apid]['a']	= (int)$p['projektart'];
		$aufw[$apid]['s']	= substr((string)$p['soll_beginn'],0,8).'000000';
		$aufw[$apid]['e']	= substr((string)$p['soll_ende'],0,8).'240000';
		$aufw[$apid]['as']	= (int)$p['projekt_status'];
		$aufw[$apid]['ap']	= (int)$p['prioritaet'];
		$aufw[$apid]['n']	= no_null($p['vorgangsnummer']).': '.$p['name_projekt'];
	}

	// Get last project status
	$sql  = "SELECT projekt_id, projekt_status, prioritaet, zeitstempel";
	$sql .= " FROM projekte_history WHERE projekt_id IN (".$pids.") AND mandanten_id=".$mid." ORDER BY zeitstempel DESC";
	$paw=db_values($sql,'*');
	if(fxIsArray($paw))
	{
		foreach($paw as $p)
		{
			$hpid=(int)$p['projekt_id'];
			if(!fxIsArray($aufw[$hpid]))
				continue;

			if($aufw[$hpid]['os'] == 1)
				$aufw[$hpid]['os']=2;
			else if($aufw[$hpid]['os'] == 2)
				$aufw[$hpid]['os']=(int)$p['projekt_status'];

			if($aufw[$hpid]['op'] == 1)
				$aufw[$hpid]['op']=2;
			else if($aufw[$hpid]['op'] == 2)
				$aufw[$hpid]['op']=(int)$p['prioritaet'];
		}
	}
//fxDebug($aufw, $pids);

	// Datas need adjusting?
	$sqc=0;
	$sql_queue=array();
	foreach($aufw as $pid => $p)
	{
		$az_update=false;
		$az_delete=false;

		// Task date doesn't match resource planning date
		$mforce=false;
		if(($p['ms'] != $p['s']) || ($p['me'] != $p['e']))
			$mforce=true;

		// Yes, if assigned task has been deleted
		if(!$p['a'])
		{
//echo(__LINE__.': Yes, if assigned task has been deleted<br />'.$GLOBALS['nl']);
			$sql_queue[$sqc]['log']='Delete resource planning because assigned task has been deleted';
			$sql_queue[$sqc]['ts']=time();
			$sql_queue[$sqc]['sql'][]="DELETE FROM maplanung WHERE projekt_id=".$pid." AND mandanten_id=".$mid;
			foreach($mpd as $m)
			{
				if($pid != $m['projekt_id'])
					continue;

				$sql_queue[$sqc]['rst'][]=restore_sql($m, 'd', 'maplanung', 'maplanung_id');
			}
			$az_delete=true;
		}
		// Yes, if outdated assignment to no longer supported (sub-)projects
		else if($p['a'] != FXP_PRJ_TASK)
		{
//echo(__LINE__.': Yes, if outdated assignment to no longer supported (sub-)projects<br />'.$GLOBALS['nl']);
			$sql_queue[$sqc]['log']='Delete resource planning because of outdated assignment to no longer supported (sub-)projects';
			$sql_queue[$sqc]['ts']=time();
			$sql_queue[$sqc]['sql'][]="DELETE FROM maplanung WHERE projekt_id=".$pid." AND mandanten_id=".$mid;
			foreach($mpd as $m)
			{
				if($pid != $m['projekt_id'])
					continue;

				$sql_queue[$sqc]['rst'][]=restore_sql($m, 'd', 'maplanung', 'maplanung_id');
			}
			$az_delete=true;
		}
		// Yes, if no planned effort and percentage sum not equal 100%
		else if(!$p['p'] && (($p['z'] != 1.0) || $force || $mforce))
		{
//echo(__LINE__.': Yes, if no planned effort and percentage sum not equal 100%<br />'.$GLOBALS['nl']);
			if($p['z'] != 1.0)
				$sql_queue[$sqc]['log']='Adjust percentage for each resource because there is no planned effort and the percentage sum ('.($p['z']*100).'%) is not equal 100%';
			else if($force)
				$sql_queue[$sqc]['log']='Adjust perecentage for each resource because there is no planned effort and Force-Mode is active';
			else
				$sql_queue[$sqc]['log']='Adjust percentage for each resource because there is no planned effort and the task date does not match anymore the resource planning date';
			$sql_queue[$sqc]['ts']=time();

			$spz=0;
			$pct=0;
			foreach($mpd as $m)
			{
				if($pid != $m['projekt_id'])
					continue;

				$epz=(int)($m['prozent'] * 10000.0);
				if(!$epz)
					$epz=(int)(10000/$p['m']);
				$pct++;
				if($pct == $p['m'])	// Letzte zugeordnete Ressource
					$ppz=10000-$spz;
				else
					$ppz=$epz;
				$spz += $ppz;

				$rpz=$ppz/10000;
				$sql_queue[$sqc]['sql'][]="UPDATE maplanung SET prozent=".$rpz.", aufwand_std=0, auslastung=0, soll_beginn='".$p['s']."', soll_ende='".$p['e']."' WHERE maplanung_id=".$m['maplanung_id']." AND mandanten_id=".$mid;
				$sql_queue[$sqc]['rst'][]=restore_sql($m, 'u', 'maplanung', 'maplanung_id');

				if(($m['soll_beginn'] != $p['s']) || ($m['soll_ende'] != $p['e']))
					$az_update=true;
			}
		}
		// Yes, if the planned effort is not equal to the sum of the resource efforts
		else if(($p['r'] != $p['p']) || $force || $mforce)
		{
//echo(__LINE__.': Yes, if the planned effort is not equal to the sum of the resource efforts<br />'.$GLOBALS['nl']);
			if($p['r'] != $p['p'])
				$sql_queue[$sqc]['log']='Adjust percentage for each resource because the planned effort ('.$p['p'].' s) is not equal to the sum of the resource efforts ('.$p['r'].' s)';
			else if($force)
				$sql_queue[$sqc]['log']='Adjust percentage for each resource because Force-Mode is active';
			else
				$sql_queue[$sqc]['log']='Adjust percentage for each resource because the task date does not match anymore the resource planning date';
			$sql_queue[$sqc]['ts']=time();

			$spz=0;
			$pct=0;
			foreach($mpd as $m)
			{
				if($pid != $m['projekt_id'])
					continue;
				$pct++;
				if($pct == $p['m'])	// Letzte zugeordnete Ressource
					$ppz=10000-$spz;
				else if($m['prozent'])
					$ppz=(int)($m['prozent'] * 10000.0);
				else if($p['r'])
					$ppz=(int)(((double)$m['aufwand_std'] / $p['r']) * 10000.0);
				else
					$ppz=0;
				$spz += $ppz;

				$rpz=((double)$ppz)/10000.0;
				$aw_std=$rpz*$p['p'];
				$ausl=auslastung($m['projekt_id'], $m['personen_id'], $aw_std);

				$sql_queue[$sqc]['sql'][]="UPDATE maplanung SET prozent=".$rpz.", aufwand_std=".$aw_std.", auslastung=".$ausl.", soll_beginn='".$p['s']."', soll_ende='".$p['e']."' WHERE maplanung_id=".$m['maplanung_id']." AND mandanten_id=".$mid;
				$sql_queue[$sqc]['rst'][]=restore_sql($m, 'u', 'maplanung', 'maplanung_id');

				if(($m['soll_beginn'] != $p['s']) || ($m['soll_ende'] != $p['e']))
					$az_update=true;
			}
		}
		// Yes, if the percentage sum is not equal 100%
		else if($p['z'] != 1.0)
		{
//echo(__LINE__.': Yes, if the percentage sum is not equal 100%<br />'.$GLOBALS['nl']);
			$sql_queue[$sqc]['log']='Adjust percentage for each resource because the sum of the percentage ('.($p['z']*100).'%) is not equal 100%';
			$sql_queue[$sqc]['ts']=time();

			$spz=0;
			$pct=0;
			foreach($mpd as $m)
			{
				if($pid != $m['projekt_id'])
					continue;
				$pct++;
				if($pct == $p['m'])	// Letzte zugeordnete Ressource
					$ppz=10000-$spz;
				else
					$ppz=(int)(((double)$m['aufwand_std'] / $p['r']) * 10000.0);
				$spz += $ppz;

				$rpz=((double)$ppz)/10000.0;
				$aw_std=$rpz*$p['p'];

				$sql_queue[$sqc]['sql'][]="UPDATE maplanung SET prozent=".$rpz." WHERE maplanung_id=".$m['maplanung_id']." AND mandanten_id=".$mid;
				$sql_queue[$sqc]['rst'][]=restore_sql($m, 'u', 'maplanung', 'maplanung_id');
			}
		}
		else
			continue;

		// Article assignment needs to be deleted?
		if($az_delete)
		{
			$asql="SELECT * FROM artikelzuordnung WHERE projekt_id=".$pid." AND artikeltyp=".$maart." AND mandanten_id=".$mid;
			$art=db_values($asql,'*');
			if(fxIsArray($art))
			{
				$sql_queue[$sqc]['sql'][]="DELETE FROM artikelzuordnung WHERE projekt_id=".$pid." AND artikeltyp=".$maart." AND mandanten_id=".$mid;
				foreach($art as $a)
					$sql_queue[$sqc]['rst'][]=restore_sql($a, 'd', 'artikelzuordnung', 'artikelzuord_id');
			}
		}

		// Adjust article assignment if date has changed
		if($az_update)
		{
			$asql="SELECT * FROM artikelzuordnung WHERE projekt_id=".$pid." AND artikeltyp=".$maart." AND artikel_id>0 AND mandanten_id=".$mid;
			$art=db_values($asql,'*');
			if(fxIsArray($art))
			{
				$sb=substr((string)$p['s'],0,8).substr((string)$art[0]['soll_beginn'], 8);
				$se=substr((string)$p['e'],0,8).substr((string)$art[0]['soll_ende'], 8);
				$sql_queue[$sqc]['sql'][]="UPDATE artikelzuordnung SET soll_beginn='".$sb."', soll_ende='".$se."' WHERE projekt_id=".$pid." AND artikeltyp=".$maart." AND artikel_id>0 AND mandanten_id=".$mid;
				foreach($art as $a)
					$sql_queue[$sqc]['rst'][]=restore_sql($a, 'u', 'artikelzuordnung', 'artikelzuord_id');
			}
		}

		$sqc++;
	}

//fxDebug($sql_queue, 'SQL-Queue');
	if(sizeof($sql_queue))
	{
		$logfile='';
		foreach($sql_queue as $sq)
		{
			if($write_log)
			{
				foreach($sq['rst'] as $z => $sql)
				{
					if(!$z)
					{
						$tm=fx_date('Y-m-d H:i:s', $sq['ts']).' ['.$sq['ts'].']';
						$logfile .= $tm.':  '.$sq['log'].$GLOBALS['nl_b'];
					}
					$logfile .= '   <- '.$sql.';'.$GLOBALS['nl_b'];
				}
			}
			foreach($sq['sql'] as $z => $sql)
			{
				if($dbaccess)
					$err=db_x($sql);
				else
				{
					$success=@db_query($sql);
					if($success)
						$err=false;
					else
						$err=true;
				}

				if($err)
					$status='ERROR -- Cancel';
				else
					$status='OK';
				if($write_log)
					$logfile .= '   -> '.$sql.';  {'.$status.'}'.$GLOBALS['nl_b'];

				if($err)
					break 2;
			}
		}

		if($logfile)
		{
			$filename=$GLOBALS['logpath'].'~rl'.fxNow().'.log';
			fxSave($filename, $logfile);
		}

		if($err)
			return true;
	}

	if($sendmail)
		sendProjectMails();

	return false;
}

/**
 * ???FUNCTION???
 *
 * @param various $pid  - Mandatory parameter:               ???PARAMETER???
 * @param integer $year - Optional parameter (default = 0):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function update_appointments($pid, $year=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$pid=(int)$pid;
	$year=(int)$year;
	$mid=$GLOBALS['fxpglobals']['client'];
	$err=false;

	$GLOBALS['_uama']=array('err'=>$err, 'ok'=>"", 'ko'=>"");

	$pa=array();
//$year=2020;
	if($year > 0)	// Alle Daten innerhalb eines Jahres anpassen: Wird aufgerufen beim Anlegen/Ändern der Feiertage
	{
		$sql  = "SELECT personen_id, termin_id, startzeit, endzeit, soll_st_tag, soll_beginn, terminart, urlaub";
		$sql .= " FROM termine WHERE mandanten_id=".$GLOBALS['fxpglobals']['client']." AND terminart IN (".$GLOBALS['_tca'][FXP_AC_ABSENT].")";
		$sql .= " AND (startzeit<='".substr('000'.$year,-4)."1231240000' AND endzeit>='".substr('000'.$year,-4)."0101000000')";
		$aa=db_values($sql,'*');
//fxDebug($aa,$sql);
		if(fxIsArray($aa))
		{
			foreach($aa as $a)
			{
				$pid=(int)$a['personen_id'];
				if(!isset($pa[$pid]))
					$pa[$pid]=array();
				if(!isset($pa[$pid]['aa']))
					$pa[$pid]['aa']=array();
				$pa[$pid]['aa'][]=$a;
			}
		}
	}
	else
		$pa[$pid]=array();
	if(!sizeof($pa))
		return $err;

	$wda=array('so', 'mo', 'di', 'mi', 'do', 'fr', 'sa');
	$ata=array();
	$pra=hasRightsForAllPersons();

	// Alle Personen durchgehen
	foreach($pa as $pid => $a)
	{
		// Personennamen ermitteln
		$noks=false;
		$nkos=false;
		$sql="SELECT pname, vorname FROM personen WHERE personen_id=".$pid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$na=db_values($sql);
		$name=$na['pname'];
		if(strlen((string)$na['vorname']))
			$name .= ', '.$na['vorname'];

		// Personenvertragsdaten ermitteln
		$da=getPersonWorkdata($pid, '', '', 'st', false, true);
//fxDebug($da,'$da: $pid='.$pid);

		// Termine ermitteln?
		if(!isset($a['aa']))
		{
			$sql  = "SELECT termin_id, startzeit, endzeit, soll_st_tag, soll_beginn, terminart, urlaub";
			$sql .= " FROM termine WHERE personen_id=".$pid." AND mandanten_id=".$GLOBALS['fxpglobals']['client']." AND terminart IN (".$GLOBALS['_tca'][FXP_AC_ABSENT].")";
			$sql .= " AND endzeit>='".$da['*']['sd']."000000'";
			$aa=db_values($sql,'*');
		}
		else
			$aa=$a['aa'];

		// Alle Termine durchgehen
		if(fxIsArray($aa))
		{
//fxDebug($aa,'$aa: $pid='.$pid);
			// Min-/Maxdatum
			$mnd='';
			$mxd='';
			foreach($aa as $a)
			{
				$sdt=substr((string)$a['startzeit'],0,8);
				$edt=substr((string)$a['endzeit'],0,8);
				if(!strlen((string)$mnd) || ($sdt < $mnd))
					$mnd=$sdt;
				if(!strlen((string)$mxd) || ($edt > $mxd))
					$mxd=$edt;
			}
//echo('$mnd=<b>'.$mnd.'</b>, $mxd=<b>'.$mxd.'</b><hr />');

			// Feiertage in diesem Zeitraum
			$fa=get_feiertage($mnd, $mxd);
//fxDebug($fa,'Feiertage im Zeitraum '.$mnd.'-'.$mxd); echo('<hr />');

			foreach($aa as $ta)
			{
//fxDebug($ta,'$ta');

				// Zeitabschnitt ermitteln
				$sdt=substr((string)$ta['startzeit'],0,8);
				$suz=substr((string)$ta['startzeit'],8);
				$edt=substr((string)$ta['endzeit'],0,8);
				$euz=substr((string)$ta['endzeit'],8);
				$abu=substr((string)$ta['soll_beginn'],8);
				$za='*';
				foreach($da as $dt => $va)
				{
					if(($dt != '*') && ($dt < $edt))
						$za=$dt;
				}

				// Termin anpassen entsprechen der Daten im entsprechenden Zeitraum
				$na=$ta;
				$hd=0.0;
				$na['startzeit']=$sdt;
				$na['endzeit']=$edt;
				if($suz == $abu)
				{
					$na['startzeit'] .= $da[$za]['zs'];
					if($ta['urlaub'] < 1.0)
					{
						$na['endzeit'] .= $da[$za]['zm'];
						$hd=0.5;
					}
					else
						$na['endzeit'] .= $da[$za]['ze'];
				}
				else
				{
					$na['startzeit'] .= $da[$za]['zn'];
					$na['endzeit'] .= $da[$za]['ze'];
					$hd=0.5;
				}
				$na['soll_st_tag']=$da[$za]['st'];
				$na['soll_beginn']='00000000'.$da[$za]['zs'];

				$cdt=$sdt;
				$wd=getWeekday($cdt);
				$dc=0.0;
				while($cdt <= $edt)
				{
					//$wda=array('so', 'mo', 'di', 'mi', 'do', 'fr', 'sa');
					if($da[$za][$wda[$wd]])
					{
						$dc += 1.0;
						if(fxIsArray($fa[$cdt]))
							$dc -= $fa[$cdt]['typ'];
					}
//echo('$cdt=<b>'.$cdt.'</b>, $wd=<b>'.$wd.'</b> &rarr; $dc=<b>'.$dc.'</b><br />');
					$cdt=time_add($cdt, 1, 'D');
					$wd++;
					if($wd > 6)
						$wd=0;
				}
				if($dc > 0.0)
					$dc -= $hd;
				$na['urlaub']=$dc;
//fxDebug($na,'$na');
//echo('<hr />');

				$sql='';
				foreach($ta as $ac => $av)
				{

					if($na[$ac] != $av)
					{
						if(strlen((string)$sql))
							$sql .= ", ";
						$sql .= $ac.'=';
						if(is_string($av))
							$sql .= "'".$na[$ac]."'";
						else
							$sql .= $na[$ac];
					}
				}
				if(strlen((string)$sql))
				{
					$sql="UPDATE termine SET ".$sql." WHERE termin_id=".$na['termin_id']." AND personen_id=".$pid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
					$err=db_x($sql);
//fxDebug($err, $sql);
					if($err)
						$em='ko';
					else
						$em='ok';

					if(!${'n'.$em.'s'})
					{
						${'n'.$em.'s'}=true;
						if(strlen((string)$GLOBALS['_uama'][$em]))
							$GLOBALS['_uama'][$em] .= '</ul><br />';
						$GLOBALS['_uama'][$em] .= '<u><b>'.$name.'</b>:</u><ul style="padding-left:16px;">';
					}

					$GLOBALS['_uama'][$em] .= '<li>'.get_entry($sdt, 'datum');
					if($edt != $sdt)
						$GLOBALS['_uama'][$em] .= ' - '.get_entry($edt, 'datum');
					else
						$GLOBALS['_uama'][$em] .= ', '.get_entry($suz,'zeit').'-'.get_entry($euz,'zeit');
					if($pra || ($pid == $GLOBALS['fxpglobals']['person']))
					{
						if(!isset($ata[$na['terminart']]))
							$ata[$na['terminart']]=meldung($na['terminart'],false);
						$GLOBALS['_uama'][$em] .= ' <i>('.$ata[$na['terminart']].')</i>';
					}
					$GLOBALS['_uama'][$em] .= '</li>';
				}
			}
		}
	}

	$GLOBALS['_uama']['err']=$err;
	if(strlen((string)$GLOBALS['_uama']['ok']))
		$GLOBALS['_uama']['ok'] .= '</ul>';
	if(strlen((string)$GLOBALS['_uama']['ko']))
		$GLOBALS['_uama']['ko'] .= '</ul>';
//fxDebug($GLOBALS['_uama'], '$GLOBALS[\'_uama\']');
	return $err;
}

/**
 * Get email addresses of persons
 *
 * @param string  $pids     - Mandatory parameter:                   CSV string of person id's
 * @param boolean $complex  - Optional parameter (default = false):  Only return email address (false) or more detailed info like name etc. (true)
 * @param boolean $settings - Optional parameter (default = false):  Don't use user settings (false) or use them like language etc. (true)
 *
 * @return Array with info or false on error
 */
function get_email_adress($pids, $complex=false, $settings=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!strlen((string)$pids))
		return false;

	$mid=$GLOBALS['fxpglobals']['client'];

	// Personen-ID's aufsplitten
	if(substr((string)$pids, 0, 1) == ',')
		$pids=trim(substr((string)$pids, 1));
	$pids=str_replace(' ', '', $pids);
	$pide=explode(',', $pids);
	if(!fxIsArray($pide))
		return false;

	$pida=array();
	foreach($pide as $pid)
		$pida[(int)$pid]['id']=(int)$pid;

	// 1.Schritt: Zusatzinfos
	if($complex)
	{
		foreach($pida as $pid => $pd)
		{
			if(!fxIsArray($GLOBALS['_person_list_store'][$pid]) || !isset($GLOBALS['_person_list_store'][$pid]['pname']))
			{
				// Geschlecht, Name + Vorname
				$sql="SELECT geschlecht, pname, vorname, anrede FROM personen WHERE personen_id=".$pid." AND mandanten_id=".$mid;
				$zd=db_values($sql);
				if(is_array($zd))
				{
					$GLOBALS['_person_list_store'][$pid]['id']=(int)$pid;
					$GLOBALS['_person_list_store'][$pid]['benutzer_id']=0;
					$GLOBALS['_person_list_store'][$pid]['geschlecht']=(int)$zd['geschlecht'];
					$GLOBALS['_person_list_store'][$pid]['pname']='';
					if(!is_null($zd['pname']) && strlen((string)$zd['pname']))
						$GLOBALS['_person_list_store'][$pid]['pname']=trim((string)$zd['pname']);
					$GLOBALS['_person_list_store'][$pid]['vorname']='';
					if(!is_null($zd['vorname']) && strlen((string)$zd['vorname']))
						$GLOBALS['_person_list_store'][$pid]['vorname']=trim((string)$zd['vorname']);
					$GLOBALS['_person_list_store'][$pid]['anrede']=(int)$zd['anrede'];
					$GLOBALS['_person_list_store'][$pid]['name1']=$GLOBALS['_person_list_store'][$pid]['pname'];
					$GLOBALS['_person_list_store'][$pid]['name2']=$GLOBALS['_person_list_store'][$pid]['pname'];
					if(strlen((string)$GLOBALS['_person_list_store'][$pid]['vorname']))
					{
						$GLOBALS['_person_list_store'][$pid]['name1'] .= ', '.$GLOBALS['_person_list_store'][$pid]['vorname'];
						$GLOBALS['_person_list_store'][$pid]['name2'] = $GLOBALS['_person_list_store'][$pid]['vorname'].' '.$GLOBALS['_person_list_store'][$pid]['name2'];
					}
					// Anrede
					$aa=array(310=>1491, 311=>1492, 312=>1493, 2803=>3276);
					$GLOBALS['_person_list_store'][$pid]['anrede_txt']=trim(meldung($GLOBALS['_person_list_store'][$pid]['anrede'], true));
					$GLOBALS['_person_list_store'][$pid]['anrede2']=$aa[$GLOBALS['_person_list_store'][$pid]['anrede']];
					$GLOBALS['_person_list_store'][$pid]['anrede2_txt']=trim(meldung($GLOBALS['_person_list_store'][$pid]['anrede2'], true));
					$GLOBALS['_person_list_store'][$pid]['edatum']='00000000000000';
					$GLOBALS['_person_list_store'][$pid]['adatum']='99999999240000';
					$GLOBALS['_person_list_store'][$pid]['pic']=person_pic($GLOBALS['_person_list_store'][$pid]['id'], 20, $GLOBALS['_person_list_store'][$pid]['geschlecht'], $mid, false);
				}
				// Eintritts- und Austrittsdatum
				$sql="SELECT maeintrittsdatum, maaustrittsdatum FROM madaten WHERE personen_id=".$pid." AND mandanten_id=".$mid;
				$zd=db_values($sql);
				if(fxIsArray($zd))
				{
					if(($zd['maeintrittsdatum'] !== NULL) && strlen((string)$zd['maeintrittsdatum']))
						$GLOBALS['_person_list_store'][$pid]['edatum']=substr((string)$zd['maeintrittsdatum'],0,8).'000000';
					if(($zd['maaustrittsdatum'] !== NULL) && strlen((string)$zd['maaustrittsdatum']))
						$GLOBALS['_person_list_store'][$pid]['adatum']=substr((string)$zd['maaustrittsdatum'],0,8).'240000';
				}
			}
			if(is_array($GLOBALS['_person_list_store'][$pid]))
				$pida[$pid]=$GLOBALS['_person_list_store'][$pid];
		}
	}

	// 2. Schritt: Email-Adressen
	foreach($pida as $pid => $pd)
	{
		if(!fxIsArray($GLOBALS['_person_list_store'][$pid]) || !fxIsArray($GLOBALS['_person_list_store'][$pid]['mails']))
		{
			$GLOBALS['_person_list_store'][$pid]['id']		= (int)$pid;
			$GLOBALS['_person_list_store'][$pid]['email']	= '';

			// Mitarbeiter nicht (mehr) in Firma, dann auch keine Emails versenden
			if(strlen((string)$GLOBALS['_person_list_store'][$pid]['edatum']) && ($GLOBALS['_person_list_store'][$pid]['edatum'] > $GLOBALS['datetime']))
				continue;
			if(strlen((string)$GLOBALS['_person_list_store'][$pid]['adatum']) && ($GLOBALS['_person_list_store'][$pid]['adatum'] < $GLOBALS['datetime']))
				continue;

			for($l=0; $l<3; $l++)
			{
				// 0. Aktive und gültige Benutzer-Email-Adressen
				if(!$l)
				{
					$sql  = "SELECT b.benutzer_id, b.e_mail_adresse FROM personen p, benutzer b";
					$sql .= " WHERE b.personen_id=p.personen_id AND b.mandanten_id=".$mid." AND p.personen_id=".$pid." AND p.mandanten_id=".$mid;
					$sql .= " AND (p.inaktiv IS NULL OR p.inaktiv=0)";
					$sql .= " AND (p.gueltig_von IS NULL OR p.gueltig_von<'".$GLOBALS['date']."240000') AND (p.gueltig_bis IS NULL OR p.gueltig_bis>'".$GLOBALS['date']."000000')";
					$sql .= " AND (b.status_benutzer IS NULL OR b.status_benutzer=0 OR b.status_benutzer=1)";
					$sql .= " AND (b.gueltig_von IS NULL OR b.gueltig_von<'".$GLOBALS['date']."240000') AND (b.gueltig_bis IS NULL OR b.gueltig_bis>'".$GLOBALS['date']."000000')";
					$sql .= " ORDER BY b.benutzer_id";
				}
				// 1. Aktive und gültige Personen-Standard-Email-Adressen
				else if($l == 1)
				{
					$sql  = "SELECT k.kommunikation_txt AS e_mail_adresse FROM personen p, kommunikation k";
					$sql .= " WHERE k.personen_id=p.personen_id AND k.mandanten_id=".$mid." AND p.personen_id=".$pid." AND p.mandanten_id=".$mid;
					$sql .= " AND (p.inaktiv IS NULL OR p.inaktiv=0) AND k.kommunikationstyp=2366";
					$sql .= " AND (p.gueltig_von IS NULL OR p.gueltig_von<'".$GLOBALS['date']."240000') AND (p.gueltig_bis IS NULL OR p.gueltig_bis>'".$GLOBALS['date']."000000')";
				}
				// 2. Aktive und gültige Benutzer-Email-Adresse des Hauptadministrators (laut Hr. Binder am 13.12.2010)
				else
				{
					$sql  = "SELECT e_mail_adresse FROM benutzer";
					$sql .= " WHERE benutzer_id=1 AND mandanten_id=".$mid;
					$sql .= " AND (status_benutzer IS NULL OR status_benutzer=0 OR status_benutzer=1)";
					$sql .= " AND (gueltig_von IS NULL OR gueltig_von<'".$GLOBALS['date']."240000') AND (gueltig_bis IS NULL OR gueltig_bis>'".$GLOBALS['date']."000000')";
				}
				$tmp=db_values($sql, '*');
				if(fxIsArray($tmp))
				{
					foreach($tmp as $t)
					{
						if(isset($t['benutzer_id']) && !$GLOBALS['_person_list_store'][$pid]['benutzer_id'])
						{
							$GLOBALS['_person_list_store'][$pid]['benutzer_id']=$t['benutzer_id'];
							$pida[$pid]['benutzer_id']=$t['benutzer_id'];
						}

						$ml=trim((string)$t['e_mail_adresse']);
						if(!strlen((string)$ml))
							continue;
						$atpos=strpos($ml, '@', 1);
						if(!$atpos)
							continue;

						$dotpos=strpos($ml, '.', $atpos);
						if(!$dotpos || ($dotpos == $atpos+1) || ($dotpos == strlen((string)$ml)-1))
							continue;

						$GLOBALS['_person_list_store'][$pid]['mails'][$ml]=true;
						if(!$GLOBALS['_person_list_store'][$pid]['email'])
							$GLOBALS['_person_list_store'][$pid]['email']=$ml;

						break;
					}
				}

				// Nur zuerst gefundenen Eintrag als Email-Adresse verwenden (lt. Hr. Binder vom 13.12.2010)
				if(strlen((string)$GLOBALS['_person_list_store'][$pid]['email']))
					break;
			}
		}
		if(fxIsArray($GLOBALS['_person_list_store'][$pid]['mails']))
		{
			$pida[$pid]['email']=$GLOBALS['_person_list_store'][$pid]['email'];
			$pida[$pid]['mails']=$GLOBALS['_person_list_store'][$pid]['mails'];
		}

		if(!$GLOBALS['_person_list_store'][$pid]['benutzer_id'])
		{
			$GLOBALS['_person_list_store'][$pid]['benutzer_id']=1;
			$pida[$pid]['benutzer_id']=1;
		}

		// Benutzereinstellungen
		if($settings && !$GLOBALS['_person_list_store'][$pid]['settings'])
		{
			$GLOBALS['_person_list_store'][$pid]['settings']=benutzereinstellungen($GLOBALS['_person_list_store'][$pid]['benutzer_id'],true,false);
			$pida[$pid]['settings']=$GLOBALS['_person_list_store'][$pid]['settings'];
		}
	}

	return $pida;
}

/**
 * Ermittelt den korrekten Vorgesetzten einer Person
 *
 * @param integer $pers - Optional parameter (default = 0): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function get_realelter($pers=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$mid=$GLOBALS['fxpglobals']['client'];
	$pers=(int)$pers;
	if(!$pers)
		$pers=$GLOBALS['fxpglobals']['person'];


	$sql="SELECT elter FROM personen WHERE personen_id=".$pers." AND mandanten_id=".$mid;
	$elt=(int)db_value($sql);
	// Prüfen, ob auch wirklich eine Person und keine Instanz als Elter eingetragen ist
	if($elt)
	{
		// 275:		Mitarbeiter (intern)
		// 620:		Auftraggeber - Ansprechpartner
		// 840:		Mitarbeiter (extern)
		// 841:		Kontakt - Ansprechpartner
		// 2090:	Mandant - Ansprechpartner
		// 2092:	Lieferant - Ansprechpartner
		$sql="SELECT personen_id FROM personen WHERE personen_id=".$elt." AND mandanten_id=".$mid." AND personentyp IN (275,620,840,841,2090,2092)";
		$elt=(int)db_value($sql);
	}

	return $elt;
}

/**
 * Personenrechte je Rollengruppe und Aktion ermitteln
 *
 * @return ???RETURN???
 */
function get_rights_gpa()
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// für Transaktion 222: Abwesenheitsplaner
	if($GLOBALS['fxpglobals']['tr'] == 222)
	{
		// Rollenbit				=> Aktion (1=Anzeigen, 2=Neuanlage, 3=Ändern, 4=Löschen) mit Werten (0=keine, 1=eingeschränkt, 2=alle)
		$gr=array(
			FXP_BAR_SYSADMIN			=> array(1=>2, 2, 2, 2),	// Sysadmin
			FXP_BAR_EXECUTIVE	=> array(1=>2, 2, 2, 2),	// Geschäftsleitung
			FXP_BAR_MANAGEMENT		=> array(1=>1, 1, 1, 1),	// Verwaltung
			FXP_BAR_PROJECTLEAD	=> array(1=>1, 0, 0, 0),	// Projektleiter
			FXP_BAR_DEPARTMENTLEAD	=> array(1=>1, 0, 0, 0),	// Abteilungsleiter
			FXP_BAR_SUPERVISOR		=> array(1=>1, 1, 1, 1)		// Vorgesetzter
		);
	}
	else
	{
		// Rollenbit				=> Aktion mit 0=keine, 1=eingeschränkt, 2=alle
		$gr=array(
			FXP_BAR_SYSADMIN			=> array(1=>2, 2, 2, 2),	// Sysadmin
			FXP_BAR_EXECUTIVE	=> array(1=>2, 2, 2, 2),	// Geschäftsleitung
			FXP_BAR_MANAGEMENT		=> array(1=>1, 1, 1, 1),	// Verwaltung
			FXP_BAR_PROJECTLEAD	=> array(1=>1, 0, 0, 0),	// Projektleiter
			FXP_BAR_DEPARTMENTLEAD	=> array(1=>1, 1, 1, 1),	// Abteilungsleiter
			FXP_BAR_SUPERVISOR		=> array(1=>1, 1, 1, 1)		// Vorgesetzter
		);
	}

	return $gr;
}

/**
 * Ermittelt die Rollengruppe einer Person sowie die Termine, die diese Person sehen darf
 *
 * @param integer $pers - Optional parameter (default = 0):    ???PARAMETER???
 * @param boolean $prs  - Optional parameter (default = true): ???PARAMETER???
 * @param boolean $app  - Optional parameter (default = true): ???PARAMETER???
 * @param string  $tart - Optional parameter (default = ''):   ???PARAMETER???
 * @param string  $von  - Optional parameter (default = ''):   ???PARAMETER???
 * @param string  $bis  - Optional parameter (default = ''):   ???PARAMETER???
 *
 * @return ???RETURN???
 */
function get_gpa($pers=0, $prs=true, $app=true, $tart='', $von='', $bis='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$mand=$GLOBALS['fxpglobals']['client'];
	$pers=(int)$pers;
	if(!$pers)
		$pers=$GLOBALS['fxpglobals']['person'];

	$gpa['grp']=FXP_BAR_SELF;

	// Zu welcher Rollengruppe gehört die Person? ...
	// ... Sysadmin
	if(!$GLOBALS['fxpglobals']['person'])
		$gpa['grp'] |= FXP_BAR_SYSADMIN;
	// ... Geschäftsleitung (auch Hauptadministrator)
	if($GLOBALS['fxpglobals']['persdat']['roles'][2] || ($GLOBALS['fxpglobals']['user'] == 1))
		$gpa['grp'] |= FXP_BAR_EXECUTIVE;
	// ... Verwaltung
	if($GLOBALS['fxpglobals']['persdat']['roles'][-1])
		$gpa['grp'] |= FXP_BAR_MANAGEMENT;
	// ... Projektleiter
	$prids="";
	if($GLOBALS['fxpglobals']['persdat']['roles'][1])
	{
		$gpa['grp'] |= FXP_BAR_PROJECTLEAD;
		$sql="SELECT projekt_id FROM projekte WHERE (projektleiter=".$pers." OR vertreter=".$pers.") AND mandanten_id=".$mand;
		$chk=(int)db_value($sql);
		if($chk)
			$prids=$sql;
	}
	// ... Abteilungsleiter
	$abids='';
	$abta=get_abteilungen($pers, $mand, true);
	if(fxIsArray($abta))
	{
		$gpa['grp'] |= FXP_BAR_DEPARTMENTLEAD;
		$abids=array_to_csv($abta);
	}
	// ... Vorgesetzter
	$sql="SELECT personen_id FROM personen WHERE elter=".$pers." AND mandanten_id=".$mand;
	$superior=db_values($sql, '*');
	if(fxIsArray($superior))
		$gpa['grp'] |= FXP_BAR_SUPERVISOR;
//echo('Person mit ID='.$pers.' gehört zur Gruppe: <B>'.$gpa['grp'].'</B><br />');

	if($prs || $app)
	{
		$gpa['par']=array($pers=>31);	// Aktionsrechte
		$gpa['pgr']=array($pers=>1);	// Rollenrechte
		$gpa['csv']='';

		// Rollengruppenrechte ermitteln
		$gr=get_rights_gpa();
//fxDebug($gr, 'Rollengruppenrechte');

		// Fremdpersonen ermitteln
		if(fxIsArray($gr))
		{
			foreach($gr as $gbit => $gakt)
			{
				$pakt=0;
				if($gakt[FXP_DISPLAY])	// Recht auf Anzeigen
					$pakt |= 2;
				if($gakt[FXP_CREATE])	// Recht auf Neuanlage
					$pakt |= 4;
				if($gakt[FXP_CHANGE])	// Recht auf Ändern
					$pakt |= 8;
				if($gakt[FXP_DELETE])	// Recht auf Löschen
					$pakt |= 16;

				// Falls die Person in dieser Rollengruppe ist...
				if($gpa['grp'] & $gbit)
				{
					$akt=(int)$gakt[$GLOBALS['aktion']];

					$gall=false;
					// Personen eingeschränkt entspr. Rollenbit
					if($akt == 1)
					{
						// Geschäftsleitung
						if($gbit == FXP_BAR_EXECUTIVE)
							$gall=true;

						// Verwaltung
						else if($gbit == FXP_BAR_MANAGEMENT)
						{
							// Gehört die Person einer Abteilung an?
							$abta=get_abteilungen($pers, $mand);
							// ...ja, dann alle Personen dieser Abteilung(en) ermitteln
							if(fxIsArray($abta))
							{
								$abt=array_to_csv($abta);
								$sql="SELECT personen_id FROM personen WHERE abt_id IN (".$abt.") AND mandanten_id=".$mand;
								$tmp=db_values($sql, '*');
								if(fxIsArray($tmp))
								{
									foreach($tmp as $t)
									{
										$gpa['par'][$t['personen_id']] |= $pakt;
										$gpa['pgr'][$t['personen_id']] |= $gbit;
									}
								}
							}
							// ...nein, dann alle Personen ermitteln
							else
								$gall=true;
						}

						// Projektleitung
						else if(($gbit == FXP_BAR_PROJECTLEAD) && strlen((string)$prids))
						{
							$sql  = "SELECT m.personen_id FROM projekte p, maplanung m";
							$sql .= " WHERE m.projekt_id=p.projekt_id AND m.personen_id IS NOT NULL AND p.projekt_id IN (".$prids.") AND p.mandanten_id=".$mand." AND m.mandanten_id=".$mand;
							$tmp=db_values($sql, '*');
							if(fxIsArray($tmp))
							{
								foreach($tmp as $t)
								{
									$gpa['par'][$t['personen_id']] |= $pakt;
									$gpa['pgr'][$t['personen_id']] |= $gbit;
								}
							}
						}

						// Abteilungsleitung
						else if(($gbit == FXP_BAR_DEPARTMENTLEAD) && $abids)
						{
							$sql="SELECT personen_id FROM personen WHERE abt_id IN (".$abids.") AND mandanten_id=".$mand;
							$tmp=db_values($sql, '*');
							if(fxIsArray($tmp))
							{
								foreach($tmp as $t)
								{
									$gpa['par'][$t['personen_id']] |= $pakt;
									$gpa['pgr'][$t['personen_id']] |= $gbit;
								}
							}
						}

						// Vorgesetzter
						else if(($gbit == FXP_BAR_SUPERVISOR) && is_array($superior))
						{
							foreach($superior as $t)
							{
								$gpa['par'][$t['personen_id']] |= $pakt;
								$gpa['pgr'][$t['personen_id']] |= $gbit;
							}
						}
					}

					// Alle Personen
					else if($akt == 2)
						$gall=true;

					if($gall)
					{
						$sql="SELECT personen_id FROM personen WHERE mandanten_id=".$mand;
						$tmp=db_values($sql, '*');
						if(fxIsArray($tmp))
						{
							foreach($tmp as $t)
							{
								$gpa['par'][$t['personen_id']] |= $pakt;
								$gpa['pgr'][$t['personen_id']] |= $gbit;
							}
						}
						break;
					}
				}
			}
		}

		// Alle Personen als CSV-String ermitteln
		if(fxIsArray($gpa['par']))
		{
			foreach($gpa['par'] as $p => $bit)
			{
				if(test_bit($bit, $GLOBALS['aktion']))
				{
					if($gpa['csv'])
						$gpa['csv'] .= ',';
					$gpa['csv'] .= $p;
				}
			}
		}
	}

	if($app)
	{
		// Zeitraum festlegen
		$gpa['flt'] |= 512;
		if(!strlen((string)$von))
		{
			$von='00000000000000';
			if($GLOBALS['fxpglobals']['setfilter'] && !$GLOBALS['fxpglobals']['filter']['fzeit_inaktiv'])
			{
				$gpa['flt'] |= 8;
				if(!$GLOBALS['ignore_von'] && !$GLOBALS['ignore_bis'] && strlen((string)$GLOBALS['fxpglobals']['filter']['ist_beg_dtm']))
				{
					$von=substr((string)$GLOBALS['fxpglobals']['filter']['ist_beg_dtm'],0,8).'000000';
					$gpa['flt'] |= 2080;
				}
			}
			else
				$gpa['flt'] |= 1040;
		}
		else
			$von=substr((string)$von,0,8).'000000';
		if(!strlen((string)$bis))
		{
			$bis='99999999999999';
			if($GLOBALS['fxpglobals']['setfilter'] && !$GLOBALS['fxpglobals']['filter']['fzeit_inaktiv'])
			{
				$gpa['flt'] |= 8;
				if(!$GLOBALS['ignore_von'] && !$GLOBALS['ignore_bis'] && strlen((string)$GLOBALS['fxpglobals']['filter']['ist_end_dtm']))
				{
					$bis=substr((string)$GLOBALS['fxpglobals']['filter']['ist_end_dtm'],0,8).'240000';
					$gpa['flt'] |= 2080;
				}
			}
			else
				$gpa['flt'] |= 1040;
		}
		else
			$bis=substr((string)$bis,0,8).'240000';
//echo('<B>$von:</B> '.$von.', <B>$bis:</B> '.$bis.'<br />');
		$gpa['sd']=$von;
		$gpa['ed']=$bis;

		// Termine ermitteln
		$sql  = "SELECT t.termin_id, t.mastertermin_id, t.personen_id, t.ansprechpartner, t.person_angelegt, t.name_termin,";
		$sql .= " t.terminart, t.startzeit, t.endzeit, t.wiederholung, t.erledigt, t.hat_bestaetigt,";
		$sql .= " m.terminart AS masterterminart,";
		$sql .= " p.pname, p.vorname";
		$sql .= " FROM (personen p LEFT JOIN termine t ON t.personen_id=p.personen_id AND p.mandanten_id=".$mand."), termine m";
		$sql .= " WHERE m.termin_id=t.mastertermin_id AND m.mandanten_id=".$mand." AND t.mandanten_id=".$mand." AND (t.archiv=0 OR t.archiv IS NULL)";

		// Personen einschränken
		$sql2=filter_person('and', false, false, 'filterperson');
		$gpa['flt'] |= $GLOBALS['filterperson_flt'];

		// Terminarten einschränken?
		if(strlen((string)$tart))
			$sql2 .= " AND t.terminart IN (".$tart.")";
//echo('<B>$tart:</B> '.$tart.'<br />');

		// Alle Termine...
		if($gpa['csv'])
			$sql2 .= " AND t.personen_id IN (".$gpa['csv'].")";
		$sql2 .= " AND (";
		// ...die im Zeitraum liegen...
		$sql2 .= "(t.startzeit<='".$bis."' AND t.endzeit>='".$von."')";
		// ...oder die im zukünftigen Zeitraum wiedervorzulegen sind...
		$sql2 .= " OR (t.wiedervorlagedb>='".$von."' AND t.wiedervorlagedb<='".$bis."')";
		// ...oder die verschiebbar sind
		$sql2 .= " OR t.verschieben=1";
		$sql2 .= ")";

		// Ausser man ist nicht im Anzeigemodus und die Termine wurden bereits abgesagt
		if($GLOBALS['aktion'] > FXP_DISPLAY)
			$sql2 .= " AND (t.bestaetigung_anmerk IS NULL OR t.bestaetigung_anmerk NOT LIKE '0-%')";

		if($_GET['Termin_ID'])
			$sql .= " AND (t.termin_id=".$_GET['Termin_ID']." OR (".substr((string)$sql2, 5)."))";
		else
			$sql .= $sql2;

		// Sortierreihenfolge
		$sql .= " ORDER BY p.pname, p.vorname, t.startzeit DESC, t.name_termin";
		$gpa['sql']=$sql;

		$gpa['app']=array();
		$tmp=db_values($sql, '*');
//fxDebug($tmp, $sql);
		if(fxIsArray($tmp))
		{
			for($i=0; $i<2; $i++)
			{
				foreach($tmp as $t)
				{
					if((!$i && ($t['personen_id'] != $pers)) || ($i && ($t['personen_id'] == $pers)))
						continue;

					$gpa['app'][$t['termin_id']]=$t;
				}
			}
		}
	}

	return $gpa;
}

/**
 * Personendaten ermitteln
 *
 * @param various $pid      - Mandatory parameter:                   ???PARAMETER???
 * @param integer $mid      - Optional parameter (default = 0):      ???PARAMETER???
 * @param boolean $full     - Optional parameter (default = true):   ???PARAMETER???
 * @param boolean $justname - Optional parameter (default = false):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function get_persondata($pid, $mid=0, $full=true, $justname=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$pid=(int)$pid;
	if($pid < 0)	// Hack: Negative Personen-IDs entsprechend Benutzer-IDs
		$pid=(int)db_value("SELECT personen_id FROM benutzer WHERE benutzer_id=".abs($pid)." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
	$mid=(int)$mid;
	if(!$mid)
		$mid=$GLOBALS['fxpglobals']['client'];

	$mpid=$mid.'_'.$pid;
	if(fxIsArray($GLOBALS['_gpda']) && fxIsArray($GLOBALS['_gpda'][$mpid]))
		return $GLOBALS['_gpda'][$mpid];

	// Name + Abteilung
	$sql  = "SELECT personen_id, mandanten_id, personentyp, geschlecht, pname, vorname, elter, abt_id, pposition";
	$sql .= " FROM personen WHERE personen_id=".$pid." AND mandanten_id=".$mid;
	$pdata=db_values($sql);
	if(fxIsArray($pdata))
	{
		$pnm=trim((string)$pdata['pname']);
		$pvn=trim((string)$pdata['vorname']);

		$pdata['name']=$pnm;
		$pdata['name2']=$pnm;

		if(strlen((string)$pvn))
		{
			$pdata['name']=$pvn.' '.$pnm;
			$pdata['name2'] .= ', '.$pvn;
		}

		if(!$justname)
		{
			$pdata['pposition']=meldung($pdata['pposition'], true);

			if($pdata['abt_id'])
			{
				$sql="SELECT abt_ebene, abt_name FROM abteilung WHERE abt_id=".$pdata['abt_id']." AND mandanten_id=".$pdata['mandanten_id'];
				$abt_data=db_values($sql);
				if(is_array($abt_data))
					$pdata['abt']=meldung(40, true).' '.$abt_data['abt_ebene'].': '.$abt_data['abt_name'];
			}
		}
	}
	else
	{
		$pdata=array(
			'personen_id'	=> $pid,
			'mandanten_id'	=> $mid,
			'name'			=> '('.$pid.')'
		);
		if(!$pid)
			$pdata['name']='[fx-project]';
		$pdata['name2']=$pdata['name'];
		$full=false;
	}

	if($full)
	{
		// Email-Adresse aus Benutzer
		if(fxIsArray($pdata))
		{
			$sql="SELECT e_mail_adresse FROM benutzer WHERE personen_id=".$pid." AND mandanten_id=".$mid;
			$pdata['e_mail_adresse']=trim((string)db_value($sql));
		}

		// Arbeitsstunden
		if(fxIsArray($pdata))
		{
			$sql  = "SELECT wochenarbeitstd, pause, arbeitstage, soll_beginn, maeintrittsdatum, maaustrittsdatum,";
			$sql .= " 0.0 AS sollstunden_tag, wochenarbeitstd AS sollstunden_woche,";
			$sql .= " sonntag AS a, montag AS b, dienstag AS c, mittwoch AS d, donnerstag AS e, freitag AS f, samstag AS g";
			$sql .= " FROM madaten WHERE personen_id=".$pid." AND mandanten_id=".$mid;
			$data=db_values($sql);
			if(!fxIsArray($data))
			{
				$data=array(
					'wochenarbeitstd'	=> 0.0,
					'pause'				=> 0.0,
					'arbeitstage'		=> 0,
					'soll_beginn'		=> '00000000000000',
					'maeintrittsdatum'	=> '00000000000000',
					'maaustrittsdatum'	=> '',
					'sollstunden_tag'	=> 0.0,
					'sollstunden_woche'	=> 0.0,
					'a'					=> 0,
					'b'					=> 0,
					'c'					=> 0,
					'd'					=> 0,
					'e'					=> 0,
					'f'					=> 0,
					'g'					=> 0
				);
			}

			$arbeitstage_woche=(int)$data['arbeitstage'];
			$sollstunden_woche=get_double($data['wochenarbeitstd']);
			$sollstunden_tag=0.0;
			if($arbeitstage_woche > 0)
				$sollstunden_tag=$sollstunden_woche/$arbeitstage_woche;

			$data['sollstunden_tag']=$sollstunden_tag;
			$data['sollstunden_woche']=$sollstunden_woche;

			for($wtc=0; $wtc<7; $wtc++)
			{
				$data[$wtc]=0;
				if($data[chr(0x61 + $wtc)])
					$data[$wtc]=1;
			}

			$pdata=array_merge($pdata, $data);
		}
	}

	if(fxIsArray($GLOBALS['_gpda']))
		$GLOBALS['_gpda']=array();
	$GLOBALS['_gpda'][$mpid]=$pdata;

	return $GLOBALS['_gpda'][$mpid];
}

/**
 * ???FUNCTION???
 *
 * @param various $pid   - Mandatory parameter:               ???PARAMETER???
 * @param integer $mxh   - Optional parameter (default = 0):  ???PARAMETER???
 * @param integer $sex   - Optional parameter (default = 0):  ???PARAMETER???
 * @param integer $cid   - Optional parameter (default = 0):  ???PARAMETER???
 * @param integer $ptype - Optional parameter (default = 0):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function getPersonSelectIcon($pid, $mxh=0, $sex=0, $cid=0, $ptype=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$s='';

	$pers_icons=getPersonIcons($pid, $mxh, $sex, $cid, $ptype, true);
//fxDebug($pers_icons,'$pers_icons: '.$p['personen_id']);
	if(!$pers_icons['def'])
	{
		if(strlen((string)$pers_icons['dsp']))
			$ds='dsp';
		else if(strlen((string)$pers_icons['256']))
			$ds='256';
		else if(strlen((string)$pers_icons['128']))
			$ds='128';
		else if(strlen((string)$pers_icons['100']))
			$ds='100';
		else
			$ds='src';
		$s .= '<tt><img src='.$pers_icons[$ds].$pers_icons['mtime'].$pers_icons['size_'.$ds].' style=max-height:'.$mxh.'px;width:auto;margin-top:4px;border-radius:4px; /></tt>';
	}
	$op='';
	if($pers_icons['def'])
		$op='opacity:0.10;';
	$s .= '<img src='.$pers_icons['ico'].$pers_icons['mtime'].' align=top width=13 height=13 style=\'border:1px solid #d2d2d2;border-radius:2px;'.$op.'\' />&nbsp;';

	return $s;
}

/**
 * ???FUNCTION???
 *
 * @param various $pid       - Mandatory parameter:                   ???PARAMETER???
 * @param integer $mxh       - Optional parameter (default = 20):     ???PARAMETER???
 * @param integer $sex       - Optional parameter (default = 0):      ???PARAMETER???
 * @param integer $cid       - Optional parameter (default = 0):      ???PARAMETER???
 * @param boolean $with_name - Optional parameter (default = true):   ???PARAMETER???
 * @param string  $style     - Optional parameter (default = ''):     ???PARAMETER???
 * @param boolean $large     - Optional parameter (default = false):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function person_pic($pid, $mxh=20, $sex=0, $cid=0, $with_name=true, $style='', $large=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$cid=(int)$cid;
	if(!$cid)
		$cid=$GLOBALS['fxpglobals']['client'];

	$pers_icons=getPersonIcons($pid, $mxh, 0, $cid, 0, true);
//fxDebug($pers_icons,'$pers_icons: $pid='.$pid.', $mxh='.$mxh);

	$fname='';
	if($large && (($mxh <= 0) || ($mxh > 20)))
	{
		if(strlen((string)$pers_icons['100']) && (($mxh > 20) && ($mxh <= 100)))
		{
			$fname=$pers_icons['100'];
			$psize=$pers_icons['size_100'];
		}
		else if(strlen((string)$pers_icons['128']) && (($mxh > 20) && ($mxh <= 128)))
		{
			$fname=$pers_icons['128'];
			$psize=$pers_icons['size_128'];
		}
		else if(strlen((string)$pers_icons['256']) && (($mxh > 20) && ($mxh <= 256)))
		{
			$fname=$pers_icons['256'];
			$psize=$pers_icons['size_256'];
		}
		else if(strlen((string)$pers_icons['src']))
		{
			$fname=$pers_icons['src'];
			$psize=$pers_icons['size_src'];
		}
	}
	if(!strlen((string)$fname) && ($mxh == 20) && !strlen((string)$pers_icons['src']) && strlen((string)$pers_icons['ico']))
	{
		$fname=$pers_icons['ico'];
		$psize=$pers_icons['size_ico'];
	}
	if(!strlen((string)$fname) && strlen((string)$pers_icons['dsp']))
	{
		$fname=$pers_icons['dsp'];
		$psize=$pers_icons['size_dsp'];
	}
	if(!strlen((string)$fname) && strlen((string)$pers_icons['ico']))
	{
		$fname=$pers_icons['ico'];
		$psize=$pers_icons['size_ico'];
	}

	if(!$large && $mxh)
		$psize=' width='.$mxh.' height='.$mxh;

	if($with_name === 0)
		return $fname;

	$style=trim((string)$style);
	if($pers_icons['def'])
		$style .= 'opacity:0.10;';
	if(strlen((string)$style))
		$style=' style=\''.$style.'\'';

	$txt='<img src='.$fname.$pers_icons['mtime'].$psize.' align=top'.$style.'>';

	if(!$with_name)
		return $txt;

	// Add name to picture
	$name=$pers_icons['name'];

	$pin=false;
	if($pers_icons['inact'] || ((($pers_icons['vfrom'] !== NULL) && strlen((string)$pers_icons['vfrom']) && (substr((string)$pers_icons['vfrom'],0,8) > substr((string)$GLOBALS['datetime'],0,8))) || (($pers_icons['vto'] !== NULL) && strlen((string)$pers_icons['vto']) && (substr((string)$pers_icons['vto'],0,8) < substr((string)$GLOBALS['datetime'],0,8)))))
		$pin=true;

	if(($pers_icons['type'] == 620) && $pers_icons['parent'])
	{
		$sql="SELECT pname, vorname, inaktiv, gueltig_von, gueltig_bis FROM personen WHERE personen_id=".$pers_icons['parent']." AND mandanten_id=".$cid;
		$ppd=db_values($sql);
//fxDebug($ppd,$sql);
		if(is_array($ppd))
		{
			if($ppd['pname'] !== NULL)
				$pfname=trim((string)$ppd['pname']);
			else
				$pfname='';
			if($ppd['vorname'] !== NULL)
				$psname=trim((string)$ppd['vorname']);
			else
				$psname='';
			if(strlen((string)$psname))
				$pfname .= ', '.$psname;
			if(!$pin && $ppd['inaktiv'] || (($ppd['gueltig_von'] !== NULL) && strlen((string)$ppd['gueltig_von']) && (substr((string)$ood['gueltig_von'],0,8) > substr((string)$GLOBALS['datetime'],0,8))) || (($ppd['gueltig_bis'] !== NULL) && strlen((string)$ppd['gueltig_bis']) && (substr((string)$ppd['gueltig_bis'],0,8) < substr((string)$GLOBALS['datetime'],0,8))))
				$pin=true;

			$name=$pfname.' - '.$name;
		}
	}

	if($pin)
		$name='<font class="grey">'.$name.'</font> <i class="lightergrey">{'.get_text(1391,'20').'}</i>';	// {inaktiv}

	return $txt.'&nbsp;'.$name;
}

/**
 * ???FUNCTION???
 *
 * @param boolean $ignore_personenrechte - Optional parameter (default = false): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function hasRightsForAllPersons($ignore_personenrechte=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$all=false;
	// ...Sysadmin, Administrator or General Manager
	if(!$GLOBALS['fxpglobals']['person'] || ($GLOBALS['fxpglobals']['user'] == 1) || $GLOBALS['fxpglobals']['persdat']['roles'][2])
		$all=true;
	// ...Resource Planner or Customer in resource programs
	else if((($GLOBALS['fxpglobals']['tr'] == 23) || ($GLOBALS['fxpglobals']['tr'] == 43) || ($GLOBALS['fxpglobals']['tr'] == 189)) && ($GLOBALS['fxpglobals']['persdat']['roles'][-3]) || ($GLOBALS['fxpglobals']['persdat']['personentyp'] == FXP_PT_CUSTOMER_CP))
		$all=true;
	// ...Set by program
	else if((isset($ignore_personenrechte) && $ignore_personenrechte) || (isset($GLOBALS['ignore_personenrechte']) && $GLOBALS['ignore_personenrechte']))
		$all=true;

	return $all;
}

/**
 * =============================================================================
 * GET_PERSONS
 * =============================================================================
 * Funktion ermittelt alle Personen-ID´s, die eine Person sehen darf.
 * -----------------------------------------------------------------------------
 * $retarray	falls true, dann als Array statt als CSV zurückgeben
 *
 * @param boolean $retarray - Optional parameter (default = false): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function get_persons($retarray=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$benu=$GLOBALS['fxpglobals']['user'];
	$pers=$GLOBALS['fxpglobals']['person'];
	$mand=$GLOBALS['fxpglobals']['client'];
//echo('$benu='.$benu.', $pers='.$pers.', $mand='.$mand.'<hr>');

/*
	if(fxIsArray($GLOBALS['vpids']))
	{
		if(!isset($GLOBALS['vpids'][-1]))
			unset($GLOBALS['vpids']);
	}
*/

	if(!fxIsArray($GLOBALS['vpids']))
	{
		$GLOBALS['vpids']=array(-1=>$GLOBALS['datetime']);

		// Erweiterte Projekterechte?
		$extended=true;
		if(($GLOBALS['fxpglobals']['tr'] == 32) || ($GLOBALS['fxpglobals']['tr'] == 49) || ($GLOBALS['fxpglobals']['tr'] == 27) || ($GLOBALS['fxpglobals']['tr'] == 222) || ($GLOBALS['fxpglobals']['tr'] == 41))	// Terminetransaktionen
			$extended=false;

		// Personen-Array vorbelegen mit akt. Person (Bit 0=1)
		$GLOBALS['vpids'][$pers]=1;

		// Recht auf alle Personen
		$all=hasRightsForAllPersons();
		if($all)
		{
			$sql="SELECT personen_id FROM personen WHERE mandanten_id=".$mand;
			$tmp=db_values($sql, '*');
			if(fxIsArray($tmp))
			{
				$bit=0;
				// Bit 15=32768: Sysadmin
				if(!$pers)
					$bit |= 32768;
				// Bit 14=16384: Administrator
				if($benu == 1)
					$bit |= 16384;
				// Bit 13=8192: Geschäftsleitung
				if($GLOBALS['fxpglobals']['persdat']['roles'][2])
					$bit |= 8192;
				// Bit 12=4096: Rechte ignorieren
				if(isset($GLOBALS['ignore_personenrechte']))
					$bit |= 4096;
				foreach($tmp as $t)
					$GLOBALS['vpids'][$t['personen_id']] |= $bit;
			}
		}

		else
		{
			// Bit 10=1024 (+ Bit 11=2048=Elter): Rechtevergabe: Benutzer, Benutzergruppe + Abteilung
			// ...Benutzergruppe
			if(!fxIsArray($GLOBALS['_tmp_bgr']) || !isset($GLOBALS['_tmp_bgr'][$pers]))
			{
				$bgrar=get_benutzergruppen($benu);
				if(!fxIsArray($GLOBALS['_tmp_bgr']))
					$GLOBALS['_tmp_bgr']=array();
				if(fxIsArray($bgrar))
					$GLOBALS['_tmp_bgr'][$pers]=array_to_csv($bgrar);
				else
					$GLOBALS['_tmp_bgr'][$pers]='';
			}
			// ...Abteilung
			if(!fxIsArray($GLOBALS['_tmp_abt']) || !isset($GLOBALS['_tmp_abt'][$pers]))
			{
				$abtar=get_abteilungen($pers);
				if(!fxIsArray($GLOBALS['_tmp_abt']))
					$GLOBALS['_tmp_abt']=array();
				if(fxIsArray($abtar))
					$GLOBALS['_tmp_abt'][$pers]=array_to_csv($abtar);
				else
					$GLOBALS['_tmp_abt'][$pers]='';
			}
			$bba=array(2163=>$benu, 2164=>$GLOBALS['_tmp_bgr'][$pers], 2165=>$GLOBALS['_tmp_abt'][$pers]);
			foreach($bba as $bga => $bgv)
			{
				if(strlen((string)$bgv))
				{
					$sql  = "SELECT p.personen_id, p.elter, p.personentyp FROM rechte_referenz r, personen p";
					$sql .= " WHERE r.rechte_ref=36 AND r.gruppeart=".$bga." AND r.benutzergr_id IN (".$bgv.") AND r.mandanten_id=".$mand;
					$sql .= " AND p.personen_id=r.id_referenz AND p.mandanten_id=".$mand;
					$tmp=db_values($sql,'*');
//fxDebug($tmp,$sql);
					if(fxIsArray($tmp))
					{
						foreach($tmp as $t)
						{
							$GLOBALS['vpids'][$t['personen_id']] |= 1024;
							if(($t['personentyp'] == FXP_PT_CLIENT_CP) || ($t['personentyp'] == FXP_PT_CUSTOMER_CP) || ($t['personentyp'] == FXP_PT_PARTNER_CP) || ($t['personentyp'] == FXP_PT_CONTACT_CP))
								$GLOBALS['vpids'][$t['elter']] |= 2048;
						}
					}
				}
			}

			// Bit 9=512: Verwaltung
			if($GLOBALS['fxpglobals']['persdat']['roles'][-1])
			{
				// Gehört die Person einer Abteilung an? (Ausnahme: Im Anzeigemodus hat Verwaltung Recht auf alle Personen, deshalb Abteilung nicht ermitteln - Anordnung WB: 12.12.2011)
				if($GLOBALS['fxpglobals']['action'] == FXP_DISPLAY)
					$abta=false;
				else
					$abta=get_abteilungen($pers, $mand);
				// ...ja, dann alle Mitarbeiter (int/ext) dieser Abteilung(en) sowie alle anderen Personenytpen ermitteln
				if(fxIsArray($abta))
				{
					$abt=array_to_csv($abta);
					$sql="SELECT personen_id FROM personen WHERE mandanten_id=".$mand." AND ((abt_id IN (".$abt.") AND personentyp IN (275,840)) OR personentyp NOT IN (275,840))";
				}
				// ...nein, dann alle Personen ermitteln
				else
					$sql="SELECT personen_id FROM personen WHERE mandanten_id=".$mand;

				$tmp=db_values($sql, '*');
//fxDebug($tmp,$sql);
				if(fxIsArray($tmp))
				{
					foreach($tmp as $t)
						$GLOBALS['vpids'][$t['personen_id']] |= 512;
				}
			}

			// Bit 8=256: Abteilungsleiter
			$abta=get_abteilungen($pers, $mand, true);
			if(fxIsArray($abta))
			{
				$abt=array_to_csv($abta);
				$sql="SELECT personen_id FROM personen WHERE abt_id IN (".$abt.") AND mandanten_id=".$mand;
				$tmp=db_values($sql, '*');
//fxDebug($tmp,$sql);
				if(fxIsArray($tmp))
				{
					foreach($tmp as $t)
						$GLOBALS['vpids'][$t['personen_id']] |= 256;
				}
			}

			// Bit 7=128: Auftraggeber-Spezialberechtigung
			$sql="SELECT kunde, ansprechpartner FROM projekte WHERE (projektmanager=".$pers." OR projektleiter=".$pers." OR vertreter=".$pers.") AND mandanten_id=".$mand;
			$tmp=db_values($sql, '*');
//fxDebug($tmp,$sql);
			if(fxIsArray($tmp))
			{
				foreach($tmp as $t)
				{
					$GLOBALS['vpids'][$t['kunde']] |= 128;
					$GLOBALS['vpids'][$t['ansprechpartner']] |= 128;
				}
			}

			// Bit 6=64: Lieferant / Lieferant - Ansprechpartner
			if($GLOBALS['fxpglobals']['persdat']['personentyp'] == FXP_PT_PARTNER_CP)
			{
				$sql="SELECT personen_id FROM personen WHERE elter=".$GLOBALS['fxpglobals']['persdat']['elter']." AND personen_id>0 AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
				$tmp=db_values($sql, '*');
//fxDebug($tmp,$sql);
				if(fxIsArray($tmp))
				{
					foreach($tmp as $t)
						$GLOBALS['vpids'][$t['personen_id']] |= 64;
				}
			}

			// Bit 5=32: Auftraggeber / Auftraggeber - Ansprechpartner
			if($GLOBALS['fxpglobals']['persdat']['personentyp'] == FXP_PT_CUSTOMER_CP)
			{
				if($extended)
				{
					$sql  = "SELECT m.personen_id FROM projekte p, maplanung m";
					$sql .= " WHERE m.projekt_id=p.projekt_id AND m.personen_id IS NOT NULL AND p.ansprechpartner=".$pers." AND p.mandanten_id=".$mand." AND m.mandanten_id=".$mand;
					$sql .= " GROUP BY m.personen_id";
					$tmp=db_values($sql, '*');
//fxDebug($tmp,$sql);
					if(fxIsArray($tmp))
					{
						foreach($tmp as $t)
							$GLOBALS['vpids'][$t['personen_id']] |= 32;
					}
				}

				$sql="SELECT personen_id FROM personen WHERE elter=".$GLOBALS['fxpglobals']['persdat']['elter']." AND personen_id>0 AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
				$tmp=db_values($sql, '*');
//fxDebug($tmp,$sql);
				if(fxIsArray($tmp))
				{
					foreach($tmp as $t)
						$GLOBALS['vpids'][$t['personen_id']] |= 32;
				}
			}

			// Bit 4=16: Projektleitung
			if($GLOBALS['fxpglobals']['persdat']['roles'][1])
			{
				$sql  = "SELECT m.personen_id FROM projekte p, maplanung m";
				$sql .= " WHERE m.projekt_id=p.projekt_id AND m.personen_id IS NOT NULL AND (p.projektleiter=".$pers." OR p.vertreter=".$pers.") AND p.mandanten_id=".$mand." AND m.mandanten_id=".$mand;
				$tmp=db_values($sql, '*');
//fxDebug($tmp,$sql);
				if(fxIsArray($tmp))
				{
					foreach($tmp as $t)
						$GLOBALS['vpids'][$t['personen_id']] |= 16;
				}
			}

			// Bit 2=4: Vorgesetzter
			$elter=$pers;
			$elt_rem=array($pers => true);
			while(strlen((string)$elter))
			{
				$sql="SELECT personen_id FROM personen WHERE elter IN (".$elter.") AND mandanten_id=".$mand;
				$tmp=db_values($sql, '*');
//fxDebug($tmp,$sql);
				$elter='';
				if(fxIsArray($tmp))
				{
					foreach($tmp as $t)
					{
						$epid=(int)$t['personen_id'];
						if(isset($elt_rem[$epid]))
							continue;

						$elt_rem[$epid]=true;
						$GLOBALS['vpids'][$epid] |= 4;

						if($elter)
							$elter .= ',';
						$elter .= $epid;
					}
				}
			}

			// Bit 1=2: Erstellt
			$sql="SELECT DISTINCT(personen_id) FROM personen_history WHERE aenderungs_id=".$pers." AND aktion=2 AND mandanten_id=".$mand;
			$tmp=db_values($sql, '*');
//fxDebug($tmp,$sql);
			if(fxIsArray($tmp))
			{
				foreach($tmp as $t)
					$GLOBALS['vpids'][$t['personen_id']] |= 2;
			}
/*
			// ...oder zuletzt geändert
			$sql="SELECT DISTINCT(personen_id) FROM personen WHERE aenderungs_id=".$pers." AND mandanten_id=".$mand;
			$tmp=db_values($sql, '*');
//fxDebug($tmp,$sql);
			if(fxIsArray($tmp))
			{
				foreach($tmp as $t)
					$GLOBALS['vpids'][$t['personen_id']] |= 2;
			}
*/
		}
//fxDebug($GLOBALS['vpids'], __LINE__.': Gefundene Personen-IDs='.sizeof($GLOBALS['vpids']));
	}

	// CSV-String erstellen...
	$spids='0';
	if(fxIsArray($GLOBALS['vpids']))
	{
		foreach($GLOBALS['vpids'] as $pid => $bit)
			$spids .= ','.$pid;
	}

	// ...danach zurückliefern
	if($retarray)
		return $GLOBALS['vpids'];

	return $spids;
}

/**
 * ???FUNCTION???
 *
 * @param various $persid - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function get_personinfo($persid)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$mid	= $GLOBALS['fxpglobals']['client'];
	$persid	= (int)$persid;
	if($persid < 1)
		return false;

	$sql="SELECT pname, vorname, personentyp, name_zusatz, geschlecht, abt_id FROM personen WHERE personen_id=".$persid." AND mandanten_id=".$mid;
	$pe_info=db_values($sql);

	// Name
	$pt=$pe_info['personentyp'];
	$pe_info['name']=$pe_info['pname'];
	if($pe_info['vorname'] && ($pt != 274) && ($pt != 276) && ($pt != 2091) && ($pt != 1372))
		$pe_info['name'] .= ', '.$pe_info['vorname'];
	if($pt == 840)
		$pe_info['name'] .= ' [Ext]';

	// Abteilung
	if(($pt == 275) || ($pt == 840))
	{
		if($pe_info['abt_id'])
			$abtinfo=db_values("SELECT abt_ebene, abt_name FROM abteilung WHERE abt_id=".$pe_info['abt_id']." AND mandanten_id=".$mid);
		else
			$abtinfo=array('*', db_value("SELECT pname FROM personen WHERE personentyp=274 AND mandanten_id=".$mid));
		if($abtinfo[0] == '*')
			$pe_info['abteilung']='['.$abtinfo[1].']';
		else
			$pe_info['abteilung']=$abtinfo[0].': ['.$abtinfo[1].']';
	}

	return $pe_info;
}

/**
 * $pers_id:	Personen-ID
 * $proj_id:	Aufgaben-ID
 * $aufw:		0=Auslastung pro Tag, 1=Aufwand pro Tag,
 * $sdate:		Startdatum
 * $edate:		Enddatum
 * $mdays:		max. Anzahl der Tage im Zeitstrahl (3 Jahre=3365=1095)
 *
 * @param various $pers_id - Mandatory parameter:                  ???PARAMETER???
 * @param various $proj_id - Mandatory parameter:                  ???PARAMETER???
 * @param integer $aufw    - Optional parameter (default = 1):     ???PARAMETER???
 * @param string  $dsdate  - Optional parameter (default = ''):    ???PARAMETER???
 * @param string  $dedate  - Optional parameter (default = ''):    ???PARAMETER???
 * @param integer $mdays   - Optional parameter (default = 1095):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function get_aufwand_detail($pers_id, $proj_id, $aufw=1, $dsdate='', $dedate='', $mdays=1095)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$pers_id=(int)$pers_id;
	if($pers_id <= 0)
		return false;
	$proj_id=(int)$proj_id;
	if($proj_id < 0)
		return false;
	$mid=$GLOBALS['fxpglobals']['client'];

	$aufw=(int)$aufw;
	if($aufw != 0)
		$aufw=1;

	// Max. Tage: 10 Jahre
	$mdays=max(0, min(3650, (int)$mdays));

	// Heutiges Datum
	$jetzt=fxNow();
	$heute=substr((string)$jetzt,0,8);

	// Zusatzdaten-Planungs-Projekte-Array
	$sql  = "SELECT";
	$sql .= " d.mandanten_id, d.maeintrittsdatum, d.maaustrittsdatum, d.wochenarbeitstd, d.arbeitstage, d.soll_beginn AS startzeit, d.pause,";
	$sql .= " d.sonntag AS a, d.montag AS b, d.dienstag AS c, d.mittwoch AS d, d.donnerstag AS e, d.freitag AS f, d.samstag AS g";
	if($proj_id)
	{
		$sql .= ", m.soll_beginn, m.soll_ende, m.auslastung, m.aufwand_std AS aufwand_sek, m.prozent,";
		$sql .= " p.projekt_status ";
	}
	else
	{
		$sql .= ", '".$heute."000000' AS soll_beginn, '".$heute."240000' AS soll_ende, 0 AS auslastung, 0 AS aufwand_sek, 0 AS prozent,";
		$sql .= " ".FXP_PS_ACTIVE." AS projekt_status ";
	}
	$sql .= "FROM";
	$sql .= " madaten d";
	if($proj_id)
		$sql .= ", maplanung m, projekte p ";
	else
		$sql .= " ";
	$sql .= "WHERE";
	if($proj_id)
	{
		$sql .= " m.personen_id=".$pers_id." AND m.projekt_id=".$proj_id." AND (m.sachmittel_id IS NULL OR m.sachmittel_id=0)";
		$sql .= " AND d.personen_id=m.personen_id AND p.projekt_id=m.projekt_id AND p.projektart=".FXP_PRJ_TASK;
		$sql .= " AND m.mandanten_id=".$mid." AND p.mandanten_id=".$mid;
	}
	else
		$sql .= " d.personen_id=".$pers_id;
	$sql .= " AND d.mandanten_id=".$mid;
	$pdt=db_values($sql);

	if(!is_array($pdt))
		return false;

	$tbd=0;
	for($i=0; $i<7; $i++)
	{
		$pdt[$i]=(int)$pdt[chr(0x61 + $i)];
		unset($pdt[chr(0x61 + $i)]);

		if($pdt[$i])
			$tbd++;
	}
//fxDebug($pdt, $sql);

	$psdate=substr((string)$pdt['soll_beginn'], 0,8);
	$pedate=substr((string)$pdt['soll_ende'], 0,8);

	// Summe Ist-Daten der Vergangenheit
	$sql  = "SELECT MIN(ist_beginn) AS istart, MAX(ist_ende) AS iende, SUM(zeitaufwand) AS aufwand_verbr";
	$sql .= " FROM zeitdaten";
	$sql .= " WHERE mandanten_id=".$mid." AND personen_id=".$pers_id." AND projekt_id=".$proj_id;
	$sql .= " AND zeitart IN (".FXP_TE_TIMEREC.",".FXP_TE_TIMEREC_SUM.",".FXP_TE_TRAVELEXP_TIMEREC.") AND ist_beginn<'".$heute."000000'";
	$idt=db_values($sql);
//fxDebug($idt, $sql);

	// Startdatum
	$sdate=$psdate;
	if(fxIsArray($idt) && !is_null($idt['istart']) && strlen((string)$idt['istart']))
		$sdate=min($sdate, substr((string)$idt['istart'], 0,8));
	if(strlen((string)$dsdate))
		$dsdate=substr((string)$dsdate, 0,8);
	else
		$dsdate=$sdate;
//echo('Startdatum: '.$sdate.' (Zeitstrahl: '.$dsdate.')<br />'.$nl);

	// Enddatum
	$edate=$pedate;
	if(fxIsArray($idt) && !is_null($idt['iende']) && strlen((string)$idt['iende']))
		$edate=max($edate, substr((string)$idt['iende'], 0,8));
	if(($pdt['projekt_status'] != FXP_PS_INACTIVE) && ($pdt['projekt_status'] != FXP_PS_COMPLETED))
		$edate=max($edate, $heute);
	$odedate='';
	if(strlen((string)$dedate))
	{
		$dedate=substr((string)$dedate,0,8);
		$odedate=$dedate;
	}
	else
		$dedate=$edate;
//echo('Enddatum: '.$edate.' (Zeitstrahl: '.$dedate.')<HR>'.$nl);

	// Zeitraum evtl. kürzen, fall zu groß
	$sea=get_datearray_range($sdate, $edate, $pdt['projekt_status'], $mdays);
	$sdate=$sea[0];
	$edate=$sea[1];
//echo('$sdate=['.$sdate.'], $edate=['.$edate.']<br />'.$nl);
	$sea=get_datearray_range($dsdate, $dedate, $pdt['projekt_status'], $mdays);
	$dsdate=$sea[0];
	$dedate=$sea[1];
//echo('$dsdate=['.$dsdate.'], $dedate=['.$dedate.']<br />'.$nl);
	$msdate=min($sdate, $dsdate);
	$medate=max($edate, $dedate);
//echo('$msdate=['.$msdate.'], $medate=['.$medate.']<br />'.$nl);
	$asdate=max($psdate,max($heute, $sdate));
	$aedate=max($pedate,max($heute, $edate));
//echo('$asdate=['.$asdate.'], $aedate=['.$aedate.']<HR>'.$nl);

	// Tagesarray
	$dar=weekdays($dsdate, $dedate);
	if(!is_array($dar))
		return false;

	// Sollstunden pro Tag
	$ssd=0.0;
	if($pdt['arbeitstage'] > 0)
	{
		$ssd=round($pdt['wochenarbeitstd']/$pdt['arbeitstage'], 2);
	}
	if($ssd == 0.0)
		$ssd=24.0;
//echo('Sollstunden pro Tag: '.$ssd.' h<br />'.$nl);

	// Restaufwand in Stunden
	$ra=0.0;
	if(($pdt['projekt_status'] != FXP_PS_INACTIVE) && ($pdt['projekt_status'] != FXP_PS_COMPLETED))
	{
		$ra=(double)$pdt['aufwand_sek'];
		if(is_array($idt))
			$ra -= (double)$idt['aufwand_verbr'];
		$ra=round(max(0.0, $ra/3600), 2);
//echo('Restaufwand in Stunden: '.$ra.' h<br />'.$nl);
	}

	// Spezialtage
	$feiertage		= $GLOBALS['feiertage'];
	$sondertage		= $GLOBALS['sondertage'];
	$termine		= $GLOBALS['termine'];
	$gleitzeiten	= $GLOBALS['gleitzeiten'];

	// Rest-Arbeitstage
	$rd=0.0;
	if(($ra > 0.0) && $tbd)
	{
		$rd=(double)countWeekdays($asdate, $aedate, $pdt[0], $pdt[1], $pdt[2], $pdt[3], $pdt[4], $pdt[5], $pdt[6]);
//echo('Restarbeitstage ohne Einschränkung: '.$rd.' d<br />'.$nl);

		// Spezialtage immer für einen längeren Zeitraum berechnen
		$ledate=max($medate, fx_date('Ymd', fx_mktime(12, 0, 0, (int)substr((string)$edate,4,2)+3, (int)substr((string)$edate,6,2), (int)substr((string)$edate,0,4))));
//echo('$ledate=['.$ledate.']<br />'.$nl);

		// Feiertage in zukünftiger Zeitspanne ermitteln
		if(!is_array($feiertage))
			$feiertage=get_feiertage($asdate, $ledate);
//fxDebug($feiertage, 'Feiertage: '.$asdate.' - '.$ledate);

		// Sondertage in zukünftiger Zeitspanne ermitteln
		if(!is_array($sondertage))
			$sondertage=get_sondertage($asdate, $ledate, $pers_id);
//fxDebug($sondertage, 'Sondertage ('.$pers_id.'): '.$asdate.' - '.$ledate);

		// Termine in zukünftiger Zeitspanne ermitteln
		if(!is_array($termine))
		{
			$terminart=FXP_AT_MEETING.','.FXP_AT_CONFERENCE.','.FXP_AT_BUSINESSDINNER.','.FXP_AT_PRIVATE.','.FXP_AT_VARIOUS;
			$termine=get_termine($asdate, $ledate, $pers_id, $terminart, $proj_id);
		}
//fxDebug($termine, 'Termine ('.$pers_id.'): '.$asdate.' - '.$ledate);

		// Gleitzeiten in zukünftiger Zeitspanne ermitteln
		if(!is_array($gleitzeiten))
		{
			$sql="SELECT ist_beginn FROM zeitdaten WHERE projekt_id=-1 AND personen_id=".$pers_id." AND mandanten_id=".$mid." AND (ist_beginn<='".$ledate."' AND ist_ende>='".$asdate."')";
			$tmp=db_values($sql, '*');
			if(is_array($tmp))
			{
				foreach($tmp as $t)
					$gleitzeiten[substr((string)$t['ist_beginn'],0,8)]=true;
			}
		}
//fxDebug($gleitzeiten, 'Gleitzeiten ('.$pers_id.'): '.$asdate.' - '.$ledate);

		if($rd > 0.0)
		{
			// Feiertag ?
			if(fxIsArray($feiertage))
			{
				foreach($feiertage as $dat => $d)
				{
					if($dat <= $aedate)
					{
						$wd=getWeekday($dat);
						if($pdt[$wd])
						{
							if($d['art'] == FXP_HL_FULL)		// Ganzer Feiertag
								$rd -= 1.0;
							else if($d['art'] == FXP_HL_HALF)	// Halber Feiertag
								$rd -= 0.5;
						}
					}
				}
			}
			// Gleittag ?
			if(is_array($gleitzeiten))
			{
				foreach($gleitzeiten as $dat => $d)
				{
					if($dat <= $aedate)
					{
						$wd=getWeekday($dat);
						if($pdt[$wd])
							$rd -= 1.0;
					}
				}
			}
			// Sondertag ?
			if(fxIsArray($sondertage))
			{
				foreach($sondertage as $dat => $d)
				{
					if($dat <= $aedate)
					{
						$wd=getWeekday($dat);
						if($pdt[$wd])
							$rd -= round($d[-1]['stunden_termin']/$ssd, 2);
					}
				}
			}
			// Termine ?
			if(fxIsArray($termine))
			{
				foreach($termine as $dat => $d)
				{
					if($dat <= $aedate)
					{
						$wd=getWeekday($dat);
						if($pdt[$wd])
							$rd -= round($d['stunden_termin']/$ssd, 2);
					}
				}
			}
		}
	}
	$rd=(int)max(0, ceil($rd));
//echo('Restarbeitstage: '.$rd.' d<br />'.$nl);

	// Restaufwand pro Arbeitstag
	$rad=(double)round($ra, 2);
	if(($ra > 0.0) && ($rd > 0.0))
		$rad=(double)round($ra/$rd, 2);
//echo('Restaufwand pro Arbeitstag: '.$rad.' h<br />'.$nl);

	// Max. Sollstunden pro Tag
	foreach($dar as $dat => $wd)
	{
		if(($dat < $asdate) || !$pdt[$wd])
		{
			$dar[$dat]=0.0;
			continue;
		}

		$sd=$ssd;
		// Feiertag ?
		if(($sd > 0.0) && fxIsArray($feiertage) && fxIsArray($feiertage[$dat]))
		{
			if($feiertage[$dat]['art'] == FXP_HL_FULL)		// Ganzer Feiertag
				$sd=0.0;
			else if($feiertage[$dat]['art'] == FXP_HL_HALF)	// Halber Feiertag
				$sd -= ($ssd/2);
		}
		// Gleittag ?
		if(($sd > 0.0) && fxIsArray($gleitzeiten) && isset($gleitzeiten[$dat]))
			$sd=0.0;
		// Sondertag ?
		if(($sd > 0.0) && fxIsArray($sondertage) && fxIsArray($sondertage[$dat]))
			$sd -= $sondertage[$dat][-1]['stunden_termin'];
		// Termine ?
		if(($sd > 0.0) && fxIsArray($termine) && fxIsArray($termine[$dat]))
		{
			foreach($termine[$dat] as $t)
				$sd -= $t['stunden_termin'];
		}
		$dar[$dat]=round(max(0.0, $sd), 2);
	}
//fxDebug($dar, 'Sollstunden - Letzter Wochentag='.$wd);

	// Detail Ist-Daten der Vergangenheit
	$ddt=array();
	$sql  = "SELECT ist_beginn AS istart, zeitaufwand AS aufwand_verbr";
	$sql .= " FROM zeitdaten";
	$sql .= " WHERE mandanten_id=".$mid." AND personen_id=".$pers_id." AND projekt_id=".$proj_id;
	$sql .= " AND zeitart IN (".FXP_TE_TIMEREC.",".FXP_TE_TIMEREC_SUM.",".FXP_TE_TRAVELEXP_TIMEREC.") AND zeitaufwand>0 AND ist_beginn>='".$sdate."000000' AND ist_beginn<'".$heute."000000'";
	$tmp=db_values($sql, '*');
//fxDebug($tmp, $sql);
	if(fxIsArray($tmp))
	{
		foreach($tmp as $t)
		{
			$cdate=substr((string)$t['istart'],0,8);
			$averb=(double)($t['aufwand_verbr']/3600);
			if(fxIsArray($ddt) && isset($ddt[$cdate]))
				$ddt[$cdate] += $averb;
			else
				$ddt[$cdate]=$averb;
		}
	}
//fxDebug($ddt, 'Ist-Daten als Array');

	// Auslastungswerte setzen
	foreach($dar as $dat => $dh)
	{
		// Ist-Daten
		if($dat < $heute)
		{
			if(fxIsArray($ddt) && $ddt[$dat])
			{
				$val=$ddt[$dat];
				if(!$aufw && ($val > 0.0))
					$val=round($val/$ssd, 2);
				$dar[$dat]=$val;
			}
			continue;
		}

		// Keine Soll-Daten
		if($dat < $asdate)
		{
			$dar[$dat]=0.0;
			continue;
		}

		// Kein Restaufwand mehr
		if($ra <= 0.0)
		{
			$dar[$dat]=0.0;
			continue;
		}

		$val=round(min($ra, min($dh, $rad)), 2);
		$ra =round(max(0.0, $ra-$val), 2);
		if(!$aufw && ($val > 0.0))
			$val=round($val/$ssd, 2);
		$dar[$dat]=$val;

		// Mehr Tage erforderlich
		if(($ra > 0.0) && ($dat == $dedate) && !strlen((string)$odedate))
		{
			$emc=0;
			while($ra > 0.0)
			{
				$dat=fx_date('Ymd', fx_mktime(12, 0, 0, (int)substr((string)$dat,4,2), (int)substr((string)$dat,6,2)+1, (int)substr((string)$dat,0,4)));

				$wd++;
				if($wd > 6)
					$wd=0;

				if(!$pdt[$wd])
					$val=0.0;
				else
				{
					$sd=$ssd;
					// Feiertag ?
					if(($sd > 0.0) && fxIsArray($feiertage) && fxIsArray($feiertage[$dat]))
					{
						if($feiertage[$dat]['art'] == FXP_HL_FULL)		// Ganzer Feiertag
							$sd=0.0;
						else if($feiertage[$dat]['art'] == FXP_HL_HALF)	// Halber Feiertag
							$sd -= ($ssd/2);
					}
					// Gleittag ?
					if(($sd > 0.0) && fxIsArray($gleitzeiten) && isset($gleitzeiten[$dat]))
						$sd=0.0;
					// Sondertag ?
					if(($sd > 0.0) && fxIsArray($sondertage) && fxIsArray($sondertage[$dat]))
						$sd -= $sondertage[$dat][-1]['stunden_termin'];
					// Termine ?
					if(($sd > 0.0) && fxIsArray($termine) && fxIsArray($termine[$dat]))
					{
						foreach($termine[$dat] as $t)
							$sd -= $t['stunden_termin'];
					}
					$sd=round(max(0.0, $sd), 2);

					$val=round(min($ra, min($sd, $rad)), 2);
					$ra =round(max(0.0, $ra-$val), 2);
					if(!$aufw && ($val > 0.0))
						$val=round($val/$ssd, 2);
				}
				$dar[$dat]=$val;

				$emc++;
				if($emc > 180)	// max. 1/2 Jahr zusätzlich
					break;
			}
		}
	}
	return $dar;
}

/**
 * $person:	Personen-Array
 * $pkey:	Index im Aufgaben-Array
 * $mode:	0=ohne Ist-Daten, 1=mit Ist-Daten
 *
 * @param various $person     - Mandatory parameter:                     ???PARAMETER???
 * @param various $pkey       - Mandatory parameter:                     ???PARAMETER???
 * @param array   $term_array - Optional parameter (default = array()):  ???PARAMETER???
 * @param integer $mode       - Optional parameter (default = 1):        ???PARAMETER???
 *
 * @return ???RETURN???
 */
function getUtilization($person, $pkey, $term_array=array(), $mode=1)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!is_array($person))
		return false;
	$pkey=(int)$pkey;
	if(!is_array($person['projekte'][$pkey]))
		return false;
	$pra=$person['projekte'][$pkey];

	$pers_id	= (int)$person['id'];
	$proj_id	= (int)$pra['projekt_id'];
	$mode		= (int)$mode;

	// Datum
	$jetzt=fxNow();
	$heute=substr((string)$jetzt,0,8);
	$sdate=substr((string)$pra['soll_beginn'],0,8);
	$edate=substr((string)$pra['ber_ende'],0,8);
//echo('$sdate=['.$sdate.'], $edate=['.$edate.']<br />'.$nl);

	// Summe Ist-Daten der Vergangenheit
	$iv=$pra['ist_sek'];
//echo('Bereits geleistete Stunden: '.round($iv/3600, 2).' h<br />');

	// Tagesarray
	$dar=weekdays($sdate, $edate);
	if(!is_array($dar))
		return false;
//fxDebug($dar, $sdate.' - '.$edate);

	// Sollstunden pro Tag
	$ssd=0.0;
	if($person['arbeitstage'] > 0)
		$ssd=round($person['wochenarbeitstd']/$person['arbeitstage'], 2);
	if($ssd == 0.0)
		$ssd=24.0;
//echo('Sollstunden pro Tag: '.$ssd.' h<br />'.$nl);

	// Tagesprozente, Sollstunden in Vergangenheit, Sollstunden in Zukunf inkl. Heute
	$ga=0.0;
	$gv=0.0;
	$gz=0.0;
	foreach($dar as $ddate => $dtype)
	{
		if(is_array($term_array) && sizeof($term_array) && is_array($term_array[$ddate]))
		{
			$percent=max(0.0, $term_array[$ddate]['h'])/$ssd;

			if($ddate < $heute)
				$gv += ($percent * $ssd);
			else
				$gz += ($percent * $ssd);

			$ga += ($percent * $ssd);
		}
		else
			$percent=0.0;
//echo($ddate.': $ga='.$ga.' h, $percent='.$percent.'<br />');
		$dar[$ddate]=$percent;
	}
//echo('Gesamtaufwand in Stunden: '.$ga.' h<br />davon in der Vergangenheit: '.$gv.' h<br />und in der Zukunft inkl. Heute: '.$gz.' h<br />'.$nl);
//fxDebug($dar, $sdate.' - '.$edate);

	// Aufwand in Stunden
	$ra=0.0;
	if(($pra['projekt_status'] != FXP_PS_INACTIVE) && ($pra['projekt_status'] != FXP_PS_COMPLETED))
	{
		// Geschätzten Fertigstellungsgrad berücksichtigen
		if($pra['fgrad'] > 0.0)
			$ra=round(max(0.0, ($pra['fgrad']-$iv)/3600), 4);
		else
			$ra=round(max(0.0, ($pra['aufwand_sek']-$iv)/3600), 4);
	}
//echo('Restaufwand in Stunden: '.$ra.' h<br />'.$nl);

	// Durchschnittsauslastung
	if($mode > 0)
	{
		if($gz)
			$aap=$ra/$gz;
		else
			$aap=0.0;
	}
	else
	{
		if($ga)
			$aap=$ra/$ga;
		else
			$aap=0.0;
	}
	$aa=$aap*$ssd;
//echo('Durchschnittsauslastung pro Tag: '.$aap.'='.$aa.' h<br />'.$nl);

	// Tagesauslastung
	$dar=array('sollstunden_pro_tag'=>$ssd, 'gesamtstunden'=>$ga, 'stunden_vergangenheit'=>$gv, 'stunden_zukunft'=>$gz, 'reststunden'=>$ra, 'delta_sollstunden'=>$aa, 'delta_auslastung'=>$aap, 'tages_sollstunden'=>$dar);
	foreach($dar['tages_sollstunden'] as $ddate => $dpercent)
	{
		if(($mode > 0) && ($ddate < $heute))
			$dar['tages_sollstunden'][$ddate]=0.0;
		else
			$dar['tages_sollstunden'][$ddate]=round($dpercent*$aa, 4);
	}
//fxDebug($dar, $sdate.' - '.$edate);

	return $dar;
}

/**
 * Beispielaufbau des Arrays (alle Einträge in Sekunden):
 * -----------------------------------------------------------------------------
 * 0		=> 900		0=Mindestpauseneintrag, hier 900 Sek.=15 Min.
 * 21600	=> 1800		ab 21600 Sek. (= 6 Std.) Arbeitszeit muss mind. eine
 * Pause von 1800 Sek. (= 30 Min.) gemacht werden
 * 32400	=> 2700		ab 32400 Sek. (= 9 Std.) Arbeitszeit muss mind. eine
 * Pause von 2700 Sek. (= 45 Min.) gemacht werden
 * ...
 *
 * @param integer $pers - Optional parameter (default = 0): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function time_break($pers=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$mid=$GLOBALS['fxpglobals']['client'];
	$pers=(int)$pers;
	if(!$pers)
		$pers=$GLOBALS['fxpglobals']['person'];

	$check=true;
	if(strtolower(substr((string)$GLOBALS['fxpglobals']['settings']['zd_pausencheck'],0,1)) == 'n')
		$check=false;
	else
	{
		$perstyp=(int)db_value("SELECT personentyp FROM personen WHERE personen_id=".$pers." AND mandanten_id=".$mid);
//fxDebug($perstyp,'perstyp von $pers='.$pers);
		if($perstyp != FXP_PT_EMPLOYEE)
			$check=false;
		else
		{
			$pause=(int)db_value("SELECT pause FROM madaten WHERE personen_id=".$pers." AND mandanten_id=".$mid);
//fxDebug($pause,'pause von $pers='.$pers);
			if(!$pause)
				$check=false;
		}
	}
//fxDebug($check,'$check');

	if($check)
	{
		// laut deutschem Arbeitszeitgesetz (ArbZG)
		$sek_array=array(0=>900, 21600=>1800, 32400=>2700);
	}
	else
		$sek_array=array(0=>0, 86400=>0);

	return $sek_array;
}

/**
 * Funktion zur Ermittlung des geplanten Stundensatzes je nach Kategorie
 * ---------------------------------------------------------------------
 * $type:		21=Personen + 66=Projekte
 * $id:			ID abhängig von $mode
 * $prjid:		Projekt-ID
 * $skill:		Zuerst nach dieser Fähigkeit suchen
 * ("f" am Ende für Fähigkeit, sonst Gruppe)
 *
 * @param various $type  - Mandatory parameter:                ???PARAMETER???
 * @param various $id    - Mandatory parameter:                ???PARAMETER???
 * @param integer $prjid - Optional parameter (default = 0):   ???PARAMETER???
 * @param string  $skill - Optional parameter (default = ''):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function calc_ppsp($type, $id, $prjid=0, $skill='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	//echo('<div style="border:1px solid black;padding:4px;background:#e1001a;"><b>calc_ekvk</B>($type='.$type.', $id='.$id.', $prjid='.$prjid.', $skill='.$skill.');</div>');
	// Return-Array
	$ekvk=array('ek'=>"", 'vk'=>"");

	// Parameter
	$mode	= (int)$mode;
	$prjid	= (int)$prjid;
	$cid	= $GLOBALS['fxpglobals']['client'];
	$sg		= 0;	// Fähigkeitsgruppe
	$sk		= 0;	// Fähigkeit

	// Prüfen, ob es diesen Typ gibt
	$sql="SELECT id FROM ekvk WHERE feldtyp=".$type." AND mandanten_id=".$cid;
	$mex=(int)db_value($sql);
//fxDebug($mex,$sql);
	if($mex)
	{
		// Fähigkeiten-IDs zuweisen, falls eine Fähigkeit als Parameter mitgegeben wurde
		if(strlen((string)$skill))
		{
			if(strtolower(substr((string)$skill,-1)) == 'f')
			{
				$sk=(int)$skill;
				$sg=(int)db_value("SELECT maart_id FROM ma_faehigkeit WHERE mafaehigkeit_id=".$sk." AND mandanten_id IN (0,".$cid.")");
			}
			else
				$sg=(int)$skill;
		}
//fxDebug($sg,'$sg: $skill='.$skill);
//fxDebug($sk,'$sk: $skill='.$skill);

		// EK/VK aus Zuordnungstabelle ermitteln
		$idt=explode(',',$id);
		$ids='';
		foreach($idt as $tid)
		{
			$tid=(int)trim((string)$tid);
			if(strlen((string)$ids))
				$ids .= ",";
			$ids .= $tid;
		}

		$pra=array();
		$sks=$sk;
		$sgs=$sg;
		$ssf=false;
		while(true)
		{
			$sql="SELECT id, ek, vk FROM ekvk WHERE id IN (".$ids.") AND feldtyp=".$type." AND mafaehigkeit_id=".$sk." AND maart_id=".$sg." AND mandanten_id=".$cid;
			$tmp=db_values($sql,'*');
//fxDebug($tmp,$sql);
			if(fxIsArray($tmp))
			{
				$ida=array();
				foreach($tmp as $t)
				{
					if(strlen((string)$t['ek']) || strlen((string)$t['vk']))
					{
						$tid=(int)$t['id'];
						if(!isset($ida[$tid]))
							$ida[$tid]=array('ek'=>$t['ek'], 'vk'=>$t['vk']);
						else
						{
							if(!strlen((string)$ida[$tid]['ek']) && strlen((string)$t['ek']))
								$ida[$tid]['ek']=$t['ek'];
							if(!strlen((string)$ida[$tid]['vk']) && strlen((string)$t['vk']))
								$ida[$tid]['vk']=$t['vk'];
						}
					}
				}
				if(sizeof($ida))
				{
//fxDebug($ida,$ids);
					$cpp=0;
					$csp=0;
					$spp=0.0;
					$ssp=0.0;
					foreach($ida as $i)
					{
						if(strlen((string)$i['ek']))
						{
							$cpp++;
							$spp += $i['ek'];
						}
						if(strlen((string)$i['vk']))
						{
							$csp++;
							$ssp += $i['vk'];
						}
					}
					if(!strlen((string)$ekvk['ek']) && $cpp)
						$ekvk['ek']=get_double($spp/$cpp);
					if(!strlen((string)$ekvk['vk']) && $csp)
						$ekvk['vk']=get_double($ssp/$csp);

					if($sk || $sg)
						$ssf=true;
				}
			}
			if($sk)
				$sk=0;
			else if($sg)
				$sg=0;
			else if($type == 66)	// Bei Projekten auch Elter durchsuchen
			{
				$sk=$sks;
				$sg=$sgs;
				$sql="SELECT elter FROM projekte WHERE projekt_id IN (".$ids.") AND mandanten_id=".$cid;
				$tmp=db_values($sql,'*');
				if(fxIsArray($tmp))
				{
					$ids='';
					foreach($tmp as $t)
					{
						$eid=(int)$t['elter'];
						if(($eid > 0) && !isset($pra[$eid]))
						{
							$pra[$eid]=true;
							if(strlen((string)$ids))
								$ids .= ",";
							$ids .= $eid;
						}
					}
					if(!strlen((string)$ids))
						break;
				}
				else
					break;
			}
			else
				break;
		}

		if(!$ssf && ($sks || $sgs))
			$ekvk=array('ek'=>"", 'vk'=>"");
	}

	return $ekvk;
}

/**
 * ???FUNCTION???
 *
 * @param various $pid              - Mandatory parameter:                     ???PARAMETER???
 * @param string  $sdate            - Optional parameter (default = ''):       ???PARAMETER???
 * @param string  $edate            - Optional parameter (default = ''):       ???PARAMETER???
 * @param array   $inf              - Optional parameter (default = array()):  ???PARAMETER???
 * @param boolean $with_workedhours - Optional parameter (default = false):    ???PARAMETER???
 *
 * @return ???RETURN???
 */
function getExtPersInfo($pid, $sdate='', $edate='', $inf=array(), $with_workedhours=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!is_array($inf))
		$inf=array();

	$pid=(int)$pid;
	if(!$pid)
		return $inf;

	if(!sizeof($inf) || !is_array($inf['data']))
		$inf['data']=get_persondata($pid);

	if(!isset($inf['medatum']))
	{
		if(($inf['data']['maeintrittsdatum'] !== NULL) && strlen((string)$inf['data']['maeintrittsdatum']))
			$inf['medatum']=substr((string)$inf['data']['maeintrittsdatum'], 0,8);
		else
			$inf['medatum']='00000000';
	}
	if(!isset($inf['madatum']))
	{
		if(($inf['data']['maaustrittsdatum'] !== NULL) && strlen((string)$inf['data']['maaustrittsdatum']))
			$inf['madatum']=substr((string)$inf['data']['maaustrittsdatum'], 0,8);
		else
			$inf['madatum']='99999999';
	}

	if(!isset($inf['sdate']))
	{
		$inf['sdate']='';
		$inf['edate']='';
	}
	if(!isset($inf['days']))
		$inf['days']=array();

	if(strlen((string)$sdate))
	{
		$sdate=substr((string)$sdate,0,8);
		if(!strlen((string)$edate))
			$edate=$sdate;
		else
			$edate=substr((string)$edate,0,8);
//echo('$sdate='.$sdate.', $edate='.$edate.'<br />');

		$timespan=array();
		if(!strlen((string)$inf['sdate']))
		{
			$timespan[]		= array('sdate'=>$sdate, 'edate'=>$edate);
			$inf['sdate']	= $sdate;
			$inf['edate']	= $edate;
		}
		else if(($sdate < $inf['sdate']) || ($edate > $inf['edate']))
		{
			if($sdate < $inf['sdate'])
			{
				$timespan[]		= array('sdate'=>$sdate, 'edate'=>fx_date('Ymd', fx_mktime(12, 0, 0, (int)substr((string)$inf['sdate'],4,2), (int)substr((string)$inf['sdate'],6,2)-1, (int)substr((string)$inf['sdate'],0,4))));
				$inf['sdate']	= $sdate;
			}
			if($edate > $inf['edate'])
			{
				$timespan[]		= array('sdate'=>fx_date('Ymd', fx_mktime(12, 0, 0, (int)substr((string)$inf['edate'],4,2), (int)substr((string)$inf['edate'],6,2)+1, (int)substr((string)$inf['edate'],0,4))), 'edate'=>$edate);
				$inf['edate']	= $edate;
			}
		}
//fxDebug($timespan, '$timespan');

		if(sizeof($timespan))
		{
			foreach($timespan as $tsdate)
			{
				// Arbeitstage, Feiertage, Sondertage + Termine
				$app_array=weekdays($tsdate['sdate'], $tsdate['edate']);
//fxDebug($app_array, '$app_array');
				if(is_array($app_array) && sizeof($app_array))
				{
					if(!isset($GLOBALS['holidays']))
						$GLOBALS['holidays']=get_feiertage($tsdate['sdate'], $tsdate['edate']);
					$specialdays	= get_sondertage($tsdate['sdate'], $tsdate['edate'], $pid);
					$appdays		= array();;
					$terminart		= FXP_AT_MEETING.','.FXP_AT_CONFERENCE.','.FXP_AT_BUSINESSDINNER.','.FXP_AT_PRIVATE.','.FXP_AT_VARIOUS;
					$appdays		= get_termine($tsdate['sdate'], $tsdate['edate'], $pid, $terminart, -1);
					if($with_workedhours)
						$workeddays=getWorkedDays($tsdate['sdate'], $tsdate['edate'], $pid);
					else
						$workeddays=array();
//fxDebug($GLOBALS['holidays'], 'Holidays: '.$tsdate['sdate'].' - '.$tsdate['edate']);
//fxDebug($specialdays, 'Specialdays ('.$pid.'): '.$tsdate['sdate'].' - '.$tsdate['edate']);
//fxDebug($appdays, 'AppDays ('.$pid.'): '.$tsdate['sdate'].' - '.$tsdate['edate']);
//fxDebug($workeddays, 'WorkedDays ('.$pid.'): '.$tsdate['sdate'].' - '.$tsdate['edate']);

					foreach($app_array as $adate => $aday)
					{
						$app_array[$adate]=array('h'=>0.0, 'ws'=>0.0, 'wd'=>0);

						if(sizeof($workeddays) && isset($workeddays[$adate]))
							$app_array[$adate]['ws']=$workeddays[$adate][-1];

						if($inf['data'][$aday] && ($adate >= $inf['medatum']) && ($adate <= $inf['madatum']))	// Arbeitstag
						{
							$hd=$inf['data']['sollstunden_tag'];
							$app_array[$adate]['wd']=1;
						}
						else																					// Frei oder noch nicht angestellt
							$hd=0.0;

						// Feiertag?
						if(is_array($GLOBALS['holidays']) && sizeof($GLOBALS['holidays']) && is_array($GLOBALS['holidays'][$adate]))
						{
							$app_array[$adate]['hd']=$GLOBALS['holidays'][$adate];
							$hd -= $GLOBALS['holidays'][$adate]['typ']*$inf['data']['sollstunden_tag'];
						}

						// Sondertag?
						if(is_array($specialdays) && sizeof($specialdays) && is_array($specialdays[$adate]))
						{
							$app_array[$adate]['sd']=$specialdays[$adate];
							$hd -= $specialdays[$adate][-1]['stunden_termin'];
						}

						// Termin?
						if(is_array($appdays) && sizeof($appdays) && is_array($appdays[$adate]))
						{
							$app_array[$adate]['ap']=array(-1 => array('art'=>0, 'stunden_termin'=>0.0));
							foreach($appdays[$adate] as $startts => $appinfo)
							{
								if(!$app_array[$adate]['ap'][-1]['art'])
									$app_array[$adate]['ap'][-1]['art']=$appinfo['art'];
								$app_array[$adate]['ap'][-1]['stunden_termin'] += $appinfo['stunden_termin'];
								$hd -= $appinfo['stunden_termin'];

								$app_array[$adate]['ap'][$startts]=$appinfo;
							}
						}

						$app_array[$adate]['h']=max(0.0, $hd);

						$inf['days'][$adate]=$app_array[$adate];
					}
				}
			}
			ksort($inf['days']);
		}
	}

//fxDebug($inf, '$inf');
	return $inf;
}

/**
 * ermittelt wieviel "netto" Tage Urlaub eine übergebene Zeitspanne tatsächlich
 * entspricht
 * -----------------------------------------------------------------------------
 * $pinfo:	Personen-ID oder Rückgabearray der Funktion get_persondata()
 * falls leer, dann wird aktuelle Personen-ID verwendet
 *
 * @param various $sdate - Mandatory parameter:               ???PARAMETER???
 * @param various $edate - Mandatory parameter:               ???PARAMETER???
 * @param integer $pinfo - Optional parameter (default = 0):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function getNetVacation($sdate, $edate, $pinfo=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!is_array($pinfo))
	{
		if(!$pinfo)
			$pinfo=$GLOBALS['fxpglobals']['person'];
		$pinfo=get_persondata((int)$pinfo);
	}

	// Datumsteil
	$sdate=substr((string)$sdate,0,8);
	$edate=substr((string)$edate,0,8);

	// Anzahl Arbeitstage
	$cwdays=countWeekdays($sdate, $edate, $pinfo[0], $pinfo[1], $pinfo[2], $pinfo[3], $pinfo[4], $pinfo[5], $pinfo[6]);

	// Anzahl ganzer und halber Feiertage
	$cwhdays=0;
	$chhdays=0;
	$sql  = "SELECT datum, halbtaegig FROM feiertage_mand WHERE mandanten_id=".(int)$pinfo['mandanten_id'];
	$sql .= " AND datum>='".$sdate."000000' AND datum<='".$edate."240000' AND (halbtaegig=".FXP_HL_FULL." OR halbtaegig=".FXP_HL_HALF.")";
	$fta=db_values($sql);
	if(is_array($fta))
	{
		foreach($fta as $f)
		{
			$fwd=getWeekday($f['datum']);
			if($pinfo[$fwd])
			{
				if($f['halbtaegig'] == FXP_HL_FULL)
					$cwhdays++;
				else
					$chhdays++;
			}
		}
	}

	// Anzahl Urlaubstage
	$vdays=max(0.0, (double)($cwdays - $cwhdays - ($chhdays*0.50)));
	return $vdays;
}

/**
 * ???FUNCTION???
 *
 * @param integer $pid       - Optional parameter (default = 0):     ???PARAMETER???
 * @param string  $ssd       - Optional parameter (default = ''):    ???PARAMETER???
 * @param string  $sed       - Optional parameter (default = ''):    ???PARAMETER???
 * @param string  $type      - Optional parameter (default = 'st'):  ???PARAMETER???
 * @param boolean $extended  - Optional parameter (default = false): ???PARAMETER???
 * @param boolean $ret_array - Optional parameter (default = false): ???PARAMETER???
 * @param integer $set_debug - Optional parameter (default = 0):     ???PARAMETER???
 *
 * @return ???RETURN???
 */
function getPersonWorkdata($pid=0, $ssd='', $sed='', $type='st', $extended=false, $ret_array=false, $set_debug=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$debug_pwd=(int)$set_debug;
	if($debug_pwd)
		echo('<hr>getPersonWorkdata('.$pid.', '.$ssd.', '.$sed.')<br /><br />');

	$mid=$GLOBALS['fxpglobals']['client'];
	$pid=(int)$pid;
	if(!$pid)
		$pid=$GLOBALS['fxpglobals']['person'];

	if(!fxIsArray($GLOBALS['pers_wd']) || !fxIsArray($GLOBALS['pers_wd'][$pid]))
	{
		$e=array('cl'=>false, 'sd'=>"00000000", 'ed'=>"99999999", 'zs'=>"000000", 'zm'=>"000000", 'zn'=>"000000", 'ze'=>"000000", 'zp'=>0.0, 'sw'=>0.0, 'st'=>0.0, 'su'=>0.0, 'at'=>0, 'so'=>0, 'mo'=>0, 'di'=>0, 'mi'=>0, 'do'=>0, 'fr'=>0, 'sa'=>0, 'mu'=>array());
		$a=array('*'=>$e, '00000000'=>$e);
		// Contract data
		$sql  = "SELECT maeintrittsdatum, maaustrittsdatum, wochenarbeitstd, soll_beginn, pause, arbeitstage, montag, dienstag, mittwoch, donnerstag, freitag, samstag, sonntag FROM madaten";
		$sql .= " WHERE personen_id=".$pid." AND mandanten_id=".$mid;
		$mdt=db_values($sql);
		if($debug_pwd > 1)
			fxDebug($mdt,$sql, 0);
		if(fxIsArray($mdt))
		{
			$sd='00000000';
			if($mdt['maeintrittsdatum'] !== NULL)
				$sd=substr((string)$mdt['maeintrittsdatum'],0,8);
			$a[$sd]=$e;
			$ed='99999999';
			if($mdt['maaustrittsdatum'] !== NULL)
			{
				$ed=substr((string)$mdt['maaustrittsdatum'],0,8);
				$d=fx_date("Ymd", fx_mktime(12,0,0, (int)substr((string)$ed,4,2), (int)substr((string)$ed,6,2)+1, (int)substr((string)$ed,0,4)));
				$a[$d]=$e;
			}

			$sql="SELECT zeitstempel,ist_beginn,zeitaufwand FROM zeitdaten WHERE personen_id=".$pid." AND mandanten_id=".$mid." AND zeitart=105";
			$tmp=db_values($sql);
			if($debug_pwd > 1)
				fxDebug($tmp,$sql);
			if(fxIsArray($tmp))
			{
				$i=substr((string)$tmp['ist_beginn'],0,8);
				$d=max($sd,fx_date("Ymd", fx_mktime(12,0,0, (int)substr((string)$i,4,2)+1, 1, (int)substr((string)$i,0,4))));
				$u=array($d => $tmp['zeitaufwand']);
				if($debug_pwd)
					fxDebug($u,'$u');

				$sql  = "SELECT zeitstempel,ist_beginn,zeitaufwand FROM zeitdaten_history";
				$sql .= " WHERE personen_id=".$pid." AND mandanten_id=".$mid." AND zeitart=105 AND aktion=2 AND ist_beginn<'".$tmp['ist_beginn']."' ORDER BY zeitstempel DESC";
				$tmp=db_values($sql,'*');
				if($debug_pwd > 1)
					fxDebug($tmp,$sql, 0);
				if(fxIsArray($tmp))
				{
					$ue='';
					foreach($tmp as $t)
					{
						$i=substr($t['ist_beginn'],0,8);
						if(!strlen($ue) || ($i < $ue))
						{
							$ue=$i;
							$d=max($sd,fx_date("Ymd", fx_mktime(12,0,0, (int)substr($i,4,2)+1, 1, (int)substr($i,0,4))));
							if(!isset($u[$d]))
								$u[$d]=$t['zeitaufwand'];
						}
					}
				}
				if($debug_pwd)
					fxDebug($u,'$u', 0);

				if(sizeof($u))
				{
					foreach($u as $d => $z)
					{
						if(!isset($a[$d]))
							$a[$d]=$e;
						$a[$d]['su']=$z;
					}
				}
			}

			ksort($a);
			$w='';
			foreach($a as $d => $da)
			{
				if(strlen((string)$d) == 8)
				{
					$a[$d]['sd']=$d;
					if(strlen((string)$w))
						$a[$w]['ed']=fx_date("Ymd", fx_mktime(12,0,0, (int)substr((string)$d,4,2), (int)substr((string)$d,6,2)-1, (int)substr((string)$d,0,4)));
					$w=$d;
				}
			}
			$stp=0.0;
			foreach($a as $d => $da)
			{
				if((strlen((string)$d) == 8) && ($d >= $sd) && ($d <= $ed))
				{
					$a[$d]['zs']=substr((string)$mdt['soll_beginn'],8);
					$a[$d]['zp']=$mdt['pause']/3600;
					$a[$d]['sw']=$mdt['wochenarbeitstd'];
					$a[$d]['at']=$mdt['arbeitstage'];
					$a[$d]['so']=$mdt['sonntag'];
					$a[$d]['mo']=$mdt['montag'];
					$a[$d]['di']=$mdt['dienstag'];
					$a[$d]['mi']=$mdt['mittwoch'];
					$a[$d]['do']=$mdt['donnerstag'];
					$a[$d]['fr']=$mdt['freitag'];
					$a[$d]['sa']=$mdt['samstag'];

					if($da['ed'] < $ed)
					{
						$st=0.0;

						$sql  = "SELECT soll_st_tag FROM termine";
						$sql .= " WHERE personen_id=".$pid." AND mandanten_id=".$mid." AND soll_st_tag IS NOT NULL AND startzeit>='".$a[$d]['sd']."000000' AND startzeit<='".$a[$d]['ed']."240000' ORDER BY startzeit DESC";
						$tmp=db_values($sql,'*');
						if($debug_pwd > 1)
							fxDebug($tmp,$sql);
						if(fxIsArray($tmp))
						{
							$sta=array();
							foreach($tmp as $t)
							{
								$s='D'.get_double($t['soll_st_tag']);
								if(!isset($sta[$s]))
									$sta[$s]=1;
								else
									$sta[$s]++;
							}
							if($debug_pwd > 1)
								fxDebug($sta,'$sta');
							$sc=0;
							foreach($sta as $s => $c)
							{
								if($c > $sc)
								{
									$st=get_double(substr((string)$s,1));
									$sc=$c;
								}
							}
						}
						if($debug_pwd)
							fxDebug($st,'$st: '.$d);

						$sql  = "SELECT zeitstempel, aktion, wochenarbeitstd, soll_beginn, pause, arbeitstage, montag, dienstag, mittwoch, donnerstag, freitag, samstag, sonntag FROM madaten_history";
						$sql .= " WHERE personen_id=".$pid." AND mandanten_id=".$mid." AND zeitstempel<='".$a[$d]['ed']."240000' AND aktion IN (2,3) ORDER BY zeitstempel DESC";
						$tmp=db_values($sql);
						if($debug_pwd > 1)
							fxDebug($tmp,$sql);
						if(fxIsArray($tmp))
						{
							$a[$d]['zs']=substr((string)$tmp['soll_beginn'],8);
							$a[$d]['zp']=$tmp['pause']/3600;
							$a[$d]['at']=$tmp['arbeitstage'];
							$a[$d]['so']=$tmp['sonntag'];
							$a[$d]['mo']=$tmp['montag'];
							$a[$d]['di']=$tmp['dienstag'];
							$a[$d]['mi']=$tmp['mittwoch'];
							$a[$d]['do']=$tmp['donnerstag'];
							$a[$d]['fr']=$tmp['freitag'];
							$a[$d]['sa']=$tmp['samstag'];

							if(!$st)
								$a[$d]['sw']=$tmp['wochenarbeitstd'];
						}

						if(!$st && $stp)
							$st=$stp;

						$a[$d]['sw']=$st*$a[$d]['at'];
					}
					else
						$a[$d]['cl']=true;

					$at=(int)$a[$d]['at'];
					if($at > 0)
						$a[$d]['st']=round(($a[$d]['sw']*100.0)/$at)/100.0;
					if($a[$d]['st'] > 0.0)
						$stp=$a[$d]['st'];

					$a[$d]['zm']=substr((string)time_add($d.$a[$d]['zs'], $a[$d]['st']/2, 'H'), 8);
					$a[$d]['zn']=substr((string)time_add($d.$a[$d]['zm'], $a[$d]['zp'], 'H'), 8);
					$a[$d]['ze']=substr((string)time_add($d.$a[$d]['zn'], $a[$d]['st']/2, 'H'), 8);

					if($a[$d]['cl'])
					{
						$a['*']=$a[$d];
						$a['*']['cl']=false;
					}
				}
			}

			// Monatsüberträge ermitteln
			if($extended)
			{
			}

		}

		// Person data
		$pdt=getPersonData($pid);
//fxDebug($pdt,'$pdt', 0);
		$a['*']['pt']=(int)$pdt['personentyp'];
		$a['*']['sx']=(int)$pdt['geschlecht'];
		$a['*']['es']=substr((string)$pdt['maeintrittsdatum'],0,8);
		$a['*']['ex']=trim((string)$pdt['maaustrittsdatum']);
		$a['*']['cn']=$pdt['vorname'];
		$a['*']['sn']=$pdt['pname'];
		$a['*']['n1']=$pdt['vn'];
		$a['*']['n2']=$pdt['nv'];

		if(strlen((string)$a['*']['ex']) > 7)
			$a['*']['ex']=substr((string)$a['*']['ex'],0,8);
		else
			$a['*']['ex']='99999999';

		if(!fxIsArray($GLOBALS['pers_wd']))
			$GLOBALS['pers_wd']=array();
		$GLOBALS['pers_wd'][$pid]=$a;

		if($debug_pwd)
			fxDebug($GLOBALS['pers_wd'][$pid], '$GLOBALS[\'pers_wd\']['.$pid.']');
	}

	if($ret_array)
		return $GLOBALS['pers_wd'][$pid];

	$st='';
	$wd='';
	$ssd=substr(trim((string)$ssd),0,8);
	$sed=substr(trim((string)$sed),0,8);
	if(!strlen((string)$sed))
		$sed=$ssd;
	if(strlen((string)$ssd) && fxIsArray($GLOBALS['pers_wd'][$pid]))
	{
		foreach($GLOBALS['pers_wd'][$pid] as $d => $da)
		{
			if(strlen((string)$d) == 8)
			{
				if($debug_pwd)
					echo('Block ['.$ssd.'-'.$sed.'] in ['.$da['sd'].'-'.$da['ed'].']<br />');
				if(($sed >= $da['sd']) && ($ssd <= $da['ed']))
				{
					$st=$da['st'];
					if(isset($da[$type]))
						$wd=$da[$type];
					if($debug_pwd)
						echo('&nbsp;&middot;&nbsp;<b>TRUE</b>: $st='.$st.', $wd='.$wd.'<br />');
					break;
				}
				else if($debug_pwd)
					echo('&nbsp;&middot;&nbsp;<font class="grey">FALSE</font><br />');
			}
		}
	}

	if(!strlen((string)$st))
	{
		if(fxIsArray($GLOBALS['pers_wd'][$pid]) && fxIsArray($GLOBALS['pers_wd'][$pid]['*']))
		{
			$st=$GLOBALS['pers_wd'][$pid]['*']['st'];
			if($debug_pwd)
				echo('<b class="blue">Default verwenden</b>: $st='.$st.'<br />');
		}
		else
			$st=0.0;
	}

	if(!strlen((string)$wd))
	{
		if(fxIsArray($GLOBALS['pers_wd'][$pid]) && fxIsArray($GLOBALS['pers_wd'][$pid]['*']) && isset($GLOBALS['pers_wd'][$pid]['*'][$type]))
		{
			$wd=$GLOBALS['pers_wd'][$pid]['*'][$type];
			if($debug_pwd)
				echo('<b class="blue">Default verwenden</b>: $wd='.$wd.'<br />');
		}
		else
			$wd=0.0;
	}

	if($debug_pwd)
		echo('<hr><b>Ermittelte Sollstunden/Tag:</b> -&gt; '.$st.', <b>Ermittelte Wochentag ['.$type.']:</b> -&gt; '.$wd.'<br />');

	if($type == 'st')
		return $st;
	return array('st'=>$st, 'wd'=>$wd);
}

/**
 * Ist-Stunden pro Tag einer Person zurückliefern
 *
 * @param string  $sdate - Optional parameter (default = ''): ???PARAMETER???
 * @param string  $edate - Optional parameter (default = ''): ???PARAMETER???
 * @param integer $pers  - Optional parameter (default = 0):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function getWorkedDays($sdate='',$edate='',$pers=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Return array
	$wdays=array();

	// Person-ID
	$pers=(int)$pers;
	if(!$pers)
		$pers=$GLOBALS['fxpglobals']['person'];

	// Start date
	$sdate=substr(trim((string)$sdate),0,8);
	$sdl=strlen((string)$sdate);
	if($sdl == 4)
		$sdate .= '0101';
	else if($sdl != 8)
		return $wdays;

	// End date
	$edate=substr(trim((string)$edate),0,8);
	$edl=strlen((string)$edate);
	if($edl == 4)
		$edate .= '1231';
	else if(!$edl)
	{
		if($sdl == 4)
			$edate=substr((string)$sdate,0,4).'1231';
		else
			$edate=$sdate;
	}
	else if($edl < 8)
		return $wdays;

	// Date syntax
	$tsdate=$sdate.'000000';
	$tedate=$edate.'240000';
//echo('<b>getActDays</b> sdate='.$sdate.', $edate='.$edate.', $pers='.$pers.'<br />');

	// Get worked hours
	$sql  = "SELECT zeit_id,projekt_id,ist_beginn,ist_ende,zeitaufwand FROM zeitdaten";
	$sql .= " WHERE personen_id=".$pers." AND mandanten_id=".$GLOBALS['fxpglobals']['client']." AND zeitart IN (".FXP_TE_TIMEREC.",".FXP_TE_TIMEREC_SUM.",".FXP_TE_TRAVELEXP_TIMEREC.") AND zeitaufwand>0 AND projekt_id>=0";
	$sql .= " AND ist_ende>='".$tsdate."' AND ist_beginn<='".$tedate."'";
	$sql .= " ORDER BY ist_beginn";
	$ta=db_values($sql,'*');
	if(fxIsArray($ta))
	{
//fxDebug($ta,$sql);
		foreach($ta as $t)
		{
			$d=substr((string)$t['ist_beginn'],0,8);
			if(!isset($wdays[$d]))
				$wdays[$d]=array();
			if(!isset($wdays[$d][-1]))
				$wdays[$d][-1]=$t['zeitaufwand'];
			else
				$wdays[$d][-1] += $t['zeitaufwand'];
			if(!isset($wdays[$d][$t['projekt_id']]))
				$wdays[$d][$t['projekt_id']]=$t['zeitaufwand'];
			else
				$wdays[$d][$t['projekt_id']] += $t['zeitaufwand'];
		}
	}

	return $wdays;
}

/**
 * ???FUNCTION???
 *
 * @param various $m  - Mandatory parameter:                ???PARAMETER???
 * @param various $p  - Mandatory parameter:                ???PARAMETER???
 * @param various $ds - Mandatory parameter:                ???PARAMETER???
 * @param string  $de - Optional parameter (default = ''):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function okTimeRegistration($m, $p,$ds,$de='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$m=(int)$m;
	$mm=$m&1;
	$ma=$m&2;
	$s=strtolower(substr((string)$GLOBALS['fxpglobals']['settings']['ze_an_abwtage'],0,1));
//fxDebug($s,'$s');
	if($s == 'n')
	{
		$p=(int)$p;
		$ds=substr((string)$ds,0,8);
		if(strlen((string)$de))
			$de=substr((string)$de,0,8);
		else
			$de=$ds;

		if($mm)	// Check if absent day is possible
		{
			$ad='';
			$da=array();
			// ...Get time recordings
			$sql  = "SELECT zeit_id,ist_beginn,ist_ende";
			$sql .= " FROM zeitdaten WHERE personen_id=".$p." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$sql .= " AND ist_beginn<='".$de."240000' AND ist_ende>='".$ds."000000'";
			$sql .= " AND zeitart IN (".FXP_TE_TIMEREC.",".FXP_TE_TIMEREC_SUM.",".FXP_TE_TRAVELEXP_TIMEREC.") AND zeitaufwand>0 AND projekt_id>=0";
			$sql .= " ORDER BY ist_beginn";
			$aa=db_values($sql,'*');
//fxDebug($aa,$sql);
			if(fxIsArray($aa))
			{
				foreach($aa as $a)
					$da[substr((string)$a['ist_beginn'],0,8)]=true;
			}
			// ...Get material usage recordings
			$sql  = "SELECT id,ist_beginn,ist_ende";
			$sql .= " FROM materialverbrauch WHERE personen_id=".$p." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$sql .= " AND ist_beginn<='".$de."240000' AND ist_ende>='".$ds."000000' AND sachmenge>0 AND projekt_id>=0";
			$sql .= " ORDER BY ist_beginn";
			$aa=db_values($sql,'*');
//fxDebug($aa,$sql);
			if(fxIsArray($aa))
			{
				foreach($aa as $a)
					$da[substr((string)$a['ist_beginn'],0,8)]=true;
			}
			if(sizeof($da))
				ksort($da, SORT_STRING);
//fxDebug($da,'$da');
			if($ma)
				return $da;
			if(sizeof($da))
			{
				foreach($da as $d => $i)
				{
					if(strlen((string)$ad))
						$ad .= '<br />';
					$ad .= '&middot; <b>'.get_entry($d,'datum').'</b>';
				}
			}
//echo('<hr>$ad='.$ad.'<hr>');
			return $ad;
		}
		else	// Check if time or material usage recording is possible
		{
			$ad='';
			$da=array();
			$sql  = "SELECT termin_id,startzeit,endzeit,soll_st_tag,soll_beginn";
			$sql .= " FROM termine WHERE personen_id=".$p." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$sql .= " AND startzeit<='".$de."240000' AND endzeit>='".$ds."000000'";
			$sql .= " AND terminart IN (".FXP_AT_VACATIONREQUEST.",".FXP_AT_VACATION.",".FXP_AT_SPECIALVACATIONREQUEST.",".FXP_AT_SPECIALVACATION.",".FXP_AT_TRAININGREQUEST.",".FXP_AT_TRAINING.",".FXP_AT_SICKCALL.",".FXP_AT_SICK.",".FXP_AT_SLIDINGDAYREQUEST.",".FXP_AT_SLIDINGDAY.",".FXP_AT_ABSENT.")";
			$aa=db_values($sql,'*');
//fxDebug($aa,$sql);
			if(fxIsArray($aa))
			{
				foreach($aa as $a)
				{
					$sd=substr((string)$a['startzeit'],0,8);
					$ed=substr((string)$a['endzeit'],0,8);
					$ok=true;
					if($sd != $ed)
						$ok=false;
					else if(substr((string)$a['startzeit'],8,6) == substr((string)$a['soll_beginn'],8,6))
					{
						$ee=substr((string)$a['endzeit'],8,6);
						$ec=secToTime(timeToSec(substr((string)$a['startzeit'],8,6)) + (int)($a['soll_st_tag']*3600));
//echo('$ee='.$ee.', $ec='.$ec.'<br />');
						if($ee >= $ec)
							$ok=false;
					}
					if(!$ok)
					{
						$td=$sd;
						while($td <= $ed)
						{
							if(($td >= $ds) && ($td <= $de))
								$da[$td]=true;
							$td=substr((string)time_add($td,1,'D'),0,8);
						}
					}
				}
			}
			if(sizeof($da))
				ksort($da, SORT_STRING);
//fxDebug($da,'$da');
			if($ma)
				return $da;
			if(sizeof($da))
			{
				foreach($da as $d => $i)
				{
					if(strlen((string)$ad))
						$ad .= '<br />';
					$ad .= '&middot; <b>'.get_entry($d,'datum').'</b>';
				}
			}
//echo('<hr>$ad='.$ad.'<hr>');
			return $ad;
		}
	}

	if($ma)
		return array();
	return '';
}

/**
 * ???FUNCTION???
 *
 * @param various $pers   - Mandatory parameter:                ???PARAMETER???
 * @param string  $sqladd - Optional parameter (default = ''):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function getExtendedPersonInfos($pers, $sqladd='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	//echo('<div style="background:#006b9f;color:#ffffff;padding:6px;border-radius:8px;margin-bottom:2px;">getExtendedPersonInfos($pers='.$pers.')</div>');

	// Client ID
	$cid=$GLOBALS['fxpglobals']['client'];
	// Person(s)
	$pers=trim((string)$pers);
	if(strlen((string)$pers))
	{
		$cp=strpos($pers,',');
		if(!$cp)
		{
			$pers=(int)$pers;
			if($pers > 0)
				$cp=1;
		}
		if($cp)
			$pers=(string)$pers;
	}
	// SQL
	$sqladd=trim((string)$sqladd);

	// Get infos for all selected persons
	$apers='';
	$pa=array();
	$sql="SELECT personen_id,personentyp,pname,vorname,geschlecht,abt_id,inaktiv FROM personen WHERE personen_id";
	if(strlen((string)$pers))
		$sql .= " IN (".$pers.")";
	else
		$sql .= ">0";
	if(strlen((string)$sqladd))
	{
		if(strtoupper(substr((string)$sqladd,0,3)) == 'AND')
			$sqladd=trim(substr((string)$sqladd,3));
		$sql .= " AND (".$sqladd.")";
	}
	$sql .= " AND mandanten_id=".$cid." ORDER BY pname,vorname";
	$tmp=db_values($sql,'*');
//fxDebug($tmp,$sql);
	if(fxIsArray($tmp))
	{
		foreach($tmp as $t)
		{
			$id=$t['personen_id'];
			$pa[$id]=array(
				'type'=>$t['personentyp'],'sex'=>$t['geschlecht'],'name'=>$t['pname'],'uname'=>$t['pname'],'sname'=>$t['pname'],'cname'=>$t['vorname'],
				'departmentid'=>$t['abt_id'],'inactive'=>$t['inaktiv'],
				'entrydate'=>'00000000','leavedate'=>'99999999',
				'weeklyhrs'=>0.0, 'dailyhrs'=>0.0, 'workdays'=>0,
				'workstarttime'=>'000000', 'workbreaktime'=>'120000', 'workendtime'=>'240000', 'workbreakhrs'=>0.0,
				'pp'=>0.0, 'sp'=>0.0,
				'su'=>0,'mo'=>0,'tu'=>0,'we'=>0,'th'=>0,'fr'=>0,'sa'=>0,
				'set'=>false,
				'tasks'=>array(),
				'sdays'=>array()
			);
			if(strlen((string)$t['vorname']))
				$pa[$id]['name'] .= ', '.$t['vorname'];

			if(strlen((string)$apers))
				$apers .= ",";
			$apers .= $id;
		}
	}
//fxDebug($pa,$sql);
//echo('$apers='.$apers.'<hr>');

	// Get unique names
	if(sizeof($pa))
	{
		$sql="SELECT personen_id,pname,vorname FROM personen WHERE mandanten_id=".$cid;
		$tmp=db_values($sql,'*');
//fxDebug($tmp,$sql);
		if(fxIsArray($tmp))
		{
			$da=array();
			foreach($tmp as $tc => $t)
			{
				$id=strtolower((string)$t['pname']);
				if(!isset($da[$id]))
					$da[$id]=array();
				$da[$id][$t['personen_id']]=$t['vorname'];
			}
//fxDebug($da,'$da');
			// Find multi used names
			if(sizeof($da))
			{
				foreach($da as $n => $a)
				{
					if(sizeof($a) > 1)
					{
//fxDebug($a,$n);
						$uc=1;
						foreach($a as $id => $v)
						{
							if(isset($pa[$id]))
							{
								$v1=substr((string)$v,0,1);
								if(!strlen((string)$v1))
								{
									$v1=$uc;
									$uc++;
								}
								$pa[$id]['uname'] .= ' '.$v1;
							}
						}
					}
				}
			}
		}
	}

	// Get additional person infos
	if(strlen((string)$apers))
	{
		$ha=array('su','mo','tu','we','th','fr','sa');

		$sql  = "SELECT personen_id,maeintrittsdatum,maaustrittsdatum,wochenarbeitstd,arbeitstage,soll_beginn,pause,";
		$sql .= "sonntag AS su,montag AS mo,dienstag AS tu,mittwoch AS we,donnerstag AS th,freitag AS fr,samstag AS sa FROM madaten";
		$sql .= " WHERE personen_id IN (".$apers.") AND mandanten_id=".$cid;
		$tmp=db_values($sql,'*');
//fxDebug($tmp,$sql);
		if(fxIsArray($tmp))
		{
			foreach($tmp as $t)
			{
				$id=$t['personen_id'];
				if(isset($pa[$id]))
				{
					if(($t['maeintrittsdatum'] !== NULL) && strlen((string)$t['maeintrittsdatum']))
						$pa[$id]['entrydate']=substr((string)$t['maeintrittsdatum'], 0,8);
					else
						$pa[$id]['entrydate']='00000000';
					if(($t['maaustrittsdatum'] !== NULL) && strlen((string)$t['maaustrittsdatum']))
						$pa[$id]['leavedate']=substr((string)$t['maaustrittsdatum'],0,8);
					else
						$pa[$id]['leavedate']='99999999';
					$pa[$id]['weeklyhrs']=round($t['wochenarbeitstd'],4);
					if(($t['soll_beginn'] !== NULL) && strlen((string)$t['soll_beginn']))
						$pa[$id]['workstarttime']=substr((string)$t['soll_beginn'],8,6);
					else
						$pa[$id]['workstarttime']='080000';
					$pa[$id]['workbreakhrs']=round($t['pause']/3600,4);
					foreach($ha as $hd)
					{
						$pa[$id][$hd]=(int)$t[$hd];
						if($pa[$id][$hd])
							$pa[$id]['workdays']++;
					}
					if($pa[$id]['workdays'] > 0)
						$pa[$id]['dailyhrs']=round($pa[$id]['weeklyhrs']/$pa[$id]['workdays'],4);
					$pa[$id]['workbreaktime']=secToTime(timeToSec($pa[$id]['workstarttime']) + $pa[$id]['dailyhrs']*1800);
					$pa[$id]['workendtime']=secToTime(timeToSec($pa[$id]['workstarttime']) + ($pa[$id]['dailyhrs']+$pa[$id]['workbreakhrs'])*3600);
				}
			}
		}

		$sql  = "SELECT personen_id, ek, vk FROM artikel";
		$sql .= " WHERE (sachmittel_id IS NULL OR sachmittel_id=0) AND personen_id IN (".$apers.") AND (archiv IS NULL OR archiv=0) AND mandanten_id=".$cid;
		$tmp=db_values($sql,'*');
//fxDebug($tmp,$sql);
		if(fxIsArray($tmp))
		{
			foreach($tmp as $t)
			{
				$id=$t['personen_id'];
				if(isset($pa[$id]))
				{
					$pa[$id]['pp']=$t['ek'];
					$pa[$id]['sp']=$t['vk'];
				}
			}
		}
	}
//fxDebug($pa,$sql);

	return $pa;
}

/**
 * ???FUNCTION???
 *
 * @param various $pa    - Mandatory parameter:                   ???PARAMETER???
 * @param various $sdt   - Mandatory parameter:                   ???PARAMETER???
 * @param string  $edt   - Optional parameter (default = ''):     ???PARAMETER???
 * @param boolean $total - Optional parameter (default = false):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function getPersonSpecialDays($pa,$sdt,$edt='',$total=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	//echo('<div style="background:#006b9f;color:#ffffff;padding:6px;border-radius:8px;margin-bottom:2px;">getPersonSpecialDays($pa='.$pa.', $sdt='.$sdt.', $edt='.$edt.', $total='.$total.')</div>');

	// Client ID
	$cid=$GLOBALS['fxpglobals']['client'];
	// Person(s)
	$single=0;
	if(!is_array($pa))
	{
		$cp=strpos($pa,',');
		if(!$cp)
		{
			$pa=(int)$pa;
			if($pa > 0)
			{
				$cp=1;
				$single=$pa;
			}
		}
		if($cp)
			$pa=(string)$pa;
		$pa=getExtendedPersonInfos($pa);
	}
	else if(!sizeof($pa))
		$pa=getExtendedPersonInfos(0);
	// Start + End
	$sdt=substr((string)$sdt,0,8);
	if(strlen((string)$edt))
		$edt=substr((string)$edt,0,8);
	else
		$edt=$sdt;

	// Get ID's
	$pers='';
	if(fxIsArray($pa))
	{
		foreach($pa as $id => $a)
			$pers .= ",".$id;
		$pers=substr((string)$pers,1);
	}
//echo('$single='.$single.'<br />$pers='.$pers.'<hr>');

	if(strlen((string)$pers))
	{
		// Month days
		$mda=array(1=>31,28,31,30,31,30,31,31,30,31,30,31);
		// Weekdays
		$ha=array('su','mo','tu','we','th','fr','sa');

		$sql  = "SELECT personen_id,termin_id,terminart,startzeit,endzeit,soll_st_tag,soll_beginn FROM termine";
		$sql .= " WHERE personen_id IN (".$pers.") AND mandanten_id=".$cid." AND (startzeit<='".$edt."240000' AND endzeit>='".$sdt."000000')";
		$sql .= " AND terminart IN (".FXP_AT_ABSENT.",".FXP_AT_SLIDINGDAYREQUEST.",".FXP_AT_SLIDINGDAY.",".FXP_AT_VACATIONREQUEST.",".FXP_AT_VACATION.",".FXP_AT_SPECIALVACATIONREQUEST.",".FXP_AT_SPECIALVACATION.",";
		$sql .= FXP_AT_TRAININGREQUEST.",".FXP_AT_TRAINING.",".FXP_AT_SICKCALL.",".FXP_AT_SICK.")";
		$sql .= " ORDER BY personen_id,startzeit";
		$tmp=db_values($sql,'*');
//fxDebug($tmp,$sql);
		if(fxIsArray($tmp))
		{
			foreach($tmp as $t)
			{
				$pid=$t['personen_id'];
				$atp=$t['terminart'];
				$asd=substr((string)$t['startzeit'],0,8);
				$aed=substr((string)$t['endzeit'],0,8);
				$ass=timeToSec($t['startzeit']);
				$aes=timeToSec($t['endzeit']);
				$whd=$pa[$pid]['dailyhrs'];
				if($t['soll_st_tag'] > 0.0)
					$whd=$t['soll_st_tag'];
				$wbt=$pa[$pid]['workbreaktime'];
				if(strlen((string)$t['soll_beginn']))
					$wbt=secToTime(timeToSec($t['soll_beginn']) + $whd*1800);

				$sy=(int)substr((string)$asd,0,4);
				$sm=(int)substr((string)$asd,4,2);
				$sd=(int)substr((string)$asd,6,2);
				$wd=fx_date('w',fx_mktime(12,0,0, $sm,$sd,$sy));
				$ly=fx_date('L',fx_mktime(12,0,0, 1,1,$sy));
				$mda[2]=28;
				if($ly)
					$mda[2]=29;
				while(true)
				{
					$dt=sprintf("%04d%02d%02d", $sy,$sm,$sd);
					if($dt > $aed)
						break;

					if(!isset($pa[$pid]['sdays'][$dt]))
						$pa[$pid]['sdays'][$dt]=array('typeAM'=>0, 'typePM'=>0, 'dur'=>0.0, 'apps'=>array());

					if($dt == $asd)
						$sss=$ass;
					else
						$sss=0;
					if($dt == $aed)
						$ess=$aes;
					else
						$ess=86400;
					$dur=min($whd,round(($ess-$sss)/3600,4));

					$ats=secToTime($sss).'-'.secToTime($ess);
					$pa[$pid]['sdays'][$dt]['apps'][$ats]=array('type'=>$atp,'dur'=>$dur);

					$ntam=0;
					$ntpm=0;
					if($dur >= $whd)
					{
						$ntam=$atp;
						$ntpm=$atp;
					}
					else
					{
						$sst=secToTime($sss);
						if($sst < $wbt)
							$ntam=$atp;
						else
							$ntpm=$atp;
					}
					if(($ntam > 0) && !$pa[$pid]['sdays'][$dt]['typeAM'])
						$pa[$pid]['sdays'][$dt]['typeAM']=$ntam;
					if(($ntpm > 0) && !$pa[$pid]['sdays'][$dt]['typePM'])
						$pa[$pid]['sdays'][$dt]['typePM']=$ntpm;

					$pa[$pid]['sdays'][$dt]['dur']=min($pa[$pid]['sdays'][$dt]['dur']+$dur,$whd);

					$wd++;
					if($wd > 6)
						$wd=0;
					$sd++;
					if($sd > $mda[$sm])
					{
						$sd=1;
						$sm++;
						if($sm > 12)
						{
							$sm=1;
							$sy++;
							$ly=fx_date('L',fx_mktime(12,0,0, 1,1,$sy));
							$mda[2]=28;
							if($ly)
								$mda[2]=29;
						}
					}
				}
			}
		}

		// Get flextime
		$sql  = "SELECT personen_id,ist_beginn FROM zeitdaten WHERE projekt_id=-1 AND personen_id IN (".$pers.")";
		$sql .= " AND (ist_beginn<='".$edt."240000' AND ist_ende>='".$sdt."000000') AND mandanten_id=".$cid;
		$tmp=db_values($sql,'*');
//fxDebug($tmp,$sql);
		if(fxIsArray($tmp))
		{
			foreach($tmp as $t)
			{
				$pid=$t['personen_id'];
				$dt=substr((string)$t['ist_beginn'],0,8);

				if(!isset($pa[$pid]['sdays'][$dt]))
					$pa[$pid]['sdays'][$dt]=array('typeAM'=>0, 'typePM'=>0, 'dur'=>0.0, 'apps'=>array());

				if(!$pa[$pid]['sdays'][$dt]['typeAM'])
					$pa[$pid]['sdays'][$dt]['typeAM']=FXP_AT_SLIDINGDAY;
				if(!$pa[$pid]['sdays'][$dt]['typePM'])
					$pa[$pid]['sdays'][$dt]['typePM']=FXP_AT_SLIDINGDAY;

				$pa[$pid]['sdays'][$dt]['dur']=max($pa[$pid]['sdays'][$dt]['dur'],$pa[$pid]['dailyhrs']);

				$ats=$pa[$pid]['workstarttime'].'-'.$pa[$pid]['workendtime'];
				$pa[$pid]['sdays'][$dt]['apps'][$ats]=array('type'=>FXP_AT_SLIDINGDAY,'dur'=>$pa[$pid]['dailyhrs']);
			}
		}
	}

	if($total)
		return $pa;

	if($single)
		return $pa[$single]['sdays'];

	$sa=array();
	if(fxIsArray($pa))
	{
		foreach($pa as $id => $a)
			$sa[$id]=$a['sdays'];
	}
	return $sa;
}

/**
 * ???FUNCTION???
 *
 * @param various $pa    - Mandatory parameter:                   ???PARAMETER???
 * @param various $prjs  - Mandatory parameter:                   ???PARAMETER???
 * @param various $sdt   - Mandatory parameter:                   ???PARAMETER???
 * @param string  $edt   - Optional parameter (default = ''):     ???PARAMETER???
 * @param boolean $total - Optional parameter (default = false):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function getPersonTasks($pa,$prjs,$sdt,$edt='',$total=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	//echo('<div style="background:#006b9f;color:#ffffff;padding:6px;border-radius:8px;margin-bottom:2px;">getPersonTasks($pa='.$pa.', $prjs='.$prjs.', $sdt='.$sdt.', $edt='.$edt.', $total='.$total.')</div>');

	// Client ID
	$cid=$GLOBALS['fxpglobals']['client'];

	// Start + End
	$sdt=substr((string)$sdt,0,8);
	if(strlen((string)$edt))
		$edt=substr((string)$edt,0,8);
	else
		$edt=$sdt;

	// Person(s)
	$single=0;
	if(!is_array($pa))
	{
		$cp=strpos($pa,',');
		if(!$cp)
		{
			$pa=(int)$pa;
			if($pa > 0)
			{
				$cp=1;
				$single=$pa;
			}
		}
		if($cp)
			$pa=(string)$pa;
		$pa=getPersonSpecialDays($pa,$sdt,$edt,true);
	}
	else if(!sizeof($pa))
		$pa=getPersonSpecialDays(0,$sdt,$edt,true);

	// Get ID's
	$pers='';
	if(fxIsArray($pa))
	{
		foreach($pa as $id => $a)
			$pers .= ",".$id;
		$pers=substr((string)$pers,1);
	}
//echo('$single='.$single.'<br />$pers='.$pers.'<hr>');

	if(!isset($GLOBALS['ta']))
		$GLOBALS['ta']=array();
	if(!isset($GLOBALS['poaa']))
		$GLOBALS['poaa']=array();

	// Get assigned tasks
	if(strlen((string)$pers))
	{
		// Today
		$tdt=substr((string)$GLOBALS['datetime'],0,8);
//echo('$tdt='.$tdt.'<br />');

		// Max assign day
		$mdt=fx_date('Ymd', fx_mktime(12, 0, 0, (int)substr((string)$tdt,4,2), (int)substr((string)$tdt,6,2)+28, (int)substr((string)$tdt,0,4)));
//echo('$mdt='.$mdt.'<br />');

		$sql  = "SELECT m.personen_id,m.projekt_id,m.soll_beginn,m.soll_ende,m.aufwand_std,m.auslastung,";
		$sql .= " p.vorgangsnummer,p.name_projekt,p.projekt_status,p.kategorie_id,p.ref_nummer,p.aufwand_soll,p.aufwand_ist,p.port";
		$sql .= " FROM maplanung m, projekte p";
		$sql .= " WHERE m.personen_id IN (".$pers.")";
		if(strlen((string)$prjs))
			$sql .= " AND m.projekt_id IN (".$prjs.")";
		$sql .= " AND m.soll_beginn<='".$edt."240000' AND m.mandanten_id=".$cid;
		if(!$GLOBALS['pil'])	// No piled up tasks
			$sql .= " AND m.soll_ende>='".$sdt."000000'";
		if(!$GLOBALS['mnt'])	// No maintenance tasks
			$sql .= " AND m.aufwand_std>0.0";
		$sql .= " AND p.projekt_id=m.projekt_id AND p.projekt_status=".FXP_PS_ACTIVE." AND p.mandanten_id=".$cid;
		$sql .= " ORDER BY p.name_projekt,m.soll_beginn,p.vorgangsnummer";
		$tmp=db_values($sql,'*');
//fxDebug($tmp,$sql);
		if(fxIsArray($tmp))
		{
			foreach($tmp as $t)
			{
				$id=$t['personen_id'];
				if(fxIsArray($pa[$id]))
				{
					$tid=$t['projekt_id'];

					$pa[$id]['tasks'][$tid]=array(
						'plan'=>true,
						'psdt'=>substr((string)$t['soll_beginn'],0,8),'pedt'=>substr((string)$t['soll_ende'],0,8),'redt'=>substr((string)$t['soll_ende'],0,8),
						'pstatus'=>$t['projekt_status'],
						'peffort'=>round($t['aufwand_soll']/3600,4), 'reffort'=>round($t['aufwand_std']/3600,4), 'rpercent'=>$t['auslastung'],
						'deffort'=>0.0
					);
					if($pa[$id]['tasks'][$tid]['redt'] < $mdt)
						$pa[$id]['tasks'][$tid]['redt']=$mdt;

					// Planned effort per day
					if($pa[$id]['tasks'][$tid]['reffort'] > 0.0)
					{
						$wdt=countWeekdays($pa[$id]['tasks'][$tid]['psdt'],$pa[$id]['tasks'][$tid]['pedt'], $pa[$id]['su'], $pa[$id]['mo'], $pa[$id]['tu'], $pa[$id]['we'], $pa[$id]['th'], $pa[$id]['fr'], $pa[$id]['sa']);
						if($wdt > 0)
							$pa[$id]['tasks'][$tid]['deffort']=round($pa[$id]['tasks'][$tid]['reffort']/$wdt,4);
//echo($tid.': '.$pa[$id]['tasks'][$tid]['reffort'].', $wdt='.$wdt.' -> '.$pa[$id]['tasks'][$tid]['deffort'].'<br />');
					}

					if(!isset($GLOBALS['ta'][$tid]))
						$GLOBALS['ta'][$tid]=poaTask($t);
				}
			}
		}

		// Get assigned plan of action tasks
		$sql  = "SELECT m.id,m.personen_id,m.projekt_id,m.soll_beginn,m.soll_ende,m.aufwand_std,0.0 AS auslastung,m.anmerkung,";
		$sql .= " p.vorgangsnummer,p.name_projekt,p.projekt_status,p.kategorie_id,p.ref_nummer,p.aufwand_soll,p.aufwand_ist,p.port";
		$sql .= " FROM maeinsatz m, projekte p";
		$sql .= " WHERE m.personen_id IN (".$pers.") AND m.sachmittel_id=0 AND m.soll_beginn<='".$edt."240000' AND m.soll_ende>='".$sdt."000000' AND m.mandanten_id=".$cid;
		$sql .= " AND p.projekt_id=m.projekt_id AND p.mandanten_id=".$cid;
		$tmp=db_values($sql,'*');
//fxDebug($tmp,$sql);
		if(fxIsArray($tmp))
		{
			foreach($tmp as $t)
			{
				$id=$t['personen_id'].'_'.$t['projekt_id'].'_'.substr((string)$t['soll_beginn'],0,8);
				$GLOBALS['poaa'][$id]=array(
					'id'=>$t['id'],
					'pid'=>$t['personen_id'], 'tid'=>$t['projekt_id'], 'adt'=>substr((string)$t['soll_beginn'],0,8),
					'st'=>substr((string)$t['soll_beginn'],8,6), 'et'=>substr((string)$t['soll_ende'],8,6),
					'eh'=>$t['aufwand_std'],
					'note'=>$t['anmerkung']
				);

				$id=$t['personen_id'];
				if(fxIsArray($pa[$id]))
				{
					$tid=$t['projekt_id'];
					if(!isset($pa[$id]['tasks'][$tid]))
					{
						$pa[$id]['tasks'][$tid]=array(
							'plan'=>false,
							'psdt'=>substr((string)$t['soll_beginn'],0,8),'pedt'=>substr((string)$t['soll_ende'],0,8),'redt'=>substr((string)$t['soll_ende'],0,8),
							'pstatus'=>$t['projekt_status'],
							'peffort'=>round($t['aufwand_soll']/3600,4), 'reffort'=>round($t['aufwand_soll']/3600,4), 'rpercent'=>$t['auslastung'],
							'deffort'=>round($t['aufwand_std'],4)
						);
						if($pa[$id]['tasks'][$tid]['redt'] < $mdt)
							$pa[$id]['tasks'][$tid]['redt']=$mdt;

						if(!isset($GLOBALS['ta'][$tid]))
							$GLOBALS['ta'][$tid]=poaTask($t);
					}
					else if(!$pa[$id]['tasks'][$tid]['plan'])
					{
						$pa[$id]['tasks'][$tid]['psdt']=min($pa[$id]['tasks'][$tid]['psdt'],substr((string)$t['soll_beginn'],0,8));
						$pa[$id]['tasks'][$tid]['pedt']=max($pa[$id]['tasks'][$tid]['pedt'],substr((string)$t['soll_ende'],0,8));
						$pa[$id]['tasks'][$tid]['redt']=max($pa[$id]['tasks'][$tid]['redt'],$pa[$id]['tasks'][$tid]['pedt']);
					}
				}
			}
		}
	}
//fxDebug($pa,'$pa');
//fxDebug($GLOBALS['ta'],'$GLOBALS[ta]');
//fxDebug($GLOBALS['poaa'],'$GLOBALS[poaa]');

	if($total)
		return $pa;

	if($single)
		return $pa[$single]['tasks'];

	$sa=array();
	if(fxIsArray($pa))
	{
		foreach($pa as $id => $a)
			$sa[$id]=$a['tasks'];
	}
	return $sa;
}

/**
 * ???FUNCTION???
 *
 * @param integer $cid - Optional parameter (default = 0):  ???PARAMETER???
 * @param string  $did - Optional parameter (default = ''): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function getDepartments($cid=0, $did='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!$cid)
		$cid=$GLOBALS['fxpglobals']['client'];
	$a=array();

	$sql="SELECT abt_id,zu_abteilung,abt_ebene,abt_name,abt_hperson FROM abteilung WHERE mandanten_id=".$cid." ORDER BY abt_ebene,abt_name";
	$tmp=db_values($sql,'*');
	if(fxIsArray($tmp))
	{
		$l=0;
		$aa=array(0=>'');
		while(sizeof($aa))
		{
			$na=array();
			// Fetch all entries with same department levels
			foreach($aa as $aid => $acnt)
			{
				$cnt=1;
				foreach($tmp as $an => $av)
				{
					if($av['zu_abteilung'] == $aid)
					{
						$ablv=$acnt.substr('000'.$cnt,-3);
						$a[$ablv]=array('departmentid'=>$av['abt_id'], 'level'=>$av['abt_ebene'], 'name'=>$av['abt_name'], 'leader'=>$av['abt_hperson']);

						$na[$av['abt_id']]=$ablv;
						$cnt++;

						$tmp[$an]['zu_abteilung']=-666;
					}
				}
			}

			$aa=$na;
			$level++;
		}
		ksort($a,SORT_STRING);
	}

	$did=trim((string)$did);
	if(!strlen((string)$did))
		return $a;

	// Return CSV
	$did=(int)$did;
	$c='';
	$s='';
	$l=0;
	if(sizeof($a))
	{
		foreach($a as $dk => $da)
		{
			$id=0;
			if(($did <= 0) || ($l && substr((string)$dk,0,$l) == $s))
				$id=$da['departmentid'];
			else if($da['departmentid'] == $did)
			{
				$id=$da['departmentid'];
				$s=$dk;
				$l=strlen((string)$s);
			}
			if($id > 0)
			{
				if(strlen((string)$c))
					$c .= ',';
				$c .= $id;
			}
		}
	}

	if(!strlen((string)$c))
		$c='-1';

	return $c;
}

/**
 * ???FUNCTION???
 *
 * @param integer $cid - Optional parameter (default = 0): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function getTeams($cid=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!$cid)
		$cid=$GLOBALS['fxpglobals']['client'];
	$a=array();

	$sql  = "SELECT w.wertetabid,w.tabwert,p.personen_id,p.leiter FROM projektteam p, wertetab w";
	$sql .= " WHERE p.mandanten_id=".$cid." AND w.mandanten_id=p.mandanten_id AND w.wertetabid=p.wertetabid AND w.id_sprache=".$GLOBALS['fxpglobals']['lang'];
	$sql .= " ORDER BY w.tabwert,p.leiter DESC,p.personen_id";
	$tmp=db_values($sql,'*');
//fxDebug($tmp,$sql);

	if(fxIsArray($tmp))
	{
		foreach($tmp as $t)
		{
			$tn=$t['tabwert'];
			$tp=$t['personen_id'];
			$tl=0;
			if($t['leiter'])
				$tl=$tp;
			if(!isset($a[$tn]))
				$a[$tn]=array('id'=>$t['wertetabid'], 'leader'=>0, 'team'=>array());
			if(!$a[$tn]['leader'] && $tl)
				$a[$tn]['leader']=$tl;
			if(!$tl)
				$a[$tn]['team'][$tp]=true;
		}
	}
//fxDebug($a,'$a');

	return $a;
}

/**
 * ???FUNCTION???
 *
 * @param various $rl       - Mandatory parameter:               ???PARAMETER???
 * @param integer $excluded - Optional parameter (default = 0):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function getRolePersons($rl, $excluded=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$rpa=array();

	// -5=Förderprojektverwaltung, -4=Budget-Genehmigung, -3=Ressourcen-Planung, -2=Projektmanagement, -1=Verwaltung, 1=Projektleitung + 2=Geschäftsleitung
	$sql  = "SELECT p.personen_id, p.pname, p.vorname, p.geschlecht, f.mafaehigkeit_id FROM ma_art_faehigkeiten f LEFT JOIN personen p ON (p.personen_id=f.personen_id AND p.mandanten_id=f.mandanten_id)";
	$sql .= " WHERE f.mafaehigkeit_id IN (".$rl.") AND f.maart_id=1 AND f.personen_id<>".$excluded." AND f.mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$sql .= " AND (p.inaktiv IS NULL OR p.inaktiv=0)";
	$tmp=db_values($sql,'*');
//fxDebug($tmp,$sql);
	if(fxIsArray($tmp))
	{
		foreach($tmp as $t)
			$rpa[$t['personen_id']]=$t;
	}

	return $rpa;
}

/**
 * ???FUNCTION???
 *
 * @param various $pid - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function persIsAbsent($pid)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$abs_app=FXP_AT_ABSENT.",".FXP_AT_VACATIONREQUEST.",".FXP_AT_VACATION.",".FXP_AT_SPECIALVACATIONREQUEST.",".FXP_AT_SPECIALVACATION.",".FXP_AT_TRAININGREQUEST.",".FXP_AT_TRAINING.",".FXP_AT_SICKCALL.",".FXP_AT_SICK.",".FXP_AT_SLIDINGDAYREQUEST.",".FXP_AT_SLIDINGDAY;

	$sql  = "SELECT termin_id FROM termine";
	$sql .= " WHERE personen_id=".$pid." AND terminart IN (".$abs_app.")";
	$sql .= " AND (startzeit<='".$GLOBALS['datetime']."' AND endzeit>='".$GLOBALS['datetime']."') AND mandanten_id=".$GLOBALS['fxpglobals']['client'];

	$id=(int)db_value($sql);
	if($id)
		return true;
	return false;
}

/**
 * Get person data as an array
 *
 * @param integer $pid        - Mandatory parameter:                    Person id
 * @param string  $empty_date - Optional parameter (default = 'NULL'):  Replacement string if person's exit date is empty
 *
 * @return Array with person's data
 */
function getPersonData($pid, $empty_date='NULL')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$pid=(int)$pid;
	if(!$pid)
		$pid=$GLOBALS['fxpglobals']['person'];

	if(!fxIsArray($GLOBALS['__pda']))
		$GLOBALS['__pda']=array();
	if(!fxIsArray($GLOBALS['__pda'][$pid]))
	{
		$sql  = "SELECT";
		$sql .= " p.personen_id, p.personentyp, p.elter, p.pname, p.vorname, p.personen_initialen, p.name_zusatz, p.anrede, p.perstitel, p.pposition, p.geschlecht, p.einsatzort, p.inaktiv,";
		$sql .= " m.geburtsname, m.geburtsdatum, m.religion, m.maeintrittsdatum, m.maaustrittsdatum, m.soll_beginn, m.pause, m.wochenarbeitstd, m.arbeitstage,";
		$sql .= " m.montag, m.dienstag, m.mittwoch, m.donnerstag, m.freitag, m.samstag, m.sonntag ";
		$sql .= "FROM";
		$sql .= " personen p LEFT JOIN madaten m ON (m.personen_id=p.personen_id AND m.mandanten_id=p.mandanten_id) ";
		$sql .= "WHERE";
		$sql .= " p.personen_id=".$pid." AND p.mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$GLOBALS['__pda'][$pid]=db_values($sql);
//fxDebug($pdat,$sql, 0);

		if(($GLOBALS['__pda'][$pid]['maaustrittsdatum'] === NULL) || !strlen((string)$GLOBALS['__pda'][$pid]['maaustrittsdatum']))
			$GLOBALS['__pda'][$pid]['maaustrittsdatum']=$empty_date;

		if($GLOBALS['__pda'][$pid]['inaktiv'])
			$GLOBALS['__pda'][$pid]['inaktiv']=1;
		else
			$GLOBALS['__pda'][$pid]['inaktiv']=0;

		$GLOBALS['__pda'][$pid]['pname']=trim((string)$GLOBALS['__pda'][$pid]['pname']);
		$GLOBALS['__pda'][$pid]['inaktiv']=0;

		if(($GLOBALS['__pda'][$pid]['vorname'] !== NULL) && strlen((string)$GLOBALS['__pda'][$pid]['vorname']))
		{
			$GLOBALS['__pda'][$pid]['vn']=$GLOBALS['__pda'][$pid]['vorname'].' '.$GLOBALS['__pda'][$pid]['pname'];
			$GLOBALS['__pda'][$pid]['nv']=$GLOBALS['__pda'][$pid]['pname'].', '.$GLOBALS['__pda'][$pid]['vorname'];
		}
		else
		{
			$GLOBALS['__pda'][$pid]['vn']=$GLOBALS['__pda'][$pid]['pname'];
			$GLOBALS['__pda'][$pid]['nv']=$GLOBALS['__pda'][$pid]['pname'];
		}
	}

	return $GLOBALS['__pda'][$pid];
}

/**
 * ???FUNCTION???
 *
 * @param various $mpa     - Mandatory parameter:                   ???PARAMETER???
 * @param various $setting - Mandatory parameter:                   ???PARAMETER???
 * @param integer $pers    - Optional parameter (default = 0):      ???PARAMETER???
 * @param boolean $pca     - Optional parameter (default = false):  ???PARAMETER???
 * @param string  $sdate   - Optional parameter (default = ''):     ???PARAMETER???
 * @param string  $edate   - Optional parameter (default = ''):     ???PARAMETER???
 *
 * @return ???RETURN???
 */
function getMailPersons($mpa, $setting, $pers=0, $pca=false, $sdate='', $edate='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!$pers)
		$pers=$GLOBALS['fxpglobals']['person'];

	// Superior?
	$tpa=getMailSuperior(array(), $setting, $pers);
	if(sizeof($tpa))
	{
		foreach($tpa as $pid => $val)
			$mpa[$pid]=$val;
	}
	// Project manager or 1st or 2nd project leader?
	$tpa=getMailProjectResponsibles(array(), $setting, $pers, $pca, $sdate, $edate);
	if(sizeof($tpa))
	{
		foreach($tpa as $pid => $val)
			$mpa[$pid]=$val;
	}
	// Resource planner?
	$tpa=getMailResourceplaner(array(), $setting, $pers);
	if(sizeof($tpa))
	{
		foreach($tpa as $pid => $val)
			$mpa[$pid]=$val;
	}
	// General manager?
	$tpa=getMailGeneralManager(array(), $setting, $pers);
	if(sizeof($tpa))
	{
		foreach($tpa as $pid => $val)
			$mpa[$pid]=$val;
	}

	// Administrator?
	$tpa=getMailAdministrator(array(), $setting, $pers);
	if(sizeof($tpa))
	{
		foreach($tpa as $pid => $val)
			$mpa[$pid]=$val;
	}

	return $mpa;
}

/**
 * ???FUNCTION???
 *
 * @param various $mpa     - Mandatory parameter:               ???PARAMETER???
 * @param various $setting - Mandatory parameter:               ???PARAMETER???
 * @param integer $pers    - Optional parameter (default = 0):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function getMailSuperior($mpa, $setting, $pers=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!$pers)
		$pers=$GLOBALS['fxpglobals']['person'];

	if(check_mail($setting,'VG'))
	{
		$sp=get_realelter($pers);
		if($sp && ($sp != $GLOBALS['fxpglobals']['person']))
			$mpa[$sp]=0;
	}

	return $mpa;
}

/**
 * ???FUNCTION???
 *
 * @param various $mpa     - Mandatory parameter:                   ???PARAMETER???
 * @param various $setting - Mandatory parameter:                   ???PARAMETER???
 * @param integer $pers    - Optional parameter (default = 0):      ???PARAMETER???
 * @param boolean $pca     - Optional parameter (default = false):  ???PARAMETER???
 * @param string  $sdate   - Optional parameter (default = ''):     ???PARAMETER???
 * @param string  $edate   - Optional parameter (default = ''):     ???PARAMETER???
 *
 * @return ???RETURN???
 */
function getMailProjectResponsibles($mpa, $setting, $pers=0, $pca=false, $sdate='', $edate='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!$pers)
		$pers=$GLOBALS['fxpglobals']['person'];

	if(!fxIsArray($pca))
		$pca=array('SV'=>"vertreter", 'PL'=>"projektleiter", 'PM'=>"projektmanager");

	if(strlen((string)$sdate))
		$sdate=substr((string)$sdate.'00000000',0,8).'000000';
	if(strlen((string)$edate))
		$edate=substr((string)$edate.'99999999',0,8).'240000';

	foreach($pca as $pt => $field)
	{
		if(check_mail($setting,$pt))
		{
			$sql  = "SELECT personen_id, pname, vorname FROM personen";
			$sql .= " WHERE personen_id IN";
			$sql .= " (SELECT ".$field." FROM projekte WHERE projekt_id IN";
			$sql .= " (SELECT projekt_id FROM maplanung WHERE personen_id=".$pers." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			if(strlen((string)$sdate) || strlen((string)$edate))
			{
				$sql .= " AND auslastung>0";
				if(strlen((string)$sdate) && strlen((string)$edate))
					$sql .= " AND (soll_beginn<='".$edate."' AND soll_ende>='".$sdate."')";
				else if(strlen((string)$sdate))
					$sql .= " AND soll_beginn>='".$sdate."'";
				else
					$sql .= " AND soll_ende<='".$edate."')";
			}
			$sql .= " ) AND projekt_status IN (".FXP_PS_PLANNED.",".FXP_PS_ACTIVE.") AND mandanten_id=".$GLOBALS['fxpglobals']['client'].")";
			$sql .= " AND personen_id<>".$GLOBALS['fxpglobals']['person']." AND (inaktiv IS NULL OR inaktiv=0) AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$tpa=db_values($sql,'*');
//fxDebug($tpa,$sql);
			if(fxIsArray($tpa))
			{
				foreach($tpa as $t)
					$mpa[$t['personen_id']]=1;
			}
		}
	}

	return $mpa;
}

/**
 * ???FUNCTION???
 *
 * @param various $mpa     - Mandatory parameter:               ???PARAMETER???
 * @param various $setting - Mandatory parameter:               ???PARAMETER???
 * @param integer $pers    - Optional parameter (default = 0):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function getMailResourceplaner($mpa, $setting, $pers=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!$pers)
		$pers=$GLOBALS['fxpglobals']['person'];

	if(check_mail($setting,'RP'))
	{
		$tpa=getRolePersons('-3',$GLOBALS['fxpglobals']['person']);
		if(sizeof($tpa))
		{
			foreach($tpa as $rid => $t)
				$mpa[$rid]=2;
		}
	}

	return $mpa;
}

/**
 * ???FUNCTION???
 *
 * @param various $mpa     - Mandatory parameter:               ???PARAMETER???
 * @param various $setting - Mandatory parameter:               ???PARAMETER???
 * @param integer $pers    - Optional parameter (default = 0):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function getMailGeneralManager($mpa, $setting, $pers=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!$pers)
		$pers=$GLOBALS['fxpglobals']['person'];

	if(check_mail($setting,'GF'))
	{
		$tpa=getRolePersons('2',$GLOBALS['fxpglobals']['person']);
		if(sizeof($tpa))
		{
			foreach($tpa as $rid => $t)
				$mpa[$rid]=3;
		}
	}

	return $mpa;
}

/**
 * ???FUNCTION???
 *
 * @param various $mpa     - Mandatory parameter:               ???PARAMETER???
 * @param various $setting - Mandatory parameter:               ???PARAMETER???
 * @param integer $pers    - Optional parameter (default = 0):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function getMailAdministrator($mpa, $setting, $pers=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!$pers)
		$pers=$GLOBALS['fxpglobals']['person'];

	if(check_mail($setting,'AD'))
	{
		$vfdate=substr((string)$GLOBALS['datetime'],0,8).'000000';
		$vtdate=substr((string)$GLOBALS['datetime'],0,8).'240000';

		$sql  = "SELECT DISTINCT p.personen_id, p.pname, p.vorname FROM benutzer b, personen p";
		$sql .= " WHERE p.personen_id=b.personen_id";
		$sql .= " AND b.b_admin=1 AND (b.status_benutzer IS NULL OR b.status_benutzer<2) AND b.mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$sql .= " AND (b.gueltig_von IS NULL OR b.gueltig_von<'".$vtdate."') AND (b.gueltig_bis IS NULL OR b.gueltig_bis>'".$vfdate."')";
		$sql .= " AND (p.inaktiv IS NULL OR p.inaktiv=0) AND p.mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$sql .= " AND (p.gueltig_von IS NULL OR p.gueltig_von<'".$vtdate."') AND (p.gueltig_bis IS NULL OR p.gueltig_bis>'".$vfdate."')";
		$tpa=db_values($sql,'*');
//fxDebug($tpa,$sql);
		if(fxIsArray($tpa))
		{
			foreach($tpa as $t)
				$mpa[$t['personen_id']]=4;
		}
	}

	return $mpa;
}

/**
 * ???FUNCTION???
 *
 * @param various $mpa  - Mandatory parameter: ???PARAMETER???
 * @param various $pers - Mandatory parameter: ???PARAMETER???
 * @param various $pnew - Mandatory parameter: ???PARAMETER???
 * @param various $pold - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function sendEmployeeDataMails($mpa, $pers, $pnew, $pold)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Init variables
	$settings_backup=$GLOBALS['fxpglobals']['settings'];
	$lang_backup=$GLOBALS['fxpglobals']['lang'];
	$sent=0;
	if(!isset($GLOBALS['_sent_success']))
		$GLOBALS['_sent_success']='';
	if(!isset($GLOBALS['_sent_error']))
		$GLOBALS['_sent_error']='';
	if(fxIsArray($mpa))
	{
//fxDebug($mpa, '$mpa');
		if($pold['maaustrittsdatum'] === 'NULL')
			$pold['maaustrittsdatum']='';
		if($pnew['maaustrittsdatum'] === 'NULL')
			$pnew['maaustrittsdatum']='';
//fxDebug($pold,'$pold');
//fxDebug($pnew,'$pnew');

		// Check for changes
		$dates=false;
		if(($pnew['maeintrittsdatum'] !== $pold['maeinstrittsdatum']) || ($pnew['maaustrittsdatum'] !== $pold['maaustrittsdatum']))
			$dates=true;
		$status=false;
		if($pnew['status'] !== $pold['status'])
			$status=true;

		if($dates || $status)
		{
			$ext=false;
			foreach($mpa as $pid => $etype)
			{
				if($etype)
				{
					$ext=true;
					break;
				}
			}

			// All languages
			$la=getLanguages();
//fxDebug($la,'$la');

			$eia=array();
			$eta=array();
			foreach($la as $lang => $langtext)
			{
				$los=0;
				if(isset($GLOBALS['_lang_override']))
					$los=$GLOBALS['_lang_override'];
				$GLOBALS['_lang_override']=$lang;

				// Mitarbeiterzusatzdaten^Das Ein-/Austrittsdatum oder der Status des folgenden Mitarbeiters hat sich ver&auml;ndert^Name|Namenszusatz|ID-Nr.|AC~Eintrittsdatum|AC~Austrittsdatum|AC~Status
				// Employee's Additional Info^The entry/leave date or the status of the following employee has changed^Name|Affix|ID-No.|AC~Entry Date|AC~Leave Date|AC~Status
				$msa=explode('^',get_message('email_pers_addinfo'));
				$eia[$lang]=array('title'=>$msa[0], 'headline'=>$msa[1], 'headers'=>$msa[2], 'mbgcolor'=>'006b9f');

				if($ext)
				{
					// Ressourceplanung^Der Mitarbeiter ist in folgenden geplanten oder aktiven Aufgaben verplant^Aufgabenname|AC~Status|AC~Priorit&auml;t|Beschreibung|AR~Prozent|AR~Soll-Aufwand|AC~Soll-Beginn|AC~Soll-Ende
					// Resource Planning^The employee is assigned to the following planned or active tasks^Task Name|AC~Status|AC~Priority|Description|AR~Percentage|AR~Planned Effort|AC~Planned Start|AC~Planned End
					$msa=explode('^',get_message('email_pers_planned'));
					$eta[$lang]=array('title'=>$msa[0], 'headline'=>$msa[1], 'headers'=>$msa[2], 'mbgcolor'=>'888888');
				}

				if(!$los)
					unset($GLOBALS['_lang_override']);
				else
					$GLOBALS['_lang_override']=$los;
			}

			if($ext)
			{
				// Aufgaben
				$sql  = "SELECT p.vorgangsnummer, p.name_projekt, p.projekt_status, p.prioritaet, p.projektmanager, p.projektleiter, p.vertreter, p.beschreibung,";
				$sql .= " m.soll_beginn, m.soll_ende, m.aufwand_std, m.prozent";
				$sql .= " FROM projekte p, maplanung m";
				$sql .= " WHERE m.personen_id=".$pers." AND (m.sachmittel_id IS NULL OR m.sachmittel_ID=0) AND m.aufwand_std>0 AND m.mandanten_id=".$GLOBALS['fxpglobals']['client'];
				$sql .= " AND p.projekt_id=m.projekt_id AND p.projekt_status IN (".FXP_PS_PLANNED.",".FXP_PS_ACTIVE.") AND p.mandanten_id=".$GLOBALS['fxpglobals']['client'];
				$sql .= " ORDER BY p.vorgangsnummer";
				$eta['tasks']=db_values($sql,'*');
			}
//fxDebug($eta,'$eta');

			if(!isset($GLOBALS['_ma']))
				$GLOBALS['_ma']=array();
			$sa=array();
			foreach($mpa as $pid => $etype)
			{
				$pdata=get_email_adress($pid,true,true);
//fxDebug($pdata,'$pdata: '.$pid);
				if(fxIsArray($pdata) && fxIsArray($pdata[$pid]) && strlen((string)$pdata[$pid]['email']) && !isset($sa[$pdata[$pid]['email']]))
				{
					$sa[$pdata[$pid]['email']]=true;

					$GLOBALS['fxpglobals']['settings']=$pdata[$pid]['settings'];
					$GLOBALS['fxpglobals']['lang']=$GLOBALS['fxpglobals']['settings']['sprache'];
					$lang=$GLOBALS['fxpglobals']['lang'];

					// ...Salutation
					$s=$pdata[$pid]['anrede2'];
					if(!isset($GLOBALS['_ma'][$s.'_'.$lang]))
						$GLOBALS['_ma'][$s.'_'.$lang]=$pdata[$pid]['anrede2_txt'];

					$content=str_replace('$var1', $pdata[$pid]['name2'], $GLOBALS['_ma'][$s.'_'.$lang]).'<br /><br /><br />'.$GLOBALS['nl'];

					// ...Name
					$nold=$pold['pname'];
					if(strlen((string)$pold['vorname']))
						$nold=$pold['vorname'].' '.$nold;
					$nnew=$pnew['pname'];
					if(strlen((string)$pnew['vorname']))
						$nnew=$pnew['vorname'].' '.$nnew;
					$data='N~W33%~';
					if($nnew != $nold)
						$data .= '<font style="color:#888888;">'.$nold.'</font>'.$GLOBALS['_carrow'].'<b>'.$nnew.'</b>';
					else
						$data .= $nnew;
					// ...Affix
					$data .= '|N~';
					if($pnew['name_zusatz'] != $pold['name_zusatz'])
						$data .= '<font style="color:#888888;">'.$pold['name_zusatz'].'</font>'.$GLOBALS['_carrow'].'<b>'.$pnew['name_zusatz'].'</b>';
					else
						$data .= $pnew['name_zusatz'];
					// ...ID-No.
					$data .= '|N~';
					if($pnew['personen_initialen'] != $pold['personen_initialen'])
						$data .= '<font style="color:#888888;">'.$pold['personen_initialen'].'</font>'.$GLOBALS['_carrow'].'<b>'.$pnew['personen_initialen'].'</b>';
					else
						$data .= $pnew['personen_initialen'];
					// ...Entry Date
					$data .= '|AC~N~';
					if($pnew['maeintrittsdatum'] != $pold['maeintrittsdatum'])
						$data .= '<font style="color:#888888;">'.get_entry($pold['maeintrittsdatum'],'datum').'</font>'.$GLOBALS['_carrow'].'<b>'.get_entry($pnew['maeintrittsdatum'],'datum').'</b>';
					else
						$data .= get_entry($pnew['maeintrittsdatum'],'datum');
					// ...Leave Date
					$data .= '|AC~N~';
					if($pnew['maaustrittsdatum'] != $pold['maaustrittsdatum'])
						$data .= '<font style="color:#888888;">'.get_entry($pold['maaustrittsdatum'],'datum').'</font>'.$GLOBALS['_carrow'].'<b>'.get_entry($pnew['maaustrittsdatum'],'datum').'</b>';
					else
						$data .= get_entry($pnew['maaustrittsdatum'],'datum');
					// ...Status
					if(!isset($GLOBALS['_ma']['st0_'.$lang]))
						$GLOBALS['_ma']['st0_'.$lang]=meldung(300);	// active
					if(!isset($GLOBALS['_ma']['st1_'.$lang]))
						$GLOBALS['_ma']['st1_'.$lang]=meldung(299);	// inactive
					$data .= '|AC~N~';
					if($pnew['inaktiv'] != $pold['inaktiv'])
						$data .= '<font style="color:#888888;">'.$GLOBALS['_ma']['st'.$pold['inaktiv'].'_'.$lang].'</font>'.$GLOBALS['_carrow'].'<b>'.$GLOBALS['_ma']['st'.$pnew['inaktiv'].'_'.$lang].'</b>';
					else
						$data .= $GLOBALS['_ma']['st'.$pnew['inaktiv'].'_'.$lang];

					$content .= mailDataTable($eia[$lang]['headers'], array($data), $eia[$lang]['headline'], $eia[$lang]['mbgcolor']);

					// ...Aufgabenliste
					if($etype && fxIsArray($eta) && fxIsArray($eta['tasks']))
					{
						$ped=substr((string)$pnew['maeintrittsdatum'],0,8);
						if(strlen((string)$pnew['maaustrittsdatum']) && ($pnew['maaustrittsdatum'] !== 'NULL'))
							$pld=substr((string)$pnew['maaustrittsdatum'],0,8);
						else
							$pld='99999999';
//echo('$ped='.$ped.', $pld='.$pld.'<hr />');

						$ada=array();
						foreach($eta['tasks'] as $p)
						{
							if(($etype > 1) || ($p['projektleiter'] == $pid))
							{
								$tsd=substr((string)$p['soll_beginn'],0,8);
								$ted=substr((string)$p['soll_ende'],0,8);
//echo('... $tsd='.$tsd.', $ted='.$ted.'<hr />');

								// Task Name
								$adata  = 'N~<b>'.no_null($p['vorgangsnummer']).'</b>&nbsp;'.$p['name_projekt'];
								// Status
								$adata .= '|AC~N~'.meldung($p['projekt_status']);
								// Priority
								$adata .= '|AC~N~'.meldung($p['prioritaet']);
								// Description
								$adata .= '|'.$p['beschreibung'];
								// Percentage
								$adata .= '|AR~N~'.get_entry($p['prozent']*100.0,'dezimal').'&nbsp;%';
								// Planned Effort
								$adata .= '|AR~N~'.get_entry($p['aufwand_std']/3600,'dezimal').'&nbsp;h';
								// Planned Start
								if(($tsd < $ped) || ($tsd > $pld) || $pnew['inaktiv'])
									$adata .= '|AC~N~<font color="#'.colorCode('red').'">'.get_entry($tsd,'datum').'</font>';
								else
									$adata .= '|AC~N~'.get_entry($tsd,'datum');
								// Planned End
								if(($ted < $ped) || ($ted > $pld) || $pnew['inaktiv'])
									$adata .= '|AC~N~<font color="'.colorCode('red').'">'.get_entry($ted,'datum').'</font>';
								else
									$adata .= '|AC~N~'.get_entry($ted,'datum');

								$ada[]=$adata;
							}
						}

						if(sizeof($ada))
						{

							$content .= '<br /><br /><br />';
							$content .= mailDataTable($eta[$lang]['headers'], $ada, $eta[$lang]['headline']);
						}
					}

					// Send mail
//echo('<div style="border:1px solid #444444;border-top-left-radius:8px;border-top-right-radius:8px;padding:8px;color:#ffffff;background:#006b9f;"><b>'.$eia[$lang]['title'].'</b> <i>(recipient='.$pid.')</i></div><div style="border:1px solid #444444;border-top:0;border-bottom-left-radius:8px;border-bottom-right-radius:8px;padding:8px;background:#ffffff;margin-bottom:6px;">'.$content.'</div>'.$GLOBALS['nl']);
//$send_error=true;
					$send_error=send_mail($pid,$eia[$lang]['title'],$content);
					if($send_error)
					{
						$sent=false;
						$GLOBALS['_sent_error'] .= '<br />&middot; <b class="red">'.getPersonName($pid,false).'</b>';
					}
					else
					{
						if($sent !== false)
							$sent++;
						$GLOBALS['_sent_success'] .= '<br />&middot; <b class="grey">'.getPersonName($pid,false).'</b>';
					}
				}
			}
		}
	}

	$GLOBALS['fxpglobals']['settings']=$settings_backup;
	$GLOBALS['fxpglobals']['lang']=$lang_backup;

	return $sent;
}

/**
 * ???FUNCTION???
 *
 * @param various $mpa     - Mandatory parameter: ???PARAMETER???
 * @param various $pers    - Mandatory parameter: ???PARAMETER???
 * @param various $setting - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function addProjectMailPerson($mpa, $pers, $setting)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$ps='';
	if(!fxIsArray($mpa) || !isset($mpa[$pers]))
		$ps=$setting;
	else
	{
		$ps=$mpa[$pers];
		$sa=explode(',', $ps);
		$sf=false;
		foreach($sa as $set)
		{
			if($set == $setting)
			{
				$sf=true;
				break;
			}
		}
		if(!$sf)
			$ps .= ','.$setting;
	}

	return $ps;
}

/**
 * ???FUNCTION???
 *
 * @param various $mpa      - Mandatory parameter:                     ???PARAMETER???
 * @param various $setting  - Mandatory parameter:                     ???PARAMETER???
 * @param various $pnew     - Mandatory parameter:                     ???PARAMETER???
 * @param array   $pold     - Optional parameter (default = array()):  ???PARAMETER???
 * @param boolean $delegate - Optional parameter (default = true):     ???PARAMETER???
 *
 * @return ???RETURN???
 */
function getProjectMailPersons($mpa, $setting, $pnew, $pold=array(), $delegate=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Inform 2nd PL (if set and is not the one who changed it)
	$pl2nset=-1;
	if($pnew['vertreter'] && ($pnew['vertreter'] != $GLOBALS['fxpglobals']['person']))
		$pl2nset=$pnew['vertreter'];
	$pl2oset=-1;
	if(fxIsArray($pold) && $pold['vertreter'] && ($pold['vertreter'] != $GLOBALS['fxpglobals']['person']))
		$pl2oset=$pold['vertreter'];
	if(check_mail($setting,'SV'))
	{
		if($pl2nset > 0)
		{
			$mpa[$pl2nset]=addProjectMailPerson($mpa,$pl2nset,$setting);
			$pl2nset=0;
		}
		if($pl2oset > 0)
		{
			$mpa[$pl2oset]=addProjectMailPerson($mpa,$pl2oset,$setting);
			$pl2oset=0;
		}
	}

	// Inform 1st PL (if set and is not the one who changed it) or 2nd PL if 1st PL is absent
	if(check_mail($setting,'PL'))
	{
		if($pnew['projektleiter'] && ($pnew['projektleiter'] != $GLOBALS['fxpglobals']['person']))
		{
			$mpa[$pnew['projektleiter']]=addProjectMailPerson($mpa,$pnew['projektleiter'],$setting);
			if($delegate && ($pl2nset > 0) && persIsAbsent($pnew['projektleiter']))
				$mpa[$pl2nset]=addProjectMailPerson($mpa,$pl2nset,$setting);
		}
		if(fxIsArray($pold) && $pold['projektleiter'] && ($pold['projektleiter'] != $GLOBALS['fxpglobals']['person']))
		{
			$mpa[$pold['projektleiter']]=addProjectMailPerson($mpa,$pold['projektleiter'],$setting);
			if($delegate && ($pl2oset > 0) && persIsAbsent($pold['projektleiter']))
				$mpa[$pl2oset]=addProjectMailPerson($mpa,$pl2oset,$setting);
		}
	}

	// Inform PM (if set and is not the one who changed it)
	if(check_mail($setting,'PM'))
	{
		if($pnew['projektmanager'] && ($pnew['projektmanager'] != $GLOBALS['fxpglobals']['person']))
			$mpa[$pnew['projektmanager']]=addProjectMailPerson($mpa,$pnew['projektmanager'],$setting);
		if(fxIsArray($pold) && $pold['projektmanager'] && ($pold['projektmanager'] != $GLOBALS['fxpglobals']['person']))
			$mpa[$pold['projektmanager']]=addProjectMailPerson($mpa,$pold['projektleiter'],$setting);
	}

	// Inform CP (if set and is not the one who changed it)
	if(check_mail($setting,'AG'))
	{
		if($pnew['ansprechpartner'] && ($pnew['ansprechpartner'] != $GLOBALS['fxpglobals']['person']))
			$mpa[$pnew['ansprechpartner']]=addProjectMailPerson($mpa,$pnew['ansprechpartner'],$setting);
		if(fxIsArray($pold) && $pold['ansprechpartner'] && ($pold['ansprechpartner'] != $GLOBALS['fxpglobals']['person']))
			$mpa[$pold['ansprechpartner']]=addProjectMailPerson($mpa,$pold['ansprechpartner'],$setting);
	}

	// Inform RP's (if set and is not the one who changed it)
	if(check_mail($setting,'RP'))
	{
		$tpa=getRolePersons('-3',$GLOBALS['fxpglobals']['person']);
		if(sizeof($tpa))
		{
			foreach($tpa as $gmid => $t)
				$mpa[$gmid]=addProjectMailPerson($mpa,$gmid,$setting);
		}
	}

	// Inform GM's (if set and is not the one who changed it)
	if(check_mail($setting,'GF'))
	{
		$tpa=getRolePersons('2',$GLOBALS['fxpglobals']['person']);
		if(sizeof($tpa))
		{
			foreach($tpa as $gmid => $t)
				$mpa[$gmid]=addProjectMailPerson($mpa,$gmid,$setting);
		}
	}

	return $mpa;
}

/**
 * ???FUNCTION???
 *
 * @param various $pl1 - Mandatory parameter:               ???PARAMETER???
 * @param integer $pl2 - Optional parameter (default = 0):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function getProjectleader($pl1, $pl2=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$plid=0;
	if($pl1 && !persIsAbsent($pl1))
		$plid=$pl1;
	else if($pl2 && !persIsAbsent($pl2))
		$plid=$pl2;
	if(!$plid)
		$plid=$pl1;

	return $plid;
}

/**
 * Get name of a person
 *
 * @param various $pid         - Mandatory parameter:                   Person's ID
 * @param boolean $last_first  - Optional parameter (default = true):   Format: "Name, Surname" (true) or "Surname Name" (false)
 * @param boolean $with_parent - Optional parameter (default = false):  Display assigned company before the person's name?
 *
 * @return string Name of person
 */
function getPersonName($pid, $last_first=true, $with_parent=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$pn='';
	$pid=(int)$pid;
	if($pid)
	{
		if(!isset($GLOBALS['_personname_buffer']))
			$GLOBALS['_personname_buffer']=array();
		if(!isset($GLOBALS['_personname_buffer'][$pid]))
		{
			$sql="SELECT pname, vorname, elter FROM personen WHERE personen_id=".$pid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$tmp=db_values($sql);
//fxDebug($tmp,$sql);
			if(fxIsArray($tmp))
			{
				$pn=$tmp['pname'];
				if(($tmp['vorname'] !== NULL) && strlen((string)$tmp['vorname']))
				{
					if($last_first)
						$pn .= ', '.$tmp['vorname'];
					else
						$pn=$tmp['vorname'].' '.$pn;
				}

				if($with_parent && $tmp['elter'])
				{
					$sql="SELECT pname, vorname FROM personen WHERE personen_id=".$tmp['elter']." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
					$tmp=db_values($sql);
//fxDebug($tmp,$sql);
					if(fxIsArray($tmp))
					{
						$en=$tmp['pname'];
						if(($tmp['vorname'] !== NULL) && strlen((string)$tmp['vorname']))
						{
							if($last_first)
								$en .= ', '.$tmp['vorname'];
							else
								$en=$tmp['vorname'].' '.$en;
						}

						$pn=$en.' - '.$pn;
					}
				}
			}
			else
				$pn='?';
			$GLOBALS['_personname_buffer'][$pid]=$pn;
		}
		else
			$pn=$GLOBALS['_personname_buffer'][$pid];
	}

	return $pn;
}

/**
 * ???FUNCTION???
 *
 * @param various $pid  - Mandatory parameter:                   ???PARAMETER???
 * @param various $aid  - Mandatory parameter:                   ???PARAMETER???
 * @param string  $mds  - Optional parameter (default = 'inv'):  ???PARAMETER???
 * @param boolean $send - Optional parameter (default = true):   ???PARAMETER???
 *
 * @return ???RETURN???
 */
function sendInvitationMails($pid, $aid, $mds='inv', $send=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	//echo('<b>sendInvitationMails</b>($pid='.$pid.', $aid='.$aid.', $mds='.$mds.', $send='.$send.')<br />');

	$pid=(int)$pid;
	$aid=(int)$aid;
	$mds=strtolower((string)trim((string)$mds));
	if(!$pid || !$aid || !strlen((string)$mds))
		return true;

	$mda=array('inv'=>0, 'disinv'=>1, 'inv_acc'=>2, 'inv_refuse'=>3);
	if(!isset($mda[$mds]))
		return true;

	// Get actual appointment data
	$sql="SELECT * FROM termine_history WHERE termin_id=".$aid." AND mandanten_id=".$GLOBALS['fxpglobals']['client']." ORDER BY zeitstempel DESC";
	$aad=db_values($sql);
//fxDebug($aad,$sql);
	if(!fxIsArray($aad))
		return true;

	$mid=(int)$aad['mastertermin_id'];
	$type=$mda[$mds];
	$md='email_'.$mds;

	// Get change counter
	$cc=0;
	if(!$type)
	{
		$sql  = "SELECT COUNT(autoid) AS cc FROM termine_history WHERE mastertermin_id=".$mid." AND termin_id<>".$mid." AND personen_id=".$pid;
		$sql .= " AND aktion=2 AND terminart=".FXP_AT_INVITATION." AND mandanten_id=".$GLOBALS['fxpglobals']['client']." AND zeitstempel<'".$aad['zeitstempel']."'";
		$cc=(int)db_value($sql);
//fxDebug($cc,$sql);
	}

	$settings_backup=$GLOBALS['fxpglobals']['settings'];
	$lang_backup=$GLOBALS['fxpglobals']['lang'];

	if(!isset($GLOBALS['_ua']))
		$GLOBALS['_ua']=array();
	if(!isset($GLOBALS['_ma']))
		$GLOBALS['_ma']=array();

	// 0=inv:			Invitation^Invitation from [{name}] to "{app}" on {date} at {time} o'clock^<table...
	// 1=disinv:		Disinvitation^Disinvitation from [{name}] to "{app}" on {date} at {time} o'clock^<table...
	// 2=inv_acc:		Acceptance^Acceptance from [{name}] for "{ref}" on {date} at {time} o'clock^<table...
	// 3=inv_refuse:	Refusal^Refusal from [{name}] for "{ref}" on {date} at {time} o'clock^<table...
	if(!isset($GLOBALS['_ma'][$md.'_'.$lang_backup]))
		$GLOBALS['_ma'][$md.'_'.$lang_backup]=explode('^',get_message($md));

	// Get recipient data
	if(!isset($GLOBALS['_ua'][$pid]))
	{
		$pd=get_email_adress($pid,true,true);
		if(fxIsArray($pd) && fxIsArray($pd[$pid]))
			$GLOBALS['_ua'][$pid]=$pd[$pid];
		else
			$GLOBALS['_ua'][$pid]=false;
	}
	if(!fxIsArray($GLOBALS['_ua'][$pid]) || !strlen((string)$GLOBALS['_ua'][$pid]['email']))
		return true;

	$pid_name=$GLOBALS['_ua'][$pid]['name2'];

	$GLOBALS['fxpglobals']['settings']=$GLOBALS['_ua'][$pid]['settings'];
	$GLOBALS['fxpglobals']['lang']=$GLOBALS['fxpglobals']['settings']['sprache'];
	$GLOBALS['lang']=$GLOBALS['fxpglobals']['lang'];

	if(!isset($GLOBALS['_ma'][$md.'_'.$GLOBALS['lang']]))
		$GLOBALS['_ma'][$md.'_'.$GLOBALS['lang']]=explode('^',get_message($md));
//fxDebug($GLOBALS['_ma'], '_ma');

	// Variables
	$ano='';
	$achg='';
	if($cc > 0)
	{
		// changed
		if(!isset($GLOBALS['_ma']['170_'.$GLOBALS['lang']]))
			$GLOBALS['_ma']['170_'.$GLOBALS['lang']]=meldung(170);

		$ano=($cc+1).'. ';
		$achg=$GLOBALS['_ma']['170_'.$GLOBALS['lang']];
	}
	$aname=getPersonName($aad['personen_id'],false);
	$adate=get_entry($aad['startzeit'],'datum');
	$atime=get_entry($aad['startzeit'],'zeit');
	if($aad['endzeit'] != $aad['startzeit'])
		$atime .= '-'.get_entry($aad['endzeit'],'zeit');
	$aref=$aad['name_termin'];
	$atype=meldung($aad['terminart']);
	$aloc=$aad['treffpunkt'];
	if(($aloc === NULL) || !strlen((string)$aloc))
		$aloc='<font style="color:#'.colorCode('lightgrey').';">---</font>';
	$adesc=$aad['beschreibung'];
	if(($adesc === NULL) && !strlen((string)$adesc))
		$adesc='<font style="color:#'.colorCode('lightgrey').';">---</font>';

	$repa=array(
		'{no}'		=> $ano,
		'{chg}'		=> $achg,
		'{name}'	=> $aname,
		'{date}'	=> $adate,
		'{time}'	=> $atime,
		'{ref}'		=> $aref,
		'{app}'		=> $atype,
		'{loc}'		=> $aloc,
		'{desc}'	=> $adesc
	);

	// E-mail content
	$content='';
	if($aad['personen_id'] != $GLOBALS['fxpglobals']['person'])	// Edited by
		$content .= '<table width="80%" cellpadding="8" cellspacing="0" style="border:1px solid #'.colorCode('red').';border-radius:4px;box-shadow:2px 2px 4px #'.colorCode('darkgrey').';"><tr><td>'.meldung(47).': <b>'.getPersonName($GLOBALS['fxpglobals']['person'],false).'</b></td></tr></table><br /><br /><br />';
	$content .= strtr($GLOBALS['_ma'][$md.'_'.$GLOBALS['lang']][2],$repa);

	// Send e-mail
	if($send)
	{
		$merr=send_mail($pid, $ano.strtr($GLOBALS['_ma'][$md.'_'.$GLOBALS['lang']][1],$repa), $content);

		$GLOBALS['fxpglobals']['settings']=$settings_backup;
		$GLOBALS['fxpglobals']['lang']=$lang_backup;

		if($merr)
		{
			// 61: An e-mail regarding $var1 could not be sent to $var2!
			error_msg(61, false, '<b>'.$ano.$GLOBALS['_ma'][$md.'_'.$lang_backup][0].': '.$aref.'</b>', '(<b style="color:#'.colorCode('darkgrey').';">'.$pid_name.'</b>)');
		}
		else
		{
			// 62: An e-mail regarding $var1 has been successfully sent to $var2.
			error_msg(62, false, '<b>'.$ano.$GLOBALS['_ma'][$md.'_'.$lang_backup][0].': '.$aref.'</b>', '(<b style="color:#'.colorCode('darkgrey').';">'.$pid_name.'</b>)');
		}
		return $merr;
	}

	return array('person'=>$pid, 'email'=>$GLOBALS['_ua'][$pid]['email'], 'title'=>strtr($GLOBALS['_ma'][$md.'_'.$GLOBALS['lang']][1],$repa), 'data'=>$content);
}

/**
 * ???FUNCTION???
 *
 * @param various $recipient  - Mandatory parameter:                  ???PARAMETER???
 * @param boolean $with_tasks - Optional parameter (default = true):  ???PARAMETER???
 * @param integer $saction    - Optional parameter (default = 0):     ???PARAMETER???
 */
function absentAppMail($recipient, $with_tasks=true, $saction=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$msg=1358;					// New absence^Changed absence^Deleted absence^Approved absence^Rejected absence
	if($GLOBALS['overtime'])
		$msg=189;				// New outpayment^Changed outpayment^Deleted outpayment^Approved outpayment^Rejected outpayment
	if(!isset($GLOBALS['_ma'][$msg.'_'.$GLOBALS['lang']]))
		$GLOBALS['_ma'][$msg.'_'.$GLOBALS['lang']]=explode('^', meldung($msg));

	// Person|Terminart|Zeitraum|AR~Dauer|Beschreibung
	// Person|App. Type|Timespan|AR~Duration|Description
	$md='email_absapp';
	if(!isset($GLOBALS['_ma'][$md.'_'.$GLOBALS['lang']]))
		$GLOBALS['_ma'][$md.'_'.$GLOBALS['lang']]=get_message($md);

	if(!$saction)
		$saction=$GLOBALS['fxpglobals']['action'];

	$subject='';
	if($GLOBALS['fxpglobals']['tr'] == 14)
	{
		if($saction == FXP_CREATE)
			$subject=$GLOBALS['_ma'][$msg.'_'.$GLOBALS['lang']][3];
		else if($saction == FXP_DELETE)
			$subject=$GLOBALS['_ma'][$msg.'_'.$GLOBALS['lang']][4];
	}
	if(!strlen((string)$subject))
		$subject=$GLOBALS['_ma'][$msg.'_'.$GLOBALS['lang']][$saction-2];

	$GLOBALS['title']=$subject.': ';

	$atype=meldung($GLOBALS['ftart'],false);
	if($GLOBALS['overtime'])
	{
		$ehours=get_entry($GLOBALS['hours'],'dezimal');
		$emonth=meldung(17+(int)substr((string)$GLOBALS['db_start'],4,2), false).' '.substr((string)$GLOBALS['db_start'],0,4);

		// 1947: $var1 Std. für $var2				z.B.	Überstundenauszahlung - Max Mustermann: 8,00 Std. für Mai 2015
		// 1947: $var1 h for $var2					e.g.	Overtime payment - John Doe: 8.0 h for May 2015
		$message=meldung(1947, false, $ehours,$emonth);
		$GLOBALS['title'] .= $atype.' - '.$GLOBALS['applicant_name'].': '.$message;

		$atime=$emonth;
		$adur=$ehours.' h';
	}
	else
	{
		$edays=get_entry($GLOBALS['days'],'dezimal');
		$etimespan=get_entry($GLOBALS['db_start'],'datetime');
		if(substr((string)$GLOBALS['db_end'],0,8) == substr((string)$GLOBALS['db_start'],0,8))
			$etimespan .= '-'.get_entry($GLOBALS['db_end'],'zeit');
		else
			$etimespan .= ' - '.get_entry($GLOBALS['db_end'],'datetime');

		// 480: $var1 Tag(e) im Zeitraum $var2		z.B.	Urlaubsantrag - Max Mustermann: 10 Tag(e) im Zeitraum 01.05.2015 - 14.05.2015
		// 480: $var1 day(s) in the timespan $var2	e.g.	Vacation application - John Doe: 10 day(s) in the timespan 05/01/2015 - 05/14/2015
		$message=meldung(480, false, $edays,$etimespan);
		$GLOBALS['title'] .= $atype.' - '.$GLOBALS['applicant_name'].': '.$message;

		$atime=$etimespan;
		$adur=$edays.' d';
	}

	if(fxIsArray($GLOBALS['old']))
	{
		$htype=meldung($GLOBALS['old']['terminart'],false);
		if(($GLOBALS['old']['terminart'] == FXP_AT_OVERTIMEOUTPAYREQUEST) || ($GLOBALS['old']['terminart'] == FXP_AT_OVERTIMEOUTPAY))
		{
			$htime=meldung(17+(int)substr((string)$GLOBALS['old']['startzeit'],4,2), false).' '.substr((string)$GLOBALS['old']['startzeit'],0,4);
			$hdur=get_entry($GLOBALS['old']['stunden'],'dezimal').' h';
		}
		else
		{
			$htime=get_entry($GLOBALS['old']['startzeit'],'datetime');
			if(substr((string)$GLOBALS['old']['endzeit'],0,8) == substr((string)$GLOBALS['old']['startzeit'],0,8))
				$htime .= '-'.get_entry($GLOBALS['old']['endzeit'],'zeit');
			else
				$htime .= ' - '.get_entry($GLOBALS['old']['endzeit'],'datetime');
			$hdur=get_entry($GLOBALS['old']['urlaub'],'dezimal').' d';
		}
		$hdesc=$GLOBALS['old']['beschreibung'];
	}
	else
	{
		$htype=$atype;
		$htime=$atime;
		$hdur=$adur;
		$hdesc=$GLOBALS['adesc'];
	}

	// ...Person
	$data ='N~';
	$data .= $GLOBALS['applicant_name'];
	// ...App. Type
	$data .= '|N~';
	if($htype != $atype)
		$data .= '<font style="color:#'.colorCode('grey').';">'.$htype.'</font>'.$GLOBALS['_carrow'].'<b>'.$atype.'</b>';
	else
		$data .= $atype;
	// ...Timespan
	$data .= '|N~';
	if($htime != $atime)
		$data .= '<font style="color:#'.colorCode('grey').';">'.$htime.'</font>'.$GLOBALS['_carrow'].'<b>'.$atime.'</b>';
	else
		$data .= $atime;
	// ...Duration
	$data .= '|AR~N~';
	if($hdur != $adur)
		$data .= '<font style="color:#'.colorCode('grey').';">'.$hdur.'</font>'.$GLOBALS['_carrow'].'<b>'.$adur.'</b>';
	else
		$data .= $adur;
	// ...Description
	$data .= '|W100%~';
	if($hdesc != $GLOBALS['adesc'])
		$data .= '<font style="color:#'.colorCode('grey').';">'.$hdesc.'</font>'.$GLOBALS['_carrow'].'<b>'.$GLOBALS['adesc'].'</b>';
	else
		$data .= $GLOBALS['adesc'];

	if($saction == FXP_CREATE)
		$GLOBALS['content']=mailDataTable($GLOBALS['_ma'][$md.'_'.$GLOBALS['lang']], array(0=>$data), $subject, colorCode('green'));
	else if($saction == FXP_CHANGE)
		$GLOBALS['content']=mailDataTable($GLOBALS['_ma'][$md.'_'.$GLOBALS['lang']], array(0=>$data), $subject, colorCode('blue'));
	else
		$GLOBALS['content']=mailDataTable($GLOBALS['_ma'][$md.'_'.$GLOBALS['lang']], array(0=>$data), $subject, colorCode('red'));

	if($with_tasks)
	{
		if(sizeof($GLOBALS['mtasks']))
		{
			// w&uuml;rde in dieser Zeitspanne in nachfolgenden Aufgaben fehlen:^Bitte stimmen Sie sich mit den Projektleitern ab vor einer Genehmigung oder Ablehnung!^Bitte stimmen Sie sich mit dem Vorgesetzten $var1ab!^fehlt in dieser Zeitspanne in nachfolgenden Aufgaben:^fehlt deshalb in dieser Zeitspanne in nachfolgenden Aufgaben nicht mehr:^Aufgabenname|AC~Status|AC~Priorit&auml;t|Projektleiter|Stellv. Projektleiter|Beschreibung|AC~Gepl. Zeitraum|AR~Prozent|AR~Gepl. Aufwand|AR~Tats. Aufwand|AR~Restaufwand
			// would be missing in the following tasks during this time span:^Please coordinate with the project leaders before approving or rejecting it!^Please coordinate with the supervisor $var1!^is missing in the following tasks during this time span:^is therefore not any longer missing in the following tasks during this time span:^Task Name|AC~Status|AC~Priority|1st Project Leader|2nd Project Leader|Description|AC~Planned Timespan|AR~Percentage|AR~Planned Effort|AR~Actual Effort|AR~Remain. Effort
			$md='email_app_tasks';
			if(!isset($GLOBALS['_ma'][$md.'_'.$GLOBALS['lang']]))
				$GLOBALS['_ma'][$md.'_'.$GLOBALS['lang']]=explode('^', get_message($md));
			$GLOBALS['content'] .= '<br /><br /><br /><br /><b>'.$GLOBALS['applicant_name'].'</b> ';
			if($saction == FXP_DELETE)
			{
				// is therefore not any longer missing in the following tasks during this time span:
				$GLOBALS['content'] .= $GLOBALS['_ma'][$md.'_'.$GLOBALS['lang']][4];
			}
			else if($GLOBALS['request'])
			{
				// would be missing in the following tasks during this time span:
				$GLOBALS['content'] .= $GLOBALS['_ma'][$md.'_'.$GLOBALS['lang']][0];
				// Exists a supervisor?
				if($GLOBALS['supervisor'])
				{
					if($recipient == $GLOBALS['supervisor'])
					{
						// Please coordinate with the project leaders before approving or rejecting it!
						$GLOBALS['content'] .= '<br /><i>('.$GLOBALS['_ma'][$md.'_'.$GLOBALS['lang']][1].')</i>';
					}
					else if(isset($GLOBALS['dtype']) && ($GLOBALS['dtype'] == 1))
					{
						// Please coordinate with the supervisor $var1!
						$GLOBALS['content'] .= '<br /><i>('.strtr($GLOBALS['_ma'][$md.'_'.$GLOBALS['lang']][2], array('$var1'=>'<b style="color:#'.colorCode('blue').';">'.$GLOBALS['supervisor_name'].'</b>')).')</i>';
					}
				}
			}
			else
			{
				// is missing in the following tasks during this time span:
				$GLOBALS['content'] .= $GLOBALS['_ma'][$md.'_'.$GLOBALS['lang']][3];
			}
			$GLOBALS['content'] .= emailAbsentTasks($GLOBALS['mtasks'], $GLOBALS['_ma'][$md.'_'.$GLOBALS['lang']][5]);
		}
		else if($saction != FXP_DELETE)
		{
			// w&uuml;rde in dieser Zeitspanne in <b>KEINER</b> Aufgaben mit geplanten Aufwand fehlen.
			// would <b>NOT</b> be missing any tasks with planned efforts during this time span.
			if(!isset($GLOBALS['_ma']['482_'.$GLOBALS['lang']]))
				$GLOBALS['_ma']['482_'.$GLOBALS['lang']]=meldung(482);
			$GLOBALS['content'] .= '<br /><br /><br /><br /><b>'.$GLOBALS['applicant_name'].'</b> '.$GLOBALS['_ma']['482_'.$GLOBALS['lang']];
		}
	}
}

/**
 * ???FUNCTION???
 *
 * @param various $persid - Mandatory parameter:               ???PARAMETER???
 * @param integer $pid    - Optional parameter (default = 0):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function getDefaultPersonCosts($persid,$pid=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$d=array();

	if(!strlen((string)$persid))
		return $d;

	$sql="SELECT personen_id FROM personen WHERE personen_id IN (".$persid.") AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$tmp=db_values($sql,'*');
//fxDebug($tmp,$sql);
	if(is_array($tmp) && sizeof($tmp))
	{
		foreach($tmp as $t)
			$d[(int)$t['personen_id']]=array('pp'=>0.0, 'sp'=>0.0, 'dpp'=>'', 'dsp'=>'');
	}

	$sql="SELECT personen_id, ek, vk FROM artikel WHERE personen_id IN (".$persid.") AND mandanten_id=".$GLOBALS['fxpglobals']['client']." AND (sachmittel_id IS NULL or sachmittel_id=0) AND (archiv IS NULL OR archiv=0)";
	$tmp=db_values($sql,'*');
//fxDebug($tmp,$sql);
	if(is_array($tmp) && sizeof($tmp))
	{
		foreach($tmp as $t)
		{
			$id=(int)$t['personen_id'];
			$d[$id]['pp']=round($t['ek'],2);
			$d[$id]['sp']=round($t['vk'],2);
		}
	}

	while($pid > 0)
	{
		$sql="SELECT mafaehigkeit_id, ek, vk FROM ekvk WHERE feldtyp=66 AND id=".$pid." AND maart_id=-".$pid." AND mafaehigkeit_id<0 AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$tmp=db_values($sql,'*');
//fxDebug($tmp,$sql);
		if(is_array($tmp) && sizeof($tmp))
		{
			foreach($tmp as $t)
			{
				$id=abs($t['mafaehigkeit_id']);
				if(isset($d[$id]))
				{
					if(!strlen((string)$d[$id]['dpp']) && strlen((string)$t['ek']))
						$d[$id]['dpp']=round($t['ek'],2);
					if(!strlen((string)$d[$id]['dsp']) && strlen((string)$t['vk']))
						$d[$id]['dsp']=round($t['vk'],2);
				}
			}
		}

		$sql="SELECT elter FROM projekte WHERE projekt_id=".$pid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$pid=(int)db_value($sql);
//fxDebug($pid,$sql);
	}

//fxDebug($d,'$d');

	return $d;
}

/**
 * ???FUNCTION???
 *
 * @param various $pid - Mandatory parameter:                   ???PARAMETER???
 * @param string  $sd  - Optional parameter (default = ''):     ???PARAMETER???
 * @param string  $ed  - Optional parameter (default = ''):     ???PARAMETER???
 * @param boolean $ret - Optional parameter (default = false):  ???PARAMETER???
 */
function warnTimeRecordingDates($pid, $sd='', $ed='', $ret=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	//echo('<b>warnTimeRecordingDates</b>($pid='.$pid.', $sd='.$sd.', $ed='.$ed);

	$GLOBALS['pers_entry_date']='00000000';
	$GLOBALS['pers_exit_date']='99999999';
	$GLOBALS['pers_name2']='???';

	$rt='';
	$pid=(int)$pid;
	if($pid)
	{
		$sdate=substr((string)$sd,0,8);
		$edate=substr((string)$ed,0,8);
		if(!strlen((string)$edate))
			$edate=$sdate;
		$pda=get_persondata($pid);
//fxDebug($pda,'$pda: $pid='.$pid);

		if(($pda['maeintrittsdatum'] !== NULL) && strlen((string)$pda['maeintrittsdatum']))
			$GLOBALS['pers_entry_date']=substr((string)$pda['maeintrittsdatum'],0,8);
		if(($pda['maaustrittsdatum'] !== NULL) && strlen((string)$pda['maaustrittsdatum']))
			$GLOBALS['pers_exit_date']=substr((string)$pda['maaustrittsdatum'],0,8);
		$GLOBALS['pers_name2']=$pda['name2'];

		$disp=false;
		$adate='';
		if(strlen((string)$sdate) || strlen((string)$edate))
		{
			if(($sdate == $edate) && (($sdate < $GLOBALS['pers_entry_date']) || ($sdate > $GLOBALS['pers_exit_date'])))
				$disp=true;
			else if(($sdate != $edate) && ($GLOBALS['pers_exit_date'] < $edate) || ($GLOBALS['pers_entry_date'] > $sdate))
				$disp=true;
			$adate=get_entry($sdate,'datum');
			if($edate != $sdate)
				$adate .= '-'.get_entry($edate,'datum');
		}
//$disp=true;

		// No time recording for &quot;$var1&quot; on the $var2 allowed, because it is outside the work contract timeframe.
		$txt=meldung(3279, true, '<b class="s4">'.$GLOBALS['pers_name2'].'</b>', '<b id="warn_pers_date" class="s4">'.$adate.'</b>');

		$msg  = '<table border=0 cellpadding=0 cellspacing=4>';
		$msg .= '	<tr>';
		$msg .= '		<td rowspan="3" nowrap><b style="font-size:64pt;line-height:.75;text-shadow:4px 4px 4px rgba(0,0,0, 0.5);">!&nbsp;</b></td>';
		$msg .= '		<td valign="top" colspan="2"><font class="s4">'.$txt.'</font><br /><br /></td>';
		$msg .= '	</tr>';
		$msg .= '	<tr>';
		$msg .= '		<td valign="top" nowrap><font class="s4">&rarr;&nbsp;&nbsp;'.get_text(54, '20').':&nbsp;&nbsp;</td>';
		$msg .= '		<td width="100%" valign="top"><b class="s4">'.get_entry($GLOBALS['pers_entry_date'], 'datum').'</b></td>';
		$msg .= '	</tr>';
		if($GLOBALS['pers_exit_date'] != '99999999')
		{
			$msg .= '	<tr>';
			$msg .= '		<td valign="top" nowrap><font class="s4">&larr;&nbsp;&nbsp;'.get_text(55, '20').':&nbsp;&nbsp;</td>';
			$msg .= '		<td width="100%" valign="top"><b class="s4">'.get_entry($GLOBALS['pers_exit_date'], 'datum').'</b></td>';
			$msg .= '	</tr>';
		}
		$msg .= '</table>';
		$msg .= '<div id="warn_pers_dates" style="position:absolute;display:none;">'.$pid.'|'.$pda['name2'].'|'.$GLOBALS['pers_entry_date'].'|'.$GLOBALS['pers_exit_date'].'</div>';

		$rt=fxp_warn($msg, 'warn_pers_div', '', $disp, true);
	}

	if($ret)
		return($rt);
	echo($rt);
}

/**
 * Insert missing information for a person's contract data, PP/SP, vacation days or start carry-overs
 *
 * @param integer $pid     - Mandatory parameter:                             Person Id
 * @param integer $cid     - Optional parameter (default = 0):                Client Id
 * @param integer $ptype   - Optional parameter (default = FXP_PT_EMPLOYEE):  Person type
 * @param boolean $execute - Optional parameter (default = false):            Execute necessary changes (true) or return sql array
 *
 * @return Array of necessary sql's (if execute is false) or error code of executed sql's (if execute is true)
 */
function fxf_checkPersonContractData($pid, $cid=0, $ptype=FXP_PT_EMPLOYEE, $execute=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$pid=(int)$pid;
	$cid=(int)$cid;
	if(!$cid)
		$cid=$GLOBALS['fxpglobals']['client'];

	// Create SQL array
	$arr=array();
	if($pid && $cid)
	{
		$ac=0;

		// Check if person has contract data
		$cdi=0;
		$ped=substr((string)$GLOBALS['date'],0,6).'01000000';
		$sql="SELECT madaten_id, maeintrittsdatum FROM madaten WHERE personen_id=".$pid." AND mandanten_id=".$cid;
		$dba=db_values($sql);
		if(fxIsArray($dba))
		{
			$cdi=(int)$dba['madaten_id'];
			if(strlen((string)$dba['maeintrittsdatum']) > 7)
				$ped=substr((string)$dba['maeintrittsdatum'],0,8).'000000';
		}
//fxDebug(array('$cdi'=>$cdi, '$ped'=>$ped),$sql, 0);

		// Check if person has active PP/SP
		$psi=0;
		$ppp=0.0;
		$psp=0.0;
		$sql="SELECT artikel_id, ek, vk FROM artikel WHERE personen_id=".$pid." AND mandanten_id=".$cid." AND (sachmittel_id=0 OR sachmittel_id IS NULL) AND (archiv=0 OR archiv IS NULL)";	// Old entries have "archiv" set
		$dba=db_values($sql);
		if(fxIsArray($dba))
		{
			$psi=(int)$dba['artikel_id'];
			$ppp=get_double($dba['ek']);
			$psp=get_double($dba['vk']);
		}
//fxDebug(array('$psi'=>$psi, '$ppp'=>$ppp, '$psp'=>$psp),$sql, 0);

		// Check if person has yearly ($yvd) and remaining vacation days ($rvd) for the entry year
		$vdi=0;
		$yvd=0.0;
		$rvd=0.0;
		$sql="SELECT artikel_id, urlaubsanspruch, resturlaub FROM artikel WHERE personen_id=".$pid." AND mandanten_id=".$cid." AND sachmittel_id=-1039 AND soll_beginn LIKE '".substr((string)$ped,0,4)."%' AND soll_ende LIKE '".substr((string)$ped,0,4)."%' AND (archiv=0 OR archiv IS NULL)";
		$dba=db_values($sql);
		if(fxIsArray($dba))
		{
			$vdi=(int)$dba['artikel_id'];
			// Only employees can have vacations
			if($ptype == FXP_PT_EMPLOYEE)
			{
				$yvd=get_double($dba['urlaubsanspruch']);
				$rvd=get_double($dba['resturlaub']);
			}
		}
//fxDebug(array('$vdi'=>$vdi, '$yvd'=>$yvd, '$rvd'=>$rvd),$sql, 0);

		// Get default values from last changed contract data if one of the checks fails amd insert missing data
		if(!$cdi || !$psi || !$vdi)
		{
			$sql  = "SELECT m.personen_id, m.firm_adress, m.religion, m.mobilbereich, m.wochenarbeitstd, m.arbeitstage, m.soll_beginn, m.pause, m.montag, m.dienstag, m.mittwoch, m.donnerstag, m.freitag, m.samstag, m.sonntag,";
			$sql .= " a.ek, a.vk";
			$sql .= " FROM personen p, madaten m LEFT JOIN artikel a ON m.personen_id=a.personen_id AND m.mandanten_id=a.mandanten_id";
			$sql .= " WHERE p.mandanten_id=".$cid." AND m.personen_id=p.personen_id AND m.mandanten_id=".$cid." AND (a.sachmittel_id=0 OR a.sachmittel_id IS NULL) AND (a.archiv=0 OR a.archiv IS NULL)";
			$sql .= " ORDER BY m.zeitstempel DESC";
			$dva=db_values($sql);
			if(fxIsArray($dva))
			{
				if(!strlen((string)$dva['religion']))
					$dva['religion']=2122;	// other
				if(!strlen((string)$dva['mobilbereich']))
					$dva['mobilbereich']=0;
				if(!strlen((string)$dva['soll_beginn']))
					$dva['soll_beginn']="00000000080000";

				if(!$psi)
				{
					$ppp=get_double($dva['ek']);
					$psp=get_double($dva['vk']);
				}

				// Only employees can have vacations
				if(!$vdi && ($ptype == FXP_PT_EMPLOYEE))
				{
					$sql="SELECT urlaubsanspruch FROM artikel WHERE personen_id=".$dva['personen_id']." AND mandanten_id=".$cid." AND sachmittel_id=-1039 AND (archiv=0 OR archiv IS NULL) ORDER BY soll_beginn DESC";
					$yvd=get_double(db_value($sql));
				}
			}
			else
			{
				$dva=array(
					'firm_adress'		=> 0,
					'religion'			=> 2122,	// other
					'mobilbereich'		=> 0,
					'wochenarbeitsstd'	=> 40.0,
					'arbeitstage'		=> 5,
					'soll_beginn'		=> "00000000080000",
					'pause'				=> 3600,
					'montag'			=> 1,
					'dienstag'			=> 1,
					'mittwoch'			=> 1,
					'donnerstag'		=> 1,
					'freitag'			=> 1,
					'samstag'			=> 0,
					'sonntag'			=> 0
				);

				// Only employees can have vacations
				if(!$vdi && ($ptype == FXP_PT_EMPLOYEE))
					$yvd=24.0;
			}
//fxDebug($dva,$sql, 0);

			// Create new contract data
			if(!$cdi)
			{
				$cdi=id_max('madaten', 'madaten_id');
				$sql  = "INSERT INTO madaten";
				$sql .= " (mandanten_id, firm_adress, personen_id, madaten_id, geburtsname, geburtsdatum, religion, mobilbereich, schwerbehindert, maeintrittsdatum, maaustrittsdatum, blz, kontonummer, iban, bic, wochenarbeitstd, arbeitstage, soll_beginn, zeiterfassung, gleitzeit, montag, dienstag, mittwoch, donnerstag, freitag, samstag, sonntag, maxstdwoche, maxstdtag, zeitraumvon, zeitraumbis, pause, anmerkung)";
				$sql .= " VALUES";
				$sql .= " (".$cid.", ".(int)$dva['firm_adress'].", ".$pid.", ".$cdi.", NULL, NULL, ".(int)$dva['religion'].", ".(int)$dva['mobilbereich'].", 0, '".$ped."', NULL, NULL, NULL, NULL, NULL, ".get_double($dva['wochenarbeitstd']).", ".(int)$dva['arbeitstage'].", '".$dva['soll_beginn']."', 1, 1, ".(int)$dva['montag'].", ".(int)$dva['dienstag'].", ".(int)$dva['mittwoch'].", ".(int)$dva['donnerstag'].", ".(int)$dva['freitag'].", ".(int)$dva['samstag'].", ".(int)$dva['sonntag'].", NULL, NULL, NULL, NULL, ".(int)$dva['pause'].", NULL)";
				$arr[$ac++]=array('sql'=>$sql, 'typ'=>'insert');
			}

			// Create new PP/SP
			if(!$psi)
			{
				$psi=id_max('artikel', 'artikel_id');
				$sql  = "INSERT INTO artikel";
				$sql .= " (mandanten_id, artikel_id, personen_id, sachmittel_id, ek, vk, mwst_satz, soll_beginn, soll_ende, urlaubsanspruch, resturlaub, archiv)";
				$sql .= " VALUES";
				$sql .= " (".$cid.", ".$psi.", ".$pid.", NULL, ".$ppp.", ".$psp.", NULL, '".$ped."', NULL, NULL, NULL, 0)";
				$arr[$ac++]=array('sql'=>$sql, 'typ'=>'insert');
			}

			// Create new vacation days
			if(!$vdi)
			{
				$vdi=id_max('artikel', 'artikel_id');
				$sql  = "INSERT INTO artikel";
				$sql .= " (mandanten_id, artikel_id, personen_id, sachmittel_id, ek, vk, mwst_satz, soll_beginn, soll_ende, urlaubsanspruch, resturlaub, archiv)";
				$sql .= " VALUES";
				$sql .= " (".$cid.", ".$vdi.", ".$pid.", -1039, NULL, NULL, NULL, '".substr((string)$ped,0,4)."0101000000', '".substr((string)$ped,0,4)."1231240000', ".$yvd.", ".$rvd.", 0)";
				$arr[$ac++]=array('sql'=>$sql, 'typ'=>'insert');
			}
		}

		// Check if person has a start carry-over value for time registrations
		$sql="SELECT zeit_id FROM zeitdaten WHERE zeitart=".FXP_TE_CARRY_START." AND personen_id=".$pid." AND mandanten_id=".$cid;
		$tri=(int)db_value($sql);
//fxDebug(array('$tri'=>$tri),$sql, 0);

		// Create new start carry-over
		if(!$tri)
		{
			// ...Date must be entry date's previous month, i.e. for January the start carry-over is set to December last year, for February to January this year, for March to Fabruary this year, etc.
			$yco=(int)substr((string)$ped,0,4);		// Year
			$mco=(int)substr((string)$ped,4,2)-1;	// Previous month
			if(!$mco)						// Special: Previous year for December
			{
				$mco=12;
				$yco--;
			}
			$cdt=$yco.substr('00'.$mco,-2).'00000000';

			$tri=id_max('zeitdaten', 'zeit_id');
			$ta=array(
				'mandanten_id'			=> $cid,
				'zuordnungs_mid'		=> $cid,
				'personen_id'			=> $pid,
				'zeit_id'				=> $tri,
				'projekt_id'			=> 0,
				'mutterprojekt_id'		=> 0,
				'rechnungs_id'			=> 0,
				'positions_id'			=> 0,
				'zkunde'				=> 0,

				'soll_beginn'			=> 'NULL',
				'soll_ende'				=> 'NULL',
				'ist_beginn'			=> "'".$cdt."'",
				'ist_ende'				=> "'".$cdt."'",
				'kw'					=> 0,
				'pause'					=> 0,
				'zeitaufwand'			=> 0,

				'kosten_ek'				=> 0.0,
				'kosten_vk'				=> 0.0,
				'kosten_mvk'			=> 0.0,
				'mwst_satz'				=> 0.0,

				'zeitart'				=> FXP_TE_CARRY_START,
				'zeitgruppe'			=> 'NULL',
				'freigabe'				=> 0,
				'offline_zd'			=> 'NULL'
			);
			$cols="";
			$vals="";
			foreach($ta as $col => $val)
			{
				if(strlen((string)$cols))
				{
					$cols .= ", ";
					$vals .= ", ";
				}
				$cols .= $col;
				$vals .= $val;
			}
			$sql='INSERT INTO zeitdaten ('.$cols.') VALUES ('.$vals.')';
			$arr[$ac++]=array('sql'=>$sql, 'typ'=>'insert');
		}
	}

	// Return array?
	if(!$execute)
		return $arr;

	// Execute SQL's?
	$eerr=0;
	if(sizeof($arr))
	{
		foreach($arr as $a)
		{
			$eerr=db_x($a['sql']);
//fxDebug($eerr,$sql, 0);
			if($eerr)
				break;
		}
	}

	return $eerr;
}

/**
 * Vacation calculation for a person
 *
 * @param integer $pid   - Mandatory parameter:                Person id
 * @param string  $sdate - Optional parameter (default = ''):  Start date (if empty person's entry date is used)
 * @param string  $edate - Optional parameter (default = ''):  End date (if empty person's exit date is used or current year)
 *
 * @return Array of this persons vacation calculation
 */
function fxf_getVacCalc($pid, $sdate='', $edate='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$pid=(int)$pid;
	if(!$pid)
		return false;

	$rdebug=-1;
//$rdebug=74;
	$ra=array();

	// Get person's contract datas (entry date, exit date, workdays, ...)
	$da=getPersonWorkdata($pid, '', '', 'st', false, true);
if($rdebug == $pid) fxDebug($da,'$da: $pid='.$pid, 0);
	if(!fxIsArray($da) || !fxIsArray($da['*']))
		return false;
if($rdebug == $pid) echo('<b>fxf_getVacCalc</b>($pid='.$pid.', $sdate='.$sdate.', $edate='.$edate.') &rarr; <b class=blue>'.$da['*']['n2'].'</b><br />');

	// Yearly vacation sums in database (defined: archiv=0)
	$mnd='99999999';
	$vsa=array();
	$ssy=0;
	$sly=0;
	$sql  = "SELECT artikel_id, soll_beginn, soll_ende, urlaubsanspruch, resturlaub, archiv";
	$sql .= " FROM artikel";
	$sql .= " WHERE personen_id=".$pid." AND sachmittel_id=-1039 AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$sql .= " ORDER BY soll_beginn";
	$tmp=db_values($sql,'*');
	if(fxIsArray($tmp))
	{
//fxDebug($tmp,$sql, 0);
		foreach($tmp as $t)
		{
			$sly=(int)substr((string)$t['soll_beginn'],0,4);
			if(!$ssy)
			{
				$ssy=$sly;
				$mnd=$sly.'0101';
			}
			$sc=1;
			if(!$t['archiv'])
				$sc=0;
			$vsa[$sly]=array('id'=>$t['artikel_id'], 'ua'=>$t['urlaubsanspruch'], 'ru'=>$t['resturlaub'], 'sc'=>$sc, 'os'=>$sc);
		}
	}
	unset($tmp);
//fxDebug($vsa,'$vsa: '.$ssy.'-'.$sly.' &rarr; $mnd='.$mnd, 0);

	// First vacation entry
	$fvy=0;
	$sql="SELECT startzeit FROM termine WHERE personen_id=".$pid." AND terminart=".FXP_AT_VACATION." AND mandanten_id=".$GLOBALS['fxpglobals']['client']."  ORDER BY startzeit";
	$fvs=db_value($sql);
	if(strlen((string)$fvs))
	{
		$fvs=substr((string)$fvs,0,8);
		$fvy=(int)substr((string)$fvs,0,4);
		if($fvs < $mnd)
			$mnd=$fvs;
	}
if($rdebug == $pid) echo('&rarr; $fvs='.$fvs.' ($fvy='.$fvy.') &rarr; $mdn='.$mnd.'<br />');

	// No minimun date means no sum entries and no vacation entry, return empty array
	if($mnd == '99999999')
		return $ra;

	// Person's entry/exit contract date
	$pedate=$da['*']['es'];
	$pxdate=$da['*']['ex'];
	if(!strlen((string)$pedate) && !strlen((string)$pxdate))
		return $ra;
	$peyear=(int)substr((string)$pedate,0,4);
	$pxyear=(int)substr((string)$pxdate,0,4);
if($rdebug == $pid) echo('&rarr; $pedate='.$pedate.' ($peyear='.$peyear.'), $pxdate='.$pxdate.' ($pxyear='.$pxyear.')<br />');

	// Do calculated entries exist outside person's employment period?
	if(sizeof($vsa))
	{
		$dsa=array();
		foreach($vsa as $y => $a)
		{
			if($a['sc'] && $a['id'] && ($y < $peyear) || ($y > $pxyear))
				$dsa[$y]=$a['id'];
		}
if($rdebug == $pid) fxDebug($dsa,'$dsa: Calculated entries outside person\'s employment period ('.$peyear.' - '.$pxyear.')', 0);
		if(sizeof($dsa))
		{
			// Start Transaction
			if(!$GLOBALS['db_in_trans']);
				db_rollback('start');
			$derr=false;

			foreach($dsa as $y => $id)
			{
				$sql=db_array2sql(array('mode'=>"delete", 'table'=>"artikel", 'where'=>"artikel_id=".$id." AND mandanten_id=".$GLOBALS['fxpglobals']['client']));
				$derr=db_x($sql);
if($rdebug == $pid) echo('&quot;'.$sql.'&quot;<br />&rarr; '.($verr?'<b style="color:#e1001a;">KO</b>':'<b style="color:#009f6b;">OK</b>').'<hr size=1 />');
				if($derr)
					break;
			}

			// Transaction Rollback?
			if($GLOBALS['db_in_trans'])
			{
				if($derr)
					db_rollback('rollback');
				else
					db_rollback('commit');
			}
		}
	}

	// Start/End date
	if(strlen((string)$sdate))
		$sdate=substr('000'.substr((string)$sdate,0,4),-4).'0101';
	if(!strlen((string)$sdate) || ($sdate < $pedate))
		$sdate=$pedate;
	if(strlen((string)$edate))
	{
		$edate=substr('000'.substr((string)$edate,0,4),-4).'1231';
		if($edate < $sdate)
			$edate=substr((string)$sdate,0,4).'1231';
	}
	if(!strlen((string)$edate) || ($edate > $pxdate))
		$edate=$pxdate;
	if(substr((string)$edate,0,4) == '9999')
		$edate=substr((string)$GLOBALS['date'],0,4).'1231';
	$syear=(int)substr((string)$sdate,0,4);
	$eyear=(int)substr((string)$edate,0,4);
if($rdebug == $pid) echo('&rarr; $sdate='.$sdate.' ($syear='.$syear.'), $edate='.$edate.' ($eyear='.$eyear.')<br />');

	// Timeframe out of person's employment period, return empty array
	if(($syear > $pxyear) || ($eyear < $peyear))
		return $ra;

	// Vacation entries
	$vya=array();
	$vsy=0;
	$vly=0;
	$sql  = "SELECT termin_id, startzeit, endzeit, urlaub";
	$sql .= " FROM termine";
	$sql .= " WHERE personen_id=".$pid." AND terminart=".FXP_AT_VACATION." AND mandanten_id=".$GLOBALS['fxpglobals']['client']." AND startzeit<='".$edate."240000' AND endzeit>='".$sdate."000000'";
	$sql .= " ORDER BY startzeit";
	$tmp=db_values($sql,'*');
	if(fxIsArray($tmp))
	{
//fxDebug($tmp,$sql, 0);
		foreach($tmp as $t)
		{
			if(!$vsy)
				$vsy=(int)substr((string)$t['startzeit'],0,4);
			$vly=(int)substr((string)$t['endzeit'],0,4);
			$vya[$t['termin_id']]=array('st'=>$t['startzeit'], 'et'=>$t['endzeit'], 'vd'=>$t['urlaub']);
		}
	}
	unset($tmp);
if($rdebug == $pid) fxDebug($vya,'$vya: ($vsy='.$vsy.' - $vly='.$vly.')', 0);

	// No defined data for entry year?
	if(!isset($vsa[$peyear]) || $vsa[$peyear]['sc'])
	{
		// ...No vacations taken, return empty array
		if(!isset($vsa[$peyear]) && !sizeof($vya))
			return $ra;

		if(!$sly || ($peyear > $sly))
			$sly=$peyear;
		if(!$ssy || ($peyear < $ssy))
			$ssy=$peyear;

		// ...Calculated entry exists, change to defined
		if(isset($vsa[$peyear]) && $vsa[$peyear]['sc'])
			$vsa[$peyear]['sc']=0;
		else
		{
			$sql  = "SELECT urlaubsanspruch";
			$sql .= " FROM artikel";
			$sql .= " WHERE personen_id=".$pid." AND mandanten_id=".$GLOBALS['fxpglobals']['client']." AND archiv=0 AND urlaubsanspruch IS NOT NULL";
			$sql .= " ORDER BY soll_beginn";
			$ua=db_value($sql);
//fxDebug($ua,$sql, 0);
			$sc=-2;

			// None found, return empty array!
			if(!strlen((string)$ua))
				return $ra;

			// Create new definition entry
			$vsa[$peyear]=array('id'=>0, 'ua'=>(double)$ua, 'ru'=>0.0, 'sc'=>0);
			ksort($vsa);
		}
	}
if($rdebug == $pid) fxDebug($vsa,'$vsa: ($ssy='.$ssy.' - $sly='.$sly.')', 0);

	// No entry for start year, calculate everything
	if(!isset($vsa[$syear]) && ($syear >= $peyear))
	{
		$sdate=substr('000'.$peyear,-4).'0101';
		$syear=(int)substr((string)$sdate,0,4);
if($rdebug == $pid) echo('&rarr; No entry for start year &rarr; NEW: $sdate='.$sdate.' ($syear='.$syear.')<br />');
	}

	// Get all holidays
	if(!fxIsArray($GLOBALS['__hda']))
	{
		$GLOBALS['__hda']=get_feiertage('','', true);
//fxDebug($GLOBALS['__hda'],'$GLOBALS[\'__hda\']: All holidays', 0);
	}

	// Get texts for tooltip
	if(!fxIsArray($GLOBALS['__hla']))
	{
		$GLOBALS['__hla']=array(
			'ytp' => explode('|', meldung(1887,false)),	// No defined start value exists|Defined start value|Calculated intermediate value|Calculated new value
			'htx' => explode('|', meldung(3397,false)),	// Vacation carryover (from previous year)|Defined vacation per year (for whole year)|Missing months (entry/exit date)|Vacation taken|Remaining vacation (in current year)|Vacation carryover (for next year)
			'hhd' => explode('|', meldung(3398,false)),	// Vacation Date|Calculation Detail: Days - Free - Holidays|Day(s)|Click to display the remaining $var1 vacation entries in a new popup
			'hms' => explode('|', meldung(3399,false)),	// The new subtotal record was successfully created!|The new subtotal record could not be created!|The subtotal record was successfully changed.|The subtotal record could not be changed!|The vacation day calculation was successfully changed.|The vacation day calculation could not be changed!
			'mon' => meldung(918,false)					// Month(s)
		);
//fxDebug($GLOBALS['__hla'],'__hla', 0);
	};

	// Employment months
	$cm=0;
	if($syear > $peyear)
	{
		$cm += 12-(int)substr((string)$pedate,4,2);	// e.g. '20220301' = 3		-> 12-3		-> 9
		if(substr((string)$pedate,6,2) == '01')		// e.g. '20220301' = '01'	-> +1		-> 10
			$cm++;

		$cy=$peyear+1;
		while($cy < $syear)
		{
			$cm += 12;
			$cy++;
		}
	}
if($rdebug == $pid) echo('&rarr; Employment months for start year: $cm='.$cm.'<br />');

	// Tooltip header with name, person picture + entry/leave dates
	$th  = fxf_getPersTooltipHeader($pid);
	$th .= '<br /><br />';

	// Cycle through all years
//$html='';
	$cy=$syear;
	$ua=0.0;
	$ru=0.0;
	$vtm=5;
	$wa=array(0=>'so','mo','di','mi','do','fr','sa');
	$vr=array();	// Remember adjusted vacation id's
	$sr=array();	// Total vacation calculation per id
	while($cy <= $eyear)
	{
		// Only if in timeframe of person's employment period
		if(($cy >= $peyear) && ($cy <= $pxyear))
		{
//$html .= '<div style="padding:4px 8px;background:#006b9f;color:#fff;font-weight:bold;">'.$cy.'</div>'.$GLOBALS['nl'];
//$html .= '<div style="margin-bottom:4px;padding:4px 8px;border:1px solid #888;">'.$GLOBALS['nl'];
			$vd=0.0;
			$ycm=0;
			$yd=fxf_mdays($cy);

			// Remember last year's remaining vacation
			$ou=$ru;

			$dbss=array();	// Sum: Suggested calculation corrections
			$dbsn=array();	// Sum: Necessary calculation corrections
			$dbvn=array();	// Vac: Necessary calculation corrections

			// Defined/Calculated start values
			if(fxIsArray($vsa[$cy]))
			{
				// ...Defined
				if(!$vsa[$cy]['sc'])
				{
					$ytp=1;	// Defined start value
					$ua=$vsa[$cy]['ua'];
					$ru=$vsa[$cy]['ru'];
					if($cy == $peyear)
						$ou=$ru;
//$html .= '<div style="padding:4px 8px;background:#009f6b;"><b>Set values from db</b>: $ua='.$ua.' (Yearly vacation), $ru='.$ru.' (Carry vacation from last year)</div>';
				}
				// ...Calculated
				else
				{
					$ytp=2;	// Calculated intermediate value
					if($cy == $syear)
					{
						$ua=$vsa[$cy]['ua'];
						$ru=$vsa[$cy]['ru'];
						$ou=$ru;
					}
//$html .= '<div style="padding:4px 8px;background:#aaa;"><b>Calculated values from db</b>: $ua='.$vsa[$cy]['ua'].' (Yearly vacation), $ru='.$vsa[$cy]['ru'].' (Carry vacation from last year)</div>';
				}
				if(!$vsa[$cy]['os'])
					$otp=1;
				else
					$otp=2;
			}
			// Missing
			else
			{
				$ytp=3;	// Calculated new value
				$otp=3;
				$dbsn[]=array('in'=>$pid.': '.$ycs.'-'.$yce, 'md'=>"insert", 'ua'=>$ua, 'ru'=>$ru, 'sc'=>-1, 'st'=>"");
//$html .= '<div style="padding:4px 8px;background:#e1001a;"><b>Definition for this year is missing!</b> Using $ua='.$ua.' (Yearly vacation), $ru='.$ru.' (Carry vacation from last year)</div>';
			}

			// Timeframe
			$vtc=0;
			$vtr='';
			$ycs='';
			$yce='';
			foreach($da as $dsd => $dsa)
			{
//echo('Contract data: '.$dsd.' ('.$dsa['sd'].' - '.$dsa['ed'].') &rarr; <b>cy='.$cy.'</b> (pedate='.$pedate.' - pxdate='.$pxdate.')<br />');
				if(($dsd != '*') && ($dsd != '00000000') && ($dsa['ed'] >= $cy.'0101') && ($dsa['ed'] >= $pedate) && ($dsa['sd'] <= $cy.'1231') && ($dsa['ed'] <= $pxdate))
				{
					$ysd=$cy.'0101';
					if($dsa['sd'] > $ysd)
						$ysd=$dsa['sd'];
					if(!$ycs || ($ysd < $ycs))
						$ycs=$ysd;
					$yed=$cy.'1231';
					if($dsa['ed'] < $yed)
						$yed=$dsa['ed'];
					if(!$yce || ($yed > $yce))
						$yce=$yed;
					// ...Calculate full months
					if((substr((string)$ysd,4,4) == '0101') && (substr((string)$yed,4,4) == '1231'))
						$ycm=12;
					else
					{
						$cms=(int)substr((string)$ysd,4,2);
						$cme=(int)substr((string)$yed,4,2);
						for($cmc=$cms; $cmc<=$cme; $cmc++)
						{
							$cmf=$cy.substr('0'.$cmc,-2).'01';
							$cml=substr((string)$cmf,0,6).$yd[$cmc];
							if(($ysd <= $cmf) && ($yed >= $cml))
								$ycm++;
						}
					}
					$cm += $ycm;
//$html .= '&rarr; <b>Contract data</b>: '.$dsd.' ('.$dsa['sd'].' - '.$dsa['ed'].') &rarr; <b>'.$ysd.' - '.$yed.'</b> &larr; [st='.$dsa['st'].' (Workhours per day), zm='.$dsa['zm'].' (AM end time)], so-sa='.$dsa['so'].$dsa['mo'].$dsa['di'].$dsa['mi'].$dsa['do'].$dsa['fr'].$dsa['sa'].' (Workdays Sunday-Saturday), ycm='.$ycm.' / cm='.$cm.' (Full months in company)</b><hr />';

					// Cycle through all vacations
					foreach($vya as $vid => $va)
					{
						// Valid, in timeframe
						if((substr((string)$va['et'],0,8) >= $ysd) && (substr((string)$va['st'],0,8) <= $yed))
						{
//$html .= '&rarr; <b>Appointment</b> '.$vid.': ('.$va['st'].' - '.$va['et'].') &larr; [vd='.$va['vd'].' (Vacation days)</b><br />';
							$vsd=substr((string)$va['st'],0,8);
							$ved=substr((string)$va['et'],0,8);
							$vcd=$vsd;
							$dsd='';
							$ded='';
							$vvs='';
							$vve='';
							$hds='';
							$cwd=getWeekday($vcd);	// Weekday: 0=Sunday, 1=Monday, 6=Saturday
							$vdy=(int)substr((string)$vcd,6,2);	// Day
							$vmo=(int)substr((string)$vcd,4,2);	// Month
							$vyr=(int)substr((string)$vcd,0,4);	// Year
							$dc=0.0;	// Day counter
							$wc=0.0;	// Workday counter
							$fc=0.0;	// Free day counter
							$vc=0.0;	// Vacation counter (for this year)
							$hd='';		// Half day vacation
							while($vcd <= $ved)
							{
								if(($vcd >= $cy.'0101') && ($vcd <= $cy.'1231'))	// This year?
								{
									if(!strlen((string)$vvs))
										$vvs=$vcd;
									$vve=$vcd;
								}

								// Invalid, i.e. not in current contract data block
								if(($vcd < $ysd) || ($vcd > $yed))
								{
//$html .= '&rarr; <b class="red">Out of contract frame</b>: '.$vcd.' ('.$ysd.' - '.$yed.')<br />';
								}
								else
								{
									if(!strlen($dsd))
										$dsd=$vcd;
									$ded=$vcd;

									$dc += 1.0;

									// Workday?
									$wd=0.0;
									if($dsa[$wa[$cwd]])	// Yes
									{
										$wd=1.0;
										// Holiday?
										if(fxIsArray($GLOBALS['__hda'][$vyr]) && fxIsArray($GLOBALS['__hda'][$vyr][$vcd]))
										{
											$hcl='grey';
											if($GLOBALS['__hda'][$vyr][$vcd]['typ'])
												$hcl='green';
//$html .= '&rarr; <b class="'.$hcl.'">Holiday</b>: '.$vcd.' = '.$GLOBALS['__hda'][$vyr][$vcd]['typ'].' ('.$GLOBALS['__hda'][$vyr][$vcd]['name'].')<br />';
											$wd=max(0.0, $wd-$GLOBALS['__hda'][$vyr][$vcd]['typ']);
											if(($vcd >= $cy.'0101') && ($vcd <= $cy.'1231') && ($GLOBALS['__hda'][$vyr][$vcd]['typ']))	// This year?
												$hds .= ' - '.get_entry($GLOBALS['__hda'][$vyr][$vcd]['typ'],'decimal').' d ['.$GLOBALS['__hda'][$vyr][$vcd]['name'].' ('.get_entry($vcd,'mdate').')]';
										}
										// Half day vacation?
										else if(($vsd == $ved) && (($va['vd'] < 1.0) || ((substr((string)$va['st'],8,6) >= $dsa['zm']) || (substr((string)$va['et'],8,6) <= $dsa['zn']))))
										{
//$html .= '&rarr; <b class="blue">Half vacation day</b>: '.$vcd.' ('.substr((string)$va['st'],8,6).' - '.substr((string)$va['et'],8,6).')<br />';
											$wd=0.5;
											$dc -= 0.5;
											$hd='&nbsp;<i class=blue>[½]</i>';
										}
										$wc += $wd;
										if(($vcd >= $cy.'0101') && ($vcd <= $cy.'1231'))	// This year?
											$vc += $wd;
									}
									else
									{
//$html .= '&rarr; <b class="grey">No workday</b>: '.$vcd.'<br />';
										if(($vcd >= $cy.'0101') && ($vcd <= $cy.'1231'))	// This year?
											$fc += 1.0;
									}
									if($wd)
									{
//$html .= '&nbsp;&nbsp;'.$vcd.' (-'.$ved.'): VD '.$dc.' &rarr; $cwd='.$cwd.' (Weekday), $wd='.$wd.' (Workday), $wc='.$wc.' (Workday counter), $vc='.$vc.' (Valid vacation counter for this year)<br />';
									}
								}
								$cwd++;
								if($cwd > 6)
									$cwd=0;
								$vdy++;
								if($vdy > $yd[$vmo])
								{
									$vdy=1;
									$vmo++;
									if($vmo > 12)
									{
										$vmo=1;
										$vyr++;
										$yd=fxf_mdays($vyr);
									}
								}
								$vcd=substr('000'.$vyr,-4).substr('0'.$vmo,-2).substr('0'.$vdy,-2);
							}
							$vd += $vc;
//$html .= '<div style="padding:4px 8px;background:#ddd;border-top:1px solid #000;"><b>&sum; '.$vsd.' - '.$ved.'</b>: $wc='.$wc.' (Part: Workday counter), $vc='.$vc.' (Part: Valid vacation counter for this year), $vd='.$vd.' (Total: Valid vacation counter for this year)</div>';
							$vts=get_entry($dsd,'mdate');
							if($ded != $dsd)
								$vts .= ' - '.get_entry($ded,'mdate');
							$vtc++;
							$bdt='';
							if(strlen((string)$vtr))
								$bdt='border-top:1px solid #ccc;';
							$trd='none';
							if($vtc <= $vtm)
								$trd='table-row';
							$vtr .= '<tr style=display:'.$trd.'>';
							// ...Vacation date
							$vtr .= '<td style=\''.$bdt.'border-right:1px solid #ccc;\'>'.$vts.$hd.'&nbsp;</td>';
							// ...Calculation detail: days - free - holidays
							$vtr .= '<td style=\''.$bdt.'border-right:1px solid #ccc;\'>&nbsp;'.get_entry($dc,'decimal').' d - '.get_entry($fc,'decimal').' d - ';
							if(strlen((string)$hds))
								$vtr .= substr((string)$hds,3);
							else
								$vtr .= get_entry(0,'decimal').' d';
							$vtr .= '&nbsp;</td>';
							// ...Day(s)
							if(!isset($sr[$vid]))
							{
								$ddd=false;
								$sr[$vid]=0;
							}
							else
								$ddd=true;
							$sr[$vid] += $wc;
							$vtr .= '<td align=right style=\''.$bdt.'\'>&nbsp;';
							if(($ddd || ($sr[$vid] != $va['vd'])) && !isset($vr[$vid]))
							{
								if(($ded == $ved) && ($sr[$vid] != $va['vd']))
								{
									$ddc='red';
									$dbvn[$vid]=array('ud'=>$sr[$vid], 'st'=>"");
//$html .= '<div style="padding:4px 8px;background:#e1001a;"><b>Calculated values do not match db:</b> Vacation days (CALC = '.$sr[$vid].' / DB = '.$va['vd'].')</div>';
								}
								else
									$ddc='lightergrey';
								$vtr .= '<i class='.$ddc.'>(&sum; '.get_entry($va['vd'],'decimal').')</i> ';
							}
							$vtr .= get_entry($vc,'decimal').' d</td>';
							$vtr .= '</tr>';
						}
					}
				}
			}

			// Remaining vacations
			if(($ycs > $cy.'0101') || ($yce < $cy.'1231'))
			{
				if(($pxyear == $cy) && ($yce > $cy.'0701') && ($cm > 5))	// Full vacation if exit date in this year after July 1st and at least 6 month in company
				{
					$cua=$ua;
//$html .= '&rarr; Full vacation granted &rarr; $cua='.$cua.'<br />';
				}
				else if($ycm)
				{
					$cua=round(($ua*$ycm)/12);
//$html .= '&rarr; ($ua='.$ua.' * $ycm='.$ycm.') / 12 &rarr; $cua='.$cua.'<br />';
				}
				else
				{
					$cua=0.0;
//$html .= '&rarr; No vacation granted &rarr; $cua='.$cua.'<br />';
				}
			}
			else
				$cua=$ua;

			$rl=$ru;
			$ry=$cua-$vd;
			$ru += $ry;

			// This year's start/end date
			$cysdate=max($pedate, $cy.'0101');
			$cyedate=min($pxdate, $cy.'1231');

			// Tooltip
			$ttjs='';
			$tt  = $th.'<table border=0 cellpadding=2 cellspacing=0>';
			$tt .= '<tr><td colspan=3 nowrap style=\'min-width:480px;padding:4px;background:#eee;border-right:1px solid #000;\'><div style=\'float:left;\'><b class=s5b>'.$cy.'</b> ('.get_entry($ycs,'mdate').' - '.get_entry($yce,'mdate').')&nbsp;<span class=lightgrey>&rarr; '.get_entry($cm,'integer').' '.$GLOBALS['__hla']['mon'].'</span>&nbsp;&nbsp;</div><div class='.(($ytp == 1)?'green':'blue').' style=\'float:right;padding-top:3px;\'>&nbsp;'.(($ytp != $otp)?'<i class=red>('.$GLOBALS['__hla']['ytp'][$otp].')</i>&nbsp;':'').$GLOBALS['__hla']['ytp'][$ytp].'</div></td></tr>';
			$tt .= '<tr><td colspan=3 style=\'border-top:1px solid #000;\'>&nbsp;</td></tr>';
			// ...Vacation carry (from previous year)
//echo('<b>'.$cy.'</b> Vacation carry (from previous year): DB='.$vsa[$cy]['ru'].', OU='.$ou.', RU='.$ru.'<hr />');
			if(fxIsArray($vsa[$cy]) && ($vsa[$cy]['ru'] != $ou))
			{
				if($ytp == 1)
					$drl=$ou;
				else
					$drl=$vsa[$cy]['ru'];
			}
			else
				$drl='';
			if($rl || strlen((string)$drl))
			{
				$tt .= '<tr><td colspan=2 nowrap>'.$GLOBALS['__hla']['htx'][0].':&nbsp;</td><td align=right nowrap>&nbsp;';
				if(strlen((string)$drl))
				{
					if($ytp == 1)	// Defined
						$icol='dc4';
					else
						$icol='e1001a';
					$tt .= '<i style=\'color:#'.$icol.';\'>('.get_entry($drl,'decimal').')</i> ';
				}
				$tt .= get_entry($rl,'decimal').' d</td></tr>';
			}
			// ...Defined vacation per year (for this whole year)
			if(fxIsArray($vsa[$cy]) && ($vsa[$cy]['ua'] != $ua))
				$dua=$vsa[$cy]['ua'];
			else
				$dua='';
			$tt .= '<tr><td colspan=2 nowrap>'.$GLOBALS['__hla']['htx'][1].':&nbsp;</td><td align=right nowrap>&nbsp;';
			if(strlen((string)$dua))
				$tt .= '<i class=red>('.get_entry($dua,'decimal').')</i> ';
			$tt .= get_entry($ua,'decimal').' d</td></tr>';
			if(strlen((string)$drl) || strlen((string)$dua))
			{
				$dbs=array('in'=>$pid.': '.$ycs.'-'.$yce, 'md'=>"update", 'id'=>$vsa[$cy]['id'], 'ua'=>$ua, 'ru'=>$rl, 'sc'=>$vsa[$cy]['sc'], 'st'=>"");
//fxDebug($dbs,'$dbs: $ytp='.$ytp, 0);
				if($ytp == 1)	// Defined
					$dbss[]=$dbs;
				else
					$dbsn[]=$dbs;
			}
			else if($otp != $ytp)
				$dbsn[]=array('in'=>$pid.': '.$ycs.'-'.$yce, 'md'=>"update", 'id'=>$vsa[$cy]['id'], 'ua'=>$vsa[$cy]['ua'], 'ru'=>$vsa[$cy]['ru'], 'sc'=>$vsa[$cy]['sc'], 'st'=>"");
			// ...Unemployed months (for this whole year)
			if($cua < $ua)
				$tt .= '<tr><td colspan=2 nowrap>'.(12-$ycm).' '.$GLOBALS['__hla']['htx'][2].':&nbsp;</td><td align=right nowrap>&nbsp;-'.get_entry($ua-$cua,'decimal').' d</td></tr>';
			$tt .= '<tr><td colspan=3 style=\'border-bottom:1px solid #888;\'></td></tr>';
			$tt .= '<tr><td colspan=3 align=right nowrap>&nbsp;<b>'.get_entry($cua+$rl,'decimal').'</b> d</td></tr>';
//$html .= '<div style="padding:4px 8px;background:#0cc;border:2px solid #000;">['.$ycs.' - '.$yce.'] &rarr; Valid vacation entitlement: $cua=<b>'.$cua.'</b> (Total vacation entitlement: $ua=<b>'.$ua.'</b>) - Valid vacation counter for this year: $vd=<b>'.$vd.'</b> = Calculated remaining vacations for this year: $ry=<b>'.$ry.'</b> + Remaing vacations from lat year: $rl=<b>'.$rl.'</b> &rarr; Total carry vacation for next year: $ru=<b>'.$ru.'</b></div>';
			$tt .= '<tr><td colspan=3>&nbsp;</td></tr>';
			// ...Vacations taken (Details)
			$bdt='';
			if(strlen((string)$vtr))
			{
				$bdt=' style=\'border-top:1px solid #888;\'';
				// Headers: Vacation Date | Calculation Detail: Days - Free - Holidays | Day(s)
				$tt .= '<tr style=display:table-row><td style=\'background:#eee;border-bottom:1px solid #aaa;border-right:1px solid #aaa;\'>&nbsp;<b>'.$GLOBALS['__hla']['hhd'][0].'</b>&nbsp;</td><td style=\'background:#eee;border-bottom:1px solid #aaa;border-right:1px solid #aaa;\'>&nbsp;<b>'.$GLOBALS['__hla']['hhd'][1].'</b>&nbsp;</td><td align=right style=\'background:#eee;border-bottom:1px solid #aaa;\'>&nbsp;<b>'.$GLOBALS['__hla']['hhd'][2].'</b>&nbsp;</td></tr>';
				$tt .= $vtr;
				if($vtc > $vtm)
				{
					// Click to display the remaining $var1 vacation entries in a new popup
					$tt .= '<tr note style=display:table-row><td colspan=3 align=center nowrap style=\'border-top:1px solid #ccc;background:#f8f8f8;height:24px;\'><i class=darkgrey>&rarr; '.str_replace('$var1', '<b>'.($vtc-$vtm).'</b>', $GLOBALS['__hla']['hhd'][3]).' &larr;</i></td></tr>';
				}
			}
			// ...Vacations taken
			$tt .= '<tr><td colspan=2 nowrap'.$bdt.'>'.$GLOBALS['__hla']['htx'][3].':&nbsp;</td><td align=right nowrap'.$bdt.'>&nbsp;<b>'.get_entry($vd,'decimal').'</b> d</td></tr>';
			$tt .= '<tr><td colspan=3>&nbsp;</td></tr>';
			// ...Remaining vacation
			$tt .= '<tr><td colspan=2 nowrap>&rarr; '.(($cy == (int)substr((string)$GLOBALS['date'],0,4))?$GLOBALS['__hla']['htx'][4]:$GLOBALS['__hla']['htx'][5]).':&nbsp;</td><td align=right nowrap>&nbsp;<b>'.get_entry($ru,'decimal').'</b> d</td></tr>';

			// Necessary database adjustments?
			$dbsn_ok=0;
			$dbsn_ko=0;
			$dbvn_ok=0;
			$dbvn_ko=0;
//fxDebug($dbsn,'$dbsn', 0); fxDebug($dbvn,'$dbvn', 0);
			if(fxIsArray($dbsn) || fxIsArray($dbvn))
			{
				// Start Transaction
				if(!$GLOBALS['db_in_trans']);
					db_rollback('start');
				$derr=false;

				// ...Sum
				if(fxIsArray($dbsn))
				{
					foreach($dbsn as $i => $a)
					{
//fxDebug($a,'Sum / $a: $i='.$i, 0);
						if($a['md'] == "insert")
						{
							$aid=id_max('artikel', 'artikel_id');
							$sql=db_array2sql(array('mode'=>"insert", 'table'=>"artikel", 'columns'=>array('mandanten_id'=>$GLOBALS['fxpglobals']['client'], 'artikel_id'=>$aid, 'personen_id'=>$pid, 'sachmittel_id'=>-1039, 'ek'=>0.0, 'vk'=>0.0, 'mwst_satz'=>0.0, 'soll_beginn'=>$cy.'0101000000', 'soll_ende'=>$cy.'1231240000', 'urlaubsanspruch'=>$a['ua'], 'resturlaub'=>$a['ru'], 'archiv'=>$a['sc'])));
						}
						else
							$sql=db_array2sql(array('mode'=>"update", 'table'=>"artikel", 'where'=>"artikel_id=".$a['id']." AND mandanten_id=".$GLOBALS['fxpglobals']['client'], 'columns'=>array('urlaubsanspruch'=>$a['ua'], 'resturlaub'=>$a['ru'], 'archiv'=>$a['sc'])));
						$verr=db_x($sql);
if($rdebug == $pid) echo('&quot;'.$sql.'&quot;<br />&rarr; '.($verr?'<b style="color:#e1001a;">KO</b>':'<b style="color:#009f6b;">OK</b>').'<hr size=1 />');
						if($verr)
						{
							$dbsn[$i]['st']='ERROR';
							$dbsn_ko++;
							$derr=true;
						}
						else
						{
							$dbsn[$i]['st']='OK';
							$dbsn_ok++;
						}
					}
				}

				// ...Vac
				if(fxIsArray($dbvn))
				{
					foreach($dbvn as $i => $a)
					{
//fxDebug($a,'Vac / $a: $i='.$i, 0);
						$vr[$i]=true;
						$sql=db_array2sql(array('mode'=>"Update", 'table'=>"termine", 'where'=>"termin_id=".$i." AND personen_id=".$pid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'], 'columns'=>array('urlaub'=>$a['ud'])));
						$verr=db_x($sql);
if($rdebug == $pid) echo('&quot;'.$sql.'&quot;<br />&rarr; '.($verr?'<b style="color:#e1001a;">KO</b>':'<b style="color:#009f6b;">OK</b>').'<hr size=1 />');
						if($verr)
						{
							$dbvn[$i]['st']='ERROR';
							$dbvn_ko++;
							$derr=true;
						}
						else
						{
							$dbvn[$i]['st']='OK';
							$dbvn_ok++;
						}
					}
				}

				// Transaction Rollback?
//$derr=true;
				if($GLOBALS['db_in_trans'])
				{
					if($derr)
						db_rollback('rollback');
					else
						db_rollback('commit');
				}
			}

			// ...Status
			if($dbsn_ko || $dbsn_ok || $dbvn_ko || $dbvn_ok)
			{
				$tt .= '<tr><td colspan=3>&nbsp;</td></tr>';
				if($dbsn_ko || $dbsn_ok)
				{
					if($dbsn_ko)
					{
						$col='e1001a';
						if($ytp == 3)
							$mk=1;	// The new subtotal record could not be created!
						else
							$mk=3;	// The subtotal record could not be changed!
					}
					else if($dbsn_ok)
					{
						$col='009f6b';
						if($ytp == 3)
							$mk=0;	// The new subtotal record was successfully created!
						else
							$mk=2;	// The subtotal record was successfully changed.
					}
					$tt .= '<tr><td colspan=3 nowrap><div style=\'padding:4px 12px;border:1px solid #'.$col.';border-right:2px solid #'.$col.';border-bottom:2px solid #'.$col.';text-align:center;\'><b style=\'color:#'.$col.';\'>'.$GLOBALS['__hla']['hms'][$mk].'</b></div></td></tr>';
				}
				if($dbvn_ko || $dbvn_ok)
				{
					if($dbvn_ko)
					{
						$col='e1001a';
						$mk=5;	// The vacation day calculation could not be changed!
					}
					else if($dbvn_ok)
					{
						$col='009f6b';
						$mk=4;	// The vacation day calculation was successfully changed.
					}
					$tt .= '<tr><td colspan=3 nowrap><div style=\'padding:4px 12px;border:1px solid #'.$col.';border-right:2px solid #'.$col.';border-bottom:2px solid #'.$col.';text-align:center;\'><b style=\'color:#'.$col.';\'>'.$GLOBALS['__hla']['hms'][$mk].'</b></div></td></tr>';
				}
			}

			$tt .= '</table>';

			$ra[$cy]=array(
				'id'=>0, 'dba'=>-1, 'type'=>$ytp,							// Data id | Data value (0=Defined, else Calculated) | Type (Lookup key for detailed value text 1887)
				'startdate'=>$cysdate, 'enddate'=>$cyedate,					// Start/End date
				'def_vacationperyear'=>$ua,									// Defined vacation per year (for this whole year)
				'calc_vacationperyear'=>$cua,								// Calculated vacation per year (for this year)
				'vacationcarry'=>$rl,										// Vacation carry (from previous year)
				'ent_vacationdays'=>$cua+$rl,								// Entitled vacation days (for this year)
				'vacationtaken'=>$vd,										// Vacation taken (this year)
				'rem_vacationdays'=>$ru,									// Remaining vacation days (this year)
				'tooltip'=>$tt,												// Tooltip
				'dbss'=>$dbss,												// Sum: Suggested calculation corrections
				'dbsn'=>$dbsn, 'dbsn_ok'=>$dbsn_ok, 'dbsn_ko'=>$dbsn_ko,	// Sum: Necessary calculation corrections | Success | Error
				'dbvn'=>$dbvn, 'dbvn_ok'=>$dbvn_ok, 'dbvn_ko'=>$dbsn_ko		// Vac: Necessary calculation corrections | Success | Error
			);
			if(isset($vsa[$cy]))
			{
				$ra[$cy]['id']=$vsa[$cy]['id'];
				$ra[$cy]['dba']=$vsa[$cy]['sc'];
			}
//echo($tt.'<hr />');
//$html .= '</div>'.$GLOBALS['nl'];
		}

		$cy++;
	}
//echo($html);

if($rdebug == $pid) fxDebug($ra,'$ra', 0);
if($rdebug == $pid) echo('<hr />');

	return $ra;
}

/**
 * Recalculate all vacations for all person from a specific year onwards
 *
 * @param integer $year - Optional parameter (default = 0):     From this year onward (if <= 0 year is ignored)
 */
function fxf_recalcVac($year=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$year=(int)$year;

	// Get all affected persons
	$sql="SELECT DISTINCT personen_id FROM artikel WHERE sachmittel_id=-1039 AND archiv<>0 AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
	if($year > 0)
		$sql .= " AND soll_beginn>='".substr('000'.$year,-4)."0101000000'";
	$cpa=db_values($sql,'*');
//fxDebug($cpa,$sql, 0);
	if(fxIsArray($cpa))	// Person found, recalculate vacation for each person
	{
		foreach($cpa as $cp)
			fxf_getVacCalc($cp['personen_id'], $year);
	}
}

/**
 * Get tooltip header with name, person picture + entry/leave dates
 *
 * @param integer $pid  - Optional parameter (default = 0):     Person id (if 0 id is ignored)
 * @param integer $year - Optional parameter (default = 0):     From this year onward (if <= 0 year is ignored)
 * @param boolean $clt  - Optional parameter (default = true):  Only for current client?
 *
 * @return Error status of executed sql (false on OK)
 */
function fxf_getPersTooltipHeader($pid)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$pid=(int)$pid;

	// Get person data
	$pa=getPersonData($pid);
//fxDebug($pa,'$pa: $pid='.$pid, 0);

	// Get texts
	if(!fxIsArray($GLOBALS['__pth']))
	{
		$GLOBALS['__pth']=array(
			'led' => get_text(54,'20'),		// Entry Date
			'lxd' => get_text(55,'20')		// Leave Date
		);
//fxDebug($GLOBALS['__pth'],'__pth', 0);
	};

	// Get picture
	$pmxh=128;
	if(!fxIsArray($GLOBALS['__ptp']))
		$GLOBALS['__ptp']=array();
	if(!fxIsArray($GLOBALS['__ptp'][$pid]))
	{
		$GLOBALS['__ptp'][$pid]=getPersonIcons($pid, $pmxh, $pa['geschlecht'], $GLOBALS['fxpglobals']['client'], $pa['personentyp']);
//fxDebug($GLOBALS['__ptp'][$pid],'__ptp['.$pid.']', 0);
	};

	$pedate=substr((string)$pa['maeintrittsdatum'],0,8);
	$pxdate=substr((string)$pa['maaustrittsdatum'],0,8);
	if(strlen((string)$pxdate) < 8)
		$pxdate='99999999';
	$pxyear=(int)substr((string)$pxdate,0,4);

	$pec='black';
	$pxc='black';
	$pnc='black';
	if($pedate > $GLOBALS['date'])
	{
		$pec='red';
		$pnc='red';
	}
	if($pxdate < $GLOBALS['date'])
	{
		$pxc='red';
		$pnc='red';
	}
	$th='<div class=\'pmtitle s5b '.$pnc.'\' style=\'position:relative;min-width:480px;padding:4px;margin-top:2px;margin-bottom:16px;\'>'.$pa['nv'];
	$pst='';
	if(strlen((string)$GLOBALS['__ptp'][$pid]['128']))
		$pst='128';
	else if(strlen((string)$GLOBALS['__ptp'][$pid]['256']))
		$pst='256';
	else if(strlen((string)$GLOBALS['__ptp'][$pid]['src']))
		$pst='src';
	if(strlen((string)$pst))
	{
		$hpw=round((int)substr((string)$GLOBALS['__ptp'][$pid]['size_'.$pst],7,3)/2);
		$th .= '<img src=\''.$GLOBALS['__ptp'][$pid][$pst].$GLOBALS['__ptp'][$pid]['mtime'].'\''.$GLOBALS['__ptp'][$pid]['size_'.$pst].' style=\'position:absolute;right:-'.$hpw.'px;bottom:-40px;max-height:'.$pmxh.'px;width:auto;background:rgba(240,240,240, 0.90);border:1px solid white;border-right-color:black;border-bottom-color:black;border-radius:8px;box-shadow:3px 3px 6px rgba(0,0,0, 0.5);display:block;\'>';
	}
	$th .= '</div>';
	$th .= '<table width=100% border=0 cellpadding=0 cellspacing=0><tr><td>';
	$th .= $GLOBALS['__pth']['led'].': <b class='.$pec.'>'.get_entry($pedate,'date').'</b>';
	if($pxyear != 9999)
		$th .= ' <span class=grey>-</span> '.$GLOBALS['__pth']['lxd'].': <b class='.$pxc.'>'.get_entry($pxdate,'date').'</b>';
	$th .= '<br /></td></tr></table>';

	return $th;
}
?>