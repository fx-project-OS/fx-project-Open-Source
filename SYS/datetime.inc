<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : datetime.inc                                                 //
// Version     : 24.1                                                         //
// Begin       : 2020-08-20                                                   //
// Last Change : 2024-04-29                                                   //
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
 * Function collection related to dates, times etc.
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

$GLOBALS['__loaded_'.basename(__FILE__)]=true;


////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/**
 * Replacement for PHP: date ( string $format [, int $timestamp = time() ] ) : string
 *
 * @param string $format    - Optional parameter (default = ''): PHP date and time format
 * @param string $timestamp - Optional parameter (default = ''): Local system timestamp
 *
 * @return Formatted local system timestamp
 */
function fx_date($format='', $timestamp='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$iformat=trim((string)$format);
	if(!strlen((string)$iformat))
		$iformat='YmdHns';

	// An empty timestamp will be replaced with the local system timestamp
	if(is_string($timestamp) && !strlen((string)$timestamp))
		$itimestamp=time();
	else
		$itimestamp=(int)$timestamp;

	$r=date($iformat, $itimestamp);
//echo('<b>fx_date</b>($format=&quot;'.$format.'&quot;, $timestamp='.$timestamp.')<br />&nbsp;&rarr; return [formatted date string] = date($iformat=&quot;'.$iformat.', $itimestamp='.$itimestamp.')<br />&nbsp;&rarr; &quot;<b>'.$r.'</b>&quot;<hr size=1 />');

	return $r;
}

