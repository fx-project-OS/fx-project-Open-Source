<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : zk_aend_strg_pre.inc                                         //
// Version     : 21.1                                                         //
// Begin       : 2020-11-12                                                   //
// Last Change : 2020-11-12                                                   //
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
 * PF 146: Time Transfers
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.1
 */

setForm(false);

// Keine Indizierung
if(isset($index))
	unset($index);

// Auswahlmaske (339) immer im Änderungsmodus + Datumsfelder inkl. Popup-Icon
$maske339_sf=true;
$GLOBALS['cpo'.$tr]=true;

// Quelle
$qarray=array(-2 => $alle_str, -1 => $unbekannt_str);		// [Alle Vorgänge] + [Unbekannt]
//	$fuerVorgang_arg=array('task_only'=> false, 'projects_only'=>false, 'with_tasks'=>true);
$fuerVorgang_arg=array('timereg'=>true, 'with_projects'=>true, 'emptycost'=>true);
$varray=auswahl_Projekt_ID(1, '', 'fuerVorgang');
//fxDebug($varray, '$varray');
if(fxIsArray($varray))
{
	foreach($varray as $tk => $tv)
		$qarray[$tk]=$tv;
}

$fuerVorgang_ds=0;
$fuerVorgang_arg=array('override'=>$qarray);
if(!isset($_POST['fuerVorgang']))
	$_POST['fuerVorgang']=$GLOBALS['fxpglobals']['project'];
if(!$_POST['fuerVorgang'] || !isset($qarray[$_POST['fuerVorgang']]))
	$_POST['fuerVorgang']=-2;	// Alle Vorgänge
$maskenfeldwerte['fuervorgang']=$_POST['fuerVorgang'];
//fxDebug($qarray, '$qarray: $_POST[\'fuerVorgang\']='.$_POST['fuerVorgang']);

// Ziel
$aufg_sql='';
$zarray=array(-1 => $leer_str, 0 => $unbekannt_str);		// -leer- + [Unbekannt]
//	$Projekt_ID_arg=array('task_only'=>true, 'projects_only'=>false, 'with_tasks'=>true, 'rp_projektstatus'=>FXP_PS_ACTIVE);
$Projekt_ID_arg=array('timereg'=>true, 'emptycost'=>true);
$aarray=auswahl_Projekt_ID(1, '', 'Projekt_ID');
//fxDebug($aarray, '$aarray');
if(fxIsArray($aarray))
{
	foreach($aarray as $tk => $tv)
		$zarray[$tk]=$tv;

	// SQL
	$aufg_sql  = "SELECT p.projekt_id FROM projekte p, zeitdaten z";
	$aufg_sql .= " WHERE p.projekt_id=z.projekt_id AND z.zeitart=102";	// AND p.projektart=".FXP_PRJ_TASK;
	if(strlen($prs))
		$aufg_sql .= " AND p.projekt_id IN (".$prs.")";
	$aufg_sql .= filter_projekt('and', true, false);
}

$Projekt_ID_ds=0;
$Projekt_ID_arg=array('override'=>$zarray);
if(!isset($_POST['Projekt_ID']) || !isset($zarray[$_POST['Projekt_ID']]))
	$_POST['Projekt_ID']=-1;	// leer
$maskenfeldwerte['projekt_id']=$_POST['Projekt_ID'];
//fxDebug($zarray, '$zarray: $_POST[\'Projekt_ID\']='.$_POST['Projekt_ID']);
//fxDebug($aufg_sql, 'Aufgaben-SQL');

// Person
$Personen_ID_ds=0;
$Personen_ID_arg=array('override'=>$parray);
if(!isset($_POST['Personen_ID']))
	$_POST['Personen_ID']=$GLOBALS['fxpglobals']['person'];
$maskenfeldwerte['personen_id']=$_POST['Personen_ID'];

// Auftraggeber
if(!$_POST['Kunde'])
	$_POST['Kunde']=0;
$maskenfeldwerte['kunde']=$_POST['Kunde'];

// Startdatum
if($_POST['Ist_Beg_Dtm'])
	$maskenfeldwerte['ist_beg_dtm']=validate_entry($_POST['Ist_Beg_Dtm'], $verr, 'datum');
else if(!isset($_POST['Personen_ID']))
	$maskenfeldwerte['ist_beg_dtm']=substr($heute,0,4).'0101000000';

// Enddatum
if($_POST['Ist_End_Dtm'])
	$maskenfeldwerte['ist_end_dtm']=validate_entry($_POST['Ist_End_Dtm'], $verr, 'datum');
else if(!isset($_POST['Personen_ID']))
	$maskenfeldwerte['ist_end_dtm']=substr($heute,0,8).'240000';

// Max. Textzeichen
if(!strlen($_POST['text_kuerzen']) && !$err)
	$_POST['text_kuerzen']='50';
$maskenfeldwerte['text_kuerzen']=$_POST['text_kuerzen'];

// Zeilenumbruch
if($_POST['Zeilenumbruch'])
	$maskenfeldwerte['zeilenumbruch']=1;
?>