<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : zk_aend_list_pre.inc                                         //
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

setForm(true);

// Indizierung
if(!isset($index))
	$index=true;

// NoWrap
$Datum_nw			= true;
$Vorgangsnummer_nw	= true;
$Kunde_alias_nw		= true;
$Taetigkeit_Text_nw	= true;
$Zeitaufwand_nw		= true;

// SQL-Parameter
$pids="";
$with_unknown=false;
$pv=(int)$_POST['fuerVorgang'];
if($pv == -1)
{
	$pids="z.projekt_id=0";
	$with_unknown=true;
}
else if(strlen($aufg_sql))
{
	if($pv > 0)
	{
		if(fxIsArray($varray))
		{
			$paa=explode('_',$_POST['fuerVorgang']);
			$pid=(int)$paa[0];
			$kid=-1;
			if(sizeof($paa) > 1)
				$kid=(int)$paa[1];
			$vn=db_value("SELECT vorgangsnummer FROM projekte WHERE projekt_id=".$pid);
			if(strlen($vn))
				$pids .= "z.projekt_id IN (".$aufg_sql." AND p.vorgangsnummer LIKE '".$vn."%')";
			if($kid >= 0)
			{
				if(strlen($vn))
					$pids .= " AND ";
				if(!$kid)
					$pids .= "z.zeit_id NOT IN (SELECT id FROM kostenzuordnung WHERE kostenstelle_id IS NOT NULL AND kostenstellenart=".FXP_TE_TIMEREC." AND kostendefinition=903 AND mandanten_id=".$GLOBALS['fxpglobals']['client'].")";
				else
					$pids .= "z.zeit_id IN (SELECT id FROM kostenzuordnung WHERE kostenstelle_id=".$kid." AND kostenstellenart=".FXP_TE_TIMEREC." AND kostendefinition=903 AND mandanten_id=".$GLOBALS['fxpglobals']['client'].")";
			}
		}
	}
	else
	{
		$pids="z.projekt_id=0 OR z.projekt_id IN (".$aufg_sql.")";
		$with_unknown=true;
	}
}
if(!strlen($pids))
	$pids="z.projekt_id=-666";
//fxDebug($pids, '$pids');

$pers="";
if($_POST['Personen_ID'] > 0)
	$pers="z.personen_id=".$_POST['Personen_ID'];
else if(is_array($parray))
	$pers="z.personen_id IN (".$pers_sql.")";
if(!strlen($pers))
	$pers="z.personen_id=-666";
//fxDebug($pers, '$pers');

$sql  = "SELECT z.zeit_id, z.projekt_id, z.mutterprojekt_id, z.zeitaufwand, z.pause, z.personen_id, z.nicht_abrechenbar, z.rk_nicht_abrechenbar,";
$sql .= " z.ist_beginn, z.ist_ende, z.rechnungs_id, z.rechnungs_id_rk, z.freigabe, z.zkunde, z.zeitart, z.zeiteinheit,";
$sql .= " t.taetigkeit_text,";
$sql .= " p.vorgangsnummer, p.name_projekt, p.projekt_status, p.projektart, p.aufwand_soll, p.aufwand_ist, p.projektmanager, p.projektleiter, p.vertreter";
$sql .= " FROM zeitdaten z, taetigkeiten t, projekte p";
$sql .= " WHERE z.zeitaufwand>0 AND z.zeitart IN (".FXP_TE_TIMEREC.",".FXP_TE_TIMEREC_SUM.",".FXP_TE_TRAVELEXP_TIMEREC.") AND z.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND (".$pids.") AND (".$pers.")";
// Auftraggeber
if($_POST['Kunde'])
	$sql .= " AND z.zkunde=".$_POST['Kunde'];
$sql .= " AND (z.ist_beginn<='$edat' AND z.ist_ende>='$sdat')";
$sql .= " AND t.zeit_id=z.zeit_id AND t.mandanten_id=z.mandanten_id AND p.projekt_id=z.projekt_id AND p.mandanten_id IN (0,".$GLOBALS['fxpglobals']['client'].")";
$sql .= " ORDER BY z.ist_beginn, z.projekt_id";
$liste=db_values($sql, '*');
fxDebug($liste, $sql, 2);