/**
 * Replacement for PHP: getdate ([ int $timestamp = time() ] ) : array
 *
 * @param string $timestamp - Optional parameter (default = ''): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function fx_getdate($timestamp='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// An empty timestamp will be replaced with the local system time
	if(is_string($timestamp) && !strlen((string)$timestamp))
		$itimestamp=time();
	else
		$itimestamp=(int)$timestamp;

	$r=getdate($itimestamp);
//echo('<b>fx_getdate</b>($timestamp='.$timestamp.')<br />&nbsp;&rarr; return [date and time array] = getdate($itimestamp='.$itimestamp.')<br />&nbsp;&rarr; &quot;<b>'.print_r($r,true).'</b>&quot;<hr size=1 />');

	return $r;
}

/**
 * Replacment for PHP: mktime ([ int $hour = date("H") [, int $minute = date("i") [, int $second = date("s") [, int $month = date("n") [, int $day = date("j") [, int $year = date("Y") [, int $is_dst = -1 ]]]]]]] ) : int
 *
 * @param string  $hour   - Optional parameter (default = ''): ???PARAMETER???
 * @param string  $minute - Optional parameter (default = ''): ???PARAMETER???
 * @param string  $second - Optional parameter (default = ''): ???PARAMETER???
 * @param string  $month  - Optional parameter (default = ''): ???PARAMETER???
 * @param string  $day    - Optional parameter (default = ''): ???PARAMETER???
 * @param string  $year   - Optional parameter (default = ''): ???PARAMETER???
 * @param various $is_dst - Optional parameter (default = -1): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function fx_mktime($hour='',$minute='',$second='', $month='',$day='',$year='', $is_dst=-1)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Empty arguments will be replaced with values from the local system time or date
	$aa=array('year'=>"Y", 'day'=>"j", 'month'=>"n", 'second'=>"s", 'minute'=>"i", 'hour'=>"H");
	foreach($aa as $ak => $av)
	{
		if(is_string(${$ak}) && !strlen((string)${$ak}))
			${'i'.$ak}=fx_date($av);
		else
			${'i'.$ak}=(int)${$ak};
	}

	$r=mktime($ihour,$iminute,$isecond, $imonth,$iday,$iyear);
//echo('<b>fx_mktime</b>($hour='.$hour.',$minute='.$minute.',$second='.$second.', $month='.$month.',$day='.$day.',$year='.$year.', $is_dst='.$is_dst.')<br />&nbsp;&rarr; return [unix timestamp] = mktime($ihour='.$ihour.',$iminute='.$iminute.',$isecond='.$isecond.', $imonth='.$imonth.',$iday='.$iday.',$iyear='.$iyear.')<br />&nbsp;&rarr; <b>'.$r.'</b><hr size=1 />');

	return $r;
}

/**
 * ???FUNCTION???
 *
 * @param string $year - Optional parameter (default = ''): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function fx_is_leap_year($year='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// An empty year will be replaced with the local year
	if(is_string($year) && !strlen((string)$year))
		$iyear=fx_date('Y');
	else
		$iyear=(int)$year;

	$l=fx_date('L', fx_mktime(12,0,0, 1,1,$iyear));
	$r=false;
	if($l)
		$r=true;
//echo('<b>fx_is_leap_year</b>($year='.$year.')<br />&nbsp;&rarr; return [leap year] = fx_date(&quot;L&quot;, fx_mktime(12,0,0, 1,1,$iyear='.$iyear.'))<br />&nbsp;&rarr; <b>'.($r?'true':'false').'</b><hr size=1 />');

	return $r;
}

/**
 * ???FUNCTION???
 *
 * @param string $fxdatetime - Optional parameter (default = ''): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function fx_datetime_to_unix($fxdatetime='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$sfxdatetime=trim((string)$fxdatetime);

	// Split datetime string to date and time values
	$aa=array('year'=>4, 'month'=>2, 'day'=>2, 'hour'=>2, 'minute'=>2, 'second'=>2);
	$s=0;
	foreach($aa as $ak => $av)
	{
		${'i'.$ak}='';
		if(strlen((string)$sfxdatetime) > $s)
			${'i'.$ak}=(int)substr((string)$sfxdatetime,$s,$av);
		$s += $av;
	}

	$r=fx_mktime($ihour,$iminute,$isecond, $imonth,$iday,$iyear);
//echo('<b>fx_datetime_to_unix</b>($fxdatetime=&quot;'.$fxdatetime.'&quot;)<br />&nbsp;&rarr; return [unix timestamp] = fx_mktime($ihour='.$ihour.',$iminute='.$iminute.',$isecond='.$isecond.', $imonth='.$imonth.',$iday='.$iday.',$iyear='.$iyear.')<br />&nbsp;&rarr; <b>'.$r.'</b><hr size=1 />');

	return $r;
}

/**
 * formatdatetime
 * Wandelt ein DatumZeitwert $dbdatumzeit aus dem DB-Format in ein ueber
 * $formatstr definiertes Format um
 * Folgende Zeichen in $formatstr werden umgewandelt:
 * yy	Jahreszahl 2-stellig
 * yyyy	Jahreszahl 4-stellig
 * m	Monate, keine vorangestellte Null
 * mm	Monate, mit vorangestellter Null
 * MM	Monatsangabe als Text (aus Wertetab)
 * M	Monatsangabe abgekürzt (die ersten 3 Zeichen, ohne Punkt) als Text (aus Wertetab)
 * d	Tag des Monats, keine vorangestellte Null
 * dd	Tag des Monats, mit vorangestellter Null
 * w Tag der Woche.  1 = 1.Wochentag (Montag), ..., 7 = 7.Wochentag (Sonntag)
 * WW Tag der Woche als Text (aus Wertetab)
 * W Tag der Woche abgekürzt (die ersten 2 Zeichen, ohne Punkt) als Text (aus Wertetab)
 * h	Stunden im 12-Stundenformat, keine vorangestellte Null
 * hh	Stunden im 12-Stundenformat, mit vorangestellter Null
 * H	Stunden im 24-Stundenformat, keine vorangestellte Null
 * HH	Stunden im 24-Stundenformat, mit vorangestellter Null
 * x	Vor-Nachmittagssymbol fuer Stunde (PMAM)
 * n	Minuten, keine vorangestellte Null
 * nn	Minuten, mit vorangestellter Null
 * s	Sekunden, keine vorangestellte Null
 * ss	Sekunden, mit vorangestellter Null
 * Autor: Steffen, Matthias, wtj
 * letzte Änderung: 042012
 *
 * @param string $formatstr   - Optional parameter (default = ''): ???PARAMETER???
 * @param string $dbdatumzeit - Optional parameter (default = ''): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function formatdatetime($formatstr='', $dbdatumzeit='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Argumente aufbereiten
	if(!strlen((string)$formatstr))
		$formatstr = $GLOBALS['fxpglobals']['settings']['formatdatum'];

	if(!strlen((string)$dbdatumzeit))
		$dbdatumzeit = fxNow();

	// Logikprüfung: Falls $dbdatumzeit bereits formatiert vorliegt, d.h. falls es nur aus Zahlen besteht, dann gleich zurückliefern ohne erneute Formatierung
	$no_number = 0;
	$datestr = '';
	for($i=0; $i<strlen((string)$dbdatumzeit); $i++)
	{
		$c = substr((string)$dbdatumzeit, $i, 1);
		if(!is_numeric($c))
			$no_number++;
		else
			$datestr .= $c;
	}
	if($no_number)
	{
//echo('<hr>formatdatetime<i>('.$formatstr.', '.$dbdatumzeit.', '.$sprache.')</i><br>-&gt; <b style="color:red;">'.$dbdatumzeit.'</b><br>');
		return $dbdatumzeit;
	}

	$dbdatumzeit = $datestr;
	if(strlen((string)$dbdatumzeit) < 14)
		$dbdatumzeit = substr((string)$dbdatumzeit.substr((string)fxNow(), strlen((string)$dbdatumzeit)), 0, 14);

	// Formatgruppen ermitteln
	$valid		= 'ymMdwWhHxnsjk';
	$s			= -1;
	$f			= '';
	$fm_split	= array();
	for($spos=0; $spos<strlen((string)$formatstr); $spos++)
	{
		$cz = substr((string)$formatstr, $spos, 1);
		if($cz == '\\')
		{
			$spos ++;
			$cz = substr((string)$formatstr, $spos, 1);
			$sf = '?';
		}
		else if(strpos($valid, $cz) === false)
			$sf = '?';
		else
			$sf = $cz;

		if($f != $sf)
		{
			$f = $sf;
			$s++;
			$fm_split[$s]['f'] = $sf;
			$fm_split[$s]['l'] = 0;
			$fm_split[$s]['w'] = '';
		}
		$fm_split[$s]['l']++;
		$fm_split[$s]['w'] .= $cz;
	}
//fxDebug($fm_split, 'Formatgruppen von ['.$formatstr.']');

	$h24 = (int)substr((string)$dbdatumzeit, 8, 2);
	$h12 = $h24;
	if($h12 >= 13)
		$h12 -= 12;

	$result = '';
	$mnames = array();
	$dnames = array();
	foreach($fm_split as $fa)
	{
		if($fa['f'] == '?')			// Trennzeichen
			$result .= $fa['w'];
		else if($fa['f'] == 'y')	// Jahreszahl
			$result .= substr((string)str_repeat('0', $fa['l']).(int)substr((string)$dbdatumzeit, 0, 4), -$fa['l']);
		else if($fa['f'] == 'm')	// Monatszahl
			$result .= substr((string)str_repeat('0', $fa['l']).(int)substr((string)$dbdatumzeit, 4, 2), -$fa['l']);
		else if($fa['f'] == 'd')	// Tageszahl
			$result .= substr((string)str_repeat('0', $fa['l']).(int)substr((string)$dbdatumzeit, 6, 2), -$fa['l']);
		else if($fa['f'] == 'h')	// Stundenzahl (im 12-Stunden-Format)
			$result .= substr((string)str_repeat('0', $fa['l']).$h12, -$fa['l']);
		else if($fa['f'] == 'H')	// Stundenzahl (im 24-Stunden-Format)
			$result .= substr((string)str_repeat('0', $fa['l']).$h24, -$fa['l']);
		else if($fa['f'] == 'n')	// Minutenzahl
			$result .= substr((string)str_repeat('0', $fa['l']).(int)substr((string)$dbdatumzeit, 10, 2), -$fa['l']);
		else if($fa['f'] == 's')	// Sekundenzahl
			$result .= substr((string)str_repeat('0', $fa['l']).(int)substr((string)$dbdatumzeit, 12, 2), -$fa['l']);
		else if($fa['f'] == 'x')	// AM/PM
			$result .= (($h24 < 12)?'A':'P').'M';
		else if($fa['f'] == 'M')	// Monatsname (abgekürzt oder ausgeschrieben)
		{
			if(!isset($GLOBALS['_date_mnames']) || !is_array($GLOBALS['_date_mnames']) || !sizeof($GLOBALS['_date_mnames']))
				$GLOBALS['_date_mnames'] = fxf_getReferenceValues('monate');
			$mname = $GLOBALS['_date_mnames'][(int)substr((string)$dbdatumzeit, 4, 2)];
			if($fa['l'] > 1)
				$result .= $mname;
			else
				$result .= substr((string)$mname, 0, 3);
		}
		else if($fa['f'] == 'w')	// Wochentag (0=So, 1=Mo, 2=Di, ..., 6=Sa)
			$result .= getWeekday($dbdatumzeit);
		else if($fa['f'] == 'W')	// Wochentagsname (abgekürzt oder ausgeschrieben)
		{
			if(!isset($GLOBALS['_date_dnames']) || !is_array($GLOBALS['_date_dnames']) || !sizeof($GLOBALS['_date_dnames']))
				$GLOBALS['_date_dnames'] = fxf_getReferenceValues('tag');
			$dname = $GLOBALS['_date_dnames'][getWeekday($dbdatumzeit)];
			if($fa['l'] > 1)
				$result .= $dname;
			else
				$result .= substr((string)$dname, 0, 2);
		}
		else if($fa['f'] == 'j')	// 3-stelliger Tag des Jahres (001-366)
		{
			$dyear = 1+fxp_date_diff(substr((string)$dbdatumzeit, 0, 4).'0101', $dbdatumzeit);
			$result .= substr('000'.$dyear, -3);
		}
		else if($fa['f'] == 'k')	// 2-stellig Kalenderwoche des Jahres (01-54)
			$result .= substr('00'.kw($dbdatumzeit), -2);
	}

//echo('<hr>formatdatetime<i>('.$formatstr.', '.$dbdatumzeit.', '.$sprache.')</i>-&gt; <b style="color:blue;">'.$result.'</b><br>');
	return $result;
}

/**
 * KW
 * Ermittelt die Kalenderwoche zu einem Datum im DB-Format entsprechend den
 * Benutzereinstellungen:
 * -----------------------------------------------------------------------------
 * Mögliche Fälle für die erste Jahreswoche:
 * - am 1. Januar
 * - Erste 4-Tages-Woche
 * - Erste volle Woche
 * -----------------------------------------------------------------------------
 * Autor: Walter_T
 *
 * @param string $dbdatumzeit - Optional parameter (default = ''): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function kw($dbdatumzeit='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Fehler abfangen und auf Standardwerte setzen
	if($dbdatumzeit == '')
		$dbdatumzeit = fxNow();
	if(!isset($GLOBALS['fxpglobals']['settings']['ersterwt']))
		$GLOBALS['fxpglobals']['settings']['ersterwt'] = 1;
	if(!isset($GLOBALS['fxpglobals']['settings']['erstejw']))
		$GLOBALS['fxpglobals']['settings']['erstejw'] = 1;

	$ewt = (int)($GLOBALS['fxpglobals']['settings']['ersterwt']);
	if($ewt > 6)
		$ewt = 0;
	$ejw = (int)($GLOBALS['fxpglobals']['settings']['erstejw']);

	$dbj = (int)substr((string)$dbdatumzeit, 0, 4);
	$dbm = (int)substr((string)$dbdatumzeit, 4, 2);
	$dbt = (int)substr((string)$dbdatumzeit, 6, 2);

	if($ejw == 1)
	{
		$offset = fx_date('z', fx_mktime(0, 0, 0, $dbm, $dbt, $dbj));
		$kw = 1 + (int)($offset/7);
		return $kw;
	}
	else
	{
		// Prüfvariable, ob Jahr zuvor ein Schaltjahr: 0=nein, 1=ja
		$lj = fx_date('L', fx_mktime(0, 0, 0, 1, 1, ($dbj - 1)));
		// Wochentag vom 1. Januar (0 = 1.Wochentag, 1 = 2.Wochentag, ..., 6 = 7.Wochentag
		$wd = fx_date('w', fx_mktime(0, 0, 0, 1, 1, $dbj)) - $ewt;
		if($wd < 0)
			$wd += 7;
		// Kalenderwoche am Neujahrstag ermitteln
		switch($ejw)
		{
			case 3: // Erste volle Woche
				if($wd == 0)
					$nkw = 1;
				else if(($wd ==  1) || (($wd == 2) && $lj))
					$nkw = 53;
				else
					$nkw = 52;
			break;

			default: // Erste 4-Tages-Woche
				if($wd < 4)
					$nkw = 1;
				else if(($wd ==  4) || (($wd == 5) && $lj))
					$nkw = 53;
				else
					$nkw = 52;
			break;
		}
	}

	// Kalenderwoche für den angeforderten Tag ermitteln
	if(($dbm > 1) || ($dbt > 1))
	{
		$offset = fx_date('z', fx_mktime(0, 0, 0, $dbm, $dbt, $dbj));
		$kw = $nkw + (int)(($wd + $offset)/7);
		if(($nkw > 1) && ($kw > $nkw))
			$kw -= $nkw;

		// Dezember evtl. anpassen, da 53.KW = 1.KW sein kann bei Erster 4-Tages-Woche
		if(($ejw == 2) && ($dbm == 12) && ($kw > 52))
		{
			$rt = 31-$dbt;
			$dwd = ($wd + $offset)%7;
			if(($rt + $dwd) < 3)
				$kw = 1;
		}
	}
	else
		$kw = $nkw;

	return $kw;
}

/**
 * ???FUNCTION???
 *
 * @param string $dt - Optional parameter (default = ''): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function getWeekday($dt='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!strlen((string)$dt))
		$dt=fxNow();

	// Wochentag des übergebenen Datums
	$wd=fx_date('w', fx_mktime(12,0,0, (int)substr((string)$dt,4,2), (int)substr((string)$dt,6,2), (int)substr((string)$dt,0,4)));

	return $wd;
}

/**
 * ???FUNCTION???
 *
 * @param string $dt - Optional parameter (default = ''): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function getFirstWeekday($dt='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!strlen((string)$dt))
		$dt=fxNow();
	if(!isset($GLOBALS['fxpglobals']['settings']['ersterwt']))
		$GLOBALS['fxpglobals']['settings']['ersterwt']=1;

	$fwd=(int)($GLOBALS['fxpglobals']['settings']['ersterwt']);
	if(($fwd < 1) || ($fwd > 6))
		$fwd=0;

	// Wochentag des übergebenen Datums
	$wd=getWeekday($dt);

	// Differenz zum ersten Wochentag
	$dd=$fwd-$wd;
	if($dd > 0)
		$dd -= 7;

	// Ersten Wochentag zurückliefern
	$fd=fx_date('Ymd', fx_mktime(12,0,0, (int)substr((string)$dt,4,2), (int)substr((string)$dt,6,2)+$dd, (int)substr((string)$dt,0,4)));

	return $fd;
}

/**
 * ???FUNCTION???
 *
 * @param string $dt - Optional parameter (default = ''): ???PARAMETER???
 * @param string $fd - Optional parameter (default = ''): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function getLastWeekday($dt='', $fd='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!strlen((string)$dt))
		$dt=fxNow();

	// Ersten Wochentag des übergebenen Datums ermitten, ausser er wurde bereits mitgeliefert
	if(!strlen((string)$fd))
		$fd=getFirstWeekday($dt);

	// Letzer Wochentag zurückliefern
	$ld=fx_date('Ymd', fx_mktime(12,0,0, (int)substr((string)$fd,4,2), (int)substr((string)$fd,6,2)+6, (int)substr((string)$fd,0,4)));

	return $ld;
}

/**
 * strtodatetime
 * das Gegenstueck zu formatdatetime: versucht aus einem String $datumstring ueber ein
 * vorgegebenes DatumZeit-Format den DatumZeit-Wert im Datenbankformat zu ermitteln.
 * fuer moegliche Werte in $formatstr siehe function formatdatetime().
 * konnte der Wert nicht ermittelt werden, gibt die Funktion false zurueck;
 * Die Routine besteht nicht darauf, dass die Trennzeichen zwischen den Zahenwerten exakt so
 * wie im $formatstr angegeben werden muessen, hauptsache die Zahlen sind irgendwie getrennt
 * ausserdem werden Datumswerte wie z.B. 31.2.2000 automatisch umgerechnet in 2.3.2000
 * !! ACHTUNG !!
 * M bzw. m steht für Monat,
 * N bzw. n steht für miNuten !
 * Autor: Steffen
 *
 * @param various $formatstr   - Mandatory parameter: ???PARAMETER???
 * @param various $datumstring - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function strtodatetime($formatstr,$datumstring)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$formatstr	= strtolower((string)$formatstr);
	$formatstr2	='';
	$ok_y = true;
	$ok_m = true;
	$ok_d = true;
	$ok_h = true;
	$ok_n = true;
	$ok_s = true;
	// erst mal den $formatstr etwas aufbereiten
	 for($i=0; $i<strlen((string)$formatstr); $i++)
	{
		$ziffer = substr((string)$formatstr, $i, 1);
		switch($ziffer)
		{
			case 'y':
				if($ok_y)
				{
					$formatstr2 .= 'y';
					$ok_y = false;
				}
			break;

			case 'm':
				if($ok_m)
				{
					$formatstr2 .= 'm';
					$ok_m = false;
				}
			break;

			case 'd':
				if($ok_d)
				{
					$formatstr2 .= 'd';
					$ok_d = false;
				}
			break;

			case 'h':
				if($ok_h)
				{
					$formatstr2 .= 'h';
					$ok_h = false;
				}
			break;

			case 'n':
				if($ok_n)
				{
					$formatstr2 .= 'n';
					$ok_n = false;
				}
			break;

			case 's':
				if($ok_s)
				{
					$formatstr2 .= 's';
					$ok_s = false;
				}
			break;
		}
	}

 	$datumstring .= ' '; // kleiner Trick, damit die Auswertung in der for-Schleife richtig funktioniert
 	$ergebnis = '00000000000000'; // Vorgabe ist null in allen Feldern
	$aktzahl = '';
	$zahlindex = 0;
 	for($i=0; $i<strlen((string)$datumstring); $i++)
 	{
		$ziffer = substr((string)$datumstring, $i, 1);
		if(($ziffer>='0') && ($ziffer<='9'))
			$aktzahl .= $ziffer;
		else
		{
			if(strlen((string)$aktzahl))
			{
				switch(substr((string)$formatstr2, $zahlindex, 1))
				{
					case 'y':
						if($aktzahl > 9999)
							return false;
						if($aktzahl < 100)
						{
							if($aktzahl < 30)
								$aktzahl += 2000;
							else
								$aktzahl += 1900; // zweistellige Jahresangaben korrigieren
						}
						$ergebnis = substr_replace($ergebnis, nullauffuellen($aktzahl, 4), 0, 4);
					break;

					case 'm':
						if(($aktzahl == 0) || ($aktzahl > 12))
							return false;
						$ergebnis = substr_replace($ergebnis, nullauffuellen($aktzahl, 2), 4, 2);
					break;

					case 'd':
						if(($aktzahl == 0) || ($aktzahl > 31))
							return false;
						$ergebnis = substr_replace($ergebnis, nullauffuellen($aktzahl, 2), 6, 2);
					break;

					case 'h':
						if($aktzahl > 23)
							return false;
						$ergebnis = substr_replace($ergebnis, nullauffuellen($aktzahl, 2), 8, 2);
					break;

					case 'n':
						if($aktzahl > 59)
							return false;
						$ergebnis = substr_replace($ergebnis, nullauffuellen($aktzahl, 2), 10, 2);
					break;

					case 's':
						if($aktzahl > 59)
							return false;
						$ergebnis = substr_replace($ergebnis, nullauffuellen($aktzahl, 2), 12, 2);
					break;
				}
				$zahlindex++;
			}
			$aktzahl = '';
		}
	}

	return $ergebnis;
}

/**
 * dbtojulian
 * berechnet den Julianischen DatumZeitwert (in Anzahl der Tage seit 4714 v.Chr.),
 * wird hauptsaechlich intern verwendet
 * Autor: Steffen
 *
 * @param various $dbwert - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function dbtojulian($dbwert)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	return gregoriantojd(substr((string)$dbwert,4,2),substr((string)$dbwert,6,2),substr((string)$dbwert,0,4)) + substr((string)$dbwert,8,2)/24 + substr((string)$dbwert,10,2)/1440 + substr((string)$dbwert,12,2)/86400;
}

/**
 * zeitsek
 * Gibt den UNIX TimestampZeitstempel(Anzahl Sekunden seit 01.01.1970) für ein Datum (in Datenbankformat) zurück
 * $wert = Datum in Datenbankformat;
 *
 * @param various $wert - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function zeitsek($wert)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// ACHTUNG!! Da fx_mktime() Datumsangaben =0 als ungültig ansieht, werden in diesem Fall die aktuellen
	// Werte verwendet. Sinnvoll also nur, wenn sich die Datumsanteile z.B. durch Subtraktion aufheben!
	if(($y=substr((string)$wert,0,4))=="0000")
		return fx_mktime(substr((string)$wert,8,2), substr((string)$wert,10,2), substr((string)$wert,12,2), 1, 1, 2000);
	else
		return fx_mktime(substr((string)$wert,8,2), substr((string)$wert,10,2), substr((string)$wert,12,2),substr((string)$wert,4,2),substr((string)$wert,6,2),substr((string)$wert,0,4));
}

/**
 * fxp_date_diff($von, $bis, $einheit)
 * Liefert die Anzahl der Tage, Wochen, Monate oder Jahre von Start bis Ende.
 * -----------------------------------------------------------------------------
 * $von und $bis sind dabei im internen DB-Format anzugeben (yyyymmdd[hhNNss])
 * $einheit: 'Y'=Jahre, 'M'=Monate, 'W'=Wochen, 'D'=Tage (Standard)
 * -----------------------------------------------------------------------------
 * Falls $von == $bis, dann wird 0 zurückgegeben, sonst die Differenz zwischen
 * den beiden Daten.
 *
 * @param various $von     - Mandatory parameter:                 ???PARAMETER???
 * @param various $bis     - Mandatory parameter:                 ???PARAMETER???
 * @param string  $einheit - Optional parameter (default = 'D'):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function fxp_date_diff($von, $bis, $einheit='D')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$dd_debug = false;

	if(!$von || !$bis)
		return 0;

	$von = substr((string)$von, 0, 8);
	$bis = substr((string)$bis, 0, 8);
	if($von == $bis)
		return 0;

	$einheit = strtoupper(substr((string)$einheit, 0, 1));

	if($dd_debug)
		echo('$von='.$von.', $bis='.$bis.', $einheit='.$einheit.' => ');

	switch($einheit)
	{
		case 'Y':	// Jahre
			$erg = (int)substr((string)$bis, 0, 4) - (int)substr((string)$von, 0, 4);
			if($dd_debug)
				echo('<b>'.$erg.' Jahr(e)</b><br>');
		break;

		case 'M':	// Monate
			$erg = (((int)substr((string)$bis, 0, 4) - (int)substr((string)$von, 0, 4)) * 12) + (int)substr((string)$bis, 4, 2) - (int)substr((string)$von, 4, 2);
			if($dd_debug)
				echo('<b>'.$erg.' Monat(e)</b><br>');
		break;

		case 'W':	// Wochen
			// Erster Wochentag: 1=Montag
			$ewt = (int)($GLOBALS['fxpglobals']['settings']['ersterwt']);
			if($von > $bis)
			{
				$von_backup = $von;
				$von = $bis;
				$bis = $von_backup;
				$vorz = -1;
			}
			else
				$vorz = 1;

			// Wochentag vom Startdatum (0 = 1.Wochentag, 1 = 2.Wochentag, ..., 6 = 7.Wochentag
			$mt	= fx_mktime( 0, 0, 1, substr((string)$von, 4, 2), substr((string)$von, 6, 2), substr((string)$von, 0, 4));
			$wd = fx_date('w', $mt) - $ewt;
			if($wd < 0)
				$wd += 7;

			$from	= (int)($mt / 86400);
			$to		= (int)(fx_mktime( 23, 59, 59, substr((string)$bis, 4, 2), substr((string)$bis, 6, 2), substr((string)$bis, 0, 4)) / 86400);
			$erg	= (int)($vorz * ($wd+$to-$from-1) / 7);
			if($dd_debug)
				echo('<b>'.$erg.' Woche(n)</b><br>');
		break;

		default:	// Tage
			$from	= (int)(fx_mktime( 0, 0, 1, substr((string)$von, 4, 2), substr((string)$von, 6, 2), substr((string)$von, 0, 4)) / 86400);
			$to		= (int)(fx_mktime( 23, 59, 59, substr((string)$bis, 4, 2), substr((string)$bis, 6, 2), substr((string)$bis, 0, 4)) / 86400);
			$erg	= $to-$from-1;
			if($dd_debug)
				echo('<b>'.$erg.' Tag(e)</b><br>');
		break;
	}

	return $erg;
}

/**
 * ultimo
 * Liefert das Datum des Monatsletzten des angegebenen Datums.
 * Eine evtl. angegebene Zeit wird ignoriert (auf 00:00:00 gesetzt).
 *
 * @param string  $day       - Optional parameter (default = ''):   ???PARAMETER???
 * @param boolean $with_time - Optional parameter (default = true): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function ultimo($day='',$with_time=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$day=substr((string)$day,0,8);
	if(!strlen((string)$day))
		$day=substr((string)$GLOBALS['datetime'],0,8);

	return makeDate((int)substr((string)$day,0,4),1+(int)substr((string)$day,4,2),0, 12,0,0, $with_time);
}

/**
 * ???FUNCTION???
 *
 * @param various $y         - Mandatory parameter:                  ???PARAMETER???
 * @param various $m         - Mandatory parameter:                  ???PARAMETER???
 * @param various $d         - Mandatory parameter:                  ???PARAMETER???
 * @param integer $h         - Optional parameter (default = 12):    ???PARAMETER???
 * @param integer $i         - Optional parameter (default = 0):     ???PARAMETER???
 * @param integer $s         - Optional parameter (default = 0):     ???PARAMETER???
 * @param boolean $with_time - Optional parameter (default = true):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function makeDate($y,$m,$d, $h=12,$i=0,$s=0, $with_time=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$ad=fx_date("YmdHis", fx_mktime((int)$h,(int)$i,(int)$s, (int)$m,(int)$d,(int)$y));
	if($with_time)
		return $ad;
	return substr((string)$ad,0,8);
}

/**
 * time_diff
 * berechnet den Unterschied zwischen zwei DatumZeitwerten im DB-Format
 * in $einheit wird festgelegt, in welcher Masseinheit das Ergebnis ausgegeben werden soll
 * moegliche Werte fuer $einheit sind:
 * 'Y'..Jahre; 'M'..Monate; 'W'..Wochen; 'D'..Tage; 'H'..Stunden; 'N'..Minuten; 'S'..Sekunden
 * Autor: Irina
 *
 * @param various $start   - Mandatory parameter: ???PARAMETER???
 * @param various $ende    - Mandatory parameter: ???PARAMETER???
 * @param various $einheit - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function time_diff($start,$ende,$einheit)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$diffsek = zeitsek($ende)-zeitsek($start);
	$difftag = ($diffsek/86400); //Differenz in Tagen
	switch(strtoupper((string)$einheit)) //In Großbuchstaben setzen
	{
		case 'Y':
			return  ($difftag/365.24);
		case 'M':
			return  ($difftag/30.41);
		case 'W':
			return ($diffsek/86400)/7;
		case 'D':
			return ($diffsek/86400);
		case 'H':
			return ($diffsek/3600);
		case 'N':
			return ($diffsek/60);
		default:
			return $diffsek;
	}
}

/**
 * time_add
 * die Funktion erwartet als Eingabe einen Datumszeitwert, einen Summanden,
 * und die Zeiteinheit des Summanden. Zurückgegeben wird das Ergebnis der Addition.
 * Hinweis: Es können auch negative Werte "addiert" werden
 * Achtung: Beim Addieren von Jahren und Monaten nur ganze Zahlen verwenden
 * Autor: Stefan
 *
 * @param various $datetime - Mandatory parameter: ???PARAMETER???
 * @param various $addend   - Mandatory parameter: ???PARAMETER???
 * @param various $unit     - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function time_add($datetime, $addend, $unit)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }


	if($datetime === NULL)
		$datetime='';
	if($addend === NULL)
		$addend=1;
	else
		$addend=(int)$addend;
	if($unit === NULL)
		$unit='S';

	//Switch-Amweisung, in der die jeweilige Einheit in Sek.
	//zurück gerechnet wird, Ausnahmen sind Monate und Jahre
	$unit=strtoupper((string)$unit);
	switch($unit)
	{
		case 'S':				// eine Sek. entspricht einer Sek.
			$addend *= 1;
		break;

		case 'N':
			$addend *= 60;		// eine Minute entspricht 60 Sek:
		break;

		case 'H':
			$addend *= 3600;	// eine Stunde entspricht 3600 Sek.
		break;

		case 'D':
			$addend *= 86400;	// ein Tag entspricht 86400 Sek.
		break;

		case 'M':				// Monats- und Jahresaddition folgt einem etwas anderen Schema
		case 'Y':
			$addend *= 1;
			$hour=substr((string)$datetime, 8,2);	//Auftrennung des Datum-Zeit-Strings
			$minute=substr((string)$datetime, 10,2);
			$second=substr((string)$datetime, 12,2);
			$month=substr((string)$datetime, 4,2);
			$day=substr((string)$datetime, 6,2);
			$year=substr((string)$datetime, 0,4);
			if($unit == 'M')
			{
				$monthadd = $month + $addend;	//Monatsanzahl wird aufaddiert
				if($monthadd > 12)				//01.14.2000 ----> 1.02.2001
				{
					$year = $year + round(($monthadd / 12), 0);
					$monthadd = $monthadd % 12;
				}
				settype($monthadd, 'string');
				if(strlen((string)$monthadd) == 1)		//24.1.1999 ----> 24.01.1999
					$monthadd = '0'.$monthadd;

				//Datumsangaben, die außerhalb des Definitionsbereiches stehen, werden korrigiert.
				//Bsp.: 31.04.2000  --->01.05.2000
				$dbdate = fx_date("YmdHis", fx_mktime($hour, $minute, $second, $monthadd, $day, $year));
			}
			else
			{
				$yearadd = $year + $addend;			//aufaddieren der Jahre

				//Wiederzusammenfügen des Strings in DB-Format
				$dbdate=fx_date("YmdHis", fx_mktime($hour, $minute, $second, $month, $day, $yearadd));
			}
		return $dbdate;		//Rückgabe des Datums im DB-Format
	}

	// Auftrennung des Datum-Zeit-Strings um "mktime" benutzen zu können
	if($unit == 'D')
	{
		$hms=substr((string)$datetime, 8);
		$hour=12;
		$minute=0;
		$second=0;
	}
	else
	{
		$hour=(int)substr((string)$datetime, 8,2);
		$minute=(int)substr((string)$datetime, 10,2);
		$second=(int)substr((string)$datetime, 12,2);
	}
	$month=(int)substr((string)$datetime, 4,2);
	$day=(int)substr((string)$datetime, 6,2);
	$year=(int)substr((string)$datetime, 0,4);

	$dbsecond=$second+$addend;	//Gesamtzahl der Sekunden

	//Hier werden die Teile des Datumsstrings zusammen gebaut,
	//und das Datum berechnet. Aus Werten wie $dbsecond=127843s wird
	//1 Tag 11 Stunden 30 Minuten und 43 Sekunden.
	if(!$year)
		$dbdate=fx_date("YmdHis", fx_mktime($hour, $minute, $dbsecond, 1, 1, 2000));
	else
		$dbdate=fx_date("YmdHis", fx_mktime($hour, $minute, $dbsecond, $month, $day, $year));

	if($unit == 'D')
		$dbdate=substr((string)$dbdate, 0,8).$hms;

	return $dbdate;
}

/**
 * Get all stored holidays from a timeframe as an array
 *
 * @param string  $sdate - Optional parameter (default = ''):     Start date (if empty then earliest stored in db)
 * @param string  $edate - Optional parameter (default = ''):     End date (if empty then use start year if possible, else latest stored in db)
 * @param boolean $ayear - Optional parameter (default = false):  Add year as first key level to array
 * @param integer $cid   - Optional parameter (default = 0):      Client id (if 0 then current client)
 * @param integer $lng   - Optional parameter (default = 0):      Language (if 0 then current language)
 *
 * @return Array of holidays
 */
