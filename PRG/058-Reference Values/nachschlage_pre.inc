<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : nachschlage_pre.inc                                          //
// Version     : 24.1                                                         //
// Begin       : 2020-08-27                                                   //
// Last Change : 2020-01-23                                                   //
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
 * PF 58: Reference Values - File for presetting the mask fields
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

unset($index);

//Auch bei Anzeige Auswahl ermöglichen
$maske194_sf=true;

// Auswahlbox "Feld" füllen (ID_Feld)
$ID_Feldselect  = "SELECT t.id_referenz, t.literal_15, t.literal_20";
$ID_Feldselect .= " FROM texte t, felder f";
$ID_Feldselect .= " WHERE t.id_referenz=f.id AND t.id_ref_art=3 AND t.id_sprache=".$GLOBALS['fxpglobals']['lang']." AND f.nachschlage<>0 AND f.nachschlage_art=776 AND (f.mandanten_id IS NULL OR f.mandanten_id IN (0,".$GLOBALS['fxpglobals']['client'].")) AND t.id_referenz NOT IN (1,10,660,1586)";
$ID_Feldselect .= " ORDER BY t.literal_20";
$ID_Feldselectfeld='literal_20';
$ID_Feldselectwert='id_referenz';

$ID_Feld_ds=0;
$maskenfeldwerte['id_feld']=$ID_Feld;

// Sprache (sprache)
$Sprache_ds=0;
$maskenfeldwerte['sprache']=$Sprache;

// Beschreibung (text_kurz)
if($ID_Feld)
	$maskenfeldwerte['text_kurz']='^class_darkgrey|~&rarr; &quot;'.db_value("SELECT text_kurz FROM texte WHERE id_sprache=".$GLOBALS['fxpglobals']['lang']." AND id_ref_art=3 AND id_referenz=".$ID_Feld).'&quot;';
?>