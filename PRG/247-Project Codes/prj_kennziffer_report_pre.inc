<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : prj_kennziffer_report_pre.inc                                //
// Version     : 21.2                                                         //
// Begin       : 2021-01-14                                                   //
// Last Change : 2021-10-06                                                   //
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
 * PF 247: Project Codes
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

if(!isset($index))
	$index=true;

$pid=$_POST['Projekt_ID'];
$pids=inferior_vn($pid);
//echo('<B><U>$pids:</U></B><BR>'.$pids.'<HR>');

// Hauptprojektinfos einlesen
$prj=array();
$sql  = "SELECT";
$sql .= " p.projekt_id, p.mutterprojekt_id, p.vorgangsnummer, p.name_projekt, p.projektart, p.elter, p.projekt_status, p.aufwand_soll, p.aufwand_ist,";
$sql .= " b.abrechnungsart, b.abrechnungsart_mat,";
$sql .= " (b.budget_pers_int_gepl + b.budget_mat_int_gepl) AS budget_int_soll,";
$sql .= " (b.budget_pers_int_verbr + b.budget_mat_int_verbr) AS budget_int_ist,";
$sql .= " (b.budget_pers_ext_gepl + b.budget_mat_ext_gepl) AS budget_ext_soll,";
$sql .= " (b.budget_pers_ext_verbr + b.budget_mat_ext_verbr) AS budget_ext_ist";
$sql .= " FROM projekte p, budget_summe b";
$sql .= " WHERE p.projekt_id IN (".$pids.") AND p.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND b.projekt_id=p.projekt_id AND b.mandanten_id=".$GLOBALS['fxpglobals']['client'];
$sql .= " ORDER BY p.vorgangsnummer";
$tmp=db_values($sql, '*');
//fxDebug($tmp, $sql);
if(fxIsArray($tmp))
{
	foreach($tmp as $t)
	{
		if(!$t['aufwand_soll'])
			$t['aufwand_soll']=0;
		if(!$t['aufwand_ist'])
			$t['aufwand_ist']=0;
		if(!$t['abrechnungsart'])
			$t['abrechnungsart']=FXP_INV_HOUR;		// Stundenbasis
		if(!$t['abrechnungsart_mat'])
			$t['abrechnungsart_mat']=FXP_INV_MAT_HU;	// Stunden- oder Mengenbasis

		$prj[$t['projekt_id']]=$t;
	}
}

