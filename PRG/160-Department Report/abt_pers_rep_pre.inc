<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : abt_pers_rep_pre.inc                                         //
// Version     : 21.1                                                         //
// Begin       : 2020-10-26                                                   //
// Last Change : 2020-10-26                                                   //
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
 * PF 100: Profile Overview
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.1
 */

$maske365_nch=true;

// Abteilungen ermitteln
$aidcsv="";
if($_POST['Abt_ID'])
{
	$zua=(int)$_POST['Abt_ID'];
	$aid_done=array($zua => true);
	$aidcsv=$zua;

	// Unterabteilung finden
	while(true)
	{
		$sql="SELECT abt_id FROM abteilung WHERE zu_abteilung IN (".$zua.") AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$zua='';
		$tmp=db_values($sql, '*');
		if(fxIsArray($tmp))
		{
			foreach($tmp as $t)
			{
				$zai=(int)$t['abt_id'];
				if($zai && !$aid_done[$zai])
				{
					if(strlen($zua))
						$zua .= ",";
					$zua .= $zai;

					$aidcsv .= ",".$zai;

					$aid_done[$zai]=true;
				}
			}
		}

		if(!strlen($zua))
			break;
	}
}

$sadd='';
if(strlen($aidcsv))
	$sadd .= " AND abt_id IN (".$aidcsv.")";