if(fxIsArray($liste))
{
	// Variablen initialisieren
	$colspan=12;
	$z=0;
	$kliste=array();
	$rliste=array();
	$mc=$GLOBALS['_maskcounter'];

	// All columns
	$fa=array('Checkbox|0', 'Datum', 'zeitanzeige', 'Vorgangsnummer', 'leistungsarten', 'Aufwaende', 'Projekt_IDopen', 'Taetigkeit_Text', 'Zeitaufwand', 'nicht_abrechenbar', 'Verrechnungs_ID', 'Freigabe', 'Kunde_alias', 'Abrechnungsart');

	// No right border in the following column
	fxf_maskColumnBorder(array('Checkbox'), 0, -1, $mc);

	// Projekt Spaltenbreite
	$pcw=354;

	// Projekt-Status
	$sql="SELECT wertetabid, tabwert FROM wertetab WHERE id_feld=137 AND id_sprache=".$GLOBALS['fxpglobals']['lang']." AND mandanten_id IN (0,".$GLOBALS['fxpglobals']['client'].")";
	$ps_txt=get_array($sql, 'wertetabid', 'tabwert');

	// Abrechnungsarten
	$aa_txt=fxf_getReferenceValues(69);
	$aa_txt[0]='<i>['.meldung(388).']</i>';	// Ohne
//fxDebug($aa_txt,'$aa_txt');
	$aa_arr=array();
	if(strlen($aufg_sql))
	{
		$sql="SELECT projekt_id, abrechnungsart FROM budget_summe WHERE projekt_id IN (".$aufg_sql.") AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$tmp=db_values($sql,'*');
		if(fxIsArray($tmp))
		{
			foreach($tmp as $t)
				$aa_arr[(int)$t['projekt_id']]=(int)$t['abrechnungsart'];
		}
	}
//fxDebug($aa_arr,'$aa_arr');

	// Auswahlfeld Aufgaben vorbereiten
	unset($zarray[-1]);
	$Projekt_IDopen_ds=0;
	$Projekt_IDopen_arg=array('override'=>$zarray);
//fxDebug($zarray,'$zarray');

	// Evtl. zugeordnete Leistungsarten ermitteln
	$zids=array_to_csv($liste, 'zeit_id');
	$sql  = "SELECT z.id, z.kostenstelle_id, s.kostennummer, s.kostenbezeichnung";
	$sql .= " FROM kostenzuordnung z, kostenstellen s";
	$sql .= " WHERE z.id IN (".$zids.") AND z.kostenstellenart=".FXP_TE_TIMEREC." AND z.kostendefinition=903 AND z.mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$sql .= " AND s.kostenstelle_id=z.kostenstelle_id AND s.id_sprache=".$GLOBALS['fxpglobals']['lang']." AND s.mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$la_zug=get_array($sql, 'id');
//fxDebug($la_zug, $sql);
	if(fxIsArray($la_zug))
		$colspan++;
	else
		$leistungsarten_nv=true;

	// Alle Personen durchgehen
	foreach($pinfo as $pdt)
	{
		$id=(int)$pdt['personen_id'];
		$nset=false;
		$olddatum='';

		// Liste durchgehen
		foreach($liste as $lw)
		{
			if($id != $lw['personen_id'])
				continue;

			// Name (über gesamte Zeile)
			if(!$nset)
			{
				$nset=true;

				// Top border with empty line
				if($z)
				{
					fxf_maskColumnBorder($fa, 1, -1, $mc, $z, true);
					$z++;
				}

				$txt='&nbsp;'.person_pic($id, 20, $pdt['geschlecht'], $GLOBALS['fxpglobals']['client'], true, 'border-top-left-radius:6px;').'&nbsp;<font class="s2 grey">'.$pdt['ptyp'].'</font>';

				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['checkbox']='~|';
				fxf_listSectHeader($mc, $z++, $txt, 'datum', 'abrechnungsart', $colspan);

				// Alle aktiven berechtigten Aufgaben der aktuellen Person
				$rliste[$id]=$zarray;
//fxDebug($rliste[$id], $id);
			}

			// Spezialrechte?
			$sr=0;
			if(!$GLOBALS['fxpglobals']['person'])					// ...Sysadmin
				$sr=7;
			else if($GLOBALS['fxpglobals']['user'] <= 1)			// ...Hauptadmin
				$sr=6;
			else if($GLOBALS['fxpglobals']['useradmin'])			// ...Admin
				$sr=5;
			else if($GLOBALS['fxpglobals']['persdat']['roles'][2])	// ...GF
				$sr=4;
			else if($GLOBALS['fxpglobals']['persdat']['roles'][-1] && (strtolower($GLOBALS['fxpglobals']['settings']['vw_darf_zem']) == 'ja'))	// ...VW, falls Mandanteneinstellung "Verwaltung darf Zeiterfassungen für zugeordnete Mitarbeiter durchführen?" gesetzt
				$sr=3;
			else if($lw['projektmanager'] == $GLOBALS['fxpglobals']['person'])
				$sr=2;
			else if(($lw['projektleiter'] == $GLOBALS['fxpglobals']['person']) || ($lw['vertreter'] == $GLOBALS['fxpglobals']['person']))
				$sr=1;

			$changeable=2;

			// Abrechnungsart
			$abr=0;
			if(isset($aa_arr[$lw['projekt_id']]))
				$abr=$aa_arr[$lw['projekt_id']];

			// Freigabe checken
			$frei=0;
			if($lw['freigabe'])
			{
				$frei=1;
				if(($GLOBALS['fxpglobals']['persdat']['role'] < ($lw['freigabe']-1)) && !$GLOBALS['fxpglobals']['useradmin'])
					$changeable=0;
			}

			$rech=false;
			// Rechnung checken
			if($lw['rechnungs_id'] || $lw['rechnungs_id_rk'])
			{
				$rech=true;
				// Abgerechnete SB auf Festpreisaufgaben können immer verändert werden von Personen mit Spezialrechten (s.o.)
				if($lw['rechnungs_id_rk'] || ($abr != FXP_INV_FIX) || !$sr)
					$changeable=0;
			}

			$sum='';
			if($lw['zeitart'] == FXP_TE_TIMEREC_SUM)
			{
				$sum='<font class="grey">&sum;</font>&nbsp;';
				if($changeable)
				{
					if($lw['zeit_id'] != $lw['zeiteinheit'])
						$changeable=1;
					else
					{
						$sql="SELECT COUNT(zeit_id) FROM zeitdaten WHERE zeiteinheit=".$lw['zeit_id']." AND zeit_id<>".$lw['zeit_id']." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
						$dts=(int)db_value($sql);
						if($dts)
							$changeable=1;
					}
				}
			}

			if($lw['projektart'] != FXP_PRJ_TASK)
				$changeable=2;

			// Farbe
			$cls='black';
			if($changeable < 2)
				$cls='grey';

			// Checkbox
			if(!$changeable)
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['checkbox']='~|';
			else if(isset($_POST['Button_Multi']) && !isset($_POST['Button_Anzeigen']))
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['checkbox']=$_POST['Checkbox'][$mc][$z];
			else if(($_POST['Projekt_ID'] != -1) && ($lw['projekt_id'] != $_POST['Projekt_ID']))
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['checkbox']=1;
			else
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['checkbox']='';

			// Datum
			$datum=substr($lw['ist_beginn'], 0, 8).'000000';
			if($olddatum != $datum)
			{
				$olddatum=$datum;
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['datum']='~'.get_entry($datum, 'datum');
			}

			// Zeit
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['zeitanzeige']='~';
			if(!strlen($sum) && ($lw['ist_beginn'] != $lw['ist_ende']))
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['zeitanzeige'] .= get_entry($lw['ist_beginn'], 'zeit').'-'.get_entry($lw['ist_ende'], 'zeit');
			else
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['zeitanzeige'] .= $sum.get_entry($lw['zeitaufwand']+$lw['pause'], 'zeitspanne').'&nbsp;h';

			// Vorg. Nr.
			$bcl='bbbbbb';
			if($lw['projektart'] != FXP_PRJ_TASK)
				$bcl='e87986';
			if($lw['vorgangsnummer'])
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['vorgangsnummer'] ='~<span tooltip="'.$lw['name_projekt'].'" style="border:1px solid #'.$bcl.';border-radius:6px;padding:0 4px;cursor:help;">'.no_null($lw['vorgangsnummer']).'</span><img src="'.$GLOBALS['gfxpath'].'tts.png" align="top">';
			else
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['vorgangsnummer']='~<i class="grey">'.$unbekannt_str.'</i>';

			// Leist.-Art
			if(!isset($leistungsarten_nv) || !$leistungsarten_nv)
			{
				if(fxIsArray($la_zug[$lw['zeit_id']]))
					$GLOBALS['fxptdata']['lvalues'][$mc][$z]['leistungsarten'] ='~&rarr;&nbsp;<span tooltip="'.$la_zug[$lw['zeit_id']]['kostenbezeichnung'].'" class="green" style="border:1px solid #'.$bcl.';border-radius:6px;padding:0 4px;cursor:help;">'.$la_zug[$lw['zeit_id']]['kostennummer'].'</span><img src="'.$GLOBALS['gfxpath'].'tts.png" align="top">';
				else
					$GLOBALS['fxptdata']['lvalues'][$mc][$z]['leistungsarten']='~<i class="grey">'.$ohne_str.'</i>';
			}

			// Aufwand
			if($lw['vorgangsnummer'])
			{
				if(!$lw['aufwand_soll'])
					$acls='lightgrey';
				else if($lw['aufwand_ist'] > $lw['aufwand_soll'])
					$acls='red';
				else
					$acls='green';
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['aufwaende']='~<font class="'.$acls.'">'.get_entry($lw['aufwand_ist']/3600, 'dezimal').'&nbsp;h</font>&nbsp;<font class="lightgrey">/</font>&nbsp;<font class="grey">'.get_entry($lw['aufwand_soll']/3600, 'dezimal').'&nbsp;h</font>';
			}
			else
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['aufwaende']='~<font class="grey">'.get_entry($lw['zeitaufwand']/3600, 'dezimal').'&nbsp;h</font>';

			// Vorgang
			$opid=$lw['projekt_id'];
			if(fxIsArray($la_zug[$lw['zeit_id']]))
				$opid .= '_'.$la_zug[$lw['zeit_id']]['kostenstelle_id'];
			if($changeable < 2)
			{
				$pid=$opid;
				if(!$pid)
					$vtxt=$unbekannt_str;
				else
				{
					if(!isset($zarray[$pid]))
					{
						$zarray[$pid]=no_null($lw['vorgangsnummer']).' - '.$lw['name_projekt'];
						if(($lw['projektart'] == FXP_PRJ_MAIN) || ($lw['projektart'] == FXP_PRJ_SUB))
							$zarray[$pid] .= '<d>';
					}
					$vtxt=$zarray[$pid];
				}
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['projekt_idopen']='~<div class="'.$cls.'" style="width:'.$pcw.'px;padding:0 2px 10px 2px;overflow:hidden;">'.$vtxt.fieldHidden("Projekt_IDopen[".$mc."][".$z."]",$pid).'</div>';
			}
			else
			{
				if(isset($_POST['Button_Multi']) && !isset($_POST['Button_Anzeigen']))
					$pid=$_POST['Projekt_IDopen'][$mc][$z];
				else if($_POST['Projekt_ID'] != -1)
					$pid=$_POST['Projekt_ID'];
				else if(isset($rliste[$id][$opid]))
					$pid=$opid;
				else
					$pid=-1;

				if(!isset($rliste[$id][$pid]))
				{
					$pid=-1;
					$GLOBALS['fxptdata']['lvalues'][$mc][$z]['checkbox']='';
				}
				if(isset($_POST['Button_Anzeigen']) && ($pid == (int)$lw['projekt_id']))
					$GLOBALS['fxptdata']['lvalues'][$mc][$z]['checkbox']='';

				$a_arg_array=array('a_name'=>"Projekt_IDopen[".$mc."][".$z."]", 's_name'=>$rliste[$id], 'a_muss'=>false, 'a_select'=>$pid, 'a_width'=>$pcw, 'a_change'=>false);
				$a_html=convert_array(2, $rliste[$id], $a_arg_array);
//fxDebug($a_html,'$a_html: $pid='.$pid);
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['projekt_idopen']='~'.$a_html;
			}

			// Tätigkeit
			if(strlen($lw['taetigkeit_text']))
			{
				$wrap=false;
				if($_POST['Zeilenumbruch'])
					$wrap=true;
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['taetigkeit_text']='~<font class="'.$cls.'">'.text_wrap($lw['taetigkeit_text'], $wrap, $_POST['text_kuerzen']).'</font>';
			}

			// Zeit
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['zeitaufwand']='~<font class="'.$cls.'">'.get_entry($lw['zeitaufwand'], 'zeitspanne').'&nbsp;h</font>';

			// n.v. (nicht verrechenbar)
			if(!$changeable)
			{
				if($lw['nicht_abrechenbar'] || !$lw['projekt_id'])
				{
					$img='tl_red';
					if($GLOBALS['fxpglobals']['lang'] == 1)
						$tt='Nicht verrechenbar';
					else
						$tt='Not invoiceable';
				}
				else
				{
					$img='tl_green';
					if($GLOBALS['fxpglobals']['lang'] == 1)
						$tt='Verrechenbar';
					else
						$tt='Invoiceable';
				}
				$verr='~<img src="'.$GLOBALS['gfxpath'].$img.'.png" tooltip="'.$tt.'" style="cursor:help;">';
				if($lw['nicht_abrechenbar'])
					$verr .= fieldHidden('nicht_abrechenbar['.$mc.']['.$z.']',1);
			}
			else
			{
				$verr='';
				if(isset($_POST['Button_Multi']) && !isset($_POST['Button_Anzeigen']))
				{
					if($_POST['nicht_abrechenbar'][$mc][$z])
						$verr='1';
				}
				else if($lw['nicht_abrechenbar'])
					$verr='1';
 			}
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nicht_abrechenbar']=$verr;

			// Ver. (Verrechnet)
			if($rech)
			{
				$img='1';
				if($GLOBALS['fxpglobals']['lang'] == 1)
					$tt='Bereits verrechnet';
				else
					$tt='Already invoiced';
			}
			else
			{
				$img='0';
				if($GLOBALS['fxpglobals']['lang'] == 1)
					$tt='Noch nicht verrechnet';
				else
					$tt='Not yet invoiced';
			}
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['verrechnungs_id']='~<img src="'.$GLOBALS['gfxpath'].$img.'.png" tooltip="'.$tt.'" style="cursor:help;">';

			// Freigabe
			if($frei)	// Freigegeben
			{
				if($changeable)	// ...änderbar
				{
					$img='1';
					if($GLOBALS['fxpglobals']['lang'] == 1)
					{
						$tt='Freigegeben';
						if($changeable > 1)
							$tt .= ' (&Auml;nderbar)';
						else
							$tt .= ' (Summensatz teilweise &auml;nderbar)';
					}
					else
					{
						$tt='Approved';
						if($changeable > 1)
							$tt .= ' (Changeable)';
						else
							$tt .= ' (Sum-record partially changeable)';
					}
				}
				else			// ...nicht änderbar
				{
					$img='tl_green';
					if($GLOBALS['fxpglobals']['lang'] == 1)
						$tt='Freigegeben (Nicht &auml;nderbar)';
					else
						$tt='Approved (Not changeable)';
				}
			}
			else		// Nicht freigegeben
			{
				if($changeable)	// ...änderbar
				{
					$img='0';
					if($GLOBALS['fxpglobals']['lang'] == 1)
					{
						$tt='Nicht freigegeben';
						if($changeable > 1)
							$tt .= ' (&Auml;nderbar)';
						else
							$tt .= ' (Summensatz teilweise &auml;nderbar)';
					}
					else
					{
						$tt='Not approved';
						if($changeable > 1)
							$tt .= ' (Changeable)';
						else
							$tt .= ' (Sum-record partially changeable)';
					}
				}
				else			// ...nicht änderbar
				{
					$img='tl_grey';
					if($GLOBALS['fxpglobals']['lang'] == 1)
						$tt='Nicht freigegeben (Nicht &auml;nderbar)';
					else
						$tt='Not approved (Not changeable)';
				}
			}
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['freigabe']='~<img src="'.$GLOBALS['gfxpath'].$img.'.png" tooltip="'.$tt.'" style="cursor:help;">';

			// Auftraggeber
			if($lw['zkunde'] && !$kliste[$lw['zkunde']])
			{
				$sql="SELECT pname, vorname, geschlecht FROM personen WHERE personen_id=".$lw['zkunde'];
				$pdt=db_values($sql);
				if(fxIsArray($pdt))
				{
					$name=$pdt['pname'];
					if(strlen(trim($pdt['vorname'])))
						$name .= ',&nbsp;'.trim($pdt['vorname']);
					$kliste[$lw['zkunde']]='~<font class="grey">'.$name.'</font>';
				}
				else
					$kliste[$lw['zkunde']]='~<font class="lightred">?'.$lw['zkunde'].'</font>';
			}
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['kunde_alias']=$kliste[$lw['zkunde']];

			// Prs-Abrechnung.
			$aa_tag='font';
			$aa_col='grey';
			if($lw['rechnungs_id'] || $lw['rechnungs_id_rk'])
			{
				$aa_tag='i';
				$aa_col='lightergrey';
				if($sr && ($abr == FXP_INV_FIX))
					$aa_col='lightred';
			}
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['abrechnungsart']='~<'.$aa_tag.' class="'.$aa_col.'">'.$aa_txt[$abr].'</'.$aa_tag.'>';

			// (Hidden)
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['mutterprojekt_id']	= $lw['mutterprojekt_id'];
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['id']					= $lw['projekt_id'];
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['zeit_id']				= $lw['zeit_id'];
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text1']			= $lw['ist_beginn'];
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text2']			= $lw['ist_ende'];
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text3']			= $lw['personen_id'];
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['zeitdaten']			= $lw['zeitaufwand'];
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_check1']			= $lw['nicht_abrechenbar'];
			if(fxIsArray($la_zug[$lw['zeit_id']]))
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['kostenstelle_id']=$la_zug[$lw['zeit_id']]['kostenstelle_id'];

			$z++;
		}
	}

	// Top border with empty line
	if($z)
	{
		fxf_maskColumnBorder($fa, 1, -1, $mc, $z, true);
		$z++;
	}
}
else
	error_msg(2187, false);
?>