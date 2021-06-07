<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : popup_cal.inc                                                //
// Version     : 21.1                                                         //
// Begin       : 2020-08-20                                                   //
// Last Change : 2020-07-23                                                   //
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
 * Mask: "Calendar Icon"
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.1
 */


// Search for and include "basics.inc" to set all definitions, variables and necessary dynamic paths
$__pnm='basics.inc';
if(!isset($GLOBALS['__loaded_'.$__pnm]))
{
	$__prg=$__pnm;
	$__pfc=0;
	while(($__pfc < 10) && !file_exists($__prg))
	{
		$__pfc++;
		$__prg='../'.$__prg;
	}
	if(file_exists($__prg))
	{
		require($__prg);
	}
	else
		die('<div style="padding:8px;"><h1 style="padding:8px;color:red;background:#fff;border:2px solid red;box-shadow:4px 4px 4px rgba(0,0,0, 0.5);">fx-project - ERROR: Necessary file &quot;'.$__pnm.'&quot; not found!</h1></div>');
}

// Was program call by JavaScript/AJAX or included
$callAJAX=false;
if(fxIsArray($_GET) && isset($_GET['fieldname']) && isset($_GET['fielddate']))
	$callAJAX=true;

// ...JavaScript/AJAX - Get submitted values
if($callAJAX)
{
	$fname=trim($_GET['fieldname']);
	$fdate=trim($_GET['fielddate']);
	$mark=trim($_GET['mark']);
	$quick=(int)$_GET['quick'];
	$person=(int)$_GET['person'];
}
// ...Included
else
{
	$fname='';
	$quick=false;

	if(!isset($person))
		$person=$GLOBALS['fxpglobals']['person'];
}
$quick=false;

if(isset($GLOBALS['datetime']))
	$today=substr($GLOBALS['datetime'],0,8);
else
	$today=substr(fxNow(),0,8);

// Quick mode without holidays etc.
if($quick)
{
	$dspos=strpos($GLOBALS['fxpglobals']['settings']['formatdatum'], 'm');
	if(!$dspos)
		$dspos=strpos($GLOBALS['fxpglobals']['settings']['formatdatum'], 'd')-1;
	else
		$dspos -= 1;
	$ds =substr($GLOBALS['fxpglobals']['settings']['formatdatum'], $dspos, 1);
	$dfa=explode($ds, $GLOBALS['fxpglobals']['settings']['formatdatum']);
	$dsa=array('0', '0', '0');
	for($di=0; $di<3; $di++)
	{
		$dt=substr($dfa[$di], 0, 1);
		if($dt == 'y')
			$dv=(int)substr($today, 0, 4);
		else if($dt == 'm')
			$dv=(int)substr($today, 4, 2);
		else
			$dv=(int)substr($today, 6, 2);
		$dsa[$di]=substr('0000'.$dv, -strlen($dfa[$di]));
	}

	// Deutsch
	if($GLOBALS['fxpglobals']['lang'] == 1)
	{
		// ...Month Names, Weekday Names, Calendar Week + Headline
		$month_names=array(1=>'Januar', 'Februar', 'März', 'April', 'Mai', 'Juni', 'Juli', 'August', 'September', 'Oktober', 'November', 'Dezember');
		$weekday_names=array(0=>'Sonntag', 'Montag', 'Dienstag', 'Mittwoch', 'Donnerstag', 'Freitag', 'Samstag', 'Sonntag');
		$calendar_week_short='KW';
		$headline='Datumsauswahl';

		$today_literal='Heute: '.$dsa[0].$ds.$dsa[1].$ds.$dsa[2];
		$close_literal='Schließen';
	}
	// English
	else
	{
		// ...Month Names, Weekday Names, Calendar Week + Headline
		$month_names=array(1=>'January', 'February', 'March', 'April', 'May', 'June', 'July', 'August', 'September', 'October', 'November', 'December');
		$weekday_names=array(0=>'Sunday', 'Monday', 'Tuesday', 'Wednesday', 'Thursday', 'Friday', 'Saturday', 'Sunday');
		$calendar_week_short='CW';
		$headline='Date Selection';

		$today_literal='Today: '.$dsa[0].$ds.$dsa[1].$ds.$dsa[2];
		$close_literal='Close';
	}
}
else
{
	// ...Month Names, Weekday Names + Calendar Week
	$month_names=fxf_getReferenceValues('months');
	$weekday_names=fxf_getWeekdayNames(2);
	$calendar_week_short=meldung(1036, true);	// KW

	$today_literal=get_text(540, '20').': '.get_entry($today, 'datum');
	$close_literal=meldung(968, true);	// Schließen

	// ...Headline
	$fid=0;
	if(strlen($fname))
	{
		$ppos=strpos($fname, '[');
		if($ppos)
			$rfname=substr($fname, 0, $ppos);
		else
			$rfname=$fname;
		$fid=(int)db_value("SELECT id FROM felder WHERE ".db_like('feldname', $rfname)." AND mandanten_id IN (0,".$GLOBALS['fxpglobals']['client'].")");
	}
	if($fid)
	{
		$msk="SELECT nummer FROM trans_maske WHERE transakt_id=".(int)$GLOBALS['fxpglobals']['tr'];
		$sql="SELECT anzeigeart FROM maskenfelder WHERE eingabefeld=".$fid." AND nummer IN (".$msk.") AND anzeigeart IN (1,2,3,4,20)";
		$anz=(int)db_value($sql);
		if(!$anz)
			$anz=3;
		$aa=array(1=>'literal_5', 'literal_10', 'literal_15', 'literal_20', 20=>'literal_text');
		$sql="SELECT ".$aa[$anz]." FROM texte WHERE id_referenz=".$fid." AND id_ref_art=3 AND id_sprache=".$GLOBALS['fxpglobals']['lang'];
		$headline=db_value($sql);
	}
	else if($GLOBALS['fxpglobals']['lang'] == 1)
		$headline='Datumsauswahl';
	else
		$headline='Date Selection';
}

