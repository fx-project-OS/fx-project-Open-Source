<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : db_tab_auswahl_ini.inc                                       //
// Version     : 21.2                                                         //
// Begin       : 2021-02-12                                                   //
// Last Change : 2021-04-14                                                   //
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
 * PF 140: Database Tables
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

$maske327_sf = true;
unset($index);

// Tabelle (Auswahlfeld)
// ...Tabellenart: 1=System, 2=Daten, 3=Intern, 4=Individuell
if($GLOBALS['prgclass'] < 40)	// Nur Sysadmin sieht alle
{
	$tabart  = " WHERE tabelleart IN (1,2,4)";
	$wtabart = " AND position IN (1,2,4)";
}
else
{
	$tabart  = " WHERE tabelleart IS NOT NULL";
	$wtabart = "";
}
$IDinTab_ds = 1079;	// Alle Tabellen
$IDinTabselect		= "SELECT id_tabellen, tabname, tabelleart FROM tabellen".$tabart." ORDER BY tabname";
$IDinTabselectfeld	= 'tabname';
$IDinTabselectwert	= 'id_tabellen';
$maskenfeldwerte['idintab'] = $IDinTab;

// Tabellenart (Auswahlfeld)
$TabelleArt_ds = 1260;	// Automatisch
if($IDinTab)
	$TabelleArt = 0;
$TabelleArtselect		= "SELECT position, tabwert FROM wertetab WHERE id_feld=585 AND mandanten_id=0 AND id_sprache=".$GLOBALS['fxpglobals']['lang'].$wtabart." ORDER BY position";
$TabelleArtselectfeld	= 'tabwert';
$TabelleArtselectwert	= 'position';
$maskenfeldwerte['tabelleart'] = $TabelleArt;

// Komplett
$maskenfeldwerte['komplett'] = $Komplett;

// Wurde auf den Button gedrückt, dann Liste anzeigen
$show=false;
if($Button_Anzeig)
	$show=true;
?>