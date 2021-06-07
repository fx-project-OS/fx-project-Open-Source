<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : rechte_vergabe_pre.inc                                       //
// Version     : 21.1                                                         //
// Begin       : 2020-12-21                                                   //
// Last Change : 2020-12-21                                                   //
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
 * PF 138: Access Rights - General
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.1
 */

$index351=true;

// Farben
if(!isset($col_h0) || !strlen($col_h0))
	$col_h0='79aec7';
if(!isset($col_h1) || !strlen($col_h1))
	$col_h1='b5cfdc';

// Maskenüberschrift
$maske351_head=get_text(351,'kurz',$GLOBALS['fxpglobals']['lang'],2).': <span class="s5 darkgrey">'.$rca[$rcat].'</span><br /><span class="grey tshadownone">&rarr;</span> <span class="blue tshadownone">';
if($rcat == 35)			// Reports
	$maske351_head .= db_value("SELECT repname FROM repliste WHERE repid=".$rid." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
else if($rcat == 36)	// Personen
{
	$arr=db_values("SELECT pname, vorname, personentyp FROM personen WHERE personen_id=".$rid." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
	$nam=$arr['pname'];
	if(strlen($arr['vorname']))
		$nam .= ', '.$arr['vorname'];
	$maske351_head .= '<span class="lightblue tshadownone">'.meldung($arr['personentyp']).'/</span>&nbsp;&nbsp;'.$nam;
}
else if($rcat == 37)	// Projekte
{
	$arr=db_values("SELECT vorgangsnummer, name_projekt FROM projekte WHERE projekt_id=".$rid." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
	$maske351_head .= '<span class="lightblue tshadownone">'.no_null($arr['vorgangsnummer']).'</span>&nbsp;&nbsp;'.$arr['name_projekt'];
}
else if($rcat == 38)	// Dokumente
{
	$doc=new fxpDoc();
	$arr=db_values("SELECT d.dok_id, d.elter_art, d.elter, d.dok_art, v.datei_name FROM d_dokumente d, d_dok_verlauf v WHERE d.dok_id=".$rid." AND v.dok_id=d.dok_id AND v.aktiv_kz=1 AND d.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND v.mandanten_id=".$GLOBALS['fxpglobals']['client']);
	$pa=$doc->getDocPath($arr['elter_art'].'_'.$arr['elter'], $arr);
	$maske351_head .= '<span class="lightblue tshadownone">'.str_replace("\\", "/", $pa['full']).'/</span>&nbsp;&nbsp;'.$arr['datei_name'];
}
$maske351_head .= '</span>';

// Zeilencounter
$z=0;

$bit=array(1, 2, 4, 8, 16, 32, 64, 128);
$chkb=array(1=>'Anzeigen', 'Anlegen', 'Aendern', 'Loeschen');

// Spaltenüberschriften anpassen
if($rcat == 38)	// Dokumente
{
	$cla=explode('|', meldung(3334,false));	// Anz.|Chk.|Änd.|Lös.|Dokument anzeigen und downloaden|Dokument auschecken und einchecken|Dokument bearbeiten, umbenennen und verschieben|Dokument löschen
//fxDebug($cla,'$cla');
	$cc=0;
	foreach($chkb as $cl)
	{
		$GLOBALS[$cl.'_lit']=$cla[$cc];
		$GLOBALS[$cl.'_lit_tooltip']=$cla[$cc+4];
		$cc++;
	}
}

// Abschnitte
$disp_usr=true;
$disp_grp=true;
$disp_dep=true;
$disp_prj=true;
if($rcat != 38)
	$disp_prj=false;

// Colspan
$csp=6;

// Bereits eingetragene Benutzergruppen / Benutzer / Abteilungen / Projekte ermitteln
$vorh_referenzen=array();
$sql  = "SELECT gruppeart, benutzergr_id, aktionscode FROM rechte_referenz";
$sql .= " WHERE id_referenz=".$rid." AND rechte_ref=".$rcat." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
$arr=db_values($sql,'*');
if(fxIsArray($arr))
{
	foreach($arr as $a)
		$vorh_referenzen[$a['gruppeart']][$a['benutzergr_id']]=$a['aktionscode'];
}

// Bezeichnungen für Indirekte Rechte
// 33=Eigentümer, 34=Zugeordnet, 41=Selbst, 209=Stellvertreter, 210=Projektmanager, 211=Abteilungsleiter, 743=Projektleiter, 620=Auftraggeber - Ansprechpartner
// 924=Geschäftsleitung, 1126=Übergeordnetes Projekt
// 961=Allgemein, 1252=Mandant
// 3074=Übergeordnete Person, 3075=Verwaltung, 3076=Ersteller
$ind_bez=array();
$sql="SELECT wertetabid, tabwert FROM wertetab WHERE wertetabid IN (33,34,41,209,210,211,743,620,924,961,1126,1252,3074,3075,3076) AND id_sprache=".$GLOBALS['fxpglobals']['lang']." AND mandanten_id=0";
$rarr=db_values($sql, '*');
if(fxIsArray($rarr))
{
	foreach($rarr as $r)
		$ind_bez[$r['wertetabid']]=$r['tabwert'];
}

// Übergeordnet bei Kategorie 37="Projekte"
if($rcat == 37)
	$elt=superior_vn($rid, false);
else
	$elt='';

$am=$GLOBALS['_maskcounter'];

// No right border in the following column
fxf_maskColumnBorder(array('Checkbox'), 0, -1, $am);

// Benutzer
if($disp_usr)
{
	// -----------------------------------------------------
	// ...Folgende Benutzertypen anzeigen: 275=I-MA, 840=E-MA, 276=AG + 620=AP-AG
	$in='275,840,276,620';
	// ...SQL zusammenbauen
	$sql  = "SELECT b.name_benutzer, b.benutzer_id, p.personen_id, w.tabwert, p.pname, p.vorname, p.geschlecht";
	$sql .= " FROM benutzer b, personen p, wertetab w";
	$sql .= " WHERE p.personen_id=b.personen_id AND p.mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$sql .= " AND w.wertetabid=p.personentyp AND w.id_sprache=".$GLOBALS['fxpglobals']['lang']." AND w.mandanten_id=0";
	$sql .= " AND b.benutzer_id>0 AND p.personen_id>0 AND (p.inaktiv=0 OR p.inaktiv IS NULL) AND b.mandanten_id=".$GLOBALS['fxpglobals']['client'];
	if($in)
		$sql .= " AND p.personentyp IN (".$in.")";
	$sql .= " ORDER BY w.tabwert, p.pname, p.vorname, b.name_benutzer";
	$arr=db_values($sql, '*');
//fxDebug($arr,$sql);
	if(fxIsArray($arr))
	{
		$indirekt=array();

		// 924: Geschäftsleitung ermitteln
		$sql="SELECT personen_id FROM ma_art_faehigkeiten WHERE maart_id=1 AND mafaehigkeit_id=2 AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$rarr=db_values($sql, '*');
		if(fxIsArray($rarr))
		{
			foreach($rarr as $r)
			{
				if(!isset($indirekt[$r['personen_id']]))
					$indirekt[$r['personen_id']]=924;
			}
		}

		// ...Kategorie 36="Personen" -> (743: Projektleiter, 209: Stellvertreter, 620: Auftraggeber - Ansprechpartner, 210: Projektmanager, 1126: Übergeordnetes Projekt)
		if($rcat == 36)
		{
			// Selbst
			if(!isset($indirekt[$rid]))
				$indirekt[$rid]=41;	// Selbst

			// Übergeordnete Person
			$sql="SELECT elter FROM personen WHERE personen_id=".$rid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$tmp=db_values($sql,'*');
//fxDebug($tmp,$sql);
			if(fxIsArray($tmp))
			{
				foreach($tmp as $t)
				{
					$pid=$t['elter'];
					if(!isset($indirekt[$pid]))
						$indirekt[$pid]=3074;
				}
			}

			// Abteilungen
			$d='';
			$da=get_abteilungen($rid, $GLOBALS['fxpglobals']['client']);
			if(fxIsArray($da))
				$d=array_to_csv($da);
//fxDebug($da,'$da: $d='.$d);

			// Verwaltung
			$sql  = "SELECT f.personen_id, p.abt_id FROM ma_art_faehigkeiten f, personen p WHERE p.personen_id=f.personen_id";
			$sql .= " AND f.maart_id=1 AND f.mafaehigkeit_id=-1 AND f.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND p.mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$tmp=db_values($sql,'*');
//fxDebug($tmp,$sql);
			if(fxIsArray($tmp))
			{
				foreach($tmp as $t)
				{
					$pid=$t['personen_id'];
					$aid=$t['abt_id'];
					if(!isset($indirekt[$pid]))
					{
						if(!$aid || !fxIsArray($da) || isset($da[$aid]))
							$indirekt[$pid]=3075;
					}
				}
			}

			// Abteilungsleiter
			if(strlen($d))
			{
				$sql="SELECT abt_hperson FROM abteilung WHERE abt_id IN (".$d.") AND abt_hperson>0 AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
				$tmp=db_values($sql,'*');
//fxDebug($tmp,$sql);
				if(fxIsArray($tmp))
				{
					foreach($tmp as $t)
					{
						$pid=$t['abt_hperson'];
						if(!isset($indirekt[$pid]))
							$indirekt[$pid]=211;
					}
				}
			}

			// PM, PL oder SPL von Auftraggeber
			$hpa=array(210=>'projektmanager', 743=>'projektleiter', 209=>'vertreter');
			foreach($hpa as $hpi => $hpf)
			{
				$sql="SELECT DISTINCT(".$hpf.") AS hpv FROM projekte WHERE (kunde=".$rid." OR ansprechpartner=".$rid.") AND ".$hpf.">0 AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
				$tmp=db_values($sql,'*');
//fxDebug($tmp,$sql);
				if(fxIsArray($tmp))
				{
					foreach($tmp as $t)
					{
						if(!isset($indirekt[$t['hpv']]))
							$indirekt[$t['hpv']]=$hpi;
					}
				}
			}

			// PL von Ressourcen
			$hpa=array(743=>'projektleiter', 209=>'vertreter');
			foreach($hpa as $hpi => $hpf)
			{
				$sql ="SELECT DISTINCT(p.".$hpf.") AS hpv FROM maplanung m, projekte p WHERE m.personen_id=".$rid." AND (m.sachmittel_id IS NULL OR m.sachmittel_id=0) AND m.mandanten_id=".$GLOBALS['fxpglobals']['client'];
				$sql .= " AND p.projekt_id=m.projekt_id AND p.".$hpf.">0 AND p.mandanten_id=".$GLOBALS['fxpglobals']['client'];
				$tmp=db_values($sql,'*');
//fxDebug($tmp,$sql);
				if(fxIsArray($tmp))
				{
					foreach($tmp as $t)
					{
						if(!isset($indirekt[$t['hpv']]))
							$indirekt[$t['hpv']]=$hpi;
					}
				}
			}

			// Ersteller
			$sql="SELECT aenderungs_id FROM personen_history WHERE personen_id=".$rid." AND aktion=2 AND mandanten_id=".$GLOBALS['fxpglobals']['client']." ORDER BY zeitstempel DESC";
			$pid=(int)db_value($sql);
//fxDebug($pid,$sql);
			if($pid && !isset($indirekt[$pid]))
				$indirekt[$pid]=3076;
		}

		// ...Kategorie 37="Projekte" -> (743: Projektleiter, 209: Stellvertreter, 620: Auftraggeber - Ansprechpartner, 210: Projektmanager, 1126: Übergeordnetes Projekt)
		if($rcat == 37)
		{
			// Elter
			if(strlen($elt))
			{
				$sql  = "SELECT b.personen_id FROM benutzer b, rechte_referenz r";
				$sql .= " WHERE b.benutzer_id=r.benutzergr_id AND r.rechte_ref=".$rcat." AND r.gruppeart=2163 AND r.id_referenz IN (".$elt.") AND r.id_referenz<>".$rid;
				$sql .= " AND r.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND b.mandanten_id=".$GLOBALS['fxpglobals']['client'];
				$earr=db_values($sql, '*');
				if(fxIsArray($earr))
				{
					foreach($earr as $r)
					{
						if(!isset($indirekt[$r['personen_id']]))
							$indirekt[$r['personen_id']]=1126;
					}
				}
			}

			$sql="SELECT projektleiter, vertreter, ansprechpartner, projektmanager FROM projekte WHERE projekt_id=".$rid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$rarr=db_values($sql);
			if(fxIsArray($rarr))
			{
				if(!isset($indirekt[$rarr['ansprechpartner']]))
					$indirekt[$rarr['ansprechpartner']]=620;
				if($rarr['projektmanager'] && !isset($indirekt[$rarr['projektmanager']]))
					$indirekt[$rarr['projektmanager']]=210;
				if($rarr['vertreter'] && !isset($indirekt[$rarr['vertreter']]))
					$indirekt[$rarr['vertreter']]=209;
				if(!isset($indirekt[$rarr['projektleiter']]))
					$indirekt[$rarr['projektleiter']]=743;
			}
		}

		// ...Kategorie 38="Dokumente" -> (33: Eigentümer + 34: Zugeordnet)
		if($rcat == 38)
		{
			$sql="SELECT dok_art,dok_kategorie,elter_art,elter,angelegt_von FROM d_dokumente WHERE dok_id=".$rid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$dia=db_values($sql);
//fxDebug($dia,$sql);
			if(fxIsArray($dia))
			{
				$own=(int)db_value("SELECT personen_id FROM benutzer WHERE benutzer_id=".$dia['angelegt_von']." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
				if(!isset($indirekt[$own]))
					$indirekt[$own]=33;

				if((($dia['elter_art'] == 2) || ($dia['elter_art'] == 9) || ($dia['elter_art'] == 11)) && !isset($indirekt[$dia['elter']]))
					$indirekt[$dia['elter']]=34;
			}
		}

		// Abschnittsüberschriftszeile (evtl. mit Leerzeile davor): Benutzer
		if($z || ($GLOBALS['fxpglobals']['tr'] == 110))
			$z=leerzeile(351,$am,$z,false,true, true);
		$txt='<div class="s5b" style="color:#ffffff;text-shadow:2px 2px 2px #000000;">&nbsp;'.meldung(2163, false).'</div>';
		$GLOBALS['fxptdata']['lvalues'][$am][$z]['checkbox']='~';
		$GLOBALS['fxptdata']['lvalues'][$am][$z]['bezeichnung_einf']='^col_'.$col_h0.'" nowrap colspan="'.$csp.'" style="padding:8px 2px;border:0;border-top-left-radius:12px;border-top-right-radius:12px;"><!--|~'.$txt;
		$GLOBALS['fxptdata']['lvalues'][$am][$z]['loeschen']='~-->'.$txt;
		$z++;

		// Personentyp
		$ptyp='';
		foreach($arr as $a)
		{
			// Personentyp
			if($a['tabwert'] != $ptyp)
			{
				$br='';
				if(strlen($ptyp))
					$br='<br />';

				$ptyp=$a['tabwert'];

				$txt='&nbsp;<b>'.$ptyp.'</b>';
				$GLOBALS['fxptdata']['lvalues'][$am][$z]['checkbox']='~';
				$GLOBALS['fxptdata']['lvalues'][$am][$z]['bezeichnung_einf']='^col_'.$col_h1.'" nowrap colspan="'.$csp.'" style="padding:4px 2px;border:0;"><!--|~'.$txt;
				$GLOBALS['fxptdata']['lvalues'][$am][$z]['loeschen']='~-->'.$txt;
				$z++;
			}

			if(fxIsArray($vorh_referenzen) && fxIsArray($vorh_referenzen[2163]) && $vorh_referenzen[2163][$a['benutzer_id']])
				$vrecht=$vorh_referenzen[2163][$a['benutzer_id']];
			else
				$vrecht=0;

			// Checkboxes
			if($err)
				$recht=setRightsCheckboxError($z);
			else
				$recht=setRightsCheckbox($z,$vrecht);

			// Bezeichnung
			$cls='';
			if($recht)
				$cls='green bold';
			$zus='';
			if($GLOBALS['fxptdata']['lvalues'][$am][$z]['checkbox'] || (!$vrecht && $recht) || (fxIsArray($indirekt) && $indirekt[$a['personen_id']]))
			{
				if(!strlen($cls))
					$cls='green';
				if(fxIsArray($indirekt) && $indirekt[$a['personen_id']])
					$zus='&nbsp;&nbsp;<i class="s2 lightergrey">('.$ind_bez[$indirekt[$a['personen_id']]].')</i>';
			}
			$name=$a['pname'];
			if(strlen(trim($a['vorname'])))
				$name .= ', '.trim($a['vorname']);
			$bez='~';
			if(strlen($cls))
				$bez .= '<font class="'.$cls.'">'.$name.'</font>';
			else
				$bez .= $name;
			$bez .= '&nbsp;<font class="darkgrey">['.$a['name_benutzer'].']</font>'.$zus;
			$GLOBALS['fxptdata']['lvalues'][$am][$z]['bezeichnung_einf']=$bez;

			// Hidden
			$GLOBALS['fxptdata']['lvalues'][$am][$z]['gruppeart']	= 2163;
			$GLOBALS['fxptdata']['lvalues'][$am][$z]['aktionscode']	= $vrecht;
			$GLOBALS['fxptdata']['lvalues'][$am][$z]['textzeile']	= $name;
			$GLOBALS['fxptdata']['lvalues'][$am][$z]['id_ref_art']	= $a['benutzer_id'];

			$z++;
		}

		// Abschnittsende
		$GLOBALS['fxptdata']['lvalues'][$am][$z]['checkbox']='~';
		$GLOBALS['fxptdata']['lvalues'][$am][$z]['bezeichnung_einf']='^col_'.$col_h0.'" nowrap colspan="'.$csp.'" style="padding:4px 24px;border:1px solid #e8e8e8;border-top:0;border-bottom-left-radius:8px;border-bottom-right-radius:8px;"><!--|~';
		$GLOBALS['fxptdata']['lvalues'][$am][$z]['loeschen']='~-->';
		$z++;
	}
}

// Benutzergruppen
if($disp_grp)
{
	// ...SQL zusammenbauen
	$sql  = "SELECT w.tabwert, g.benutzergr_id";
	$sql .= " FROM benutzergruppen g, wertetab w";
	$sql .= " WHERE w.wertetabid=g.gruppenname_id AND w.id_sprache=".$GLOBALS['fxpglobals']['lang']." AND g.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND w.mandanten_id IN (0,".$GLOBALS['fxpglobals']['client'].")";
	$sql .= " ORDER BY w.tabwert";
	$arr=db_values($sql, '*');
//fxDebug($arr,$sql);
	if(fxIsArray($arr))
	{
		$indirekt=array();
		// Projekte -> (1126: Übergeordnetes Projekt)
		if($elt)
		{
			$sql  = "SELECT benutzergr_id FROM rechte_referenz";
			$sql .= " WHERE rechte_ref=".$rcat." AND gruppeart=2164 AND id_referenz IN (".$elt.") AND id_referenz<>".$rid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$earr=db_values($sql, '*');
			if(fxIsArray($earr))
			{
				foreach($earr as $r)
					$indirekt[$r['benutzergr_id']]=1126;
			}
		}

		// Abschnittsüberschriftszeile (evtl. mit Leerzeile davor): Benutzerprofilgruppe
		if($z)
			$z=leerzeile(351,$am,$z);
		$txt='<div class="s5b" style="color:#ffffff;text-shadow:2px 2px 2px #000000;">&nbsp;'.meldung(2164, false).'</div>';
		$GLOBALS['fxptdata']['lvalues'][$am][$z]['checkbox']='~';
		$GLOBALS['fxptdata']['lvalues'][$am][$z]['bezeichnung_einf']='^col_'.$col_h0.'" nowrap colspan="'.$csp.'" style="padding:8px 2px;border:0;border-top-left-radius:12px;border-top-right-radius:12px;"><!--|~'.$txt;
		$GLOBALS['fxptdata']['lvalues'][$am][$z]['loeschen']='~-->'.$txt;
		$z++;

		foreach($arr as $a)
		{
			if(fxIsArray($vorh_referenzen) && fxIsArray($vorh_referenzen[2164]) && $vorh_referenzen[2164][$a['benutzergr_id']])
				$vrecht=$vorh_referenzen[2164][$a['benutzergr_id']];
			else
				$vrecht=0;

			// Checkboxes
			if($err)
				$recht=setRightsCheckboxError($z);
			else
				$recht=setRightsCheckbox($z,$vrecht);

			// Bezeichnung
			$cls='';
			if($recht)
				$cls='green bold';
			$zus='';
			if($GLOBALS['fxptdata']['lvalues'][$am][$z]['checkbox'] || (!$vrecht && $recht) || (fxIsArray($indirekt) && $indirekt[$a['benutzergr_id']]))
			{
				if(!strlen($cls))
					$cls='green';
				if(fxIsArray($indirekt) && $indirekt[$a['benutzergr_id']])
					$zus='&nbsp;&nbsp;<i class="s2 lightergrey">('.$ind_bez[$indirekt[$a['benutzergr_id']]].')</i>';
			}
			$bez='~';
			if(strlen($cls))
				$bez .= '<font class="'.$cls.'">';
			$bez .= $a['tabwert'];
			if(strlen($cls))
				$bez .= '</font>';
			$bez .= $zus;
			$GLOBALS['fxptdata']['lvalues'][$am][$z]['bezeichnung_einf']=$bez;

			// Hidden
			$GLOBALS['fxptdata']['lvalues'][$am][$z]['gruppeart']	= 2164;
			$GLOBALS['fxptdata']['lvalues'][$am][$z]['aktionscode']	= $vrecht;
			$GLOBALS['fxptdata']['lvalues'][$am][$z]['textzeile']	= $a['tabwert'];
			$GLOBALS['fxptdata']['lvalues'][$am][$z]['id_ref_art']	= $a['benutzergr_id'];

			$z++;
		}

		// Abschnittsende
		$GLOBALS['fxptdata']['lvalues'][$am][$z]['checkbox']='~';
		$GLOBALS['fxptdata']['lvalues'][$am][$z]['bezeichnung_einf']='^col_'.$col_h0.'" nowrap colspan="'.$csp.'" style="padding:4px 24px;border:1px solid #e8e8e8;border-top:0;border-bottom-left-radius:8px;border-bottom-right-radius:8px;"><!--|~';
		$GLOBALS['fxptdata']['lvalues'][$am][$z]['loeschen']='~-->';
		$z++;
	}
}

// Abteilungen
if($disp_dep)
{
	// ...SQL zusammenbauen
	$sql  = "SELECT abt_id, abt_ebene, abt_name";
	$sql .= " FROM abteilung";
	$sql .= " WHERE mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$sql .= " ORDER BY abt_ebene";
	$arr=db_values($sql, '*');
//fxDebug($arr,$sql);
	if(fxIsArray($arr))
	{
		$indirekt=array();
		// Projekte -> (1126: Übergeordnetes Projekt)
		if($elt)
		{
			$sql  = "SELECT benutzergr_id AS abt_id FROM rechte_referenz";
			$sql .= " WHERE rechte_ref=".$rcat." AND gruppeart=2165 AND id_referenz IN (".$elt.") AND id_referenz<>".$rid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$earr=db_values($sql, '*');
			if(fxIsArray($earr))
			{
				foreach($earr as $r)
					$indirekt[$r['abt_id']]=1126;
			}
		}

		// Abschnittsüberschriftszeile (evtl. mit Leerzeile davor): Abteilung
		if($z)
			$z=leerzeile(351,$am,$z);
		$txt='<div class="s5b" style="color:#ffffff;text-shadow:2px 2px 2px #000000;">&nbsp;'.meldung(2165, false).'</div>';
		$GLOBALS['fxptdata']['lvalues'][$am][$z]['checkbox']='~';
		$GLOBALS['fxptdata']['lvalues'][$am][$z]['bezeichnung_einf']='^col_'.$col_h0.'" nowrap colspan="'.$csp.'" style="padding:8px 2px;border:0;border-top-left-radius:12px;border-top-right-radius:12px;"><!--|~'.$txt;
		$GLOBALS['fxptdata']['lvalues'][$am][$z]['loeschen']='~-->'.$txt;
		$z++;

		foreach($arr as $a)
		{
			if(fxIsArray($vorh_referenzen) && fxIsArray($vorh_referenzen[2165]) && $vorh_referenzen[2165][$a['abt_id']])
				$vrecht=$vorh_referenzen[2165][$a['abt_id']];
			else
				$vrecht=0;

			// Checkboxes
			if($err)
				$recht=setRightsCheckboxError($z);
			else
				$recht=setRightsCheckbox($z,$vrecht);

			// Bezeichnung
			$cls='';
			if($recht)
				$cls='green bold';
			$zus='';
			if($GLOBALS['fxptdata']['lvalues'][$am][$z]['checkbox'] || (!$vrecht && $recht) || (fxIsArray($indirekt) && $indirekt[$a['abt_id']]))
			{
				if(!strlen($cls))
					$cls='green';
				if(fxIsArray($indirekt) && $indirekt[$a['abt_id']])
					$zus='&nbsp;&nbsp;<i class="s2 lightergrey">('.$ind_bez[$indirekt[$a['abt_id']]].')</i>';
			}
			$bez='~';
			if(strlen($cls))
				$bez .= '<font class="'.$cls.'">';
			$bez .= trim($a['abt_ebene']).': '.$a['abt_name'];
			if(strlen($cls))
				$bez .= '</font>';
			$bez .= $zus;
			$GLOBALS['fxptdata']['lvalues'][$am][$z]['bezeichnung_einf']=$bez;

			// Hidden
			$GLOBALS['fxptdata']['lvalues'][$am][$z]['gruppeart']	= 2165;
			$GLOBALS['fxptdata']['lvalues'][$am][$z]['aktionscode']	= $vrecht;
			$GLOBALS['fxptdata']['lvalues'][$am][$z]['textzeile']	= $a['abt_name'];
			$GLOBALS['fxptdata']['lvalues'][$am][$z]['id_ref_art']	= $a['abt_id'];

			$z++;
		}

		// Abschnittsende
		$GLOBALS['fxptdata']['lvalues'][$am][$z]['checkbox']='~';
		$GLOBALS['fxptdata']['lvalues'][$am][$z]['bezeichnung_einf']='^col_'.$col_h0.'" nowrap colspan="'.$csp.'" style="padding:4px 24px;border:1px solid #e8e8e8;border-top:0;border-bottom-left-radius:8px;border-bottom-right-radius:8px;"><!--|~';
		$GLOBALS['fxptdata']['lvalues'][$am][$z]['loeschen']='~-->';
		$z++;
	}
}

// Projekte
if($disp_prj)
{
	// ...Prüfen, ob gewähltes Dokument ein Projektdokument ist
	$prjdoc=0;
	$sql="SELECT elter_art, elter FROM d_dokumente WHERE dok_id=".$rid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$typ=db_values($sql);
//fxDebug($typ,$sql);
	if(fxIsArray($typ) && ($typ['elter_art'] == 3))
		$prjdoc=(int)$typ['elter'];
	if($prjdoc)
	{
		// Projektinfo
		$sql="SELECT vorgangsnummer, name_projekt, projektart FROM projekte WHERE projekt_id=".$prjdoc." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$prj=db_values($sql);
		if(fxIsArray($prj))
		{
			// Abschnittsüberschriftszeile (evtl. mit Leerzeile davor): Projektbeteiligte
			if($z)
				$z=leerzeile(351,$am,$z);
			$txt='<div class="s5b" style="color:#ffffff;text-shadow:2px 2px 2px #000000;">&nbsp;'.meldung(1920, false).'</div>';
			$GLOBALS['fxptdata']['lvalues'][$am][$z]['checkbox']='~';
			$GLOBALS['fxptdata']['lvalues'][$am][$z]['bezeichnung_einf']='^col_'.$col_h0.'" nowrap colspan="'.$csp.'" style="padding:8px 2px;border:0;border-top-left-radius:12px;border-top-right-radius:12px;"><!--|~'.$txt;
			$GLOBALS['fxptdata']['lvalues'][$am][$z]['loeschen']='~-->'.$txt;
			$z++;

			if(fxIsArray($vorh_referenzen) && fxIsArray($vorh_referenzen[1920]) && $vorh_referenzen[1920][$prjdoc])
				$vrecht=$vorh_referenzen[1920][$prjdoc];
			else
				$vrecht=0;

			// Checkboxes
			if($err)
				$recht=setRightsCheckboxError($z);
			else
				$recht=setRightsCheckbox($z,$vrecht);

			// Bezeichnung
			$bez='';
			if($GLOBALS['fxptdata']['lvalues'][$am][$z]['checkbox'] || (!$vrecht && $recht))
				$bez='^class_crk'.($z%2).'|';
			$bez .= '~'.no_null($prj['vorgangsnummer']).' - '.$prj['name_projekt'];
			$GLOBALS['fxptdata']['lvalues'][$am][$z]['bezeichnung_einf']=$bez;

			// Hidden
			$GLOBALS['fxptdata']['lvalues'][$am][$z]['gruppeart']	= 1920;
			$GLOBALS['fxptdata']['lvalues'][$am][$z]['aktionscode']	= $vrecht;
			$GLOBALS['fxptdata']['lvalues'][$am][$z]['textzeile']	= no_null($prj['vorgangsnummer']).' - '.$prj['name_projekt'];
			$GLOBALS['fxptdata']['lvalues'][$am][$z]['id_ref_art']	= $prjdoc;
			$z++;

			// Abschnittsende
			$GLOBALS['fxptdata']['lvalues'][$am][$z]['checkbox']='~';
			$GLOBALS['fxptdata']['lvalues'][$am][$z]['bezeichnung_einf']='^col_'.$col_h0.'" nowrap colspan="'.$csp.'" style="padding:4px 24px;border:1px solid #e8e8e8;border-top:0;border-bottom-left-radius:8px;border-bottom-right-radius:8px;"><!--|~';
			$GLOBALS['fxptdata']['lvalues'][$am][$z]['loeschen']='~-->';
			$z++;
		}
	}
}

// Leerzeile am Ende?
if($GLOBALS['fxpglobals']['tr'] == 110)
	$z=leerzeile(351,$am,$z,false,true, true);


////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/**
 * ???FUNCTION???
 *
 * @param various $z - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function setRightsCheckboxError($z)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$am=$GLOBALS['am'];

	$recht=0;
	$GLOBALS['fxptdata']['lvalues'][$am][$z]['checkbox']=$_POST['Checkbox'][$am][$z];

	$GLOBALS['fxptdata']['lvalues'][$am][$z]['alle_auswaehlen']=$_POST['alle_auswaehlen'][$am][$z];
	if($GLOBALS['fxptdata']['lvalues'][$am][$z]['alle_auswaehlen'])
		$recht |= 30;

	$GLOBALS['fxptdata']['lvalues'][$am][$z]['anzeigen']=$_POST['Anzeigen'][$am][$z];
	if($GLOBALS['fxptdata']['lvalues'][$am][$z]['anzeigen'])
		$recht |= 2;

	$GLOBALS['fxptdata']['lvalues'][$am][$z]['anlegen']=$_POST['Anlegen'][$am][$z];
	if($GLOBALS['fxptdata']['lvalues'][$am][$z]['anlegen'])
		$recht |= 4;

	$GLOBALS['fxptdata']['lvalues'][$am][$z]['aendern']=$_POST['Aendern'][$am][$z];
	if($GLOBALS['fxptdata']['lvalues'][$am][$z]['aendern'])
		$recht |= 8;

	$GLOBALS['fxptdata']['lvalues'][$am][$z]['loeschen']=$_POST['Loeschen'][$am][$z];
	if($GLOBALS['fxptdata']['lvalues'][$am][$z]['loeschen'])
		$recht |= 16;

	// JavaScript
	$GLOBALS['Checkbox'.$z.'_jf']=' '.fxf_jsFunction('checkLine', $am, $z, -1);
	$GLOBALS['alle_auswaehlen'.$z.'_jf']=' '.fxf_jsFunction('checkLine', $am, $z, 0);
	for($c=1; $c<5; $c++)
		$GLOBALS[$GLOBALS['chkb'][$c].$z.'_jf']=' '.fxf_jsFunction('checkLine', $am, $z, $GLOBALS['bit'][$c]);

	return $recht;
}

/**
 * ???FUNCTION???
 *
 * @param various $z     - Mandatory parameter: ???PARAMETER???
 * @param various $recht - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function setRightsCheckbox($z, $recht)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$am=$GLOBALS['am'];

	// Checkbox
	if($recht)
		$GLOBALS['fxptdata']['lvalues'][$am][$z]['checkbox']='1';

	// Alle
	if(($recht&30) == 30)
		$GLOBALS['fxptdata']['lvalues'][$am][$z]['alle_auswaehlen']='1';

	// Anz.
	if($recht&2)
		$GLOBALS['fxptdata']['lvalues'][$am][$z]['anzeigen']='1';
	// Anl.
	if($recht&4)
		$GLOBALS['fxptdata']['lvalues'][$am][$z]['anlegen']='1';
	// Änd.
	if($recht&8)
		$GLOBALS['fxptdata']['lvalues'][$am][$z]['aendern']='1';
	// Lös.
	if($recht&16)
		$GLOBALS['fxptdata']['lvalues'][$am][$z]['loeschen']='1';

	// JavaScript
	$GLOBALS['Checkbox'.$z.'_jf']=' '.fxf_jsFunction('checkLine', $am, $z, -1);
	$GLOBALS['alle_auswaehlen'.$z.'_jf']=' '.fxf_jsFunction('checkLine', $am, $z, 0);
	for($c=1; $c<5; $c++)
		$GLOBALS[$GLOBALS['chkb'][$c].$z.'_jf']=' '.fxf_jsFunction('checkLine', $am, $z, $GLOBALS['bit'][$c]);

	return $recht;
}
?>