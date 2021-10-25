<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : termine_abw_val.inc                                          //
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
 * PF 222: Absence Planner
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

$Termin_ID=$GLOBALS['fxptdata']['terminid'];
$dbwerte['termin_id']=$Termin_ID;

// Modus ermitteln
if(($dbwerte['fehltagsart'] == FXP_AT_SLIDINGDAYREQUEST) || ($dbwerte['fehltagsart'] == FXP_AT_SLIDINGDAY))
	$tmode = 1;
else if(($dbwerte['fehltagsart'] == FXP_AT_OVERTIMEOUTPAYREQUEST) || ($dbwerte['fehltagsart'] == FXP_AT_OVERTIMEOUTPAY))
	$tmode = 2;
else
{
	$tmode = 0;
	// Keine halben (Sonder-)Urlaubstage erlaubt?
	if((($dbwerte['fehltagsart'] == FXP_AT_VACATIONREQUEST) || ($dbwerte['fehltagsart'] == FXP_AT_VACATION) || ($dbwerte['fehltagsart'] == FXP_AT_SPECIALVACATIONREQUEST) || ($dbwerte['fehltagsart'] == FXP_AT_SPECIALVACATION)) && (strtolower(substr($GLOBALS['fxpglobals']['settings']['halbe_urlaube'], 0, 1)) == 'n'))
		$ganztaegig = 2571;
}
if($tmode)
	$ganztaegig = 2571;	// Bei Gleitzeit und Überstundenauszahlung immer "ganztägig"
if($ganztaegig != 2571)
	$Soll_End_Dtm = '';

// Personendaten ermitteln
$pdata = get_persondata($persid);
if(!is_array($pdata))
{
	// 550: Die gewählte Person besitzt noch keine Zusatzdaten!
	error_msg(550, false);
	$err = true;
}

// Datumsprüfung
if(!$err)
{
	$db_startdatum = substr($dbwerte['soll_beg_dtm'], 1, 8);
	// Enddatum
	if($Soll_End_Dtm && ($tmode < 2))
		$db_enddatum = substr($dbwerte['soll_end_dtm'], 1, 8);
	else
		$db_enddatum = $db_startdatum;
	// Enddatum vor dem Startdatum
	if($db_enddatum < $db_startdatum)
	{
		// 551: Das Enddatum liegt vor dem Startdatum!
		error_msg(551, -1, 'Soll_End_Dtm');
		$err = true;
	}
}

// Halbtägige Abwesenheitstage können nur für einen Tag genommen werden
if(!$err && ($ganztaegig != 2571) && ($db_startdatum != $db_enddatum))
{
	// 476: Ein halbtägiger Abwesenheitstag darf nicht über einen mehrtägigen Zeitraum genommen werden!
	error_msg(476, -1, 'Soll_End_Dtm');
	$err = true;
}

// Prüfen bei ganztägigen Abwesenheiten, ob bereits Zeiterfassung - falls ja, dann Warnhinweis anzeigen
if(!$err && ($ganztaegig == 2571))
{
	$trd=okTimeRegistration(1,$persid,$db_startdatum,$db_enddatum);
	if(strlen($trd))
	{
		error_msg(3015, array('Soll_Beg_Dtm','Soll_End_Dtm'), $trd);	// Zeit- oder Materialverbrauchserfassung an folgenden Tagen vorhanden:<br />$var1<br /><br /><font class=red>Es dürfen an diesen Tagen keine ganztägigen Abwesenheitstage genommen werden!</font>
		$err=true;
	}
}

// Start- + Endzeit
if(!$err && ($tmode != 2))
{
	$asc = validate_entry(get_entry($pdata['soll_beginn'], 'zeit'), $ierr, 'zeitspanne');
	$ast = ($pdata['wochenarbeitstd']*3600)/$pdata['arbeitstage'];
	// Startzeit
	if($ganztaegig == 2573) // halbtägig (2. Hälfte)
	{
		$ssc = $asc+($ast/2)+$pdata['pause'];
		$h = (int)($ssc/3600); $ssc = $ssc-$h*3600;
		$m = (int)($ssc/60);
		$s = $ssc-$m*60;
		$db_startzeit = substr('0'.$h, -2).substr('0'.$m, -2).substr('0'.$s, -2);
	}
	else
		$db_startzeit = substr($pdata['soll_beginn'], 8);
	// Endzeit
	if($ganztaegig == 2572) // halbtägig (1. Hälfte)
			$ssc = $asc+($ast/2);
		else
			$ssc = $asc+$ast+$pdata['pause'];
	$h = (int)($ssc/3600); $ssc = $ssc-$h*3600;
	$m = (int)($ssc/60);
	$s = $ssc-$m*60;
	$db_endzeit = substr('0'.$h, -2).substr('0'.$m, -2).substr('0'.$s, -2);

	$db_start = $db_startdatum.$db_startzeit;
	$db_end = $db_enddatum.$db_endzeit;
//fxDebug($db_end, $db_start);
}