if($_POST['Personen_ID'])
{
	$pai=db_value("SELECT abt_id FROM personen WHERE personen_id=".$_POST['Personen_ID']." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
	if($pai)
		$sadd .= " OR abt_id=".$pai;
	$sadd .= " OR abt_hperson=".$_POST['Personen_ID'];
}
$abt=getDepStructure($sadd);
if(fxIsArray($abt))
{
fxDebug($abt, 'Abteilungen', 2);
	$linakt='&nbsp;<i class="lightergrey">{'.strtolower(get_text(1391, '20')).'}</i>';	// {inaktiv}

	// Alle Personen ermitteln
	$sql  = "SELECT DISTINCT(p.personen_id), p.mandanten_id, p.abt_id, p.pname, p.vorname, p.geschlecht, p.inaktiv FROM personen p, madaten m";
	$sql .= " WHERE p.personen_id=m.personen_id AND p.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND m.mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$flt=filter_person('and', true, false, 'Dummy');
	$maske365_flt=$mskfilter;
	if(strlen($flt))
		$sql .= " AND (((m.maaustrittsdatum IS NULL OR m.maaustrittsdatum>'".fxNow()."') ".$flt.") OR p.personen_id IN (SELECT abt_hperson FROM abteilung WHERE mandanten_id=".$GLOBALS['fxpglobals']['client']."))";
	else
		$sql .= " AND (m.maaustrittsdatum IS NULL OR m.maaustrittsdatum>'".fxNow()."')";
	$sql .= " ORDER BY p.pname, p.vorname";
	$prs=db_values($sql, '*');
	if(fxIsArray($prs))
	{
fxDebug($prs, $sql, 2);
		$hod_title=meldung(211, false);	// Abteilungsleiter(in)
		$pinab=array();

		// Alle Abteilungen durchgehen
		$mc=$GLOBALS['_maskcounter'];
		$z=0;
		foreach($abt as $a)
		{
			// Leerzeile mit Top Border?
			if($z)
			{
				fxf_maskColumnBorder(array('pers_abt'), 1, -1, $mc, $z, true);
				$z++;
			}

			$pf=0;

			// Abteilung
			$txt='&nbsp;<font class="s4b">'.$a['abt_ebene'].':</b>&nbsp;<font class="s4b grey">'.$a['abt_name'].'</font>';
			fxf_listSectHeader($mc, $z++, $txt, 'pers_abt', '', 1, -2, 1);

			// Abteilungsleiter
			if($a['abt_hperson'])
			{
				foreach($prs as $p)
				{
					if($p['personen_id'] == $a['abt_hperson'])	// gefunden
					{
						$pinab[$p['personen_id']]=true;

						$pre='';
						if($p['personen_id'] == $_POST['Personen_ID'])
							$pre='^col_3db38c|';

						$cls='black';
						if($p['personen_id'] == $GLOBALS['fxpglobals']['person'])
							$cls='blue';
						else if($p['inaktiv'])
							$cls='grey';

						$txt=person_pic($p['personen_id'], 20, $p['geschlecht'], $GLOBALS['fxpglobals']['client'], false).'&nbsp;<u class="'.$cls.'">'.$p['pname'];
						if(strlen(trim($p['vorname'])))
							$txt .= ',&nbsp;'.trim($p['vorname']);
						$txt .= '</u>';
						if($p['inaktiv'])
							$txt .= $linakt;
						$txt .= '&nbsp;&nbsp;<font class="s2 darkgrey">['.$hod_title.']</font>';

						$GLOBALS['fxptdata']['lvalues'][$mc][$z++]['pers_abt']=$pre.'~'.$txt;
						$pf++;
						break;
					}
				}
			}
			// Personen
			foreach($prs as $p)
			{
				if($pinab[$p['personen_id']])
					continue;
				if(($p['personen_id'] != $a['abt_hperson']) && ($p['abt_id'] == $a['abt_id']))	// gefunden
				{
					$pinab[$p['personen_id']]=true;

					$pre='';
					if($p['personen_id'] == $_POST['Personen_ID'])
						$pre='^col_3db38c|';

					$cls='black';
					if($p['personen_id'] == $GLOBALS['fxpglobals']['person'])
						$cls='blue';
					else if($p['inaktiv'])
						$cls='grey';

					$txt='&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;'.person_pic($p['personen_id'], 20, $p['geschlecht'], $GLOBALS['fxpglobals']['client'], false).'&nbsp;<font class="'.$cls.'">'.$p['pname'];
					if(strlen(trim($p['vorname'])))
						$txt .= ',&nbsp;'.trim($p['vorname']);
					$txt .= '</font>';
					if($p['inaktiv'])
						$txt .= $linakt;

					$GLOBALS['fxptdata']['lvalues'][$mc][$z++]['pers_abt']=$pre.'~'.$txt;
					$pf++;
				}
			}

			// Keine Person zugeordnet?
			if(!$pf)
			{
				if(!isset($pft))
					$pft=meldung(1691,true);	// Keine Mitarbeiter zugeordnet!
				$GLOBALS['fxptdata']['lvalues'][$mc][$z++]['pers_abt']='~<i class="lightergrey">('.$pft.')</i>';
			}
		}

		// Sind Personen in keiner Abteilung
		// ... Personen rausfiltern, welche zu einer Abteilung gehören, jedoch nicht in obiger Auswahl stehen
		foreach($prs as $p)
		{
			if($p['abt_id'] > 0)
				$pinab[$p['personen_id']]=true;
		}
		// ... Personen rausfiltern, welche Abteilungsleiter sind, jedoch nicht in obiger Auswahl stehen
		$sql="SELECT abt_hperson FROM abteilung WHERE mandanten_id=".$GLOBALS['fxpglobals']['client']." AND abt_hperson IS NOT NULL AND abt_hperson>0";
		$abl=db_values($sql, '*');
fxDebug($abl, $sql, 2);
		if(fxIsArray($abl))
		{
			foreach($abl as $a)
				$pinab[$a['abt_hperson']]=true;
		}
		// ... Rest gehört zu keiner Abteilung
		if(sizeof($pinab) < sizeof($prs))
		{
			$ohnetitel=false;

			// Personen
			foreach($prs as $p)
			{
				if(!$pinab[$p['personen_id']])	// gefunden
				{
					if(!$ohnetitel)
					{
						$ohnetitel=true;

						// Leerzeile mit Top Border?
						if($z)
						{
							fxf_maskColumnBorder(array('pers_abt'), 1, -1, $mc, $z, true);
							$z++;
						}

						// Ohne Abteilung
						$txt='&nbsp;<font class="s4b red">'.meldung(308, true).'</font>';
						fxf_listSectHeader($mc, $z++, $txt, 'pers_abt', '', 1, -2, 1);
					}

					$pinab[$p['personen_id']]=true;

					$pre='';
					if($p['personen_id'] == $_POST['Personen_ID'])
						$pre='^col_3db38c|';

					$cls='black';
					if($p['personen_id'] == $GLOBALS['fxpglobals']['person'])
						$cls='blue';
					else if($p['inaktiv'])
						$cls='grey';

					$txt=person_pic($p['personen_id'], 20, $p['geschlecht'], $GLOBALS['fxpglobals']['client'], false).'&nbsp;<font class="'.$cls.'">'.$p['pname'];
					if(strlen(trim($p['vorname'])))
						$txt .= ',&nbsp;'.trim($p['vorname']);
					$txt .= '</font>';
					if($p['inaktiv'])
						$txt .= $linakt;

					$GLOBALS['fxptdata']['lvalues'][$mc][$z++]['pers_abt']=$pre.'~'.$txt;
				}
			}
		}

		// Leerzeile mit Top Border?
		if($z)
		{
			fxf_maskColumnBorder(array('pers_abt'), 1, -1, $mc, $z, true);
			$z++;
		}
	}
}
?>