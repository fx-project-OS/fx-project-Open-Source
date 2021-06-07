<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : urlaubsplan_pre.inc                                          //
// Version     : 21.1                                                         //
// Begin       : 2021-01-15                                                   //
// Last Change : 2021-01-15                                                   //
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
 * PF 92: Absence + Timetable Chart
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.1
 */

$maske258_sf=true;
echo(maske(258));
$maske258_nv=true;

$fixcols=1;

// Mitarbeiterdaten einlesen, wenn Anzeige-Button gedrückt wurde und keine Fehler vorliegen
if($_POST['Button_Anzeigen'] && !$err)
{
	$personen=array();

	// 1. Schritt:	Personendaten der Mitarbeiter auslesen (Name, verfügbare Wochentage)
	$wochentage=array(1 => 'montag', 'dienstag', 'mittwoch', 'donnerstag', 'freitag', 'samstag', 'sonntag');
	$bit=array(1, 2, 4, 8, 16, 32, 64, 128);

	$sql  = "SELECT p.pname, p.vorname, p.personen_id, p.geschlecht, p.personentyp, p.inaktiv, m.maeintrittsdatum, m.maaustrittsdatum, m.montag, m.dienstag, m.mittwoch, m.donnerstag, m.freitag, m.samstag, m.sonntag";
	$sql .= " FROM personen p LEFT JOIN madaten m ON p.personen_id=m.personen_id AND m.mandanten_id=".$GLOBALS['fxpglobals']['client'];

	// Personentyp 275=Mitarbeiter, 840=ext. Mitarbeiter
	$sql .= " WHERE p.personentyp IN (275,840) AND p.mandanten_id=".$GLOBALS['fxpglobals']['client'];

	// Abteilung?
	if($_POST['Abt_ID'])
	{
		$aabt=inferior_abteilung($_POST['Abt_ID']);
		$sql .= " AND p.abt_id IN (".inferior_abteilung($_POST['Abt_ID']).")";
	}

	// Filter?
	$flt=filter_person('', true, false, 'Personen_ID');
	$maske258_flt=$mskfilter;
	if(strlen($flt))
		$sql .= " AND ".$flt;

	$sql .= " ORDER BY p.pname, p.vorname, p.personen_id";

	$zeile=2;  // 0: Monat, 1: KW, 2: Tag
	$item=db_values($sql, '*');
	$icnt=0;
	if(fxIsArray($item))
		$ilen=sizeof($item);
	else
		$ilen=0;

	// 2. Schritt:	Personendaten der Mitarbeiter aussortieren
	$ma_zeile=array();
	while($icnt < $ilen)
	{
		$i=$item[$icnt]['personen_id'];

		$ma_zeile[$i]=++$zeile;
		$personen[$i]['fullname']=$item[$icnt]['pname'];
		if(strlen(trim($item[$icnt]['vorname'])))
			$personen[$i]['fullname'] .= ',&nbsp;'.trim($item[$icnt]['vorname']);
		$personen[$i]['geschl']=$item[$icnt]['geschlecht'];
		$personen[$i]['inakt']=$item[$icnt]['inaktiv'];
		$personen[$i]['typ']=$item[$icnt]['personentyp'];
		$personen[$i]['ed']=$item[$icnt]['maeintrittsdatum'];
		$personen[$i]['ad']=$item[$icnt]['maaustrittsdatum'];
		$personen[$i]['tage']=0;
		for($wt=1; $wt<8; $wt++)
		{
			if($item[$icnt][$wochentage[$wt]] == 1) // Wochentage zuordnen 1=Montag, 2=Dienstag,...
				$personen[$i]['tage'] |= $bit[$wt-1];
		}
		$icnt++;
	}
	if($pb_debug && $pb_debug <= 2)
		fxDebug($personen, '$personen');

	// Daten für Anzeige ermitteln nur, wenn Personen gefunden
	if(fxIsArray($personen))
	{
		// Besitzt dieser Mandant überhaupt eine Abteilungs-Struktur
		$own_sabt=db_value("SELECT abt_id FROM abteilung WHERE mandanten_id=".$GLOBALS['fxpglobals']['client']);
		$own_eabts='';
		if(strlen($own_sabt))	// Ja
		{
			$own_sabt=true;
			// Eigene Abteilungs-ID suchen
			$own_pabt=db_value("SELECT abt_id FROM personen WHERE personen_id=".$GLOBALS['fxpglobals']['person']." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
			if(!strlen($own_pabt))
				$own_pabt=0;
			else
			{
				$own_eabt=db_value("SELECT abt_ebene FROM abteilung WHERE mandanten_id=".$GLOBALS['fxpglobals']['client']." AND abt_id=".$own_pabt);
				if(strlen($own_eabt))
					$own_eabts=db_values("SELECT abt_id FROM abteilung WHERE mandanten_id=".$GLOBALS['fxpglobals']['client']." AND abt_ebene LIKE '".$own_eabt."%'", '*');
			}
		}
		else					// Nein
		{
			$own_sabt=false;
			$own_pabt=0;
		}

		$inakt='{'.get_text(1391, '20').'}';	// {inaktiv}

		// Kalender erstellen
		$GLOBALS['fxptdata']['cal']=array();

		$xoffset=2;
		$utage=0;
		$sutage=0;
		$stage=0;
		$ktage=0;
		$gtage=0;
		$atage=0;
		$itage=0;

		// Zeile 0 (0): Zeitspanne
		$psvon=formatdatetime($GLOBALS['fxpglobals']['settings']['formatdatum'], $von);
		$psbis=formatdatetime($GLOBALS['fxpglobals']['settings']['formatdatum'], $bis);
		if(!$psvon && !$psbis)
			$zs=$GLOBALS['pb_string']['ka']; // (Keine Angaben)
		else
			$zs=$psvon.'&nbsp;-&nbsp;'.$psbis; // Von - Bis
		$GLOBALS['fxptdata']['cal'][0][0]['text']='&nbsp;<b>'.$GLOBALS['pb_string']['zeitspanne'].'</b>:&nbsp;<br />&nbsp;'.$zs.'&nbsp;';
		$GLOBALS['fxptdata']['cal'][0][0]['color']='ffffff';
		$GLOBALS['fxptdata']['cal'][0][0]['rowspan']=2;
		$GLOBALS['fxptdata']['cal'][0][0]['typ']='All';

		if($_POST['Urlaub'])
		{
			// Urlaubstage
			$uva=explode('|', meldung(2849));	// 2849: Arbeitstage|Feiertage|Abwesenheitstage|Urlaubstage|Sonderurlaubstage|Schulungstage|Krankheitstage|Startübertrag|Soll-Stunden|Ist-Stunden|Minusstunden|Überstunden|Deltastunden
			$GLOBALS['fxptdata']['cal'][0][$xoffset-1]['text']='&nbsp;<b>'.$uva[3].'</b>&nbsp;';	// Urlaubstage
			$GLOBALS['fxptdata']['cal'][0][$xoffset-1]['color']='ffffff';
			$GLOBALS['fxptdata']['cal'][0][$xoffset-1]['align']='center';
			$GLOBALS['fxptdata']['cal'][0][$xoffset-1]['colspan']=3;
			$GLOBALS['fxptdata']['cal'][0][$xoffset-1]['rowspan']=2;
			$GLOBALS['fxptdata']['cal'][0][$xoffset-1]['typ']='All';

			// Urlaub
			$GLOBALS['fxptdata']['cal'][0][$xoffset+2]['text']='&nbsp;<b>'.$farbnamen[767].'</b>&nbsp;';
			$GLOBALS['fxptdata']['cal'][0][$xoffset+2]['color']='ffffff';
			$GLOBALS['fxptdata']['cal'][0][$xoffset+2]['align']='center';
			$GLOBALS['fxptdata']['cal'][0][$xoffset+2]['colspan']=2;
			$GLOBALS['fxptdata']['cal'][0][$xoffset+2]['rowspan']=2;
			$GLOBALS['fxptdata']['cal'][0][$xoffset+2]['typ']='All';

			// Sonderurl.
			$GLOBALS['fxptdata']['cal'][0][$xoffset+4]['text']='&nbsp;<b>'.$farbnamen[2882].'</b>&nbsp;';
			$GLOBALS['fxptdata']['cal'][0][$xoffset+4]['color']='ffffff';
			$GLOBALS['fxptdata']['cal'][0][$xoffset+4]['align']='center';
			$GLOBALS['fxptdata']['cal'][0][$xoffset+4]['colspan']=2;
			$GLOBALS['fxptdata']['cal'][0][$xoffset+4]['rowspan']=2;
			$GLOBALS['fxptdata']['cal'][0][$xoffset+4]['typ']='All';

			$xoffset += 7;
			$fixcols += 3;
		}
		if($_POST['Schulung'])
		{
			// Schulung
			$GLOBALS['fxptdata']['cal'][0][$xoffset-1]['text']='&nbsp;<b>'.$farbnamen[2107].'</b>&nbsp;';
			$GLOBALS['fxptdata']['cal'][0][$xoffset-1]['color']='ffffff';
			$GLOBALS['fxptdata']['cal'][0][$xoffset-1]['align']='center';
			$GLOBALS['fxptdata']['cal'][0][$xoffset-1]['colspan']=2;
			$GLOBALS['fxptdata']['cal'][0][$xoffset-1]['rowspan']=2;
			$GLOBALS['fxptdata']['cal'][0][$xoffset-1]['typ']='All';

			$xoffset += 2;
		}
		if($_POST['Krank'])
		{
			// Krank
			$GLOBALS['fxptdata']['cal'][0][$xoffset-1]['text']='&nbsp;<b>'.$farbnamen[2108].'</b>&nbsp;';
			$GLOBALS['fxptdata']['cal'][0][$xoffset-1]['color']='ffffff';
			$GLOBALS['fxptdata']['cal'][0][$xoffset-1]['align']='center';
			$GLOBALS['fxptdata']['cal'][0][$xoffset-1]['colspan']=2;
			$GLOBALS['fxptdata']['cal'][0][$xoffset-1]['rowspan']=2;
			$GLOBALS['fxptdata']['cal'][0][$xoffset-1]['typ']='All';

			$xoffset += 2;
		}
		if($_POST['Gleitzeit'])
		{
			// Gleittag
			$GLOBALS['fxptdata']['cal'][0][$xoffset-1]['text']='&nbsp;<b>'.$farbnamen[2407].'</b>&nbsp;';
			$GLOBALS['fxptdata']['cal'][0][$xoffset-1]['color']='ffffff';
			$GLOBALS['fxptdata']['cal'][0][$xoffset-1]['align']='center';
			$GLOBALS['fxptdata']['cal'][0][$xoffset-1]['colspan']=2;
			$GLOBALS['fxptdata']['cal'][0][$xoffset-1]['rowspan']=2;
			$GLOBALS['fxptdata']['cal'][0][$xoffset-1]['typ']='All';

			$xoffset += 2;
		}
		if($_POST['Urlaub'] || $_POST['Krank'] || $_POST['Schulung'])
		{
			// Abwesend
			$GLOBALS['fxptdata']['cal'][0][$xoffset-1]['text']='&nbsp;<b>'.$farbnamen[2880].'</b>&nbsp;';
			$GLOBALS['fxptdata']['cal'][0][$xoffset-1]['color']='ffffff';
			$GLOBALS['fxptdata']['cal'][0][$xoffset-1]['align']='center';
			$GLOBALS['fxptdata']['cal'][0][$xoffset-1]['rowspan']=2;
			$GLOBALS['fxptdata']['cal'][0][$xoffset-1]['typ']='All';

			$xoffset++;
		}
		if($_POST['ISTStunden'])
		{
			// IST-Stunden
			$GLOBALS['fxptdata']['cal'][0][$xoffset-1]['text']='&nbsp;<b>'.$GLOBALS['pb_string']['istd'].'</b>&nbsp;';
			$GLOBALS['fxptdata']['cal'][0][$xoffset-1]['color']='ffffff';
			$GLOBALS['fxptdata']['cal'][0][$xoffset-1]['align']='center';
			$GLOBALS['fxptdata']['cal'][0][$xoffset-1]['rowspan']=2;
			$GLOBALS['fxptdata']['cal'][0][$xoffset-1]['typ']='All';

			$xoffset++;
		}

		// Info
		$GLOBALS['fxptdata']['cal'][0][$xoffset-1]['text']='&nbsp;<b>'.$GLOBALS['pb_string']['info'].'</b>&nbsp;';
		$GLOBALS['fxptdata']['cal'][0][$xoffset-1]['color']='ffffff';
		$GLOBALS['fxptdata']['cal'][0][$xoffset-1]['align']='center';
		$GLOBALS['fxptdata']['cal'][0][$xoffset-1]['rowspan']=2;
		$GLOBALS['fxptdata']['cal'][0][$xoffset-1]['typ']='All';

		// Zeile 2 (0-$xoffset): Überschriften "Projektmitarbeiter", ..., "Info"
		$GLOBALS['fxptdata']['cal'][2][0]['text']	= '&nbsp;<b>'.$GLOBALS['pb_string']['mitarbeiter'].'</b>&nbsp;'; // Projektmitarbeiter
		$GLOBALS['fxptdata']['cal'][2][0]['color']	= 'e0e0e0';
		$GLOBALS['fxptdata']['cal'][2][0]['typ']	= 'All';

		$cc=1;
		if($_POST['Urlaub'])
		{
			if(substr($bis, 4, 4) == '1231')
				$GLOBALS['fxptdata']['cal'][2][$cc]['text']	= '&nbsp;<b>'.$GLOBALS['pb_string']['resturlaub'].'</b>&nbsp;';		// Übertr. / Jahres- / Resturl.
			else
				$GLOBALS['fxptdata']['cal'][2][$cc]['text']	= '&nbsp;<b>'.$GLOBALS['pb_string']['resturlaub_tj'].'</b>&nbsp;';	// Übertr. / Teil- (Jahres-) / Resturl. (Jahr)
			$GLOBALS['fxptdata']['cal'][2][$cc]['align']	= 'center';
			$GLOBALS['fxptdata']['cal'][2][$cc]['color']	= 'e0e0e0';
			$GLOBALS['fxptdata']['cal'][2][$cc]['colspan']	= 3;
			$GLOBALS['fxptdata']['cal'][2][$cc]['typ']		= 'All';
			$cc += 3;

			$GLOBALS['fxptdata']['cal'][2][$cc]['text']		= '&nbsp;<b>'.$GLOBALS['pb_string']['urlaub'].'</b>&nbsp;'; // Antr. / Urlaub
			$GLOBALS['fxptdata']['cal'][2][$cc]['align']	= 'center';
			$GLOBALS['fxptdata']['cal'][2][$cc]['color']	= 'e0e0e0';
			$GLOBALS['fxptdata']['cal'][2][$cc]['colspan']	= 2;
			$GLOBALS['fxptdata']['cal'][2][$cc]['typ']		= 'All';
			$utage=$cc;
			$cc += 2 ;

			$GLOBALS['fxptdata']['cal'][2][$cc]['text']		= '&nbsp;<b>'.$GLOBALS['pb_string']['sonderurlaub'].'</b>&nbsp;'; // Antr. / Sonderurl.
			$GLOBALS['fxptdata']['cal'][2][$cc]['align']	= 'center';
			$GLOBALS['fxptdata']['cal'][2][$cc]['color']	= 'e0e0e0';
			$GLOBALS['fxptdata']['cal'][2][$cc]['colspan']	= 2;
			$GLOBALS['fxptdata']['cal'][2][$cc]['typ']		= 'All';
			$sutage=$cc;
			$cc += 2 ;
		}

		if($_POST['Schulung'])
		{
			$GLOBALS['fxptdata']['cal'][2][$cc]['text']		= '&nbsp;<b>'.$GLOBALS['pb_string']['schulung'].'</b>&nbsp;'; // Antr. / Schulung
			$GLOBALS['fxptdata']['cal'][2][$cc]['align']	= 'center';
			$GLOBALS['fxptdata']['cal'][2][$cc]['color']	= 'e0e0e0';
			$GLOBALS['fxptdata']['cal'][2][$cc]['colspan']	= 2;
			$GLOBALS['fxptdata']['cal'][2][$cc]['typ']		= 'All';
			$stage=$cc;
			$cc += 2;
		}

		if($_POST['Krank'])
		{
			$GLOBALS['fxptdata']['cal'][2][$cc]['text']		= '&nbsp;<b>'.$GLOBALS['pb_string']['krank'].'</b>&nbsp;'; // Mldg. / Krank
			$GLOBALS['fxptdata']['cal'][2][$cc]['align']	= 'center';
			$GLOBALS['fxptdata']['cal'][2][$cc]['color']	= 'e0e0e0';
			$GLOBALS['fxptdata']['cal'][2][$cc]['colspan']	= 2;
			$GLOBALS['fxptdata']['cal'][2][$cc]['typ']		= 'All';
			$ktage=$cc;
			$cc += 2;
		}

		if($_POST['Gleitzeit'])
		{
			$GLOBALS['fxptdata']['cal'][2][$cc]['text']		= '&nbsp;<b>'.$GLOBALS['pb_string']['gleit'].'</b>&nbsp;'; // Antr. / Gleittag
			$GLOBALS['fxptdata']['cal'][2][$cc]['align']	= 'center';
			$GLOBALS['fxptdata']['cal'][2][$cc]['color']	= 'e0e0e0';
			$GLOBALS['fxptdata']['cal'][2][$cc]['colspan']	= 2;
			$GLOBALS['fxptdata']['cal'][2][$cc]['typ']		= 'All';
			$gtage=$cc;
			$cc += 2;
		}

		if($_POST['Urlaub'] || $_POST['Krank'] || $_POST['Schulung'])
		{
			$GLOBALS['fxptdata']['cal'][2][$cc]['text']		= '&nbsp;<i>'.meldung(1255).'</i>&nbsp;'; // Abwesend: (Tage)
			$GLOBALS['fxptdata']['cal'][2][$cc]['color']	= 'e0e0e0';
			$GLOBALS['fxptdata']['cal'][2][$cc]['align']	= 'center';
			$GLOBALS['fxptdata']['cal'][2][$cc]['typ']		= 'All';
			$atage=$cc;
			$cc += 1;
		}

		if($_POST['ISTStunden'])
		{
			$GLOBALS['fxptdata']['cal'][2][$cc]['text']		= '&nbsp;<i>'.meldung(1174).'</i>&nbsp;'; // IST-Stunden: (Stunden)
			$GLOBALS['fxptdata']['cal'][2][$cc]['color']	= 'e0e0e0';
			$GLOBALS['fxptdata']['cal'][2][$cc]['align']	= 'center';
			$GLOBALS['fxptdata']['cal'][2][$cc]['typ']		= 'All';
			$itage=$cc;
			$cc += 1;
		}

		$GLOBALS['fxptdata']['cal'][2][$cc]['text']		= '&nbsp;<i>?</i>&nbsp;'; // Info
		$GLOBALS['fxptdata']['cal'][2][$cc]['color']	= 'e0e0e0';
		$GLOBALS['fxptdata']['cal'][2][$cc]['align']	= 'center';
		$GLOBALS['fxptdata']['cal'][2][$cc]['typ']		= 'All';

		// Zeile 0-2 ($xoffset(2)-$xcolumns): Loop über alle Tage des Zeitraums
		$holidays	= get_feiertage($von, $bis, $GLOBALS['fxpglobals']['client']);
		$xcolumns	= kalendertage($von, $bis, $ke);
		if($ka_debug == 6)
			fxDebug($holidays, 'Feiertage');

		// Zeile 3-?: Personen + Sondertage eintragen in jede Zeile
		$aktzeile=3;
		foreach($personen as $key => $pers)
			uk_kalenderzeile($key, $pers, $aktzeile, $pb_debug, $ke);

		$GLOBALS['fxptdata']['cal'][0][0]['fixcols']=$fixcols;
		ksort($GLOBALS['fxptdata']['cal']);
//fxDebug($GLOBALS['fxptdata']['cal'],'cal');

		// Kalenderüberschrift: Grafische Darstellung der Abwesenheitstage und/oder der gearbeiteten Ist-Stunden der Mitarbeiter
		$mhead=1355;
		$mha=array('D'=>0, 'W'=>1, 'M'=>2, 'Y'=>3);
		$mhead_text=meldung($mhead).' ('.meldung(1261+$mha[$ke]).')';
		if(fxIsArray($personen) && fxIsArray($GLOBALS['fxptdata']) && fxIsArray($GLOBALS['fxptdata']['cal']) && fxIsArray($GLOBALS['fxptdata']['cal'][3][$xoffset]))
		{
			// Legende zeichnen
			$legende=farblegende('get', false);
			kalender($mhead, $legende, '', $mhead_text, '', 1355, $fixcols);
		}
		// Fehlermeldung, falls keine Daten gefunden wurden
		else if($Button_Anzeigen && !$fehler)
		{
			kalender($mhead, '?'.meldung(1526), '', $mhead_text, '', 1355);
		}
	}
}


////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/**
 * -----------------------------------------------------------------------------
 * ------------------------------------------------------------ UK_KALENDERZEILE
 * -----------------------------------------------------------------------------
 *
 * @param various $persid    - Mandatory parameter:                 ???PARAMETER???
 * @param various $person    - Mandatory parameter:                 ???PARAMETER???
 * @param various &$aktzeile - Mandatory parameter:                 ???PARAMETER???
 * @param various $ka_debug  - Mandatory parameter:                 ???PARAMETER???
 * @param string  $einheit   - Optional parameter (default='D'):  ???PARAMETER???
 */
function uk_kalenderzeile($persid, $person, &$aktzeile, $ka_debug, $einheit='D')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	global $nl, $von, $bis, $xcolumns, $holidays, $xoffset, $utage, $sutage, $ktage, $stage, $gtage, $atage, $itage;

	if(!isset($GLOBALS['bwid']))
	{
		$GLOBALS['bwid']=get_uskg_array(false);
		unset($GLOBALS['bwid']['arbeit']);
		unset($GLOBALS['bwid']['frei']);
	}

	if(fxIsArray($person))
	{
		// Prüfen, ob Tage vorhanden
		// ...Ist-Stunden
		$wdays=array();
		if($_POST['ISTStunden'])
			$wdays=getWorkedDays($von,$bis,$persid);
		$wdc=sizeof($wdays);
//fxDebug($wdays, '$wdays: '.$wdc.' ('.$persid.')');
		// ...Sondertage
		$tcsv .= "";
		if($_POST['Urlaub'])
			$tcsv .= ",".FXP_AT_VACATIONREQUEST.",".FXP_AT_VACATION.",".FXP_AT_SPECIALVACATIONREQUEST.",".FXP_AT_SPECIALVACATION;
		if($_POST['Krank'])
			$tcsv .= ",".FXP_AT_SICKCALL.",".FXP_AT_SICK;
		if($_POST['Schulung'])
			$tcsv .= ",".FXP_AT_TRAININGREQUEST.",".FXP_AT_TRAINING;
		if($_POST['Gleitzeit'])
			$tcsv .= ",".FXP_AT_SLIDINGDAYREQUEST.",".FXP_AT_SLIDINGDAY;
		if($_POST['Urlaub'] || $_POST['Krank'] || $_POST['Schulung'])
			$tcsv .= ",".FXP_AT_ABSENT;
		if(strlen($tcsv))
			$tcsv=substr($tcsv,1);
		$sdays=get_sondertage($von,$bis,$persid,$tcsv,true);
		$sdc=sizeof($sdays);
//fxDebug($sdays, '$sdays: '.$sdc.' ('.$persid.')');
//echo(__LINE__.': <b>'.$person['fullname'].' ('.$persid.')</b> $wdc='.$wdc.', $sdc='.$sdc.'<br>');
		if(!$wdc && !$sdc && $_POST['akt_ma'])
			return;

		// Prüfen, ob im angegebene Zeitraum Angestellter der Firma
		$sd=substr($von,0,8);
		$ed=substr($bis,0,8);
		$psd=substr($person['ed'],0,8);
		$ped=substr($person['ad'],0,8);
		if(!$ped)
			$ped='99999999';
		if(($psd > $ed) || ($ped < $sd))
			return;
//echo('<b>'.$persid.' - '.$person['fullname'].'</b>: $sd='.$sd.', $ed='.$ed.' -- $psd='.$psd.', $ped='.$ped.'<br />');

		// Zeile x (Spalte 0: "Mitarbeiter")
		$pmxh=128;
		$pers_icons=getPersonIcons($persid, $pmxh, $person['geschl'], $GLOBALS['fxpglobals']['client'], $person['typ']);
//fxDebug($pers_icons,'$pers_icons');
		$sa='';
		if($pers_icons['def'])
			$sa=' style="opacity:0.1;"';
		$GLOBALS['fxptdata']['cal'][$aktzeile][0]['text']='&nbsp;<img src="'.$pers_icons['ico'].$pers_icons['mtime'].'" align="top" width="20" height="20"'.$sa.'>&nbsp;&nbsp;';
		if($person['inakt'])
			$GLOBALS['fxptdata']['cal'][$aktzeile][0]['text'] .= '<i class="grey">'.$person['fullname'].'</i>&nbsp;<i class="lightgrey">'.$GLOBALS['inakt'].'</i>';
		else if(($psd > $sd) || ($ped < $ed))
			$GLOBALS['fxptdata']['cal'][$aktzeile][0]['text'] .= '<font class="red">'.$person['fullname'].'</font>';
		else
			$GLOBALS['fxptdata']['cal'][$aktzeile][0]['text'] .= $person['fullname'];
		if($person['ad'])
			$GLOBALS['fxptdata']['cal'][$aktzeile][0]['text'] .= '&nbsp;<i class="lightgrey">{X}</i>';
		$GLOBALS['fxptdata']['cal'][$aktzeile][0]['text'] .= '&nbsp;';
		$GLOBALS['fxptdata']['cal'][$aktzeile][0]['class']='yat'.($aktzeile%2);
		$GLOBALS['fxptdata']['cal'][$aktzeile][0]['typ']='All';

		$th='<div class=\'s5b black\' style=\'position:relative;background:#dddddd;min-width:320px;border-radius:2px;padding:4px;margin-top:2px;\'>'.$person['fullname'];
		$pst='';
		if(strlen($pers_icons['128']))
			$pst='128';
		else if(strlen($pers_icons['256']))
			$pst='256';
		else if(strlen($pers_icons['src']))
			$pst='src';
		if(strlen($pst))
		{
			$hpw=round((int)substr($pers_icons['size_'.$pst],7,3)/2);
			$th .= '<img src=\''.$pers_icons[$pst].$pers_icons['mtime'].'\''.$pers_icons['size_'.$pst].' style=\'position:absolute;right:-'.$hpw.'px;bottom:-40px;max-height:'.$pmxh.'px;width:auto;background:rgba(240,240,240, 0.90);border:1px solid white;border-right-color:black;border-bottom-color:black;border-radius:8px;box-shadow:3px 3px 6px rgba(0,0,0, 0.5);\'>';
		}
		$th .= '</div>';

		// Eintritts- + Austrittsdatum
		if(!isset($GLOBALS['lit_elda']))
			$GLOBALS['lit_elda']=array('ed'=>get_text(54,'20'), 'ld'=>get_text(55,'20'));
		$lel=$GLOBALS['lit_elda']['ed'];
		$tel=get_entry($person['ed'], 'datum');
		if($person['ad'])
		{
			$lel .= '/'.$GLOBALS['lit_elda']['ld'];
			$tel .= '&nbsp;-&nbsp;'.get_entry($person['ad'], 'datum');
		}
		$GLOBALS['fxptdata']['cal'][$aktzeile][0]['tooltip_js']=$th.'<br /><span class=\'s4b black\'>'.$lel.':</span>&nbsp;&nbsp;<font class=black>'.$tel.'</font>';

		$ma_akt=false;
		$rerr=false;
		// Zeile x (Spalte "Urlaub")
		if($utage)
		{
			// Verwaltung, Geschäftsführer oder Selbst -> Urlaubstage anzeigen
			if($GLOBALS['fxpglobals']['persdat']['roles'][-1] || $GLOBALS['fxpglobals']['persdat']['roles'][2] || ($persid == $GLOBALS['fxpglobals']['person']))
			{
				$rua=get_resturlaub($persid, $von, $bis);
//fxDebug($rua, 'get_resturlaub('.$persid.', '.$von.', '.$bis.');');

				// Gesamtjahresurlaubsanspruch
				if($rua['akt']['urlaub_pro_jahr'])
					$uansp=get_entry($rua['akt']['urlaub_pro_jahr'], 'dezimal');
				else
					$uansp=get_entry(0, 'dezimal');

				// Übertr. (= Urlaubsübertrag aus Vorjahr)
				if($rua['akt']['urlaubsuebertrag_ganzjahr'])
					$url_vj=get_entry($rua['akt']['urlaubsuebertrag_ganzjahr'], 'dezimal');
				else
					$url_vj=get_entry(0, 'dezimal');

				// Teil- (= Teiljahresurlaub bis Ende angezeigter Zeitspanne)
				if($rua['akt']['urlaubsanspruch_teiljahr'])
					$url_tj=get_entry($rua['akt']['urlaubsanspruch_teiljahr'], 'dezimal');
				else
					$url_tj=get_entry(0, 'dezimal');
				// (Jahres-) (= Jahresurlaub)
				if($rua['akt']['urlaubsanspruch_ganzjahr'])
					$url_gj=get_entry($rua['akt']['urlaubsanspruch_ganzjahr'], 'dezimal');
				else
					$url_gj=get_entry(0, 'dezimal');

				// Resturl. (= Resturlaub bis Ende angezeigter Zeitspanne)
				if($rua['akt']['resturlaub_teiljahr'])
					$url_rtj=get_entry($rua['akt']['resturlaub_teiljahr'], 'dezimal');
				else
					$url_rtj=get_entry(0, 'dezimal');
				// (Jahr) (= Resturlaub bis Jahresende)
				if($rua['akt']['resturlaub_ganzjahr'])
					$url_rgj=get_entry($rua['akt']['resturlaub_ganzjahr'], 'dezimal');
				else
					$url_rgj=get_entry(0, 'dezimal');

				// Berechnungstooltip
				$tj =(int)substr($rua['akt']['datum_bis'], 0, 4);

				$tt  = '<span class=\'s4b black\'>'.$GLOBALS['pb_string']["tt_headline"].' '.$tj.'</span><br><br>';
				$tt .= '<table width=100% border=0 cellspacing=1 cellpadding=1>';
				// ...Eingetragener Gesamtjahresurlaubsanspruch für
				$ua=$url_gj;
				if($uansp != $url_gj)
					$ua .= '&nbsp;('.$uansp.')';
				$tt .= '<tr><td nowrap><font class=blue>'.$GLOBALS['pb_string']["tt_vacationyear"].' '.$tj.'</font>&nbsp;</td><td align=right>&nbsp;<font class=blue>'.$ua.'</font></td></tr>';

				$tt .= '<tr><td colspan=2>&nbsp;</td></tr>';

				if(substr($rua['akt']['datum_bis'], 4, 4) != '1231')
				{
					$tt .= '<tr><td colspan=2 style=\'border-bottom:1px solid #bbbbbb;\'></td></tr>';
					// ...Zeitspanne
					$tt .= '<tr><td nowrap bgcolor=#dddddd colspan=2><b class=black>'.$GLOBALS['pb_string']["zeitspanne"].': '.get_entry($rua['akt']['datum_von'], "datum").'-'.get_entry($rua['akt']['datum_bis'], "datum").'</b></td></tr>';
					$tt .= '<tr><td colspan=2 style=\'border-top:1px solid #bbbbbb;\'></td></tr>';
					// ...Teilurlaubsanspruch für
					$tt .= '<tr><td nowrap>'.$GLOBALS['pb_string']["tt_pvacation"].' '.$GLOBALS['pb_string']["zeitspanne"].' '.get_entry($rua['akt']['datum_von'], "datum").'-'.get_entry($rua['akt']['datum_bis'], "datum").'&nbsp;</td><td align=right>&nbsp;'.$url_tj.'</td></tr>';
					// ...Urlaubsübertrag aus Vorjahr
					$tt .= '<tr><td nowrap>&nbsp;+ '.$GLOBALS['pb_string']["tt_carryvacation"].' '.($tj-1).'&nbsp;</td><td align=right>&nbsp;'.$url_vj.'</td></tr>';
					$tt .= '<tr><td colspan=2 style=\'border-bottom:1px solid #bbbbbb;\'></td></tr>';
					// ...Gesamturlaubsanspruch für
					$tt .= '<tr><td nowrap>&nbsp;= <font class=black>'.$GLOBALS['pb_string']["tt_totalvacation"].' '.$GLOBALS['pb_string']["zeitspanne"].' '.get_entry($rua['akt']['datum_von'], "datum").'-'.get_entry($rua['akt']['datum_bis'], "datum").'</font>&nbsp;</td><td align=right>&nbsp;<font class=black>'.get_entry($rua['akt']['gesamtanspruch_teiljahr'], "dezimal").'</font></td></tr>';
					// ...Urlaub bisher genommen im Jahr
					$tt .= '<tr><td nowrap>&nbsp;- '.$GLOBALS['pb_string']["tt_vacationtaken"].' '.$tj.'&nbsp;</td><td align=right>&nbsp;'.get_entry($rua['akt']['urlaub_genommen'], "dezimal").'</td></tr>';
					$tt .= '<tr><td colspan=2 style=\'border-bottom:1px solid #bbbbbb;\'></td></tr>';
					// ...Resturlaub bis Ende
					$tt .= '<tr><td nowrap bgcolor=#eeeeee>&nbsp;= <font class=black>'.$GLOBALS['pb_string']["tt_remvacation"].' '.get_entry($rua['akt']['datum_bis'], "datum").'</font>&nbsp;</td><td align=right bgcolor=#eeeeee>&nbsp;<font class=black>'.$url_rtj.'</font></td></tr>';
					$tt .= '<tr><td colspan=2 style=\'border-top:1px solid #bbbbbb;\'></td></tr>';

					$tt .= '<tr><td colspan=2>&nbsp;</td></tr>';

					$tt .= '<tr><td colspan=2 style=\'border-bottom:1px solid #bbbbbb;\'></td></tr>';
					$tt .= '<tr><td nowrap bgcolor=#dddddd colspan=2><b class=black>'.$GLOBALS['pb_string']["jahr"].': '.$tj.'</b></td></tr>';
					$tt .= '<tr><td colspan=2 style=\'border-top:1px solid #bbbbbb;\'></td></tr>';
				}

				// ...Gesamturlaubsanspruch für
				$tt .= '<tr><td nowrap>'.$GLOBALS['pb_string']["tt_tvacation"].' '.$GLOBALS['pb_string']["jahr"].' '.$tj.'&nbsp;</td><td align=right>&nbsp;'.$url_gj.'</td></tr>';
				// ...Urlaubsübertrag aus Vorjahr
				$tt .= '<tr><td nowrap>&nbsp;+ '.$GLOBALS['pb_string']["tt_carryvacation"].' '.($tj-1).'&nbsp;</td><td align=right>&nbsp;'.$url_vj.'</td></tr>';
				$tt .= '<tr><td colspan=2 style=\'border-bottom:1px solid #bbbbbb;\'></td></tr>';
				// ...Gesamturlaubsanspruch für
				$tt .= '<tr><td nowrap>&nbsp;= <font class=black>'.$GLOBALS['pb_string']["tt_totalvacation"].' '.$GLOBALS['pb_string']["jahr"].' '.$tj.'</font>&nbsp;</td><td align=right>&nbsp;<font class=black>'.get_entry($rua['akt']['gesamtanspruch_ganzjahr'], "dezimal").'</font></td></tr>';
				// ...Urlaub bisher genommen im Jahr
				$tt .= '<tr><td nowrap>&nbsp;- '.$GLOBALS['pb_string']["tt_vacationtaken"].' '.$tj.'&nbsp;</td><td align=right>&nbsp;'.get_entry($rua['akt']['urlaub_genommen'], "dezimal").'</td></tr>';
				$tt .= '<tr><td colspan=2 style=\'border-bottom:1px solid #bbbbbb;\'></td></tr>';
				// ...Resturlaub bis Ende
				$tt .= '<tr><td nowrap bgcolor=#eeeeee>&nbsp;= <font class=black>'.$GLOBALS['pb_string']["tt_remvacation"].' '.$tj.'</font>&nbsp;</td><td align=right bgcolor=#eeeeee>&nbsp;<font class=black>'.$url_rgj.'</font></td></tr>';
				$tt .= '<tr><td colspan=2 style=\'border-top:1px solid #bbbbbb;\'></td></tr>';
				$tt .= '</table>';
			}
			else
			{
				$url_vj='<i class="lightgrey">x</i>';
				$url_tj='<i class="lightgrey">x</i>';
				$url_gj='<i class="lightgrey">x</i>';
				$url_rtj='<i class="lightgrey">x</i>';
				$url_rgj='<i class="lightgrey">x</i>';
			}

			// Urlaubsübertrag aus Vorjahr
			$GLOBALS['fxptdata']['cal'][$aktzeile][$utage-3]['text']='&nbsp;<font class="green">'.$url_vj.'</font>&nbsp;';
			$GLOBALS['fxptdata']['cal'][$aktzeile][$utage-3]['align']	= 'right';
			$GLOBALS['fxptdata']['cal'][$aktzeile][$utage-3]['typ']		= 'All';
			$GLOBALS['fxptdata']['cal'][$aktzeile][$utage-3]['class']	= 'yat'.($aktzeile%2);

			// Teil. (Jahres-) Urlaub
			$GLOBALS['fxptdata']['cal'][$aktzeile][$utage-2]['text']='&nbsp;<font class="blue">'.$url_tj;
			if(substr($bis, 4, 4) != '1231')
				$GLOBALS['fxptdata']['cal'][$aktzeile][$utage-2]['text'] .= '&nbsp;('.$url_gj.')';
			$GLOBALS['fxptdata']['cal'][$aktzeile][$utage-2]['text'] .= '</font>&nbsp;';
			$GLOBALS['fxptdata']['cal'][$aktzeile][$utage-2]['align']	= 'right';
			$GLOBALS['fxptdata']['cal'][$aktzeile][$utage-2]['typ']		= 'All';
			$GLOBALS['fxptdata']['cal'][$aktzeile][$utage-2]['class']	= 'yat'.($aktzeile%2);

			// Resturlaub
			$GLOBALS['fxptdata']['cal'][$aktzeile][$utage-1]['text']='&nbsp;'.$url_rtj;
			if(substr($bis, 4, 4) != '1231')
				$GLOBALS['fxptdata']['cal'][$aktzeile][$utage-1]['text'] .= '&nbsp;('.$url_rgj.')';
			$GLOBALS['fxptdata']['cal'][$aktzeile][$utage-1]['text'] .= '<img src="'.$GLOBALS['gfxpath'].'tts.png" align="top" style="cursor:help;">&nbsp;';
			$GLOBALS['fxptdata']['cal'][$aktzeile][$utage-1]['tooltip_js']	= $th.'<br />'.$tt;
			$GLOBALS['fxptdata']['cal'][$aktzeile][$utage-1]['align']		= 'right';
			$GLOBALS['fxptdata']['cal'][$aktzeile][$utage-1]['typ']			= 'All';
			$GLOBALS['fxptdata']['cal'][$aktzeile][$utage-1]['class']		= 'crk'.(($aktzeile+1)%2);

			$GLOBALS['fxptdata']['cal'][$aktzeile][0]['tooltip_js']	.= '<br /><br /><hr /><br />'.$tt;

			// Urlaubsantrag
			$GLOBALS['fxptdata']['cal'][$aktzeile][$utage]['text']		= '&nbsp;<font class="lightgrey">x</font>&nbsp;';
			$GLOBALS['fxptdata']['cal'][$aktzeile][$utage]['align']		= 'right';
			$GLOBALS['fxptdata']['cal'][$aktzeile][$utage]['typ']		= 'All';
			$GLOBALS['fxptdata']['cal'][$aktzeile][$utage]['class']		= 'yat'.($aktzeile%2);
			// Urlaub
			$GLOBALS['fxptdata']['cal'][$aktzeile][$utage+1]['text']	= '&nbsp;<font class="grey">x</font>&nbsp;';
			$GLOBALS['fxptdata']['cal'][$aktzeile][$utage+1]['align']	= 'right';
			$GLOBALS['fxptdata']['cal'][$aktzeile][$utage+1]['typ']		= 'All';
			$GLOBALS['fxptdata']['cal'][$aktzeile][$utage+1]['class']	= 'crk'.(($aktzeile+1)%2);
		}

		// Zeile x (Spalte "Sonderurlaub")
		if($sutage)
		{
			// Sonderurlaubsantrag
			$GLOBALS['fxptdata']['cal'][$aktzeile][$sutage]['text']		= '&nbsp;<font class="lightgrey">x</font>&nbsp;';
			$GLOBALS['fxptdata']['cal'][$aktzeile][$sutage]['align']	= 'right';
			$GLOBALS['fxptdata']['cal'][$aktzeile][$sutage]['typ']		= 'All';
			$GLOBALS['fxptdata']['cal'][$aktzeile][$sutage]['class']	= 'yat'.($aktzeile%2);
			// Sonderurlaub
			$GLOBALS['fxptdata']['cal'][$aktzeile][$sutage+1]['text']	= '&nbsp;<font class="grey">x</font>&nbsp;';
			$GLOBALS['fxptdata']['cal'][$aktzeile][$sutage+1]['align']	= 'right';
			$GLOBALS['fxptdata']['cal'][$aktzeile][$sutage+1]['typ']	= 'All';
			$GLOBALS['fxptdata']['cal'][$aktzeile][$sutage+1]['class']	= 'crk'.(($aktzeile+1)%2);
		}

		// Zeile x (Spalte "Schulung")
		if($stage)
		{
			// Schulungsantrag
			$GLOBALS['fxptdata']['cal'][$aktzeile][$stage]['text']		= '&nbsp;<font class="lightgrey">x</font>&nbsp;';
			$GLOBALS['fxptdata']['cal'][$aktzeile][$stage]['align']		= 'right';
			$GLOBALS['fxptdata']['cal'][$aktzeile][$stage]['typ']		= 'All';
			$GLOBALS['fxptdata']['cal'][$aktzeile][$stage]['class']		= 'yat'.($aktzeile%2);
			// Schulung
			$GLOBALS['fxptdata']['cal'][$aktzeile][$stage+1]['text']	= '&nbsp;<font class="grey">x</font>&nbsp;';
			$GLOBALS['fxptdata']['cal'][$aktzeile][$stage+1]['align']	= 'right';
			$GLOBALS['fxptdata']['cal'][$aktzeile][$stage+1]['typ']		= 'All';
			$GLOBALS['fxptdata']['cal'][$aktzeile][$stage+1]['class']	= 'crk'.(($aktzeile+1)%2);
		}

		// Zeile x (Spalte "Krank")
		if($ktage)
		{
			// Krankmeldung
			$GLOBALS['fxptdata']['cal'][$aktzeile][$ktage]['text']		= '&nbsp;<font class="lightgrey">x</font>&nbsp;';
			$GLOBALS['fxptdata']['cal'][$aktzeile][$ktage]['align']		= 'right';
			$GLOBALS['fxptdata']['cal'][$aktzeile][$ktage]['typ']		= 'All';
			$GLOBALS['fxptdata']['cal'][$aktzeile][$ktage]['class']		= 'yat'.($aktzeile%2);
			// Krank
			$GLOBALS['fxptdata']['cal'][$aktzeile][$ktage+1]['text']	= '&nbsp;<font class="grey">x</font>&nbsp;';
			$GLOBALS['fxptdata']['cal'][$aktzeile][$ktage+1]['align']	= 'right';
			$GLOBALS['fxptdata']['cal'][$aktzeile][$ktage+1]['typ']		= 'All';
			$GLOBALS['fxptdata']['cal'][$aktzeile][$ktage+1]['class']	= 'crk'.(($aktzeile+1)%2);
		}

		// Zeile x (Spalte "Gleittag")
		if($gtage)
		{
			// Gleittagantrag
			$GLOBALS['fxptdata']['cal'][$aktzeile][$gtage]['text']		= '&nbsp;<font class="lightgrey">x</font>&nbsp;';
			$GLOBALS['fxptdata']['cal'][$aktzeile][$gtage]['align']		= 'right';
			$GLOBALS['fxptdata']['cal'][$aktzeile][$gtage]['typ']		= 'All';
			$GLOBALS['fxptdata']['cal'][$aktzeile][$gtage]['class']		= 'yat'.($aktzeile%2);
			// Gleittag
			$GLOBALS['fxptdata']['cal'][$aktzeile][$gtage+1]['text']	= '&nbsp;<font class="grey">x</font>&nbsp;';
			$GLOBALS['fxptdata']['cal'][$aktzeile][$gtage+1]['align']	= 'right';
			$GLOBALS['fxptdata']['cal'][$aktzeile][$gtage+1]['typ']		= 'All';
			$GLOBALS['fxptdata']['cal'][$aktzeile][$gtage+1]['class']	= 'crk'.(($aktzeile+1)%2);
		}

		// Zeile x (Spalte "Abwesend")
		if($atage)
		{
			// Abwesend
			$GLOBALS['fxptdata']['cal'][$aktzeile][$atage]['text']		= '&nbsp;<font class="grey">x</font>&nbsp;';
			$GLOBALS['fxptdata']['cal'][$aktzeile][$atage]['align']		= 'right';
			$GLOBALS['fxptdata']['cal'][$aktzeile][$atage]['typ']		= 'All';
			$GLOBALS['fxptdata']['cal'][$aktzeile][$atage]['class']		= 'cnb'.(($aktzeile+1)%2);
		}

		// Zeile x (Spalte "Ist-Stunden")
		if($itage)
		{
			// Ist-Stunden
			$GLOBALS['fxptdata']['cal'][$aktzeile][$itage]['text']		= '&nbsp;<font class="grey">x</font>&nbsp;';
			$GLOBALS['fxptdata']['cal'][$aktzeile][$itage]['align']		= 'right';
			$GLOBALS['fxptdata']['cal'][$aktzeile][$itage]['typ']		= 'All';
			$GLOBALS['fxptdata']['cal'][$aktzeile][$itage]['class']		= 'rg'.(($aktzeile+1)%2);
		}

		// Zeile x (Spalte "Info")
		if($GLOBALS['fxpglobals']['persdat']['roles'][-1] || $GLOBALS['fxpglobals']['persdat']['roles'][2] || ($persid == $GLOBALS['fxpglobals']['person']))
			$GLOBALS['fxptdata']['cal'][$aktzeile][$xoffset-1]['text']='<span '.fxf_jsFunction('fxLink', false, 'loader.php?url=prs_info.inc&pid='.$persid, false).' tooltip="Info" style="cursor:pointer;border:1px solid #cccccc;border-radius:4px;padding:0 4px;">?</span>';
		else
			$GLOBALS['fxptdata']['cal'][$aktzeile][$xoffset-1]['text']='<font class="lightred">x</font>';
		$GLOBALS['fxptdata']['cal'][$aktzeile][$xoffset-1]['align']	= 'center';
		$GLOBALS['fxptdata']['cal'][$aktzeile][$xoffset-1]['class']	= 'yat'.($aktzeile%2);
		$GLOBALS['fxptdata']['cal'][$aktzeile][$xoffset-1]['typ']	= 'All';

		// Falls keine Stunden- oder Sondertage ermittelt wurden, dann nur leeren Kalender anzeigen
		if(!$wdc && !$sdc)
		{
			if($ka_debug && $ka_debug <= 3)
				echo('<B>Keine Sondertage, deshalb leere Kalenderzeile anzeigen</B><BR>');
			$GLOBALS['fxptdata']['cal'][$aktzeile][$xoffset]['colspan']=$xcolumns-$xoffset;
			$aktzeile++;
		}

		// Stunden- oder Sondertage gefunden
		else
		{
			// Sondertage aufspalten
			$aktspalte=$xoffset;

			$abw	= 0.0;	// Abwesend
			$url_a	= 0.0;	// Urlaub	(Antrag)
			$url_g	= 0.0;	// Urlaub	(genehmigt)
			$surl_a	= 0.0;	// Sonderurlaub	(Antrag)
			$surl_g	= 0.0;	// Sonderurlaub	(genehmigt)
			$sch_a	= 0.0;	// Schulung	(Antrag)
			$sch_g	= 0.0;	// Schulung	(genehmigt)
			$kra_a	= 0.0;	// Krank	(Meldung)
			$kra_g	= 0.0;	// Krank	(genehmigt)
			$gle_a	= 0.0;	// Gleittag	(Antrag)
			$gle_g	= 0.0;	// Gleittag	(genehmigt)
			$std	= 0;	// Arbeitsstunden

			$bit=array(64, 1, 2, 4, 8, 16, 32); // So, Mo, Di, Mi, Do, Fr, Sa
			$stag 	= (int) substr($von, 6, 2);
			$smonat=(int) substr($von, 4, 2);
			$sjahr 	= (int) substr($von, 0, 4);
			$stg=fx_date('w', fx_mktime(0, 0, 0, $smonat, $stag, $sjahr));

			if($ka_debug == 6)
			{
				fxDebug($person, 'Arbeitstage');
				fxDebug($sdays, 'Sondertage');
				echo('<B>$stag</B>="'.$stag.', <B>$smonat</B>="'.$smonat.', <B>$sjahr</B>="'.$sjahr.', <B>$stg</B>="'.$stg.', <B>$xoffset</B>="'.$xoffset.', <B>$xcolumns</B>="'.$xcolumns.', <B>$einheit</B>="'.$einheit.'"<BR>');
			}

			// Array vorbereiten: Arbeitstage eintragen mit '#', und freie Tage mit '_'
			$warr=array();
			$darr=array();
			for($spalte=$xoffset; $spalte<$xcolumns; $spalte++)
			{
				$warr[$spalte]=0;

				if($einheit == 'D')
				{
					$atag=($stg+$spalte-$xoffset)%7;
					if($person['tage'] & $bit[$atag]) // Arbeitstag
						$darr[$spalte]='#';
					else
						$darr[$spalte]='_';
				}
				else
					$darr[$spalte]='#';
			}
			$dinf=array();

			// Arbeitsstunden eintragen
			if($wdc)
			{
				foreach($wdays as $wdate => $wdata)
				{
					// Aktuellen Tagesindex ermitteln
					$pd=fxp_date_diff($von,$wdate,$einheit) + $xoffset;
					if(($pd >= $xoffset) && ($pd < $xcolumns))
					{
						$warr[$pd] += $wdata[-1];
						$std += $wdata[-1];
					}
				}
			}

			// Feiertage in Verarbeitungs-Array eintragen, nur falls Tagesanzeige
			if(($einheit == 'D') && sizeof($holidays))
			{
				foreach($holidays as $datum => $ft)
				{
					// Aktuellen Tagesindex ermitteln
					$ptag=fxp_date_diff($von, $datum, $einheit) + $xoffset;
					if(($ptag < $xoffset) || ($ptag >= $xcolumns) || ($darr[$ptag] == '_')) // Index nicht im Anzeigebereich oder freier Tag, dann nächster Eintrag
						continue;
					if($ft['art'] == FXP_HL_FULL)	// Ganzer Feiertag
						$darr[$ptag]='_';
				}
			}

			// Was darf der MA sehen?
			$ds_abw=true;
			$ds_url	= false;
			$ds_sch	= false;
			$ds_kra	= false;
			$ds_gle	= false;
			$ds_std=false;
			// Verwaltung, Geschäftsführer oder Selbst -> alle Daten anzeigen
			if($GLOBALS['fxpglobals']['persdat']['roles'][-1] || $GLOBALS['fxpglobals']['persdat']['roles'][2] || ($persid == $GLOBALS['fxpglobals']['person']))
			{
				$ds_url	= true;
				$ds_sch	= true;
				$ds_kra	= true;
				$ds_gle	= true;
				$ds_std=true;
			}
			// ansonsten je nach Einstellungen
			else
			{
				// Ist MA in selber Abteilung
				$inAbt=false;
				if(!$GLOBALS['own_pabt'])			// Zugeordnet zur Firma, deshalb in allen Abteilungen
					$inAbt=true;
				else if($GLOBALS['own_pabt'] > 0)	// sonst prüfen
				{
					$pabt=db_value("SELECT abt_id FROM personen WHERE personen_id=".$persid." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
					if(!strlen($pabt))
						$pabt=0;
					if(fxIsArray($GLOBALS['own_eabts']))
					{
						foreach($GLOBALS['own_eabts'] as $vglabt)
						{
							if($pabt == $vglabt['abt_id'])
							{
								$inAbt=true;
								break;
							}
						}
					}
				}

				// Urlaub bzw. Sonderurlaub
				if(((int)$GLOBALS['fxpglobals']['settings']['up_urlaub'] == 3) || (((int)$GLOBALS['fxpglobals']['settings']['up_urlaub'] == 2) && $inAbt))
					$ds_url=true;
				// Schulung
				if(((int)$GLOBALS['fxpglobals']['settings']['up_schulung'] == 3) || (((int)$GLOBALS['fxpglobals']['settings']['up_schulung'] == 2) && $inAbt))
					$ds_sch=true;
				// Krank
				if(((int)$GLOBALS['fxpglobals']['settings']['up_krank'] == 3) || (((int)$GLOBALS['fxpglobals']['settings']['up_krank'] == 2) && $inAbt))
					$ds_kra=true;
				// Gleittag
				if(((int)$GLOBALS['fxpglobals']['settings']['up_gleit'] == 3) || (((int)$GLOBALS['fxpglobals']['settings']['up_gleit'] == 2) && $inAbt))
					$ds_gle=true;
			}

			$sptag=0;
			$cnt=0;
			$datum_r=0;
			foreach($sdays as $datum => $stype)
			{
				// Aktuellen Tagesindex ermitteln
				if(!$datum_r)
					$datum_r=$datum;
				$ptag=fxp_date_diff($von, $datum, $einheit) + $xoffset;
				if((($ptag < $xoffset) || ($ptag >= $xcolumns)) && $stype) // Index nicht im Anzeigebereich, dann nächster Eintrag
					continue;

				// Anzeige-Hintergrund ermitteln
				$bg		= '';
				$bgadd	= 0.0;
				$dae	= substr($darr[$ptag],0,1);
				foreach($stype as $st => $sdata)
				{
					$valid=false;
					if($ds_abw && ($st == FXP_AT_ABSENT))
						$valid=true;
					if($_POST['Urlaub'] && $ds_url && (($st == FXP_AT_VACATIONREQUEST) || ($st == FXP_AT_VACATION) || ($st == FXP_AT_SPECIALVACATIONREQUEST) || ($st == FXP_AT_SPECIALVACATION)))
						$valid=true;
					if($_POST['Schulung'] && $ds_sch && (($st == FXP_AT_TRAININGREQUEST) || ($st == FXP_AT_TRAINING)))
						$valid=true;
					if($_POST['Krank'] && $ds_kra && (($st == FXP_AT_SICKCALL) || ($st == FXP_AT_SICK)))
						$valid=true;
					if($_POST['Gleitzeit'] && $ds_gle && (($st == FXP_AT_SLIDINGDAYREQUEST) || ($st == FXP_AT_SLIDINGDAY)))
						$valid=true;

					if(!$valid) // nichts passendes gefunden, dann nächster Eintrag
						continue;
					else
					{
						$bg='au'.$st;
						$bn=$GLOBALS['farbnamen'][$GLOBALS['bwid'][(int)substr($bg, 2)]];
						$bgadd=$sdata[-1]['dauer'];
						if($holidays[$datum]['art'] == FXP_HL_HALF)
							$bgadd=0.5;
					}

					// Gültige Tage aufaddieren: nur Arbeitstage
					$valid_day=false;
					if((($einheit != 'D') || ($dae != '_')) && ($holidays[$datum]['art'] != FXP_HL_FULL))
					{
						$atag=($stg + fxp_date_diff($von, $datum, 'D'))%7;
						if($person['tage'] & $bit[$atag]) // Arbeitstag
						{
							$valid_day=true;
//echo('<B>Arbeitstag:</B> $datum="'.$datum.'", $atag="'.$atag.'"<BR>');
							if($utage)
							{
								if($st == FXP_AT_VACATIONREQUEST)
									$url_a += $bgadd;
								else if($st == FXP_AT_VACATION)
									$url_g += $bgadd;
							}
							if($sutage)
							{
								if($st == FXP_AT_SPECIALVACATIONREQUEST)
									$surl_a += $bgadd;
								else if($st == FXP_AT_SPECIALVACATION)
									$surl_g += $bgadd;
							}
							if($stage)
							{
								if($st == FXP_AT_TRAININGREQUEST)
									$sch_a += $bgadd;
								else if($st == FXP_AT_TRAINING)
									$sch_g += $bgadd;
							}
							if($ktage)
							{
								if($st == FXP_AT_SICKCALL)
									$kra_a += $bgadd;
								else if($st == FXP_AT_SICK)
									$kra_g += $bgadd;
							}
							if($gtage)
							{
								if($st == FXP_AT_SLIDINGDAYREQUEST)
									$gle_a += $bgadd;
								else if($st == FXP_AT_SLIDINGDAY)
									$gle_g += $bgadd;
							}
							if($atage)
							{
								if($st == FXP_AT_ABSENT)
									$abw += $bgadd;
							}
						}
					}

					// Hintergrundgrafik setzen
					$bgz='';
					if($bgadd == 0.5)
						$bgz='_2';
					if($dae == '#')
						$darr[$ptag]=$bg.$bgz;
					else if($dae == '_')
						$darr[$ptag] .= $bg.$bgz;

					// Eintrag in Info-Array
					for($tc=$sptag; $tc<$ptag; $tc++)
					{
						if(substr($darr[$tc],0,1) == '#')
						{
							$sptag=0;
							$cnt=0;
							break;
						}
					}
					if(($einheit != 'D') || !$sptag)
						$sptag=$ptag;
					if($einheit != 'D')
						$ddiff=fxp_date_diff($datum_r, $datum, 'D');
					else
						$ddiff=1;
					$ptag=fxp_date_diff($von, $datum, $einheit) + $xoffset;
					if(($ddiff != 1) || ($dinf[$sptag][$cnt]['bg'] != $bg)) // Neue Sondertags-Kategorie
					{
						$success=false;
						$sptag=$ptag;
						if(fxIsArray($dinf[$sptag])) // Bereits Eintrag vorhanden
							$cnt=sizeof($dinf[$sptag]);
						else
							$cnt=0;
						$dinf[$sptag][$cnt]['bg']=$bg;
						$dinf[$sptag][$cnt]['bn']=$bn;
						if((substr($darr[$sptag], 0, 1) == '_') || ($holidays[$datum]['art'] == FXP_HL_FULL))
						{
							$dinf[$sptag][$cnt]['fdc']=0;
							$dinf[$sptag][$cnt]['hdc']=0;
							$dinf[$sptag][$cnt]['dc']=0;
							$success=true;
						}
						else if($valid_day)
						{
							if($bgadd == 1)
							{
								$dinf[$sptag][$cnt]['fdc']=1;
								$dinf[$sptag][$cnt]['hdc']=0;
							}
							else
							{
								$dinf[$sptag][$cnt]['fdc']=0;
								$dinf[$sptag][$cnt]['hdc']=1;
							}
							$dinf[$sptag][$cnt]['dc']=$bgadd;
							$success=true;
						}
						if(!$dinf[$sptag][$cnt]['dc'])
							$success=false;
						if($success)
						{
							$dinf[$sptag][$cnt]['sd']	= $datum;	// Startdatum
							$dinf[$sptag][$cnt]['ed']	= $datum;	// Enddatum
						}
						else
							unset($dinf[$sptag][$cnt]);
					}
					else if(fxIsArray($dinf[$sptag][$cnt]))
					{
						if($valid_day && ((substr($darr[$ptag], 0, 1) != '_') || ($holidays[$datum]['art'] != FXP_HL_FULL)))
						{
							if($bgadd == 1)
								$dinf[$sptag][$cnt]['fdc']++;
							else
								$dinf[$sptag][$cnt]['hdc']++;
							$dinf[$sptag][$cnt]['dc'] += $bgadd;
						}
						$dinf[$sptag][$cnt]['ed']=$datum;
					}
				}
				$datum_r=$datum;
			}

			// Sondertage in Kalender einfügen
			if($darr)
			{
				if($ka_debug == 6)
				{
					fxDebug($darr, 'Urlaub/Schulung/Krank - Array');
					fxDebug($dinf, 'Urlaub/Schulung/Krank - Info');
				}

				// Recht auf Transaktion 27
				$recht_te=tr_rechte(27);	// Rechte für Termine

				// 1. Daten initialisieren
				$aktspalte_r	= $xoffset;
				$aktspalte		= $xoffset;
				$bg				= '-';
				$tm				= false;
				$whr			= 0;
				$colspan		= 0;
				$toffset		= fxp_date_diff($von,substr($GLOBALS['datetime'],0,8),$einheit) + $xoffset;

				// Spaltenzähler
				$spalte=$xoffset;
//fxDebug($darr, '$darr');

				while(true)
				{
					$nwhr=(int)$warr[$spalte];

					// Neuen Hintergrund ermitteln
					$nbg=$darr[$spalte];
					$nbf=substr($nbg,0,1);
					if($nbf == '_')
						$nbg='';
					else if($nbf == '#')
						$nbg='nv';
					$ntm=false;
					if($toffset == $spalte)
						$ntm=true;

					if($bg == '-')
					{
						$whr=$nwhr;
						$bg=$nbg;
						$tm=$ntm;
					}

					if(($spalte >= $xoffset) && (($whr != $nwhr) || ($bg != $nbg) || ($tm != $ntm) || ($einheit != 'D') || ($spalte >= $xcolumns)))
					{
						if($ka_debug == 6)
							echo('<b class="green">DISPLAY</b> ('.$spalte.'): $aktspalte="'.$aktspalte.'", $aktspalte_r="'.$aktspalte_r.'", $colspan="'.$colspan.'", $bg="'.$bg.'", $nbg="'.$nbg.'", $whr='.$whr.'<br>');
						// Anzeige der bisher gesammelten Infos
						$GLOBALS['fxptdata']['cal'][$aktzeile][$aktspalte]['colspan']=$colspan;
						if($whr)
						{
							$GLOBALS['fxptdata']['cal'][$aktzeile][$aktspalte]['class']='s2';
							$GLOBALS['fxptdata']['cal'][$aktzeile][$aktspalte]['align']='right';
							$GLOBALS['fxptdata']['cal'][$aktzeile][$aktspalte]['itext']='&nbsp;'.get_entry(round($whr/3600), 'ganzzahl').'&nbsp;';
						}
						if($tm)
							$GLOBALS['fxptdata']['cal'][$aktzeile][$aktspalte]['style']='border-left:2px solid #ffffff;border-right:1px solid #666666;';
						if(strlen($bg))
						{
							$rt=getDayAppr((int)substr($bg,2));
							if($whr &&  ($einheit != 'D'))
								$GLOBALS['fxptdata']['cal'][$aktzeile][$aktspalte]['typ']='nv';
							else if($bg == 'nv')
								$GLOBALS['fxptdata']['cal'][$aktzeile][$aktspalte]['typ']='nv';
							else
							{
								$GLOBALS['fxptdata']['cal'][$aktzeile][$aktspalte]['typ']='background';
								$GLOBALS['fxptdata']['cal'][$aktzeile][$aktspalte]['background']=$bg;
								if(!$whr && strlen($rt))
								{
									$GLOBALS['fxptdata']['cal'][$aktzeile][$aktspalte]['align']='center';
									$GLOBALS['fxptdata']['cal'][$aktzeile][$aktspalte]['itext']=$rt;
								}
							}
							if((substr($bg, -1) == '2') && ($dinf[$aktspalte][0]['dc'] >= 1) && ($einheit != 'D'))
								$GLOBALS['fxptdata']['cal'][$aktzeile][$aktspalte]['background']=substr($bg, 0, -1);

							// Tooltip
							if(($bg != 'nv') && $aktspalte_r && fxIsArray($dinf[$aktspalte_r]) && fxIsArray($dinf[$aktspalte_r][0]) && ($aktspalte < $aktspalte_r+$dinf[$aktspalte_r][0]['dc']))
							{
								$tooltip_js='<span class=\'s4b black\'>'.$person['fullname'].'</span><br><br>';
								$tooltip_js .= '<TABLE BORDER=0 CELLSPACING=0 CELLPADDING=1>';
								for($tcnt=0; $tcnt<sizeof($dinf[$aktspalte_r]); $tcnt++)
								{
									if($tcnt)
										$tooltip_js .= '<TR><TD COLSPAN=2 WIDTH=100%><hr class=fxhr style=margin:8px></TD></TR>';
									if($tcnt == 8)	// max. 8 Tooltipeinträge je Spalte anzeigen
									{
										$tooltip_js .= '<TR><TD COLSPAN=2 WIDTH=100% ALIGN=center>:</TR>';
										break;
									}

									$tooltip_js .= '<TR><TD NOWRAP COLSPAN=2><u class=black>'.$dinf[$aktspalte_r][$tcnt]['bn'].'</u></TD></TR>';
									$tooltip_js .= '<TR><TD NOWRAP>&middot; '.$GLOBALS['pb_string']['zeitspanne'].':</TD><TD NOWRAP>'.get_entry($dinf[$aktspalte_r][$tcnt]['sd'], 'datum');
									if($dinf[$aktspalte_r][$tcnt]['sd'] != $dinf[$aktspalte_r][$tcnt]['ed'])
										$tooltip_js	.= '&nbsp;-&nbsp;'.get_entry($dinf[$aktspalte_r][$tcnt]['ed'], 'datum');
									$tooltip_js .= '</TR><TR><TD NOWRAP>&middot; '.$GLOBALS['pb_string']['tage'].':</TD><TD NOWRAP>'.$dinf[$aktspalte_r][$tcnt]['dc'].'/'.($dinf[$aktspalte_r][$tcnt]['fdc']+$dinf[$aktspalte_r][$tcnt]['hdc']).' (';
									if($dinf[$aktspalte_r][$tcnt]['fdc'])
										$tooltip_js	.= $dinf[$aktspalte_r][$tcnt]['fdc'].'x1';
									if($dinf[$aktspalte_r][$tcnt]['hdc'])
									{
										if($dinf[$aktspalte_r][$tcnt]['fdc'])
											$tooltip_js	.= '&nbsp;+&nbsp;';
										$tooltip_js	.= $dinf[$aktspalte_r][$tcnt]['hdc'].'x½';
									}
									$tooltip_js	.= ')</TD></TR>';
								}
								$tooltip_js .= '</TABLE>';
								$GLOBALS['fxptdata']['cal'][$aktzeile][$aktspalte]['tooltip_js']=$tooltip_js;
								if($whr)
									$GLOBALS['fxptdata']['cal'][$aktzeile][$aktspalte]['tooltip_ic']=true;
							}
						}

						// Neue Infos
						$aktspalte	= $spalte;
						$whr		= $nwhr;
						$bg			= $nbg;
						$tm			= $ntm;
						$colspan	= 1;
					}
					else
					{
						$colspan++;
						if($ka_debug == 6)
							echo('<b class="blue">SAME</b> ('.$spalte.'): $colspan='.$colspan.'</B><BR>');
					}

					// Neuer Index
					if($dinf[$spalte])
					{
						if($ka_debug == 6)
							echo('<B><FONT COLOR="#000088">-> NEUER INDEX</FONT> -- '.$spalte.'</B><BR>');
						$aktspalte_r=$spalte;
					}

					// Abbruch, falls Zähler über Tagesanzahl
					if($spalte >= $xcolumns)
					{
						if($ka_debug == 6)
							echo('<B><FONT COLOR="#880000">-> ABBRUCH</FONT></B><BR>');
						break;
					}

					if($ka_debug == 6)
						echo('<B>'.substr('000'.$spalte, -3).':</B> $aktspalte="'.$aktspalte.'", $aktspalte_r="'.$aktspalte_r.'", $colspan="'.$colspan.'", $bg="'.$bg.'", $nbg="'.$nbg.'"<BR>');

					$spalte++;
				}
			}

			// (Antr.) Urlaub
			if($utage && ($GLOBALS['fxpglobals']['persdat']['roles'][-1] || $GLOBALS['fxpglobals']['persdat']['roles'][2] || ($persid == $GLOBALS['fxpglobals']['person'])))
			{
				// (Antr.)
				if($url_a)
				{
					$usk_a=get_entry($url_a, 'dezimal');
					$ma_akt=true;
				}
				else
					$usk_a=get_entry(0, 'dezimal');
				$GLOBALS['fxptdata']['cal'][$aktzeile][$utage]['text']='&nbsp;<font class="darkgrey">'.$usk_a.'</font>&nbsp;';

				// Urlaub
				if($url_g)
				{
					$usk_g=get_entry($url_g, 'dezimal');
					$ma_akt=true;
				}
				else
					$usk_g=get_entry(0, 'dezimal');
				$GLOBALS['fxptdata']['cal'][$aktzeile][$utage+1]['text']='&nbsp;<font class="black">'.$usk_g.'</font>&nbsp;';
			}

			// (Antr.) Sonderurl.
			if($sutage && ($GLOBALS['fxpglobals']['persdat']['roles'][-1] || $GLOBALS['fxpglobals']['persdat']['roles'][2] || ($persid == $GLOBALS['fxpglobals']['person'])))
			{
				// (Antr.)
				if($surl_a)
				{
					$usk_a=get_entry($surl_a, 'dezimal');
					$ma_akt=true;
				}
				else
					$usk_a=get_entry(0, 'dezimal');
				$GLOBALS['fxptdata']['cal'][$aktzeile][$sutage]['text']='&nbsp;<font class="darkgrey">'.$usk_a.'</font>&nbsp;';

				// Sonderurlaub
				if($surl_g)
				{
					$usk_g=get_entry($surl_g, 'dezimal');
					$ma_akt=true;
				}
				else
					$usk_g=get_entry(0, 'dezimal');
				$GLOBALS['fxptdata']['cal'][$aktzeile][$sutage+1]['text']='&nbsp;<font class="black">'.$usk_g.'</font>&nbsp;';
			}

			// (Antr.) Schulung
			if($stage && ($GLOBALS['fxpglobals']['persdat']['roles'][-1] || $GLOBALS['fxpglobals']['persdat']['roles'][2] || ($persid == $GLOBALS['fxpglobals']['person'])))
			{
				// (Antr.)
				if($sch_a)
				{
					$usk_a=get_entry($sch_a, 'dezimal');
					$ma_akt=true;
				}
				else
					$usk_a=get_entry(0, 'dezimal');
				$GLOBALS['fxptdata']['cal'][$aktzeile][$stage]['text']='&nbsp;<font class="darkgrey">'.$usk_a.'</font>&nbsp;';

				// Schulung
				if($sch_g)
				{
					$usk_g=get_entry($sch_g, 'dezimal');
					$ma_akt=true;
				}
				else
					$usk_g=get_entry(0, 'dezimal');
				$GLOBALS['fxptdata']['cal'][$aktzeile][$stage+1]['text']='&nbsp;<font class="black">'.$usk_g.'</font>&nbsp;';
			}

			// (Mldg.) Krank
			if($ktage && ($GLOBALS['fxpglobals']['persdat']['roles'][-1] || $GLOBALS['fxpglobals']['persdat']['roles'][2] || ($persid == $GLOBALS['fxpglobals']['person'])))
			{
				// (Mldg.)
				if($kra_a)
				{
					$usk_a=get_entry($kra_a, 'dezimal');
					$ma_akt=true;
				}
				else
					$usk_a=get_entry(0, 'dezimal');
				$GLOBALS['fxptdata']['cal'][$aktzeile][$ktage]['text']='&nbsp;<font class="darkgrey">'.$usk_a.'</font>&nbsp;';

				// Krank
				if($kra_g)
				{
					$usk_g=get_entry($kra_g, 'dezimal');
					$ma_akt=true;
				}
				else
					$usk_g=get_entry(0, 'dezimal');
				$GLOBALS['fxptdata']['cal'][$aktzeile][$ktage+1]['text']='&nbsp;<font class="black">'.$usk_g.'</font>&nbsp;';
			}

			// (Antr.) Gleittag
			if($gtage && ($GLOBALS['fxpglobals']['persdat']['roles'][-1] || $GLOBALS['fxpglobals']['persdat']['roles'][2] || ($persid == $GLOBALS['fxpglobals']['person'])))
			{
				// (Antr.)
				if($gle_a)
				{
					$usk_a=get_entry($gle_a, 'dezimal');
					$ma_akt=true;
				}
				else
					$usk_a=get_entry(0, 'dezimal');
				$GLOBALS['fxptdata']['cal'][$aktzeile][$gtage]['text']='&nbsp;<font class="darkgrey">'.$usk_a.'</font>&nbsp;';

				// Gleittag
				if($gle_g)
				{
					$usk_g=get_entry($gle_g, 'dezimal');
					$ma_akt=true;
				}
				else
					$usk_g=get_entry(0, 'dezimal');
				$GLOBALS['fxptdata']['cal'][$aktzeile][$gtage+1]['text']='&nbsp;<font class="black">'.$usk_g.'</font>&nbsp;';
			}

			// Abwesend
			if($atage && ($GLOBALS['fxpglobals']['persdat']['roles'][-1] || $GLOBALS['fxpglobals']['persdat']['roles'][2] || ($persid == $GLOBALS['fxpglobals']['person'])))
			{
				if($abw)
				{
					$usk_g=get_entry($abw, 'dezimal');
					$ma_akt=true;
				}
				else
					$usk_g=get_entry(0, 'dezimal');
				$GLOBALS['fxptdata']['cal'][$aktzeile][$atage]['text']='&nbsp;<font class="black">'.$usk_g.'</font>&nbsp;';
			}

			// Ist-Stunden
			if($itage && ($GLOBALS['fxpglobals']['persdat']['roles'][-1] || $GLOBALS['fxpglobals']['persdat']['roles'][2] || ($persid == $GLOBALS['fxpglobals']['person'])))
			{
				if($std)
				{
					$usk_g=get_entry($std/3600, 'dezimal');
					$ma_akt=true;
				}
				else
					$usk_g=get_entry(0, 'dezimal');
				$GLOBALS['fxptdata']['cal'][$aktzeile][$itage]['text']='&nbsp;<font class="black">'.$usk_g.'</font>&nbsp;';
			}

			// Aktiver MA
			if($ma_akt || !$_POST['akt_ma'])
				$aktzeile++;
			else
				unset($GLOBALS['fxptdata']['cal'][$aktzeile]);
		} // END -> (Sondertage anzeigen)

	} // END -> if (Personen-Darstellung)
}
?>