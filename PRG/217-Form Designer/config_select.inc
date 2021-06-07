<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : config_select.inc                                            //
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
 * PF 217: Form Designer - Configuration Select Changed
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
if(fxIsArray($_GET) && isset($_GET['ktyp']))
	$callAJAX=true;

$get_array=array('imid'=>$GLOBALS['fxpglobals']['client'], 'iktyp'=>2022);
foreach($get_array as $get_var => $get_default)
{
	$get_type=substr($get_var, 0, 1);
	$get_var=substr($get_var, 1);
	if(fxIsArray($_GET) && isset($_GET[$get_var]))
	{
		if($get_type == 'i')
			${$get_var}=(int)$_GET[$get_var];
		else if($get_type == 'd')
			${$get_var}=(double)$_GET[$get_var];
		else
			${$get_var}=$_GET[$get_var];
	}
	else if(!isset(${$get_var}))
		${$get_var}=$get_default;
//echo('<b>$'.$get_var.'</b>=['.${$get_var}.']<br>');
}

// Auswahlarray entsprechend des Konfigurationstyps füllen
$neu='['.meldung(564, false).']';
$rarr=array();
if(($ktyp == 2019) || ($ktyp == 2021))	// 2019=Leistungsnachweis oder 2021=Rechnung
{
	if($ktyp == 2019)
	{
		$cname='cfg_plnw.cfg';
		$pnarr=select_pdfconfig(1, $mid, true);
	}
	else
	{
		$cname='cfg_prng.cfg';
		$pnarr=select_pdfconfig(2, $mid, true);
	}
//fxDebug($pnarr, $cname, 0);
	$cadd=1;
	if(fxIsArray($pnarr))
	{
		$cadd=sizeof($pnarr);
		$rarr=$pnarr;
	}
	while(true)
	{
		$newrf=$GLOBALS['datpath'].$mid.'/'.substr($cname, 0, -4).substr('000'.$cadd, -3).substr($cname, -4);
		if(file_exists($newrf))
			$cadd++;
		else
			break;
	}
	$rarr['_'.$newrf]=$neu;
}
else
{
	$karr=array(2018=>'cfg_email.cfg', 2819=>'cfg_prkr.cfg', 2020=>'cfg_ppst.cfg', 2022=>'cfg_pstd.cfg');
	$cname=$karr[$ktyp];
	$cpath=$GLOBALS['datpath'].$mid.'/'.$cname;
	if(!file_exists($cpath))
		$rarr['_'.$cpath]=$neu;
	else
		$rarr[$cpath]=$mid.': '.$cname;
}
//fxDebug($rarr, '$rarr', 0);

// Ergebnis im AJAX-Fall am Bildschirm ausgeben
$kdatei_ds=0;
if($callAJAX)
{
	fxSession('s','l');
	$GLOBALS['fxpselects']['kdatei']=array();
	if(fxIsArray($rarr))
	{
		$conv_array=fxReplaceSpecialCharacters(false, false);
		$cnt=0;
		foreach($rarr as $key => $value)
		{
			$GLOBALS['fxpselects']['kdatei'][$key]=$value;
			if(!$cnt)
				$kdatei_ds=$key;
		}
	}
	fxSession('s','s');

	echo($kdatei_ds);
}
else
	$kdatei_array=$rarr;
?>