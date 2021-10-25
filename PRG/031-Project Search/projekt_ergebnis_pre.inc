<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : projekt_ergebnis_pre.inc                                     //
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
 * PF 31: Project Search
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

$pids="";
$mc=$GLOBALS['_maskcounter'];

// Zusatzabfrage für SQL
$sql_add='';
if($pa)
	$sql_add .= " AND p.projektart=".$pa;
if($pl)
	$sql_add .= " AND p.projektleiter=".$pl;
if($ag)
	$sql_add .= " AND p.kunde=".$ag;
if(strlen($sv))
	$sql_add .= " AND (z.soll_ende>='".$sv."' OR (z.ist_ende IS NOT NULL AND z.ist_ende>='".$sv."'))";
if(strlen($sb))
	$sql_add .= " AND (z.soll_beginn<='".$sb."' OR (z.ist_beginn IS NOT NULL AND z.ist_beginn<='".$sb."'))";

// SQL zusammenbasteln...
if(strlen($st))
{
	// ...für Suche nach Vorgangsnummer
	$vns=trim(str_replace('0000', '', add_null($st)));
	if(strlen($vns))
	{
		$sql  = "SELECT p.projekt_id FROM projekte p, zeitdaten z";
		$sql .= " WHERE z.projekt_id=p.projekt_id AND z.zeitart=102 AND p.mandanten_id=".$GLOBALS['fxpglobals']['client'].$sql_add;
		$sql .= " AND ".db_like('p.vorgangsnummer', $vns.'%');

		$pids=add_pids($pids, $sql);
	}

	// ...für Suche nach Name
	if($cn)
	{
		$sql  = "SELECT p.projekt_id FROM projekte p, zeitdaten z";
		$sql .= " WHERE z.projekt_id=p.projekt_id AND z.zeitart=102 AND p.mandanten_id=".$GLOBALS['fxpglobals']['client'].$sql_add;
		$sql .= " AND ".db_like('p.name_projekt', '%'.$st.'%');

		$pids=add_pids($pids, $sql);
	}

	// ...für Suche nach PCA-Code
	if($cp)
	{
		$sql  = "SELECT p.projekt_id FROM projekte p, zeitdaten z, projekte_parameter m";
		$sql .= " WHERE z.projekt_id=p.projekt_id AND z.zeitart=102 AND m.projekt_id=p.projekt_id";
		$sql .= " AND p.mandanten_id=".$GLOBALS['fxpglobals']['client'].$sql_add;
		$sql .= " AND m.parameter_typ=376 AND m.parameter_var='TXT' AND ".db_like('m.parameter_txt', '%'.$st.'%');

		$pids=add_pids($pids, $sql);
	}

	// ...für Suche nach ID
	if($ci)
	{
		$sql  = "SELECT p.projekt_id FROM projekte p, zeitdaten z";
		$sql .= " WHERE z.projekt_id=p.projekt_id AND z.zeitart=102 AND p.mandanten_id=".$GLOBALS['fxpglobals']['client'].$sql_add;
		$sql .= " AND p.projekt_id=".(int)$st;

		$pids=add_pids($pids, $sql);
	}
}
else
{
	$sql  = "SELECT p.projekt_id FROM projekte p, zeitdaten z";
	$sql .= " WHERE z.projekt_id=p.projekt_id AND z.zeitart=102 AND p.mandanten_id=".$GLOBALS['fxpglobals']['client'].$sql_add;

	$pids=add_pids($pids, $sql);
}

// Projektinfos ermitteln
$pinf=array();
if(strlen($pids))
{
	$maske146_flt=0;

	$sql  = "SELECT";
	$sql .= " p.projekt_id, p.mutterprojekt_id, p.vorgangsnummer, p.name_projekt, p.projektart, p.kunde, p.projektleiter, p.beschreibung,";
	$sql .= " z.soll_beginn, z.soll_ende ";
	$sql .= "FROM";
	$sql .= " projekte p, zeitdaten z ";
	$sql .= "WHERE";
	$sql .= " z.projekt_id=p.projekt_id AND z.zeitart=102 AND p.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND (".$pids.")";
	if($GLOBALS['fxpglobals']['person'] && ($GLOBALS['fxpglobals']['user'] > 1) && !$GLOBALS['fxpglobals']['persdat']['roles'][2])
		$sql .= " AND p.projekt_id IN (".prights_get(0).")";
	$sql .= filter_projekt('and', false, false, 'projekt_id')." ";
	$sql .= "ORDER BY";
	$sql .= " p.vorgangsnummer";

	$pinf=db_values($sql, '*');
//fxDebug($pinf, $sql);

	$maske146_flt=$GLOBALS['mskfilter'];
}