function get_feiertage($sdate='', $edate='', $ayear=false, $cid=0, $lng=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// ...Start/End date
	if(strlen((string)$sdate))
	{
		$sdate=substr(trim((string)$sdate), 0,8);
		if(strlen((string)$sdate) == 4)
			$sdate .= '0101';
	}
	if(strlen((string)$edate))
	{
		$edate=substr(trim((string)$edate), 0,8);
		if(strlen((string)$edate) == 4)
			$edate .= '1231';
	}
	else if(strlen((string)$sdate))
		$edate=substr((string)$sdate,0,4).'1231';
	if(!strlen((string)$sdate))
	{
		$sql="SELECT MIN(datum) AS hsy, MAX(datum) as hly FROM feiertage_mand WHERE mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$tmp=db_values($sql);
		if(fxIsArray($tmp))
		{
//fxDebug($tmp,$sql, 0);
			$sdate=substr((string)$tmp['hsy'],0,4).'0101';
			$edate=substr((string)$tmp['hly'],0,4).'1231';
		}
		else if(strlen((string)$edate))
			$sdate=substr((string)$edate,0,4).'0101';
	}

	// ...Add year?
	if($ayear)
		$ayear=true;
	else
		$ayear=false;

	// ...Client id
	$cid=(int)$cid;
	if(!$cid)
		$cid=$GLOBALS['fxpglobals']['client'];

	// ...Language
	$lng=(int)$lng;
	if(!$lng)
		$lng=$GLOBALS['fxpglobals']['lang'];

	// SQL to get holidays in this timespan
	$sql  = "SELECT m.datum, m.halbtaegig, n.name_feiertag FROM feiertage_mand m, feiertage_name n";
	$sql .= " WHERE m.mandanten_id=".$cid." AND n.mandanten_id IN (0,".$cid.") AND n.feiertag_name=m.feiertag_name AND n.id_sprache=".$lng;
	if(strlen((string)$sdate))
		$sql .= " AND m.datum>='".$sdate."'";
	if(strlen((string)$edate))
		$sql .= " AND m.datum<='".$edate."'";
	$sql .= " ORDER BY m.datum";
	$tmp=db_values($sql, '*');
//fxDebug($tmp,$sql, 0);

	// Insert holidays into array
	$a=array();
	if(fxIsArray($tmp))
	{
		$hta=array(0=>'0', 5=>'½', 10=>'1');
		foreach($tmp as $t)
		{
			$d=substr((string)$t['datum'], 0,8);
			$a[$d]=array('name'=>$t['name_feiertag'], 'art'=>(int)$t['halbtaegig'], 'typ'=>1.0, 'txt'=>$hta[10]);
			if($a[$d]['art'] == FXP_HL_HALF)		// Half day
				$a[$d]['typ']=0.5;
			else if($a[$d]['art'] != FXP_HL_FULL)	// No full day
				$a[$d]['typ']=0.0;
			$a[$d]['txt']=$hta[(int)($a[$d]['typ']*10)];
		}
	}
	unset($tmp);
//fxDebug($a,'$a: Holidays in timespan '.$sdate.'-'.$edate, 0);

	// Return without year key?
	if(!$ayear || !sizeof($a))
		return $a;

	// Create year key array
	$k=array();
	foreach($a as $d => $c)
	{
		$y=(int)substr((string)$d,0,4);
		if(!isset($k[$y]))
			$k[$y]=array();
		$k[$y][$d]=$c;
	}
	unset($a);
	return $k;
}

/**
 * get_sondertage
 * Liefert als Ergebnis ein Array zurück mit den einzelnen Daten als Key und
 * der Terminart als Value:
 * 2879 = Abwesend
 * 1178 = Urlaub (Antrag)			1039 = Urlaub
 * 2877 = Sonderurlaub (Antrag)	2878 = Sonderurlaub
 * 2106 = Schulung (Antrag)		1468 = Schulung
 * 2105 = Krank (Antrag)			1040 = Krank
 * 1940 = Gleittag (Antrag)		1941 = Gleittag
 *
 * @param various $sdatum  - Mandatory parameter:                   ???PARAMETER???
 * @param string  $edatum  - Optional parameter (default = ''):     ???PARAMETER???
 * @param integer $pid     - Optional parameter (default = 0):      ???PARAMETER???
 * @param string  $typ     - Optional parameter (default = ''):     ???PARAMETER???
 * @param boolean $tid     - Optional parameter (default = false):  ???PARAMETER???
 * @param boolean $alldays - Optional parameter (default = true):   ???PARAMETER???
 *
 * @return ???RETURN???
 */
