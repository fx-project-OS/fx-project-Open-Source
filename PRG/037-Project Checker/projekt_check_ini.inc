<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : projekt_check_ini.inc                                        //
// Version     : 24.1                                                         //
// Begin       : 2020-12-14                                                   //
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
 * 37: Project Checker
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

//$swritelog = '_vorgaenge_ueberpruefen';

// Funktionen einbinden
$req_fn=fxf_fn_reqFilename('project_check_functions', 37);
if(strlen((string)$req_fn))
	require($req_fn);

// Checkbox-Kategorien
$cat_array=array('Zeitdaten'=>1, 'Zeitaufwand'=>2, 'budget_ges_int'=>4, 'budget_ges_ext'=>8, 'rechnung_ext'=>16);
$cat_bits=0;

// Mutterprojekt-ID
$mpid=-1;
if($GLOBALS['gproject'])
{
	$mpid=$GLOBALS['gproject'];
	foreach($cat_array as $cat_var => $cat_bit)
		$_POST[$cat_var]=1;
}
else if(($_GET['uid'] == 'newtr') && $GLOBALS['fxpglobals']['project'])
	$mpid=$GLOBALS['fxpglobals']['project'];
else if(fxIsArray($_POST) && isset($_POST['Projekt_ID']))
	$mpid=$_POST['Projekt_ID'];
if($mpid > 0)
{
	$pt=(int)db_value("SELECT projektart FROM projekte WHERE projekt_id=".$mpid." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
	if($pt != FXP_PRJ_MAIN)
		$mpid=-1;
}
//fxDebug($mpid,'$mpid', 0);

// Liste anzeigen
$show_list=false;
if(($mpid >= 0) && ($_POST['Button_Select'] || $_POST['Button_Anpassen']))
{
	// Prüfen, wieviele Kategorien überprüft werden sollen
	$cat_count=0;
	foreach($cat_array as $cat_var => $cat_bit)
	{
		if($_POST[$cat_var])
		{
			$cat_count++;
			$cat_bits |= $cat_bit;
		}
	}
//echo('$cat_bits='.$cat_bits.', $cat_count='.$cat_count.'<br>'.$nl);

	if(!$cat_count && !$_POST['autovn'])
	{
		// 1646: Es wurde keine notwendige Checkbox markiert!
		error_msg(1646, false);
		$err=true;
	}
	else
		$show_list=true;
}
?>