<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : benutzerstatus_pre.inc                                       //
// Version     : 21.2                                                         //
// Begin       : 2020-10-26                                                   //
// Last Change : 2021-04-08                                                   //
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
 * PF 79: User Accounts
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

$index=true;

// Maskenüberschrift setzen je nach Modus
$maske213_head=$modtitle;

// Felder ausblenden
if(!$Modus)
{
	$Checkbox_nv=true;
	$Button_Multi_nv=true;
}

// NoWrap Felder
$Name_Benutzer_alias_nw=true;
$Datum_nw=true;
$Status_Benutzer_nw=true;

// Filter
$flt=filter_person('and', true, false, 'Personen_ID');
$maske213_flt=$mskfilter;

// SQL
$sql  = "SELECT b.benutzer_id, b.name_benutzer, b.status_benutzer, p.personen_id, p.pname, p.vorname, p.personentyp, p.geschlecht, p.inaktiv, m.maeintrittsdatum, m.maaustrittsdatum";
$sql .= " FROM personen p, benutzer b LEFT JOIN madaten m ON b.personen_id=m.personen_id AND b.mandanten_id=m.mandanten_id";
$sql .= " WHERE b.personen_id=p.personen_id AND b.benutzer_id>1 AND b.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND p.mandanten_id=".$GLOBALS['fxpglobals']['client'];
if(strlen($flt))
	$sql .= " ".$flt;

switch($Modus)
{
	case 2:	// Sperre aufheben
		$sql .= " AND b.status_benutzer=3";
	break;

	case 3:	// Benutzer aktivieren
		$sql .= " AND b.status_benutzer=2";
	break;

	case 1:	// Passwort generieren
	case 4:	// Benutzer deaktivieren
		$sql .= " AND (b.status_benutzer IS NULL OR b.status_benutzer=0 OR b.status_benutzer=1)";
	break;
}
$sql .= " ORDER BY p.pname, p.vorname, b.name_benutzer";
$larr=db_values($sql, '*');
fxDebug($larr, $sql, 2);

if(fxIsArray($larr))
{
	$mc=$GLOBALS['_maskcounter'];
	$inakt=' <i class="lightergrey">{'.strtolower(get_text(1391, '20')).'}</i>';	// {inaktiv}

	// 1592: Status-Benutzer=0: aktiv (mit eigenem Passwort)
	// 1593: Status-Benutzer=1: aktiv (mit automatisch vom Administrator zugewiesenem Passwort)
	// 1594: Status-Benutzer=2: inaktiv
	// 1595: Status-Benutzer=3: gesperrt
	$sbm=array(1592=>quot2apos(meldung(1592, true)), 1593=>quot2apos(meldung(1593, true)), 1594=>quot2apos(meldung(1594, true)), 1595=>quot2apos(meldung(1595, true)));

	foreach($larr as $i => $l)
	{
		$GLOBALS['fxptdata']['lvalues'][$mc][$i]=$l;
		$away=false;
		$cls='';
		if(!strlen($l['maeintrittsdatum']) || (strlen($l['maeintrittsdatum']) && (substr($l['maeintrittsdatum'],0,8) > $GLOBALS['date'])) || (strlen($l['maaustrittsdatum']) && (substr($l['maaustrittsdatum'],0,8) < $GLOBALS['date'])))
		{
			$away=true;
			if(!strlen($cls))
				$cls='red';
		}
		if(!strlen($cls) && (($l['status_benutzer'] == FXP_USER_INACTIVE) || $l['inaktiv']))
			$cls='grey';
		if(!strlen($cls) && ($l['status_benutzer'] == FXP_USER_LOCKED))
			$cls='red';
		if(!strlen($cls))
			$cls='black';

		// Checkbox
		$cb='';
		if($err && fxIsArray($_POST['Checkbox']) && fxIsArray($_POST['Checkbox'][$mc]) && $_POST['Checkbox'][$mc][$i])
			$cb='1';
		$GLOBALS['fxptdata']['lvalues'][$mc][$i]['checkbox']='^class_?|'.$cb;

		// Benutzername
		$pb=person_pic($l['personen_id'], 20, $l['geschlecht'], $GLOBALS['fxpglobals']['client'], false);
		$nb='<font class='.$cls.'>'.$l['pname'];
		if(strlen($l['vorname']))
			$nb .= ', '.$l['vorname'];
		$nb .= '</font> [<b class='.$cls.'>'.strtolower($l['name_benutzer']).'</b>]';
		if($l['inaktiv'])
			$nb .= $inakt;
		$GLOBALS['fxptdata']['lvalues'][$mc][$i]['name_benutzer_alias']='~'.$pb.'&nbsp;'.$nb.fieldHidden('nb['.$mc.']['.$i.']',$nb).fieldHidden('nbs['.$mc.']['.$i.']',$l['name_benutzer']);

		// Datum
		$GLOBALS['fxptdata']['lvalues'][$mc][$i]['datum']='~';
		if(!strlen($l['maeintrittsdatum']))
			$GLOBALS['fxptdata']['lvalues'][$mc][$i]['datum'] .= '<font class="red">???</font>';
		else
		{
			if($away)
				$GLOBALS['fxptdata']['lvalues'][$mc][$i]['datum'] .= '<font class="red">';
			$GLOBALS['fxptdata']['lvalues'][$mc][$i]['datum'] .= get_entry($l['maeintrittsdatum'], 'datum');
			if($l['maaustrittsdatum'])
				$GLOBALS['fxptdata']['lvalues'][$mc][$i]['datum'] .= '&nbsp;-&nbsp;'.get_entry($l['maaustrittsdatum'], 'datum');
			if($away)
				$GLOBALS['fxptdata']['lvalues'][$mc][$i]['datum'] .= '</font>';
		}

		// Benutzerstatus
		if(!$l['status_benutzer'])
			$l['status_benutzer']=0;
		$GLOBALS['fxptdata']['lvalues'][$mc][$i]['status_benutzer']='~'.$sbm[1592+$l['status_benutzer']];
	}

	// Leerzeile mit Top Border?
	if($i)
	{
		$i++;
		fxf_maskColumnBorder(array('Checkbox', 'Name_Benutzer_alias', 'Datum', 'Status_Benutzer'), 1, -1, $mc, $i, true);
	}
}
?>