if(!strlen($fdate))
	$fdate=$today;

$year=(int)substr($fdate,0,4);
$acty=(int)substr($today,0,4);
if(!$GLOBALS['fxptdata']['ignore_date_boundaries'])
{
	setDateLimits();
	$miny=(int)$GLOBALS['fxpvars']['date_minmax']['min'];
	$maxy=(int)$GLOBALS['fxpvars']['date_minmax']['max'];
	if(!$miny)
		$miny=$acty-100;
	if(!$maxy)
		$maxy=$acty+50;
	$year=max(min($year, $GLOBALS['fxpvars']['date_minmax']['max']), $GLOBALS['fxpvars']['date_minmax']['min']);
}
else
{
	$miny=$acty-100;
	$maxy=$acty+50;
}
$h='<div id="cal_minmax_year_'.$fname.'" name="cal_minmax_year_'.$fname.'" style="display:none;">'.$miny.','.$maxy.'</div>';

$month=(int)substr($fdate, 4, 2);
$day=min((int)substr($fdate, 6, 2), (int)fx_date('d', fx_mktime(12, 0, 0, $month+1, 0, $year)));

$fdate=substr('0000'.$year, -4).substr('00'.$month, -2).substr('00'.$day, -2);
$h .= '<div id="cal_fdate_'.$fname.'" style="display:none;">'.$fdate.'</div>';

$mdays=array(1=>31,28,31,30,31,30,31,31,30,31,30,31);
$start_weekday=fx_date('w', fx_mktime(12, 0, 0, $month, 1, $year));
$first_weekday=(int)$GLOBALS['fxpglobals']['settings']['ersterwt'];
// Start Date
if($start_weekday != $first_weekday)
{
	$day_diff=$first_weekday-$start_weekday;
	if($start_weekday < $first_weekday)
		$day_diff -= 7;
	$start_date=fx_date('Ymd', fx_mktime(12, 0, 0, $month, 1+$day_diff, $year));
	$start_weekday=$first_weekday;
}
else
	$start_date=fx_date('Ymd', fx_mktime(12, 0, 0, $month, 1, $year));
// End Date
$end_date=fx_date('Ymd', fx_mktime(12, 0, 0, $month+1, 0, $year));
//$h .= '$start_weekday='.$start_weekday.', $first_weekday='.$first_weekday.', $start_date='.$start_date.', $end_date='.$end_date.'<hr>';

$pwidth=256;

