<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : personsuche_ini.inc                                          //
// Version     : 24.1                                                         //
// Begin       : 2020-12-22                                                   //
// Last Change : 2020-12-22                                                   //
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
 * PF 82: Person Search
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

$maske219_sf=true;

$Abt_IDselect="SELECT abt_id, abt_name, abt_ebene FROM abteilung WHERE mandanten_id=".$GLOBALS['fxpglobals']['client']." AND abt_id>0 ORDER BY abt_ebene";
$Abt_IDselectfeld='abt_name';
$Abt_IDselectwert='abt_id';
$Abt_IDselectfunc='$selectwerte["abt_name"]=$selectwerte["abt_ebene"]." ".$selectwerte["abt_name"];';

if(!$_POST['Kontaktmoeglichkeit'])
	$Kontaktmoeglichkeit='';

// Zuweisen der Werte zu den jew. Feldern
$maskenfeldwerte['pname']=$Pname;
$maskenfeldwerte['vorname']=$Vorname;
$maskenfeldwerte['personentyp']=$PersonenTyp;
$maskenfeldwerte['pposition']=$Pposition;
$maskenfeldwerte['kontaktmoeglichkeit']=$Kontaktmoeglichkeit;
$maskenfeldwerte['abt_id']=$Abt_ID;
?>