// Stunden
if($tmode == 2)		// Überstundenauszahlung
{
	$_POST['stunden'] = validate_entry($_POST['stunden'], $verr, 'dezimal', '2');
	if($verr)
	{
		// 596: Das Feld "$var1" darf nicht leer sein. Bitte geben Sie eine gültige, positive Zahl ungleich 0 oder 0,00 ein!
		error_msg(596, -1, 'stunden');
		$err = true;
	}
	else
	{
		$db_stunden = $_POST['stunden'];

		// Prüfen, ob Monatsletzter eingegeben wurde...
		$year	= (int)substr($db_startdatum, 0, 4);
		$month	= (int)substr($db_startdatum, 4, 2);
		$last	= fx_date('Ymd', fx_mktime(12, 0, 0, $month+1, 0, $year));

		// ...falls Nein, dann Datum anpassen und Hinweis anzeigen
		if($db_startdatum != $last)
		{
			// 1944: Die Überstundenauszahlung kann nur am Letzten eines Monats erfolgen! Ihre Datumseingabe [= $var1] wurde deshalb abgeändert auf [$var2].
			error_msg(1944, array('Soll_Beg_Dtm'), '<b>'.get_entry($db_startdatum, 'datum').'</b>', '<b>'.get_entry($last, 'datum').'</b>');
			$err = true;
		}

		$db_startdatum	= $last;
		$db_enddatum	= $last;
		$db_start		= $last.'000000';
		$db_end			= $db_start;

		$_POST['Soll_Beg_Dtm']				= validate_entry($db_start, $verr, 'datum');
		$maskenfeldwerte['soll_beg_dtm']	= $db_startdatum;

		// Prüfen, ob eingegebene Stundenanzahl auch tatsächlich als Überstunden geleistet wurden...
		$first = fx_date('Ymd', fx_mktime(12, 0, 0, $month+1, 1, $year));
		$real_overtime_sec	= (int)summe_zeitkonto($persid, $first);
		$pay_overtime_sec	= (int)($db_stunden*3600);

		// ...falls Nein, dann Fehlermeldung
		if($pay_overtime_sec > $real_overtime_sec)
		{
			// 1945: Sie können keine $var1 h Überstundenauszahlung in Anspruch nehmen, da Ihre Überstunden am Ende dieses Monats lediglich $var2 h betragen!
			error_msg(1945, array('stunden'), '<b>'.get_entry($pay_overtime_sec/3600, 'dezimal').'</b>', '<b>'.get_entry($real_overtime_sec/3600, 'dezimal').'</b>');
			$err = true;
		}
	}
}
else if($tmode == 1)	// Gleitzeit
	$db_stunden = $pdata['sollstunden_tag'];
else					// Abwesenheit
	$db_stunden = 'NULL';

// Prüfen, ob Feiertage bereits für die betroffenen Jahre eingetragen wurden
if(!$err && ($tmode < 2))
{
	$year_array = array((int)substr($db_startdatum, 0, 4) => true);
	$year_array[(int)substr($db_enddatum, 0, 4)] = true;
	foreach($year_array as $year => $dummy)
	{
		if(!isset($valid_years[$year]))
		{
			// 2801: Es können noch keine Termine für das Jahr $var1 erfasst werden, da für dieses Jahr noch keine Feiertage hinterlegt wurden! Bitte legen Sie zuerst Feiertage an oder wenden Sie sich an Ihren Administrator.
			error_msg(2801, false, '<b>'.$year.'</b>');
			$err = true;
			break;
		}
	}
}

// Externe Mitarbeiter haben keinen (Sonder-)Urlaubsanspruch
// HINWEIS: Wird nur zur Sicherheit geprüft, da diese Auswahlmöglichkeit eigentlich auf Javascript-Ebene bereits blockiert wird
if(!$err && $persid && isset($contractors_arr[$persid]) && !$cont_like_emp && (($dbwerte['fehltagsart'] == FXP_AT_VACATIONREQUEST) || ($dbwerte['fehltagsart'] == FXP_AT_VACATION) || ($dbwerte['fehltagsart'] == FXP_AT_SPECIALVACATIONREQUEST) || ($dbwerte['fehltagsart'] == FXP_AT_SPECIALVACATION)))
{
	// 2582: Externe Mitarbeiter können keinen Urlaub nehmen!
	error_msg(2582, -1, 'Personen_ID');
	$err = true;
}

// Abwesenheitstage können logischerweise nur in dem Zeitraum stattfinden, in dem der Mitarbeiter in der Firma ist
if(!$err)
{
	if(substr($db_start, 0, 8) < substr($pdata['maeintrittsdatum'], 0, 8))
	{
		// 473: Das Eintrittsdatum ($var2) des Mitarbeiters liegt nach dem geplanten Startdatum ($var3)!
		error_msg(473, -1, 'Soll_Beg_Dtm', get_entry($pdata['maeintrittsdatum'], 'datum'), get_entry($db_start, 'datum'));
		$err = true;
	}
	if($pdata['maaustrittsdatum'] && (substr($db_end, 0, 8) > substr($pdata['maaustrittsdatum'], 0, 8)))
	{
		// 474: Das Austrittsdatum ($var2) des Mitarbeiters liegt vor dem geplanten Enddatum ($var3)!
		error_msg(474, -1, 'Soll_End_Dtm', get_entry($pdata['maaustrittsdatum'], 'datum'), get_entry($db_end, 'datum'));
		$err = true;
	}
}

