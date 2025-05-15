<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : projekt_report_pre.inc                                       //
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
 * PF 118: Project Detail Report
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

$mc=$GLOBALS['_maskcounter'];

// Keine Überschriften + Report komplett weiss
$maske301_nch=true;
$maske301_aw=true;

$dat=array();
if(!$err && $Button_Submit && $ProjektID)
{
	$sql="SELECT vorgangsnummer FROM projekte WHERE projekt_id=".$ProjektID." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$svn=db_value($sql);
//fxDebug($svn, $sql);
	if(strlen((string)$svn))
	{
		$pflt=filter_projekt('and', false, false);

		$pstat='';
		if($_POST['pmenu_gepla'])
			$pstat .= " OR p.projekt_status=".FXP_PS_PLANNED;
		if($_POST['pmenu_inakt'])
			$pstat .= " OR p.projekt_status=".FXP_PS_INACTIVE;
		if($_POST['pmenu_aktiv'])
			$pstat .= " OR p.projekt_status=".FXP_PS_ACTIVE;
		if($_POST['pmenu_abges'])
			$pstat .= " OR p.projekt_status=".FXP_PS_COMPLETED;

		$sqlz  = "FROM";
		$sqlz .= " projekte p, budget_summe b, zeitdaten z ";
		$sqlz .= "WHERE";
		$sqlz .= " b.projekt_id=p.projekt_id AND z.projekt_id=p.projekt_id AND z.zeitart=102";
		$sqlz .= " AND (p.vorgangsnummer LIKE '".$svn."%'";
		if(strlen((string)$svn) > 4)
			$sqlz .= " OR p.vorgangsnummer='".substr((string)$svn, 0, -5)."'";
		$sqlz .= ")";
		if(strlen((string)$pflt))
			$sqlz .= $pflt;
		if(strlen((string)$pstat))
			$sqlz .= " AND (".substr((string)$pstat, 3).")";
		if($_POST['Kunde'])
			$sqlz .= " AND p.kunde=".(int)$_POST['Kunde'];
		if($_POST['Projektmanager'])
			$sqlz .= " AND p.projektmanager=".(int)$_POST['Projektmanager'];
		if($_POST['Projektleiter'])
			$sqlz .= " AND (p.projektleiter=".(int)$_POST['Projektleiter']." OR p.vertreter=".(int)$_POST['Projektleiter'].")";
		if($_POST['MaArt_ID'])
		{
			if(substr((string)$_POST['MaArt_ID'], -1) == 'f')
				$sqlz .= " AND p.mafaehigkeit_id=".(int)substr((string)$_POST['MaArt_ID'], 0, -1);
			else
				$sqlz .= " AND p.maart_id=".(int)$_POST['MaArt_ID'];
		}
		if($pmenu_von)
			$sqlz .= " AND (z.soll_ende>='".substr((string)$pmenu_von, 0, 8)."000000' OR z.ist_ende>='".substr((string)$pmenu_von, 0, 8)."000000')";
		if($pmenu_bis)
			$sqlz .= " AND (z.soll_beginn<='".substr((string)$pmenu_bis, 0, 8)."240000' OR z.ist_beginn<='".substr((string)$pmenu_bis, 0, 8)."240000')";

		$sqlz .= " AND p.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND b.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND z.mandanten_id=".$GLOBALS['fxpglobals']['client']." ";

		$sql  = "SELECT";
		$sql .= " p.projekt_id, p.mutterprojekt_id, p.elter, p.projektart, p.vorgangsnummer, p.name_projekt, p.beschreibung, p.kategorie_id,";
		$sql .= " p.kunde, p.ansprechpartner, p.projektmanager, p.projektleiter, p.vertreter, p.projekt_status, p.prioritaet, p.verlauf,";
		$sql .= " p.deadline, p.maart_id, p.mafaehigkeit_id, p.anmerkung, p.anmerkungintern, p.ref_nummer, p.abhaengigkeit, p.wiederholungart,";
		$sql .= " p.anzahl, p.erstelldatum, p.aufwand_soll, p.aufwand_ist, p.aufwand_todo, p.max_zeit_aufw,";
		$sql .= " b.abrechnungsart, b.abrechnungsart_mat, b.ek_voraus, b.vk_voraus, b.ek, b.vk,";
		$sql .= " b.budgetrahmen_pers_int, b.budgetrahmen_pers_ext, budgetrahmen_mat_int, budgetrahmen_mat_ext,";
		$sql .= " b.budget_pers_int_gepl, b.budget_pers_ext_gepl, b.budget_mat_int_gepl, b.budget_mat_ext_gepl,";
		$sql .= " b.budget_pers_int_gen, b.budget_pers_ext_gen, b.budget_mat_int_gen, b.budget_mat_ext_gen,";
		$sql .= " b.budget_pers_int_akt, b.budget_pers_ext_akt, b.budget_mat_int_akt, b.budget_mat_ext_akt,";
		$sql .= " b.budget_pers_int_verpl, b.budget_pers_ext_verpl, b.budget_mat_int_verpl, b.budget_mat_ext_verpl,";
		$sql .= " b.budget_pers_int_verbr, b.budget_pers_ext_verbr, b.budget_mat_int_verbr, b.budget_mat_ext_verbr,";
		$sql .= " b.rechnung_fest_ext, b.rechnung_std_ext, b.rechnung_reise_ext, b.rechnung_mat_ext,";
		$sql .= " b.rechnung_fest_ext_off, b.rechnung_std_ext_off, b.rechnung_reise_ext_off, b.rechnung_mat_ext_off,";
		$sql .= " z.soll_beginn, z.soll_ende, z.gen_beginn, z.gen_ende, z.ist_beginn, z.ist_ende, z.soll_verzoeg, z.ist_verzoeg ";
		$sql .= $sqlz;
		$sql .= "ORDER BY";
		$sql .= " p.vorgangsnummer";

		$dat=db_values($sql, '*');
//fxDebug($dat, $sql);
	}
}
else
	$maske301_nv=true;

