<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : design.inc                                                   //
// Version     : 21.1                                                         //
// Begin       : 2020-08-20                                                   //
// Last Change : 2020-09-10                                                   //
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
 * Changes designs and/or sets menu colors
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

// Client
$c=0;
if(is_array($_GET) && isset($_GET['c']))
	$c=(int)$_GET['c'];

// Person
$p=0;
if(is_array($_GET) && isset($_GET['p']))
	$p=(int)$_GET['p'];

// User
$u=0;
if(is_array($_GET) && isset($_GET['u']))
	$u=(int)$_GET['u'];

// Menu length
$m=0;
if(is_array($_GET) && isset($_GET['m']))
	$m=(int)$_GET['m'];

// Design
$style='';
if(is_array($_GET) && isset($_GET['d']))
{
	$GLOBALS['fxpglobals']['design']='';
	$d=trim($_GET['d']);
	if(strlen($d))
	{
		$ucf=fxpDesignPath($d,$c,$u);
    	if(strlen($ucf))
    	{
			$style=fxLoad($ucf);
			$GLOBALS['fxpglobals']['design']=$d;
		}
		else
			$d='';
	}
	fxSession('g','s');

	// Save design for person login
	if($c > 0)
		tSaveDesign($d, $c,$p);
}

// Menu colors
$cstyle=fxpCSSDesign(true,false,$style);
$dsa=fxpClientDesign($cstyle,$m);
$m0col=$dsa['cmenu1-i']['background'];
$m1col=array();
$c=1;
if(fxIsArray($dsa))
{
	while(fxIsArray($dsa['cmenu-c'.$c]))
	{
		if(isset($dsa['cmenu-c'.$c]['background']) && strlen($dsa['cmenu-c'.$c]['background']) && ($dsa['cmenu-c'.$c]['background'] != 'transparent'))
			$m1col[]=$dsa['cmenu-c'.$c]['background'];
		$c++;
	}
}
$mca=getMenuColors($m, $m0col, $m1col);
$style .= '/* Menu colors */'.$GLOBALS['nl'];
$style .= getMenuColorsCSS($m, $mca);

echo($style);
?>