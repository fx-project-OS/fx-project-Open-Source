<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : projekt_waehlen_pre.inc                                      //
// Version     : 21.1                                                         //
// Begin       : 2020-12-16                                                   //
// Last Change : 2020-12-16                                                   //
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
 * PF 234: Destroy Projects
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.1
 */

$maske474_sf=true;

// Auswahlfeld füllen
$Projekt_ID_arg=array('mp_only'=>true);
$Projekt_ID_ds=0;

// Einstellungswert berücksichtigen [pm_aendert_mp]: "Mutterprojekte dürfen nur vom zugeteilten Projektmanager verändert werden (ausser Hauptadministrator und Geschäftsführung)?"
if($GLOBALS['fxpglobals']['person'] && ($GLOBALS['fxpglobals']['user'] != 1) && !$GLOBALS['fxpglobals']['persdat']['roles'][2] && (strtolower($GLOBALS['fxpglobals']['settings']['pm_aendert_mp']) == 'ja'))
	$Projekt_ID_arg['rp_projektmanager']=$GLOBALS['fxpglobals']['person'];

if(!$_POST['Projekt_ID'])
	$_POST['Projekt_ID']=$GLOBALS['fxpglobals']['project'];
$maskenfeldwerte['projekt_id']=$_POST['Projekt_ID'];
?>