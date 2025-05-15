<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : rechte_vergabe_ref_ini.inc                                   //
// Version     : 24.1                                                         //
// Begin       : 2020-12-21                                                   //
// Last Change : 2024-04-24                                                   //
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
 * PF 138: Access Rights - General
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

// Referenzkategorie, -ordner + -ID
$rcat=0;
$rfld=0;
$rid=0;

// Auswahlparameter forcieren?
$forced=false;
// ...falls der Benutzer über das Projektmenü-Popup kommt
if(!$forced && (isset($GLOBALS['gproject']) || (fxIsArray($_GET) && isset($_GET['uid']) && isset($GLOBALS['fxpglobals']['project']))))
{
	if(isset($GLOBALS['gproject']))
		$rid=(int)$GLOBALS['gproject'];
	else
		$rid=(int)$GLOBALS['fxpglobals']['project'];
	if($rid)
	{
		$rcat=37;	// Projekte
		$rfld=(int)db_value("SELECT mutterprojekt_id FROM projekte WHERE projekt_id=".$rid." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);

		$forced=true;
	}
}
// ...oder falls der Benutzer aus der DMS-Verwaltung kommt
if(!$forced && (fxIsArray($_GET) && isset($_GET['aus_trans']) && ($_GET['aus_trans'] == 106)))
{
	$rcat=38;	// Dokumente
	$rid=(int)$_GET['ID_Referenz'];
	$rfld=(int)db_value("SELECT elter_art FROM d_dokumente WHERE dok_id=".$rid." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);

	$forced=true;
}

if(!$forced)
{
	// Referenzkategorie
	$tra=array(192=>37,36,38,35);
	if($GLOBALS['fxpglobals']['tr'] != 138)
		$rcat=$tra[$GLOBALS['fxpglobals']['tr']];
	if(!$rcat && (!fxIsArray($_GET) || !isset($_GET['uid'])) && fxIsArray($_POST) && isset($_POST['Rechte_Ref']))
		$rcat=(int)$_POST['Rechte_Ref'];

	// Referenzordner
	if($rcat && (!fxIsArray($_GET) || !isset($_GET['uid'])))
	{
		if(fxIsArray($_POST) && isset($_POST['Rechte_ReferenzID']))
			$rfld=(int)$_POST['Rechte_ReferenzID'];
	}

	// Referenz-ID
	if($rcat == 35)
		$rid=$rfld;
	else if($rfld && (!fxIsArray($_GET) || !isset($_GET['uid'])))
	{
		if(fxIsArray($_POST) && isset($_POST['ID_Referenz']))
			$rid=(int)$_POST['ID_Referenz'];
	}
}

// Datenbankaktionen ausführen?
$db_update=false;
if($rid && fxIsArray($_POST) && isset($_POST['Button_Speichern']))
	$db_update=true;
$_POST['db_update']=$db_update;

// Rechtevergabemaske einblenden?
$show_vergabe=false;
if($rid && ($forced || (fxIsArray($_POST) && (isset($_POST['Button_Anzeigen']) || isset($_POST['Button_Speichern'])))))
	$show_vergabe=true;
echo(fieldHidden('temp_rcat', $rcat));
//echo('$rcat=<b>'.$rcat.'</b>, $rfld=<b>'.$rfld.'</b>, $rid=<b>'.$rid.'</b><br />$db_update=<b>'.$db_update.'</b><hr />');
?>