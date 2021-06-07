<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : projekt_check_auswahl_pre.inc                                //
// Version     : 21.1                                                         //
// Begin       : 2020-12-14                                                   //
// Last Change : 2020-12-14                                                   //
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
 * @version 21.1
 */

$Projekt_ID_ds=1884;
$Projekt_ID_arg=array('mp_only'=>true);	// Nur Mutterprojekte in Auswahlmaske
$mpa=auswahl_Projekt_ID(1);
$Projekt_ID_arg['override']=$mpa;
//fxDebug($mpa,'$mpa', 0);

if(($mpid < 0) && $GLOBALS['fxpglobals']['project'])
	$mpid=(int)db_value("SELECT mutterprojekt_id FROM projekte WHERE projekt_id=".$GLOBALS['fxpglobals']['project']." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
$maskenfeldwerte['projekt_id']=$mpid;

// Auswahlfeld: Sortierfolge [sortierreihe]
$sortierreihe_ds=0;
$sortierreihe_array=explode('|',meldung(3167,false));
$so=0;
if($_POST['sortierreihe'])
	$so=1;
$maskenfeldwerte['sortierreihe']=$so;

// Checkbox: Autom. fortlaufende Nummerierung [autovn]
if($_POST['autovn'])
	$maskenfeldwerte['autovn']=1;

// Checkboxen: Zeitdaten [Zeitdaten], Zeitaufwand [Zeitaufwand], Int. Budgets [budget_ges_int], Ext. Budgets [budget_ges_ext] + Abgerechnet [rechnung_ext]
foreach($cat_array as $cat_var => $cat_bit)
{
	if($_POST[$cat_var])
		$maskenfeldwerte[strtolower($cat_var)]=1;
}

// Checkbox: Historie [historie]
if($_POST['historie'])
	$maskenfeldwerte['historie']=1;
?>