// Toolbar
$h .= '<div id="cal_pop_div" style="position:relative;width:'.($pwidth+32).'px;white-space:normal;">';

// Navigation Help
if(!$callAJAX)
{
	$nhelp=explode('^', get_message('Navigationshilfe'));
//fxDebug($nhelp, '$nhelp', 0);

	$h .= '<table width="'.($pwidth+28).'" border="0" cellspacing="0" cellpadding="0">';
	$h .= '	<tr>';
	$h .= '		<td class="rhd" valign="top" width="100%" style="height:22px;"><img id="ncimage" src="'.$GLOBALS['gfxpath'].'fl_op.png" tooltip="'.$nhelp[1].'" style="position:absolute;top:4px;right:4px;cursor:help;border:1px solid #c8c8c8;" onclick="$(\'calhelp\').toggle(); if($(\'calhelp\').style.display != \'none\') { $(this).src=\''.$GLOBALS['gfxpath'].'fl_cl.png\'; } else { $(this).src=\''.$GLOBALS['gfxpath'].'fl_op.png\'; }">&nbsp;<font class="s5b darkgrey">'.$nhelp[0].'</font></td>';
	$h .= '	</tr>';
	$h .= '	<tr>';
	$h .= '		<td valign="top" width="100%">';
	$h .= '			<div style="border:1px dotted #cccccc;border-top:0;padding:3px;">';
	$h .= '				<div class="darkgrey" align="justify" style="padding:0;margin:0;font-size:9pt;">';
	$h .= '					'.$nhelp[2];
	$h .= '				</div>';
	$h .= '				<div class="darkgrey" id="calhelp" style="padding:0;margin:0;font-size:9pt;display:none;">';
	$h .= '					<br />'.extract_meldung($nhelp[3]);
	$h .= '				</div>';
	$h .= '			</div><br />';
	$h .= '		</td>';
	$h .= '	</tr>';
	$h .= '</table>';
}

// Today/Month/Year
$c  = '<table class="rhd" width="100%" border="0" cellspacing="0" cellpadding="1" style="padding:6px 1px;border-top-left-radius:9px;border-top-right-radius:9px;">';
$c .= '<tr>';
// ...Today
$c .= '<td align="left" nowrap>';
$c .= '<div id="today" class="ftd" tooltip="'.$today_literal.'" style="left:4px;width:24px;height:20px;cursor:pointer;" '.fxf_jsFunction('changeDay', $fname, (int)substr($today, 0,4), (int)substr($today, 4,2), (int)substr($today, 6,2)).'><font id="todayx" class="s2" tooltip="'.$today_literal.'">x</font><img id="todayi" src="'.$GLOBALS['gfxpath'].'today_trans.png" tooltip="'.$today_literal.'" style="position:absolute;left:1px;top:2px;"></div>';
$c .= '</td>';
// ...Month
$c .= '<td align="center" nowrap><div style="position:relative;left:7px;">';
$pyear	= $year;
$pmonth=$month-1;
$pday	= $day;
if($pmonth < 1)
{
	$pmonth=12;
	$pyear--;
}
$mdays[2]=28;
if(fx_date('L', fx_mktime(0, 0, 0, 1, 1, $pyear)))
	$mdays[2]=29;
$pday=min($pday, $mdays[$pmonth]);
if($pyear < $miny)
	$c .= '<div class="fal" style="float:left;width:12px;height:20px;cursor:default;"><span class="fali ainactive"></span></div>';
else
	$c .= '<div class="fal" style="float:left;width:12px;height:20px;cursor:pointer;" '.fxf_jsFunction('changeMonth', $fname, $pyear, $pmonth, $pday).'><span class="fali aactive"></span></div>';

$md='';
if(strlen($GLOBALS['fxpglobals']['browsermobile']))
	$md=' '.fxf_jsFunction('mouseDown');

selEntriesJS('cal_month_'.$fname, $month_names);
$c .= '<div id="cal_month_'.$fname.'" name="cal_month_'.$fname.'" class="fmfsl" svalue="'.$month.'" style="float:left;width:90px;height:16px;text-align:left;"'.$md.'>'.$month_names[$month].'<div class="fmfslc" style="cursor:pointer;"><span class="fmfsli"></span></div></div>';
$nyear	= $year;
$nmonth=$month+1;
$nday	= $day;
if($nmonth > 12)
{
	$nmonth=1;
	$nyear++;
}
$mdays[2]=28;
if(fx_date('L', fx_mktime(0, 0, 0, 1, 1, $nyear)))
	$mdays[2]=29;