// Liste anzeigen
if(sizeof($prj))
{
	// Maskenspalten
	$fa=fxf_getMaskFields(492);
//fxDebug($fa,'$fa', 0);

	// No-Wrap-Felder
	$Mitarbeiter_nw=true;

	// Heutiges Datum
	$am=substr($GLOBALS['datetime'],0,6).'01000000';
//echo('$am='.$am.'<HR>');

	// Summendatensätze
	$pk=array();
	for($md=0; $md<4; $md++)
	{
		$pk[$md]=array
		(
			'z'=>false,
			'a'=>array('s'=>0.00, 'i'=>0.00, 'p'=>0.00, 'r'=>0.00, 'e'=>0.00, 'v'=>0.00),
			'h'=>array('s'=>0.00, 'i'=>0.00, 'p'=>0.00, 'r'=>0.00, 'e'=>0.00, 'v'=>0.00)
		);
	}

	// Alle Vormonats Soll-Aufwände ermitteln
	$hsa=array();
	$sql  = "SELECT projekt_id, aufwand_soll, zeitstempel";
	$sql .= " FROM projekte_history";
	$sql .= " WHERE projekt_id IN (".$pids.") AND zeitstempel<'".$am."' AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$sql .= " ORDER BY zeitstempel DESC";
	$tmp=db_values($sql, '*');
//fxDebug($tmp, $sql);
	if(fxIsArray($tmp))
	{
		foreach($tmp as $t)
		{
			$id=$t['projekt_id'];
			if(!$hsa[$id] || !$hsa[$id]['z'] || ($t['zeitstempel'] > $hsa[$id]['z']))
			{
				$a=$t['aufwand_soll'];
				if(!$a)
					$a=0;
				$hsa[$id]=array('a'=>$a, 'z'=>$t['zeitstempel']);
			}
		}
	}
//fxDebug($hsa, 'Vormonats Soll-Aufwände');

	// Alle abgerechneten Beträge ermitteln
	$abr=array();
	$sql  = "SELECT p.projekt_id, p.gesamtpreis, p.mwst, r.datum";
	$sql .= " FROM rechnungsposition p, rechnung r";
	$sql .= " WHERE p.projekt_id IN (".$pids.") AND p.mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$sql .= " AND r.rechnungs_id=p.rechnungs_id AND (r.archiv IS NULL OR r.archiv=0) AND r.mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$tmp=db_values($sql, '*');
//fxDebug($tmp, $sql);
	if(fxIsArray($tmp))
	{
		foreach($tmp as $t)
		{
			$abr['a'][$t['projekt_id']] += $t['gesamtpreis'];
			if($t['datum'] < $am)
				$abr['h'][$t['projekt_id']] += $t['gesamtpreis'];
		}
	}
//fxDebug($abr, 'Rechnung');

	// Soll-Aufwände + Rechnungsbeträge verteilen in die Projektkennziffern
	foreach($prj as $id => $p)
	{
		// Modus ermitteln
		if($p['abrechnungsart'] == FXP_INV_NOT)		// Nicht abrechenbar			-> 0=Leistungsvertrag intern
			$md=0;
		else if($p['abrechnungsart'] == FXP_INV_FIX)	// Festpreis -> Werksvertrag
		{
			if($p['budget_ext_soll'])			// Externer Festpreis			-> 3=Werksvertrag extern
				$md=3;
			else
				$md=2;							// Interner Festpreis			-> 2=Werksvertrag intern
		}
		else
			$md=1;								// Stundenbasis					-> 1=Leistungsvertrag extern

		// Anzeigen
		$pk[$md]['z']=true;

		// Rechnungspreis
		if($abr['a'][$id])
		{
			$pk[$md]['z']=true;

			$pk[$md]['a']['r'] += $abr['a'][$id];
			if($abr['h'][$id])
				$pk[$md]['h']['r'] += $abr['h'][$id];
		}

		if($p['projektart'] != FXP_PRJ_TASK)	// Alle Nicht-Aufgaben überspringen
			continue;

		// Soll-Aufwand
		$a=$p['aufwand_soll'];
		$pk[$md]['a']['s'] += $a;
		// Vormonat
		if($hsa[$id])
			$a=$hsa[$id]['a'];
		$pk[$md]['h']['s'] += $a;
	}

	// Alle produktiven Leistungsarten ermitteln
	$pla=array();
	$sql  = "SELECT z.zeit_id, z.projekt_id, z.zeitaufwand, z.ist_beginn, k.kostenstelle_id, s.kostennummer, s.kostenbezeichnung";
	$sql .= " FROM zeitdaten z, kostenstellen s, kostenzuordnung k";
	$sql .= " WHERE z.zeit_id=k.id AND s.kostenstelle_id=k.kostenstelle_id";
	$sql .= " AND k.kostenstellenart=".FXP_TE_TIMEREC." AND k.kostendefinition=903 AND k.mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$sql .= " AND z.projekt_id IN (".$pids.") AND z.zeitart IN (".FXP_TE_TIMEREC.",".FXP_TE_TIMEREC_SUM.") AND z.mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$sql .= " AND s.id_sprache=".$GLOBALS['fxpglobals']['lang']." AND s.kostendefinition=903 AND (s.produktiv IS NOT NULL OR s.produktiv<>0) AND s.mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$tmp=db_values($sql, '*');
	if(fxIsArray($tmp))
	{
		foreach($tmp as $t)
			$pla[$t['zeit_id']]=$t;
	}
//fxDebug($pla, $sql);

	// Personendaten ermitteln
	$pers_arr=array();
	$sql  = "SELECT p.personen_id, p.pname, p.vorname, p.geschlecht, m.projekt_id, m.aufwand_std, m.prozent";
	$sql .= " FROM personen p, projekte r, maplanung m";
	$sql .= " WHERE m.projekt_id IN (".$pids.") AND p.personen_id=m.personen_id AND r.projekt_id=m.projekt_id AND r.projektart=".FXP_PRJ_TASK;
	$sql .= " AND p.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND r.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND m.mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$sql .= " ORDER BY p.pname, p.vorname";
	$pda=db_values($sql, '*');
	if(fxIsArray($pda))
	{
		foreach($pda as $p)
		{
			if(!fxIsArray($pers_arr[$p['personen_id']]))
			{
				$fn=trim($p['pname']);
				$nm=$fn;
				$vn=trim($p['vorname']);
				if(strlen($vn))
					$nm .= ', '.$vn;
				$pers_arr[$p['personen_id']]=array('gs'=>$p['geschlecht'], 'nm'=>$nm, 'fn'=>$fn, 'vn'=>$vn, 'pa'=>array());
			}
			$pers_arr[$p['personen_id']]['pa'][$p['projekt_id']]=$prj[$p['projekt_id']]['aufwand_soll']*$p['prozent'];
		}
	}
	else
		$pers_arr[0]=array('gs'=>8);
//fxDebug($pers_arr, $sql);

	// Alle Zeitdatensätze ermitteln
	$sql  = "SELECT zeit_id, personen_id, projekt_id, ist_beginn, zeitaufwand, kosten_ek, kosten_vk, rk_ek, rk_vk, nicht_abrechenbar, rk_nicht_abrechenbar";
	$sql .= " FROM zeitdaten";
	$sql .= " WHERE projekt_id IN (".$pids.") AND zeitart IN (".FXP_TE_TIMEREC.",".FXP_TE_TIMEREC_SUM.",".FXP_TE_TRAVELEXP_TIMEREC.",".FXP_TE_TRAVELEXP.") AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$zds=db_values($sql, '*');
//fxDebug($zds, $sql);

	// Alle Zeiterfassungswerte ermitteln
	if(sizeof($pers_arr))
	{
		foreach($pers_arr as $pe => $ped)
		{
			if(fxIsArray($ped['pa']))
			{
				foreach($ped['pa'] as $id => $sa)
				{
					// Modus ermitteln
					if($prj[$id]['abrechnungsart'] == FXP_INV_NOT)		// Nicht abrechenbar			-> 0=Leistungsvertrag intern
						$md=0;
					else if($prj[$id]['abrechnungsart'] == FXP_INV_FIX)	// Festpreis -> Werksvertrag
					{
						if($prj[$id]['budget_ext_soll'])			// Externer Festpreis			-> 3=Werksvertrag extern
							$md=3;
						else
							$md=2;									// Interner Festpreis			-> 2=Werksvertrag intern
					}
					else
						$md=1;										// Stundenbasis					-> 1=Leistungsvertrag extern

					$pk[$md]['z']=true;

					// Akt. Personensatz
//echo('<B>$pe:</B> '.$pe.', <B>$id:</B> '.$id.', <B>$pid:</B> '.$pid.', <B>$sa:</B> '.$sa.'<BR>');
					if(!fxIsArray($pk[$md]['a']['m']) || !fxIsArray($pk[$md]['a']['m'][$pe]))
						$pk[$md]['a']['m'][$pe]=array('s'=>$sa, 'i'=>0.00, 'p'=>0.00, 'e'=>0.00, 'v'=>0.00);
					else
						$pk[$md]['a']['m'][$pe]['s'] += $sa;
					// Vormonats-Personensatz
					if($prj[$pid]['aufwand_soll'])
						$vsa=$sa*$pk[$md]['h']['s']/$prj[$pid]['aufwand_soll'];
					else
						$vsa=0.00;
					if(!fxIsArray($pk[$md]['h']['m']) || !fxIsArray($pk[$md]['h']['m'][$pe]))
						$pk[$md]['h']['m'][$pe]=array('s'=>$vsa, 'i'=>0.00, 'p'=>0.00, 'e'=>0.00, 'v'=>0.00);
					else
						$pk[$md]['h']['m'][$pe]['s'] += $vsa;

					if(fxIsArray($zds))
					{
						foreach($zds as $t)
						{
							if(($t['personen_id'] == $pe) && ($t['projekt_id'] == $id))
							{
								// Ist-Aufwand
								$ia=$t['zeitaufwand'];
								if(!$ia)
									$ia=0.00;
								// Ist-Produktiv
								if(($kr_aktiv && $pla[$t['zeit_id']]) || !$kr_aktiv)
									$ip=$ia;
								else
									$ip=0.00;
								// EK
								$ek=$t['kosten_ek'];
								if(!$ek)
									$ek=0.00;
								$rek=$t['rk_ek'];
								if(!$rek)
									$rek=0.00;
								$ek += $rek;
								// VK
								$vk=$t['kosten_vk'];
								if(!$vk || ($md == 0) || ($md == 2) || $t['nicht_abrechenbar'])
									$vk=0.00;
								$rvk=$t['rk_vk'];
								if(!$rvk || ($md == 0) || ($md == 2) || $t['rk_nicht_abrechenbar'])
									$rvk=0.00;
								$vk += $rvk;
//echo('md['.$md.']: $pe='.$pe.', $ia='.$ia.', $ip='.$ip.', $rc='.$rc.', $ek+$rek='.$ek.', $vk+$rvk='.$vk.'<BR>');
								// Akt. Gesamtsatz
								$pk[$md]['a']['i'] += $ia;
								$pk[$md]['a']['p'] += $ip;
								$pk[$md]['a']['e'] += $ek;
								$pk[$md]['a']['v'] += $vk;
								// Vormonats-Gesamtsatz
								if($t['ist_beginn'] < $am)
								{
									$pk[$md]['h']['i'] += $ia;
									$pk[$md]['h']['p'] += $ip;
									$pk[$md]['h']['e'] += $ek;
									$pk[$md]['h']['v'] += $vk;
								}
								// Akt. Personensatz
								$pk[$md]['a']['m'][$pe]['i'] += $ia;
								$pk[$md]['a']['m'][$pe]['p'] += $ip;
								$pk[$md]['a']['m'][$pe]['e'] += $ek;
								$pk[$md]['a']['m'][$pe]['v'] += $vk;
								// Vormonats-Personensatz
								if($t['ist_beginn'] < $am)
								{
									$pk[$md]['h']['m'][$pe]['i'] += $ia;
									$pk[$md]['h']['m'][$pe]['p'] += $ip;
									$pk[$md]['h']['m'][$pe]['e'] += $ek;
									$pk[$md]['h']['m'][$pe]['v'] += $vk;
								}
							}
						}
					}
				}
			}
		}
	}
//fxDebug($pk, 'Projektkennziffern');

	// Maskentitel
	$sql="SELECT vorgangsnummer, name_projekt FROM projekte WHERE projekt_id=".$pid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$pif=db_values($sql);
	// 814: Projektkennziffern-Report von $var1 für $var2
	$maske492_head=meldung(814, false, '['.no_null($pif['vorgangsnummer']).': '.$pif['name_projekt'].']', meldung((int)substr($am, 4, 2)+17, true).' '.substr($am, 0, 4));

	// Liste aufbereiten
	if($pk[0]['z'] || $pk[1]['z'] || $pk[2]['z'] || $pk[3]['z'])
	{
		$cs=18;
		if(!$pk[0]['z'])
		{
			$cs--;
			$pk_lp_i_nv=true;
		}
		if(!$pk[1]['z'])
		{
			$cs--;
			$pk_lp_e_nv=true;
		}
		if(!$pk[2]['z'])
		{
			$cs--;
			$pk_fp_i_nv=true;
		}
		if(!$pk[3]['z'])
		{
			$cs--;
			$pk_fp_e_nv=true;
		}

		$waehrung=$GLOBALS['fxpglobals']['settings']['waehrung'];
		$gesamtsumme=meldung(1555, true);		// Gesamtsumme
		$r100=meldung(1781, true).' > 100%';	// Ressourcenzuordnung

		$z=0;
		$mc=$GLOBALS['_maskcounter'];
		for($md=0; $md<4; $md++)
		{
			if(!$pk[$md]['z'])
				continue;

			// Überschrift
			$txt='<b class="s4b">&nbsp;'.meldung(810+$md, true).'&nbsp;</b>';
			fxf_listSectHeader($mc, $z, $txt, 'mitarbeiter', 'pk_rch_v', $cs);
			$z++;

			// Mitarbeiterzeilen
			$sss=0.00;
			if(sizeof($pers_arr))
			{
				foreach($pers_arr as $id => $p)
				{
					if(!$pk[$md]['a']['m'][$id])
						continue;

					// Mitarbeiter
					$txt=person_pic($id);
					$GLOBALS['fxptdata']['lvalues'][$mc][$z]['mitarbeiter']='~'.$txt;
					// Soll-Std.
					$GLOBALS['fxptdata']['lvalues'][$mc][$z]['pk_std_soll']='~'.get_entry($pk[$md]['a']['m'][$id]['s']/3600, 'dezimal').'&nbsp;h';
					$sss += $pk[$md]['a']['m'][$id]['s'];
					// Ist-Std.
					$GLOBALS['fxptdata']['lvalues'][$mc][$z]['pk_std_ist']='~'.get_entry($pk[$md]['a']['m'][$id]['i']/3600, 'dezimal').'&nbsp;h';
					// Prod. Std.
					$GLOBALS['fxptdata']['lvalues'][$mc][$z]['pk_std_prod']='~'.get_entry($pk[$md]['a']['m'][$id]['p']/3600, 'dezimal').'&nbsp;h';
					// h-Satz (I)
					if($pk[$md]['a']['m'][$id]['i'] > 0.00)
						$hi=$pk[$md]['a']['m'][$id]['e']/($pk[$md]['a']['m'][$id]['i']/3600);
					else
						$hi=0.00;
					$GLOBALS['fxptdata']['lvalues'][$mc][$z]['pk_ek']='~'.get_entry($hi, 'dezimal').'&nbsp;'.$waehrung;
					// h-Satz (E)
					if($pk[$md]['a']['m'][$id]['i'] > 0.00)
						$he=$pk[$md]['a']['m'][$id]['v']/($pk[$md]['a']['m'][$id]['i']/3600);
					else
						$he=0.00;
					$GLOBALS['fxptdata']['lvalues'][$mc][$z]['pk_vk']='~'.get_entry($he, 'dezimal').'&nbsp;'.$waehrung;
					// Prod. (I)
					$psi=($pk[$md]['a']['m'][$id]['p']/3600)*$hi;
					$GLOBALS['fxptdata']['lvalues'][$mc][$z]['pk_pstd_i']='~'.get_entry($psi, 'dezimal').'&nbsp;'.$waehrung;
					// Prod. (E)
					$pse=($pk[$md]['a']['m'][$id]['p']/3600)*$he;
					$GLOBALS['fxptdata']['lvalues'][$mc][$z]['pk_pstd_e']='~'.get_entry($pse, 'dezimal').'&nbsp;'.$waehrung;
					// Restl. Infos nicht anzeigen
					$GLOBALS['fxptdata']['lvalues'][$mc][$z]['pk_lp_i']		= '~';
					$GLOBALS['fxptdata']['lvalues'][$mc][$z]['pk_lp_e']		= '~';
					$GLOBALS['fxptdata']['lvalues'][$mc][$z]['pk_fp_i']		= '~';
					$GLOBALS['fxptdata']['lvalues'][$mc][$z]['pk_fp_e']		= '~';
					$GLOBALS['fxptdata']['lvalues'][$mc][$z]['pk_agrad']	= '~';
					$GLOBALS['fxptdata']['lvalues'][$mc][$z]['pk_fgrad']	= '~';
					$GLOBALS['fxptdata']['lvalues'][$mc][$z]['pk_ums_g']	= '~';
					$GLOBALS['fxptdata']['lvalues'][$mc][$z]['pk_ums_v']	= '~';
					$GLOBALS['fxptdata']['lvalues'][$mc][$z]['pk_ums_m']	= '~';
					$GLOBALS['fxptdata']['lvalues'][$mc][$z]['pk_rch_v']	= '~';

					$z++;
				}
			}

			// N.N. (nomen nominandum=der zu nennende Name) anzeigen, falls Summe der Soll-Aufwände der zugeteilten Personen nicht die Gesamtsumme Soll-Aufwand ist
			// -> dies bedeutet das zu einigen Aufgaben noch kein Mitarbeiter zugeordnet wurde
			if($sss != $pk[$md]['a']['s'])
			{
				$diff=$pk[$md]['a']['s']-$sss;
				if($diff < 0)
				{
					$tx='R100';
					$tt=$r100;
				}
				else
				{
					$tx='N.N.';
					$tt='nomen nominandum';
				}

				// Mitarbeiter
				$txt='&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font class="red" tooltip="'.$tt.'">'.$tx.'</font>';
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['mitarbeiter']='~'.$txt;
				// Soll-Std.
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['pk_std_soll']='~'.get_entry($diff/3600, 'dezimal').'&nbsp;h&nbsp;';
				// Restl. Infos nicht anzeigen
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['pk_std_ist']	= '~';
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['pk_std_prod']	= '~';
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['pk_ek']		= '~';
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['pk_vk']		= '~';
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['pk_pstd_i']	= '~';
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['pk_pstd_e']	= '~';
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['pk_lp_i']		= '~';
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['pk_lp_e']		= '~';
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['pk_fp_i']		= '~';
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['pk_fp_e']		= '~';
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['pk_agrad']	= '~';
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['pk_fgrad']	= '~';
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['pk_ums_g']	= '~';
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['pk_ums_v']	= '~';
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['pk_ums_m']	= '~';
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['pk_rch_v']	= '~';

				$z++;
			}

			$cls='^class_cr3 bold|';
			$cli='^class_cr0|~';

			// Gesamtsumme
			$GLOBALS['Mitarbeiter_height_'.$z]=22;
			$GLOBALS['msk_nbcol_492_'.$z]=$GLOBALS['msk_tbcol'];
			fxf_maskColumnBorder($fa, 3, -1, $mc, $z, true);
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['mitarbeiter']=$cls.'~'.$gesamtsumme;
			// Soll-Std.
			$pk_std_soll	= get_double(get_entry($pk[$md]['a']['s']/3600, 'dezimal'));
			$pk_std_soll_v	= get_double(get_entry($pk[$md]['h']['s']/3600, 'dezimal'));
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['pk_std_soll']=$cls.'~'.get_entry($pk_std_soll, 'dezimal').'&nbsp;h';
			// Ist-Std.
			$pk_std_ist		= get_double(get_entry($pk[$md]['a']['i']/3600, 'dezimal'));
			$pk_std_ist_v	= get_double(get_entry($pk[$md]['h']['i']/3600, 'dezimal'));
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['pk_std_ist']=$cls.'~'.get_entry($pk_std_ist, 'dezimal').'&nbsp;h';
			// Prod. Std.
			$pk_std_prod	= get_double(get_entry($pk[$md]['a']['p']/3600, 'dezimal'));
			$pk_std_prod_v	= get_double(get_entry($pk[$md]['h']['p']/3600, 'dezimal'));
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['pk_std_prod']=$cls.'~'.get_entry($pk_std_prod, 'dezimal').'&nbsp;h';
			// h-Satz (I)
			if($pk_std_ist > 0.00)
				$pk_ek=get_double(get_entry($pk[$md]['a']['e']/$pk_std_ist, 'dezimal'));
			else
				$pk_ek=0.00;
			if($pk_std_ist_v > 0.00)
				$pk_ek_v=get_double(get_entry($pk[$md]['h']['e']/$pk_std_ist_v, 'dezimal'));
			else
				$pk_ek_v=0.00;
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['pk_ek']=$cls.'~'.get_entry($pk_ek, 'dezimal').'&nbsp;'.$waehrung;
			// h-Satz (E)
			if($pk_std_ist > 0.00)
				$pk_vk=get_double(get_entry($pk[$md]['a']['v']/$pk_std_ist, 'dezimal'));
			else
				$pk_vk=0.00;
			if($pk_std_ist_v > 0.00)
				$pk_vk_v=get_double(get_entry($pk[$md]['h']['v']/$pk_std_ist_v, 'dezimal'));
			else
				$pk_vk_v=0.00;
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['pk_vk']=$cls.'~'.get_entry($pk_vk, 'dezimal').'&nbsp;'.$waehrung;
			// Prod. (I)
			$pk_pstd_i		= $pk_std_prod*$pk_ek;
			$pk_pstd_i_v	= $pk_std_prod_v*$pk_ek_v;
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['pk_pstd_i']=$cls.'~'.get_entry($pk_pstd_i, 'dezimal').'&nbsp;'.$waehrung;
			// Prod. (E)
			$pk_pstd_e		= $pk_std_prod*$pk_vk;
			$pk_pstd_e_v	= $pk_std_prod_v*$pk_vk_v;
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['pk_pstd_e']=$cls.'~'.get_entry($pk_pstd_e, 'dezimal').'&nbsp;'.$waehrung;
			// Summenspalte
			if($md == 0)		// Leistungsvertrag intern
			{
				$gs		= $pk_std_soll*$pk_ek;
				$gs_v	= $pk_std_soll_v*$pk_ek_v;
				if(!$gs)
					$gs=0.00;
				if(!$gs_v)
					$gs_v=0.00;
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['pk_lp_i']=$cls.'~'.get_entry($gs, 'dezimal').'&nbsp;'.$waehrung;
			}
			else if($md == 1)	// Leistungsvertrag extern
			{
				$gs		= $pk_std_soll*$pk_vk;
				$gs_v	= $pk_std_soll_v*$pk_vk_v;
				if(!$gs)
					$gs=0.00;
				if(!$gs_v)
					$gs_v=0.00;
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['pk_lp_e']=$cls.'~'.get_entry($gs, 'dezimal').'&nbsp;'.$waehrung;
			}
			else if($md == 2)	// Werksvertrag intern
			{
				$gs 	= $prj[$pid]['budget_int_soll'];
				$gs_v	= $prj[$pid]['budget_int_soll'];
				if(!$gs)
					$gs=0.00;
				if(!$gs_v)
					$gs_v=0.00;
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['pk_fp_i']=$cls.'~'.get_entry($gs, 'dezimal').'&nbsp;'.$waehrung;
			}
			else				// Werksvertrag extern
			{
				$gs		= $prj[$pid]['budget_ext_soll'];
				$gs_v	= $prj[$pid]['budget_ext_soll'];
				if(!$gs)
					$gs=0.00;
				if(!$gs_v)
					$gs_v=0.00;
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['pk_fp_e']=$cls.'~'.get_entry($gs, 'dezimal').'&nbsp;'.$waehrung;
			}
			if(!$GLOBALS['fxptdata']['lvalues'][$mc][$z]['pk_lp_i'])
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['pk_lp_i']=$cli;
			if(!$GLOBALS['fxptdata']['lvalues'][$mc][$z]['pk_lp_e'])
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['pk_lp_e']=$cli;
			if(!$GLOBALS['fxptdata']['lvalues'][$mc][$z]['pk_fp_i'])
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['pk_fp_i']=$cli;
			if(!$GLOBALS['fxptdata']['lvalues'][$mc][$z]['pk_fp_e'])
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['pk_fp_e']=$cli;
			// A-Grad
			if($md < 2)
				$cw=$pk_pstd_i;
			else
				$cw=$pk_pstd_e;
			if($gs > 0.00)
			{
				$ag=($cw*100)/$gs;
				if($ag > 100.00)
					$ag=100.00;
			}
			else
				$ag=100.00;
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['pk_agrad']=$cls.'~'.get_entry($ag, 'dezimal').'&nbsp;%';
			// F-Grad
			if($pk_std_soll > 0.00)
			{
				$fg=($pk_std_ist*100)/$pk_std_soll;
				if($fg > 100.00)
					$fg=100.00;
			}
			else
				$fg=100.00;
			if($pk_std_soll_v > 0.00)
			{
				$fg_v=($pk_std_ist_v*100)/$pk_std_soll_v;
				if($fg_v > 100.00)
					$fg_v=100.00;
			}
			else
				$fg_v=100.00;
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['pk_fgrad']=$cls.'~'.get_entry($fg, 'dezimal').'&nbsp;%';
			// Umsatz (G)
			$ug=get_double(get_entry(($fg/100.00)*$gs, 'dezimal'));
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['pk_ums_g']=$cls.'~'.get_entry($ug, 'dezimal').'&nbsp;'.$waehrung;
			// Umsatz (V)
			$uv=get_double(get_entry(($fg_v/100.00)*$gs_v, 'dezimal'));
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['pk_ums_v']=$cls.'~'.get_entry($uv, 'dezimal').'&nbsp;'.$waehrung;
			// Umsatz (M)
			$um=$ug-$uv;
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['pk_ums_m']=$cls.'~'.get_entry($um, 'dezimal').'&nbsp;'.$waehrung;
			// Abger. (V)
			$av=$pk[$md]['h']['r'];
			if(!$av)
				$av=0.00;
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['pk_rch_v']=$cls.'~'.get_entry($av, 'dezimal').'&nbsp;'.$waehrung;
			$z++;

			$GLOBALS['msk_nbcol_492_'.$z]=$GLOBALS['msk_sbcol'];
			fxf_maskColumnBorder($fa, 1, -1, $mc, $z, true);
			$z++;
		}
	}
}
?>