<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : termine_ergebnis_pre.inc                                     //
// Version     : 24.1                                                         //
// Begin       : 2020-12-22                                                   //
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
 * PF 32: Appointment Search
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

$Startzeit_nw		= true;
$Endzeit_nw			= true;
$Terminart_nw		= true;

// SQL zusammenbauen
$sql ="SELECT t.termin_id, t.personen_id, t.startzeit, t.endzeit, t.terminart, t.erledigt, t.name_termin, p.pname, p.vorname, p.geschlecht";
$sql .= " FROM termine t, personen p";
$sql .= " WHERE t.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND (t.archiv=0 OR t.archiv IS NULL) AND (p.inaktiv=0 OR p.inaktiv IS NULL)";
if($_POST['Personen_ID'])
	$sql .= " AND t.personen_id=".$_POST['Personen_ID'];
else
	$sql .= " AND t.personen_id IN (".$pids.")";
if($_POST['Terminart'])
	$sql .= " AND t.terminart=".$_POST['Terminart'];
if($von && $bis)
	$sql .= " AND (t.startzeit<'".$bis."' AND t.endzeit>'".$von."')";
else if($von)
	$sql .= " AND t.endzeit>'".$von."'";
else if($bis)
	$sql .= " AND t.startzeit<'".$bis."'";
if($_POST['Name_Termin'])
{
	$dblnt='%'.trim((string)$_POST['Name_Termin']).'%';
	$sql .= " AND ".db_like('t.name_termin', $dblnt);
}
$sql .= " AND p.personen_id=t.personen_id AND p.mandanten_id=".$GLOBALS['fxpglobals']['client'];
$sql .= " ORDER BY p.pname, p.vorname, t.personen_id, t.startzeit";
$termine=db_values($sql, '*');
//fxDebug($termine, $sql);

// Termine anzeigen
if(fxIsArray($termine))
{
	// Untergeordnete
	if($GLOBALS['fxpglobals']['persdat']['roles'][-1] || $GLOBALS['fxpglobals']['persdat']['roles'][2])
		$slaves[0]=true;
	else
	{
		$slaves='';
		$sql="SELECT personen_id FROM personen WHERE elter=".$GLOBALS['fxpglobals']['person']." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$arr=db_values($sql, '*');
		if(fxIsArray($arr))
		{
			foreach($arr as $a)
				$slaves[$a['personen_id']]=true;
		}
	}
//fxDebug($slaves, 'Untergeordnete');

	// Terminarten (0=Abwesend, 1=Termine, 2=Einladung, ...)
	$ta=array(meldung(185, false), meldung(186, false), meldung(187, false));
	$sql="SELECT wertetabid, tabwert FROM wertetab WHERE id_feld=434 AND mandanten_id=0 AND id_sprache=".$GLOBALS['fxpglobals']['lang'];
	$arr=db_values($sql, '*');
	if(fxIsArray($arr))
	{
		foreach($arr as $a)
			$ta[$a['wertetabid']]=$a['tabwert'];
	}
//fxDebug($ta, 'Terminarten');

	// Abwesenheitstermine
	$abs_app=get_uskg_array();

	// Mask fields
	$mc=$GLOBALS['_maskcounter'];
	fxf_maskColumnBorder(array('icon_ok'), 0, -1, $mc);
	$fa=fxf_getMaskFields(144,false,array('icon_ok'=>0));
//fxDebug($fa,'$fa', 0);

	$z=0;
	$p=0;
	foreach($termine as $t)
	{
		// Person
		if($t['personen_id'] != $p)
		{
			$p=$t['personen_id'];

			// Leerzeile
			if($z)
			{
				fxf_maskColumnBorder($fa, 1, -1, $mc, $z, true);
				$z++;
			}

			// Titelzeile
			$name=trim((string)$t['pname']);
			if(strlen((string)trim((string)$t['vorname'])))
				$name .= ', '.trim((string)$t['vorname']);
			if($p == $GLOBALS['fxpglobals']['person'])
				$col=12;
			else if($slaves[0] || $slaves[$p])
				$col=1;
			else
				$col=3;
			fxf_listSectHeader($mc, $z, '&nbsp;'.person_pic($p, 20, $t['geschlecht'], $GLOBALS['fxpglobals']['client'], false, 'border-top-left-radius:6px;').'&nbsp;&nbsp;'.$name, 'startzeit', 'name_termin', 4, $col, 2);
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['icon_ok']='~|';
			$z++;
		}

		// Terminstart
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['startzeit']='~'.get_entry($t['startzeit'], 'datum').' <font class="grey">'.get_entry($t['startzeit'], 'zeit').'</font>';

		// Terminende
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['endzeit']='~'.get_entry($t['endzeit'], 'datum').' <font class="grey">'.get_entry($t['endzeit'], 'zeit').'</font>';

		// Terminart
		$lnk=0;
		$ata=$t['terminart'];
		if(isset($abs_app[$ata]))
			$tc='darkgrey';
		else if($ata == 187)
			$tc='grey';
		else
			$tc='black';
		$txt=$ta[$ata];
		if($GLOBALS['fxpglobals']['person'] == $p)
		{
			if(isset($abs_app[$ata]))
				$lnk=222;
			else
				$lnk=27;
		}
		else if(isset($abs_app[$ata]))
		{
			if($slaves[0] || $slaves[$p])
				$lnk=222;
			else
				$txt=$ta[0];
		}
		else if($ata == 187)
			$txt=$ta[2];
		else
			$txt=$ta[1];
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['terminart']='~<font class="'.$tc.'">'.$txt.'</font>';

		// Betreff + Icon
		if($lnk)
		{
			if($t['erledigt'])
				$akt=1;
			else
				$akt=3;
			if(!isset($GLOBALS['ltt_'.$lnk]))
				$GLOBALS['ltt_'.$lnk]='<b>'.meldung(1697,false).'</b>: '.trPath($lnk);	// Sprung zu
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['name_termin']='~'.$t['name_termin'];
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['icon_ok']='~'.fieldLinkImage('lnk'.$z, 'icln', '?Personen_ID='.$p.'&Termin_ID='.$t['termin_id'].'&aktion='.$akt, $GLOBALS['ltt_'.$lnk], $lnk);
		}
		else
		{
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['name_termin']='~<i class="lightgrey">---</i>';
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['icon_ok']='~|';
		}

		$z++;
	}

	// Last line with Top Border?
	if($z)
	{
		fxf_maskColumnBorder($fa, 1, -1, $mc, $z, true);
		$z++;
	}
}
?>