<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : madaten_val.inc                                              //
// Version     : 21.2                                                         //
// Begin       : 2020-10-20                                                   //
// Last Change : 2021-04-01                                                   //
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
 * PF 7: Employee's Contract Data
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

// Alte Werte einlesen
$sql="SELECT * FROM madaten WHERE personen_id=".$Personen_ID." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
$odbarr=db_values($sql);
if(!strlen(trim($odbarr['zeitbeschraenkung'])))
{
	if($_POST['pt'] == FXP_PT_EMPLOYEE)
		$odbarr['zeitbeschraenkung']=1;
	else
		$odbarr['zeitbeschraenkung']=0;
}
$odbarr['ek']=get_double($_POST['aek']);
$odbarr['vk']=get_double($_POST['avk']);
$odbarr['urlaubsanspruch']=get_double($_POST['ua']);
$odbarr['resturlaub']=get_double($_POST['uu']);
foreach($odbarr as $cname => $cvalue)
{
	if(is_null($cvalue))
		$odbarr[$cname]='NULL';
	else if(is_string($cvalue))
		$odbarr[$cname]=db_convert_string($cvalue);
}
fxDebug($odbarr,'$odbarr',3);

$ndbarr=array();
$udbarr=array();

// Checkboxen (+ Tagesanzahl)
$dm=false;
$dc=0;
foreach($cbarr as $cbname)
{
	$cblname=strtolower($cbname);
	if($cblname == 'montag')
		$dm=true;

	if(isset($_POST[$cbname]) && $_POST[$cbname])
	{
		$maskenfeldwerte[$cblname]=1;
		if($dm)
			$dc++;
	}
	else
		$maskenfeldwerte[$cblname]=0;
	$ndbarr[$cblname]=$maskenfeldwerte[$cblname];

	if($odbarr[$cbname])
		$odbarr[$cbname]=1;
	else
		$odbarr[$cbname]=0;
}
$ndbarr['arbeitstage']=$dc;

// Datumsfelder
$dterr=false;
foreach($dtarr as $dtname)
{
	$dtlname=strtolower($dtname);
	$muss='0';
	if($dzlname == 'maeintrittsdatum')
		$muss='1';
	$maskenfeldwerte[$dtlname]=validate_entry($_POST[$dtname], $dterr, 'datum', $muss);
fxDebug($maskenfeldwerte[$dtlname],$dtlname.': value='.$_POST[$dtname], 3);
	if($dterr)
	{
		$verr=true;
		error_msg($dterr, false, $dtname);
	}
	else
		$ndbarr[$dtlname]=db_convert_string($maskenfeldwerte[$dtlname]);
}

// Dezimalfelder
$dzerr=false;
foreach($dzarr as $dzname)
{
	$dzlname=strtolower($dzname);
	$muss='3';
	if($dzlname == 'wochenarbeitstd')
		$muss='1';
	else if(($dzlname == 'resturlaub') || ($dzlname == 'uu'))
		$muss='0';
	$maskenfeldwerte[$dzlname]=validate_entry($_POST[$dzname], $dzerr, 'dezimal', $muss);
fxDebug($maskenfeldwerte[$dzlname],$dzlname.': value='.$_POST[$dzname], 3);
	if($dzerr)
	{
		$verr=true;
		error_msg($dzerr, false, $dzname);
	}
	else
		$ndbarr[$dzlname]=get_double($maskenfeldwerte[$dzlname]);
}

// Zeitfelder
$zterr=false;
foreach($ztarr as $ztname)
{
	$ztlname=strtolower($ztname);
	$muss='0';
	$maskenfeldwerte[$ztlname]=validate_entry($_POST[$ztname], $zterr, 'zeit', $muss);
fxDebug($maskenfeldwerte[$ztlname],$ztlname.': value='.$_POST[$ztname], 3);
	if($zterr)
	{
		$verr=true;
		error_msg($zterr, false, $ztname);
	}
	else if($ztlname == 'arbeitsbeginn')
		$ndbarr['soll_beginn']=db_convert_string($maskenfeldwerte[$ztlname]);
	else
		$ndbarr[$ztlname]=db_convert_string($maskenfeldwerte[$ztlname]);
}

// Zeitspannenfelder
$zserr=false;
foreach($zsarr as $zsname)
{
	$zslname=strtolower($zsname);
	$muss='0';
	$maskenfeldwerte[$zslname]=validate_entry($_POST[$zsname], $zserr, 'zeitspanne', $muss);
fxDebug($maskenfeldwerte[$zslname],$zslname.': value='.$_POST[$zsname], 3);
	if($zserr)
	{
		$verr=true;
		error_msg($zserr, false, $zsname);
	}
	else
		$ndbarr[$zslname]=(int)$maskenfeldwerte[$zslname];
}

