<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : rechte_uebersicht_pre.inc                                    //
// Version     : 24.1                                                         //
// Begin       : 2020-12-23                                                   //
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
 * PF 157: Authorisation Overview
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

$groups=get_benutzergruppen($_POST['Benutzer_ID'], $GLOBALS['fxpglobals']['client'], true);
if(!$groups)
	$groups='0';
else
	$groups='0,'.$groups;
//fxDebug($groups, 'Benutzergruppen', 0);

// Personeninfo ermitteln
$abt='0';
$sql="SELECT personen_id FROM benutzer WHERE benutzer_id=".$_POST['Benutzer_ID']." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
$pid=(int)db_value($sql);
if($pid)
{
	// Zugeordnete Abteilung ermitteln
	$sql="SELECT pname, vorname, geschlecht, abt_id FROM personen WHERE personen_id=".$pid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$pinfo=db_values($sql);
	if(fxIsArray($pinfo) && $pinfo['abt_id'])
		$abt .= ','.$pinfo['abt_id'];

	// Abteilungen ermitteln bei denen die Person Abteilungsleiter ist
	$sql="SELECT abt_id FROM abteilung WHERE abt_hperson=".$pid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$al=db_values($sql, '*');
	if(fxIsArray($al))
	{
		foreach($al as $a)
			$abt .= ','.$a['abt_id'];
	}
}
//fxDebug($abt, 'Abteilungen');

// Alle Referenz ermitteln
$ref_txt=array();
$rr='';
if(!$_POST['Rechte_Ref'])
{
	$sql="SELECT wertetabid, tabwert FROM wertetab WHERE id_feld=1311 AND id_sprache=".$GLOBALS['fxpglobals']['lang']." AND mandanten_id=0 ORDER BY position";
	$rra=db_values($sql, '*');
	if(fxIsArray($rra))
	{
		foreach($rra as $r)
		{
			$ref_txt[$r['wertetabid']]=$r['tabwert'];
			$rr .= ','.$r['wertetabid'];
		}
		$rr=substr((string)$rr, 1);
	}
}
else
{
	$rr=$_POST['Rechte_Ref'];
	$ref_txt[$_POST['Rechte_Ref']]=meldung($_POST['Rechte_Ref'], false);
}

// Alle zu den Referenzen zugeordneten Objekte ermitteln
if(strlen((string)$rr))
{
	$sqlz  = " FROM rechte_referenz WHERE rechte_ref IN (".$rr.") AND mandanten_id=".$GLOBALS['fxpglobals']['client']." AND (";
	// Benutzer
	$sqlz .= "(gruppeart=2163 AND benutzergr_id=".$_POST['Benutzer_ID'].")";
	// Benutzergruppe
	$sqlz .= " OR (gruppeart=2164 AND benutzergr_id IN (".$groups."))";
	// Abteilung
	$sqlz .= " OR (gruppeart=2165 AND benutzergr_id IN (".$abt."))";
	$sqlz .= ")";

	$sql="SELECT rechte_ref, id_referenz, gruppeart, aktionscode".$sqlz;
	$sql_id="SELECT id_referenz".$sqlz;

	$obj=db_values($sql, '*');
}
else
	$obj=false;
//fxDebug($obj, $sql, 0);

