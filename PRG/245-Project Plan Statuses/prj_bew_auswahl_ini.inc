<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : prj_bew_auswahl_ini.inc                                      //
// Version     : 21.1                                                         //
// Begin       : 2021-01-14                                                   //
// Last Change : 2021-01-14                                                   //
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
 * PF 245: Project Plan Statuses
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.1
 */

// Do we have ChartDirector?
$req_fn=fxf_fn_reqFilename('ext_chartdir');
if(strlen($req_fn))
	require($req_fn);

$pid=0;
// Validierung
if($_POST['Button_Anzeigen'] || $_POST['Button_Multi'])
{
	$pid=$_POST['Projekt_ID'];
	if(!$pid)
	{
		error_msg(789, -1, 'Projekt_ID');	// Im Feld $var1 muss ein Eintrag ausgewählt werden!
		$err=true;
	}
	if(!$err && $_POST['Button_Anzeigen'])
		$GLOBALS['fxptdata']['backup']=array('Projekt_ID'=>$_POST['Projekt_ID'], 'Projekt_HistoryID'=>$_POST['Projekt_HistoryID'], 'ausfuehrlich'=>$_POST['ausfuehrlich'], 'bild'=>$_POST['bild']);
}
else if(is_array($GLOBALS['fxptdata']['backup']))
	$_POST=$GLOBALS['fxptdata']['backup'];

// Welche Maske soll eingeblendet werden
if($_POST['Button_Anzeigen'] || $_POST['Button_Multi'])
{
	$show_preport=true;
	$show_pliste=true;

	if($GLOBALS['fxpglobals']['action'] == FXP_DISPLAY)
		$show_pliste=false;
	else
		$show_preport=false;

	if($show_preport && $_POST['Button_Anzeigen'] && $err)
		$show_preport=false;
	if($show_pliste && $_POST['Button_Anzeigen'] && $err)
		$show_pliste=false;
}
else
{
	$show_preport=false;
	$show_pliste=false;
}
?>