// Projektinfos anzeigen
if(fxIsArray($pinf))
{
	// Nowrap
	$Vorgangsnummer_nw	= true;
	$Name_Projekt_nw	= true;
	$ID_nw				= true;
	$Projektart_nw		= true;
	$Soll_Beg_Dtm_nw	= true;
	$Soll_End_Dtm_nw	= true;
	$Projektleiter_nw	= true;
	$Kunde_nw			= true;
	$pca_code_nw		= true;
	$link_nw			= true;

	// Spalten ausblenden
	if(!$cp)
		$pca_code_nv=true;
	if(!$ci)
		$ID_nv=true;

	// Projektart
	$part=kategorie_array('projektart');

	// Link-Tooltip
	$ltt='';
	if(tr_rechte(23))
		$ltt='<b>'.meldung(1697,false).'</b>: '.trPath(23);	// Sprung zu
	else
		$link_nv=true;

	// Listenvariablen
	$pers=array();

	// No right border in the following column
	fxf_maskColumnBorder(array('link'), 0, -1, $mc);

	// Mask fields
	$fa=fxf_getMaskFields(146,false,array('link'=>0));
//fxDebug($fa,'$fa', 0);

	// Liste erzeugen
	$z=0;
	$n='';
	foreach($pinf as $p)
	{
		// New main project?
		$m=substr($p['vorgangsnummer'],0,4);
		if(strlen($n) && ($m != $n))
		{
			fxf_maskColumnBorder($fa, 1, -1, $mc, $z, true);
			$z++;
		}

		// Nr.
		$fcl='black';
		if($p['projektart'] == FXP_PRJ_MAIN)
			$fcl='black bold';
		else if($p['projektart'] == FXP_PRJ_SUB)
			$fcl='darkgrey italic';
		$spacer=str_repeat('&nbsp;', (int)substr_count($p['vorgangsnummer'],'.')*2);
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['vorgangsnummer']='~'.$spacer.'<font class="'.$fcl.'">'.no_null($p['vorgangsnummer']).'</font>';
		$n=$m;

		// Vorgangs-Name
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['name_projekt']='~<font class="'.$fcl.'">'.$p['name_projekt'].'</font>';

		// ID
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['id']='~<font class="s2 lightblue">'.$p['projekt_id'].'</font>';

		// Vorgangsart
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['projektart']='~'.$part[$p['projektart']];

		// Startdatum
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['soll_beg_dtm']='~'.get_entry($p['soll_beginn'], 'datum');

		// Enddatum
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['soll_end_dtm']='~'.get_entry($p['soll_ende'], 'datum');

		// Projektleiter
		$ppl=(int)$p['projektleiter'];
		if($ppl)
		{
			if(!isset($pers[$ppl]))
				$pers[$ppl]=person_pic($ppl);
			if(!isset($pers[$ppl]))
				$pers[$ppl]='[?]';
			$txt=$pers[$ppl];
		}
		else
			$txt='[!]';
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['projektleiter']='~'.$txt;

		// Auftraggeber
		$pag=(int)$p['kunde'];
		if($pag)
		{
			if(!isset($pers[$pag]))
				$pers[$pag]=person_pic($pag);
			if(!isset($pers[$pag]))
				$pers[$pag]='[?]';
			$txt=$pers[$pag];
		}
		else
			$txt='[!]';
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['kunde']='~'.$txt;

		// PCA-Code
		$pcac=pca_code($p['projekt_id']);
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['pca_code']='~<font class="s2 grey">['.$pcac.']</font';

		// Link
		if(!isset($link_nv))
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['link']='~'.fieldLinkImage('lnk'.$z, 'icln', '?suche=true&Projekt_ID='.(int)$p['projekt_id'].'&Mutterprojekt_ID='.(int)$p['mutterprojekt_id'], $ltt, 23);

		$z++;
	}

	// Last line with Top Border?
	if($z)
	{
		fxf_maskColumnBorder($fa, 1, -1, $mc, $z, true);
		$z++;
	}
}


////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/**
 * ???FUNCTION???
 *
 * @param various $pids - Mandatory parameter: ???PARAMETER???
 * @param various $sql  - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function add_pids($pids, $sql)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(strlen($pids))
		$pids .= " OR ";
	$pids .= "p.projekt_id IN (".$sql.")";

	return $pids;
}
?>