function get_sondertage($sdatum, $edatum='', $pid=0, $typ='', $tid=false, $alldays=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$sdatum = substr(trim((string)$sdatum), 0, 8);
	$edatum = substr(trim((string)$edatum), 0, 8);

	$pid = (int)$pid;
	if(!$pid)
		$pid = $GLOBALS['fxpglobals']['person'];

	$tcsv=trim((string)$typ);
	$typ=0;
	if(!strlen((string)$tcsv) || ($tcsv === '0'))
		$tcsv=FXP_AT_ABSENT.",".FXP_AT_SLIDINGDAYREQUEST.",".FXP_AT_SLIDINGDAY.",".FXP_AT_VACATIONREQUEST.",".FXP_AT_VACATION.",".FXP_AT_SPECIALVACATIONREQUEST.",".FXP_AT_SPECIALVACATION.",".FXP_AT_TRAININGREQUEST.",".FXP_AT_TRAINING.",".FXP_AT_SICKCALL.",".FXP_AT_SICK;
	else
	{
		$tarr=explode(',', $tcsv);
		foreach($tarr as $tc => $typ)
			$tarr[$tc]=(int)trim((string)$typ);
		if(sizeof($tarr) == 1)
			$typ=$tarr[0];
	}

	// Wenn kein Start-Datum mitgegeben wurde, dann Tschüß
	$sdl = strlen((string)$sdatum);
	if($sdl == 4)
		$sdatum = $sdatum.'0101';
	else if($sdl < 8)
		return array();

	// Kein End-Datum angegeben, dann End-Datum = Start-Datum
	$edl = strlen((string)$edatum);
	if($edl == 4)
		$edatum = $edatum.'1231';
	else if(!$edl)
	{
		if($sdl == 4)
			$edatum = substr((string)$sdatum, 0, 4).'1231';
		else
			$edatum = $sdatum;
	}
	else if($edl < 8)
		return array();

	// Start- bzw. End-Datum entspr. unserer Syntax umformatieren
	$sdatum = substr((string)$sdatum, 0, 8).'000000';
	$edatum = substr((string)$edatum, 0, 8).'240000';

	$usk_sql  = "SELECT terminart, startzeit, endzeit, soll_st_tag, soll_beginn, termin_id FROM termine";
	$usk_sql .= " WHERE personen_id=".$pid." AND mandanten_id=".$GLOBALS['fxpglobals']['client']." AND (startzeit<='".$edatum."' AND endzeit>='".$sdatum."')";
	$usk_sql .= " AND terminart IN (".$tcsv.")";
	$usk_sql .= " ORDER BY startzeit";
	$usk_array = db_values($usk_sql, '*');
//fxDebug($usk_array, $usk_sql);
	$usk_tage = array();

	// Soll-Zeiten des MAs
	$soll_sql  = "SELECT soll_beginn, pause, wochenarbeitstd, arbeitstage,";
	$soll_sql .= " sonntag AS w0, montag AS w1, dienstag AS w2, mittwoch AS w3, donnerstag AS w4, freitag AS w5, samstag AS w6";
	$soll_sql .= " FROM madaten WHERE personen_id=".$pid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$ma_soll  = db_values($soll_sql);
//fxDebug($ma_soll, $soll_sql);

	$sollstunden_tag = 0.00;
	if(fxIsArray($ma_soll) && ((int)$ma_soll['arbeitstage'] > 0))
		$sollstunden_tag = get_double($ma_soll['wochenarbeitstd']/(int)$ma_soll['arbeitstage']);

	if(fxIsArray($usk_array))
	{
		// Einträge nach Gewichtung sortieren: Abwesend -> Gleittag -> Sonderurlaub -> Urlaub -> Schulung -> Krank
		$usk_sort = array(FXP_AT_ABSENT, FXP_AT_SLIDINGDAYREQUEST, FXP_AT_SLIDINGDAY, FXP_AT_SPECIALVACATIONREQUEST, FXP_AT_SPECIALVACATION, FXP_AT_VACATIONREQUEST, FXP_AT_VACATION, FXP_AT_TRAININGREQUEST, FXP_AT_TRAINING, FXP_AT_SICKCALL, FXP_AT_SICK);
		$usk_index = 0;
		$usk_sort_array = array();
		foreach($usk_sort as $usk_art)
		{
			foreach($usk_array as $usk_entry)
			{
				if($usk_entry['terminart'] == $usk_art)
					$usk_sort_array[$usk_index++] = $usk_entry;
			}
		}
		// Feiertage
		if(fxIsArray($GLOBALS['feiertage']))
			$feiertage = $GLOBALS['feiertage'];
		else
			$feiertage = get_feiertage($sdatum, $edatum);
		// Einträge tagesweise aufsplitten
		if(sizeof($usk_sort_array))
		{
			foreach($usk_sort_array as $usk_entry)
			{
//fxDebug($usk_entry, '$usk_entry');

				$usk_art	= (int)$usk_entry['terminart'];
				$usk_ende	= (int)substr((string)$usk_entry['endzeit'], 0,8);
				$usk_jahr	= (int)substr((string)$usk_entry['startzeit'], 0,4);
				$usk_monat	= (int)substr((string)$usk_entry['startzeit'], 4,2);
				$usk_tag	= (int)substr((string)$usk_entry['startzeit'], 6,2);

				$start_sek	= timeToSec($usk_entry['startzeit']);
				$ende_sek	= timeToSec($usk_entry['endzeit']);

				$mo_tage=fxf_mdays($usk_jahr);

				// Startwochentag ermitteln
				$wtg = fx_date('w', fx_mktime(12, 0, 0, $usk_monat, $usk_tag, $usk_jahr));

				while(true)
				{
					$usk_datum = (int)sprintf("%04d%02d%02d", $usk_jahr, $usk_monat, $usk_tag);
					$usk_dvgl = $usk_datum.'000000';
					if($usk_datum > $usk_ende)
						break;

					$workday = 1.0;
					if(!$ma_soll['w'.$wtg] || (fxIsArray($feiertage[$usk_datum]) && ($feiertage[$usk_datum]['art'] == FXP_HL_FULL)))
						$workday = 0.0;
					else if(fxIsArray($feiertage[$usk_datum]) && ($feiertage[$usk_datum]['art'] == FXP_HL_HALF))
						$workday = 0.5;

					if(($alldays || ($workday > 0.00)) && ($usk_dvgl >= $sdatum) && ($usk_dvgl <= $edatum))
					{
						$dauer_p = 0.00;
						$std_tag = $sollstunden_tag;
						if($usk_entry['soll_st_tag'] > 0.00)
							$std_tag = $usk_entry['soll_st_tag'];

						if($usk_datum == substr((string)$usk_entry['startzeit'], 0, 8))
							$startzeit = $start_sek;
						else
							$startzeit = 0;
						if($usk_datum == substr((string)$usk_entry['endzeit'], 0, 8))
							$endzeit = $ende_sek;
						else
							$endzeit = 86400;
						$dauer_h = MIN($std_tag, ($endzeit-$startzeit)/3600.00);

						if($workday > 0.00)
						{
							if($dauer_h < $std_tag)
								$dauer_p = 0.50;
							else
								$dauer_p = 1.00;
							if($workday == 0.50)
								$dauer_p -= $workday;
						}

						if(!fxIsArray($usk_tage[$usk_datum]))
							$usk_tage[$usk_datum] = array(-1=>array('typ'=>0, 'dauer'=>0.00, 'stunden_termin'=>0.00, 'stunden_frei'=>$std_tag, 'start_sek'=>$startzeit, 'end_sek'=>$endzeit));

						if(!fxIsArray($usk_tage[$usk_datum][$usk_art]))
							$usk_tage[$usk_datum][$usk_art] = array(-1=>array('dauer'=>0.00, 'stunden_termin'=>0.00, 'start_sek'=>$startzeit, 'end_sek'=>$endzeit));

						$usk_tage[$usk_datum][$usk_art][$startzeit.'-'.$endzeit]	= array('dauer'=>$dauer_p, 'stunden_termin'=>$dauer_h, 'termin_id'=>$usk_entry['termin_id']);

						$usk_tage[$usk_datum][$usk_art][-1]['dauer']				= MIN(1.00, $usk_tage[$usk_datum][$usk_art][-1]['dauer'] + $dauer_p);
						$usk_tage[$usk_datum][$usk_art][-1]['stunden_termin']		= $usk_tage[$usk_datum][$usk_art][-1]['stunden_termin'] + $dauer_h;
						$usk_tage[$usk_datum][$usk_art][-1]['start_sek']			= MIN($usk_tage[$usk_datum][$usk_art][-1]['start_sek'], $startzeit);
						$usk_tage[$usk_datum][$usk_art][-1]['end_sek']				= MAX($usk_tage[$usk_datum][$usk_art][-1]['end_sek'], $endzeit);

						if(!$usk_tage[$usk_datum][-1]['typ'])
							$usk_tage[$usk_datum][-1]['typ']						= $usk_art;

						$usk_tage[$usk_datum][-1]['dauer']							= MIN(1.00, $usk_tage[$usk_datum][-1]['dauer'] + $dauer_p);
						$usk_tage[$usk_datum][-1]['stunden_termin']					= $usk_tage[$usk_datum][-1]['stunden_termin'] + $dauer_h;
						$usk_tage[$usk_datum][-1]['stunden_frei']					= MAX(0.00, $usk_tage[$usk_datum][-1]['stunden_frei'] - $dauer_h);
						$usk_tage[$usk_datum][-1]['start_sek']						= MIN($usk_tage[$usk_datum][-1]['start_sek'], $startzeit);
						$usk_tage[$usk_datum][-1]['end_sek']						= MAX($usk_tage[$usk_datum][-1]['end_sek'], $endzeit);
//fxDebug($usk_tage, '$usk_tage: '.$usk_datum);
					}

					$wtg++;
					if($wtg > 6)
						$wtg = 0;

					$usk_tag++;
					if($usk_tag > $mo_tage[$usk_monat])
					{
						$usk_tag = 1;
						$usk_monat++;
						if($usk_monat > 12)
						{
							$usk_monat = 1;
							$usk_jahr++;
							$mo_tage=fxf_mdays($usk_jahr);
						}
					}
				}
			}
			if(sizeof($usk_tage))
				ksort($usk_tage);
		}
	}

//fxDebug($usk_tage, '$usk_tage');
	return $usk_tage;
}

/**
 * get_termine
 * Liefert als Ergebnis ein Array zurück mit den einzelnen Daten als Key und
 * der Terminart als Value:
 *
 * @param various $sdatum  - Mandatory parameter:                ???PARAMETER???
 * @param string  $edatum  - Optional parameter (default = ''):  ???PARAMETER???
 * @param integer $pid     - Optional parameter (default = 0):   ???PARAMETER???
 * @param string  $tart    - Optional parameter (default = ''):  ???PARAMETER???
 * @param integer $proj_id - Optional parameter (default = 0):   ???PARAMETER???
 *
 * @return ???RETURN???
 */
function get_termine($sdatum, $edatum='', $pid=0, $tart='', $proj_id=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$sdatum = substr(trim((string)$sdatum), 0, 8);
	$edatum = substr(trim((string)$edatum), 0, 8);

	$pid = (int)$pid;
	if(!$pid)
		$pid = $GLOBALS['fxpglobals']['person'];

	$tart = trim((string)$tart);
	if(!strlen((string)$tart))
		$tart = FXP_AT_INVITATION.','.FXP_AT_MEETING.','.FXP_AT_CONFERENCE.','.FXP_AT_BUSINESSDINNER.','.FXP_AT_CONTACT.','.FXP_AT_PRIVATE.','.FXP_AT_VARIOUS;

	// Wenn kein Start-Datum mitgegeben wurde, dann Tschüß
	$sdl = strlen((string)$sdatum);
	if($sdl == 4)
		$sdatum = $sdatum.'0101';
	else if($sdl != 8)
		return '';

	// Kein End-Datum angegeben, dann End-Datum = Start-Datum
	$edl = strlen((string)$edatum);
	if($edl == 4)
		$edatum = $edatum.'1231';
	else if(!$edl)
	{
		if($sdl == 4)
			$edatum = substr((string)$sdatum, 0, 4).'1231';
		else
			$edatum = $sdatum;
	}

	// Start- bzw. End-Datum entspr. unserer Syntax umformatieren
	$sdatum = substr((string)$sdatum, 0, 8).'000000';
	$edatum = substr((string)$edatum, 0, 8).'240000';

	$tm_sql  = "SELECT terminart, startzeit, endzeit, termin_id";
	$tm_sql .= " FROM termine";
	$tm_sql .= " WHERE personen_id=".$pid." AND mandanten_id=".$GLOBALS['fxpglobals']['client']." AND (startzeit<='".$edatum."' AND endzeit>='".$sdatum."')";
	$tm_sql .= " AND terminart IN (".$tart.") AND (bestaetigung_anmerk IS NULL OR bestaetigung_anmerk NOT LIKE '0-%')";
	if($proj_id)
		$tm_sql .= " AND (projekt_id IS NULL OR projekt_id=0 OR projekt_id<>".$proj_id.")";
	$tm_sql .= " ORDER BY startzeit";
	$tm_array = db_values($tm_sql, '*');
//fxDebug($tm_array, $tm_sql);
	if(!fxIsArray($tm_array))
		return false;

	$tm_tage = array();
	if($proj_id)
	{
		// Soll-Zeiten des MAs
		$sollstunden_tag	= 0.00;
		$soll_beginn		= '120000';
		$soll_ende			= '120000';

		$soll_sql = "SELECT soll_beginn, pause, wochenarbeitstd, arbeitstage FROM madaten WHERE personen_id=".$pid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$ma_soll  = db_values($soll_sql);
		if(fxIsArray($ma_soll) && ((int)$ma_soll['arbeitstage'] > 0))
		{
			$sollstunden_tag	= get_double($ma_soll['wochenarbeitstd']/(int)$ma_soll['arbeitstage']);
			$soll_beginn		= substr((string)$ma_soll['soll_beginn'], 8);
			$soll_ende			= substr((string)time_add('20000101'.$soll_beginn, $sollstunden_tag + ($ma_soll['pause'] / 3600) , "H"), 8);
		}
//echo('$sollstunden_tag='.$sollstunden_tag.', $soll_beginn='.$soll_beginn.', $soll_ende='.$soll_ende.'<HR>');
	}

	// Einträge tagesweise aufsplitten
	foreach($tm_array as $tm_entry)
	{
		$sdatum		= substr((string)$tm_entry['startzeit'], 0, 8);
		$edatum		= substr((string)$tm_entry['endzeit'], 0, 8);
		$datum		= $sdatum;
		$startzeit	= substr((string)$tm_entry['startzeit'], 8);
		$endzeit	= substr((string)$tm_entry['endzeit'], 8);
		while($datum <= $edatum)
		{
			if($datum == $sdatum)
				$szeit = $datum.$startzeit;
			else
				$szeit = $datum.'000000';

			if($datum == $edatum)
				$ezeit = $datum.$endzeit;
			else
				$ezeit = $datum.'240000';

			$tm_tage[$datum][$szeit] = array
			(
				'id'				=> (int)$tm_entry['termin_id'],
				'art'				=> (int)$tm_entry['terminart'],
				'start'				=> $szeit,
				'ende'				=> $ezeit
			);

			if($proj_id)
			{
				$mstart	= min(max($datum.$soll_beginn, $szeit), $ezeit);
				$mende	= max(min($datum.$soll_ende, $ezeit), $szeit);
				$diff	= time_diff($mstart, $mende, 'H');
//echo('<B>DIFF:</B> $mstart='.$mstart.' - $mende='.$mende.' = '.$diff.' h<HR>');

				$tm_tage[$datum][$szeit]['stunden_termin'] = $diff;
			}

			$datum = fx_date('Ymd', fx_mktime(12, 0, 0, (int)substr((string)$datum, 4, 2), (int)substr((string)$datum, 6, 2)+1, (int)substr((string)$datum, 0, 4)));
		}
	}

	if(sizeof($tm_tage))
		ksort($tm_tage);

//fxDebug($tm_tage, '$tm_tage');
	return $tm_tage;
}

