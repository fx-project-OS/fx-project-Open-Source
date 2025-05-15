<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : terminsuche_pre.inc                                          //
// Version     : 24.1                                                         //
// Begin       : 2020-12-22                                                   //
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
 * PF 32: Appointment Search
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

$maske143_sf=true;
$cpo32=true;

// Person
$Personen_ID_ds=1422;	// Alle
if($_POST['Personen_ID'])
	$maskenfeldwerte['personen_id']=$_POST['Personen_ID'];
else if($_GET['nid'] || $_GET['akfr'])
	$maskenfeldwerte['personen_id']=$GLOBALS['fxpglobals']['person'];

// Betreff
$maskenfeldwerte['name_termin']=$_POST['Name_Termin'];

// Suche von + Suche bis
if($von)
	$maskenfeldwerte['suche_von']=$von;
else if($_GET['nid'] || $_GET['akfr'])
	$maskenfeldwerte['suche_von']=$GLOBALS['datetime'];
if($bis)
	$maskenfeldwerte['suche_bis']=$bis;
else if($_GET['nid'] || $_GET['akfr'])
	$maskenfeldwerte['suche_bis']=fx_date('YmdHis', fx_mktime(12, 0, 0, (int)substr((string)$GLOBALS['datetime'],4,2), (int)substr((string)$GLOBALS['datetime'],6,2)+6, (int)substr((string)$GLOBALS['datetime'],0,4)));

// Terminart
$abs_app=FXP_AT_ABSENT.",".FXP_AT_VACATIONREQUEST.",".FXP_AT_VACATION.",".FXP_AT_SPECIALVACATIONREQUEST.",".FXP_AT_SPECIALVACATION.",".FXP_AT_TRAININGREQUEST.",".FXP_AT_TRAINING.",".FXP_AT_SICKCALL.",".FXP_AT_SICK.",".FXP_AT_SLIDINGDAYREQUEST.",".FXP_AT_SLIDINGDAY;
$app=FXP_AT_MEETING.",".FXP_AT_CONFERENCE.",".FXP_AT_BUSINESSDINNER.",".FXP_AT_CONTACT.",".FXP_AT_MESSAGE.",".FXP_AT_PRIVATE.",".FXP_AT_VARIOUS;
$Terminart_ds=1422;	// Alle
$Terminart_arg=array('tart' => $abs_app.",".$app);
$maskenfeldwerte['terminart']=$_POST['Terminart'];

// Backup der Variablen
if($err)
	unset($GLOBALS['fxptdata']['backup']);
else
	$GLOBALS['fxptdata']['backup']['post']=$_POST;
?>