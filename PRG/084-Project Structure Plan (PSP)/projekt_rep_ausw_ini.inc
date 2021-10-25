<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : projekt_rep_ausw_ini.inc                                     //
// Version     : 21.2                                                         //
// Begin       : 2020-12-23                                                   //
// Last Change : 2020-12-23                                                   //
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
 * PF 84: Project Structure Plan (PSP)
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

$cpo118=true;
$cpo152=true;

$fa=array('pmenu_aktiv', 'pmenu_inakt', 'pmenu_gepla', 'pmenu_abges', 'kritisch', 'recalc_tasks', 'pmenu_von', 'pmenu_bis', 'Level', 'projekt_kuerzen', 'ausfuehrlich', 'anteilig', 'Projekt_ID', 'Kunde', 'Projektmanager', 'Projektleiter', 'MaArt_ID');

$spid=0;
// Kommt der Benutzer aus Projektmenü-Popup
if($GLOBALS['gproject'])
	$spid=$GLOBALS['gproject'];
else if(fxIsArray($_GET) && isset($_GET['psid']))
	$spid=(int)$_GET['psid'];
else if(($_GET['uid'] == 'newtr') && $GLOBALS['fxpglobals']['project'])
	$spid=$GLOBALS['fxpglobals']['project'];
if($spid)
{
	$ProjektID			= $spid;

	$pmenu_aktiv		= '1';
	$pmenu_inakt		= '1';
	$pmenu_gepla		= '1';
	$pmenu_abges		= '1';
	$kritisch			= '';
	$recalc_tasks		= '';

	$pmenu_von			= '';
	$pmenu_bis			= '';
	$Level				= 99;
	$projekt_kuerzen	= '';
	$ausfuehrlich		= '1';
	$anteilig			= '';

	$Kunde				= 0;
	$Projektmanager		= 0;
	$Projektleiter		= 0;
	$MaArt_ID			= 0;

	$Button_Submit		= true;

	foreach($fa as $f)
		$_POST[$f]=${$f};
}

// Füllen des Feldes ProjektID
if($GLOBALS['fxpglobals']['tr'] != 84)
	$Proj_Arr=auswahl_ProjektID(1);

// Auf "Übernehmen" geklickt
if($Button_Submit)
{
	$db_update=false;

	// Projekt-Status
	if(!$pmenu_aktiv)
		$pmenu_aktiv='';
	if(!$pmenu_inakt)
		$pmenu_inakt='';
	if(!$pmenu_gepla)
		$pmenu_gepla='';
	if(!$pmenu_abges)
		$pmenu_abges='';

	// Aufgaben
	if(!$Aufgaben)
		$Aufgaben='';

	// Mutterprojekt
	if(!$Mutterprojekt_ID)
		$Mutterprojekt_ID=0;

	// Projekt-Zeitraum
	if(!$pmenu_von)
		$pmenu_von=false;
	else
		$pmenu_von=checkdatum('pmenu_von');
	if(!$pmenu_bis)
		$pmenu_bis=false;
	else
		$pmenu_bis=checkdatum('pmenu_bis');

	// Personen
	if(!$Kunde)
		$Kunde=0;
	if(!$Projektleiter)
		$Projektleiter=0;
	if(!$Projektmanager)
		$Projektmanager=0;

	// Abteilung und Fähigkeit
	if(!$Abt_ID)
		$Abt_ID=0;
	if(!$MaArt_ID)
		$MaArt_ID=0;

	// Fehlerprüfung, ob Enddatum vor Beginndatum liegt
	if(!$err && ($pmenu_bis < $pmenu_von) && $pmenu_bis)
	{
		error_msg(1065, false); // Endzeit vor Startzeit
		$err=true;
	}
	// Fehlerprüfung, beim Projektreport MUSS ein Projekt gewählt werden
	if(!$err && ($GLOBALS['fxpglobals']['tr'] == 118) && !$ProjektID)
	{
		error_msg(2189, -2, 'ProjektID'); // Bitte wählen Sie ein Projekt aus!
		$err=true;
	}
}
?>