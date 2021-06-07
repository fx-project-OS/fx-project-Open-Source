<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : faefaehigkeitenstrg_pre.inc                                  //
// Version     : 21.1                                                         //
// Begin       : 2020-10-19                                                   //
// Last Change : 2018-10-19                                                   //
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
 * PF 17: Skills
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.1
 */

$maske137_sf=true;
$maskenfeldwerte['maart_id']=$MaArt_ID;

if($GLOBALS['fxpglobals']['tr'] == 17)
	$MaArt_ID_ds=0;
else
	$MaArt_ID_ds=1129;

$MaArt_IDselect  = "SELECT maart_id, maart_name FROM ma_art WHERE id_sprache=".$GLOBALS['fxpglobals']['lang']." AND ";
$MaArt_IDselect .= "(mandanten_id=".$GLOBALS['fxpglobals']['client']." OR (mandanten_id=0 AND maart_id>1))";
$MaArt_IDselect .= " ORDER BY maart_name";

$MaArt_IDselectfeld='maart_name';
$MaArt_IDselectwert='maart_id';

// Prüfen, ob zumindest ein Eintrag vorhanden ist
$matest=db_values($MaArt_IDselect);
if(!fxIsArray($matest))
{
	$maskenfeldwerte['maart_id']='~<b>'.meldung(712, true).'</b>';
	$Button_Submit_nv=1;
}
?>