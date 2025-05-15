<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : prteam_auswahl_pre.inc                                       //
// Version     : 24.1                                                         //
// Begin       : 2020-11-16                                                   //
// Last Change : 2020-11-16                                                   //
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
 * PF 230: Team Builder
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

$maske467_sf=true;


// Projekt-Team:
if($GLOBALS['fxpglobals']['action'] != FXP_CREATE)
{
	$projektteam_id_ds=0;
	if(!$_POST['projektteam_id'])
		$_POST['projektteam_id']=$projektteam_id_tmp;
	$maskenfeldwerte['projektteam_id']=$_POST['projektteam_id'];
	protectVar($_POST['projektteam_id'], 'projektteam_id_tmp');
}

// Vorgang:
$maskenfeldwerte['projekt_id']=$_POST['Projekt_ID'];


// Felder ausblenden
if(($GLOBALS['fxpglobals']['action'] == 1) || ($GLOBALS['fxpglobals']['action'] == 4))
	$Projekt_ID_nv=true;
else if($GLOBALS['fxpglobals']['action'] == 2)
	$projektteam_id_nv=true;

// Nur-Lesen-Feld
if((($GLOBALS['fxpglobals']['action'] == 3) || ($GLOBALS['fxpglobals']['action'] == 4)) && $_POST['Button_Multi'])
	$projektteam_id_sf=1;
?>