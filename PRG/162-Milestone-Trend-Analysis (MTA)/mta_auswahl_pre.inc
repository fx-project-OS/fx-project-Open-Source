<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : mta_auswahl_pre.inc                                          //
// Version     : 24.1                                                         //
// Begin       : 2021-01-13                                                   //
// Last Change : 2021-10-06                                                   //
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
 * PF 162: Milestone-Trend-Analysis (MTA)
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

$maske372_sf=true;
// Kalenderpopup-Icons anzeigen
${'cpo'.$tr}=true;

// Wiedervorbelegen der eingengebenen Werte
if($GLOBALS['fxptdata']['backup'])
{
	// Name Vorgang
	$maskenfeldwerte['projekt_id']		= $GLOBALS['fxptdata']['backup']['Projekt_ID'];

	// Zeitspanne von / bis
	$maskenfeldwerte['zeitspanne_von']	= validate_entry($GLOBALS['fxptdata']['backup']['Zeitspanne_von'], $errv, 'datum');
	$maskenfeldwerte['zeitspanne_bis']	= validate_entry($GLOBALS['fxptdata']['backup']['Zeitspanne_bis'], $errv, 'datum');

	// Unterproj. anz.
	$maskenfeldwerte['unterprojekte']	= $GLOBALS['fxptdata']['backup']['Unterprojekte'];
	// Tabellenansicht
	$maskenfeldwerte['tabelle']			= $GLOBALS['fxptdata']['backup']['Tabelle'];

	// Genauigkeit
	$maskenfeldwerte['checkpunkte']		= $GLOBALS['fxptdata']['backup']['Checkpunkte'];
	// Größe
	$maskenfeldwerte['dateigroesse']	= $GLOBALS['fxptdata']['backup']['dateigroesse'];
}

// Feld: Projektname
$sql_addon="";
if($GLOBALS['fxpglobals']['tr'] == 162)
	$sql_addon="AND p.projekt_id IN (SELECT projekt_id FROM meilensteine WHERE mandanten_id=".$GLOBALS['fxpglobals']['client'].")";
$Projekt_ID_arg=array('build'=>true, 'use_actual'=>false, 'with_all'=>false, 'with_projects'=>true, 'sql_addon'=>$sql_addon);
$Proj_Arr=auswahl_Projekt_ID(1);
$Projekt_ID_arg=array('override'=>$Proj_Arr);
$Projekt_ID_ds=0;
?>