$nday=min($nday, $mdays[$nmonth]);
if($nyear > $maxy)
	$c .= '<div class="far" style="float:left;width:12px;height:20px;cursor:default;"><span class="fari ainactive"></span></div>';
else
	$c .= '<div class="far" style="float:left;width:12px;height:20px;cursor:pointer;" '.fxf_jsFunction('changeMonth', $fname, $nyear, $nmonth, $nday).'><span class="fari"></span></div>';

$c .= '</div></td>';
// ...Year
$vspace=1;
if(!$GLOBALS['fxptdata']['ignore_date_boundaries'])
	$vspace=0;
$c .= '<td align="right" nowrap><div style="position:relative;left:2px;">';
$pyear	= $year-1;
$pmonth=$month;
$pday	= $day;
$mdays[2]=28;
if(fx_date('L', fx_mktime(0, 0, 0, 1, 1, $pyear)))
	$mdays[2]=29;
$pday=min($pday, $mdays[$pmonth]);
if($pyear < $miny)
	$c .= '<div class="fal" style="float:left;width:12px;height:20px;cursor:default;"><span class="fali ainactive"></span></div>';
else
	$c .= '<div class="fal" style="float:left;width:12px;height:20px;cursor:pointer;" '.fxf_jsFunction('changeYear', $fname, $pyear, $pmonth, $pday).'><span class="fali aactive"></span></div>';

if(!$GLOBALS['fxptdata']['ignore_date_boundaries'])
{
	$year_array=array();
	for($y=$miny; $y<=$maxy; $y++)
		$year_array[$y]=$y;
	selEntriesJS('cal_year_'.$fname, $year_array);
	$c .= '<div id="cal_year_'.$fname.'" name="cal_year_'.$fname.'" class="fmfsl" svalue="'.$year.'" style="float:left;width:54px;height:16px;text-align:left;"'.$md.'>'.$year.'<div class="fmfslc" style="cursor:pointer;"><span class="fmfsli"></span></div></div>';
}
else
	$c .= '<input type="text" id="cal_year_'.$fname.'" name="cal_year_'.$fname.'" class="fmftx" value="'.$year.'" maxlength="4" style="float:left;width:54px;height:16px;text-align:center;" '.fxf_jsFunction('changeYear', '*onchange', $fname, 0, $month, $day).'>';
$nyear=$year+1;
$nmonth=$month;
$nday=$day;
$mdays[2]=28;
if(fx_date('L', fx_mktime(0, 0, 0, 1, 1, $nyear)))
	$mdays[2]=29;
$nday=min($nday, $mdays[$pmonth]);
if($nyear > $maxy)
	$c .= '<div class="far" style="float:left;width:12px;height:20px;cursor:default;"><span class="fari ainactive"></span></div>';
else
	$c .= '<div class="far" style="float:left;width:12px;height:20px;cursor:pointer;" '.fxf_jsFunction('changeYear', $fname, $nyear, $nmonth, $nday).'><span class="fari"></span></div>';
$c .= '</div></td>';
$c .= '</tr>';
$c .= '</table>';

// Clickable month and year
if(!$callAJAX)
{
	$onaction=' onmouseover="$(this).className=\'blue\';" onmouseout="$(this).className=\'darkgrey\';"';
	$c .= '<table class="rhd" width="100%" border="0" cellspacing="0" cellpadding="0" style="padding:6px;">';
	$c .= '<tr>';
	$c .= '<td align="center">';
	$c .= '<b class="darkgrey" style="cursor:pointer;"'.$onaction.' '.fxf_jsFunction('changeMonth', $fname, $year, $month, $day).'>'.$month_names[$month].'</b>';
	$c .= '&nbsp;<b class="darkgrey" style="cursor:pointer;"'.$onaction.' '.fxf_jsFunction('changeYear', $fname, $year, $month, $day).'>'.$year.'</b>';
	$c .= '</td>';
	$c .= '</tr>';
	$c .= '</table>';
}