// Auswahlfelder
foreach($afarr as $afname)
{
	$aflname=strtolower($afname);
	$maskenfeldwerte[$aflname]=(int)$_POST[$afname];
	$ndbarr[$aflname]=(int)$maskenfeldwerte[$aflname];
}

// Textfelder
foreach($txarr as $txname)
{
	$txlname=strtolower($txname);
	$maskenfeldwerte[$txlname]=trim($_POST[$txname]);
	if($txlname == 'anmerkungintern')
		$ndbarr['anmerkung']=db_convert_string($maskenfeldwerte[$txlname]);
	else
		$ndbarr[$txlname]=db_convert_string($maskenfeldwerte[$txlname]);
}
fxDebug($ndbarr,'$ndbarr', 3);

// Arbeitsstunden/Tag (+ Pause)
$ast=0.0;
if($ndbarr['arbeitstage'] > 0)
	$ast=round($ndbarr['wochenarbeitstd']*100.0/$ndbarr['arbeitstage'])/100.0;
$asp=round(($ast+$ndbarr['pause']/3600)*100.0)/100.0;
//echo('$ast='.$ast.', $asp='.$asp.'<hr />');

// Validierungen?
// ...Pause > 6 Std.  oder  Arbeitsstunden/Tag mit Pause > 24 Std.
if(($ndbarr['pause'] > 21600) || ($asp > 24.0))
{
	error_msg(1439, false, 'Pause');	// Die Werte, die Sie im Feld "$var1" angegeben haben, sind unlogisch. Bitte korrigieren Sie ihre Eingabe.
	$verr=true;
}
// ...Jahresurlaub > 365 Tage
if($ndbarr['urlaubsanspruch'] > 365.0)
{
	error_msg(100, false, 'Urlaubsanspruch');	// Ein Mitarbeiter kann nicht mehr als 365 Urlaubstage pro Jahr haben!
	$verr=true;
}
// ...Arbeitsstunden/Woche > 168 Std.  oder  Arbeitsstunden/Tag (mit Pause) > 24 Std.
if(($ndbarr['wochenarbeitstd'] > 168.0) || ($asp > 24.0))
{
	error_msg(1439, false, 'WochenArbeitStd');	// Die Werte, die Sie im Feld "$var1" angegeben haben, sind unlogisch. Bitte korrigieren Sie ihre Eingabe.
	$verr=true;
}
else if(($_POST['pt'] == FXP_PT_EMPLOYEE) && $ndbarr['zeitbeschraenkung'])
{
	// ...Arbeitsstunden/Woche > Mandantenwert
	if($ndbarr['wochenarbeitstd'] > $GLOBALS['fxpglobals']['settings']['maxstd_woche'])
	{
		// Ein Mitarbeiter darf pro Woche maximal $var1 Stunden arbeiten.
		error_msg(2439, array('WochenArbeitStd'), '<b>'.get_entry($GLOBALS['fxpglobals']['settings']['maxstd_woche'], 'dezimal').'</b>');
		$verr=true;
	}
	// ...Arbeitsstunden/Tag > Mandantenwert
	if($ast > $GLOBALS['fxpglobals']['settings']['maxstd_tag'])
	{
		// Ein Mitarbeiter darf pro Tag maximal $var1 Stunden arbeiten. Dieser Mitarbeiter müsste aber nach Ihren Angaben täglich $var2 Stunden leisten.
		error_msg(2438,array('WochenArbeitStd'), '<b>'.get_entry($GLOBALS['fxpglobals']['settings']['maxstd_tag'], 'dezimal').'</b>', '<b class=red>'.get_entry($ast, 'dezimal').'</b>');
		$verr=true;
	}
}

// Update ausführen?
if(!$verr)
{
	foreach($ndbarr as $cname => $cvalue)
	{
		if(isset($odbarr[$cname]) && ($cvalue != $odbarr[$cname]))
			$udbarr[$cname]=$cvalue;
	}
	// ...falls "EK" und/oder "VK" verändert wurde, dann immer beide updaten
	if(isset($udbarr['ek']) || isset($udbarr['vk']))
	{
		if(!isset($udbarr['ek']))
			$udbarr['ek']=$ndbarr['ek'];
		if(!isset($udbarr['vk']))
			$udbarr['vk']=$ndbarr['vk'];
	}
	// ...falls "Urlaubsanspruch" und/oder "Resturlaub" verändert wurde, dann immer beide updaten
	if(isset($udbarr['urlaubsanspruch']) || isset($udbarr['resturlaub']))
	{
		if(!isset($udbarr['urlaubsanspruch']))
			$udbarr['urlaubsanspruch']=$ndbarr['urlaubsanspruch'];
		if(!isset($udbarr['resturlaub']))
			$udbarr['resturlaub']=$ndbarr['resturlaub'];
	}
fxDebug($udbarr,'$udbarr', 2);
}

//$verr=true; $kerr=true;
if($verr || $kerr)
	$err=true;
?>