// Liste anzeigen?
if(fxIsArray($dat))
{
	// Aufgaben-IDs
	$aufids="SELECT p.projekt_id ".$sqlz."AND p.projektart=".FXP_PRJ_TASK;

	// Literale holen
	$lit_name_projekt			= get_text(46, '20');		// Name des Vorgangs
	$lit_elter					= get_text(113, '20');		// Zugeordnet zu Objekt
	$lit_abhaengigkeit			= get_text(381, '20');		// Abhängig von Vorgang

	$lit_projektart				= get_text(366, '20');		// Art des Vorgangs
	$lit_projekt_status			= get_text(137, '20');		// Vorgangs-Status
	$lit_prioritaet				= get_text(47, '20');		// Priorität
	$lit_erstelldatum			= get_text(6, '20');		// Erstelldatum

	$lit_ansprechpartner		= get_text(349, '20');		// Ansprechp. Auftragg.
	$lit_projektmanager			= get_text(1454, '20');		// Projektmanager
	$lit_projektleiter			= get_text(121, '20');		// Projektleiter
	$lit_vertreter				= get_text(1386, '20');		// Stell. Projektleiter

	$lit_soll_beginn			= get_text(130, '20');		// Geplantes Startdatum
	$lit_soll_ende				= get_text(76, '20');		// Geplantes Enddatum
	$lit_gen_beginn				= get_text(1772, '20');		// Genehm. Startdatum
	$lit_gen_ende				= get_text(1773, '20');		// Genehm. Enddatum
	$lit_ist_beginn				= get_text(129, '20');		// Tatsächl. Startdatum
	$lit_ist_ende				= get_text(67, '20');		// Tatsächl. Enddatum

	$lit_deadline				= get_text(48, '20');		// Deadline
	$lit_aufwand_soll			= get_text(297, '20');		// Aufwand Soll
	$lit_aufwand_ist			= get_text(298, '20');		// Aufwand Ist

	$lit_max_zeit_aufw			= get_text(132, '20');		// max. Zeitaufwand
	$lit_soll_verzoeg			= get_text(108, '20');		// voraus. Verzögerung
	$lit_ist_verzoeg			= get_text(334, '20');		// Abweichung

	$lit_kategorie_id			= get_text(1, '15');		// Prj.-Kategorie
	$lit_pca_code				= get_text(1847, '20');		// PCA-Code
	$lit_ref_nummer				= get_text(1342, '20');		// Referenznummer
	$lit_mafaehigkeit_id		= get_text(375, '20');		// Fähigkeit
	$lit_meilenstein			= get_text(656, '20');		// Meilensteinname
	$lit_methode				= get_text(1659, '20');		// Ermittlungsmethode
	$lit_id						= get_text(179, '20');		// ID
	$lit_zeitpunkt				= get_text(1660, '20');		// Berichtszeitpunkt

	$lit_budgetrahmen_pers_int	= get_text(1781, '20');		// Int. PRahmen
	$lit_budgetrahmen_mat_int	= get_text(1782, '20');		// Int. MRahmen
	$lit_budgetrahmen_ges_int	= get_text(1828, '20');		// Int. GRahmen

	$lit_budget_pers_int_gepl	= get_text(1754, '20');		// Int. gepl. Personen
	$lit_budget_pers_int_gen	= get_text(1758, '20');		// Int. geneh. Personen
	$lit_budget_pers_int_akt	= get_text(1783, '20');		// Int. aktiv. Personen
	$lit_budget_pers_int_verpl	= get_text(1762, '20');		// Int. verpl. Personen
	$lit_budget_pers_int_verbr	= get_text(1766, '20');		// Int. verbr. Personen
	$lit_budget_pers_int_rest	= get_text(1806, '20');		// Int. Rest Personen

	$lit_budget_mat_int_gepl	= get_text(1756, '20');		// Int. gepl. Material
	$lit_budget_mat_int_gen		= get_text(1760, '20');		// Int. geneh. Material
	$lit_budget_mat_int_akt		= get_text(1785, '20');		// Int. aktiv. Material
	$lit_budget_mat_int_verpl	= get_text(1764, '20');		// Int. verpl. Material
	$lit_budget_mat_int_verbr	= get_text(1768, '20');		// Int. verbr. Material
	$lit_budget_mat_int_rest	= get_text(1808, '20');		// Int. Rest Material

	$lit_budget_ges_int_gepl	= get_text(1795, '20');		// Int. gepl. Gesamtb.
	$lit_budget_ges_int_gen		= get_text(1797, '20');		// Int. geneh. Gesamtb.
	$lit_budget_ges_int_akt		= get_text(1799, '20');		// Int. aktiv. Gesamtb.
	$lit_budget_ges_int_verpl	= get_text(1801, '20');		// Int. verpl. Gesamtb.
	$lit_budget_ges_int_verbr	= get_text(1803, '20');		// Int. verbr. Gesamtb.
	$lit_budget_ges_int_rest	= get_text(1810, '20');		// Int. Rest Gesamtb.

	$lit_budgetrahmen_pers_ext	= get_text(1859, '20');		// Ext. PRahmen
	$lit_budgetrahmen_mat_ext	= get_text(1857, '20');		// Ext. MRahmen
	$lit_budgetrahmen_ges_ext	= get_text(1855, '20');		// Ext. GRahmen

	$lit_budget_pers_ext_gepl	= get_text(1755, '20');		// Ext. gepl. Personen
	$lit_budget_pers_ext_gen	= get_text(1759, '20');		// Ext. geneh. Personen
	$lit_budget_pers_ext_akt	= get_text(1784, '20');		// Ext. aktiv. Personen
	$lit_budget_pers_ext_verpl	= get_text(1763, '20');		// Ext. verpl. Personen
	$lit_budget_pers_ext_verbr	= get_text(1767, '20');		// Ext. verbr. Personen
	$lit_budget_pers_ext_rest	= get_text(1807, '20');		// Ext. Rest Personen

	$lit_budget_mat_ext_gepl	= get_text(1757, '20');		// Ext. gepl. Material
	$lit_budget_mat_ext_gen		= get_text(1761, '20');		// Ext. geneh. Material
	$lit_budget_mat_ext_akt		= get_text(1786, '20');		// Ext. aktiv. Material
	$lit_budget_mat_ext_verpl	= get_text(1765, '20');		// Ext. verpl. Material
	$lit_budget_mat_ext_verbr	= get_text(1769, '20');		// Ext. verbr. Material
	$lit_budget_mat_ext_rest	= get_text(1809, '20');		// Ext. Rest Material

	$lit_budget_ges_ext_gepl	= get_text(1796, '20');		// Ext. gepl. Gesamtb.
	$lit_budget_ges_ext_gen		= get_text(1798, '20');		// Ext. geneh. Gesamtb.
	$lit_budget_ges_ext_akt		= get_text(1800, '20');		// Ext. aktiv. Gesamtb.
	$lit_budget_ges_ext_verpl	= get_text(1802, '20');		// Ext. verpl. Gesamtb.
	$lit_budget_ges_ext_verbr	= get_text(1804, '20');		// Ext. verbr. Gesamtb.
	$lit_budget_ges_ext_rest	= get_text(1811, '20');		// Ext. Rest Gesamtb.

	$lit_beschreibung			= get_text(38, '20');		// Beschreibung
	$lit_anmerkung				= get_text(44, '20');		// Anmerkungen
	$lit_anmerkungintern		= get_text(110, '20');		// Interne Anmerkung

	$lit_personen				= get_text(627, 'text');	// Zugeord. Personen

	$lit_phistorie				= get_text(243, '20');		// Projekt-Historie
	$lit_bhistorie				= get_text(363, '20');		// Budget-Historie
	$lit_rhistorie				= meldung(2821, false);		// Ressourcen-Historie

	// Sonstige Bezeichnungen
	$bez_std			= '&nbsp;h';
	$bez_waehrung		= '&nbsp;'.$GLOBALS['fxpglobals']['settings']['waehrung'];
	$bez_waehrung_std	= $bez_waehrung.'/h';

	$bez_ohne_zuordnung	= '<i class="grey">('.meldung(597, false).')</i>';	// Ohne Zuordnung
	$bez_ohne			= '<i class="grey">('.meldung(388, false).')</i>';	// Ohne
	$bez_offen			= '<i class="grey">('.meldung(1794, false).')</i>';	// Offen
	$bez_unbegrenzt		= '<i class="grey">('.meldung(1678, false).')</i>';	// Unbegrenzt
	$bez_leer			= '<i class="grey">'.meldung(794, false).'</i>';	// -leer-

	// 2822: Projekterstellung,Projektbasis,Budgeterstellung,Budgetbasis,Ressourcenzuordnung,Ressourcenänderung,Ressourcenentfernung
	$bez_hist=explode(',', meldung(2822, false));

	// Mask fields + line heights
	$fa=fxf_getMaskFields(301);
//fxDebug($fa,'$fa', 0);
	foreach($fa as $fn)
		${$fn.'_height'}=20;

	// Spaltenklassen
	$colclln='~-->';
	$colclle='~-->';

	// Nachschlagewerte
	$arr_projektart=kategorie_array('projektart');
	$arr_projekt_status=fxf_getReferenceValues('projekt_status');
	$arr_prioritaet=fxf_getReferenceValues('prioritaet');

	// Alle Projektkategorien ermitteln
	$kcs=false;
	$kar=fxf_getReferenceValues('kategorie');
	if(fxIsArray($kar))
	{
		foreach($kar as $kid => $ktxt)
		{
			$kar[$kid]=pk_backgrounds(1,$ktxt);
			if($kar[$kid]['set'])
				$kcs=true;
		}
	}
//fxDebug($kar, '$kar');

	// Budgetschleife
	$budget_arten=array('gepl', 'gen', 'akt', 'verpl', 'verbr');
	// Ressourcendaten
	$pdarray=array('soll_beginn', 'soll_ende', 'auslastung', 'aufwand_std', 'prozent');

	// Referenzarray, Aufgaben-IDs + Verantwortlichkeits-Personenliste aufbauen
	$refarr=array();
	$perarr=array();
	$minlvl=999;
	foreach($dat as $dc => $d)
	{
		$refarr[$d['projekt_id']]=$dc;
		if(strlen((string)$d['vorgangsnummer']) < strlen((string)$svn))
			continue;

		$pid=(int)$d['ansprechpartner'];
		if($pid && !$perarr[$pid])
			$perarr[$pid]=person_pic($pid);

		$pid=(int)$d['projektmanager'];
		if($pid && !$perarr[$pid])
			$perarr[$pid]=person_pic($pid);

		$pid=(int)$d['projektleiter'];
		if($pid && !$perarr[$pid])
			$perarr[$pid]=person_pic($pid);

		$pid=(int)$d['vertreter'];
		if($pid && !$perarr[$pid])
			$perarr[$pid]=person_pic($pid);

		$lvl=ceil(strlen((string)$d['vorgangsnummer'])/5);
		$dat[$dc]['lvl']=$lvl;
		$minlvl=MIN($minlvl, $lvl);
	}
//fxDebug($refarr, 'Referenz-Array');
//fxDebug($aufids, 'Aufgaben-IDs');

	// Personen-Ressourcen ermitteln
	$hr=array();
	if(strlen((string)$aufids))
	{
		$sql  = "SELECT m.personen_id, m.projekt_id, m.soll_beginn, m.soll_ende, m.auslastung, m.aufwand_std, m.prozent, m.e_mail ";
		$sql .= "FROM";
		$sql .= " maplanung m, personen p ";
		$sql .= "WHERE";
		$sql .= " p.personen_id=m.personen_id AND m.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND m.personen_id>0 AND (m.sachmittel_id IS NULL OR m.sachmittel_id=0) AND m.projekt_id IN (".$aufids.")  AND p.mandanten_id=".$GLOBALS['fxpglobals']['client']." ";
		if($_POST['Abt_ID'])
			$sql .= "AND p.abt_id IN (".inferior_abteilung($_POST['Abt_ID']).") ";
		$sql .= "ORDER BY";
		$sql .= " p.pname, p.vorname";
		$hrt=db_values($sql, '*');
//fxDebug($hrt, $sql);
		if(fxIsArray($hrt))
		{
			foreach($hrt as $h)
			{
				$pid=(int)$h['personen_id'];
				if(!$perarr[$pid])
					$perarr[$pid]=person_pic($pid);

				$hr[$h['projekt_id']][$pid]=$h;
			}
		}
	}
//fxDebug($perarr, 'Personen-Array');
//fxDebug($hr, 'Personen-Ressourcen');

	// Anzeigekriterien
	$ausf=(int)$_POST['ausfuehrlich'];
	$aufw=(int)$_POST['Aufwaende'];
	$kost=(int)$_POST['Kosten_rep'];
	if($kost)
	{
		$budgetkst=array('I'=>1, 'E'=>2, 'B'=>3);
		$budgetart=strtoupper(substr((string)$GLOBALS['fxpglobals']['settings']['budgetart'], 0, 1));
		$kost=(int)$budgetkst[$budgetart];
	}

	// Auftraggeber
	$ag=0;
	if($GLOBALS['fxpglobals']['persdat']['personentyp'] == 620)
		$ag=(int)$GLOBALS['fxpglobals']['persdat']['elter'];
	$pers=$GLOBALS['fxpglobals']['person'];

	// Daten anzeigen
	$z=0;
	foreach($dat as $d)
	{
		// Im gewünschten Level
		if(strlen((string)$d['vorgangsnummer']) < strlen((string)$svn))
			continue;
		$alvl=(int)$_POST['Level'];
		$mlvl=$minlvl+$alvl;
		if($d['lvl'] > $mlvl)
			continue;

		// Welche Kosten darf der Benutzer sehen?
		$dkst=$bkst;
		if($dkst < 0)
		{
			$dkst=0;
			// Projektmanager, Projektleiter und Stellvertreter dürfen interne Kosten sehen
			if(($d['projektmanager'] == $pers) || ($d['projektleiter'] == $pers) || ($d['vertreter'] == $pers))
				$dkst=1;
			// Auftraggeber darf externe Kosten sehen
			else if($d['kunde'] == $ag)
				$dkst=2;
		}
		switch($kost)
		{
			case 0:	// Keine Kosten
				$dkst=0;
			break;

			case 1:	// Nur Interne Kosten
				if($dkst == 2)
					$dkst=0;
				else if($dkst == 3)
					$dkst=1;
			break;

			case 2:	// Nur Externe Kosten
				if($dkst == 1)
					$dkst=0;
				else if($dkst == 3)
					$dkst=2;
			break;
		}

/*
		// Leerzeile?
		if($z)
		{
			$txt='<br /><hr class="fxhr" /><br />';
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text1']='^class_transparent" colspan="6" align="center"><!--|~'.$txt;
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text6']='~-->'.$txt;
			$z++;
		}
*/

		// Zeile 1
		// ...Name des Vorgangs
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text1']=literalClass('', false);
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text2']=columnClass(5, 'l', false);
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text6']=$colclln;
		$z++;
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text1']=literalClass($lit_name_projekt, false);
		$txt='<span class="s4 darkgrey">'.no_null($d['vorgangsnummer']).'</span>&nbsp;<b class="s4">'.$d['name_projekt'].'</b>';
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text2']=columnClass(5, 'l', false).$txt;
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text6']=$colclln.$txt.'&nbsp;';
		$z++;
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text1']=literalClass('', false);
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text2']=columnClass(5, 'l', false);
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text6']=$colclln;
		$z++;

		if($ausf)
		{
			// Zeile 2
			// ...Zugeordnet zu Objekt
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text1']=literalClass().$lit_elter.':&nbsp;';
			if($d['elter'])
				$txt=no_null($dat[$refarr[$d['elter']]]['vorgangsnummer']).'&nbsp;<font class="darkgrey">'.$dat[$refarr[$d['elter']]]['name_projekt'].'</font>';
			else
				$txt=$bez_ohne_zuordnung;
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text2']=columnClass(5).$txt;
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text6']=$colclln.$txt.'&nbsp;';
			$z++;

/*
			// Zeile 3
			// ...Abhängig von Vorgang
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text1']=literalClass().$lit_abhaengigkeit.':&nbsp;';
			if($d['abhaengigkeit'])
				$txt=no_null($dat[$refarr[$d['abhaengigkeit']]]['vorgangsnummer']).'&nbsp;<font class="darkgrey">'.$dat[$refarr[$d['abhaengigkeit']]]['name_projekt'].'</font>';
			else
				$txt=$bez_ohne;
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text2']=columnClass(5).$txt;
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text6']=$colclln.$txt.'&nbsp;';
			$z++;
*/
		}

		// Leerzeile
		fxf_maskColumnBorder($fa, 1, -1, $mc, $z, true);
		$z++;

		// Zeile 4
		// ...Art des Vorgangs
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text1']=literalClass($lit_projektart, false);
		$txt=$arr_projektart[$d['projektart']];
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text2']=columnClass(0, 'l', false).$txt.'&nbsp;';
		// ...Ansprechp. Auftragg.
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text3']=literalClass($lit_ansprechpartner, false);
		if($d['ansprechpartner'] && $perarr[$d['ansprechpartner']])
			$txt=$perarr[$d['ansprechpartner']];
		else
			$txt=$bez_ohne;
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text4']=columnClass(3, 'l', false).$txt;
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text6']=$colclln.$txt.'&nbsp;';
		$z++;

		// Zeile 5
		// ...Vorgangs-Status
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text1']=literalClass($lit_projekt_status);
		$txt=$arr_projekt_status[$d['projekt_status']];
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text2']=columnClass().$txt.'&nbsp;';
		// ...Projektmanager
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text3']=literalClass($lit_projektmanager);
		if($d['projektmanager'] && $perarr[$d['projektmanager']])
			$txt=$perarr[$d['projektmanager']];
		else
			$txt=$bez_ohne;
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text4']=columnClass(3).$txt;
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text6']=$colclln.$txt.'&nbsp;';
		$z++;

		// Zeile 6
		// ...Priorität
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text1']=literalClass($lit_prioritaet);
		$txt=$arr_prioritaet[$d['prioritaet']];
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text2']=columnClass().$txt.'&nbsp;';
		// ...Projektleiter
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text3']=literalClass($lit_projektleiter);
		if($d['projektleiter'] && $perarr[$d['projektleiter']])
			$txt=$perarr[$d['projektleiter']];
		else
			$txt=$bez_ohne;
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text4']=columnClass(3).$txt;
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text6']=$colclln.$txt.'&nbsp;';
		$z++;

		// Zeile 7
		// ...Erstelldatum
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text1']=literalClass($lit_erstelldatum);
		$txt=get_entry($d['erstelldatum'], 'datetime');
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text2']=columnClass().$txt.'&nbsp;';
		// ...Stell. Projektleiter
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text3']=literalClass($lit_vertreter);
		if($d['vertreter'] && $perarr[$d['vertreter']])
			$txt=$perarr[$d['vertreter']];
		else
			$txt=$bez_ohne;
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text4']=columnClass(3).$txt;
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text6']=$colclln.$txt.'&nbsp;';
		$z++;

		// Zeile 12-13
		if($ausf)
		{
			// Leerzeile
			fxf_maskColumnBorder($fa, 1, -1, $mc, $z, true);
			$z++;

			// ...ID
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text1']=literalClass($lit_id, false);
			$txt=(int)$d['projekt_id'];
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text2']=columnClass(0, 'l', false).$txt.'&nbsp;';
			// ...PCA-Code
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text3']=literalClass($lit_pca_code, false);
			$sql="SELECT parameter_txt FROM projekte_parameter WHERE projekt_id=".$d['projekt_id']." AND mandanten_id=".$GLOBALS['fxpglobals']['client']." AND parameter_typ=376 AND parameter_var='TXT'";
			$txt=trim((string)db_value($sql));
			if(!strlen((string)$txt))
				$txt=$bez_ohne;
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text4']=columnClass(3, 'l', false).$txt;
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text6']=$colclln.$txt.'&nbsp;';
			$z++;

			// ...Prj.-Kategorie
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text1']=literalClass($lit_kategorie_id);
			if($d['kategorie_id'] && isset($kar[$d['kategorie_id']]))
			{
				$bg='';
				if($kar[$d['kategorie_id']]['set'])
					$bg='<span style="border-radius:4px;background:'.$kar[$d['kategorie_id']]['bg'].';">&nbsp;&nbsp;</span>&nbsp;';
				$txt=$bg.$kar[$d['kategorie_id']]['txt'];
			}
			else
				$txt=$bez_ohne;
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text2']=columnClass().$txt.'&nbsp;';
			// ...Fähigkeit
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text3']=literalClass($lit_mafaehigkeit_id);
			if($d['maart_id'])
			{
				$txt=hist_value('maart_id', $d['maart_id'], false, false);
				if($d['mafaehigkeit_id'])
					$txt .= ' -&gt; '.hist_value('mafaehigkeit_id', $d['mafaehigkeit_id'], false, false);
			}
			else
				$txt=$bez_ohne;
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text4']=columnClass(3).$txt;
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text6']=$colclln.$txt.'&nbsp;';
			$z++;

			// ...Referenznummer
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text1']=literalClass($lit_ref_nummer);
			$txt='';
			if(!is_null($d['ref_nummer']) && strlen((string)$d['ref_nummer']))
				$txt=trim((string)$d['ref_nummer']);
			if(!strlen((string)$txt))
				$txt=$bez_ohne;
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text2']=columnClass().$txt.'&nbsp;';
			// ...Ermittlungsmethode
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text3']=literalClass($lit_methode);
			$sql="SELECT methode, zeitpunkt, tage FROM projektfortschritt WHERE projekt_id=".$d['projekt_id']." AND mandanten_id=".$GLOBALS['fxpglobals']['client']." AND personen_id IS NULL AND aktiv_kz=1";
			$tmp=db_values($sql);
			if(is_array($tmp))
				$txt=meldung($tmp['methode'], false);
			else
				$txt=$bez_ohne;
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text4']=columnClass(3).$txt;
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text6']=$colclln.$txt.'&nbsp;';
			$z++;

			// ...Meilensteinname
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text1']=literalClass($lit_meilenstein);
			$sql="SELECT name_meilenstein FROM meilensteine WHERE projekt_id=".$d['projekt_id']." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$txt=trim((string)db_value($sql));
			if(!strlen((string)$txt))
				$txt=$bez_ohne;
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text2']=columnClass().$txt.'&nbsp;';
			// ...Berichtszeitpunkt
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text3']=literalClass($lit_zeitpunkt);
			if(is_array($tmp))
			{
				$txt=meldung($tmp['zeitpunkt'], false);
				if($tmp['zeitpunkt'] == 1792)	// Alle [x] Tage nach Soll-Startdatum
					$txt=str_replace('[x]', $tmp['tage'], $txt);
			}
			else
				$txt=$bez_ohne;
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text4']=columnClass(3).$txt;
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text6']=$colclln.$txt.'&nbsp;';
			$z++;
		}

		// Leerzeile
		fxf_maskColumnBorder($fa, 1, -1, $mc, $z, true);
		$z++;

		// Zeile 8
		// ...Geplantes Startdatum
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text1']=literalClass($lit_soll_beginn, false);
		$txt=get_entry($d['soll_beginn'], 'datum');
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text2']=columnClass(0, 'l', false).$txt.'&nbsp;';
		// ...Genehm. Startdatum
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text3']=literalClass($lit_gen_beginn, false);
		if($d['gen_beginn'])
			$txt=get_entry($d['gen_beginn'], 'datum');
		else
			$txt='<i>['.$txt.']</i>';
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text4']=columnClass(0, 'l', false).$txt.'&nbsp;';
		// ...Tatsächl. Startdatum
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text5']=literalClass($lit_ist_beginn, false);
		if($d['ist_beginn'])
			$txt=get_entry($d['ist_beginn'], 'datum');
		else
			$txt=$bez_offen;
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text6']=columnClass(0, 'l', false).$txt.'&nbsp;';
		$z++;

		// Zeile 9
		// ...Geplantes Enddatum
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text1']=literalClass($lit_soll_ende);
		$txt=get_entry($d['soll_ende'], 'datum');
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text2']=columnClass().$txt.'&nbsp;';
		// ...Genehm. Enddatum
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text3']=literalClass($lit_gen_ende);
		if($d['gen_ende'])
			$txt=get_entry($d['gen_ende'], 'datum');
		else
			$txt='<i>['.$txt.']</i>';
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text4']=columnClass().$txt.'&nbsp;';
		// ...Tatsächl. Enddatum
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text5']=literalClass($lit_ist_ende);
		if($d['ist_ende'])
			$txt=get_entry($d['ist_ende'], 'datum');
		else
			$txt=$bez_offen;
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text6']=columnClass().$txt.'&nbsp;';
		$z++;

		// Zeile 10
		if(!$aufw)
			fxf_maskColumnBorder($fa, 1, -1, $mc, $z, true);
		// ...Deadline
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text1']=literalClass($lit_deadline);
		if($d['deadline'])
			$txt=get_entry($d['deadline'], 'datum');
		else
			$txt=$bez_unbegrenzt;
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text2']=columnClass().$txt.'&nbsp;';
		if($aufw)
		{
			// ...Aufwand Soll
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text3']=literalClass($lit_aufwand_soll);
			if($d['aufwand_soll'])
				$txt=get_entry($d['aufwand_soll']/3600, 'dezimal').$bez_std;
			else
				$txt=$bez_ohne;
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text4']=columnClass(0, 'r').$txt.'&nbsp;';
			// ...Aufwand Ist
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text5']=literalClass($lit_aufwand_ist);
			if(!$d['aufwand_ist'])
				$d['aufwand_ist']=0.00;
			$txt=get_entry($d['aufwand_ist']/3600, 'dezimal').$bez_std;
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text6']=columnClass(0, 'r').$txt.'&nbsp;';
		}
		$z++;

		// Zeile 11
		if($aufw)
		{
			// ...max. Zeitaufwand
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text1']=literalClass($lit_max_zeit_aufw);
			if($d['max_zeit_aufw'])
				$txt=get_entry($d['max_zeit_aufw']/3600, 'dezimal').$bez_std;
			else
				$txt=$bez_unbegrenzt;
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text2']=columnClass(0, 'r').$txt.'&nbsp;';
			// ...voraus. Verzögerung
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text3']=literalClass($lit_soll_verzoeg);
			if($d['soll_verzoeg'])
				$txt=get_entry($d['soll_verzoeg']/3600, 'dezimal').$bez_std;
			else
				$txt=$bez_ohne;
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text4']=columnClass(0, 'r').$txt.'&nbsp;';
			// ...Abweichung
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text5']=literalClass($lit_ist_verzoeg);
			if(!$d['ist_verzoeg'])
				$d['ist_verzoeg']=0.00;
			$txt=get_entry($d['ist_verzoeg']/3600, 'dezimal').$bez_std;
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text6']=columnClass(0, 'r').$txt.'&nbsp;';
			$z++;
		}

		if($dkst > 0)
		{
			// Leerzeile
			fxf_maskColumnBorder($fa, 1, -1, $mc, $z, true);
			$z++;
		}

		if(($dkst == 1) || ($dkst == 3))
		{
			// Zeile 12
			// ...Int. PRahmen
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text1']=literalClass($lit_budgetrahmen_pers_int, false);
			if($d['budgetrahmen_pers_int'] != 0.00)
				$txt=get_entry($d['budgetrahmen_pers_int'], 'dezimal').$bez_waehrung;
			else
				$txt=$bez_ohne;
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text2']=columnClass(0, 'r', false).$txt.'&nbsp;';
			// ...Int. MRahmen
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text3']=literalClass($lit_budgetrahmen_mat_int, false);
			if($d['budgetrahmen_mat_int'] != 0.00)
				$txt=get_entry($d['budgetrahmen_mat_int'], 'dezimal').$bez_waehrung;
			else
				$txt=$bez_ohne;
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text4']=columnClass(0, 'r', false).$txt.'&nbsp;';
			// ...Int. GRahmen
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text5']=literalClass($lit_budgetrahmen_ges_int, false);
			$budgetrahmen_ges_int=$d['budgetrahmen_pers_int']+$d['budgetrahmen_mat_int'];
			if($budgetrahmen_ges_int != 0.00)
				$txt=get_entry($budgetrahmen_ges_int, 'dezimal').$bez_waehrung;
			else
				$txt=$bez_ohne;
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text6']=columnClass(0, 'r', false).$txt.'&nbsp;';
			$z++;
		}

		if((($dkst == 1) || ($dkst == 3)) && $ausf)
		{
			// Zeile 13-17
			foreach($budget_arten as $bart)
			{
				// ...Int. ? Personen
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text1']=literalClass(${'lit_budget_pers_int_'.$bart});
				if(!$d['budget_pers_int_'.$bart])
					$d['budget_pers_int_'.$bart]=0.00;
				$txt=get_entry($d['budget_pers_int_'.$bart], 'dezimal').$bez_waehrung;
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text2']=columnClass(0, 'r').$txt.'&nbsp;';
				// ...Int. ? Material
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text3']=literalClass(${'lit_budget_mat_int_'.$bart});
				if(!$d['budget_mat_int_'.$bart])
					$d['budget_mat_int_'.$bart]=0.00;
				$txt=get_entry($d['budget_mat_int_'.$bart], 'dezimal').$bez_waehrung;
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text4']=columnClass(0, 'r').$txt.'&nbsp;';
				// ...Int. ? Gesamtb.
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text5']=literalClass(${'lit_budget_ges_int_'.$bart});
				$budget_ges_int=$d['budget_pers_int_'.$bart]+$d['budget_mat_int_'.$bart];
				if(!$budget_ges_int)
					$budget_ges_int=0.00;
				$txt=get_entry($budget_ges_int, 'dezimal').$bez_waehrung;
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text6']=columnClass(0, 'r').$txt.'&nbsp;';
				$z++;
			}
		}

		if(($dkst == 1) || ($dkst == 3))
		{
			// Zeile 18
			// ...Int. Rest Personen
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text1']=literalClass($lit_budget_pers_int_rest);
			$budget_pers_int_rest=$d['budget_pers_int_gen']-$d['budget_pers_int_verpl']-$d['budget_pers_int_verbr'];
			if(!$budget_pers_int_rest)
				$budget_pers_int_rest=0.00;
			$txt=get_entry($budget_pers_int_rest, 'dezimal').$bez_waehrung;
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text2']=columnClass(0, 'r').$txt.'&nbsp;';
			// ...Int. Rest Material
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text3']=literalClass($lit_budget_mat_int_rest);
			$budget_mat_int_rest=$d['budget_mat_int_gen']-$d['budget_mat_int_verpl']-$d['budget_mat_int_verbr'];
			if(!$budget_mat_int_rest)
				$budget_mat_int_rest=0.00;
			$txt=get_entry($budget_mat_int_rest, 'dezimal').$bez_waehrung;
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text4']=columnClass(0, 'r').$txt.'&nbsp;';
			// ...Int. Rest Gesamtb.
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text5']=literalClass($lit_budget_ges_int_rest);
			$budget_ges_int_rest=$budget_pers_int_rest+$budget_mat_int_rest;
			if(!$budget_ges_int_rest)
				$budget_ges_int_rest=0.00;
			$txt=get_entry($budget_ges_int_rest, 'dezimal').$bez_waehrung;
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text6']=columnClass(0, 'r').$txt.'&nbsp;';
			$z++;
		}

		if(($dkst == 2) || ($dkst == 3))
		{
			// Zeile 19
			// ...Ext. PRahmen
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text1']=literalClass($lit_budgetrahmen_pers_ext);
			if($d['budgetrahmen_pers_ext'] != 0.00)
				$txt=get_entry($d['budgetrahmen_pers_ext'], 'dezimal').$bez_waehrung;
			else
				$txt=$bez_ohne;
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text2']=columnClass(0, 'r').$txt.'&nbsp;';
			// ...Ext. MRahmen
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text3']=literalClass($lit_budgetrahmen_mat_ext);
			if($d['budgetrahmen_mat_ext'] != 0.00)
				$txt=get_entry($d['budgetrahmen_mat_ext'], 'dezimal').$bez_waehrung;
			else
				$txt=$bez_ohne;
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text4']=columnClass(0, 'r').$txt.'&nbsp;';
			// ...Ext. GRahmen
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text5']=literalClass($lit_budgetrahmen_ges_ext);
			$budgetrahmen_ges_ext=$d['budgetrahmen_pers_ext']+$d['budgetrahmen_mat_ext'];
			if($budgetrahmen_ges_ext != 0.00)
				$txt=get_entry($budgetrahmen_ges_ext, 'dezimal').$bez_waehrung;
			else
				$txt=$bez_ohne;
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text6']=columnClass(0, 'r').$txt.'&nbsp;';
			$z++;
		}

		if((($dkst == 2) || ($dkst == 3)) && $ausf)
		{
			// Zeile 20-24
			foreach($budget_arten as $bart)
			{
				// ...Ext. ? Personen
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text1']=literalClass(${'lit_budget_pers_ext_'.$bart});
				if(!$d['budget_pers_ext_'.$bart])
					$d['budget_pers_ext_'.$bart]=0.00;
				$txt=get_entry($d['budget_pers_ext_'.$bart], 'dezimal').$bez_waehrung;
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text2']=columnClass(0, 'r').$txt.'&nbsp;';
				// ...Ext. ? Material
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text3']=literalClass(${'lit_budget_mat_ext_'.$bart});
				if(!$d['budget_mat_ext_'.$bart])
					$d['budget_mat_ext_'.$bart]=0.00;
				$txt=get_entry($d['budget_mat_ext_'.$bart], 'dezimal').$bez_waehrung;
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text4']=columnClass(0, 'r').$txt.'&nbsp;';
				// ...Ext. ? Gesamtb.
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text5']=literalClass(${'lit_budget_ges_ext_'.$bart});
				$budget_ges_ext=$d['budget_pers_ext_'.$bart]+$d['budget_mat_ext_'.$bart];
				if(!$budget_ges_ext)
					$budget_ges_ext=0.00;
				$txt=get_entry($budget_ges_ext, 'dezimal').$bez_waehrung;
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text6']=columnClass(0, 'r').$txt.'&nbsp;';
				$z++;
			}
		}

		if(($dkst == 2) || ($dkst == 3))
		{
			// Zeile 25
			// ...Ext. Rest Personen
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text1']=literalClass($lit_budget_pers_ext_rest);
			$budget_pers_ext_rest=$d['budget_pers_ext_gen']-$d['budget_pers_ext_verpl']-$d['budget_pers_ext_verbr'];
			if(!$budget_pers_ext_rest)
				$budget_pers_ext_rest=0.00;
			$txt=get_entry($budget_pers_ext_rest, 'dezimal').$bez_waehrung;
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text2']=columnClass(0, 'r').$txt.'&nbsp;';
			// ...Ext. Rest Material
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text3']=literalClass($lit_budget_mat_ext_rest);
			$budget_mat_ext_rest=$d['budget_mat_ext_gen']-$d['budget_mat_ext_verpl']-$d['budget_mat_ext_verbr'];
			if(!$budget_mat_ext_rest)
				$budget_mat_ext_rest=0.00;
			$txt=get_entry($budget_mat_ext_rest, 'dezimal').$bez_waehrung;
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text4']=columnClass(0, 'r').$txt.'&nbsp;';
			// ...Ext. Rest Gesamtb.
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text5']=literalClass($lit_budget_ges_ext_rest);
			$budget_ges_ext_rest=$budget_pers_ext_rest+$budget_mat_ext_rest;
			if(!$budget_ges_ext_rest)
				$budget_ges_ext_rest=0.00;
			$txt=get_entry($budget_ges_ext_rest, 'dezimal').$bez_waehrung;
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text6']=columnClass(0, 'r').$txt.'&nbsp;';
			$z++;
		}

		// Leerzeile
		fxf_maskColumnBorder($fa, 1, -1, $mc, $z, true);
		$z++;

		// Zeile 26
		// ...Beschreibung
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text1']=literalClass($lit_beschreibung, false);
		$txt=text_wrap($d['beschreibung'], true, 110);
		if(!strlen((string)$txt))
			$txt=$bez_ohne;
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text2']=columnClass(5, 'l', false).$txt;
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text6']=$colclle.$txt;
		$z++;

		if($ausf)
		{
			// Zeile 27
			// ...Anmerkungen
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text1']=literalClass($lit_anmerkung);
			$txt=text_wrap($d['anmerkung'], true, 110);
			if(!strlen((string)$txt))
				$txt=$bez_ohne;
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text2']=columnClass(5).$txt;
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text6']=$colclle.$txt;
			$z++;

			// Zeile 28
			// ...Interne Anmerkung
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text1']=literalClass($lit_anmerkungintern);
			$txt=text_wrap($d['anmerkungintern'], true, 110);
			if(!strlen((string)$txt))
				$txt=$bez_ohne;
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text2']=columnClass(5).$txt;
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text6']=$colclle.$txt;
			$z++;

			// Leerzeile
			fxf_maskColumnBorder($fa, 1, -1, $mc, $z, true);
			$z++;

			// Zeile 29
			// ...Zugeord. Personen
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text1']=literalClass($lit_personen, false);
			if(is_array($hr) && is_array($hr[$d['projekt_id']]))
			{
				$txt='';
				foreach($hr[$d['projekt_id']] as $pid => $pd)
				{
					if(strlen((string)$txt))
						$txt .= '<br />';
					$txt .= $perarr[$pid].'&nbsp;&nbsp;['.get_entry($pd['prozent']*100, 'dezimal').'%]';
				}
			}
			else
				$txt=$bez_ohne;
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text2']=columnClass(5, 'l', false).$txt;
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text6']=$colclln.$txt.'&nbsp;';
			$z++;
		}

		// Leerzeile
		fxf_maskColumnBorder($fa, 1, -1, $mc, $z, true);
		$z++;

		// Historie: Zeile 30-
		if($Projekt_HistoryID)
		{
			// ...Projekt-Historie
			$txt='';
			$sql="SELECT * FROM projekte_history WHERE projekt_id=".$d['projekt_id']." AND mandanten_id=".$GLOBALS['fxpglobals']['client']." ORDER BY zeitstempel, aktion, autoid";
			$tmp=db_values($sql, '*');
//fxDebug($tmp, $sql, 0);
			if(fxIsArray($tmp))
			{
				// Leerzeile
				fxf_maskColumnBorder($fa, 0, -1, $mc, $z, true);
				$z++;

				// Überschriftszeile über komplette Breite
				fxf_listSectHeader($mc, $z, '&nbsp;'.$lit_phistorie, 'nv_text1', 'nv_text6', 6);
				$z++;

				$src=array();
				foreach($tmp as $t)
				{
					$txt_date='&nbsp;<img src="'.$GLOBALS['gfxpath'].'tim_b_16x16.png" align="top">&nbsp;<b>'.get_entry($t['zeitstempel'], 'datetimesec').'</b>&nbsp;';
					$txt_person=hist_pers($t['aenderungs_id'], $t['zeitstempel']).'&nbsp;&nbsp;<i class="lightgrey">['.hist_trans($t['trans_akt_id']).']</i>';
					$txt_field='';
					$txt_old='';
					$txt_new='';
					if(!sizeof($src))
					{
						if($t['aktion'] == FXP_CREATE)
							$txt_field='&rarr; <b>['.$bez_hist[0].']</b>';	// Projekterstellung
						else
							$txt_field='$rarr; <b>['.$bez_hist[1].']</b>';	// Projektbasis
						$src=$t;

						$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text1']=literalClass('', false, 'l', 2, 'bg_lightestgrey').$txt_date;
						$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text2']=$colclln.$txt_date;
						$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text3']=columnClass(4, 'l', false, 'bg_lightestgrey').$txt_person;
						$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text6']=$colclln.$txt_person;
						$z++;
						$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text1']=literalClass('', false, 'l', 2);
						$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text2']=$colclln;
						$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text3']=columnClass(4, 'l', false).$txt_field;
						$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text6']=$colclln.$txt_field;
						$z++;
					}
					else
					{
						$dateline=false;
						$btop=false;
						foreach($t as $field => $value)
						{
							$valid_field=true;
							if(($field == 'autoid') || ($field == 'aktion') || ($field == 'trans_akt_id') || ($field == 'zeit_id') || ($field == 'budget_id') || ($field == 'historie') || ($field == 'transid') || ($field == 'aenderungs_id') || ($field == 'zeitstempel'))
								$valid_field=false;
							if($valid_field && (!$aufw || ($d['projektart'] < FXP_PRJ_TASK)) && (($field == 'aufwand_soll') || ($field == 'aufwand_ist')))
								$valid_field=false;
							if($valid_field && $aufw && !$ausf && ($field == 'aufwand_ist'))
								$valid_field=false;

							if($valid_field)
							{
								if($value != $src[$field])
								{
									$txt_field=hist_field($field);
									$txt_old='<font class="grey">'.hist_value($field, $src[$field], $t['autoid'], $t['zeitstempel']).'</font><div style="float:right;color:#888;">&nbsp;&rarr;&nbsp;</div>';
									$txt_new=hist_value($field, $value, $t['autoid'], $t['zeitstempel']);
									$src[$field]=$value;

									if(!$dateline)
									{
										$dateline=true;
										$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text1']=literalClass('', true, 'l', 2, 'bg_lightestgrey').$txt_date;
										$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text2']=$colclln.$txt_date;
										$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text3']=columnClass(4, 'l', true, 'bg_lightestgrey').$txt_person;
										$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text6']=$colclln.$txt_person;
										$z++;
									}

									$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text1']=literalClass($txt_field, false, 'r', 2);
									$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text2']=$colclln.'&nbsp;'.$txt_field.':&nbsp;';
									$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text3']=columnClass(2, 'l', $btop).$txt_old;
									$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text4']=$colclln.$txt_old;
									$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text5']=columnClass(2, 'l', $btop).$txt_new;
									$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text6']=$colclln.$txt_new;
									$z++;

									$btop=true;
								}
							}
						}
					}
				}

				// Leerzeile
				fxf_maskColumnBorder($fa, 1, -1, $mc, $z, true);
				$z++;
			}

			// ...Budget-Historie
			if($kost)
			{
				$sql="SELECT * FROM budget_summe_history WHERE projekt_id=".$d['projekt_id']." AND mandanten_id=".$GLOBALS['fxpglobals']['client']." ORDER BY zeitstempel, aktion, autoid";
				$tmp=db_values($sql, '*');
//fxDebug($tmp, $sql, 0);
				if(fxIsArray($tmp))
				{
					// Leerzeile
					fxf_maskColumnBorder($fa, 0, -1, $mc, $z, true);
					$z++;

					// Überschriftszeile über komplette Breite
					fxf_listSectHeader($mc, $z, '&nbsp;'.$lit_bhistorie, 'nv_text1', 'nv_text6', 6);
					$z++;

					$src=array();
					foreach($tmp as $t)
					{
						$txt_date='&nbsp;<img src="'.$GLOBALS['gfxpath'].'tim_b_16x16.png" align="top">&nbsp;<b>'.get_entry($t['zeitstempel'], 'datetimesec').'</b>&nbsp;';
						$txt_person=hist_pers($t['aenderungs_id'], $t['zeitstempel']).'&nbsp;&nbsp;<i class="lightgrey">['.hist_trans($t['trans_akt_id']).']</i>';
						$txt_field='';
						$txt_old='';
						$txt_new='';
						if(!sizeof($src))
						{
							if($t['aktion'] == FXP_CREATE)
								$txt_field='&rarr; <b>['.$bez_hist[2].']</b>';	// Budgeterstellung
							else
								$txt_field='$rarr; <b>['.$bez_hist[3].']</b>';	// Budgetbasis
							$src=$t;

							$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text1']=literalClass('', false, 'l', 2, 'bg_lightestgrey').$txt_date;
							$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text2']=$colclln.$txt_date;
							$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text3']=columnClass(4, 'l', false, 'bg_lightestgrey').$txt_person;
							$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text6']=$colclln.$txt_person;
							$z++;
							$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text1']=literalClass('', false, 'l', 2);
							$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text2']=$colclln;
							$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text3']=columnClass(4, 'l', false).$txt_field;
							$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text6']=$colclln.$txt_field;
							$z++;
						}
						else
						{
							$dateline=false;
							$btop=false;
							foreach($t as $field => $value)
							{
								$valid_field=true;
								if(($field == 'autoid') || ($field == 'aktion') || ($field == 'trans_akt_id') || ($field == 'projekt_status') || ($field == 'projektart') || ($field == 'historie') || ($field == 'transid') || ($field == 'aenderungs_id') || ($field == 'zeitstempel'))
									$valid_field=false;
								if($valid_field && ($d['projektart'] == FXP_PRJ_TASK) && (substr((string)$field, 0, 13) == 'budgetrahmen_'))
									$valid_field=false;
								if($valid_field && ($d['projektart'] < FXP_PRJ_TASK) && ((substr((string)$field, 0, 2) == 'ek') || (substr((string)$field, 0, 2) == 'vk') || (substr((string)$field, 0, 7) == 'budget_') || (substr((string)$field, 0, 9) == 'rechnung_')))
									$valid_field=false;
								if($valid_field && !$ausf && ((substr((string)$field, -5) == 'verbr') || (substr((string)$field, -5) == 'abger') || (substr((string)$field, 0, 9) == 'rechnung_')))
									$valid_field=false;

								if($valid_field)
								{
									if($value != $src[$field])
									{
										$txt_field=hist_field($field);
										$txt_old='<font class="grey">'.hist_value($field, $src[$field], $t['autoid'], $t['zeitstempel']).'</font><div style="float:right;color:#888;">&nbsp;&rarr;&nbsp;</div>';
										$txt_new=hist_value($field, $value, $t['autoid'], $t['zeitstempel']);
										$src[$field]=$value;

										if(!$dateline)
										{
											$dateline=true;
											$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text1']=literalClass('', true, 'l', 2, 'bg_lightestgrey').$txt_date;
											$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text2']=$colclln.$txt_date;
											$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text3']=columnClass(4, 'l', true, 'bg_lightestgrey').$txt_person;
											$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text6']=$colclln.$txt_person;
											$z++;
										}

										$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text1']=literalClass($txt_field, false, 'r', 2);
										$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text2']=$colclln.'&nbsp;'.$txt_field.':&nbsp;';
										$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text3']=columnClass(2, 'l', $btop).$txt_old;
										$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text4']=$colclln.$txt_old;
										$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text5']=columnClass(2, 'l', $btop).$txt_new;
										$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text6']=$colclln.$txt_new;
										$z++;

										$btop=true;
									}
								}
							}
						}
					}

					// Leerzeile
					fxf_maskColumnBorder($fa, 1, -1, $mc, $z, true);
					$z++;
				}
			}

			// ...Ressourcen-Historie
			if($ausf && ($d['projektart'] == FXP_PRJ_TASK))
			{
				$txt='';
				$sql="SELECT * FROM maplanung_history WHERE projekt_id=".$d['projekt_id']." AND mandanten_id=".$GLOBALS['fxpglobals']['client']." ORDER BY zeitstempel, aktion, autoid";
				$tmp=db_values($sql, '*');
//fxDebug($tmp, $sql, 0);
				if(fxIsArray($tmp))
				{
					// Leerzeile
					fxf_maskColumnBorder($fa, 0, -1, $mc, $z, true);
					$z++;

					// Überschriftszeile über komplette Breite
					fxf_listSectHeader($mc, $z, '&nbsp;'.$lit_rhistorie, 'nv_text1', 'nv_text6', 6);
					$z++;

					$src=array();
					foreach($tmp as $t)
					{
						$pers=(int)$t['personen_id'];
						$txt_date='&nbsp;<img src="'.$GLOBALS['gfxpath'].'tim_b_16x16.png" align="top">&nbsp;<b>'.get_entry($t['zeitstempel'], 'datetimesec').'</b>&nbsp;';
						$txt_person=hist_pers($t['aenderungs_id'], $t['zeitstempel']).'&nbsp;&nbsp;<i class="lightgrey">['.hist_trans($t['trans_akt_id']).']</i>';

						if(!sizeof($src) || !isset($src[$pers]))
						{
							$txt_action='&rarr; <b>['.$bez_hist[4].']</b>:&nbsp;'.hist_pers($pers, $t['zeitstempel']);	// Ressourcenzuordnung

							$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text1']=literalClass('', false, 'l', 2, 'bg_lightestgrey').$txt_date;
							$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text2']=$colclln.$txt_date;
							$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text3']=columnClass(4, 'l', false, 'bg_lightestgrey').$txt_person;
							$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text6']=$colclln.$txt_person;
							$z++;
							$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text1']=literalClass('', false, 'l', 2);
							$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text2']=$colclln;
							$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text3']=columnClass(4, 'l', false).$txt_action;
							$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text6']=$colclln.$txt_action;
							$z++;

							foreach($pdarray as $pdfield)
							{
								$txt_field=hist_field($pdfield);
								$txt_old='';
								$txt_new=hist_value($pdfield, $t[$pdfield], $t['autoid'], $t['zeitstempel']);

								$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text1']=literalClass($txt_field, false, 'r', 2);
								$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text2']=$colclln.'&nbsp;'.$txt_field.':&nbsp;';
								$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text3']=columnClass(2).$txt_old;
								$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text4']=$colclln.$txt_old;
								$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text5']=columnClass(2).$txt_new;
								$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text6']=$colclln.$txt_new;
								$z++;

								$btop=true;
							}

							$src[$pers]=$t;
						}
						else if($t['aktion'] == FXP_DELETE)
						{
							$txt_action='&rarr; <b>['.$bez_hist[6].']</b>:&nbsp;'.hist_pers($pers, $t['zeitstempel']);	// Ressourcenentfernung

							$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text1']=literalClass('', false, 'l', 2, 'bg_lightestgrey').$txt_date;
							$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text2']=$colclln.$txt_date;
							$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text3']=columnClass(4, 'l', false, 'bg_lightestgrey').$txt_person;
							$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text6']=$colclln.$txt_person;
							$z++;
							$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text1']=literalClass('', false, 'l', 2);
							$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text2']=$colclln;
							$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text3']=columnClass(4, 'l', false).$txt_action;
							$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text6']=$colclln.$txt_action;
							$z++;

							unset($src[$pers]);
						}
						else
						{
							$dateline=false;
							foreach($t as $field => $value)
							{
								$valid_field=true;
								if(($field == 'autoid') || ($field == 'aktion') || ($field == 'trans_akt_id') || ($field == 'projekt_status') || ($field == 'maplanung_id') || ($field == 'e_mail') || ($field == 'benutzergr_id') || ($field == 'historie') || ($field == 'transid') || ($field == 'aenderungs_id') || ($field == 'zeitstempel'))
									$valid_field=false;

								if($valid_field)
								{
									if($value != $src[$pers][$field])
									{
										$txt_field=hist_field($field);
										$txt_old='<font class="grey">'.hist_value($field, $src[$pers][$field], $t['autoid'], $t['zeitstempel']).'</font><div style="float:right;color:#888;">&nbsp;&rarr;&nbsp;</div>';
										$txt_new=hist_value($field, $value, $t['autoid'], $t['zeitstempel']);
										$src[$pers][$field]=$value;

										if(!$dateline)
										{
											$dateline=true;
											$txt_action='&rarr; <b>['.$bez_hist[5].']</b>:&nbsp;'.hist_pers($pers, $t['zeitstempel']);	// Ressourcenänderung

											$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text1']=literalClass('', true, 'l', 2, 'bg_lightestgrey').$txt_date;
											$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text2']=$colclln.$txt_date;
											$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text3']=columnClass(4, 'l', true, 'bg_lightestgrey').$txt_person;
											$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text6']=$colclln.$txt_person;
											$z++;
											$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text1']=literalClass('', false, 'l', 2);
											$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text2']=$colclln;
											$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text3']=columnClass(4, 'l', false).$txt_action;
											$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text6']=$colclln.$txt_action;
											$z++;
										}

										$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text1']=literalClass($txt_field, false, 'r', 2);
										$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text2']=$colclln.'&nbsp;'.$txt_field.':&nbsp;';
										$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text3']=columnClass(2).$txt_old;
										$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text4']=$colclln.$txt_old;
										$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text5']=columnClass(2).$txt_new;
										$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text6']=$colclln.$txt_new;
										$z++;
									}
								}
							}
						}
					}

					// Leerzeile
					fxf_maskColumnBorder($fa, 1, -1, $mc, $z, true);
					$z++;
				}
			}
		}
	}
}