/**
 * ???FUNCTION???
 *
 * @param various $sdate - Mandatory parameter:                ???PARAMETER???
 * @param string  $edate - Optional parameter (default = ''):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function dwmyArray($sdate, $edate='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Start date
	$sdate=trim((string)$sdate);
	if(!strlen((string)$sdate))
		return true;
	$sdate=substr((string)$sdate,0,8);

	// End date
	$edate=trim((string)$edate);
	if(!strlen((string)$edate))
 		$edate=$sdate;
 	else
	 	$edate=substr((string)$edate,0,8);

	// Switch dates?
	if($edate < $sdate)
	{
		$bdate=$sdate;
		$sdate=$edate;
		$edate=$bdate;
	}

	// Adjust to date limits
	setDateLimits();
	$y=(int)substr((string)$sdate,0,4);
	if($y < $GLOBALS['fxpvars']['date_minmax']['min'])
		$sdate=substr('0000'.$GLOBALS['fxpvars']['date_minmax']['min'], -4).'0101';
	else if($y > $GLOBALS['fxpvars']['date_minmax']['max'])
		$sdate=substr('0000'.$GLOBALS['fxpvars']['date_minmax']['max'], -4).'1231';
	$y=(int)substr((string)$edate,0,4);
	if($y < $GLOBALS['fxpvars']['date_minmax']['min'])
		$edate=substr('0000'.$GLOBALS['fxpvars']['date_minmax']['min'], -4).'0101';
	else if($y > $GLOBALS['fxpvars']['date_minmax']['max'])
		$edate=substr('0000'.$GLOBALS['fxpvars']['date_minmax']['max'], -4).'1231';
//echo('$sdate=['.$sdate.'], $edate=['.$edate.']<br />');

	$dwmy=array('days'=>array(), 'weeks'=>array(), 'months'=>array(), 'years'=>array());
	$adate=$sdate;
	if($adate <= $edate)
	{
		$y=(int)substr((string)$adate,0,4);
		$m=(int)substr((string)$adate,4,2);
		$d=(int)substr((string)$adate,6,2);
		$aday=getWeekday($adate);
		$cw=kw($adate);
		$fwday=(int)($GLOBALS['fxpglobals']['settings']['ersterwt']);
		$month_days=fxf_mdays($y);

		while($adate <= $edate)
		{
//echo($adate.': '.$aday.'<br />');
			// ...Days
			$dwmy['days'][$adate]=$aday;
			// ...Weeks
			$acw=substr('0'.$cw,-2).$y;
			if(!isset($dwmy['weeks'][$acw]))
				$dwmy['weeks'][$acw]=array('sday'=>$adate, 'eday'=>$adate, 'days'=>1);
			else
			{
				$dwmy['weeks'][$acw]['eday']=$adate;
				$dwmy['weeks'][$acw]['days']++;
			}
			// ...Months
			$am=substr('0'.$m,-2).$y;
			if(!isset($dwmy['months'][$am]))
				$dwmy['months'][$am]=array('sday'=>$adate, 'eday'=>$adate, 'days'=>1);
			else
			{
				$dwmy['months'][$am]['eday']=$adate;
				$dwmy['months'][$am]['days']++;
			}
			// ...Years
			if(!isset($dwmy['years'][$y]))
				$dwmy['years'][$y]=array('sday'=>$adate, 'eday'=>$adate, 'days'=>1);
			else
			{
				$dwmy['years'][$y]['eday']=$adate;
				$dwmy['years'][$y]['days']++;
			}

			// Next day
			$aday++;
			if($aday > 6)
				$aday=0;
			$d++;
			if($d > $month_days[$m])
			{
				$d=1;
				$m++;
				if($m > 12)
				{
					$m=1;
					$y++;
					$month_days=fxf_mdays($y);
				}
			}

			$adate=substr('0000'.$y, -4).substr('00'.$m, -2).substr('00'.$d, -2);

			if($aday == $fwday)
			{
				$cw++;
				if($cw > 50)
					$cw=kw($adate);
			}
		}
	}
//fxDebug($dwmy,'$dwmy');

	return $dwmy;
}

/**
 * ???FUNCTION???
 *
 * @param various $sdate - Mandatory parameter:                ???PARAMETER???
 * @param string  $edate - Optional parameter (default = ''):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function weekdays($sdate, $edate='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$dwmy=dwmyArray($sdate,$edate);

	return $dwmy['days'];
}

/**
 * ???FUNCTION???
 *
 * @param boolean $with_names - Optional parameter (default = false): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function getAppIcons($with_names=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$app_icons=array
	(
		FXP_AT_REMINDER				=> 'c_rem',
		FXP_AT_INVITATION				=> 'c_inv',
		FXP_AT_MEETING				=> 'c_meet',
		FXP_AT_CONFERENCE				=> 'c_conf',
		FXP_AT_BUSINESSDINNER		=> 'c_eat',
		FXP_AT_CONTACT				=> 'c_cont',
		FXP_AT_MESSAGE				=> 'c_msg',
		FXP_AT_PROJECTMESSAGE		=> 'c_pmsg',
		FXP_AT_PRIVATE			=> 'c_priv',
		FXP_AT_TODO					=> 'c_todo',

		FXP_AT_ABSENT				=> 'c_abs',
		FXP_AT_VACATION					=> 'c_hday',
		FXP_AT_VACATIONREQUEST			=> 'c_hday',
		FXP_AT_SPECIALVACATION			=> 'c_shday',
		FXP_AT_SPECIALVACATIONREQUEST	=> 'c_shday',
		FXP_AT_SICK					=> 'c_sick',
		FXP_AT_SICKCALL			=> 'c_sick',
		FXP_AT_TRAINING				=> 'c_trn',
		FXP_AT_TRAININGREQUEST		=> 'c_trn',
		FXP_AT_SLIDINGDAY				=> 'c_flex',
		FXP_AT_SLIDINGDAYREQUEST			=> 'c_flex',
		FXP_AT_OVERTIMEOUTPAY			=> 'c_otpay',
		FXP_AT_OVERTIMEOUTPAYREQUEST	=> 'c_otpay',

		FXP_AT_VARIOUS			=> 'c_empty'
	);

	if($with_names)
	{
		$loop_icons=$app_icons;
		foreach($loop_icons as $app_id => $app_gif)
			$app_icons['name_'.$app_id]=meldung($app_id, true);
	}

	return $app_icons;
}

/**
 * ???FUNCTION???
 *
 * @param boolean $with_messages - Optional parameter (default = true): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function get_uskg_array($with_messages=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$ra = array(
		FXP_AT_ABSENT=>2880,
		FXP_AT_VACATIONREQUEST=>1245,			FXP_AT_VACATION=>767,
		FXP_AT_SPECIALVACATIONREQUEST=>2881,		FXP_AT_SPECIALVACATION=>2882,
		FXP_AT_TRAININGREQUEST=>2005,			FXP_AT_TRAINING=>2107,
		FXP_AT_SICKCALL=>768,				FXP_AT_SICK=>2108,
		FXP_AT_SLIDINGDAYREQUEST=>2385,			FXP_AT_SLIDINGDAY=>2407,
		'frei'=>1502,
		'arbeit'=>1503
	);

	if($with_messages)
	{
		foreach($ra as $id => $msg_id)
			$ra[$id] = meldung($msg_id);
	}

	return $ra;
}

/**
 * ???FUNCTION???
 *
 * @param various $sdate  - Mandatory parameter:                  ???PARAMETER???
 * @param various $edate  - Mandatory parameter:                  ???PARAMETER???
 * @param string  $force  - Optional parameter (default = 'A'):   ???PARAMETER???
 * @param boolean $dsplit - Optional parameter (default = true):  ???PARAMETER???
 */
function dateSplit($sdate, $edate, $force='A', $dsplit=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$sdate = substr((string)$sdate, 0, 8);
	$edate = substr((string)$edate, 0, 8);

	$tdate = substr((string)fxNow(), 0, 8);
	if(($GLOBALS['fxpglobals']['tr'] == 92) || !$dsplit)
		$tdate = $sdate;

	$force = strtoupper(substr(trim((string)$force), 0, 1));
	if(!strlen((string)$force) || ($force == 'A'))
		$force = -666;
	else
	{
		$farr = array('D'=>0, 'W'=>1, 'M'=>2, 'Q'=>3, 'Y'=>4);
		if(isset($farr[$force]))
			$force = $farr[$force];
		else
			$force = -666;
	}
//echo('sdate: <b>'.$sdate.'</b>, edate: <b>'.$edate.'</b>, today: <b>'.$tdate.'</b><br>force: <b>'.$force.'</b><hr>');

	$cyears_l		= 0;
	$cquarts_l		= 0;
	$cmonths_l		= 0;
	$cweeks_l		= 0;
	$cdays_l		= 0;

	$cdays_r		= 0;
	$cweeks_r		= 0;
	$cmonths_r		= 0;
	$cquarts_r		= 0;
	$cyears_r		= 0;

	$days = 1+fxp_date_diff($sdate, $edate);

	// Bereich
	$sdiff = fxp_date_diff($tdate, $sdate);
	$ediff = fxp_date_diff($tdate, $edate);
//echo('Difference in days: <b>'.$days.'</b><br>sdiff=<b>'.$sdiff.'</b>, ediff=<b>'.$ediff.'</b><hr>');
	if($sdiff < 0)
	{
		if($ediff < 0)
		{
			$sdiff = -$days+1;
			$ediff = 0;
		}
	}
	else
	{
		$sdiff = 0;
		$ediff = $days-1;
	}
//echo('Difference in days: <b>'.$days.'</b><br>sdiff=<b>'.$sdiff.'</b>, ediff=<b>'.$ediff.'</b><hr>');

	if(isset($GLOBALS['fxpglobals']['settings']['cal_maxdays_l']))
	{
		$maxdays_l		= (int)$GLOBALS['fxpglobals']['settings']['cal_maxdays_l'];	// Default: 56 (max. 8 Wochen (= 56 Tage) taggenau anzeigen)
		$maxweeks_l		= (int)$GLOBALS['fxpglobals']['settings']['cal_maxweeks_l'];	// Default: 24 (max. 6 Monate (= 24 Wochen = 168 Tage) wochenweise anzeigen)
		$maxmonths_l	= (int)$GLOBALS['fxpglobals']['settings']['cal_maxmonths_l'];	// Default: 24 (max. 2 Jahre (= 24 Monate = 720 Tage) monatsweise anzeigen)
		$maxquarts_l	= (int)$GLOBALS['fxpglobals']['settings']['cal_maxquarts_l'];	// Default: 16 (max. 4 Jahre (= 16 Quartale = 1920 Tage) quartalsweise anzeigen)

		$maxdays_r		= (int)$GLOBALS['fxpglobals']['settings']['cal_maxdays_r'];	// Default: 56 (max. 8 Wochen (= 56 Tage) taggenau anzeigen)
		$maxweeks_r		= (int)$GLOBALS['fxpglobals']['settings']['cal_maxweeks_r'];	// Default: 24 (max. 6 Monate (= 24 Wochen = 168 Tage) wochenweise anzeigen)
		$maxmonths_r	= (int)$GLOBALS['fxpglobals']['settings']['cal_maxmonths_r'];	// Default: 24 (max. 2 Jahre (= 24 Monate = 720 Tage) monatsweise anzeigen)
		$maxquarts_r	= (int)$GLOBALS['fxpglobals']['settings']['cal_maxquarts_r'];	// Default: 16 (max. 4 Jahre (= 16 Quartale = 1920 Tage) quartalsweise anzeigen)
	}
	else
	{
		$maxdays_l		= 56;
		$maxweeks_l		= 24;
		$maxmonths_l	= 24;
		$maxquarts_l	= 16;

		$maxdays_r		= 56;
		$maxweeks_r		= 24;
		$maxmonths_r	= 24;
		$maxquarts_r	= 16;
	}

	$maxdayweeks_l	= $maxweeks_l*7;
	$maxdaymonths_l	= $maxmonths_l*30;
	$maxdayquarts_l	= $maxquarts_l*120;

	$maxdayweeks_r	= $maxweeks_r*7;
	$maxdaymonths_r	= $maxmonths_r*30;
	$maxdayquarts_r	= $maxquarts_r*120;

	if($ediff > $maxdays_r)
		$maxdays_l = round($maxdays_l/2);

//echo('PAST (left): $maxdays_l: <b>'.$maxdays_l.'</b>, $maxweeks_l: <b>'.$maxweeks_l.'</b> (= '.$maxdayweeks_l.' days), $maxmonths_l: <b>'.$maxmonths_l.'</b> (= '.$maxdaymonths_l.' days), $maxquarts_l: <b>'.$maxquarts_l.'</b> (= '.$maxdayquarts_l.' days)<br>');
//echo('FUTURE (right): $maxdays_r: <b>'.$maxdays_r.'</b>, $maxweeks_r: <b>'.$maxweeks_r.'</b> (= '.$maxdayweeks_r.' days), $maxmonths_r: <b>'.$maxmonths_r.'</b> (= '.$maxdaymonths_r.' days), $maxquarts_r: <b>'.$maxquarts_r.'</b> (= '.$maxdayquarts_r.' days)<hr>');

	$GLOBALS['darr'] = array();
	$GLOBALS['oarr'] = array();

	// Linker Bereich [-start bis -1]
	$dtyp = 0;	// 0=taggenau, 1=wochengenau, 2=monatsgenau
	if($force != -666)
		$dtyp = $force;
	$dcnt = 0;
	if($sdiff < 0)
	{
//echo('<b>LEFT</b>...<br>');
		$farr	= array();
		$larr	= array();
		$adiff	= 0;
		if($ediff > 0)
			$adiff	= -1;
		$adate	= fx_date('Ymd', fx_mktime(12, 0, 0, (int)substr((string)$sdate, 4, 2), (int)substr((string)$sdate, 6, 2)-$sdiff+$adiff, (int)substr((string)$sdate, 0, 4)));
//echo('$adate='.$adate.'<hr>');
		$ayear	= (int)substr((string)$adate, 0, 4);
		$amonth	= (int)substr((string)$adate, 4, 2);
		$aday	= (int)substr((string)$adate, 6, 2);
		$wday	= fx_date('w', fx_mktime(12, 0, 0, $amonth, $aday, $ayear));
		$aweek	= kw($adate);
		$aquart	= 1+(int)(($amonth-1)/3);

		$dmonth=fxf_mdays($ayear);

		while($adiff >= $sdiff)
		{
			$adate	= substr('0000'.$ayear, -4).substr('00'.$amonth, -2).substr('00'.$aday, -2);
			$rdays	= $adiff-$sdiff;
			$diff	= abs($adiff);
//echo($adiff.' ('.$diff.'): ['.$wday.'] '.$adate.' ('.$dtyp.'_'.$rdays.')<br>');

			$farr[$adate] = $dcnt;

			switch($dtyp)
			{
				case 0:		// Tagesgenau
					$cdays_l++;
					$larr[$dcnt] = array('type'=>"day", 'day'=>$aday, 'week'=>$aweek, 'month'=>$amonth, 'quart'=>$aquart, 'year'=>$ayear, 'wday'=>$wday, 'date'=>$adate);

					// Prüfen, ob der Genauigkeitsmodus geändert werden soll
					if(($force == -666) && ($diff >= $maxdays_l) && ($rdays > 11) && ($wday == 1))
						$dtyp++;
					$dcnt++;
				break;

				case 1:		// Wochengenau
					if(!fxIsArray($larr[$dcnt]))
					{
						$cweeks_l++;
						$larr[$dcnt] = array('type'=>"week", 'week'=>$aweek, 'month'=>$amonth, 'quart'=>$aquart, 'year'=>$ayear, 'days'=>1, 'mindate'=>$adate, 'mindate_wday'=>$wday, 'maxdate'=>$adate, 'maxdate_wday'=>$wday);
					}
					else
					{
						$larr[$dcnt]['days']++;
						$larr[$dcnt]['mindate'] = $adate;
						$larr[$dcnt]['mindate_wday'] = $wday;
					}

					// Prüfen, ob der Genauigkeitsmodus geändert werden soll (nur am Quartalsanfang)
					if(($force == -666) && ($diff >= $maxdayweeks_l) && ($rdays > 45) && ($wday == 1) && ($amonth%4 == 0) && ($aday > $dmonth[$amonth]-7))
					{
						$dtyp++;
						$dcnt++;
					}
					else if($wday == 1)
						$dcnt++;
				break;

				case 2:		// Monatsgenau
					if(!fxIsArray($larr[$dcnt]))
					{
						$cmonths_l++;
						$larr[$dcnt] = array('type'=>"month", 'month'=>$amonth, 'quart'=>$aquart, 'year'=>$ayear, 'days'=>1, 'mindate'=>$adate, 'mindate_wday'=>$wday, 'maxdate'=>$adate, 'maxdate_wday'=>$wday);
					}
					else
					{
						$larr[$dcnt]['days']++;
						$larr[$dcnt]['mindate'] = $adate;
						$larr[$dcnt]['mindate_wday'] = $wday;
					}

					// Prüfen, ob der Genauigkeitsmodus geändert werden soll (nur am Jahresbeginn)
					if(($force == -666) && ($diff >= $maxdaymonths_l) && ($rdays > 180) && ($amonth == 1) && ($aday == 1))
					{
						$dtyp++;
						$dcnt++;
					}
					else if($aday == 1)
						$dcnt++;
				break;

				case 3:		// Quartalsgenau
					if(!fxIsArray($larr[$dcnt]))
					{
						$cquarts_l++;
						$larr[$dcnt] = array('type'=>"quart", 'quart'=>$aquart, 'year'=>$ayear, 'days'=>1, 'mindate'=>$adate, 'mindate_wday'=>$wday, 'maxdate'=>$adate, 'maxdate_wday'=>$wday);
					}
					else
					{
						$larr[$dcnt]['days']++;
						$larr[$dcnt]['mindate'] = $adate;
						$larr[$dcnt]['mindate_wday'] = $wday;
					}

					// Prüfen, ob der Genauigkeitsmodus geändert werden soll (nur am Jahresende)
					if(($force == -666) && ($diff >= $maxdayquarts_l) && ($rdays > 540) && ($amonth == 1) && ($aday == 1))
					{
						$dtyp++;
						$dcnt++;
					}
					else if(($aday == 1) && ($amonth%4 == 0))
						$dcnt++;
				break;

				default:	// Jahresgenau
					if(!fxIsArray($larr[$dcnt]))
					{
						$cyears_l++;
						$larr[$dcnt] = array('type'=>"year", 'year'=>$ayear, 'days'=>1, 'mindate'=>$adate, 'mindate_wday'=>$wday, 'maxdate'=>$adate, 'maxdate_wday'=>$wday);
					}
					else
					{
						$larr[$dcnt]['days']++;
						$larr[$dcnt]['mindate'] = $adate;
						$larr[$dcnt]['mindate_wday'] = $wday;
					}
					if(($amonth == 1) && ($aday == 1))
						$dcnt++;
				break;
			}

			// Vorheriger Wochentag
			$wday--;
			if($wday < 0)
				$wday = 6;

			// Vorheriger Tag
			$adiff--;
			$aday--;
			if(!$aday)
			{
				$amonth--;
				if(!$amonth)
				{
					$amonth = 12;
					$ayear--;
					$dmonth=fxf_mdays($ayear);
				}
				$aday = $dmonth[$amonth];
				$aquart	= 1+(int)(($amonth-1)/3);
			}

			// Vorherige Woche
			if(!$wday)
			{
				$aweek--;
				if(!$aweek)
					$aweek = kw(substr('0000'.$ayear, -4).substr('00'.$amonth, -2).substr('00'.$aday, -2));
			}
		}
		if(sizeof($larr))
		{
			$dcnt = sizeof($larr);
			$rcnt = 0;
			for($lcnt=$dcnt-1; $lcnt>=0; --$lcnt)
				$GLOBALS['darr'][$rcnt++] = $larr[$lcnt];
			unset($larr);

			ksort($farr);
			foreach($farr as $d => $dc)
				$GLOBALS['oarr'][$d] = $dcnt-$dc-1;
			unset($farr);
		}
	}

	// Rechter Bereich
	if(($ediff > 0) || (!$ediff && !$sdiff))
	{
//echo('<b>RIGHT</b>...<br>');
		$dtyp = 0;
		if($force != -666)
			$dtyp = $force;

		$adiff	= 0;
		$adate	= fx_date('Ymd', fx_mktime(12, 0, 0, (int)substr((string)$sdate, 4, 2), (int)substr((string)$sdate, 6, 2)-$sdiff, (int)substr((string)$sdate, 0, 4)));
//echo('$adate='.$adate.'<hr>');
		$ayear	= (int)substr((string)$adate, 0, 4);
		$amonth	= (int)substr((string)$adate, 4, 2);
		$aday	= (int)substr((string)$adate, 6, 2);
		$wday	= fx_date('w', fx_mktime(12, 0, 0, $amonth, $aday, $ayear));
		$aweek	= kw($adate);
		$aquart	= 1+(int)(($amonth-1)/3);

		$dmonth=fxf_mdays($ayear);

		while($adiff <= $ediff)
		{
			$adate	= substr('0000'.$ayear, -4).substr('00'.$amonth, -2).substr('00'.$aday, -2);
			$rdays	= $ediff-$adiff;
			$diff	= abs($adiff);
//echo($adiff.' ('.$diff.'): ['.$wday.'] '.$adate.' ('.$dtyp.'_'.$rdays.')<br>');

			$GLOBALS['oarr'][$adate] = $dcnt;

			switch($dtyp)
			{
				case 0:		// Tagesgenau
					$cdays_r++;
					$GLOBALS['darr'][$dcnt] = array('type'=>"day", 'day'=>$aday, 'week'=>$aweek, 'month'=>$amonth, 'quart'=>$aquart, 'year'=>$ayear, 'wday'=>$wday, 'date'=>$adate);

					// Prüfen, ob der Genauigkeitsmodus geändert werden soll
					if(($force == -666) && ($diff >= $maxdays_r) && ($rdays > 11) && !$wday)
						$dtyp++;
					$dcnt++;
				break;

				case 1:		// Wochengenau
					if(!fxIsArray($GLOBALS['darr'][$dcnt]))
					{
						$cweeks_r++;
						$GLOBALS['darr'][$dcnt] = array('type'=>"week", 'week'=>$aweek, 'month'=>$amonth, 'quart'=>$aquart, 'year'=>$ayear, 'days'=>1, 'mindate'=>$adate, 'mindate_wday'=>$wday, 'maxdate'=>$adate, 'maxdate_wday'=>$wday);
					}
					else
					{
						$GLOBALS['darr'][$dcnt]['days']++;
						$GLOBALS['darr'][$dcnt]['maxdate'] = $adate;
						$GLOBALS['darr'][$dcnt]['maxdate_wday'] = $wday;
					}

					// Prüfen, ob der Genauigkeitsmodus geändert werden soll (nur am Quartalsbeginn)
					if(($force == -666) && ($diff >= $maxdayweeks_r) && ($rdays > 45) && !$wday && ($amonth%4 == 0) && ($aday < 7))
					{
						$dtyp++;
						$dcnt++;
					}
					else if(!$wday)
						$dcnt++;
				break;

				case 2:		// Monatsgenau
					if(!fxIsArray($GLOBALS['darr'][$dcnt]))
					{
						$cmonths_r++;
						$GLOBALS['darr'][$dcnt] = array('type'=>"month", 'month'=>$amonth, 'quart'=>$aquart, 'year'=>$ayear, 'days'=>1, 'mindate'=>$adate, 'mindate_wday'=>$wday, 'maxdate'=>$adate, 'maxdate_wday'=>$wday);
					}
					else
					{
						$GLOBALS['darr'][$dcnt]['days']++;
						$GLOBALS['darr'][$dcnt]['maxdate'] = $adate;
						$GLOBALS['darr'][$dcnt]['maxdate_wday'] = $wday;
					}

					// Prüfen, ob der Genauigkeitsmodus geändert werden soll (nur am Jahresende)
					if(($force == -666) && ($diff >= $maxdaymonths_r) && ($rdays > 180) && ($aday == $dmonth[$amonth]) && ($amonth == 12) && ($aday == 31))
					{
						$dtyp++;
						$dcnt++;
					}
					else if($aday == $dmonth[$amonth])
						$dcnt++;
				break;

				case 3:		// Quartalsgenau
					if(!fxIsArray($GLOBALS['darr'][$dcnt]))
					{
						$cquarts_r++;
						$GLOBALS['darr'][$dcnt] = array('type'=>"quart", 'quart'=>$aquart, 'year'=>$ayear, 'days'=>1, 'mindate'=>$adate, 'mindate_wday'=>$wday, 'maxdate'=>$adate, 'maxdate_wday'=>$wday);
					}
					else
					{
						$GLOBALS['darr'][$dcnt]['days']++;
						$GLOBALS['darr'][$dcnt]['maxdate'] = $adate;
						$GLOBALS['darr'][$dcnt]['maxdate_wday'] = $wday;
					}

					// Prüfen, ob der Genauigkeitsmodus geändert werden soll (nur am Jahresende)
					if(($force == -666) && ($diff >= $maxdayquarts_r) && ($rdays > 540) && ($amonth == 12) && ($aday == 31))
					{
						$dtyp++;
						$dcnt++;
					}
					else if(($aday == $dmonth[$amonth]) && ($amonth%3 == 0))
						$dcnt++;
				break;

				default:	// Jahres
					if(!fxIsArray($GLOBALS['darr'][$dcnt]))
					{
						$cyears_r++;
						$GLOBALS['darr'][$dcnt] = array('type'=>"year", 'year'=>$ayear, 'days'=>1, 'mindate'=>$adate, 'mindate_wday'=>$wday, 'maxdate'=>$adate, 'maxdate_wday'=>$wday);
					}
					else
					{
						$GLOBALS['darr'][$dcnt]['days']++;
						$GLOBALS['darr'][$dcnt]['maxdate'] = $adate;
						$GLOBALS['darr'][$dcnt]['maxdate_wday'] = $wday;
					}
					if(($amonth == 12) && ($aday == 31))
						$dcnt++;
				break;
			}

			// Nächster Wochentag
			$wday++;
			if($wday > 6)
				$wday = 0;

			// Nächster Tag
			$adiff++;
			$aday++;
			if($aday > $dmonth[$amonth])
			{
				$aday = 1;
				$amonth++;
				if($amonth > 12)
				{
					$amonth = 1;
					$ayear++;
					$dmonth=fxf_mdays($ayear);
				}
				$aquart	= 1+(int)(($amonth-1)/3);
			}

			// Nächste Woche
			if($wday == 1)
			{
				$aweek++;
				if($aweek > 50)
					$aweek = kw(substr('0000'.$ayear, -4).substr('00'.$amonth, -2).substr('00'.$aday, -2));
			}
		}
	}
//echo('<b>columns='.($cyears_l+$cquarts_l+$cmonths_l+$cweeks_l+$cdays_l+$cdays_r+$cweeks_r+$cmonths_r+$cquarts_r+$cyears_r).'</b>: $cyears_l='.$cyears_l.', $cquarts_l='.$cquarts_l.', $cmonths_l='.$cmonths_l.', $cweeks_l='.$cweeks_l.', $cdays_l='.$cdays_l.', $cdays_r='.$cdays_r.', $cweeks_r='.$cweeks_r.', $cmonths_r='.$cmonths_r.', $cquarts_r='.$cquarts_r.', $cyears_r='.$cyears_r.'<hr>');

	$GLOBALS['darr'][0]['info'] = array('mindate'=>$sdate, 'maxdate'=>$edate, 'cyears_l'=>$cyears_l, 'cquarts_l'=>$cquarts_l, 'cmonths_l'=>$cmonths_l, 'cweeks_l'=>$cweeks_l, 'cdays_l'=>$cdays_l, 'cdays_r'=>$cdays_r, 'cweeks_r'=>$cweeks_r, 'cmonths_r'=>$cmonths_r, 'cquarts_r'=>$cquarts_r, 'cyears_r'=>$cyears_r);
}

