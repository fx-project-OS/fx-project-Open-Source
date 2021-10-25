<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : terminsuche_ini.inc                                          //
// Version     : 21.2                                                         //
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
 * PF 32: Appointment Search
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

if(!$_POST['Button_suchen'] && is_array($GLOBALS['fxptdata']['backup']['post']))
{
	$_POST=$GLOBALS['fxptdata']['backup']['post'];
	if(!$_GET['nid'] && !$_GET['akfr'])
		$_POST['Button_suchen']=true;
	else
		$_POST['Button_suchen']='';
	$Button_suchen=$_POST['Button_suchen'];
}

// Suchen von
$von='';
if($_POST['suche_von'])
{
	$verr=false;
	$von=validate_entry($_POST['suche_von'], $verr, 'datum');
	if($verr)
	{
		error_msg($verr, false, 'suche_von');
		$err=true;
	}
}

// Suche bis
$bis='';
if($_POST['suche_bis'])
{
	$verr=false;
	$bis=validate_entry($_POST['suche_bis'], $verr, 'datum');
	if($verr)
	{
		error_msg($verr, false, 'suche_bis');
		$err=true;
	}
}

// Bis-Datum kleiner als Von-Datum => Fehlermeldung
if($von && $bis && ($bis < $von))
{
	error_msg(551, array('suche_von', 'suche_bis'));
	$err=true;
}

// Terminart darf nur ausgewählt werden bei eigener Person
if($_POST['Terminart'] && ($_POST['Personen_ID'] != $GLOBALS['fxpglobals']['person']))
{
	error_msg(292, array('Terminart'));
	$err=true;
}


// Personen-ID des aktuellen Mandanten holen
$mpid=db_value("SELECT personen_id FROM personen WHERE personentyp=274 AND mandanten_id=".$GLOBALS['fxpglobals']['client']);

// Person...
$maske143_flt=128;
$parr=array($GLOBALS['fxpglobals']['person'] => true);

// ...Int. Mitarbeiter (275) + Ext. Mitarbeiter (840)
$zptyp='275,840';

// ...Ansprechpartner
// - Mandant (2090)
if(tr_rechte(164) && (!$GLOBALS['fxpglobals']['setfilter'] || $GLOBALS['fxpglobals']['filter']['fperson_inaktiv'] || $GLOBALS['fxpglobals']['filter']['personentyp_2090']))
{
	$zptyp .= ',2090';
	$maske143_flt |= 1;
}
// - Auftraggeber (620)
if(tr_rechte(170) && (!$GLOBALS['fxpglobals']['setfilter'] || $GLOBALS['fxpglobals']['filter']['fperson_inaktiv'] || $GLOBALS['fxpglobals']['filter']['personentyp_620']))
{
	$zptyp .= ',620';
	$maske143_flt |= 1;
}
// - Lieferant (2092)
if(tr_rechte(174) && (!$GLOBALS['fxpglobals']['setfilter'] || $GLOBALS['fxpglobals']['filter']['fperson_inaktiv'] || $GLOBALS['fxpglobals']['filter']['personentyp_2092']))
{
	$zptyp .= ',2092';
	$maske143_flt |= 1;
}
$sql="SELECT personen_id FROM personen WHERE personentyp IN (".$zptyp.") AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
$zpids=db_values($sql, '*');
//fxDebug($zpids, $sql);
if(fxIsArray($zpids))
{
	foreach($zpids as $zpid)
		$parr[$zpid['personen_id']]=true;
}

// ...Ansprechpartner - Kontakt (841)
if(tr_rechte(172) && $GLOBALS['fxpglobals']['filter']['personentyp_841'])
{
	$maske143_flt |= 1;
	$sql="SELECT p.personen_id FROM personen p WHERE p.personentyp=841 AND p.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND (p.elter IN (SELECT personen_id FROM personen WHERE elter=0 OR elter IS NULL OR elter=".$GLOBALS['fxpglobals']['person']." OR elter=".$mpid." OR aenderungs_id=".$GLOBALS['fxpglobals']['person'].") OR p.elter=0 OR p.elter IS NULL OR p.elter=".$GLOBALS['fxpglobals']['person']." OR p.elter=".$mpid." OR p.aenderungs_id=".$GLOBALS['fxpglobals']['person'].")";
	$zpids=db_values($sql, '*');
//fxDebug($zpids, $sql);
	if(fxIsArray($zpids))
	{
		foreach($zpids as $zpid)
			$parr[$zpid['personen_id']]=true;
	}
}

// Personentypen
$pt=array();
$sql="SELECT wertetabid, tabwert FROM wertetab WHERE id_feld=30 AND id_sprache=".$GLOBALS['fxpglobals']['lang']." ORDER BY position";
$arr=db_values($sql, '*');
if(fxIsArray($arr))
{
	foreach($arr as $a)
		$pt[$a['wertetabid']]=$a['tabwert'];
}

$pids=array_to_csv($parr);
?>