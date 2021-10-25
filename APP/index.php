<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : index.php                                                    //
// Version     : 21.2                                                         //
// Begin       : 2021-03-18                                                   //
// Last Change : 2021-03-18                                                   //
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
 * PF 21: Time Recording - App - Main start program to check if the APP is avalable
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

$GLOBALS['__loaded_'.basename(__FILE__)]=true;


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

// Do we have the App?
if(isset($GLOBALS['prgdir'][21]) || !isset($GLOBALS['sppdir'][21]) || !function_exists('fxsppCheckEBTR') || !fxsppCheckEBTR(21) || !file_exists($GLOBALS['sppdir'][21].'/021_ini.inc'))
{
	if($GLOBALS['_elinks'])
	{
		$mlas='a href="https://www.fx-project.org" target="_blank"';
		$mlae='a';
	}
	else
	{
		$mlas='span';
		$mlae='span';
	}

	$_title='Notice';
	$_message='>The time recording App has not been enabled in this system.|The App can be purchased from the manufacturer<br />of <'.$mlas.' style="font-size:18pt;font-weight:bold;color:#fff;text-decoration:underline;text-shadow:2px 2px 2px rgba(0,0,0, 0.66);">fx-project</'.$mlae.'> as an extension package.';
	if($GLOBALS['fxpglobals']['lang'] == 1)	// Deutsch
	{
		$_title='Hinweis';
		$_message='>Die Zeiterfassungs-App wurde in diesem System nicht freigeschaltet.|Die App kann beim Hersteller von <'.$mlas.' style="font-size:18pt;font-weight:bold;color:#fff;text-decoration:underline;text-shadow:2px 2px 2px rgba(0,0,0, 0.66);">fx-project</'.$mlae.'><br />als Erweiterungspaket erworben werden.';
	}

	require($GLOBALS['mainpath'].'error.inc');
	die;
}

// Call the App
//die($GLOBALS['sppdir'][21].'/021_ini.inc');
require($GLOBALS['sppdir'][21].'/021_ini.inc');
?>