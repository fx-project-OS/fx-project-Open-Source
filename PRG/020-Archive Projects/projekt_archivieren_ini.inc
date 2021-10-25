<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : projekt_archivieren_ini.inc                                  //
// Version     : 21.2                                                         //
// Begin       : 2020-12-15                                                   //
// Last Change : 2020-12-15                                                   //
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
 * PF 20: Archive Projects
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

$sort_number=false;
if(strtolower($GLOBALS['fxpglobals']['settings']['vmn_projektnr']) == 'ja')
	$sort_number=true;

// Einstellungswert berücksichtigen [pm_aendert_mp]: "Mutterprojekte dürfen nur vom zugeteilten Projektmanager verändert werden (ausser Hauptadministrator und Geschäftsführung)?"
$only_pm=false;
if(strtolower($GLOBALS['fxpglobals']['settings']['pm_aendert_mp']) == 'ja')
	$only_pm=true;

// Alle vorhanden Mutterprojekte zählen
$sql="SELECT COUNT(projekt_id) FROM projekte WHERE projektart=".FXP_PRJ_MAIN." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
$mp_all=(int)db_value($sql);

if($GLOBALS['fxpglobals']['tr'] == 281)	// Projekte einchecken
	$cop=prjGetCheckedOut(true);

// Alle berechtigten Mutterprojekte ermitteln
$sql  = "SELECT";
$sql .= " p.projekt_id, p.name_projekt, p.vorgangsnummer, p.projekt_status, p.projekt_archiviert,";
if($GLOBALS['fxpglobals']['tr'] == 281)	// Projekte einchecken
	$sql .= " p.projektmanager, p.projektleiter, p.vertreter, p.cout_id, p.cout_date,";
$sql .= " z.soll_beginn, z.soll_ende, z.ist_beginn, z.ist_ende ";
$sql .= "FROM";
$sql .= " projekte p, zeitdaten z ";
$sql .= "WHERE";
$sql .= " p.projektart=".FXP_PRJ_MAIN." AND p.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND z.projekt_id=p.projekt_id AND z.zeitart=102 AND z.mandanten_id=".$GLOBALS['fxpglobals']['client'];
// Falls kein Systemadministrator, kein Hauptadministrator und kein Geschäftsführer, dann Projekte beschränken...
if($GLOBALS['fxpglobals']['person'] && ($GLOBALS['fxpglobals']['user'] != 1) && !$GLOBALS['fxpglobals']['persdat']['roles'][2])
{
	$sql .= " AND (";
	// ...auf Projekte, bei denen man der Projektmanager ist
	$sql .= "(p.projektmanager=".$GLOBALS['fxpglobals']['person'].")";
	// ...auf Projekte, bei denen man der Projektleiter oder der Stellvertreter ist
	if(!$only_pm)
		$sql .= " OR (p.projektleiter=".$GLOBALS['fxpglobals']['person']." OR p.vertreter=".$GLOBALS['fxpglobals']['person'].")";
	$sql .= ")";
}
// Filterkriterien ohne Archiv einbeziehen
$GLOBALS['mskfilter']=0;
$GLOBALS['keep_ignore']=true;
$GLOBALS['ignore_archiv']=true;
if($GLOBALS['fxpglobals']['tr'] == 281)	// Projekte einchecken
{
	$GLOBALS['ignore_projektfilter']=true;
	$sql .= filter_projekt('and', false, false);
	$maske5_flt=$GLOBALS['mskfilter'];
}
else
{
	$sql .= filter_projekt('and', false, false, 'projekt_archiviert');
	$maske536_flt=$GLOBALS['mskfilter'];
}

// Sortierung, je nach Einstellungswert
$sql .= " ORDER BY ";
if(!$sort_number)
	$sql .= "p.name_projekt, ";
$sql .= "p.vorgangsnummer";

$tmp=db_values($sql, '*');
//fxDebug($tmp, $sql, 0);

// Mutterprojektliste erstellen
$mp_list=array();
if(fxIsArray($tmp))
{
	foreach($tmp as $t)
		$mp_list[(int)$t['projekt_id']]=$t;
}

unset($tmp);
//fxDebug($mp_list, $sql, 0);

// Alle berechtigten Mutterprojekte zählen
$mp_rights=sizeof($mp_list);

// Hinweise anzeigen?
if(!$mp_all || !$mp_rights)
{
	// ...falls noch keine Mutterprojekte definiert wurden
	if(!$mp_all)
		$warning=meldung(1719, true);				// Es wurden für diesen Mandanten noch keine Projekte angelegt!
	// ...falls man keine Berechtigung auf eines der Mutterprojekte hat
	else if(!$mp_rights)
		$warning=meldung(2868, true, $mp_all);	// Sie besitzen nicht die notwendigen Rechte um irgendeines der gefunden $var1 Mutterprojekte abändern zu dürfen!

	fxp_warn($warning);

	// Listenmaske ausblenden
	if($GLOBALS['fxpglobals']['tr'] == 281)	// Projekte einchecken
		$maske5_nv=true;
	else
		$maske536_nv=true;
}

if($GLOBALS['fxpglobals']['tr'] == 281)	// Projekte einchecken
{
	// Delete all checked in main projects, so that only the checked out remain
	if(sizeof($mp_list))
	{
		foreach($mp_list as $mp_id => $mp_data)
		{
			$coid=(int)$mp_data['cout_id'];
			if(!$coid)
				unset($mp_list[$mp_id]);
		}
	}
	// Display message if no checked out main project exists
	if($mp_rights && !sizeof($mp_list))
	{
		if($GLOBALS['fxpglobals']['person'] && ($GLOBALS['fxpglobals']['user'] != 1) && !$GLOBALS['fxpglobals']['persdat']['roles'][2])
			$note=meldung(3021,true);	// At the moment no project, for which you are authorized, is blocked and in use by another person!
		else
			$note=meldung(3020,true);	// At the moment no project is blocked and in use by another person!
		fxp_note($note);
		$maske5_nv=true;
	}
}
?>