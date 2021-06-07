<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : zk_aend_list_ini.inc                                         //
// Version     : 21.1                                                         //
// Begin       : 2020-11-12                                                   //
// Last Change : 2021-03-01                                                   //
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

$mc=2;
if(fxIsArray($_POST['Zeit_ID']))
{
	reset($_POST['Zeit_ID']);
	$mc=key($_POST['Zeit_ID']);
}

// Einstellungen zwischenspeichern
if($_POST['Button_Anzeigen'])
{
	$GLOBALS['fxptdata']['backup']=array();
	$bka=array('fuerVorgang', 'Projekt_ID', 'Personen_ID', 'Kunde', 'Ist_Beg_Dtm', 'Ist_End_Dtm', 'text_kuerzen', 'Zeilenumbruch');
	foreach($bka as $vname)
		$GLOBALS['fxptdata']['backup'][$vname]=$_POST[$vname];
}
else if(is_array($GLOBALS['fxptdata']['backup']))
{
	foreach($GLOBALS['fxptdata']['backup'] as $vname => $vval)
		$_POST[$vname]=$vval;
}

// Standardtexte setzen
$unbekannt_str	= meldung(1449, false);			// [Unbekannt]
$ohne_str		= '['.meldung(388, false).']';	// [Ohne]
$alle_str		= '['.meldung(1077, false).']'; // [Alle Vorgänge]
$leer_str		= meldung(794, false);			// -leer-

$heute			= fxNow();

// Ist die Kostenrechnung aktiv?
if(function_exists('fxsppCheckEBTR') && fxsppCheckEBTR(241) && (substr(strtolower($GLOBALS['fxpglobals']['settings']['kr_aktiv']), 0, 1) == 'j'))	// Kostenrechnung aktiv
	$kr_aktiv=true;
else
	$kr_aktiv=false;

// Falls Kostenrechnung aktiv - Muss eine Leistungsart angegeben werden?
if(strtolower($GLOBALS['fxpglobals']['settings']['kr_lamuss']) == 'ja')
	$kr_lamuss=true;
else
	$kr_lamuss=false;

// Personen
$pers_sql="";
$parray=array();
$aarray=array();

$sqlz  = " FROM personen p, artikel a";
$sqlz .= " WHERE a.personen_id=p.personen_id AND p.personentyp IN (275,840) AND (a.sachmittel_id=0 OR a.sachmittel_id IS NULL) AND (a.archiv IS NULL OR a.archiv=0)";
$sqlz .= filter_person('and', true, false, 'Personen_ID');

$sql="SELECT p.personen_id, p.pname, p.vorname, p.personentyp, p.geschlecht, p.inaktiv, p.personen_initialen, a.artikel_id".$sqlz." ORDER BY p.pname, p.vorname";
$pinfo=db_values($sql, '*');
//fxDebug($pinfo, $sql);
if(fxIsArray($pinfo))
{
	$pers_sql="SELECT p.personen_id".$sqlz;

	$ptyp=array(275=>get_text(1225, '5'), 840=>get_text(1229, '5'));	// 275=Int. - 840=Ext.
	$inakt=' {*'.get_text(1391, '20').'*}';								// {*inaktiv*}

	if(sizeof($pinfo) > 1)
		$parray[-1]='['.meldung(1410, false).']';	// Alle Personen

	foreach($pinfo as $l => $p)
	{
		$p_id		= (int)$p['personen_id'];
		$p_typ		= (int)$p['personentyp'];
		$p_inaktiv	= (int)$p['inaktiv'];
		$p_name		= trim($p['pname']);
		$p_vorname	= trim($p['vorname']);
		$p_init		= trim($p['personen_initialen']);

		$pinfo[$l]['ptyp']='';
		if(strlen($p_init))
			$pinfo[$l]['ptyp'] .= ' ('.$p_init.')';
		$pinfo[$l]['ptyp'] .= ' ['.$ptyp[$p_typ].']';
		if($p_inaktiv)
			$pinfo[$l]['ptyp'] .= $inakt;

		$parray[$p_id]=$p_name;
		if(strlen($p_vorname))
			$parray[$p_id] .= ', '.$p_vorname;
		$parray[$p_id] .= $pinfo[$l]['ptyp'];

		$aarray[$p_id]=array('artikel_id'=>(int)$p['artikel_id'], 'ek_standard'=>'', 'vk_standard'=>'');
	}
}
else
	$parray[0]=$leer_str;
//fxDebug($parray, '$parray');
//fxDebug($aarray, '$aarray');

$Personen_ID=0;
if($_POST['Personen_ID'])
	$Personen_ID=$_POST['Personen_ID'];

// Projektrechte ermitteln als SQL
$prs="";
if($GLOBALS['fxpglobals']['person'] && ($GLOBALS['fxpglobals']['user'] > 1) && !$GLOBALS['fxpglobals']['persdat']['roles'][2])
	$prs=prights_get(0);

// Zeitliche Einschränkungen
if($_POST['Ist_Beg_Dtm'])
	$sdat=substr(validate_entry($_POST['Ist_Beg_Dtm'], $verr, 'datum'), 0, 8).'000000';
else
	$sdat='00000000000000';
if($_POST['Ist_End_Dtm'])
	$edat=substr(validate_entry($_POST['Ist_End_Dtm'], $verr, 'datum'), 0, 8).'240000';
else
	$edat='99999999999999';

// Keine DB-Änderungen falls auf "Anzeigen" geklickt wurde
if($_POST['Button_Anzeigen'])
	$db_update=false;

// Liste anzeigen?
$zshow_list=false;
if($_POST['Button_Multi'] || (!$err && $_POST['Button_Anzeigen']))
	$zshow_list=true;
?>