//$c .= '<table width="100%" border="0" cellspacing="2" cellpadding="0" style="border:1px solid #e8e8e8;border-top:0;border-bottom-left-radius:9px;border-bottom-right-radius:9px;">';
$c .= '<table class="bshadowhd" width="100%" border="0" cellspacing="3" cellpadding="0" style="border:1px solid #e8e8e8;border-top:0;border-bottom-left-radius:9px;border-bottom-right-radius:9px;">';

// Calendar Headline
$c .= '<tr>';
for($x=0; $x<8; $x++)
{
	if(!$x)	// CW
	{
		$val=$calendar_week_short;
		$height='height:18px;';
		$col='79aec7';
	}
	else	// Weekday Name
	{
		$weekday=$first_weekday+$x-1;
		if($weekday > 6)
			$weekday -= 7;
		$val=$weekday_names[$weekday];
		$height='';
		$col='006b9f';
		if(($weekday < 1) || ($weekday > 5))	// Weekend
			$col='3d8cb3';
	}
	$c .= '<td align="center" style="padding-top:2px;background:#f1f1f1;color:#'.$col.';width:29px;border-right:1px solid #888888;border-bottom:1px solid #666666;'.$height.'"><b>'.$val.'</b></td>';
}
$c .= '</tr>';

// Entry + Exit Dates, Workdays, Holidays + Special days
$entry_date			= '00000000';
$exit_date			= '99999999';
$pers_workdays_arr	= array();
$holidays_arr		= array();
$specialdays_arr	= array();
if(!$quick)
{
	if(!$person)
		$person=$GLOBALS['fxpglobals']['person'];

	// ...Entry + Exit Dates
	$sql="SELECT maeintrittsdatum, maaustrittsdatum FROM madaten WHERE personen_id=".$person." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$tmp=db_values($sql);
	if(fxIsArray($tmp))
	{
		if(strlen($tmp['maeintrittsdatum']))
			$entry_date=substr($tmp['maeintrittsdatum'],0,8);
		if(strlen($tmp['maaustrittsdatum']))
			$exit_date=substr($tmp['maaustrittsdatum'],0,8);
	}

	// ...Workdays
	$sql  = "SELECT sonntag AS a, montag AS b, dienstag AS c, mittwoch AS d, donnerstag AS e, freitag AS f, samstag AS g";
	$sql .= " FROM madaten WHERE personen_id=".$person." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$pers_workdays_arr=db_values($sql);

	// ...Holidays
	$start_date_h=$start_date;
	$end_date_h=fx_date('Ymd', fx_mktime(12, 0, 0, $month+2, 0, $year));
	$holidays_arr=get_feiertage($start_date_h, $end_date_h);
	$holidays_type=array(FXP_HL_FULL=>' [1]', FXP_HL_HALF=>' [½]', FXP_HL_DISPLAY=>' [0]');

	// ...Special days (Vacation/Training/Sick/Flextime)
	$specialdays_arr=get_sondertage($start_date_h, $end_date_h, $person);
	$specialdays_type=get_uskg_array();
}
//echo('$entry_date: <b>'.$entry_date.'</b><br />$exit_date: <b>'.$exit_date.'</b><hr />');

