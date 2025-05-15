<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : freigabe_pre.inc                                             //
// Version     : 24.1                                                         //
// Begin       : 2020-11-12                                                   //
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
 * PF 156: Time Approval
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

if($_POST['Button_Anzeigen'] || $_POST['Button_Speichern'])
{
	// Form setzen für Hidden
	setForm('datmask_3');

	// Variablen
	$amonth=(int)substr((string)$GLOBALS['datetime'],4,2);
	$ayear=(int)substr((string)$GLOBALS['datetime'],0,4);
	$tcbcnt=0;
	$maxentry=(int)$GLOBALS['fxpglobals']['settings']['maxliste'];	// Maximale Anzahl der anzuzeigenden Zeiteinträge
	$mmxh=256;	// Maximale Bilderhöhe

	// Häufig benötigte Strings
	$datchange		= meldung(1496);			// Datensatz ändern
	$unbekannt		= meldung(1449);			// [Unbekannt]
	$gleitzeit		= meldung(1525);			// [Gleitzeit]
	$leer			= meldung(794);				// -leer-
	$indfp			= '<b class=grey>'.ucfirst(meldung(3280)).'</b>';	// Bereits indirekt abgerechnet (Festpreis!)
	$abger			= '<b class=black>'.ucfirst(meldung(1686)).'</b>';	// Bereits abgerechnet
	$nverr			= '<b class=red>'.ucfirst(meldung(3275)).'</b>';	// Nicht verrechenbar
	$uebertrag		= meldung(1637, false, '');	// Übertrag
	$mitfahrer_str	= meldung(2808, false);		// Mitfahrer
	$weekdays		= fxf_getWeekdayNames(2);
	$stt			= explode('|',meldung(3002,false));	// Master|Teilsumme|Hier klicken um zum Master-Eintrag zu springen.|Reisekosten-Eintrag bearbeiten|Zeiterfassungs-Eintrag bearbeiten|Zeitsummen-Eintrag bearbeiten|Materialverbrauchs-Eintrag bearbeiten|Reisekosten|Zeiterfassung|Materialverbrauch|Vorherigen Monat anzeigen|Nächser Monat anzeigen|Sofortmaterial|Sofortmaterials-Eintrag bearbeiten
//fxDebug($stt,'$stt');
	$ntr			= explode('|',meldung(3270,false));	// An folgenden Arbeitstagen im Anzeigezeitraum ist (noch) keine Zeiterfassung vorhanden|An allen Arbeitstagen im Anzeigezeitraum ist (noch) keine Zeiterfassung vorhanden
//fxDebug($ntr,'$ntr');

	$ico_rchng='<div style="float:right;margin:3px;margin-bottom:0;cursor:help;"><img src="'.$GLOBALS['gfxpath'].'wrn_b_13x13.png" width="13" height="13" tooltip="'.$abger.' &rarr; <b>{RN}</b>"></div>';
	$ico_rchfp='<div style="float:right;margin:3px;margin-bottom:0;cursor:help;"><img src="'.$GLOBALS['gfxpath'].'wrn_b_13x13.png" width="13" height="13" tooltip="'.$indfp.' &rarr; <b>{RN}</b>" style="opacity:0.5;cursor:help;"></div>';

	// 1: MA, 2: PL, 3: VW/GF
	$txt_rl=array(1=>array('txt'=>meldung(2278), 'col'=>'grey'), 2=>array('txt'=>meldung(2279), 'col'=>'blue'), 3=>array('txt'=>meldung(2280), 'col'=>'green'));

	// Reisekosten-Kategorien
	$rk_art=fxf_getReferenceValues(1884);
	$rk_kat=rk_kategorie_array(false);
	$rk_land=array();

	// Evtl. selbstdefinierten Begriff statt "Tagesangaben" (ID=1980) verwenden
	if(($GLOBALS['fxpglobals']['settings']['txt_tangaben'] !== NULL) && strlen((string)$GLOBALS['fxpglobals']['settings']['txt_tangaben']))
		$rk_kat[1980]=trim((string)$GLOBALS['fxpglobals']['settings']['txt_tangaben']);

	// Maskendefinitionen
	$index=true;
	$maske355_nw=true;
	// ...Navigation: Vorherigen Monat anzeigen
	$ly=(int)substr((string)$Zeitspanne_von,0,4);
	$lm=(int)substr((string)$Zeitspanne_von,4,2)-1;
	if(!$lm)
	{
		$lm=12;
		$ly--;
	}
	$sd=makeDate($ly,$lm,1);
	$ud=ultimo($sd);
	$maske355_head=fieldLinkImage('left', 'it_back', '?zvon='.$sd.'&zbis='.$ud, $stt[10].': <b>'.meldung(17+$lm).', '.$ly.'</b>', '', 'float:left;');
	// ...Navigation: Nächsten Monat anzeigen
	$ny=(int)substr((string)$Zeitspanne_bis,0,4);
	$nm=(int)substr((string)$Zeitspanne_bis,4,2)+1;
	if($nm == 13)
	{
		$nm=1;
		$ny++;
	}
	$sd=makeDate($ny,$nm,1);
	$ud=ultimo($sd);
	$maske355_head .= fieldLinkImage('left', 'it_next', '?zvon='.$sd.'&zbis='.$ud, $stt[11].': <b>'.meldung(17+$nm).', '.$ny.'</b>', '', 'float:left;margin-left:2px;');
	$maske355_head .= '&nbsp;&nbsp;<span class="maskht">'.get_text(355,'kurz',0,2).'</span>&nbsp;&nbsp;<font class="s2 lightgrey">('.get_entry($Zeitspanne_von,'datum').' - '.get_entry($Zeitspanne_bis,'datum').')</font>';

	// Checkbox-Alias
	maskClearLC();
	$delchk_alias='Status_Benutzer';

	// Personen
	if($Personen_ID)
	{
		$pids=(int)$Personen_ID;
		$Pers_Arr=array($pids => $Pers_Arr[$pids]);
	}
	else
		$pids=auswahl_Personen_ID(-1);

	// Projekte
	$tids=auswahl_Projekt_ID(-1);
	if($Projekt_ID)
		$tinf=inferior_vn($Projekt_ID);
	else
		$tinf='-1';

	// SQL's festlegen
	if($Zeitdaten || $Reisekosten)
	{
		// ...Zeitdaten + Reisekosten
		$sql_zselect  = "SELECT";
		$sql_zselect .= " z.personen_id, z.zeit_id, 0 AS id, z.projekt_id, z.ist_beginn, z.ist_ende, z.pause, z.zeitaufwand,";
		$sql_zselect .= " z.rk_art, z.land_id, z.rk_kategorie, z.rk_ort1, z.rk_ort2, z.rk_anznacht AS rk_mitfahrer, z.rk_km, z.belegnr,";
		$sql_zselect .= " z.nicht_abrechenbar, z.rk_nicht_abrechenbar, z.zeitart, z.zeiteinheit, z.freigabe, z.rechnungs_id, z.positions_id, z.rechnungs_id_rk, z.positions_id_rk,";
		$sql_zselect .= " t.taetigkeit_text, t.anmerkungintern, t.taetigkeit_id";

		$sql_zfrom  = "FROM";
		$sql_zfrom .= " zeitdaten z, taetigkeiten t, projekte p, personen pe ".$nl;

		$sql_zwhere  = "WHERE";
		$sql_zwhere .= " z.personen_id IN (".$pids.") AND z.zeitart IN ({za}) AND (z.zeiteinheit IS NULL OR z.zeiteinheit=0 OR z.zeitart=".FXP_TE_TIMEREC_SUM.")";
		$sql_zwhere .= " AND z.ist_beginn<='".substr((string)$Zeitspanne_bis,0,8)."240000' AND z.ist_ende>='".substr((string)$Zeitspanne_von,0,8)."000000'";
		// Nur Einträge anzeigen, die für ausgew. Kunden gebucht wurden?
		if($Kunde)
			$sql_zwhere .= " AND z.zkunde=".$Kunde;
		// Bereits freigegebene (oder abgerechnete) Datensätze ausblenden?
		if($freigabe_ignorieren)
			$sql_zwhere .= " AND (z.freigabe IS NULL OR z.freigabe=0) AND (z.rechnungs_id IS NULL OR z.rechnungs_id=0)";
		$sql_zwhere .= " AND z.personen_id=pe.personen_id AND z.projekt_id=p.projekt_id AND z.zeit_id=t.zeit_id";
		$sql_zwhere .= " AND z.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND t.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND pe.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND p.mandanten_id IN (0,".$GLOBALS['fxpglobals']['client'].")";
		if($Projekt_ID)
			$sql_zwhere .= " AND z.projekt_id IN (".$tids.") AND z.projekt_id IN (".$tinf.")";
		else
			$sql_zwhere .= " AND (z.projekt_id=0 OR z.projekt_id IN (".$tids."))";
		$sql_zwhere .= " ".$nl;

		$sql_zorder  = "ORDER BY";
		$sql_zorder .= " z.personen_id, z.ist_beginn, z.ist_ende";
	}

	// ...Material
	if($mit_SM)
	{
		// ...Verbrauchsmaterial
		$sql_vselect  = "SELECT";
		$sql_vselect .= " m.personen_id, 0 AS zeit_id, m.id, m.sachmittel_id, m.projekt_id, m.ist_beginn, m.ist_ende, m.sachmenge, m.beschreibung, m.anmerkung,";
		$sql_vselect .= " m.nicht_abrechenbar, m.freigabe, m.rechnungs_id, m.positions_id";

		$sql_vfrom  = "FROM";
		$sql_vfrom .= " materialverbrauch m, projekte p, personen pe ".$nl;

		$sql_vwhere  = "WHERE";
		$sql_vwhere .= " m.personen_id IN (".$pids.") AND m.ist_beginn<='".substr((string)$Zeitspanne_bis,0,8)."240000' AND m.ist_ende>='".substr((string)$Zeitspanne_von,0,8)."000000'";
		// Nur Einträge anzeigen, die für ausgew. Kunden gebucht wurden?
		if($Kunde)
			$sql_vwhere .= " AND m.kunde=".$Kunde;
		// Bereits freigegebene (oder abgerechnete) Datensätze ausblenden?
		if($freigabe_ignorieren)
			$sql_vwhere .= " AND (m.freigabe IS NULL OR m.freigabe=0) AND (m.rechnungs_id IS NULL OR m.rechnungs_id=0)";
		$sql_vwhere .= " AND m.personen_id=pe.personen_id AND m.projekt_id=p.projekt_id";
		$sql_vwhere .= " AND m.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND pe.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND p.mandanten_id IN (0,".$GLOBALS['fxpglobals']['client'].")";
		if($Projekt_ID)
			$sql_vwhere .= " AND m.projekt_id IN (".$tids.") AND m.projekt_id IN (".$tinf.")";
		else
			$sql_vwhere .= " AND (m.projekt_id=0 OR m.projekt_id IN (".$tids."))";
		$sql_vwhere .= " ".$nl;

		$sql_vorder  = "ORDER BY";
		$sql_vorder .= " m.personen_id, m.ist_beginn, m.ist_ende";

		// ...Sofortmaterial (bereits bezahlt)
		$sql_sselect  = "SELECT";
		$sql_sselect .= " m.angelegt_von, m.sachmittel_id, m.fuervorgang, m.soll_beginn, m.soll_ende, m.artikelnummer_int, m.artikelnummer_ext, m.name_sachmittel, m.verkaufseinheit_id,";
		$sql_sselect .= " m.sachmenge, m.beschreibung, m.freigabe, m.rechnungs_id, m.positions_id ";

		$sql_sfrom  = "FROM";
		$sql_sfrom .= " sachmittel m, projekte p".$nl;

		$sql_swhere  = "WHERE";
		$sql_swhere .= " m.verwendungsart=3149 AND m.sachmittelstatus=1237 AND m.mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$sql_swhere .= " AND m.soll_beginn<='".substr((string)$Zeitspanne_bis,0,8)."240000' AND m.soll_ende>='".substr((string)$Zeitspanne_von,0,8)."000000'";
		// Bereits freigegebene (oder abgerechnete) Datensätze ausblenden?
		if($freigabe_ignorieren)
			$sql_swhere .= "AND (m.freigabe IS NULL OR m.freigabe=0) AND (m.rechnungs_id IS NULL OR m.rechnungs_id=0)";
		// Projekt
		$sql_swhere .= " AND m.fuervorgang=p.projekt_id AND p.mandanten_id IN (0,".$GLOBALS['fxpglobals']['client'].")";
		if($Projekt_ID)
			$sql_swhere .= " AND m.fuervorgang IN (".$tids.") AND m.fuervorgang IN (".$tinf.")";
		else
			$sql_swhere .= " AND (m.fuervorgang=0 OR m.fuervorgang IN (".$tids."))";
		// VW+GF alle, sonst für ausgewählte Person oder PL/SP nur zu eigenen Vorgängen
		if(!$GLOBALS['fxpglobals']['persdat']['roles'][-1] && !$GLOBALS['fxpglobals']['persdat']['roles'][2])
			$sql_swhere .= " AND (m.angelegt_von IN (".$pids.") OR p.projektleiter=".$GLOBALS['fxpglobals']['person']." OR p.vertreter=".$GLOBALS['fxpglobals']['person'].")";

		// Nur Einträge anzeigen, die für ausgew. Kunden gebucht wurden?
		if($Kunde)
			$sql_swhere .= " AND p.kunde=".$Kunde;
		$sql_swhere .= " ".$nl;

		$sql_sorder  = "ORDER BY";
		$sql_sorder .= " m.soll_beginn, m.soll_ende";
	}

	$sql_pselect=",".$nl." p.vorgangsnummer, p.name_projekt, p.projektleiter, p.vertreter ".$nl;
	$sql_aselect=$sql_pselect.",".$nl." pe.pname, pe.vorname, pe.geschlecht ".$nl;

	// Anzahl der Zeitdaten ermitteln
	$anzz=0;
	if(fxIsArray($Pers_Arr) && $Zeitdaten)
	{
		$sql="SELECT COUNT(z.zeit_id) ".$sql_zfrom.str_replace('{za}', FXP_TE_TIMEREC.",".FXP_TE_TIMEREC_SUM.",".FXP_TE_TRAVELEXP_TIMEREC, $sql_zwhere);
		$anzz=(int)db_value($sql);
//fxDebug($anzz,$sql);
	}

	// Anzahl der Reisekosten ermitteln
	$anzr=0;
	if(fxIsArray($Pers_Arr) && $Reisekosten)
	{
		$sql="SELECT COUNT(z.zeit_id) ".$sql_zfrom.str_replace('{za}', FXP_TE_TRAVELEXP_TIMEREC.",".FXP_TE_TRAVELEXP, $sql_zwhere);
		$anzr=(int)db_value($sql);
//fxDebug($anzr,$sql);
	}

	// Anzahl des Materialverbrauchs ermitteln
	$anzv=0;
	if(fxIsArray($Pers_Arr) && $mit_SM)
	{
		$sql="SELECT COUNT(m.id) ".$sql_vfrom.$sql_vwhere;
		$anzv=(int)db_value($sql);
//fxDebug($anzv,$sql);
	}

	// Anzahl des Sofortmaterials ermitteln
	$anzs=0;
	if(fxIsArray($Pers_Arr) && $mit_SM)
	{
		$sql="SELECT COUNT(sachmittel_id) ".$sql_sfrom.$sql_swhere;
		$anzs=(int)db_value($sql);
//fxDebug($anzs,$sql);
	}

	// Warnung anzeigen, falls Gesamtanzahl zu viel
	$anz=$anzz+$anzr+$anzv+$anzs;
	if($maxentry && ($anz > $maxentry))
	{
		// 1743: Zu viele Listeneinträge ($var1) gefunden! Entsprechend den aktuellen Einstellungen wird die Liste nach maximal $var2 Einträgen abgebrochen. Bitte schränken Sie die Auswahlkriterien ein oder kontaktieren Sie Ihren Administrator.
		error_msg(1743, false, $anz, $maxentry);
		$anz=$maxentry;
	}

	if($anz || $Zeitdaten)
	{
		// Rechnungsnummern-Array
		$rnra=array();

		// Units
		$mua=array();
		$una=fxf_getReferenceValues(1054);
//fxDebug($una,'$una');

		// Zeilenumbruch
		$wrap=false;
		if($Zeilenumbruch)
			$wrap=true;

		// Aktueller Maskenzähler
		$am=$GLOBALS['_maskcounter'];
		$psshown=array();
		$az=0;

		// All columns
		$fa=array('Checkbox|0', 'Datum', 'Zeit_ID', 'Projekt', 'typ', 'Beschreibung', 'AnmerkungIntern', 'SachMenge', 'Verkaufseinheit', 'nicht_abrechenbar', 'rk_nicht_abrechenbar', 'mat_nicht_abrechenbar', 'Status_Benutzer', 'Dummy|0');

		// No right border in the following column
		fxf_maskColumnBorder(array('Checkbox'), 0, -1, $am);

		// Aktuelle Rolle
		$mr=$GLOBALS['fxpglobals']['persdat']['role'];
		if($GLOBALS['fxpglobals']['persdat']['roles'][-1])
			$mr=2;

		// Colspan + Not visible
		$colspan=12;
		$csa=array('AnmerkungIntern'=>'Anmerkung', 'nicht_abrechenbar'=>'anzz', 'rk_nicht_abrechenbar'=>'anzr', 'mat_nicht_abrechenbar'=>'anzv');
		foreach($csa as $csf => $csc)
		{
			$GLOBALS[$csf.'_nv']=false;
			if(!$GLOBALS[$csc])
			{
				$GLOBALS[$csf.'_nv']=true;
				$colspan--;
			}
		}

		// Personen
		$vpa=array();

		// Alle Einträge ermitteln
		$aa=array();
		$ia=array();
		$ca=array();
		$lw=array();

		// ...Zeitdaten
		if($anzz)
		{
			$ia['z']='<img src="'.$GLOBALS['gfxpath'].'tim_b_16x16.png" width="16" height="16" tooltip="'.$stt[8].'" style="cursor:help;">';

			$sql=$sql_zselect.$sql_aselect.$sql_zfrom.str_replace('{za}', FXP_TE_TIMEREC.",".FXP_TE_TIMEREC_SUM.",".FXP_TE_TRAVELEXP_TIMEREC, $sql_zwhere).$sql_zorder;
			$tmp=db_values($sql,'*');
//fxDebug($tmp,$sql);
			if(fxIsArray($tmp))
			{
				foreach($tmp as $ta)
				{
					$dt=substr((string)$ta['ist_beginn'],0,8);
					if(!isset($ca[$dt]))
						$ca[$dt]=1;
					else
						$ca[$dt]++;
					$lw[$dt.substr('00000'.$ca[$dt],-6).'z']=$ta;
					$vpa[$ta['personen_id']]=true;

					$apid=(int)$ta['projekt_id'];
					if($apid > 0)
						$aa[$apid]=0;
				}
			}
		}
		// ...Reisekosten
		if($anzr)
		{
			$ia['r']='<img src="'.$GLOBALS['gfxpath'].'lst_b_16x16.png" width="16" height="16" tooltip="'.$stt[7].'" style="cursor:help;">';							// Sonstiges
			$ia['r1981']='<img src="'.$GLOBALS['gfxpath'].'car_b_16x16.png" width="16" height="16" tooltip="'.$stt[7].'" style="cursor:help;">';							// Inlandsreise
			$ia['r1982']='<img src="'.$GLOBALS['gfxpath'].'pln_b_16x16.png" width="16" height="16" tooltip="'.$stt[7].'" style="cursor:help;">';							// Auslandsreise
			$ia['r1983']='<img src="'.$GLOBALS['gfxpath'].'por_b_16x16.png" width="16" height="16" tooltip="'.$stt[7].'" style="cursor:help;">';							// Belege ohne Reise
			$ia['rds']='<img src="'.$GLOBALS['gfxpath'].'cas_b_12x12.png" width="12" height="12" tooltip="'.$stt[7].': '.$rk_kat[1980].'" style="cursor:help;">';			// Tagesangaben
			$ia['rv']='<img src="'.$GLOBALS['gfxpath'].'por_b_12x12.png" width="12" height="12" tooltip="'.$stt[7].': '.get_text(1887,'10').'" style="cursor:help;">';	// Beleg

			$sql=$sql_zselect.$sql_aselect.$sql_zfrom.str_replace('{za}', FXP_TE_TRAVELEXP_TIMEREC.",".FXP_TE_TRAVELEXP, $sql_zwhere).$sql_zorder;
			$tmp=db_values($sql,'*');
//fxDebug($tmp,$sql);
			if(fxIsArray($tmp))
			{
				foreach($tmp as $ta)
				{
					$dt=substr((string)$ta['ist_beginn'],0,8);
					if(!isset($ca[$dt]))
						$ca[$dt]=1;
					else
						$ca[$dt]++;
					$lw[$dt.substr('00000'.$ca[$dt],-6).'r']=$ta;
					$vpa[$ta['personen_id']]=true;

					$apid=(int)$ta['projekt_id'];
					if($apid > 0)
						$aa[$apid]=0;
				}
			}
		}
		// ...Verbrauchsmaterial
		if($anzv)
		{
			$ia['v']='<img src="'.$GLOBALS['gfxpath'].'tol_b_16x16.png" width="16" height="16" tooltip="'.$stt[9].'" style="cursor:help;">';

			$sql=$sql_vselect.$sql_aselect.$sql_vfrom.$sql_vwhere.$sql_vorder;
			$tmp=db_values($sql,'*');
//fxDebug($tmp,$sql);
			if(fxIsArray($tmp))
			{
				foreach($tmp as $ta)
				{
					$dt=substr((string)$ta['ist_beginn'],0,8);
					if(!isset($ca[$dt]))
						$ca[$dt]=1;
					else
						$ca[$dt]++;
					$lw[$dt.substr('00000'.$ca[$dt],-6).'v']=$ta;
					$vpa[$ta['personen_id']]=true;

					$apid=(int)$ta['projekt_id'];
					if($apid > 0)
						$aa[$apid]=0;
				}
			}
		}

		if(sizeof($lw))
			ksort($lw,SORT_STRING);
//fxDebug($lw,'$lw');

		// Abrechnungsarten
		if(sizeof($aa) > 0)
		{
			$ac=array_to_csv($aa);
			$at=db_values("SELECT projekt_id, abrechnungsart, abrechnungsart_mat FROM budget_summe WHERE projekt_id IN (".$ac.") AND mandanten_id=".$GLOBALS['fxpglobals']['client'], '*');
//fxDebug($at,'$at');
			if(fxIsArray($at))
			{
				foreach($at as $t)
					$aa[(int)$t['projekt_id']]=array('z'=>(int)$t['abrechnungsart'], 'm'=>(int)$t['abrechnungsart_mat']);
			}
//fxDebug($aa,'$aa: $ac='.$ac);
		}

		// Alle Personen durchgehen
		foreach($Pers_Arr as $pid => $name)
		{
			if((!$Zeitdaten && !isset($vpa[$pid])) || $psshown[$pid])
				continue;

			// Prüfen, ob Rechte auf Transaktionen für Link
			$recht_ze=0;
			$recht_zs=0;
			$recht_rk=0;
			$recht_mv=0;
			if($GLOBALS['fxpglobals']['persdat']['roles'][-1] || $GLOBALS['fxpglobals']['persdat']['roles'][2] || ($GLOBALS['fxpglobals']['person'] == $pid))
			{
				$recht_ze=tr_rechte(81);	// Zeiterfassung (Tag)
				$recht_zs=tr_rechte(34);	// Zeiterfassung (Summe)
				$recht_rk=tr_rechte(150);	// Reisekostenerfassung
				$recht_mv=tr_rechte(102);	// Materialverbrauch
			}

			// Backupvariablen löschen
			$bak_datum='';
			$bak_projekt='';

			// Prüfung bei Zeitdaten, ob leere Arbeitstage im Anzeigebereich vohanden sind
			$term_array=array();
			$sotm=0;
			if($Zeitdaten)
			{
				// Alle Arbeitstage ermitteln
				$zsv=substr((string)$Zeitspanne_von,0,8);
				$zsb=substr((string)$Zeitspanne_bis,0,8);
				$pers_array=getExtPersInfo($pid, $zsv, $zsb);
//fxDebug($pers_array, 'getExtPersInfo('.$pid.', '.$zsv.', '.$zsb.');');
				$term_array=$pers_array['days'];
//fxDebug($term_array, '$term_array');

				// ...Freie Tage eliminieren
				foreach($term_array as $tdate => $tarray)
				{
					if($tarray['h'] <= 0.0)
						unset($term_array[$tdate]);
				}
				$sotm=sizeof($term_array);

				// ...Tage mit vorhandener Zeiterfassung eliminieren
				$sql  = "SELECT ist_beginn FROM zeitdaten";
				$sql .= " WHERE personen_id=".$pid." AND mandanten_id=".$GLOBALS['fxpglobals']['client']." AND zeitart IN (".FXP_TE_TIMEREC.",".FXP_TE_TIMEREC_SUM.",".FXP_TE_TRAVELEXP_TIMEREC.")";
				$sql .= " AND (zeiteinheit IS NULL OR zeiteinheit=0 OR zeitart=".FXP_TE_TIMEREC_SUM.")";
				$sql .= " AND ist_beginn<='".$zsb."240000' AND ist_ende>='".$zsv."000000'";
				$sql .= " ORDER BY ist_beginn";
				$zelw=db_values($sql,'*');
//fxDebug($zelw,$sql);
				if(fxIsArray($zelw))
				{
					foreach($zelw as $tarray)
					{
						$tdate=substr((string)$tarray['ist_beginn'],0,8);
						if(isset($term_array[$tdate]))
							unset($term_array[$tdate]);
					}
				}
			}
			$sota=sizeof($term_array);
//fxDebug($term_array, '$term_array: $sotm='.$sotm.', $sota='.$sota);

			if(sizeof($lw))
			{
				foreach($lw as $lt => $prdat)
				{
					$lt=substr((string)$lt,-1);

					// Nur aktuelle Person
					if($prdat['personen_id'] != $pid)
						continue;

					// VW+GF+Eigene Person immer, PL/SP nur zu eigenen Vorgängen
					if(!$GLOBALS['fxpglobals']['persdat']['roles'][-1] && !$GLOBALS['fxpglobals']['persdat']['roles'][2] && ($GLOBALS['fxpglobals']['person'] != $pid) && ($prdat['projektleiter'] != $GLOBALS['fxpglobals']['person']) && ($prdat['vertreter'] != $GLOBALS['fxpglobals']['person']))
						continue;

//fxDebug($prdat,'$pid='.$pid.', $lt='.$lt.', $lc='.$lc);

					// Name der Person bereits angezeigt?
					if(!$psshown[$pid])
					{
						$psshown[$pid]=true;

						// Leerzeile zwischen den einzelnen Personen
						if($az)
						{
							fxf_maskColumnBorder($fa, 1, -1, $am, $az, true);
							$az++;
						}

						// Name (über gesamte Zeile)
						$name=preg_replace(array('/<tt>.*<\/tt>/is', '/<img.*\/>/is'), array('', ''), $name);
						$fc='fff';
						if(substr((string)$name,0,5) == '<red>')
						{
							$fc='e1001a';
							$name=substr((string)$name,5);
						}
						$opp=strpos($name,'(');
						if($opp)
							$name=trim(substr((string)$name,0,$opp));
						$txt='<div class="s5b tshadow" style="color:#'.$fc.';">&nbsp;'.person_pic($prdat['personen_id'], 20, $prdat['geschlecht'], $GLOBALS['fxpglobals']['client'], false, 'border-top-left-radius:6px;').'&nbsp;&nbsp;'.$name.'</div>';
						$GLOBALS['fxptdata']['lvalues'][$am][$az]['checkbox']='~';
						$GLOBALS['fxptdata']['lvalues'][$am][$az]['dummy']='~';
						fxf_listSectHeader($am, $az++, $txt, 'datum', 'status_benutzer', $colspan, -2, 1, 'linear-gradient(to bottom,#fff,#79aec7,#79aec7,#006b9f)', 'padding:4px;');

						// Leere Arbeitstage vorhanden?
						if($sota)
						{
							$txt='';
							foreach($term_array as $tdate => $tarray)
							{
								if(strlen((string)$txt))
									$txt .= ', ';
								$txt .= get_entry($tdate,'datum');
							}
							// An folgenden Arbeitstagen im Anzeigezeitraum ist (noch) keine Zeiterfassung vorhanden
							$txt='<div class="red">&nbsp;'.$ntr[0].':</div><div class="lightred wrap" style="padding-left:20px;"><br />'.$txt.'&nbsp;&nbsp;<br /><br /></div>';
							$GLOBALS['fxptdata']['lvalues'][$am][$az]['checkbox']='~';
							$GLOBALS['fxptdata']['lvalues'][$am][$az]['dummy']='~';
							$GLOBALS['fxptdata']['lvalues'][$am][$az]['datum']='^col_?" nowrap colspan="'.$colspan.'" style="padding:4px 2px;background:linear-gradient(to bottom,rgba(225,0,26, 0),rgba(225,0,26, 0.1));border-right:1px solid #'.$GLOBALS['msk_nbcol'].';border-bottom:1px solid #'.$GLOBALS['msk_nbcol'].';"><!--|~'.$txt;
							$GLOBALS['fxptdata']['lvalues'][$am][$az]['status_benutzer']='~-->'.$txt;
							$az++;
						}
					}

					$ze=false;
					$zs=false;
					$rk=false;
					$mv=false;
					if($lt == 'z')
					{
						if($prdat['zeitart'] == FXP_TE_TIMEREC_SUM)
							$zs=true;
						else
							$ze=true;
					}
					else if($lt == 'r')
						$rk=true;
					else if($lt == 'v')
						$mv=true;

					// Prüfen, ob bereits Freigabe erfolgt ist (Variablen werden in Funktion geupdated)
					check_freigabe($prdat['zeit_id'],$prdat['id']);

					// HIDDEN
					relSetHidden($prdat);

					// Prüfen, ob Zeitdatensatz bereits abgerechnet
					$abgerechnet=(int)$prdat['rechnungs_id'];
					if($abgerechnet > 0)
					{
						$apid=(int)$prdat['projekt_id'];
						if(($apid > 0) && fxIsArray($aa[$apid]) && ($aa[$apid]['z'] == FXP_INV_FIX))	// Festpreis
							$abgerechnet=-$abgerechnet;
					}
					if($rk && !$abgerechnet)
						$abgerechnet=(int)$prdat['rechnungs_id_rk'];

					// (Checkbox)
					$changeable=false;
					if(($abgerechnet <= 0) && ($mr >= $freigabe))
					{
						$GLOBALS['fxptdata']['lvalues'][$am][$az]['checkbox']='';
						if(fxIsArray($_POST) && fxIsArray($_POST['Checkbox']) && fxIsArray($_POST['Checkbox'][$am]) && isset($_POST['Checkbox'][$am][$az]) && $_POST['Checkbox'][$am][$az] && $err)
							$GLOBALS['fxptdata']['lvalues'][$am][$az]['checkbox']='1';
						$changeable=true;
					}
					else
						$GLOBALS['fxptdata']['lvalues'][$am][$az]['checkbox']='~';

					// Datum (Datum)
					$GLOBALS['fxptdata']['lvalues'][$am][$az]['datum']='~';
					$akt_datum=get_entry($prdat['ist_beginn'], 'datum');
					if($akt_datum != $bak_datum)
					{
						$bak_projekt='';
						$bak_datum=$akt_datum;
						$wday=getWeekday($prdat['ist_beginn']);

						$GLOBALS['fxptdata']['lvalues'][$am][$az]['datum'] .= '<b>'.substr((string)$weekdays[$wday],0,2).'</b>&nbsp;'.$akt_datum;
					}

					// Zeit (Zeit_ID)
					$zeit='';
					if($zs)
					{
						$zeit='&sum;';
						if($prdat['zeit_id'] == $prdat['zeiteinheit'])
							$zeit .= '&nbsp;<i class="s2 lightgrey">'.$stt[0].'</i>';
					}
					else
					{
						$szeit=get_entry($prdat['ist_beginn'], 'zeit');
						$ezeit=get_entry($prdat['ist_ende'], 'zeit');
						if($ezeit != $szeit)
						{
							$zeit=$szeit;
							if(substr((string)$prdat['ist_beginn'],0,8) == substr((string)$prdat['ist_ende'],0,8))
								$zeit .= '-'.$ezeit;
						}
					}
					$GLOBALS['fxptdata']['lvalues'][$am][$az]['zeit_id']='~'.$zeit;

					// Projekt (Projekt)
					$GLOBALS['fxptdata']['lvalues'][$am][$az]['projekt']='~';
					if($prdat['projekt_id'] !== $bak_projekt)
					{
						$bak_projekt=$prdat['projekt_id'];
						if($prdat['projekt_id'] == 0)
							$GLOBALS['fxptdata']['lvalues'][$am][$az]['projekt'] .= '<i class="grey">'.$unbekannt.'</i>';
						else if($prdat['projekt_id'] == -1)
						{
							$GLOBALS['fxptdata']['lvalues'][$am][$az]['projekt'] .= '<i class="darkgrey">'.$gleitzeit.'</i>';
							$prdat['zeitaufwand']=0;
						}
						else
							$GLOBALS['fxptdata']['lvalues'][$am][$az]['projekt'] .= '<font class="darkgrey">'.no_null($prdat['vorgangsnummer']).'</font> - '.text_wrap($prdat['name_projekt'], false, $projekt_kuerzen);
					}

					// Typ (typ)
					if(!$rk)
						$GLOBALS['fxptdata']['lvalues'][$am][$az]['typ']='~'.$ia[$lt];

					// Beschreibung (Beschreibung) ff.
					// ...Materialvebrauch
					if($mv)
					{
						$mid=(int)$prdat['sachmittel_id'];
						if(!isset($mua[$mid]))
						{
							$mua[$mid]=db_values("SELECT artikelnummer_int,artikelnummer_ext,name_sachmittel,verkaufseinheit_id FROM sachmittel WHERE sachmittel_id=".$mid." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
							$mua[$mid]['beschreibung']='<font style="color:#816340;">'.$mua[$mid]['name_sachmittel'].'</font>';
							if(strlen((string)$mua[$mid]['artikelnummer_int']))
								$mua[$mid]['beschreibung']='<i class="lightgrey">'.$mua[$mid]['artikelnummer_int'].':</i> '.$mua[$mid]['beschreibung'];
							$mia=getMaterialIcons($mid,$mmxh);
							if(strlen((string)$mia['256']))
								$mua[$mid]['beschreibung']='<div tooltip="<img src=\''.$mia['256'].$mia['mtime'].'\''.$mia['size_256'].' style=\'max-height:'.$mmxh.'px;width:auto;margin-top:4px;\' />" style="float:right;width:22px;height:22px;border:1px solid #e2e2e2;border-radius:2px;margin-left:64px;cursor:help;"><img src="'.$mia['ico'].$mia['mtime'].'"'.$mia['size_ico'].' /></div><div style="margin-right:32px;">'.$mua[$mid]['beschreibung'].'</div>';
							else
								$mua[$mid]['beschreibung']='<div class="ainactive" style="float:right;width:22px;height:22px;border:1px solid #e2e2e2;border-radius:2px;margin-left:64px;"><img src="'.$mia['ico'].$mia['mtime'].'"'.$mia['size_ico'].' /></div><div style="margin-right:32px;">'.$mua[$mid]['beschreibung'].'</div>';
						}
						$GLOBALS['fxptdata']['lvalues'][$am][$az]['beschreibung']='~'.$mua[$mid]['beschreibung'];
						if(($prdat['beschreibung'] !== NULL) && strlen((string)$prdat['beschreibung']))
							$GLOBALS['fxptdata']['lvalues'][$am][$az]['beschreibung'] .= '<br />'.text_wrap($prdat['beschreibung'], $wrap, $text_kuerzen);

						fillColumns($prdat);
					}

					// ...Reisekosten
					else if($rk)
					{
						// Reise
						$art=(int)$prdat['rk_art'];
						$land=(int)$prdat['land_id'];
						$fahrzeug=(int)$prdat['rk_kategorie'];
						$rnr='';
						if($prdat['belegnr'] !== NULL)
							$rnr=trim((string)$prdat['belegnr']);
						if(strlen((string)$rnr))
							$rnr='&nbsp;['.$rnr.']';
						if(!$art)			// Alte Einträge
						{
							$art=1984;		// ...Sonstiges
							$fahrzeug=1966;	// ...Kein Fahrzeug
						}

						// Typ
						if(isset($ia[$lt.$art]))
							$GLOBALS['fxptdata']['lvalues'][$am][$az]['typ']='~'.$ia[$lt.$art];
						else
							$GLOBALS['fxptdata']['lvalues'][$am][$az]['typ']='~'.$ia[$lt];

						if($art == 1983)	// Beleg(e) ohne Reise
						{
							$GLOBALS['fxptdata']['lvalues'][$am][$az]['beschreibung']='~<font class="green">'.$rk_art[1983].$rnr.'</font>';
						}
						else
						{
							$GLOBALS['fxptdata']['lvalues'][$am][$az]['beschreibung']='~<font class="green"><u>'.$rk_art[$art].$rnr.':</u></font>';
							if(($art == 1982) && $land)	// Auslandsreise
							{
								if(!$rk_land[$land])
									$rk_land[$land]=db_value("SELECT land_ort FROM rkpauschalen WHERE land_id=".$land." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
								$GLOBALS['fxptdata']['lvalues'][$am][$az]['beschreibung'] .= '&nbsp;<i>('.$rk_land[$land].')</i>';
							}
							fillColumns(false);

							// ...Zeitraum
							$GLOBALS['fxptdata']['lvalues'][$am][$az]['beschreibung']='~'.dummySpace(16).get_entry($prdat['ist_beginn'], 'datetime').'&nbsp;-&nbsp;'.get_entry($prdat['ist_ende'], 'datetime');
							fillColumns(false);

							// ...Start-Zielort, Fahrzeugart  (km)
							$GLOBALS['fxptdata']['lvalues'][$am][$az]['beschreibung']='~'.dummySpace(16).'<font class="darkgrey">';
							if(strlen((string)$prdat['rk_ort1']))
								$GLOBALS['fxptdata']['lvalues'][$am][$az]['beschreibung'] .= $prdat['rk_ort1'].'-'.$prdat['rk_ort2'].',&nbsp;';
							$GLOBALS['fxptdata']['lvalues'][$am][$az]['beschreibung'] .= $rk_kat[$fahrzeug];
							if(($prdat['rk_km'] > 0) || ($prdat['rk_mitfahrer'] > 0))
							{
								$GLOBALS['fxptdata']['lvalues'][$am][$az]['beschreibung'] .= '&nbsp;&nbsp;(';
								if($prdat['rk_km'] > 0)
								{
									$GLOBALS['fxptdata']['lvalues'][$am][$az]['beschreibung'] .= $prdat['rk_km'].'&nbsp;km';
									if($prdat['rk_mitfahrer'] > 0)
										$GLOBALS['fxptdata']['lvalues'][$am][$az]['beschreibung'] .= ',&nbsp;';
								}
								if($prdat['rk_mitfahrer'] > 0)
									$GLOBALS['fxptdata']['lvalues'][$am][$az]['beschreibung'] .= '+'.$prdat['rk_mitfahrer'].substr((string)$GLOBALS['mitfahrer_str'], 0, 1);
								$GLOBALS['fxptdata']['lvalues'][$am][$az]['beschreibung'] .= ')';
							}
							$GLOBALS['fxptdata']['lvalues'][$am][$az]['beschreibung'] .= '</font>';
						}
						fillColumns($prdat);

						// Tagesangaben
						$tsql  = $sql_zselect." ".$nl;
						$tsql .= "FROM";
						$tsql .= " zeitdaten z, taetigkeiten t ".$nl;
						$tsql .= "WHERE";
						$tsql .= " z.zeit_id=t.zeit_id AND z.mandanten_id=t.mandanten_id AND z.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND z.rk_kategorie=1980 AND z.zeiteinheit=".$prdat['zeit_id'];
						// Bereits freigegebene Datensätze ausblenden?
						if($freigabe_ignorieren)
							$tsql .= " AND (z.freigabe IS NULL OR z.freigabe=0)";
						$angaben=db_values($tsql);
//fxDebug($angaben, 'Tagesangaben: '.$tsql);
						if(fxIsArray($angaben))
						{
							$az++;
							check_freigabe($angaben['zeit_id'],$angaben['id']);

							// HIDDEN
							relSetHidden($angaben);

							// Prüfen, ob Zeitdatensatz bereits abgerechnet
							$abgerechnet=(int)$angaben['rechnungs_id_rk'];

							// Zeile (Checkbox)
							$changeable=false;
							if(($abgerechnet <= 0) && ($mr >= $freigabe))
							{
								$GLOBALS['fxptdata']['lvalues'][$am][$az]['checkbox']='';
								if(fxIsArray($_POST) && fxIsArray($_POST['Checkbox']) && fxIsArray($_POST['Checkbox'][$am]) && isset($_POST['Checkbox'][$am][$az]) && $_POST['Checkbox'][$am][$az] && $err)
									$GLOBALS['fxptdata']['lvalues'][$am][$az]['checkbox']='1';
								$changeable=true;
							}
							else
								$GLOBALS['fxptdata']['lvalues'][$am][$az]['checkbox']='~';

							$GLOBALS['fxptdata']['lvalues'][$am][$az]['typ']='~'.$ia['rds'];

							$GLOBALS['fxptdata']['lvalues'][$am][$az]['beschreibung']='~'.dummySpace(16).'<b class="darkgrey">'.$rk_kat[1980].'</b>';
							if($land)
							{
								if(!$rk_land[$land])
									$rk_land[$land]=db_value("SELECT land_ort FROM rkpauschalen WHERE land_id=".$land." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
								$GLOBALS['fxptdata']['lvalues'][$am][$az]['beschreibung'] .= '&nbsp;<font class="grey">('.$rk_land[$land].')</font>';
							}
							fillColumns($angaben);
						}

						// Einzelbelege
						$bsql = $sql_zselect." ".$nl;
						$bsql .= "FROM";
						$bsql .= " zeitdaten z, taetigkeiten t ".$nl;
						$bsql .= "WHERE";
						$bsql .= " z.zeit_id=t.zeit_id AND z.mandanten_id=t.mandanten_id AND z.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND z.rk_kategorie<>1980 AND z.zeiteinheit=".$prdat['zeit_id']." ".$nl;
						// Bereits freigegebene Datensätze ausblenden?
						if($freigabe_ignorieren)
							$bsql .= "AND (z.freigabe IS NULL OR z.freigabe=0) ".$nl;
						$bsql .= "ORDER BY";
						$bsql .= " z.zeit_id";
						$belege=db_values($bsql, '*');
//fxDebug($belege, 'Belege: '.$bsql);
						if(fxIsArray($belege))
						{
							foreach($belege as $rke)
							{
								$az++;
								check_freigabe($rke['zeit_id'],$rke['id']);

								$blnr=trim((string)$rke['belegnr']);
								if(strlen((string)$blnr))
									$blnr .= ':&nbsp;&nbsp;';

								// HIDDEN
								relSetHidden($rke);

								// Prüfen, ob Zeitdatensatz bereits abgerechnet
								$abgerechnet=(int)$rke['rechnungs_id_rk'];

								// Zeile (Checkbox)
								$changeable=false;
								if(($abgerechnet <= 0) && ($mr >= $freigabe))
								{
									$GLOBALS['fxptdata']['lvalues'][$am][$az]['checkbox']='';
									if(fxIsArray($_POST) && fxIsArray($_POST['Checkbox']) && fxIsArray($_POST['Checkbox'][$am]) && isset($_POST['Checkbox'][$am][$az]) && $_POST['Checkbox'][$am][$az] && $err)
										$GLOBALS['fxptdata']['lvalues'][$am][$az]['checkbox']='1';
									$changeable=true;
								}
								else
									$GLOBALS['fxptdata']['lvalues'][$am][$az]['checkbox']='~';

								$GLOBALS['fxptdata']['lvalues'][$am][$az]['typ']='~'.$ia['rv'];

								$GLOBALS['fxptdata']['lvalues'][$am][$az]['beschreibung']='~'.dummySpace(16).'<font class="grey">&middot;</font>&nbsp;<font class="darkgrey">'.$blnr.'</font><font class="grey">'.$rk_kat[$rke['rk_kategorie']].'</font>';
								fillColumns($rke);
							}
						}
					}

					// ...Zeiterfassung
					else
					{
						$GLOBALS['fxptdata']['lvalues'][$am][$az]['beschreibung']='~'.text_wrap($prdat['taetigkeit_text'], $wrap, $text_kuerzen);

						fillColumns($prdat);
					}

					$az++;
				}
			}

			// Keine einzige Zeiterfassung im Anzeigebereich vorhanden?
			if($sota && !$psshown[$pid])
			{
				$psshown[$pid]=true;

				// Leerzeile zwischen den einzelnen Personen
				if($az)
				{
					fxf_maskColumnBorder($fa, 1, -1, $am, $az, true);
					$az++;
				}

				// Name (über gesamte Zeile)
				$name=preg_replace(array('/<tt>.*<\/tt>/is', '/<img.*\/>/is'), array('', ''), $name);
				$fc='223';
				if(substr((string)$name,0,5) == '<red>')
				{
					$fc='e1001a';
					$name=substr((string)$name,5);
				}
				$opp=strpos($name,'(');
				if($opp)
					$name=trim(substr((string)$name,0,$opp));

				// Geschlecht
				if(fxIsArray($pers_array) && fxIsArray($pers_array['data']) && ($pers_array['data']['personen_id'] == $pid))
					$prsges=$pers_array['data']['geschlecht'];
				else
					$prsges=0;
				$txt='<div class="s5b" style="color:#'.$fc.';">&nbsp;'.person_pic($pid, 20, $prsges, $GLOBALS['fxpglobals']['client'], false, 'border-top-left-radius:6px;').'&nbsp;&nbsp;'.$name.'</div>';
				$GLOBALS['fxptdata']['lvalues'][$am][$az]['checkbox']='~';
				$GLOBALS['fxptdata']['lvalues'][$am][$az]['dummy']='~';
				fxf_listSectHeader($am, $az++, $txt, 'datum', 'status_benutzer', $colspan, -2, 1, '', 'padding:4px;');

				// Leere Arbeitstage vorhanden?
				if($sota >= $sotm)
				{
					// An allen Arbeitstagen im Anzeigezeitraum ist (noch) keine Zeiterfassung vorhanden
					$txt='<div class="red">&nbsp;'.$ntr[1].'!</div>';
					$bb='';
				}
				else
				{
					// An folgenden Arbeitstagen im Anzeigezeitraum ist (noch) keine Zeiterfassung vorhanden
					$txt='';
					foreach($term_array as $tdate => $tarray)
					{
						if(strlen((string)$txt))
							$txt .= ', ';
						$txt .= get_entry($tdate,'datum');
					}
					$txt='<div class="red">&nbsp;'.$ntr[0].':</div><div class="lightred wrap" style="padding-left:20px;"><br />'.$txt.'&nbsp;&nbsp;<br /><br /></div>';
					$bb='border-bottom:1px solid #'.$GLOBALS['msk_nbcol'].';';
				}
				$GLOBALS['fxptdata']['lvalues'][$am][$az]['checkbox']='~';
				$GLOBALS['fxptdata']['lvalues'][$am][$az]['dummy']='~';
				$GLOBALS['fxptdata']['lvalues'][$am][$az]['datum']='^col_?" nowrap colspan="'.$colspan.'" style="padding:4px 2px;background:linear-gradient(to bottom,rgba(225,0,26, 0),rgba(225,0,26, 0.1));border-right:1px solid #'.$GLOBALS['msk_nbcol'].';'.$bb.'"><!--|~'.$txt;
				$GLOBALS['fxptdata']['lvalues'][$am][$az]['status_benutzer']='~-->'.$txt;
				$az++;
			}
		}

		// Alle Sofortmaterialeinträge ermitteln
		$ca=array();
		$lw=array();
		if($anzs)
		{
			$ia['s']='<img src="'.$GLOBALS['gfxpath'].'mat_b_16x16.png" width="16" height="16" tooltip="'.$stt[12].'" style="cursor:help;">';

			$sql=$sql_sselect.$sql_pselect.$sql_sfrom.$sql_swhere.$sql_sorder;
			$tmp=db_values($sql,'*');
//fxDebug($tmp,$sql);
			if(fxIsArray($tmp))
			{
				foreach($tmp as $ta)
				{
					$dt=substr((string)$ta['ist_beginn'],0,8);
					if(!isset($ca[$dt]))
						$ca[$dt]=1;
					else
						$ca[$dt]++;
					$lw[$dt.substr('00000'.$ca[$dt],-6).'s']=$ta;
				}
			}
		}

		// Alle Sachmittel durchgehen
		if(sizeof($lw))
		{
			ksort($lw,SORT_STRING);
//fxDebug($lw,'$lw');

			// Prüfen, ob Rechte auf Transaktionen für Link
			$recht_ms=0;
			if($GLOBALS['fxpglobals']['persdat']['roles'][-1] || $GLOBALS['fxpglobals']['persdat']['roles'][2] || ($GLOBALS['fxpglobals']['person'] == $pid))
				$recht_ms=tr_rechte(78);	// Materialdefinition

			// Backupvariablen löschen
			$bak_datum='';
			$bak_projekt='';

			// Sofortmaterialabschnitt
			if($az)
			{
				fxf_maskColumnBorder($fa, 1, -1, $am, $az, true);
				$az++;
			}

			$txt='<div class="s5b green">&nbsp;'.meldung(3149).' ('.meldung(1645).')</div>';	// Sofortmaterial (bezahlt)
			$GLOBALS['fxptdata']['lvalues'][$am][$az]['checkbox']='~';
			$GLOBALS['fxptdata']['lvalues'][$am][$az]['dummy']='~';
			fxf_listSectHeader($am, $az++, $txt, 'datum', 'status_benutzer', $colspan, -2, 1, '', 'padding:4px;');
			$az++;

			foreach($lw as $lt => $prdat)
			{
				$lt=substr((string)$lt,-1);
				$ms=true;

				// Prüfen, ob bereits Freigabe erfolgt ist (Variablen werden in Funktion geupdated)
				check_freigabe(-$prdat['sachmittel_id'],$prdat['sachmittel_id']);

				// HIDDEN
				$GLOBALS['fxptdata']['lvalues'][$am][$az]['nv_ganzzahl1']=-$prdat['sachmittel_id'];
				$GLOBALS['fxptdata']['lvalues'][$am][$az]['nv_ganzzahl2']=$prdat['sachmittel_id'];
				$GLOBALS['fxptdata']['lvalues'][$am][$az]['nv_ganzzahl3']=$prdat['fuervorgang'];

				// Prüfen, ob Zeitdatensatz bereits abgerechnet
				$abgerechnet=(int)$prdat['rechnungs_id'];
				if($abgerechnet > 0)
				{
					$apid=(int)$prdat['projekt_id'];
					if(($apid > 0) && fxIsArray($aa[$apid]) && ($aa[$apid]['m'] == FXP_INV_FIX))	// Festpreis
						$abgerechnet=-$abgerechnet;
				}

				// (Checkbox)
				$changeable=false;
				if(($abgerechnet <= 0) && ($mr >= $freigabe))
				{
					$GLOBALS['fxptdata']['lvalues'][$am][$az]['checkbox']='';
					if(fxIsArray($_POST) && fxIsArray($_POST['Checkbox']) && fxIsArray($_POST['Checkbox'][$am]) && isset($_POST['Checkbox'][$am][$az]) && $_POST['Checkbox'][$am][$az] && $err)
						$GLOBALS['fxptdata']['lvalues'][$am][$az]['checkbox']='1';
					$changeable=true;
				}
				else
					$GLOBALS['fxptdata']['lvalues'][$am][$az]['checkbox']='~';

				// Datum (Datum) + Zeit (Zeit_ID)
				$akt_datum=get_entry($prdat['soll_beginn'], 'datum');
				if(substr((string)$prdat['soll_ende'],0,8) != substr((string)$prdat['soll_beginn'],0,8))
					$akt_datum .= '-'.get_entry($prdat['soll_ende'], 'datum');
				if($akt_datum != $bak_datum)
				{
					$bak_projekt='';
					$bak_datum=$akt_datum;
				}
				else
					$akt_datum='';
				$GLOBALS['fxptdata']['lvalues'][$am][$az]['datum']='^class_?" nowrap align="center" colspan="2"><!--|~'.$akt_datum;
				$GLOBALS['fxptdata']['lvalues'][$am][$az]['zeit_id']='~-->'.$akt_datum;

				// Projekt (Projekt)
				$GLOBALS['fxptdata']['lvalues'][$am][$az]['projekt']='~';
				if($prdat['fuervorgang'] !== $bak_projekt)
				{
					$bak_projekt=$prdat['fuervorgang'];
					if($prdat['fuervorgang'] == 0)
						$GLOBALS['fxptdata']['lvalues'][$am][$az]['projekt'] .= '<i class="grey">'.$unbekannt.'</i>';
					else
						$GLOBALS['fxptdata']['lvalues'][$am][$az]['projekt'] .= '<font class="darkgrey">'.no_null($prdat['vorgangsnummer']).'</font> - '.text_wrap($prdat['name_projekt'], false, $projekt_kuerzen);
				}

				// Typ (typ)
				$GLOBALS['fxptdata']['lvalues'][$am][$az]['typ']='~'.$ia[$lt];

				// Beschreibung (Beschreibung) ff.
				// ...Sofortmaterial
				$mid=(int)$prdat['sachmittel_id'];
				if(!isset($mua[$mid]))
				{
					$mua[$mid]=array('artikelnummer_int'=>$prdat['artikelnummer_int'], 'artikelnummer_ext'=>$prdat['artikelnummer_ext'], 'name_sachmittel'=>$prdat['name_sachmittel'], 'verkaufseinheit_id'=>$prdat['verkaufseinheit_id']);
					$mua[$mid]['beschreibung']='<font style="color:#816340;">'.$mua[$mid]['name_sachmittel'].'</font>';
					if(strlen((string)$mua[$mid]['artikelnummer_int']))
						$mua[$mid]['beschreibung']='<i class="lightgrey">'.$mua[$mid]['artikelnummer_int'].':</i> '.$mua[$mid]['beschreibung'];
					$mia=getMaterialIcons($mid,$mmxh);
					if(strlen((string)$mia['ico']))
						$mua[$mid]['beschreibung']='<div tooltip="<img src=\''.$mia['256'].$mia['mtime'].'\''.$mia['size_256'].' style=\'max-height:'.$mmxh.'px;width:auto;margin-top:4px;\' />" style="float:right;width:22px;height:22px;border:1px solid #e2e2e2;border-radius:2px;margin-left:64px;cursor:help;"><img src="'.$mia['ico'].$mia['mtime'].'"'.$mia['size_ico'].' /></div><div style="margin-right:32px;">'.$mua[$mid]['beschreibung'].'</div>';
				}
				$GLOBALS['fxptdata']['lvalues'][$am][$az]['beschreibung']='~'.$mua[$mid]['beschreibung'];

				fillColumns($prdat);

				$az++;
			}
		}

		if($az)
		{
			fxf_maskColumnBorder($fa, 1, -1, $am, $az, true);
			$az++;
		}
	}

	if(!$az)
		$GLOBALS['fxptdata']['lvalues'][$am]=true;
}


////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/**
 * ???FUNCTION???
 *
 * @param boolean $prdat - Optional parameter (default = false): ???PARAMETER???
 */
function fillColumns($prdat=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(fxIsArray($prdat))
	{
//fxDebug($prdat, '$prdat');
		$lt=$GLOBALS['lt'];

		// Anmerkung int. (AnmerkungIntern)
		if(!$GLOBALS['AnmerkungIntern_nv'])
		{
			if(isset($prdat['anmerkungintern']))
				$txt=$prdat['anmerkungintern'];
			else if(isset($prdat['anmerkung']))
				$txt=$prdat['anmerkung'];
			else
				$txt=$prdat['beschreibung'];
			if(strlen((string)$txt))
				$txt='<i class="grey">'.text_wrap($txt, $GLOBALS['wrap'], $GLOBALS['text_kuerzen']).'</i>';
			$GLOBALS['fxptdata']['lvalues'][$GLOBALS['am']][$GLOBALS['az']]['anmerkungintern']='~'.$txt;
		}

		// Menge (SachMenge) + Einheit (Verkaufseinheit)
		$a='';
		$u='';
		if($lt == 'z')
		{
			$a=get_entry($prdat['zeitaufwand'],'zeitspanne');
			$u=$GLOBALS['una'][1429];	// Std.
		}
		else if(($lt == 'v') || ($lt == 's'))
		{
			$ui=$GLOBALS['mua'][(int)$prdat['sachmittel_id']]['verkaufseinheit_id'];
			if($ui == 1429)	// Std.
				$a=get_entry($prdat['sachmenge']*3600,'zeitspanne');
			else
				$a=get_entry($prdat['sachmenge'],'dezimal');
			$u=$GLOBALS['una'][$ui];
		}
		$GLOBALS['fxptdata']['lvalues'][$GLOBALS['am']][$GLOBALS['az']]['sachmenge']='~'.$a;
		$GLOBALS['fxptdata']['lvalues'][$GLOBALS['am']][$GLOBALS['az']]['verkaufseinheit']='~'.$u;

		// ZE nv (nicht_abrechenbar), RK nv (rk_nicht_abrechenbar) + MV nv (mat_nicht_abrechenbar)
		$naa=array('z'=>'nicht_abrechenbar', 'r'=>'rk_nicht_abrechenbar', 'v'=>'mat_nicht_abrechenbar');
		foreach($naa as $ltc => $naf)
		{
			if($lt == $ltc)
			{
				if($lt == 'r')
					$n=$prdat['rk_nicht_abrechenbar'];
				else
					$n=$prdat['nicht_abrechenbar'];

				if(!$GLOBALS['changeable'])
				{
					if($n)
						$chk='~<img src="'.$GLOBALS['gfxpath'].'nnl_c_13x13.png" width="13" height="13" tooltip="'.$GLOBALS['nverr'].'" style="cursor:help;">'.fieldHidden($naf.'['.$GLOBALS['am'].']['.$GLOBALS['az'].']',1);
					else
						$chk='~';
				}
				else
				{
					if($GLOBALS['err'])
					{
						if(fxIsArray($_POST) && fxIsArray($_POST[$naf]) && fxIsArray($_POST[$naf][$GLOBALS['am']]) && isset($_POST[$naf][$GLOBALS['am']][$GLOBALS['az']]) && $_POST[$naf][$GLOBALS['am']][$GLOBALS['az']])
							$chk='1';
						else
							$chk='';
					}
					else if($n)
						$chk='1';
					else
						$chk='';

					if(!isset($GLOBALS['fxptdata']['lvalues'][$GLOBALS['am']][$GLOBALS['az']]['nv_ganzzahl1']))
						relSetHidden($prdat);
				}
			}
			else
				$chk='~';
			$GLOBALS['fxptdata']['lvalues'][$GLOBALS['am']][$GLOBALS['az']][$naf]=$chk;
//fxDebug($GLOBALS['fxptdata']['lvalues'][$GLOBALS['am']][$GLOBALS['az']][$naf], $GLOBALS['az'].' - '.$naf);
		}

		// Status (status_benutzer)
		$pd='<div style="width:96px;">';
		if($GLOBALS['freigabe'])
		{
			$GLOBALS['fxptdata']['lvalues'][$GLOBALS['am']][$GLOBALS['az']]['status_benutzer']=$GLOBALS['freigabe_str'].'~'.$pd.'<div style="float:left;width:72px;">';
			if($lt == 'r')
				$pos_id=$prdat['positions_id_rk'];
			else
				$pos_id=$prdat['positions_id'];
			// Freigabe löschbar?
			if(!$pos_id && (($GLOBALS['mr'] == 2) || ($GLOBALS['fxpglobals']['person'] == $GLOBALS['freigabe_pers'])))
			{
				if($GLOBALS['err'] && !$_POST['delchk'][$GLOBALS['am']][$GLOBALS['az']])
					$checked='';
				else
					$checked='1';

				$GLOBALS['fxptdata']['lvalues'][$GLOBALS['am']][$GLOBALS['az']]['status_benutzer'] .= fieldCheckbox('delchk['.$GLOBALS['am'].']['.$GLOBALS['az'].']', '', $checked).fieldHidden('oldfreigabe['.$GLOBALS['am'].']['.$GLOBALS['az'].']',$GLOBALS['freigabe']);

				$GLOBALS['tcbcnt']++;
				if(!isset($GLOBALS['cmskarray'][$GLOBALS['_masknumber']]))
					$GLOBALS['cmskarray'][$GLOBALS['_masknumber']]=array();
				$GLOBALS['cmskarray'][$GLOBALS['_masknumber']][10]=$GLOBALS['tcbcnt'];
//fxDebug($GLOBALS['cmskarray'], __LINE__.': cmskarray');

				if(!isset($GLOBALS['fxptdata']['lvalues'][$GLOBALS['am']][$GLOBALS['az']]['nv_ganzzahl1']))
					relSetHidden($prdat);

				$usi='';
			}
			else
				$usi='&nbsp;<img src="'.$GLOBALS['gfxpath'].'us'.$GLOBALS['freigabe_geschl'].'_b_13x13.png" width="13" height="13" style="margin-right:3px;">';
			$GLOBALS['fxptdata']['lvalues'][$GLOBALS['am']][$GLOBALS['az']]['status_benutzer'] .= $usi.'&nbsp;<font class="'.$GLOBALS['txt_rl'][$GLOBALS['freigabe']]['col'].'" style="vertical-align:top;">'.$GLOBALS['txt_rl'][$GLOBALS['freigabe']]['txt'].'</font>';
			$GLOBALS['fxptdata']['lvalues'][$GLOBALS['am']][$GLOBALS['az']]['status_benutzer'] .= '</div>';
		}
		else
			$GLOBALS['fxptdata']['lvalues'][$GLOBALS['am']][$GLOBALS['az']]['status_benutzer']='~'.$pd;

		if($GLOBALS['abgerechnet'])
		{
			$arid=abs($GLOBALS['abgerechnet']);
			if(!isset($GLOBALS['rnra'][$arid]))
			{
				$sql="SELECT rechnn_f FROM rechnung WHERE rechnungs_id=".$arid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
				$GLOBALS['rnra'][$arid]=db_value($sql);
			}
			if(isset($GLOBALS['rnra'][$arid]) && ($GLOBALS['rnra'][$arid]))
			{
				if($GLOBALS['abgerechnet'] > 0)
					$GLOBALS['fxptdata']['lvalues'][$GLOBALS['am']][$GLOBALS['az']]['status_benutzer'] .= str_replace('{RN}', $GLOBALS['rnra'][$arid], $GLOBALS['ico_rchng']);
				else
					$GLOBALS['fxptdata']['lvalues'][$GLOBALS['am']][$GLOBALS['az']]['status_benutzer'] .= str_replace('{RN}', $GLOBALS['rnra'][$arid], $GLOBALS['ico_rchfp']);
			}
		}
		$GLOBALS['fxptdata']['lvalues'][$GLOBALS['am']][$GLOBALS['az']]['status_benutzer'] .= '</div>';

		// Link (Dummy)
		$clnk=0;
		$ctt='';
		if($GLOBALS['rk'])
		{
			if($GLOBALS['recht_rk'])
			{
				$clnk=150;
				$ctt=$GLOBALS['stt'][3];
			}
		}
		else if($GLOBALS['zs'])
		{
			if($GLOBALS['recht_zs'])
			{
				$clnk=34;
				$ctt=$GLOBALS['stt'][5];
			}
		}
		else if($GLOBALS['mv'])
		{
			if($GLOBALS['recht_mv'])
			{
				$clnk=102;
				$ctt=$GLOBALS['stt'][6];
			}
		}
		else if($GLOBALS['ms'])
		{
			if($GLOBALS['recht_ms'])
			{
				$clnk=78;
				$ctt=$GLOBALS['stt'][13];
			}
		}
		else if($GLOBALS['recht_ze'])
		{
			$clnk=81;
			$ctt=$GLOBALS['stt'][4];
		}
		if($clnk)
		{
			$GLOBALS['fxptdata']['lvalues'][$GLOBALS['am']][$GLOBALS['az']]['dummy']='~';
			if($GLOBALS['changeable'])
			{
				if($clnk != 102)
					$GLOBALS['fxptdata']['lvalues'][$GLOBALS['am']][$GLOBALS['az']]['dummy'] .= fieldLinkImage('dummy', 'icch', '?nid=1&tid='.$prdat['taetigkeit_id'], $ctt, $clnk);
				else
					$GLOBALS['fxptdata']['lvalues'][$GLOBALS['am']][$GLOBALS['az']]['dummy'] .= fieldLinkImage('dummy', 'icch', '?mrid='.$prdat['id'].'&mid='.$prdat['sachmittel_id'].'&gd='.substr((string)$prdat['ist_beginn'],0,8), $ctt, $clnk);
			}
		}

		// HIDDEN
		if($lt == 's')
			$ht='0000';
		else if($lt == 'v')
		{
			$ht='00';
			if($prdat['nicht_abrechenbar'])
				$ht .= '1';
			else
				$ht .= '0';
		}
		else
		{
			if($prdat['nicht_abrechenbar'])
				$ht='1';
			else
				$ht='0';
			if($prdat['rk_nicht_abrechenbar'])
				$ht .= '1';
			else
				$ht .= '0';
			$ht .= '0';
		}
		$GLOBALS['fxptdata']['lvalues'][$GLOBALS['am']][$GLOBALS['az']]['nv_text1']=$ht;
	}
	else
	{
		$GLOBALS['fxptdata']['lvalues'][$GLOBALS['am']][$GLOBALS['az']]['anmerkungintern']			= '~';
		$GLOBALS['fxptdata']['lvalues'][$GLOBALS['am']][$GLOBALS['az']]['sachmenge']				= '~';
		$GLOBALS['fxptdata']['lvalues'][$GLOBALS['am']][$GLOBALS['az']]['verkaufseinheit']			= '~';
		$GLOBALS['fxptdata']['lvalues'][$GLOBALS['am']][$GLOBALS['az']]['nicht_abrechenbar']		= '~';
		$GLOBALS['fxptdata']['lvalues'][$GLOBALS['am']][$GLOBALS['az']]['rk_nicht_abrechenbar']		= '~';
		$GLOBALS['fxptdata']['lvalues'][$GLOBALS['am']][$GLOBALS['az']]['mat_nicht_abrechenbar']	= '~';
		$GLOBALS['fxptdata']['lvalues'][$GLOBALS['am']][$GLOBALS['az']]['status_benutzer']			= '~';

		$GLOBALS['az']++;

		$GLOBALS['fxptdata']['lvalues'][$GLOBALS['am']][$GLOBALS['az']]['checkbox']					= '~';
		$GLOBALS['fxptdata']['lvalues'][$GLOBALS['am']][$GLOBALS['az']]['datum']					= '~';
		$GLOBALS['fxptdata']['lvalues'][$GLOBALS['am']][$GLOBALS['az']]['zeit_id']					= '~';
		$GLOBALS['fxptdata']['lvalues'][$GLOBALS['am']][$GLOBALS['az']]['projekt']					= '~';
	}
}

/**
 * Check if entry has been released already
 *
 * @param integer $zeit_id - Mandatory parameter: Entry id "zeit_id"
 * @param integer $id      - Mandatory parameter: Entry id "id"
 */
function check_freigabe($zeit_id, $id)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$GLOBALS['freigabe']=0;
	$GLOBALS['freigabe_pers']=0;
	$GLOBALS['freigabe_geschl']='m';
	$GLOBALS['freigabe_str']='';

	$freigabe_sql  = "SELECT z.freigabe, z.aenderungs_id, p.pname, p.vorname, p.geschlecht, z.zeitstempel";
	$freigabe_sql .= " FROM zeitdaten_freigabe z, personen p";
	$freigabe_sql .= " WHERE z.aenderungs_id=p.personen_id AND z.mandanten_id=p.mandanten_id AND z.zeit_id=".(int)$zeit_id." AND z.id=".(int)$id." AND z.mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$freigabe_sql .= " ORDER BY z.zeitstempel";
	$freigabe_arr=db_values($freigabe_sql, '*');
//fxDebug($freigabe_arr, $freigabe_sql);
	if(fxIsArray($freigabe_arr))
	{
		$GLOBALS['freigabe_str'] .= '~#<table border=0 cellspacing=1 cellpadding=2>';
		foreach($freigabe_arr as $fra)
		{
			$fname=$fra['pname'];
			if(strlen((string)trim((string)$fra['vorname'])))
				$fname .= ', '.trim((string)$fra['vorname']);
			$GLOBALS['freigabe_str'] .= '<tr><td nowrap bgcolor=#dddddd style=\'border-radius:4px;\'>&nbsp;<font class='.$GLOBALS['txt_rl'][$fra['freigabe']]['col'].'>'.$GLOBALS['txt_rl'][$fra['freigabe']]['txt'].'</font>&nbsp;</td><td nowrap>'.get_entry($fra['zeitstempel'], 'datetime').'</td><td nowrap><font class=black>'.$fname.'</font></td></tr>';
			if($fra['freigabe'] > $GLOBALS['freigabe'])
			{
				$GLOBALS['freigabe']=$fra['freigabe'];
				$GLOBALS['freigabe_pers']=$fra['aenderungs_id'];
				if($fra['geschlecht'] == 9)
					$GLOBALS['freigabe_geschl']='f';
				else
					$GLOBALS['freigabe_geschl']='m';
			}
		}
		$GLOBALS['freigabe_str'] .= '</table>|';
	}
//fxDebug($freigabe_str, $freigabe);
}

/**
 * Set hidden fields
 *
 * @param various $a - Mandatory parameter:  Array conating "zeit_id", "id" + "projekt_id"
 */
function relSetHidden($a)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$GLOBALS['fxptdata']['lvalues'][$GLOBALS['am']][$GLOBALS['az']]['nv_ganzzahl1']=(int)$a['zeit_id'];
	$GLOBALS['fxptdata']['lvalues'][$GLOBALS['am']][$GLOBALS['az']]['nv_ganzzahl2']=(int)$a['id'];
	$GLOBALS['fxptdata']['lvalues'][$GLOBALS['am']][$GLOBALS['az']]['nv_ganzzahl3']=(int)$a['projekt_id'];
}
?>