////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/**
 * Get the display class for a literal column
 *
 * @param string  $literal - Optional parameter (default = ''):    Literal to display
 * @param boolean $btop    - Optional parameter (default = true):  Top border?
 * @param string  $align   - Optional parameter (default = 'r'):   Align attribute -> 'r'=>Right, 'c'=>Center, else Left
 * @param integer $colspan - Optional parameter (default = 0):     Colspan attribute
 *
 * @return ???RETURN???
 */
function literalClass($literal='', $btop=true, $align='r', $colspan=0, $cls='rv2')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$literal=trim((string)$literal);
	if(strlen((string)$literal))
		$literal='&nbsp;'.$literal.':&nbsp;';

	$align=strtolower(substr((string)$align,0,1));
	if($align == 'c')
		$align=' align="center"';
	else if($align == 'r')
		$align=' align="right"';
	else
		$align='';

	if($btop)
		$btop='border-top:1px solid #'.$GLOBALS['msk_nbcol'].';';
	else
		$btop='';

	$colspan=(int)$colspan;
	if($colspan > 1)
	{
		$colspan=' colspan="'.$colspan.'"';
		$colend='"><!--';
	}
	else
	{
		$colspan=' width="160"';
		$colend='';
	}

	return '^class_'.$cls.' darkgrey"'.$colspan.' nowrap'.$align.' valign="middle" height="'.$GLOBALS['nv_text1_height'].'" style="padding:0 3px;border:0;'.$btop.$colend.'|~'.$literal;
}