// Calendar
$act_date=$start_date;
$l=0;
while(($act_date <= $end_date) && ($l < 6))
{
	// Calendar Week
	$act_year			= (int)substr($act_date, 0, 4);
	$act_month			= (int)substr($act_date, 4, 2);
	$act_day			= (int)substr($act_date, 6, 2);
	$act_calendarweek	= kw($act_date);
	$c .= '<tr>';
	$onaction='';
	$cursor='';
	if(!$callAJAX)
	{
		$onaction=' onmouseover="$(this).style.backgroundColor=\'#c3c3c3\';" onmouseout="$(this).style.backgroundColor=\'#e1e1e1\';" '.fxf_jsFunction('changeWeek', $fname, $act_year, $act_month, $act_day, $act_calendarweek);
		$cursor=' cursor:pointer;';
	}
	$nxt_cw_startday=fx_date('Ymd', fx_mktime(12, 0, 0, $act_month, $act_day+7, $act_year));
	$sbr='';
	if((int)substr($nxt_cw_startday,4,2) > $month)
		$sbr='border-bottom-left-radius:9px;';
	$c .= '<td bgcolor="#e1e1e1" align="center" style="color:#5a5a5a;border-left:1px solid #f1f1f1;border-right:1px solid #a0a0a0;border-bottom:1px solid #a0a0a0;'.$sbr.'height:18px;'.$cursor.'"'.$onaction.'><b>'.$act_calendarweek.'</b></td>';

	// Calendar Days
	for($d=0; $d<7; $d++)
	{
		$act_year	= (int)substr($act_date, 0, 4);
		$act_month	= (int)substr($act_date, 4, 2);
		$act_day	= (int)substr($act_date, 6, 2);

		$weekday=$first_weekday+$d;
		if($weekday > 6)
			$weekday -= 7;

		if(($act_date < $entry_date) || ($act_date > $exit_date))
			$workday=-1;
		else if(is_array($pers_workdays_arr) && sizeof($pers_workdays_arr))
			$workday=(int)$pers_workdays_arr[chr(97+$start_weekday)];
		else if(!$weekday || ($weekday > 5))
			$workday=0;
		else
			$workday=1;

		// Standardfarben für Wochentage bzw. Wochenenden
		$onmouse='';
		$title='';
		$sta='';
		$brdrad='border-radius:6px;';
		$brdcol='ccccdd';
//echo('$act_date: <b>'.$act_date.'</b>, $weekday: <b>'.$weekday.'</b>, $workday: </b>'.$workday.'</b><br />');

		// Color: Outside valid dates
		if($workday < 0)
		{
			$bg='bgcolor="#e43d50"';
			$brdrad='border-radius:3px;';
			$brdcol='bf000c';
			$onmouse=' onmouseover="$(this).style.backgroundColor=\'#e1001a\';" onmouseout="$(this).style.backgroundColor=\'#e43d50\';"';
		}
		// Color: Real holidays
		else if(sizeof($holidays_arr) && is_array($holidays_arr[$act_date]) && ($holidays_arr[$act_date]['typ'] > 0.00))
		{
			$bg='bgcolor="#ffff44"';
			$onmouse=' onmouseover="$(this).style.backgroundColor=\'#dddd22\';" onmouseout="$(this).style.backgroundColor=\'#ffff44\';"';
			$brdcol='bbbb00';
			$title=' title="'.$holidays_arr[$act_date]['name'].$holidays_type[$holidays_arr[$act_date]['art']].'"';
		}
		// Color: No Workday or Weekend
		else if(!$workday)
		{
			$bg='bgcolor="#dddddd"';
			$onmouse=' onmouseover="$(this).style.backgroundColor=\'#bbbbbb\';" onmouseout="$(this).style.backgroundColor=\'#dddddd\';"';
			$brdcol='999999';
		}
		// Color: Special days
		else if(sizeof($specialdays_arr) && is_array($specialdays_arr[$act_date]))
		{
			$type=(int)$specialdays_arr[$act_date][-1]['typ'];
			if($type == FXP_AT_ABSENT)
			{
				$bg='bgcolor="#bbbbbb"';
				$onmouse=' onmouseover="$(this).style.backgroundColor=\'#999999\';" onmouseout="$(this).style.backgroundColor=\'#bbbbbb\';"';
				$brdcol='777777';
			}
			else if(($type == FXP_AT_VACATIONREQUEST) || ($type == FXP_AT_VACATION))
			{
				$bg='bgcolor="#88ff88"';
				$onmouse=' onmouseover="$(this).style.backgroundColor=\'#66dd66\';" onmouseout="$(this).style.backgroundColor=\'#88ff88\';"';
				$brdcol='44bb44';
			}
			else if(($type == FXP_AT_SPECIALVACATIONREQUEST) || ($type == FXP_AT_SPECIALVACATION))
			{
				$bg='bgcolor="#88ffbb"';
				$onmouse=' onmouseover="$(this).style.backgroundColor=\'#66dd99\';" onmouseout="$(this).style.backgroundColor=\'#88ffbb\';"';
				$brdcol='44bb77';
			}
			else if(($type == FXP_AT_TRAININGREQUEST) || ($type == FXP_AT_TRAINING))
			{
				$bg='bgcolor="#79aec7"';
				$onmouse=' onmouseover="$(this).style.backgroundColor=\'#578ca5\';" onmouseout="$(this).style.backgroundColor=\'#79aec7\';"';
				$brdcol='356a83';
			}
			else if(($type == FXP_AT_SICKCALL) || ($type == FXP_AT_SICK))
			{
				$bg='bgcolor="#ff8888"';
				$onmouse=' onmouseover="$(this).style.backgroundColor=\'#dd6666\';" onmouseout="$(this).style.backgroundColor=\'#ff8888\';"';
				$brdcol='bb4444';
			}
			else
			{
				$bg='bgcolor="#bbaacc"';
				$onmouse=' onmouseover="$(this).style.backgroundColor=\'#9988aa\';" onmouseout="$(this).style.backgroundColor=\'#bbaacc\';"';
				$brdcol='776688';
			}
			$title=' title="'.$specialdays_type[$type].'"';
			$sta='box-shadow:1px 1px 2px #ffffff inset,-1px -1px 2px #222233 inset;';
		}
		// Color: Just display holidays
		else if(sizeof($holidays_arr) && is_array($holidays_arr[$act_date]) && ($holidays_arr[$act_date]['typ'] == 0.00))
		{
			$bg='bgcolor="#ffffaa"';
			$brdrad='border-radius:9px;';
			$onmouse=' onmouseover="$(this).style.backgroundColor=\'#dddd88\';" onmouseout="$(this).style.backgroundColor=\'#ffffaa\';"';
			$brdcol='bbbb66';
			$title=' title="'.$holidays_arr[$act_date]['name'].$holidays_type[$holidays_arr[$act_date]['art']].'"';
		}
		// Color: Workday
		else
		{
			$bg='bgcolor="#ffffff"';
			$brdrad='border-radius:9px;';
			$onmouse=' onmouseover="$(this).style.backgroundColor=\'#dddddd\';" onmouseout="$(this).style.backgroundColor=\'#ffffff\';"';
			$brdcol='bbbbbb';
		}
		// Color: Mark
		if(strlen($mark) && ($act_date == $mark))
			$sta='box-shadow:3px 3px 3px rgba(0,0,0, 0.75) inset;';

		// Today
		$img='';
		if($act_date == $today)
			$img='<span style="position:relative;"><img src="'.$GLOBALS['gfxpath'].'today_trans.png" style="position:absolute;width:42px;height:30px;left:-28px;top:-7px;"></span>';

		if($workday < 0)
			$font_style='color:#6f000c;';
		else
			$font_style='color:#3b3b3b;';

		if($act_month != $month)
		{
			$font_style .= 'opacity:0.33;';
			$act_attr_s='<i>';
			$act_attr_e='</i>';
		}
		else
		{
			$act_attr_s='';
			$act_attr_e='';
		}

		$brd='border-right:1px solid #'.$brdcol.';border-bottom:1px solid #'.$brdcol.';';
		$c .= '<td '.$bg.' align="center" style="'.$sta.$brd.$brdrad.'cursor:pointer;"'.$onmouse.$title.' '.fxf_jsFunction('changeDay', $fname, $act_year, $act_month, $act_day).'>'.$act_attr_s.'<font style="'.$font_style.'">'.$act_day.'</font>'.$act_attr_e.$img.'</td>';

		$start_weekday++;
		if($start_weekday > 6)
			$start_weekday=0;

		$act_date=fx_date('Ymd', fx_mktime(12, 0, 0, $act_month, $act_day+1, $act_year));
//$c .= '$act_date='.$act_date.'<br />');
	}
	$c .= '</tr>';

	$l++;
}
$c .= '</table>';

if($callAJAX)
	$c=popupMask($headline, '', $c, '', $pwidth+16, -1, false, false, 0, 0, 'cal_'.$fname);
else
	$c='<div class="maskc" style="position:absolute;width:'.$pwidth.'px;padding:6px;">'.$c.'</div><div style="position:relative;width:1px;height:192px;"></div>';

$h .= $c.'</div>';
//$h .= '<hr>$start_date='.$start_date.', $end_date='.$end_date.', $act_date='.$act_date.'<br />');

echo($h);
?>