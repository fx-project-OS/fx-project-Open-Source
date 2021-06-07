<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : ma_planung_pre.inc                                           //
// Version     : 21.1                                                         //
// Begin       : 2020-10-29                                                   //
// Last Change : 2020-11-16                                                   //
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
 * PF 28: Ressourceplanner
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.1
 */

$waehrung=' '.$GLOBALS['fxpglobals']['settings']['waehrung'];

if(($GLOBALS['fxpglobals']['tr'] == 23) || (($GLOBALS['fxpglobals']['tr'] == 28) && $Button_sort && !$err))
{
	$entries_all=0;
	$entries_max=0;
	if(!fxIsArray($liste))
	{
		// Alle Mitarbeiter gewählt => "alle" aus Array lesen (abhängig von Abtlg. und Teams)
		if(!isset($_POST['Personen_ID']) || !$_POST['Personen_ID'] || fxIsArray($_POST['Personen_ID']))
		{
			// Als SQL
			$Personenauswahl=auswahl_Personen_ID(-1);
			if(!strlen($Personenauswahl))
				$Personenauswahl='-1';
		}
		else if($_POST['Personen_ID'])
			$Personenauswahl=$_POST['Personen_ID'];

		if(strlen($zp_v) && strlen($zp_b))
			$zeitraum=" AND (mp.soll_beginn<='".$zp_b."' AND mp.soll_ende>='".$zp_v."')";
		else if(strlen($zp_v))
			$zeitraum=" AND mp.soll_beginn>='".$zp_v."'";
		else if(strlen($zp_b))
			$zeitraum=" AND mp.soll_ende<='".$zp_b."'";
		else
			$zeitraum="";

		$sqla  = " FROM personen p, projekte pr, maplanung mp, artikelzuordnung az";
		$sqla .= " WHERE mp.personen_id=p.personen_id AND mp.maplanung_id=az.maplanung_id AND mp.projekt_id=pr.projekt_id";
		$sqla .= " AND mp.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND p.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND az.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND pr.mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$sqla .= " AND p.personentyp IN (275,840) AND p.personen_id IN (".$Personenauswahl.")".$zeitraum.$Projektauswahl;

		$sql="SELECT COUNT(mp.maplanung_id)".$sqla;
		$entries_all=(int)db_value($sql);
fxDebug($entries_all,$sql, 3);

		$sql  = "SELECT mp.maplanung_id, pr.name_projekt, pr.vorgangsnummer,";
		$sql .= " mp.maart_id, mp.mafaehigkeit_id, mp.aufwand_std, mp.soll_beginn AS plan_beg_dtm, mp.soll_ende AS plan_end_dtm, (mp.auslastung * 100) AS auslastung, (mp.prozent * 100) AS prozent, mp.maplanung_id,";
		$sql .= " p.personen_id, p.pname, p.vorname, p.geschlecht,";
		$sql .= " az.ek_projekt, az.vk_projekt";
		$sql .= $sqla." ORDER BY pr.vorgangsnummer, p.pname, p.vorname";
		$liste=db_values($sql,0,$GLOBALS['fxpglobals']['settings']['maxliste']-1);
fxDebug($liste,$sql, 3);
	}
	else
		$entries_all=sizeof($liste);
	if(fxIsArray($liste))
	{
		$entries_max=sizeof($liste);
//echo('<b>$entries_all</b>='.$entries_all.', <b>$entries_max</b>='.$entries_max.'<hr />');

		// Maskenüberschrift anpassen?
		if($entries_max < $entries_all)
		{
			// 1743: Zu viele Listeneinträge ($var1) gefunden! Entsprechend den aktuellen Einstellungen wird die Liste nach maximal $var2 Einträgen abgebrochen. Bitte schränken Sie die Auswahlkriterien ein oder kontaktieren Sie Ihren Administrator.
			$tt=meldung(1743,true, '<b class=red>'.$entries_all.'</b>', '<b>'.$entries_max.'</b>');
			$maske27_head=get_text(27,'kurz',0,2).'<div tooltip="'.$tt.'" style="float:right;cursor:help;">'.$entries_max.' <font class="normal lightgrey">/</font> <b class="red">'.$entries_all.'</b></div>';
		}

		// Diese Felder ausblenden
		$Name_Sachmittel_nv=true;
		$Sachmitteltyp_nv=true;

		// Diese Felder ausblenden, falls keine Einträge gefunden werden
		$art_cnt=0;
		$faehigkeit_cnt=0;

		// Alle Gruppen und Fähigkeiten in Klartext
		$GruFae=auswahl_maart_id(1);

		// Prüfen, ob Gruppe oder Fähigkeit angezeigt werden sollen
		foreach($liste as $lw)
		{
			// Leereinträge (Inkosistenzen!) filtern
			if(!$lw['name_projekt'] || !$lw['vorgangsnummer'])
				continue;

			// Gruppe
			if($GruFae[$lw['maart_id']])
				$art_cnt++;
			// Fähigkeit
			if($GruFae[$lw['mafaehigkeit_id'].'f'])
				$faehigkeit_cnt++;
		}
		$cs=10;
		if(!$art_cnt)
		{
			$MaArt_Name_nv=true;
			$cs--;
		}
		if(!$faehigkeit_cnt)
		{
			$MaFaehigkeit_Name_nv=true;
			$cs--;
		}

		$mc=$GLOBALS['_maskcounter'];
		$l=0;
		$pnr='';

		$fa=array('Pname', 'Name_Sachmittel', 'Sachmitteltyp', 'MaArt_Name', 'MaFaehigkeit_Name', 'Plan_Beg_Dtm', 'Plan_End_Dtm', 'EK_Projekt', 'VK_Projekt', 'Aufwand_Std', 'prozent', 'Auslastung');

		foreach($liste as $lw)
		{
			// Leereinträge (Inkosistenzen!) filtern
			if(!$lw['name_projekt'] || !$lw['vorgangsnummer'])
				continue;

			// Vorgang
			if($lw['vorgangsnummer'] != $pnr)
			{
				// Leerzeile
				if($l)
				{
					fxf_maskColumnBorder($fa, 1, -1, $mc, $l, true);
					$l++;
				}

				$pnr=$lw['vorgangsnummer'];
				$txt='&nbsp;'.no_null($pnr).'&nbsp;<b>'.$lw['name_projekt'].'</b>&nbsp;';
				fxf_listSectHeader($mc, $l++, $txt, 'pname', 'auslastung', $cs);
			}

			// Name
			$GLOBALS['fxptdata']['lvalues'][$mc][$l]['pname']='~'.person_pic($lw['personen_id']);
			// Gruppe
			if($GruFae[$lw['maart_id']])
				$GLOBALS['fxptdata']['lvalues'][$mc][$l]['maart_name']=$GruFae[$lw['maart_id']];
			// Fähigkeit
			if($GruFae[$lw['mafaehigkeit_id'].'f'])
				$GLOBALS['fxptdata']['lvalues'][$mc][$l]['mafaehigkeit_name']=$GruFae[$lw['mafaehigkeit_id'].'f'];
			// Beginn
			$GLOBALS['fxptdata']['lvalues'][$mc][$l]['plan_beg_dtm']='~'.get_entry($lw['plan_beg_dtm'], 'datum');
			// Ende
			$GLOBALS['fxptdata']['lvalues'][$mc][$l]['plan_end_dtm']='~<font class="darkgrey">'.get_entry($lw['plan_end_dtm'], 'datum').'</font>';
			// EK
			$GLOBALS['fxptdata']['lvalues'][$mc][$l]['ek_projekt']='~'.get_entry($lw['ek_projekt'], "betrag").$waehrung;
			// VK
			$GLOBALS['fxptdata']['lvalues'][$mc][$l]['vk_projekt']='~'.get_entry($lw['vk_projekt'], "betrag").$waehrung;
			// Aufw.
			$GLOBALS['fxptdata']['lvalues'][$mc][$l]['aufwand_std']='~'.get_entry($lw['aufwand_std']/3600, 'dezimal').'&nbsp;h';
			// Prozent
			if(!$lw['prozent'])
				$lw['prozent']=0.00;
			$GLOBALS['fxptdata']['lvalues'][$mc][$l]['prozent']='~'.get_entry($lw['prozent'], "dezimal").' %';
			// Auslastung
			if(strlen($lw['auslastung']))
			{
				if(($lw['auslastung'] > 0.00) && ($lw['auslastung'] < 5.00))
					$ag='010';
				else if($lw['auslastung'] >= 5.00)
				{
					$ag=substr('000'.round($lw['auslastung']/10)*10, -3);
					if($ag > 200)
						$ag='_x';
				}
				else
					$ag='000';
			}
			else
				$ag='_ud';
//echo('Auslastung: '.$lw['auslastung'].' -> '.$ag.'<BR>');
			if($lw['auslastung'] > 100)
				$GLOBALS['fxptdata']['lvalues'][$mc][$l]['auslastung']='~<b class="red">'.get_entry($lw['auslastung'], "dezimal").' %</b>';
			else
				$GLOBALS['fxptdata']['lvalues'][$mc][$l]['auslastung']='~<b>'.get_entry($lw['auslastung'], "dezimal").' %</b>';
			$GLOBALS['fxptdata']['lvalues'][$mc][$l]['auslastung'] .= '&nbsp;<img src="'.$GLOBALS['gfxpath'].'au'.$ag.'.png" width="10" height="18" align="center" style="border:1px solid #dddddd;border-radius:4px;">';

			$l++;
		}

		// Leerzeile
		if($l)
		{
			fxf_maskColumnBorder($fa, 1, -1, $mc, $l, true);
			$l++;
		}

		// ...
		if($entries_max < $entries_all)
		{
			fxf_listSectHeader($mc, $l++, '<br /><div style="text-align:center;">...</div>', 'pname', 'auslastung', $cs, -99);
			fxf_maskColumnBorder($fa, 1, -1, $mc, $l, true);
			$l++;
		}
	}
}

// No wrap
$Pname_nw=true;
$Name_Sachmittel_nw=true;
$Sachmitteltyp_nw=true;
$Plan_Beg_Dtm_nw=true;
$Plan_End_Dtm_nw=true;
?>