if(fxIsArray($obj))
{
	$bga_txt=array();
	$sql="SELECT wertetabid, tabwert FROM wertetab WHERE id_feld=139 AND id_sprache=".$GLOBALS['fxpglobals']['lang']." AND mandanten_id=0";
	$bga=db_values($sql, '*');
	if(fxIsArray($bga))
	{
		foreach($bga as $bg)
			$bga_txt[$bg['wertetabid']]=$bg['tabwert'];
	}

	// Typ-Icons
	$dt_icons=getDocIcons();

	// Mask fields
	$fa=fxf_getMaskFields(359);
//fxDebug($fa,'$fa', 0);

	// Line height
	$Bezeichnung_height=20;

	$mc=$GLOBALS['_maskcounter'];
	$z=0;

	foreach($ref_txt as $ref => $rtxt)
	{
		$ids=$sql_id." AND rechte_ref=".$ref;
		$ag=array();
		foreach($obj as $o)
		{
			if($o['rechte_ref'] == $ref)
			{
				$ag[$o['id_referenz']]['ac'] |= $o['aktionscode'];
				$ag[$o['id_referenz']]['ga']=$o['gruppeart'];
			}
		}
//fxDebug($ag, $ids, 0);

		if(fxIsArray($ag))
		{
			// Finalen SQL ermitteln
			$bez='';
			if($ref == 35)		// Reports
			{
				$sql="SELECT DISTINCT repid AS id, repname AS name FROM repliste WHERE repid IN (".$ids.") AND mandanten_id=".$GLOBALS['fxpglobals']['client']." ORDER BY repname";
				$bez=db_values($sql, '*');
			}
			else if($ref == 37)	// Vorgänge
			{
				$sql="SELECT DISTINCT projekt_id AS id, vorgangsnummer AS nr, name_projekt AS name, projektart AS pa  FROM projekte WHERE projekt_id IN (".$ids.") AND mandanten_id=".$GLOBALS['fxpglobals']['client']." ORDER BY vorgangsnummer";
				$bez=db_values($sql, '*');
				if(fxIsArray($bez))
				{
					foreach($bez as $bc => $bv)
						$bez[$bc]['name']='<img src="'.$GLOBALS['gfxpath'].'pt'.$bv['pa'].'.png" width="16" height="16" align="top" style="margin-top:2px;">&nbsp;'.no_null($bv['nr']).'&nbsp;-&nbsp;'.$bv['name'];
				}
			}
			else if($ref == 38)	// Dokumente
			{
				$sql  = "SELECT DISTINCT dok_id AS id, dok_originalname AS name FROM d_dokumente WHERE dok_id IN (".$ids.") AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
				$sql .= " AND dok_art<>2579";	// E-Mails ausschließen
				$sql .= " ORDER BY dok_originalname";
				$bez=db_values($sql, '*');
			}
//fxDebug($bez, $sql, 0);

			if(fxIsArray($bez))
			{
				// Divider with Top Border?
				if($z)
				{
					fxf_maskColumnBorder($fa, 1, -1, $mc, $z, true);
					$z++;
				}

				fxf_listSectHeader($mc, $z, '&nbsp;'.$rtxt, 'bezeichnung', 'loeschen', 5);
				$z++;

				foreach($bez as $b)
				{
					if($ref == 35)		// Reports
						$pre='~<img src="'.$GLOBALS['gfxpath'].'rep_b_16x16.png" width="16" height="16" align="top" style="margin-top:2px;">&nbsp;';
					else if($ref == 38)	// Dokument
					{
						// Suffix ermitteln
						$pktpos=strrpos($b['name'], '.');
						if($pktpos)
						{
							$suffix=strtolower(substr((string)$b['name'], $pktpos+1, 3));
							if(!strlen((string)$dt_icons[$suffix]))
								$suffix='?';
						}
						else
							$suffix='?';

						$pre='~<img src="'.$GLOBALS['gfxpath'].'dt_'.$dt_icons[$suffix].'.png" width="16" height="16" align="top" style="margin-top:2px;">&nbsp;';
					}
					else
						$pre='~';

					// Bezeichnung
					$GLOBALS['fxptdata']['lvalues'][$mc][$z]['bezeichnung']=$pre.$b['name'];
					if($bga_txt[$ag[$b['id']]['ga']])
						$GLOBALS['fxptdata']['lvalues'][$mc][$z]['bezeichnung'] .= '&nbsp;<i class="s1 grey">('.$bga_txt[$ag[$b['id']]['ga']].')</i>';

					// Rechte...
					$code=$ag[$b['id']]['ac'];
					// ...Anzeigen
					if($code & 2)
						$img='1';
					else
						$img='0';
					$GLOBALS['fxptdata']['lvalues'][$mc][$z]['anzeigen']='~<img src="'.$GLOBALS['gfxpath'].$img.'.png" style="margin-top:4px;">';
					// ...Anlegen
					if($code & 4)
						$img='1';
					else
						$img='0';
					$GLOBALS['fxptdata']['lvalues'][$mc][$z]['anlegen']='~<img src="'.$GLOBALS['gfxpath'].$img.'.png" style="margin-top:4px;">';
					// ...Ändern
					if($code & 8)
						$img='1';
					else
						$img='0';
					$GLOBALS['fxptdata']['lvalues'][$mc][$z]['aendern']='~<img src="'.$GLOBALS['gfxpath'].$img.'.png" style="margin-top:4px;">';
					// ...Löschen
					if($code & 16)
						$img='1';
					else
						$img='0';
					$GLOBALS['fxptdata']['lvalues'][$mc][$z]['loeschen']='~<img src="'.$GLOBALS['gfxpath'].$img.'.png" style="margin-top:4px;">';

					$z++;
				}
			}
		}
	}

	// Last line with Top Border?
	if($z)
	{
		fxf_maskColumnBorder($fa, 1, -1, $mc, $z, true);
		$z++;
	}
}
?>