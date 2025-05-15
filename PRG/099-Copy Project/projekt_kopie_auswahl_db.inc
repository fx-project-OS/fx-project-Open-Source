<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : projekt_kopie_auswahl_db.inc                                 //
// Version     : 24.1                                                         //
// Begin       : 2020-12-10                                                   //
// Last Change : 2020-12-10                                                   //
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
 * 99: Copy Project
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

if($_POST['Ansprechpartner'] == -1)
	$kunde=-1;
else if($_POST['Ansprechpartner'])
	$kunde=(int)db_value("SELECT elter FROM personen WHERE personen_id=".$_POST['Ansprechpartner']);
else
	$kunde=0;
$addinfo=array
(
	'kunde'				=> $kunde,
	'ansprechpartner'	=> $_POST['Ansprechpartner'],

	'projektmanager'	=> $_POST['Projektmanager'],
	'projektleiter'		=> $_POST['Projektleiter'],
	'vertreter'			=> $_POST['Vertreter'],

	'kategorie_id'		=> $_POST['Kategorie_ID'],

	'methode'			=> $_POST['methode'],
	'zeitpunkt'			=> $_POST['zeitpunkt'],
	'tage'				=> (int)$_POST['tage']
);
$err=copy_project($src_id, $dst_id, $inf, $res, $tad, $timeadjust, $GLOBALS['fxpglobals']['client'], $addinfo, true);
if($err)
	error_msg($err);
else
{
	// Masken ausblenden
	$pk_param=false;
	$pk_vorschau=false;

	// Zwischenspeicher löschen
	unset($GLOBALS['_cppj']);

	// Die Vorgänge wurden erfolgreich kopiert.
	error_msg(1841);
}
//fxDebug($GLOBALS['_cppj']['data'], 'data');
?>