/**
 * Get the display class for a column
 *
 * @param integer $colspan - Optional parameter (default = 0):     Colspan attribute
 * @param string  $align   - Optional parameter (default = 'l'):   Align attribute -> 'r'=>Right, 'c'=>Center, else Left
 * @param boolean $btop    - Optional parameter (default = true):  Top border?
 *
 * @return ???RETURN???
 */
function columnClass($colspan=0, $align='l', $btop=true, $cls='bg_white')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$colspan=(int)$colspan;
	if($colspan > 1)
	{
		$colspan=' colspan="'.$colspan.'"';
		$colend='"><!--';
	}
	else
	{
		$colspan='';
		$colend='';
	}

	$align=strtolower(substr((string)$align,0,1));
	if($align == 'c')
		$align=' align="center"';
	else if($align == 'r')
		$align=' align="right"';
	else
		$align='';

	if($btop)
		$btop='border-top:1px solid #'.$GLOBALS['msk_nbcol'].';';
	else
		$btop='';

	return '^class_'.$cls.'"'.$colspan.$align.' valign="middle" height="'.$GLOBALS['nv_text1_height'].'" style="padding:0 3px;'.$btop.'border-right:1px solid #'.$GLOBALS['msk_nbcol'].';'.$colend.'|~';
}

/**
 * ???FUNCTION???
 *
 * @param various $pers      - Mandatory parameter: ???PARAMETER???
 * @param various $timestamp - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function hist_pers($pers, $timestamp)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$pers=(int)$pers;
	if($pers)
	{
		$sql="SELECT pname, vorname, personen_initialen FROM personen_history WHERE personen_id=".$pers." AND mandanten_id=".$GLOBALS['fxpglobals']['client']." AND zeitstempel<='".$timestamp."'";
		$tmp=db_values($sql);
		if(!is_array($tmp))
		{
			$sql="SELECT pname, vorname, personen_initialen FROM personen WHERE personen_id=".$pers." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$tmp=db_values($sql);
		}
		if(is_array($tmp))
		{
			$name=trim((string)$tmp['pname']);
			$fnam=trim((string)$tmp['vorname']);
			if(strlen((string)$fnam))
				$name .= ', '.$fnam;
			$init=trim((string)$tmp['personen_initialen']);
			if(strlen((string)$init))
				$name .= ' ('.$init.')';

			return $name;
		}
	}

	return $GLOBALS['bez_ohne'];
}

/**
 * Return the program function name from its id
 *
 * @param integer $trans - Mandatory parameter:  Program function id
 *
 * @return string Program function name
 */