/**
 * Liefert die Anzahl gewünschter Wochentage für eine übergebene Zeitspanne zurück
 *
 * @param various $sdate - Mandatory parameter:                   ???PARAMETER???
 * @param various $edate - Mandatory parameter:                   ???PARAMETER???
 * @param integer $su    - Optional parameter (default = 0):      ???PARAMETER???
 * @param integer $mo    - Optional parameter (default = 1):      ???PARAMETER???
 * @param integer $tu    - Optional parameter (default = 1):      ???PARAMETER???
 * @param integer $we    - Optional parameter (default = 1):      ???PARAMETER???
 * @param integer $th    - Optional parameter (default = 1):      ???PARAMETER???
 * @param integer $fr    - Optional parameter (default = 1):      ???PARAMETER???
 * @param integer $sa    - Optional parameter (default = 0):      ???PARAMETER???
 * @param boolean $rev   - Optional parameter (default = false):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function countWeekdays($sdate, $edate, $su=0, $mo=1, $tu=1, $we=1, $th=1, $fr=1, $sa=0, $rev=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$wda = array('su', 'mo', 'tu', 'we', 'th', 'fr', 'sa');
	$stm = fx_mktime(12, 0, 0, (int)substr((string)$sdate, 4, 2), (int)substr((string)$sdate, 6, 2), (int)substr((string)$sdate, 0, 4));
	$swd = fx_date('w', $stm);
	$etm = fx_mktime(12, 0, 0, (int)substr((string)$edate, 4, 2), (int)substr((string)$edate, 6, 2), (int)substr((string)$edate, 0, 4));
	$ewd = fx_date('w', $etm);
	$wds = 0;

	for($cwd=0; $cwd<7; $cwd++)
	{
		$wd = ${$wda[$cwd]};
		if($rev)
			$wd = !$wd;
		if($wd)
		{
			if($cwd < $swd)
				$cstm = $stm + (7-$swd+$cwd) * 86400;
			else if($cwd > $swd)
				$cstm = $stm + ($cwd-$swd) * 86400;
			else
				$cstm = $stm;

			$cetm = $etm - abs(7-$cwd+$ewd)%7 * 86400;

			$wds += 1 + ($cetm-$cstm) / 604800;
		}
	}

	return round($wds, 0);
}

/**
 * ???FUNCTION???
 *
 * @param integer $sc - Optional parameter (default = 0):     ???PARAMETER???
 * @param boolean $hf - Optional parameter (default = false): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function secToTime($sc=0, $hf=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$sc = (int)$sc;

	$h = floor($sc/3600);
	$m = floor(($sc-($h*3600))/60);
	$s = $sc-($h*3600)-($m*60);

	if($hf)
		$tm=sprintf("%02d:%02d", $h, $m);
	else
		$tm=sprintf("%02d%02d%02d", $h, $m, $s);

	return $tm;
}

/**
 * ???FUNCTION???
 *
 * @param string $tm - Optional parameter (default = ''): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function timeToSec($tm='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$cp=strpos($tm,':');
	if($cp)
		$tm=substr((string)str_replace(':','',$tm).'000000',0,6);
	else
		$tm=sprintf("%06d", trim(substr((string)$tm, -6)));

	$h = (int)substr((string)$tm, 0, 2);
	$m = (int)substr((string)$tm, 2, 2);
	$s = (int)substr((string)$tm, 4);

	$sc = ($h*3600)+($m*60)+$s;

	return $sc;
}

/**
 * Get date limits
 */
function setDateLimits()
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!isset($GLOBALS['fxpvars']['date_minmax']))
	{
		$act_year=(int)fx_date('Y', time());

		// +/- x years (= admin setting)
		$GLOBALS['fxpvars']['date_minmax']=array(
			'min'=>$act_year-$GLOBALS['fxpglobals']['settings']['mindatum'],
			'max'=>$act_year+$GLOBALS['fxpglobals']['settings']['maxdatum']
		);

		$sql="SELECT MIN(soll_beginn) AS msb FROM zeitdaten WHERE soll_beginn IS NOT NULL AND zeitart=102 AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$min_prjyear=db_value($sql);
		if(($min_prjyear !== NULL) && strlen((string)$min_prjyear))
			$GLOBALS['fxpvars']['date_minmax']['min']=min($GLOBALS['fxpvars']['date_minmax']['min'], (int)substr((string)$min_prjyear,0,4));
	}
}