// Prüfen, ob bereits Termine in diesem Zeitraum eingetragen sind
if(!$err && ($dbwerte['fehltagsart'] != FXP_AT_OVERTIMEOUTPAYREQUEST) && ($dbwerte['fehltagsart'] != FXP_AT_OVERTIMEOUTPAY))
{
	// Abwesenheitstermine dürfen sich generell nicht mit anderen Abwesenheitsterminen überschneiden
	$termine_no = FXP_AT_ABSENT.",".FXP_AT_VACATIONREQUEST.",".FXP_AT_VACATION.",".FXP_AT_SPECIALVACATIONREQUEST.",".FXP_AT_SPECIALVACATION.",".FXP_AT_TRAININGREQUEST.",".FXP_AT_TRAINING.",".FXP_AT_SICKCALL.",".FXP_AT_SICK.",".FXP_AT_SLIDINGDAYREQUEST.",".FXP_AT_SLIDINGDAY;

	$stsql  = "SELECT termin_id, startzeit, endzeit, terminart, name_termin FROM termine";
	$stsql .= " WHERE (archiv=0 OR archiv IS NULL) AND personen_ID=".$persid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$stsql .= " AND terminart IN (".$termine_no.")";
	$stsql .= " AND (bestaetigung_anmerk IS NULL OR bestaetigung_anmerk NOT LIKE '0-%') AND startzeit<='".$db_end."' AND endzeit>='".$db_start."'";
	if($dbwerte['termin_id'])
		$stsql .= " AND termin_id<>".$dbwerte['termin_id'];
	$stsql .= " ORDER BY startzeit";
	$starr = db_values($stsql, '*');
//fxDebug($starr, $stsql);
	if(is_array($starr))
	{
		$txt = '<br><ul>';
		foreach($starr as $stv)
			$txt .= '<li><font color=#0000aa>['.$tarten[$stv['terminart']].']</font><br>'.get_entry($stv['startzeit'], 'datetime').' - '.get_entry($stv['endzeit'], 'datetime').': <b>'.$stv['name_termin'].'</b></li>';
		$txt .= '</ul>';

		// 475: In diesem Zeitraum ($var1) können keine Abwesenheitstage eingetragen werden, da Termine vorhanden sind!$var2
		error_msg(475, false, get_entry($db_start, 'datetime').' - '.get_entry($db_end, 'datetime'), $txt);
		$err = true;
	}
}

// Abwesenheitstage zählen
$awtage = 0.00;
$satage = array();
if(!$err && ($tmode != 2))
{
	$mtage = array(1=>31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31);
	$feiertage = get_feiertage($db_start, $db_end);
	$atag = $db_startdatum;
	$etag = $db_enddatum;
	$st = (int)substr($atag, 6, 2);
	$sm = (int)substr($atag, 4, 2);
	$sj = (int)substr($atag, 0, 4);
	$lw = fx_date('Lw', fx_mktime(12, 0, 0, $sm, $st, $sj));
	if(substr($lw, 0, 1))
		$mtage[2] = 29;
	// Wochentag: 0=Sonntag, ..., 6=Samstag
	$wt = (int)substr($lw, 1);
	while($atag <= $etag)
	{
		// Arbeitstag?
		if($pdata[chr(0x61+$wt)])
		{
			$add = 1.00;
			// Feiertag
			if(fxIsArray($feiertage,$atag))
			{
				// Ganzer Feiertag
				if($feiertage[$atag]['art'] == FXP_HL_FULL)
					$add = 0.00;
				// Halber Feiertag
				else if($feiertage[$atag]['art'] == FXP_HL_HALF)
					$add = 0.50;
			}
			if($ganztaegig == 2571)
				$awtage += $add;
			else
				$awtage += ($add/2.0);

			// Gleittag
			if($add && ($tmode == 1))
				$satage[$atag] = $add;
		}

		// Tag erhöhen
		$st++;
		$wt++;
		if($wt > 6)
			$wt = 0;
		if($st > $mtage[$sm])
		{
			$st = 1;
			$sm++;
			if($sm > 12)
			{
				$sm = 1;
				$sj++;
				if(fx_date('L', fx_mktime(12, 0, 0, $sm, $st, $sj)))
					$mtage[2] = 29;
				else
					$mtage[2] = 28;
			}
		}
		$atag = $sj.substr('00'.$sm, -2).substr('00'.$st, -2);
	}

	if($awtage == 0.00)
	{
		// 477: In dem angegebenen Zeitraum fallen keine Abwesenheitstage an.
		error_msg(477, -1, 'Ist_Beg_Dtm');
		$err = true;
	}
}

if(!sizeof($satage))
	$satage[$db_startdatum] = 0.00;
?>