function hist_trans($trans)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!isset($GLOBALS['hist_trans']) || !is_array($GLOBALS['hist_trans']))
		$GLOBALS['hist_trans']=array();

	$trans=(int)$trans;
	if(!isset($GLOBALS['hist_trans'][$trans]))
		$GLOBALS['hist_trans'][$trans]=trPath($trans);

	return $GLOBALS['hist_trans'][$trans];
}

/**
 * ???FUNCTION???
 *
 * @param various $field - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function hist_field($field)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!isset($GLOBALS['hist_fields']) || !is_array($GLOBALS['hist_fields']))
		$GLOBALS['hist_fields']=array();

	if(!isset($GLOBALS['hist_fields'][$field]))
	{
		$sql="SELECT t.literal_20 FROM texte t, felder f WHERE ".db_like('f.feldname', $field)." AND t.id_referenz=f.id AND t.id_ref_art=3 AND t.id_sprache=".$GLOBALS['fxpglobals']['lang']." AND f.mandanten_id IN (0,".$GLOBALS['fxpglobals']['client'].")";
		$GLOBALS['hist_fields'][$field]=db_value($sql).' <i class=s2>['.$field.']</i>';
	}

	return $GLOBALS['hist_fields'][$field];
}

/**
 * ???FUNCTION???
 *
 * @param various $field     - Mandatory parameter: ???PARAMETER???
 * @param various $value     - Mandatory parameter: ???PARAMETER???
 * @param various $autoid    - Mandatory parameter: ???PARAMETER???
 * @param various $timestamp - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function hist_value($field, $value, $autoid, $timestamp)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if((substr((string)$field, 0, 13) == 'budgetrahmen_') || (substr((string)$field, 0, 7) == 'budget_') || (substr((string)$field, 0, 9) == 'rechnung_'))
		$value=get_entry($value, "dezimal").$GLOBALS['bez_waehrung'];
	else
	{
		switch($field)
		{
			case 'vorgangsnummer':
				$value=no_null($value);
			break;

			case 'projekt_id':
			case 'mutterprojekt_id':
			case 'elter':
			case 'abhaengigkeit':
				$value=(int)$value;
				if($value)
				{
					$sql="SELECT vorgangsnummer, name_projekt FROM projekte_history WHERE projekt_id=".$value." AND mandanten_id=".$GLOBALS['fxpglobals']['client']." AND autoid<".$autoid;
					$tmp=db_values($sql);
					if(is_array($tmp))
						$value=no_null($tmp['vorgangsnummer']).': '.$tmp['name_projekt'];
				}
				if(!$value)
				{
					if($field == 'elter')
						$value=$GLOBALS['bez_ohne_zuordnung'];
					else
						$value=$GLOBALS['bez_ohne'];
				}
			break;

			case 'aufwand_ist':
			case 'aufwand_soll':
			case 'aufwand_std':
			case 'verlauf':
			case 'aufwand_todo':
			case 'max_zeit_aufw':
				if($value)
					$value=get_entry($value/3600, "dezimal").$GLOBALS['bez_std'];
				else if($field == 'aufwand_soll')
					$value=$GLOBALS['bez_ohne'];
				else if($field == 'max_zeit_aufw')
					$value=$GLOBALS['bez_unbegrenzt'];
				else
					$value=get_entry(0, "dezimal").$GLOBALS['bez_std'];
			break;

			case 'deadline':
				if(strlen((string)$value))
					$value=get_entry($value, "datum");
				else
					$value='<i>['.$GLOBALS['unbegrenzt'].']</i>';
			break;

			case 'projektart':
			case 'projekt_status':
			case 'prioritaet':
				$value=$GLOBALS['arr_'.$field][(int)$value];
			break;

			case 'kategorie_id':
			case 'abrechnungsart':
			case 'abrechnungsart_mat':
			case 'budgettyp':
				if(!$value)
					$value=$GLOBALS['bez_ohne'];
				else
					$value=meldung($value, false);
			break;

			case 'beschreibung':
			case 'anmerkung':
			case 'anmerkungintern':
			case 'ref_nummer':
			case 'name_projekt':
				if(!strlen((string)$value))
					$value=$GLOBALS['bez_ohne'];
				else
					$value=text_wrap($value, true, 50);
			break;

			case 'kunde':
			case 'projektleiter':
			case 'vertreter':
			case 'ansprechpartner':
			case 'projektmanager':
				$value=hist_pers($value, $timestamp);
			break;

			case 'maart_id':
			case 'mafaehigkeit_id':
				$value=(int)$value;
				if($value)
				{
					$column=substr((string)$field, 0, -2).'name';
					$table	= 'ma_'.substr((string)$field, 2, -3);
					$sql="SELECT ".$column." FROM ".$table." WHERE ".$field."=".$value." AND id_sprache=".$GLOBALS['fxpglobals']['lang']." AND mandanten_id IN (0,".$GLOBALS['fxpglobals']['client'].")";
					$tmp=db_value($sql);
					if($tmp)
						$value=$tmp;
				}
				if(!$value)
					$value=$GLOBALS['bez_ohne'];
			break;

			case 'ek_voraus':
			case 'vk_voraus':
			case 'ek':
			case 'vk':
				$value=get_entry(get_double($value), "dezimal").$GLOBALS['bez_waehrung_std'];
			break;

			case 'soll_beginn':
			case 'soll_ende':
				$value=get_entry($value, "datum");
			break;

			case 'prozent':
			case 'auslastung':
				$value=get_entry($value*100.00, "dezimal");
			break;
		}
	}

	return $value;
}
?>