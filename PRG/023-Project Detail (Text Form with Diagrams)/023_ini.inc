<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : 023_ini.inc                                                  //
// Version     : 24.1                                                         //
// Begin       : 2020-10-29                                                   //
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
 * PF 23: Project Detail (Text Form with Diagrams)
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

// We need the resource class
$GLOBALS['__includer']['resource']=true;
require('includer.inc');

// Do we have ChartDirector?
$req_fn=fxf_fn_reqFilename('ext_chartdir');
if(strlen((string)$req_fn))
	require($req_fn);

// Use PF 241: Cost Main Data
$_apfa=array(241);

// Use PF 28: Ressourceplanner (for Resource Assignment mask)
if($GLOBALS['fxpglobals']['action'] == FXP_DISPLAY)
	$_apfa[]=28;

if(!$suche)
{
	// Gibt es unter diesem Mandant bereits Projekte???
	$Projekt_da=(int)db_value("SELECT projekt_id FROM projekte WHERE mandanten_id=".$GLOBALS['fxpglobals']['client']." ORDER BY projekt_id DESC");

	//... nein, dann wird der User zum Anlegen "genötigt"
	if(!$Projekt_da && !$_POST['Button_Submit'] && !$_POST['Button_okay'] && !$_POST['Button_Anpassen'])
	{
		$GLOBALS['fxpglobals']['action']=FXP_CREATE;

		// Initialisieren der (Mutter)Projekt-IDs
		$Projekt_ID=id_max('projekte', 'projekt_id', '', '', 1, false);
		$GLOBALS['fxpglobals']['project']=$Projekt_ID;
		$Mutterprojekt_ID=$Projekt_ID;
		$_mutterprojektid=$Projekt_ID;

		error_msg(2283);	// 2283: Sie müssen ein Mutterprojekt anlegen!
		$no_project=true;
	}
}
?>