/**
 * Create iCalendar file (ics)
 *
 * @param various $aid             - Mandatory parameter:                  ???PARAMETER???
 * @param integer $cid             - Optional parameter (default = 0):     ???PARAMETER???
 * @param boolean $with_attachment - Optional parameter (default = true):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function iCalendar($aid, $cid=0, $with_attachment=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$afn='';

	$aid=(int)$aid;
	if(!$cid)
		$cid=$GLOBALS['fxpglobals']['client'];

	// Create iCalendar
	if($aid)
	{
		// Get latest entry of this appointment
		$sql  = "SELECT aktion, personen_id, terminart, name_termin, startzeit, endzeit, treffpunkt, beschreibung, zeitstempel";
		$sql .= " FROM termine_history";
		$sql .= " WHERE termin_id=".$aid." AND mandanten_id=".$cid;
		$sql .= " ORDER BY autoid DESC";
		$app=db_values($sql);
//fxDebug($app,$sql);

		if(!fxIsArray($app))
		{
			$sql  = "SELECT 2 AS aktion, personen_id, terminart, name_termin, startzeit, endzeit, treffpunkt, beschreibung, zeitstempel";
			$sql .= " FROM termine";
			$sql .= " WHERE termin_id=".$aid." AND mandanten_id=".$cid;
			$app=db_values($sql);
//fxDebug($app,$sql);
		}

		if(fxIsArray($app))
		{
			$pid=(int)$app['personen_id'];
			$aat=(int)$app['terminart'];
			$nat=getAcceptedApplicationID($aat);
			$nan=meldung($nat,false);

			$sql  = "SELECT b.name_benutzer, b.e_mail_adresse, p.pname, p.vorname";
			$sql .= " FROM personen p, benutzer b";
			$sql .= " WHERE p.personen_id=".$pid." AND b.personen_id=".$pid." AND p.mandanten_id=".$cid." AND b.mandanten_id=".$cid;
			$pdt=db_values($sql);
//fxDebug($pdt,$sql);

			$sql="SELECT COUNT(aktion) FROM termine_history WHERE termin_id=".$aid." AND aktion<>4 AND mandanten_id=".$cid;
			$seq=max(0,(int)db_value($sql)-1);
//fxDebug($seq,$sql);

			$uid=substr('0000'.$cid,-4).'-'.substr('000000000'.$pid,-9).'-'.substr('000000000'.$aid,-9);
			$dts=substr((string)$app['startzeit'],0,8);
			$dte=substr((string)$app['endzeit'],0,8);
			$sum=$pdt['vorname'].' '.$pdt['pname'].' ('.$nan.'): '.get_entry($dts,'datum');
			$fln=$pdt['vorname'].' '.$pdt['pname'].' ['.$nan.' '.get_entry($dts,'datum');
			if($dts != $dte)
			{
				$tts='000000';
				$tte='235959';
				$sum .= '-'.get_entry($dte,'datum');
				$fln .= '-'.get_entry($dte,'datum');
			}
			else
			{
				$tts=max('000000',substr((string)$app['startzeit'],8,6));
				$tte=min('235959',substr((string)$app['endzeit'],8,6));
				$sum .= ', '.get_entry($dts.$tts,'zeit').'-'.get_entry($dte.$tte,'zeit');
				$fln .= ', '.str_replace(':','.', get_entry($dts.$tts,'zeit').'-'.get_entry($dte.$tte,'zeit'));
			}
			$fln .= ']';

			if($app['aktion'] == FXP_DELETE)
				$method='CANCEL';
			else
				$method='PUBLISH';

			$nl="\r\n";

			$vloc='';
			if(strlen((string)$app['treffpunkt']))
			{
				$vloc=text_wrap($app['treffpunkt'],true,75,"\t","",$nl,false);
				if(function_exists('mb_convert_encoding'))
					$vloc=mb_convert_encoding($vloc, 'UTF-8', mb_detect_encoding($vloc));
			}
			$vsum=text_wrap($sum,true,75,"\t","",$nl,false);
			if(function_exists('mb_convert_encoding'))
				$vsum=mb_convert_encoding($vsum, 'UTF-8', mb_detect_encoding($vsum));
			$vdesc='';
			if(strlen((string)$app['treffpunkt']))
			{
				$vdesc=text_wrap($app['beschreibung'],true,75,"\t","",$nl,false);
				if(function_exists('mb_convert_encoding'))
					$vdesc=mb_convert_encoding($vdesc, 'UTF-8', mb_detect_encoding($vdesc));
			}

			$vc  = "BEGIN:VCALENDAR".$nl;
			$vc .= "VERSION:2.0".$nl;
			$vc .= "PRODID:-//fx-project v".$GLOBALS['version']."//DE".$nl;
			$vc .= "X-WR-RELCALID:".$uid.$nl;
			$vc .= "METHOD:".$method.$nl;
			$vc .= "CALSCALE:GREGORIAN".$nl;
			$vc .= "TZ:+00".$nl;

			$vc .= "BEGIN:VEVENT".$nl;
			$vc .= "UID:".$uid.$nl;
			$vc .= "SEQUENCE:".$seq.$nl;
			$vc .= "CATEGORIES:".$nan.$nl;
			$vc .= "CLASS:PUBLIC".$nl;
			$vc .= "CREATED:".getUTC($GLOBALS['datetime']).$nl;
			$vc .= "DTSTAMP:".getUTC($app['zeitstempel']).$nl;
			$vc .= "DTSTART:".getUTC($dts.$tts).$nl;
			$vc .= "DTEND:".getUTC($dte.$tte).$nl;
			$vc .= "LAST-MODIFIED:".getUTC($GLOBALS['datetime']).$nl;
			if(strlen($vloc))
				$vc .= "LOCATION:".$vloc.$nl;
			$vc .= "SUMMARY:".$vsum.$nl;
			if(strlen($vdesc))
				$vc .= "DESCRIPTION:".$vdesc.$nl;
			if($app['aktion'] == FXP_DELETE)
				$vc .= "STATUS:CANCELLED".$nl;
			$vc .= "END:VEVENT".$nl;

			$vc .= "END:VCALENDAR";
//echo('<pre>'.fxHtmlEncode($vc).'</pre>');

			// Save file
			$afn=$GLOBALS['tmppath'].$uid.'.ics';
//echo('$afn='.$afn.'<br />');
			$fe=fxSave($afn, $vc);
			// Add attachment
			if(!$fe && $with_attachment)
			{
				if(!isset($GLOBALS['mail_attachments']))
					$GLOBALS['mail_attachments']=array();
				$GLOBALS['mail_attachments'][$afn]=array('name'=>$fln.'.ics', 'summary'=>$sum, 'content'=>$vc);
			}
		}
	}

	return $afn;
}

/**
 * ???FUNCTION???
 *
 * @param various $timestamp - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function getUTC($timestamp)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$td=substr((string)$timestamp,0,8);

	$y=(int)substr((string)$td,0,4);
	$m=(int)substr((string)$td,4,2);
	$d=(int)substr((string)$td,6,2);

	$tt=substr((string)$timestamp,8,6);
	if($tt >= '240000')
		$tt='235959';

	$h=(int)substr((string)$tt,0,2);
	$i=(int)substr((string)$tt,2,2);
	$s=(int)substr((string)$tt,4,2);

	$utc=gmdate("Ymd\THis\Z", mktime($h,$i,$s, $m,$d,$y));

	return $utc;
}

/**
 * ???FUNCTION???
 *
 * @param integer $button  - Optional parameter (default = 0):  ???PARAMETER???
 * @param string  $pattern - Optional parameter (default = ''): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function iCalendarCombine($button=0,$pattern='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$ica=array('counter'=>0, 'file'=>"", 'names'=>"", 'content'=>"");
	if(fxIsArray($GLOBALS['mail_attachments']))
	{
//fxDebug($GLOBALS['mail_attachments'], 'mail_attachments');
		$econtent='';
		$nid='';
		$pln=strlen((string)$pattern);
		foreach($GLOBALS['mail_attachments'] as $af => $aa)
		{
			$uid=substr((string)$af,$afl-28,24);

			// Pattern match?
			$match=true;
			if(strlen((string)$pattern) && (substr((string)$uid,0,$pln) !== $pattern))
				$match=false;

			if($match)
			{
				if(!strlen((string)$ica['file']))
				{
					if(sizeof($GLOBALS['mail_attachments']) > 1)
						$ica['file']=substr((string)$af,0,-13).'000000000.ics';
					else
						$ica['file']=$af;
					$nid=substr((string)$ica['file'],$afl-28,24);
				}
//echo('$pattern=['.$pattern.'] -- $uid='.$uid.', $nid='.$nid.'<br />');

				if(strlen((string)$ica['names']))
					$ica['names'] .= '<br />';
				$ica['names'] .= $aa['summary'];

				$ica['counter']++;

				$bve=stripos($aa['content'],'BEGIN:VEVENT');
				if($bve)
				{
					if(!strlen((string)$ica['content']))
					{
						$afl=strlen((string)$af);
						$ica['content']=substr((string)$aa['content'],0,$bve);
						$ica['content']=str_replace($uid,$nid,$ica['content']);
					}
					$evc=stripos($aa['content'],'END:VCALENDAR');
					if($evc)
					{
						if(!strlen((string)$econtent))
							$econtent=substr((string)$aa['content'],$evc);
						$ica['content'] .= substr((string)$aa['content'],$bve,$evc-$bve);
					}
				}
			}
		}
		$ica['content'] .= $econtent;
//echo('<b class="blue">'.$ica['file'].'</b><hr /><b>'.$ica['names'].'</b><hr /><pre>'.fxHtmlEncode($ica['content']).'</pre>');

		fxSave($ica['file'], $ica['content']);
	}

	// Download-Button
	if($button === true)
		$button=1;
	else if($button === false)
		$button=0;
	else
		$button=(int)$button;
	if(strlen((string)$ica['file']) && $button && ($ica['counter'] >= $button))
	{
		if(!isset($GLOBALS['icta']))
			$GLOBALS['icta']=explode('|',meldung(3144));	// Termin im iCalendar-Format downloaden|Alle Termine im iCalendar-Format downloaden

		if($ica['counter'] == 1)
			$tt=$GLOBALS['icta'][0];
		else
			$tt=$GLOBALS['icta'][1];

		$GLOBALS['fxpvars']['ibuffer'][-9] .= '<a href="'.$ica['file'].'"><img src="'.$GLOBALS['gfxpath'].'im_cal.png" tooltip="'.$tt.'<br ><br /><i class=blue>'.$ica['names'].'</i>" style="position:relative;cursor:pointer;"></a>';
	}

	return $ica;
}

/**
 * ???FUNCTION???
 *
 * @param various $y     - Mandatory parameter:                     ???PARAMETER???
 * @param integer $m     - Optional parameter (default = 0):        ???PARAMETER???
 * @param integer $w     - Optional parameter (default = 0):        ???PARAMETER???
 * @param integer $d     - Optional parameter (default = 0):        ???PARAMETER???
 * @param string  $f     - Optional parameter (default = 'right'):  ???PARAMETER???
 * @param boolean $trlnk - Optional parameter (default = false):    ???PARAMETER???
 *
 * @return ???RETURN???
 */
