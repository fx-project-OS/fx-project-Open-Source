<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : eva_auswahl_pre.inc                                          //
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
 * PF 13: Earned-Value-Analysis (EVA)
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

$maske534_sf=true;

// Selection fields:
// ...Project
$Projekt_ID_ds=0;
// ...Customer
$Kunde_ds=1422;				// "All"
$Kunde_arg=array('pr_check' => true);
// ...Project Manager
$Projektmanager_ds=1422;	// "All"
// ...Project Leader
$Projektleiter_ds=1422;		// "All"
// ...Display type
$Anzeigeart_ds=1260;		// "Automatically"
$Anzeigeartselect="SELECT wertetabid, tabwert FROM wertetab WHERE wertetabid IN (917,1554,918,1586,919) and id_sprache=".$GLOBALS['fxpglobals']['lang']." AND mandanten_id=0 ORDER BY position";
$Anzeigeartselectfeld='tabwert';
$Anzeigeartselectwert='wertetabid';

// ...logged in as a customer => do not allow selection of foreign customers
if($GLOBALS['fxpglobals']['persdat']['personentyp'] == FXP_PT_CUSTOMER_CP)
	$Kunde_ds=0;
?>