function addNavigation($y, $m=0, $w=0, $d=0, $f='right', $trlnk=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$y=(int)$y;
	$m=(int)$m;
	$d=(int)$d;

	$td=substr((string)$GLOBALS['datetime'],0,8);
	$ay=(int)substr((string)$td,0,4);
	if(!$GLOBALS['fxptdata']['ignore_date_boundaries'])
	{
		setDateLimits();
		$miny=(int)$GLOBALS['fxpvars']['date_minmax']['min'];
		$maxy=(int)$GLOBALS['fxpvars']['date_minmax']['max'];
		if(!$miny)
			$miny=$ay-100;
		if(!$maxy)
			$maxy=$ay+50;
		$y=max(min($y, $GLOBALS['fxpvars']['date_minmax']['max']), $GLOBALS['fxpvars']['date_minmax']['min']);
	}
	else
	{
		$miny=$ay-100;
		$maxy=$ay+50;
	}

	$jt=meldung(1697).': ';	// Sprung zu

	$n=' <div style="float:'.$f.';margin-right:8px;">';
	// Jahresnavigation
	if(!$m)
	{
		if($f == 'right')
		{
			// ...Navigation: Vorheriges Jahr
			$n .= '<img src="'.$GLOBALS['gfxpath'].'it_back.png" ';
			if($y > $miny)
			{
				if($trlnk)
					$l=fxf_jsFunction('loadTR', '^loading', $GLOBALS['fxpglobals']['tr'], 'nav&keep_get=1&navy='.($y-1));
				else
					$l=fxf_jsFunction('changeYear', '', ($y-1), 1, 1);
				$n .= ' tooltip="'.$jt.'<b>'.($y-1).'</b>" '.$l.' style="cursor:pointer;">';
			}
			else
				$n .= ' style="opacity:0.25;">';
			// ...Navigation: Nächstes Jahr
			$n .= '<img src="'.$GLOBALS['gfxpath'].'it_next.png" ';
			if($y < $maxy)
			{
				if($trlnk)
					$l=fxf_jsFunction('loadTR', '^loading', $GLOBALS['fxpglobals']['tr'], 'nav&keep_get=1&navy='.($y+1));
				else
					$l=fxf_jsFunction('changeYear', '', ($y+1), 1, 1);
				$n .= ' tooltip="'.$jt.'<b>'.($y+1).'</b>" '.$l.' style="cursor:pointer;margin-left:3px;">';
			}
			else
				$n .= ' style="opacity:0.25;">';
		}
		else
		{
			if($trlnk)
				$l=fxf_jsFunction('loadTR', '^loading', $GLOBALS['fxpglobals']['tr'], 'nav&keep_get=1&navy='.$y);
			else
				$l=fxf_jsFunction('changeYear', '', $y, 1, 1);
			$n .= '<img src="'.$GLOBALS['gfxpath'].'ic_undo.png" tooltip="'.$jt.'<b>'.$y.'</b>" '.$l.' style="cursor:pointer;">';
		}
	}
	// Monatsnavigation
	else if(!$w)
	{
		// Monatsnamen
		if(!fxIsArray($GLOBALS['monatsnamen']))
			$GLOBALS['monatsnamen'] = db_values("SELECT tabwert FROM wertetab WHERE id_feld=62 AND id_sprache=".$GLOBALS['fxpglobals']['lang']." AND mandanten_id=0 ORDER BY position", '*');
		if($f == 'right')
		{
			// ...Navigation: Vorheriges Monat
			$n .= '<img src="'.$GLOBALS['gfxpath'].'it_back.png" ';
			if(($y > $miny) || ($m > 1))
			{
				$ny=$y;
				$nm=$m-1;
				if(!$nm)
				{
					$nm=12;
					$ny--;
				}
				if($trlnk)
					$l=fxf_jsFunction('loadTR', '^loading', $GLOBALS['fxpglobals']['tr'], 'nav&keep_get=1&navy='.$ny.'&navm='.$nm);
				else
					$l=fxf_jsFunction('changeMonth', '', $ny, $nm, 1);
				$n .= ' tooltip="'.$jt.'<b>'.$GLOBALS['monatsnamen'][$nm-1]['tabwert'].' '.$ny.'</b>" '.$l.' style="cursor:pointer;">';
			}
			else
				$n .= ' style="opacity:0.25;">';
			// ...Navigation: Nächstes Monat
			$n .= '<img src="'.$GLOBALS['gfxpath'].'it_next.png" ';
			if(($y < $maxy) || ($m < 12))
			{
				$ny=$y;
				$nm=$m+1;
				if($nm > 12)
				{
					$nm=1;
					$ny++;
				}
				if($trlnk)
					$l=fxf_jsFunction('loadTR', '^loading', $GLOBALS['fxpglobals']['tr'], 'nav&keep_get=1&navy='.$ny.'&navm='.$nm);
				else
					$l=fxf_jsFunction('changeMonth', '', $ny, $nm, 1);
				$n .= ' tooltip="'.$jt.'<b>'.$GLOBALS['monatsnamen'][$nm-1]['tabwert'].' '.$ny.'</b>" '.$l.' style="cursor:pointer;margin-left:3px;">';
			}
			else
				$n .= ' style="opacity:0.25;">';
		}
		else
		{
			if($trlnk)
				$l=fxf_jsFunction('loadTR', '^loading', $GLOBALS['fxpglobals']['tr'], 'nav&keep_get=1&navy='.$y.'&navm='.$m);
			else
				$l=fxf_jsFunction('changeMonth', '', $y, $m, 1);
			$n .= '<img src="'.$GLOBALS['gfxpath'].'ic_undo.png" tooltip="'.$jt.'<b>'.$GLOBALS['monatsnamen'][$m-1]['tabwert'].' '.$y.'</b>" '.$l.' style="cursor:pointer;">';
		}
	}
	// Wochennavigation
	else if(!$d)
	{
		$jt .= '<b>'.meldung(1036).' ';	// KW
		$dt=substr('0000'.$y,-4).substr('00'.$m,-2).substr('00'.$w,-2);
		$et=getFirstWeekday($dt);
		if($f == 'right')
		{
			// ...Navigation: Vorherige Woche
			$n .= '<img src="'.$GLOBALS['gfxpath'].'it_back.png" ';
			if(($y > $miny) || ($m > 1) || ($w > 7))
			{
				$pd=substr((string)time_add($et,-7,'D'),0,8);
				$cw=kw($pd);
				if($trlnk)
					$l=fxf_jsFunction('loadTR', '^loading', $GLOBALS['fxpglobals']['tr'], 'nav&keep_get=1&navy='.(int)substr((string)$pd,0,4).'&navm='.(int)substr((string)$pd,4,2).'$navd='.(int)substr((string)$pd,6,2).'&navw='.$cw);
				else
					$l=fxf_jsFunction('changeWeek', '', (int)substr((string)$pd,0,4), (int)substr((string)$pd,4,2), (int)substr((string)$pd,6,2), $cw);
				$n .= ' tooltip="'.$jt.$cw.', '.substr((string)$pd,0,4).'</b>" '.$l.' style="cursor:pointer;">';
			}
			else
				$n .= ' style="opacity:0.25;">';
			// ...Navigation: Nächstes Woche
			$n .= '<img src="'.$GLOBALS['gfxpath'].'it_next.png" ';
			if(($y < $maxy) || ($m < 12) || ($w < 31))
			{
				$nd=substr((string)time_add($et,7,'D'),0,8);
				$cw=kw($nd);
				if($trlnk)
					$l=fxf_jsFunction('loadTR', '^loading', $GLOBALS['fxpglobals']['tr'], 'nav&keep_get=1&navy='.(int)substr((string)$nd,0,4).'&navm='.(int)substr((string)$nd,4,2).'$navd='.(int)substr((string)$nd,6,2).'&navw='.$cw);
				else
					$l=fxf_jsFunction('changeWeek', '', (int)substr((string)$nd,0,4), (int)substr((string)$nd,4,2), (int)substr((string)$nd,6,2), $cw);
				$n .= ' tooltip="'.$jt.$cw.', '.substr((string)$nd,0,4).'</b>" '.$l.' style="cursor:pointer;margin-left:3px;">';
			}
			else
				$n .= ' style="opacity:0.25;">';
		}
		else
		{
			$cw=kw($et);
			if($trlnk)
				$l=fxf_jsFunction('loadTR', '^loading', $GLOBALS['fxpglobals']['tr'], 'nav&keep_get=1&navy='.(int)substr((string)$et,0,4).'&navm='.(int)substr((string)$et,4,2).'$navd='.(int)substr((string)$et,6,2).'&navw='.$cw);
			else
				$l=fxf_jsFunction('changeWeek', '', (int)substr((string)$et,0,4), (int)substr((string)$et,4,2), (int)substr((string)$et,6,2), $cw);
			$n .= '<img src="'.$GLOBALS['gfxpath'].'ic_undo.png" tooltip="'.$jt.$cw.', '.substr((string)$et,0,4).'</b>" '.$l.' style="cursor:pointer;">';
		}
	}
	// Tagesnavigation
	else
	{
		// Wochentagsnamen
		if(!fxIsArray($GLOBALS['wochentagsnamen']))
			$GLOBALS['wochentagsnamen']=db_values("SELECT tabwert FROM wertetab WHERE id_feld=289 AND id_sprache=".$GLOBALS['fxpglobals']['lang']." AND mandanten_id=0 ORDER BY position", '*');

		$dt=substr('0000'.$y,-4).substr('00'.$m,-2).substr('00'.$d,-2);
		$wt=getWeekday($dt);
		if($f == 'right')
		{
			// ...Navigation: Vorherigen Arbeitstag
			$n .= '<img src="'.$GLOBALS['gfxpath'].'it_back.png" ';
			if(($y > $miny) || ($m > 1) || ($d > 1))
			{
				$dd=-1;
				if($wt == 1)	// Am Montag zum letzten Freitag springen
					$dd=-3;
				$pd=substr((string)time_add($dt,$dd,'D'),0,8);
				$pwt=getWeekday($pd);
				if($trlnk)
					$l=fxf_jsFunction('loadTR', '^loading', $GLOBALS['fxpglobals']['tr'], 'nav&keep_get=1&navy='.(int)substr((string)$pd,0,4).'&navm='.(int)substr((string)$pd,4,2).'$navd='.(int)substr((string)$pd,6,2));
				else
					$l=fxf_jsFunction('changeDay', '', (int)substr((string)$pd,0,4), (int)substr((string)$pd,4,2), (int)substr((string)$pd,6,2));
				$n .= ' tooltip="'.$jt.'<b>'.$GLOBALS['wochentagsnamen'][$pwt]['tabwert'].', '.get_entry($pd,'datum').'</b>" '.$l.' style="cursor:pointer;">';
			}
			else
				$n .= ' style="opacity:0.25;">';
			// ...Navigation: Nächsten Arbeitstag
			$n .= '<img src="'.$GLOBALS['gfxpath'].'it_next.png" ';
			if(($y < $maxy) || ($m < 12) || ($d < 31))
			{
				$dd=1;
				if($wt == 5)	// Am Freitag zum nächsten Montag springen
					$dd=3;
				$nd=substr((string)time_add($dt,$dd,'D'),0,8);
				$nwt=getWeekday($nd);
				if($trlnk)
					$l=fxf_jsFunction('loadTR', '^loading', $GLOBALS['fxpglobals']['tr'], 'nav&keep_get=1&navy='.(int)substr((string)$nd,0,4).'&navm='.(int)substr((string)$nd,4,2).'$navd='.(int)substr((string)$nd,6,2));
				else
					$l=fxf_jsFunction('changeDay', '', (int)substr((string)$nd,0,4), (int)substr((string)$nd,4,2), (int)substr((string)$nd,6,2));
				$n .= ' tooltip="'.$jt.'<b>'.$GLOBALS['wochentagsnamen'][$nwt]['tabwert'].', '.get_entry($nd,'datum').'</b>" '.$l.' style="cursor:pointer;margin-left:3px;">';
			}
			else
				$n .= ' style="opacity:0.25;">';
		}
		else
		{
			if($trlnk)
				$l=fxf_jsFunction('loadTR', '^loading', $GLOBALS['fxpglobals']['tr'], 'nav&keep_get=1&navy='.(int)substr((string)$dt,0,4).'&navm='.(int)substr((string)$dt,4,2).'$navd='.(int)substr((string)$dt,6,2));
			else
				$l=fxf_jsFunction('changeDay', '', (int)substr((string)$dt,0,4), (int)substr((string)$dt,4,2), (int)substr((string)$dt,6,2));
			$n .= '<img src="'.$GLOBALS['gfxpath'].'ic_undo.png" tooltip="'.$jt.'<b>'.$GLOBALS['wochentagsnamen'][$wt]['tabwert'].', '.get_entry($dt,'datum').'</b>" onclick="'.$l.'" style="cursor:pointer;">';
		}
	}
	$n .= '</div>';

	return $n;
}

/**
 * Convert year
 *
 * @param integer $y - Optional parameter (default = 0):  Year to be converted - current year if <= 0
 *
 * @return Converted integer year
 */
function fxf_getYear($y=0)
{
	$cy=(int)substr((string)$GLOBALS['date'],0,4);						// Current year
	$cc=(int)$GLOBALS['fxpglobals']['settings']['jhwechsel'];	// Century change value

	// Year value -> Return current year if <= 0
	$y=(int)$y;
	if($y <= 0)
		return $cy;

	// Return calculated year depending on the digit numbers
	$l=strlen((string)$y);

	// 1-digit year?
	// ...Assume current century + value, e.g. 8 for the year 2020 -> 2000 + 8 = 2008
	if($l == 1)
		return floor($cy/100)*100 + $y;

	// 2-digit year?
	// ...Assume current century + value if the value is less then the defined century change value, e.g. 22 for the year 2020 -> 2000 + 22 = 2022
	// ...Assume previous century + value if the value is greater then the defined century change value, e.g. 69 for the year 2020 -> 1900 + 69 = 1969
	if($l == 2)
	{
		if($y < $cc)
			return floor($cy/100)*100 + $y;
		return (floor($cy/100)-1)*100 + $y;
	}

	// 3-digit year?
	// ...Assume current millenium + value if the value is less then the defined century change value * 100, e.g. 123 for the year 2020 -> 2000 + 123 = 2123
	// ...Assume previous millenium + value if the value is greater then the defined century change value * 100, e.g. 969 for the year 2020 -> 1000 + 969 = 1969
	if($l == 2)
	{
		if($y < $cc*100)
			return floor($cy/1000)*1000 + $y;
		return (floor($cy/1000)-1)*1000 + $y;
	}

	// 4-digit year (should be maximum, unless this program is stilled running beyond the year 10000)
	return $y;
}

/**
 * Calculate a specific date for a year
 *
 * @param string  $d - Optional parameter (default = ''):  Date formula - current date if empty
 * @param integer $y - Optional parameter (default = 0):   Year - current year if <= 0
 *
 * @return Day string in the format YYYYMMDD
 */
function fxf_calcDate($d='', $y=0)
{
	// Date formula
	$d=strtoupper(trim((string)$d));
	if(!strlen($d))
		return $GLOBALS['date'];

	// Year
	$y=fxf_getYear($y);
//echo('<b>fxf_calcDate</b>($d=\''.$d.'\', $y='.$y.')<br />');

	// ...Check specified
	$cc='';
	$cd='';
	$pp=strpos($d,'|');
	if($pp)
	{
		$cc=substr($d,$pp+1);
		$d=substr($d,0,$pp);
		$qp=strpos($cc,'?');
		if($qp)
		{
			$cd=substr($cc,$qp+1);
			$cc=substr($cc,0,$qp);
		}
	}
	if(strlen($cc) && strlen($cd))
	{
		$cc=fxf_calcDate($cc, $y);
		$cd=fxf_calcDate($cd, $y);
	}

	// ...Adjustments specified
	$ad='';
	$gp=strpos($d,'>');
	if($gp)
	{
		$ad=substr($d,$gp+1);
		$d=substr($d,0,$gp);
	}
//echo('&rarr; $cc=\''.$cc.'\', $cd=\''.$cd.'\', $d=\''.$d.'\', $ad=\''.$ad.'\'<br />');

	// Calculate day from format
	// ...Easter calculations start with "E"
	if(substr($d,0,1) == 'E')
	{
		$d=substr($d,1);

		// [E] -> Easter falls on the first Sunday after the cyclically determined full moon, which takes place on or after March 21. If the church spring full moon falls on a Sunday, Easter is celebrated on the following Sunday.
	    $ed=easter_days($y);
		$dt=new DateTime($y.'-03-21');
		$dt->add(new DateInterval('P'.$ed.'D'));
//fxDebug($dt,'Easter - $dt: $y='.$y.', $ed='.$ed, 0);
	}
	// ...Advent calculations start with "A"
	else if(substr($d,0,1) == 'A')
	{
		$d=substr($d,1);

		// [A] -> Eternity Sunday (4th Advent) is the Sunday before or on Christmas Eve, December 24
		$dt=new DateTime($y.'-12-24');
		$dt->sub(new DateInterval('P'.$dt->format('w').'D'));
//fxDebug($dt,'Advent - $dt: $y='.$y, 0);
	}
	// ...Everything else
	else
	{
		$b=substr('0000'.$d,-4);
		$bm=(int)substr($b,0,2);
		$bd=(int)substr($b,2,2);
		$dt=new DateTime($y.'-'.substr('0'.$bm,-2).'-'.substr('0'.$bd,-2));
//fxDebug($dt,'Normal - $dt: $y='.$y, 0);
	}

	if(strlen($ad))
		$dt->modify($ad);
	$rd=$dt->format('Ymd');

	// Check
	if(strlen($cc) && strlen($cd) && ($rd == $cc))
		$rd=$cd;

//echo('&rarr; $rd=\'<b>'.$rd.'</b>\'<hr />');
	return $rd;
}

/**
 * ???FUNCTION???
 *
 * @param boolean $old   - Optional parameter (default = true):  ???PARAMETER???
 * @param boolean $short - Optional parameter (default = false): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function fxf_getWeekdayNames($short=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$wda=fxf_getReferenceValues('weekdays');
//fxDebug($wda,'$wda', 0);
	if(!fxIsArray($wda))
		$wda=array('Sunday', 'Monday', 'Tuesday', 'Wednesday', 'Thursday', 'Friday', 'Saturday');

	$short=(int)$short;
	if($short > 0)
	{
		foreach($wda as $k => $v)
			$wda[$k]=substr((string)$v,0, $short);
	}

	return $wda;
}

/**
 * Convert date into ms format
 *
 * @param string $dt - Mandatory parameter:  Date in fx-project format, i.e. YYYYMMDD
 * @param string $df - Mandatory parameter:  Destination format
 *
 * @return string Converted date
 */
function msdatum($dt, $df)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$dr=array('yyyy'=>substr((string)$dt, 0,4), 'yy'=>substr((string)$dt, 2,2), 'mm'=>substr((string)$dt, 4,2), 'dd'=>substr((string)$dt, 6,2));
	$dw=fx_date('w', fx_mktime(12,0,0, (int)$dr['mm'],(int)$dr['dd'],(int)$dr['yyyy']));
	$rd=strtr($df, $dr);
	if(fxIsArray($GLOBALS['wtn']) && $GLOBALS['wtn'][$dw])
		$rd=$GLOBALS['wtn'][$dw].' '.$rd;

	return $rd;
}

/**
 * Get array of days per month
 *
 * @param integer $year - Mandatory parameter:  Year
 *
 * @return Array of days per motnh
 */
function fxf_mdays($year)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$a=array(1=>31,28,31,30,31,30, 31,31,30,31,30,31);
	$l=fx_date('L', fx_mktime(12,0,0, 1,1,$year));
	if($l)
		$a[2]=29;

	return $a;
}
?>