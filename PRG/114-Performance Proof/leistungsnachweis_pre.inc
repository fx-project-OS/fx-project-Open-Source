<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : leistungsnachweis_pre.inc                                    //
// Version     : 24.1                                                         //
// Begin       : 2021-01-18                                                   //
// Last Change : 2024-04-30                                                   //
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
 * 114: Performance Proof
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

$Personenanzahl=0;
$zm=0;
$pids='';
$Projekte=array();
$laufgaben=array();
$larten=array();
$zdata=array();
$showla=false;

$ptrans=array(chr(13).chr(10)=>'<br />', chr(10).chr(13)=>'<br />', chr(13)=>'<br />', chr(10)=>'<br />');

if($Button_Anzeigen)
{
	// Personen
	if($Personen_ID)
	{
		$pids=(int)$Personen_ID;
		$Pers_Arr=array($pids => $Pers_Arr[$pids]);
	}
	else
		$pids=auswahl_Personen_ID(-1);
//fxDebug($pids, '$pids');
}

// Alle Personen ermitteln
if($pids != '0')
{
	// Keine Projektrechteprüfung für...
	$prc=true;
	if(!$GLOBALS['fxpglobals']['person'])					// ...Sysadmin
		$prc=false;
	else if($GLOBALS['fxpglobals']['user'] <= 1)			// ...Hauptadmin
		$prc=false;
	else if($GLOBALS['fxpglobals']['useradmin'])			// ...Admin
		$prc=false;
	else if($GLOBALS['fxpglobals']['persdat']['roles'][2])	// ...GF
		$prc=false;
	else if($GLOBALS['fxpglobals']['persdat']['roles'][-1] && (strtolower((string)$GLOBALS['fxpglobals']['settings']['vw_darf_zem']) == 'ja'))	// ...VW, falls Mandanteneinstellung "Verwaltung darf Zeiterfassungen für zugeordnete Mitarbeiter durchführen?" gesetzt
		$prc=false;
	$prids="";
	if($prc)
		$prids=auswahl_Projekt_ID(-1);
//echo('<b>$prids:</b><br />'.$prids.'<hr />');

	$infprids="";
	if($Projekt_ID)
		$infprids=inferior_vn($Projekt_ID);
//echo('<b>$infprids:</b><br />'.$infprids.'<hr />');

	$GLOBALS['dbconv']=true;
	$sql  = "SELECT p.personen_id, p.pname, p.vorname, p.geschlecht, p.personentyp, p.personen_initialen, m.maeintrittsdatum, m.maaustrittsdatum";
	$sql .= " FROM personen p, madaten m";
	$sql .= " WHERE p.personen_id IN (".$pids.") AND p.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND m.personen_id=p.personen_id AND m.mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$sql .= " ORDER BY p.pname, p.vorname";
	$Person=db_values($sql,'*');
	if(fxIsArray($Person))
		$Personenanzahl=sizeof($Person);
//fxDebug($Person, $Personenanzahl);

	// Leistungsarten, Tätigkeiten und Anmerkungen standardmäßig ausblenden
	$leistungsarten_nv=true;
	$Taetigkeit_Text_nv=true;
	$Anmerkung_nv=true;

	// Alle Zeitdaten ermitteln
	if($Personenanzahl)
	{

		$pna=array();
		foreach($Person as $psa)
		{
			$pname=$psa['pname'];
			$vname=$psa['vorname'];
			if(strlen((string)$vname))
				$pname .= ', '.$vname;
			$pna[$psa['personen_id']]=$psa;
			$pna[$psa['personen_id']]['name']=$pname;
		}
//fxDebug($pna, '$pna');

		$sql  = "SELECT";
		$sql .= " z.personen_id, z.mandanten_id, z.zuordnungs_mid, z.zeit_id, z.projekt_id, z.zkunde, z.rechnungs_id, z.positions_id, z.rechnungs_id_rk, z.positions_id_rk,";
		$sql .= " z.ist_beginn, z.ist_ende, z.pause, z.zeitaufwand, z.kosten_ek, z.kosten_vk, z.mwst_satz,";
		$sql .= " z.rk_art, z.belegnr, z.land_id, z.rk_kategorie, z.rk_ort1, z.rk_ort2, z.rk_anznacht AS rk_mitfahrer, z.rk_km, z.rk_ek, z.rk_vk, z.rk,";
		$sql .= " z.nicht_abrechenbar, z.rk_nicht_abrechenbar, z.zeitart, z.zeiteinheit, z.freigabe, z.aenderungs_id, z.zeitstempel,";
		$sql .= " t.taetigkeit_text, t.anmerkungintern, t.taetigkeit_id,";
		$sql .= " p.vorgangsnummer, p.name_projekt, p.projektart, p.projektleiter, p.vertreter, p.projekt_status,";
		$sql .= " k.kostenstelle_id".$nl;
		$sql .= " FROM";
		$sql .= " zeitdaten z LEFT JOIN kostenzuordnung k ON (z.zeit_id=k.id AND k.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND k.kostenstellenart=".FXP_TE_TIMEREC." AND k.kostendefinition=903),";
		$sql .= " taetigkeiten t, projekte p".$nl;
		$sql .= " WHERE";
		// ...Version mit Rechteprüfung und Filtereinstellungen entspr. Auswahlfeld
		$sql .= " (z.projekt_id=p.projekt_id) AND p.mandanten_id IN (0,".$GLOBALS['fxpglobals']['client'].") AND (z.zeit_id=t.zeit_id) AND t.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND z.mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$sql .= " AND ((p.projekt_id";
		if(strlen((string)$prids))
			$sql .= " IN (".$prids.")".$nl;
		else
			$sql .= ">0";
		if(strlen((string)$infprids))
			$sql .= " AND p.projekt_id IN (".$infprids.")".$nl;
		$sql .= ") OR p.projekt_id=";
		// "Unbekannte" Zeitdaten anzeigen
		if($unbekannt_anzeigen)
			$sql .= "0";
		else
			$sql .= "-666";
		$sql .= ")".$nl." AND z.personen_id IN (".$pids.") AND (z.zeiteinheit IS NULL OR z.zeiteinheit=0 OR z.zeitart=".FXP_TE_TIMEREC_SUM.") AND z.zeitart IN (".FXP_TE_TIMEREC.",".FXP_TE_TIMEREC_SUM.",".FXP_TE_TRAVELEXP_TIMEREC;
		if($Reisedaten)
			$sql .= ",".FXP_TE_TRAVELEXP;
		$sql .= ") AND z.ist_beginn<='".substr((string)$bis, 0, 8)."240000' AND z.ist_ende>='".substr((string)$von, 0, 8)."000000'";
		// If "Release" checkbox is set then only display released or invoiced entries, else display all
		if($Check_Freigabe && $Freigabe)
			$sql .= " AND ((z.freigabe IS NOT NULL AND z.freigabe>0) OR (z.rechnungs_id IS NOT NULL AND z.rechnungs_id>0) OR (z.rechnungs_id_rk IS NOT NULL AND z.rechnungs_id_rk>0))";
		// Nur Einträge anzeigen, die für ausgew. Kunden gebucht wurden?
		if($Kunde)
			$sql .= " AND z.zkunde=".$Kunde;
		$sql .= $nl." ORDER BY";
		$sql .= " z.personen_id, z.ist_beginn, z.ist_ende";
		$zdata=db_values($sql, '*');
//fxDebug($zdata, $sql);

		// Projekt- und Leistungarten-Array erstellen
		if(fxIsArray($zdata))
		{
			$vnt=array();
			foreach($zdata as $z)
			{
				if(isset($Taetigkeit_Text_nv) && $z['taetigkeit_text'])
					unset($Taetigkeit_Text_nv);
				if(isset($Anmerkung_nv) && $z['anmerkung_intern'])
					unset($Anmerkung_nv);

				if($z['vorgangsnummer'] == '0')
					continue;

				$vnt[substr((string)$z['vorgangsnummer'], 0,4)]=true;
			}

			// Projekt-Array
			if(fxIsArray($vnt))
			{
				$vnor='';
				foreach($vnt as $vn => $dmy)
				{
					if($vnor)
						$vnor .= " OR ";
					$vnor .= "p.vorgangsnummer LIKE '".$vn."%'";
				}
//fxDebug($vnt, '$vnt');

				$sql  = "SELECT p.projekt_id, p.name_projekt, p.vorgangsnummer, p.projektart, p.ref_nummer, p.projekt_status,";
				$sql .= " b.abrechnungsart";
				$sql .= " FROM projekte p, budget_summe b";
				$sql .= " WHERE p.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND (";
				$sql .= "(p.projekt_id";
				if(strlen((string)$prids))
					$sql .= " IN (".$prids.")".$nl;
				else
					$sql .= ">0";
				if(strlen((string)$infprids))
					$sql .= " AND p.projekt_id IN (".$infprids.")";
				$sql .= " AND p.projektart=".FXP_PRJ_TASK.") OR ";
				$sql .= "((".$vnor.") AND p.projektart IN (".FXP_PRJ_MAIN.",".FXP_PRJ_SUB."))";
				$sql .= ") AND b.projekt_id=p.projekt_id AND b.mandanten_id=".$GLOBALS['fxpglobals']['client'];
				$sql .= " ORDER BY ".($GLOBALS['Vorgangsnummer'] ? "" : "name_projekt, ")."vorgangsnummer";
				$par=db_values($sql, '*');
				if(fxIsArray($par))
				{
//fxDebug($par, $sql);
					foreach($par as $p)
					{
						$p['name_projekt']=strtr($p['name_projekt'], $GLOBALS['caption_strtr']);
						$Projekte[$p['vorgangsnummer']]=$p;
					}
					unset($par);
//fxDebug($Projekte, '$Projekte');
				}

				// Alle zu Aufgaben zugeordneten Kostenarten mit untergeordneten Leistungsarten ermitteln
				$sql  = "SELECT z.id, z.kostenstelle_id, b.kostenstelle_id AS leistungsart";
				$sql .= " FROM kostenzuordnung z, kostenbeziehungen b";
				$sql .= " WHERE z.kostenstellenart=904 AND z.kostendefinition=902 AND z.mandanten_id=".$GLOBALS['fxpglobals']['client'];
				$sql .= " AND z.id IN (SELECT p.projekt_id FROM projekte p WHERE p.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND (".$vnor."))";
				$sql .= " AND b.elter=z.kostenstelle_id AND b.mandanten_id=".$GLOBALS['fxpglobals']['client'];
				$tmp=db_values($sql, '*');
//fxDebug($tmp, $sql);
				if(fxIsArray($tmp))
				{
					foreach($tmp as $t)
						$laufgaben[(int)$t['id']]=false;
				}
			}

			// Leistungsarten-Array
			if(fxIsArray($laufgaben))
			{
				$laa=array();
				foreach($zdata as $z)
				{
					if(isset($laufgaben[$z['projekt_id']]))
						$laufgaben[$z['projekt_id']]=true;

					if($z['kostenstelle_id'])
						$laa[$z['kostenstelle_id']]=true;
				}
				foreach($laufgaben as $tid => $lstatus)
				{
					if(!$lstatus)
						unset($laufgaben[$tid]);
				}
//fxDebug($laufgaben, '$laufgaben');
				if(fxIsArray($laufgaben) || fxIsArray($laa))
				{
					$showla=true;
					if(fxIsArray($laa))
						$larten=getActivityTypes($laa);
					else
						$larten=getActivityTypes('');
				}
			}
//fxDebug($larten, '$larten');
		}
	}
	unset($GLOBALS['dbconv']);
}

// Leistungsnachweis für Mitarbeiter
$GLOBALS['fxptdata']['backup']['Zeit_IDs']=array();
if($Personenanzahl)
{
	// Ist ein Ansprechpartner eines Auftraggebers eingeloggt?
	$kunde_id=0;
	if($GLOBALS['fxpglobals']['persdat']['personentyp'] == 620)
	{
		$kunde_id=(int)db_value("SELECT elter FROM personen WHERE personen_id=".$GLOBALS['fxpglobals']['person']." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
//fxDebug($kunde_id, '$kunde_id');
	}

	// Variablen
	$anztage=fxp_date_diff($von, $bis);
	$maske231_nw=true;
	$lz=0;
	$mc=$GLOBALS['_maskcounter'];
	if(isset($GLOBALS['fxptdata']['lvalues']) && isset($GLOBALS['fxptdata']['lvalues'][$mc]))
		unset($GLOBALS['fxptdata']['lvalues'][$mc]);

	// Maskenfelder
	$fa=fxf_getMaskFields(294,false,array('Dummy'=>0));
//fxDebug($fa,'$fa', 0);

	// Anzeigemodus ermitteln -> $zm: -1=Detailiert, 1=Tag(e), 2=Woche(n), 3=Monat(e), 4=Jahr(e)
	if(!$Anzeigeart) // Automatisch
	{
		if($anztage < 32)			// Detailiert
			$zm=-1;
		else if($anztage < 64)		// Tag(e)
			$zm=1;
		else if($anztage < 256)		// Woche(n)
			$zm=2;
		else if($anztage < 1024)	// Monat(e)
			$zm=3;
		else						// Jahr(e)
			$zm=4;
	}
	else // je nach Auswahl
	{
		$az_array=array(741=>-3, 740=>-2, 1553=>-1, 917=>1, 1554=>2, 918=>3, 919=>4);
		$zm=$az_array[$Anzeigeart];
	}
//echo('<b>Anzeigeart</b>: '.$zm.'<hr />');

	// Anzeigetexte
	$m_zwischensumme=meldung(1436, false); // Zwischensumme
	$m_zclass='^class_cr1|';
	$c_zclass=substr((string)$m_zclass,0,-1);
	$m_gesamtsumme=meldung(1555, false); // Gesamtsumme
	$m_gclass='^class_cr3|';
	$c_gclass=substr((string)$m_gclass,0,-1);
	$unbekannt=meldung(1449, true); // [Unbekannt]
	$m_unbekannt='~<i class="grey">'.$unbekannt.'</font>'; // [Unbekannt]
	$m_zeitblock=get_text(1292, '20'); // Zeitdaten-Block
	$m_reiseblock=get_text(1293, '20'); // Reisekosten-Block
	$zeros=10; // 10 Stellen
	$mitfahrer_str=meldung(2808, false); // Mitfahrer

	$stt=explode('|',meldung(3002,false)); // Master|Teilsumme|Hier klicken um zum Master-Eintrag zu springen.|Reisekosten-Eintrag bearbeiten|Zeiterfassungs-Eintrag bearbeiten|Zeitsummen-Eintrag bearbeiten

	// Reisekosten-Arrays
	$rk_art=fxf_getReferenceValues(1884);
	$rk_kat=rk_kategorie_array(false);
	$rk_belege=get_text(1887,'20');
	$rk_land=array();

	// Evtl. selbstdefinierten Begriff statt "Tagesangaben" (ID=1980) verwenden
	if(!is_null($GLOBALS['fxpglobals']['settings']['txt_tangaben']) && strlen((string)$GLOBALS['fxpglobals']['settings']['txt_tangaben']))
		$rk_kat[1980]=trim((string)$GLOBALS['fxpglobals']['settings']['txt_tangaben']);

	// Literal
	$Tage_lit=get_text(289, '10');	// Tage

	// Nowrap-Felder definieren
	$Tage_nw			= true;
	$Datum_nw			= true;
	$Zeit_ID_nw			= true;
	$Pause_nw			= true;
	$Projekt_nw			= true;
	$leistungsarten_nw	= true;
	$Taetigkeit_Text_nw	= true;
	$Aenderungs_ID_nw	= true;
	$Ges_Std_nw			= true;
	$Ver_Std_nw			= true;

	// Monatsnamen
	$monatsnamen=db_values("SELECT tabwert FROM wertetab WHERE id_feld=62 AND id_sprache=".$GLOBALS['fxpglobals']['lang']." AND mandanten_id=0 ORDER BY position", '*');

	// Maskendefinitionen
	if($Personen_ID)
	{
		$fullname=$Person[0]['pname'];
		if($Person[0]['vorname'])
			$fullname .= ', '.$Person[0]['vorname'];
		$maske294_head=meldung(1482, false, '', $fullname, get_entry($von, 'datum'), get_entry($bis, 'datum'), (1+$anztage));
	}
	else
		$maske294_head=meldung(1482, false, '', meldung(1410), get_entry($von, 'datum'), get_entry($bis, 'datum'), (1+$anztage));

	// Monatsanzeige in Anzeigeart Detailiert, Tag(e) oder Woche(n)?
//echo('$zm='.$zm.' - $von='.$von.', $bis='.$bis.'<hr />');
	if(($zm < 3) && (substr((string)$von,0,6) == substr((string)$bis,0,6)) && (substr((string)$von,6,2) == '01') && (substr((string)$bis,6,2) == substr((string)ultimo($bis),6,2)))
	{
		$ay=(int)substr((string)$von,0,4);
		$am=(int)substr((string)$von,4,2);
		$ad=(int)substr((string)$von,6,2);

//function addNavigation($y, $m=0, $w=0, $d=0, $f='right', $trlnk=false)
		$maske294_head .= addNavigation($ay,$am,0,0,'right',true);
	}

	if($zm >= 0)
		$Dummy_nv=true; // Nur bei Detailanzeige Link zur Zeiteingabe

	// Monatstage
	$mo_tage=array(1 => 31, 28, 31, 30,  31,  30,  31,  31,  30,  31,  30,  31);
	// Wochentagsnamen
	$wochentagsnamen=fxf_getWeekdayNames();
	$wochentagsnamen2=fxf_getWeekdayNames(2);

	// ###############################################################################################################
	// #  Einmalige Daten aufbereiten je nach Anzeigeart                                                             #
	// #  $zm: -3=Projekt-Personal, -2=Personal-Projekt, -1=Detailiert, 1=Tag(e), 2=Woche(n), 3=Monat(e), 4=Jahr(e)  #
	// ###############################################################################################################
	switch($zm)
	{
		// PROJEKT-PERSONAL, PERSONAL-PROJEKT + DETAILIERT
		case -3:
		case -2:
		case -1:
			$kw=meldung(1036); 				// Kalenderwoche-Kurzzeichen "KW"
			$zeiterfassung=meldung(1496); 	// Datensatz ändern
			$uskg=get_uskg_array();
			$fart=array(FXP_HL_FULL=>' [1]', FXP_HL_HALF=>' [½]', FXP_HL_DISPLAY=>' [0]');

			// Feiertage ermitteln
			$feiertage=get_feiertage($von, $bis);

			// Leistungsart ausblenden, falls nichts gefunden wurde
			if(!$showla)
				$leistungsarten_nv=true;
		break;

		default:
			// Zeit, Pause, Leistungsart, Tätigkeit + Geänd. von ausblenden
			$Zeit_ID_nv=true;
			$Pause_nv=true;
			$leistungsarten_nv=true;
			$Taetigkeit_Text_nv=true;
			$Aenderungs_ID_nv=true;

			if($zm == 1)									// TAG(E)
			{
				// Feiertage ermitteln
				$feiertage=get_feiertage($von, $bis);
			}
			else if($zm == 2)								// WOCHE(N)
			{
				// Kalenderwoche-Kurzzeichen "KW"
				$kw=meldung(1036);
			}
		break;
	}

	// Colspan sowie letzte anzuzeigene Spalte für Personenüberschrift ermitteln
	$columns=array('Tage'=>3, 'Datum'=>10, 'Zeit_ID'=>12, 'Pause'=>6, 'Projekt'=>26, 'leistungsarten'=>20, 'Taetigkeit_Text'=>58, 'Anmerkung'=>58, 'Aenderungs_ID'=>20, 'Ges_Std'=>5, 'Ver_Std'=>5);
	$pncs=sizeof($columns);
	$lcol='';
	$sncs=0;
	$slcol='';
	foreach($columns as $column => $zeichen)
	{
		if((isset(${$column.'_nv'}) && ${$column.'_nv'}) || !$_POST['rc_'.$GLOBALS['_masknumber'].'_'.$column])
		{
			$pncs -= 1;
			${'col_'.$column}=0;
		}
		else
		{
			${'col_'.$column}=1;
			$lcol=$column;
			if(($column != 'Ges_Std') && ($column != 'Ver_Std'))
			{
				$sncs++;
				$slcol=$column;
			}
		}
	}
	$lcol=strtolower((string)$lcol);
	$slcol=strtolower((string)$slcol);

	// Summe nur anzeigen, falls Stundenspalten ausgewählt wurden
	$disp_sum=false;
	if((!isset($Ges_Std_nv) || !$Ges_Std_nv) && $_POST['rc_'.$GLOBALS['_masknumber'].'_Ges_Std'])
		$disp_sum=true;
	if((!isset($Ver_Std_nv) || !$Ver_Std_nv) && $_POST['rc_'.$GLOBALS['_masknumber'].'_Ver_Std'])
		$disp_sum=true;

	// Zeilenumbruch und max. Zeichen ermitteln
	$zuz=false;
	$mtx=(int)$_POST['text_kuerzen'];
	$ptx=(int)$_POST['projekt_kuerzen'];
//echo(__LINE__.': $zuz='.$zuz.' &rarr; $mtx='.$mtx.', $ptx='.$ptx.'<br />');
	if($_POST['Zeilenumbruch'])
	{
		$zuz=true;
		if(!$mtx && ($col_Taetigkeit_Text || $col_Anmerkung))
		{
			$mtx=$columns['Taetigkeit_Text'];
			if(!$col_Pause)
				$mtx += $columns['Pause'];
			if(!$col_Projekt)
				$mtx += $columns['Projekt'];
		}
		if(!$ptx && $col_Projekt)
		{
			$ptx=$columns['Projekt'];
			if(!$col_Taetigkeit_Text && !$col_Anmerkung)
			{
				$ptx += $columns['Taetigkeit_Text'];
				if(!$col_Pause)
					$mtx += $columns['Pause'];
			}
		}
	}
//echo(__LINE__.': $zuz='.$zuz.' &rarr; $mtx='.$mtx.', $ptx='.$ptx.'<hr size=1 />');

	$ap_ges=0;
	$ap_ver=0;

	// ###############################################################################################################
	// #  Daten aufbereiten pro Person je nach Anzeigeart                                                            #
	// #  $zm: -3=Projekt-Personal, -2=Personal-Projekt, -1=Detailiert, 1=Tag(e), 2=Woche(n), 3=Monat(e), 4=Jahr(e)  #
	// ###############################################################################################################
	if($zm > -3)
	{
		foreach($Person as $p => $pdata)	// Alle Mitarbeiter durchgehen
		{
			// Name
			$lz_name=$pdata['pname'];
			if(strlen((string)trim((string)$pdata['vorname'])))
				$lz_name .= ', '.trim((string)$pdata['vorname']);

			// Prüfen, ob für die Person Zeitdaten gefunden wurden und der Benutzer Rechte darauf hat
			$pzdata=array();
			if(fxIsArray($zdata))
			{
				foreach($zdata as $zdt)
				{
					if($zdt['personen_id'] != $pdata['personen_id'])
						continue;

					// VW+GF+Selber immer, PL/SP nur zu eigenen Vorgängen
					if(!$GLOBALS['fxpglobals']['persdat']['roles'][-1] && !$GLOBALS['fxpglobals']['persdat']['roles'][2] && ($GLOBALS['fxpglobals']['person'] != $pdata['personen_id']) && ($zdt['projektleiter'] != $GLOBALS['fxpglobals']['person']) && ($zdt['vertreter'] != $GLOBALS['fxpglobals']['person']) && ($kunde_id && ($zdt['zkunde'] != $kunde_id)))
						continue;

					$pzdata[]=$zdt;
				}
			}
			if(!sizeof($pzdata))
				continue;
fxDebug($pzdata,'$pzdata: '.$lz_name, 3);

			// Reisekosten-Stack aufbauen
			$RK_Stack=array();

			$ge_ges=0;
			$ge_ver=0;

			$ln_cnt=0;
			$zs_cnt=0;

			// Name (über gesamte Zeile), falls mehr als eine Person gewählt
			if($Personenanzahl > 1)
			{
				$txt='&nbsp;'.person_pic($pdata['personen_id'], 20, $pdata['geschlecht'], $GLOBALS['fxpglobals']['client'], false, 'border-top-left-radius:6px;').'&nbsp;&nbsp;'.$lz_name;
				fxf_listSectHeader($mc, $lz, $txt, 'tage', $lcol, $pncs);
				$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['dummy']='~|';
				$lz++;
			}

			$recht81=0;		// Rechte für ZE (Tag)
			$recht34=0;		// Rechte für ZE (Summe)
			$recht150=0;	// Rechte für Reisekosten
			switch($zm)
			{
				// PERSONAL-PROJEKT
				case -2:
					if($GLOBALS['fxpglobals']['persdat']['roles'][-1] || $GLOBALS['fxpglobals']['persdat']['roles'][2] || ($GLOBALS['fxpglobals']['person'] == $pdata['personen_id']))
					{
						$recht81=tr_rechte(81);		// Rechte für ZE (Tag)
						$recht34=tr_rechte(34);		// Rechte für ZE (Summe)
						$recht150=tr_rechte(150);	// Rechte für Reisekosten
					}

					// Sondertage (Urlaubs-, Schulungs-, Krankheits- bzw. Gleittage) ermitteln
					$uskgtage=array();
					if($Tage)
						$uskgtage=get_sondertage($von, $bis, $pdata['personen_id'], 0, false, false);
//fxDebug($uskgtage, '$uskgtage');

					// Aufgabenreihenfolge ermitteln
					$tsa=array();
					if(sizeof($pzdata))
					{
						foreach($pzdata as $pzk => $pza)
						{
							if(!isset($tsa[$pza['vorgangsnummer']]))
								$tsa[$pza['vorgangsnummer']]=array();
							$tsa[$pza['vorgangsnummer']][]=$pza;
						}
					}
//if(sizeof($tsa)) fxDebug($tsa,'$tsa');

					$dvn='';

					if(sizeof($tsa))
					{
						ksort($tsa);
						foreach($tsa as $tvn => $ppdata)
						{
							$dvn=$tvn;

							// Aufgabe (über gesamte Zeile)
							$tname=$unbekannt;
							if($tvn)
							{
								$tname=$Projekte[$tvn]['name_projekt'];
								if($GLOBALS['Vorgangsnummer'])
									$tname=no_null($tvn).'&nbsp;&nbsp;'.$tname;
							}
							$txt='&nbsp;<b>'.$tname.'</b>';
							$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['tage']='^col_d8d8d8" colspan="'.$pncs.'" height="26" nowrap><!--|~'.$txt;
							$GLOBALS['fxptdata']['lvalues'][$mc][$lz][$lcol]='~-->'.$txt;
							$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['dummy']='~|';
							$lz++;

							$zw_ges=0;
							$zw_ver=0;

							// Starttag aufsplitten
							$jahr=(int)substr((string)$von, 0,4);
							$monat=(int)substr((string)$von, 4,2);
							$tag=(int)substr((string)$von, 6,2);
							$lj=fx_date('L', fx_mktime(0,0,0, 1,1,$jahr));
							if($lj)
								$mo_tage[2]=29;
							else
								$mo_tage[2]=28;

							// Startwochentag ermitteln
							$erwt=(int)$GLOBALS['fxpglobals']['settings']['ersterwt'];
							$wtg=fx_date('w', fx_mktime(0,0,0, $monat, $tag, $jahr));
							$stg=$wtg;

							// Alle Tage durchgehen
							for($tz=0; $tz<=$anztage; $tz++)
							{
								$lz_start=$lz;
								$wtag=substr('0000'.$jahr, -4).substr('00'.$monat, -2).substr('00'.$tag, -2);

								// Wochentagsnamen
								if(!$stg || ($stg > 5))	// Wochenende
									$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['tage']='~<font class="grey">'.$wochentagsnamen2[$stg].'</font>';
								else
									$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['tage']='~'.$wochentagsnamen2[$stg];

								$uskg_tag='';

								// Datum (farblich aufbereitet)
								$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['datum']='';
								$volltag=get_entry($wtag, 'datum');
								// ...Feiertag
								if(fxIsArray($feiertage[$wtag]) && ($feiertage[$wtag]['typ'] > 0.00))
									$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['datum']='~<font class="red" tooltip="'.$feiertage[$wtag]['name'].'">'.$volltag.'</font>';
								// ...Sondertag
								if(fxIsArray($uskgtage[$wtag]))
								{
									$uskg_wtag=(int)$uskgtage[$wtag][-1]['typ'];
									foreach($uskgtage[$wtag] as $uskgtyp => $uskgdata)
									{
										if($uskgtyp == -1)
											continue;

										if(strlen((string)$uskg_tag))
											$uskg_tag .= ' + ';
										$uskg_tag .= $uskg[$uskgtyp];
										if($uskgdata[-1]['dauer'] == 0.5)
											$uskg_tag .= ' [½]';
										else
											$uskg_tag .= ' [1]';
									}

									if(!strlen((string)$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['datum']))
										$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['datum']='~<font class="blue" tooltip="'.$uskg_tag.'">'.$volltag.'</font>';
								}
								if(!strlen((string)$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['datum']))
								{
									// ...Wochentag
									if($stg && ($stg < 6))
										$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['datum']='~'.$volltag;
									// ...Wochenende
									else
										$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['datum']='~<font class="grey">'.$volltag.'</font>';
								}

								// Projekt-Daten
								$zvon=$wtag;
								$zbis=$wtag;
								$zeit_von='';
								$zeit_bis='';

								$ppr=array();	// Daten-Array für Zeiterfassung
								$rpr=array();	// Daten-Array für Reiseerfassung

								$sc=ln_pzdata($ppdata, $zvon, $zbis);
								ln_pa_ra($pdata['personen_id'], $sc, $Projekte, $ppr, $rpr);

								ln_ppr($mc, $lz, $ppr, $pdata['mandanten_id']);
								ln_rpr($mc, $lz, $rpr);

								// Urlaub/Schulung/Krank oder Feiertag
								$tt=trim((string)$GLOBALS['fxptdata']['lvalues'][$mc][$lz_start]['taetigkeit_text']);
								if(substr((string)$tt,0,1) == '~')
									$tt=substr((string)$tt,1);
								if($uskg_tag)
								{
									$tu='<i class="lightblue">'.$uskg_tag.'</i>';
									if(strlen((string)$tt))
										$tt=$tu.' <font class="lightgrey">+</font> '.$tt;
									else
										$tt=$tu;
								}
								if(!strlen((string)$tt))
									$tt='^class_?" height="19|~';
								else
									$tt='~'.$tt;
								$GLOBALS['fxptdata']['lvalues'][$mc][$lz_start]['taetigkeit_text']=$tt;

								// Geänd. von
								if(fxIsArray($sc))
								{
//fxDebug($sc,'$sc: $lz_start='.$lz_start);
									if(!isset($pna[$sc[0]['aenderungs_id']]))
										$pna[$sc[0]['aenderungs_id']]=array('name'=>getPersonName($sc[0]['aenderungs_id']));
									$GLOBALS['fxptdata']['lvalues'][$mc][$lz_start]['aenderungs_id']='~<font class="darkgrey">'.$pna[$sc[0]['aenderungs_id']]['name'].'</font><br /><i class="grey">('.get_entry($sc[0]['zeitstempel'],'datetime').')</i>';
								}

								// Nächster Tag
								$stg++;
								if($stg > 6)
									$stg=0;

								// Einen Tag hochzählen
								$tag++;
								if($tag > $mo_tage[$monat])
								{
									$tag=1;
									$monat++;
									if($monat > 12)
									{
										$monat=1;
										$jahr++;
										$lj=fx_date('L', fx_mktime(0,0,0, 1,1,$jahr));
										if($lj)
											$mo_tage[2]=29;
										else
											$mo_tage[2]=28;
									}
								}
							}

							if($zwischensumme)
							{
								$zs_cnt++;
								$GLOBALS['Ges_Std_height_'.$lz]=20;
								fxf_maskColumnBorder($fa, 3, -1, $mc, $lz, true);
								$txt='&nbsp;<b>'.$m_zwischensumme.':</b>&nbsp;'.$lz_name.'&nbsp;<i>('.$tname.')</i>';
								$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['tage']		= $c_zclass.'" colspan="'.$sncs.'" nowrap style="border-top:1px solid #'.$GLOBALS['msk_nbcol'].';border-right:1px solid #'.$GLOBALS['msk_nbcol'].';"><!--|~'.$txt;
								$GLOBALS['fxptdata']['lvalues'][$mc][$lz][$slcol]		= '~-->'.$txt;
								$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['ges_std']	= $c_zclass.' bold|'.$zw_ges.'^';
								$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['ver_std']	= $c_zclass.' bold|'.$zw_ver.'^';
								$lz++;

								$GLOBALS['Ges_Std_height_'.$lz]=8;
								$GLOBALS['msk_nbcol_294_'.$lz]=$GLOBALS['msk_tbcol'];
								fxf_maskColumnBorder($fa, 1, -1, $mc, $lz, true);
								$lz++;
							}

							$zw_ges=0;
							$zw_ver=0;
							$ln_cnt=0;
						}
					}
				break;

				// DETAILIERT
				case -1:
					if($GLOBALS['fxpglobals']['persdat']['roles'][-1] || $GLOBALS['fxpglobals']['persdat']['roles'][2] || ($GLOBALS['fxpglobals']['person'] == $pdata['personen_id']))
					{
						$recht81=tr_rechte(81);		// Rechte für ZE (Tag)
						$recht34=tr_rechte(34);		// Rechte für ZE (Summe)
						$recht150=tr_rechte(150);	// Rechte für Reisekosten
					}

					// Sondertage (Urlaubs-, Schulungs-, Krankheits- bzw. Gleittage) ermitteln
					$uskgtage=array();
					if($Tage)
						$uskgtage=get_sondertage($von, $bis, $pdata['personen_id'], 0, false, false);
//fxDebug($uskgtage, '$uskgtage');

					// Starttag aufsplitten
					$jahr=(int)substr((string)$von, 0,4);
					$monat=(int)substr((string)$von, 4,2);
					$tag=(int)substr((string)$von, 6,2);
					$lj=fx_date('L', fx_mktime(0,0,0, 1,1,$jahr));
					if($lj)
						$mo_tage[2]=29;
					else
						$mo_tage[2]=28;

					// Startwochentag ermitteln
					$erwt=(int)$GLOBALS['fxpglobals']['settings']['ersterwt'];
					$wtg=fx_date('w', fx_mktime(0,0,0, $monat,$tag,$jahr));
					$stg=$wtg;

					$zw_ges=0;
					$zw_ver=0;

					// Alle Tage durchgehen
					for($tz=0; $tz<=$anztage; $tz++)
					{
						$lz_start=$lz;
						$wtag=substr('0000'.$jahr, -4).substr('00'.$monat, -2).substr('00'.$tag, -2);

						// Wochentagsnamen
						if(!$stg || ($stg > 5))	// Wochenende
							$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['tage']='~<font class="grey">'.$wochentagsnamen2[$stg].'</font>';
						else
							$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['tage']='~'.$wochentagsnamen2[$stg];

						$uskg_tag='';

						// Datum (farblich aufbereitet)
						$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['datum']='';
						$volltag=get_entry($wtag, 'datum');
						// ...Feiertag
						if(fxIsArray($feiertage[$wtag]) && ($feiertage[$wtag]['typ'] > 0.00))
							$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['datum']='~<font class="red" tooltip="'.$feiertage[$wtag]['name'].'">'.$volltag.'</font>';
						// ...Sondertag
						if(fxIsArray($uskgtage[$wtag]))
						{
							$uskg_wtag=(int)$uskgtage[$wtag][-1]['typ'];
							foreach($uskgtage[$wtag] as $uskgtyp => $uskgdata)
							{
								if($uskgtyp == -1)
									continue;

								if(strlen((string)$uskg_tag))
									$uskg_tag .= ' + ';
								$uskg_tag .= $uskg[$uskgtyp];
								if($uskgdata[-1]['dauer'] == 0.5)
									$uskg_tag .= ' [½]';
								else
									$uskg_tag .= ' [1]';
							}

							if(!strlen((string)$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['datum']))
								$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['datum']='~<font class="blue" tooltip="'.$uskg_tag.'">'.$volltag.'</font>';
						}
						if(!strlen((string)$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['datum']))
						{
							// ...Wochentag
							if($stg && ($stg < 6))
								$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['datum']='~'.$volltag;
							// ...Wochenende
							else
								$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['datum']='~<font class="grey">'.$volltag.'</font>';
						}

						// Projekt-Daten
						$zvon=$wtag;
						$zbis=$wtag;
						$zeit_von='';
						$zeit_bis='';

						$ppr=array();	// Daten-Array für Zeiterfassung
						$rpr=array();	// Daten-Array für Reiseerfassung

						$sc=ln_pzdata($pzdata, $zvon, $zbis);
						ln_pa_ra($pdata['personen_id'], $sc, $Projekte, $ppr, $rpr);

						ln_ppr($mc, $lz, $ppr, $pdata['mandanten_id']);
						ln_rpr($mc, $lz, $rpr);

						// Urlaub/Schulung/Krank oder Feiertag
						$tt='';
						if(!is_null($GLOBALS['fxptdata']['lvalues'][$mc][$lz_start]['taetigkeit_text']) && strlen((string)$GLOBALS['fxptdata']['lvalues'][$mc][$lz_start]['taetigkeit_text']))
							$tt=trim((string)$GLOBALS['fxptdata']['lvalues'][$mc][$lz_start]['taetigkeit_text']);
						if(substr((string)$tt,0,1) == '~')
							$tt=substr((string)$tt,1);
						if($uskg_tag)
						{
							$tu='<i class="lightblue">'.$uskg_tag.'</i>';
							if(strlen((string)$tt))
								$tt=$tu.' <font class="lightgrey">+</font> '.$tt;
							else
								$tt=$tu;
						}
						if(!strlen((string)$tt))
							$tt='^class_?" height="19|~';
						else
							$tt='~'.$tt;
						$GLOBALS['fxptdata']['lvalues'][$mc][$lz_start]['taetigkeit_text']=$tt;

						// Geänd. von
						if(fxIsArray($sc))
						{
//fxDebug($sc,'$sc: $lz_start='.$lz_start);
							if(!isset($pna[$sc[0]['aenderungs_id']]))
								$pna[$sc[0]['aenderungs_id']]=array('name'=>getPersonName($sc[0]['aenderungs_id']));
							$GLOBALS['fxptdata']['lvalues'][$mc][$lz_start]['aenderungs_id']='~<font class="darkgrey">'.$pna[$sc[0]['aenderungs_id']]['name'].'</font><br /><i class="grey">('.get_entry($sc[0]['zeitstempel'],'datetime').')</i>';
						}

						// Leereintrag
						if($lz_start == $lz)
						{
							if($leer_anzeigen)
							{
								$lz++;
								$ln_cnt++;
							}
							else
								unset($GLOBALS['fxptdata']['lvalues'][$mc][$lz]);
						}
						else
							$ln_cnt++;

						// Nächster Tag
						$stg++;
						if($stg > 6)
							$stg=0;

						// Checken, ob Zwischensumme angezeigt werden soll
						$szs=0;
						if($stg == $erwt)
						{
							if($leer_anzeigen)
								$szs=1;
							if($zwischensumme)
								$szs=2;
						}
						else if(($tz == $anztage) && $zs_cnt)
							$szs=2;

						if($szs)
						{
							if($szs == 2)
							{
								$zs_cnt++;
								$GLOBALS['Ges_Std_height_'.$lz]=20;
								$cbf=2;
								$btf='';
								if($ln_cnt)
								{
									$cbf=3;
									$btf='border-top:1px solid #'.$GLOBALS['msk_nbcol'].';';
								}
								fxf_maskColumnBorder($fa, $cbf, -1, $mc, $lz, true);
								$txt='&nbsp;<b>'.$m_zwischensumme.':</b>&nbsp;'.$lz_name.'&nbsp;<i>('.$jahr.',&nbsp;'.$kw.'&nbsp;'.kw($wtag).')</i>';
								$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['tage']		= $c_zclass.'" colspan="'.$sncs.'" nowrap style="'.$btf.'border-right:1px solid #'.$GLOBALS['msk_nbcol'].';"><!--|~'.$txt;
								$GLOBALS['fxptdata']['lvalues'][$mc][$lz][$slcol]		= '~-->'.$txt;
								$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['ges_std']	= $c_zclass.' bold|'.$zw_ges.'^';
								$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['ver_std']	= $c_zclass.' bold|'.$zw_ver.'^';
								$lz++;
							}

							$GLOBALS['Ges_Std_height_'.$lz]=8;
							$GLOBALS['msk_nbcol_294_'.$lz]=$GLOBALS['msk_tbcol'];
							fxf_maskColumnBorder($fa, 1, -1, $mc, $lz, true);
							$lz++;

							$zw_ges=0;
							$zw_ver=0;
							$ln_cnt=0;
						}

						// Einen Tag hochzählen
						$tag++;
						if($tag > $mo_tage[$monat])
						{
							$tag=1;
							$monat++;
							if($monat > 12)
							{
								$monat=1;
								$jahr++;
								$lj=fx_date('L', fx_mktime(0,0,0, 1,1,$jahr));
								if($lj)
									$mo_tage[2]=29;
								else
									$mo_tage[2]=28;
							}
						}
					}
				break;

				// TAG(E)
				case 1:
					// Sondertage (Urlaubs-, Schulungs- bzw. Krankheitstage) ermitteln
					$uskgtage=array();
					if($Tage)
						$uskgtage=get_sondertage($von, $bis, $pdata['personen_id'], 0, false, false);
//fxDebug($uskgtage, '$uskgtage');

					// Starttag aufsplitten
					$jahr=(int)substr((string)$von, 0, 4);
					$monat=(int)substr((string)$von, 4, 2);
					$tag=(int)substr((string)$von, 6, 2);
					$lj=fx_date('L', fx_mktime(0,0,0, 1,1,$jahr));
					if($lj)
						$mo_tage[2]=29;
					else
						$mo_tage[2]=28;

					// Startwochentag ermitteln
					$wtg=fx_date('w', fx_mktime(0,0,0, $monat,$tag,$jahr));
					$stg=$wtg;

					$zw_ges=0;
					$zw_ver=0;

					// Alle Tage durchgehen
					for($tz=0; $tz<=$anztage; $tz++)
					{
						$lz_start=$lz;
						$wtag=substr('0000'.$jahr, -4).substr('00'.$monat, -2).substr('00'.$tag, -2);

						// Wochentagsnamen
						$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['tage']='~<b>'.$wochentagsnamen2[$stg].'</b>';

						// Datum (farblich aufbereitet)
						$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['datum']='';
						$volltag=get_entry($wtag, 'datum');
						// ...Feiertag
						if(fxIsArray($feiertage[$wtag]) && ($feiertage[$wtag]['typ'] > 0.00))
							$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['datum']='^class_tft|~'.$volltag;
						// ...Sondertag
						if(fxIsArray($uskgtage[$wtag]) && ($uskgtage[$wtag][-1]['typ'] > 0.00) && !strlen((string)$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['datum']))
							$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['datum']='^class_t'.(int)$uskgtage[$wtag][-1]['typ'].'|~'.$volltag;
						if(!strlen((string)$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['datum']))
						{
							// ...Wochentag
							if($stg && ($stg < 6))
								$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['datum']='~'.$volltag;
							// ...Wochenende
							else
								$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['datum']='^class_twe|~'.$volltag;
						}

						// Projekt-Daten
						$zvon=$wtag;
						$zbis=$wtag;
						$zeit_von='';
						$zeit_bis='';

						$ppr=array();	// Daten-Array für Zeiterfassung
						$rpr=array();	// Daten-Array für Reiseerfassung

						$sc=ln_pzdata($pzdata, $zvon, $zbis);
						ln_pa_ra($pdata['personen_id'], $sc, $Projekte, $ppr, $rpr);

						ln_ppr($mc, $lz, $ppr, $pdata['mandanten_id']);
						ln_rpr($mc, $lz, $rpr);

						// Leereintrag
						if($lz_start == $lz)
						{
							if($leer_anzeigen)
							{
								$lz++;
								$ln_cnt++;
							}
							else
								unset($GLOBALS['fxptdata']['lvalues'][$mc][$lz]);
						}
						else
							$ln_cnt++;

						// Nächster Tag
						$stg++;
						if($stg > 6)
							$stg=0;

						// Checken, ob Zwischensumme angezeigt werden soll
						$szs=0;
						if($stg == $erwt)
						{
							if($leer_anzeigen)
								$szs=1;
							if($zwischensumme)
								$szs=2;
						}
						else if(($tz == $anztage) && $zs_cnt)
							$szs=2;

						if($szs)
						{
							if($szs == 2)
							{
								$zs_cnt++;
								$GLOBALS['Ges_Std_height_'.$lz]=20;
								$cbf=2;
								$btf='';
								if($ln_cnt)
								{
									$cbf=3;
									$btf='border-top:1px solid #'.$GLOBALS['msk_nbcol'].';';
								}
								fxf_maskColumnBorder($fa, $cbf, -1, $mc, $lz, true);
								$txt='&nbsp;<b>'.$m_zwischensumme.':</b>&nbsp;'.$lz_name.'&nbsp;<i>('.$jahr.',&nbsp;'.$kw.'&nbsp;'.kw($wtag).')</i>';
								$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['tage']		= $c_zclass.'" colspan="'.$sncs.'" nowrap style="'.$btf.'border-right:1px solid #'.$GLOBALS['msk_nbcol'].';"><!--|~'.$txt;
								$GLOBALS['fxptdata']['lvalues'][$mc][$lz][$slcol]		= '~-->'.$txt;
								$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['ges_std']	= $c_zclass.' bold|'.$zw_ges.'^';
								$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['ver_std']	= $c_zclass.' bold|'.$zw_ver.'^';
								$lz++;
							}

							$GLOBALS['Ges_Std_height_'.$lz]=8;
							$GLOBALS['msk_nbcol_294_'.$lz]=$GLOBALS['msk_tbcol'];
							fxf_maskColumnBorder($fa, 1, -1, $mc, $lz, true);
							$lz++;

							$zw_ges=0;
							$zw_ver=0;
							$ln_cnt=0;
						}

						// Einen Tag hochzählen
						$tag++;
						if($tag > $mo_tage[$monat])
						{
							$tag=1;
							$monat++;
							if($monat > 12)
							{
								$monat=1;
								$jahr++;
								$lj=fx_date('L', fx_mktime(0,0,0, 1, 1, $jahr));
								if($lj)
									$mo_tage[2]=29;
								else
									$mo_tage[2]=28;
							}
						}
					}
				break;

				// WOCHE(N)
				case 2:
					// Starttag aufsplitten
					$von=(int)substr((string)$von, 0,8);
					$bis=(int)substr((string)$bis, 0,8);

					$jahr=(int)substr((string)$von, 0,4);
					$monat=(int)substr((string)$von, 4,2);
					$tag=(int)substr((string)$von, 6,2);
					$wtag=substr('0000'.$jahr, -4).substr('00'.$monat, -2).substr('00'.$tag, -2);
					$woche=(int)kw(substr((string)$von, 0,8));

					$ejahr=(int)substr((string)$bis, 0,4);
					$emonat=(int)substr((string)$bis, 4,2);
					$etag=(int)substr((string)$bis, 6,2);
					$ewtag=substr('0000'.$ejahr, -4).substr('00'.$emonat, -2).substr('00'.$etag, -2);
					$ewoche=(int)kw(substr((string)$bis, 0,8));

					// Startwochentag ermitteln
					$erwt=(int)$GLOBALS['fxpglobals']['settings']['ersterwt'];
					$wtg=fx_date('w', fx_mktime(0,0,0, $monat,$tag,$jahr));
					$wadd=$erwt-$wtg;
					if($erwt < $wtg)
						$wadd += 7;

					$zw_ges=0;
					$zw_ver=0;

					// Alle Wochen durchgehen
					while($wtag <= $ewtag)
					{
						$lz_start=$lz;

						$jahr=(int)substr((string)$wtag,0,4);
						$woche=(int)kw($wtag);

						$zvon=$wtag;
						if($wadd)
						{
							$zbis=(int)date('Ymd', fx_mktime(0,0,0, (int)substr((string)$wtag, 4,2), (int)substr((string)$wtag, 6,2)+$wadd, $jahr));
							$wadd=0;
						}
						else
							$zbis=(int)date('Ymd', fx_mktime(0,0,0, (int)substr((string)$wtag, 4,2), (int)substr((string)$wtag, 6,2)+6, $jahr));
						if($zbis > $bis)
							$zbis=$bis;

						// Wochennamen + Datum
						$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['tage']='~<b>'.$jahr.'</b>,&nbsp;'.$kw.'&nbsp;'.substr('00'.$woche, -2);
						$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['datum']='~'.get_entry($zvon, 'datum').'&nbsp;-&nbsp;'.get_entry($zbis, 'datum');

						// Projektdaten ermitteln
						$ppr=array();	// Daten-Array für Zeiterfassung
						$rpr=array();	// Daten-Array für Reiseerfassung

						$sc=ln_pzdata($pzdata, $zvon, $zbis);
						ln_pa_ra($pdata['personen_id'], $sc, $Projekte, $ppr, $rpr);

						ln_ppr($mc, $lz, $ppr);
						ln_rpr($mc, $lz, $rpr);

						// Leereintrag
						if($lz_start == $lz)
						{
							if($leer_anzeigen)
							{
								$lz++;
								$ln_cnt++;
							}
							else
								unset($GLOBALS['fxptdata']['lvalues'][$mc][$lz]);
						}
						else
							$ln_cnt++;

						// Eine Woche hochzählen
						$wtag=(int)date('Ymd', fx_mktime(0,0,0, (int)substr((string)$zbis, 4,2), (int)substr((string)$zbis, 6,2)+1, (int)substr((string)$zbis, 0,4)));

						// Checken, ob Zwischensumme angezeigt werden soll
						$szs=0;
						if((int)substr((string)$wtag,0,4) > $jahr)
						{
							if($leer_anzeigen)
								$szs=1;
							if($zwischensumme)
								$szs=2;
						}
						else if(($zbis == $bis) && $zs_cnt)
							$szs=2;

						if($szs)
						{
							if($szs == 2)
							{
								$zs_cnt++;
								$GLOBALS['Ges_Std_height_'.$lz]=20;
								$cbf=2;
								$btf='';
								if($ln_cnt)
								{
									$cbf=3;
									$btf='border-top:1px solid #'.$GLOBALS['msk_nbcol'].';';
								}
								fxf_maskColumnBorder($fa, $cbf, -1, $mc, $lz, true);
								$txt='&nbsp;<b>'.$m_zwischensumme.':</b>&nbsp;'.$lz_name.'&nbsp;<i>('.$jahr.')</i>';
								$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['tage']		= $c_zclass.'" colspan="'.$sncs.'" nowrap style="'.$btf.'border-right:1px solid #'.$GLOBALS['msk_nbcol'].';"><!--|~'.$txt;
								$GLOBALS['fxptdata']['lvalues'][$mc][$lz][$slcol]		= '~-->'.$txt;
								$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['ges_std']	= $c_zclass.' bold|'.$zw_ges.'^';
								$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['ver_std']	= $c_zclass.' bold|'.$zw_ver.'^';
								$lz++;
							}

							$GLOBALS['Ges_Std_height_'.$lz]=8;
							$GLOBALS['msk_nbcol_294_'.$lz]=$GLOBALS['msk_tbcol'];
							fxf_maskColumnBorder($fa, 1, -1, $mc, $lz, true);
							$lz++;

							$zw_ges=0;
							$zw_ver=0;
							$ln_cnt=0;
						}
					}
				break;

				// MONAT(E)
				case 3:
					// Starttag aufsplitten
					$jahr=(int)substr((string)$von, 0,4);
					$monat=(int)substr((string)$von, 4,2);
					$tag=(int)substr((string)$von, 6,2);
					$lj=fx_date('L', fx_mktime(0,0,0, 1,1,$jahr));
					if($lj)
						$mo_tage[2]=29;
					$ejahr=(int)substr((string)$bis, 0,4);
					$emonat=(int)substr((string)$bis, 4,2);
					$etag=(int)substr((string)$bis, 6,2);

					$zw_ges=0;
					$zw_ver=0;

					// Alle Monate durchgehen
					while(($jahr < $ejahr) || (($jahr == $ejahr) && ($monat <= $emonat)))
					{
						$lz_start=$lz;

						$zvon=substr('0000'.$jahr, -4).substr('00'.$monat, -2).substr('00'.$tag, -2);
						if(($jahr == $ejahr) && ($monat == $emonat))
							$zbis=substr('0000'.$jahr, -4).substr('00'.$monat, -2).substr('00'.$etag, -2);
						else
							$zbis=substr('0000'.$jahr, -4).substr('00'.$monat, -2).substr('00'.$mo_tage[$monat], -2);

						// Monatsnamen + Datum
						$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['tage']='~<b>'.$jahr.'</b>,&nbsp;'.$monatsnamen[$monat-1]['tabwert'];
						$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['datum']='~'.get_entry($zvon, 'datum').'&nbsp;-&nbsp;'.get_entry($zbis, 'datum');

						// Projektdaten ermitteln
						$ppr=array();	// Daten-Array für Zeiterfassung
						$rpr=array();	// Daten-Array für Reiseerfassung

						$sc=ln_pzdata($pzdata, $zvon, $zbis);
						ln_pa_ra($pdata['personen_id'], $sc, $Projekte, $ppr, $rpr);

						ln_ppr($mc, $lz, $ppr);
						ln_rpr($mc, $lz, $rpr);

						// Leereintrag
						if($lz_start == $lz)
						{
							if($leer_anzeigen)
							{
								$lz++;
								$ln_cnt++;
							}
							else
								unset($GLOBALS['fxptdata']['lvalues'][$mc][$lz]);
						}
						else
							$ln_cnt++;

						// Einen Monat hochzählen
						$sjahr=0;
						$monat++;
						$tag=1;
						if($monat > 12)
						{
							$monat=1;
							$sjahr=$jahr;
							$jahr++;
							$lj=fx_date('L', fx_mktime(0,0,0, 1, 1, $jahr));
							if($lj)
								$mo_tage[2]=29;
							else
								$mo_tage[2]=28;
						}

						// Checken, ob Zwischensumme angezeigt werden soll
						$szs=0;
						if($sjahr)
						{
							if($leer_anzeigen)
								$szs=1;
							if($zwischensumme)
								$szs=2;
						}
						else if((($jahr > $ejahr) || (($jahr == $ejahr) && ($monat > $emonat))) && $zs_cnt)
						{
							$sjahr=$ejahr;
							$szs=2;
						}

						if($szs)
						{
							if($szs == 2)
							{
								$zs_cnt++;
								$GLOBALS['Ges_Std_height_'.$lz]=20;
								$cbf=2;
								$btf='';
								if($ln_cnt)
								{
									$cbf=3;
									$btf='border-top:1px solid #'.$GLOBALS['msk_nbcol'].';';
								}
								fxf_maskColumnBorder($fa, $cbf, -1, $mc, $lz, true);
								$txt='&nbsp;<b>'.$m_zwischensumme.':</b>&nbsp;'.$lz_name.'&nbsp;<i>('.$sjahr.')</i>';
								$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['tage']		= $c_zclass.'" colspan="'.$sncs.'" nowrap style="'.$btf.'border-right:1px solid #'.$GLOBALS['msk_nbcol'].';"><!--|~'.$txt;
								$GLOBALS['fxptdata']['lvalues'][$mc][$lz][$slcol]		= '~-->'.$txt;
								$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['ges_std']	= $c_zclass.' bold|'.$zw_ges.'^';
								$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['ver_std']	= $c_zclass.' bold|'.$zw_ver.'^';
								$lz++;
							}

							$GLOBALS['Ges_Std_height_'.$lz]=8;
							$GLOBALS['msk_nbcol_294_'.$lz]=$GLOBALS['msk_tbcol'];
							fxf_maskColumnBorder($fa, 1, -1, $mc, $lz, true);
							$lz++;

							$zw_ges=0;
							$zw_ver=0;
							$ln_cnt=0;
						}
					}
				break;

				// JAHR(E)
				case 4:
					// Starttag aufsplitten
					$jahr=(int)substr((string)$von, 0,4);
					$monat=(int)substr((string)$von, 4,2);
					$tag=(int)substr((string)$von, 6,2);
					$lj=fx_date('L', fx_mktime(0,0,0, 1,1,$jahr));
					if($lj)
						$mo_tage[2]=29;
					$ejahr=(int)substr((string)$bis, 0,4);
					$emonat=(int)substr((string)$bis, 4,2);
					$etag=(int)substr((string)$bis, 6,2);

					$zw_ges=0;
					$zw_ver=0;

					// Alle Jahre durchgehen
					while($jahr <= $ejahr)
					{
						$lz_start=$lz;

						$zvon=substr('0000'.$jahr, -4).substr('00'.$monat, -2).substr('00'.$tag, -2);
						if($jahr == $ejahr)
							$zbis=substr('0000'.$ejahr, -4).substr('00'.$emonat, -2).substr('00'.$etag, -2);
						else
							$zbis=substr('0000'.$jahr, -4).'1231';

						// Jahr + Datum
						$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['tage']='~<b>'.$jahr.'</b>';
						$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['datum']='~'.get_entry($zvon, 'datum').'&nbsp;-&nbsp;'.get_entry($zbis, 'datum');

						// Projektdaten ermitteln
						$ppr=array();	// Daten-Array für Zeiterfassung
						$rpr=array();	// Daten-Array für Reiseerfassung

						$sc=ln_pzdata($pzdata, $zvon, $zbis);
						ln_pa_ra($pdata['personen_id'], $sc, $Projekte, $ppr, $rpr);

						ln_ppr($mc, $lz, $ppr);
						ln_rpr($mc, $lz, $rpr);

						// Leereintrag
						if($lz_start == $lz)
						{
							if($leer_anzeigen)
							{
								$lz++;
								$ln_cnt++;
							}
							else
								unset($GLOBALS['fxptdata']['lvalues'][$mc][$lz]);
						}
						else
							$ln_cnt++;

						$tag=1;
						$monat=1;
						$jahr++;
					}
				break;
			}

			$ap_ges += $ge_ges;
			$ap_ver += $ge_ver;

			// Gesamtsumme der Person
			if($disp_sum)
			{
				$GLOBALS['Ges_Std_height_'.$lz]=20;
				$GLOBALS['msk_nbcol_294_'.$lz]=$GLOBALS['msk_tbcol'];
				$cbf=2;
				$btf='';
				if(!$zs_cnt)
				{
					$cbf=3;
					$btf='border-top:1px solid #'.$GLOBALS['msk_tbcol'].';';
				}
				fxf_maskColumnBorder($fa, $cbf, -1, $mc, $lz, true);
				$txt='&nbsp;<b>'.$m_gesamtsumme.':</b>&nbsp;'.$lz_name.'&nbsp;<i>('.get_entry($von,'datum').' - '.get_entry($bis,'datum').')</i>';
				$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['tage']		= $c_gclass.'" colspan="'.$sncs.'" nowrap style="'.$btf.'border-right:1px solid #'.$GLOBALS['msk_tbcol'].';"><!--|~'.$txt;
				$GLOBALS['fxptdata']['lvalues'][$mc][$lz][$slcol]		= '~-->'.$txt;
				$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['ges_std']	= $c_gclass.' bold|'.$ge_ges.'^';
				$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['ver_std']	= $c_gclass.' bold|'.$ge_ver.'^';
				$lz++;

				$GLOBALS['msk_nbcol_294_'.$lz]=$GLOBALS['msk_sbcol'];
				fxf_maskColumnBorder($fa, 1, -1, $mc, $lz, true);
				$lz++;
			}
		}

		// Gesamtsumme aller Personen
		if($disp_sum && $lz && ($Personenanzahl > 1))
		{
			$GLOBALS['Ges_Std_height_'.$lz]=22;
			$GLOBALS['msk_nbcol_294_'.$lz]=$GLOBALS['msk_tbcol'];
			fxf_maskColumnBorder($fa, 0, -1, $mc, $lz, true);
			fxf_maskColumnBorder(array('Ges_Std', 'Ver_Std'), 2, -1, $mc, $lz);
			$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['ges_std']='^class_cr3 bold|'.$ap_ges.'^';
			$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['ver_std']='^class_cr3 bold|'.$ap_ver.'^';
			$lz++;

			$GLOBALS['msk_nbcol_294_'.$lz]=$GLOBALS['msk_sbcol'];
			fxf_maskColumnBorder($fa, 0, -1, $mc, $lz, true);
			fxf_maskColumnBorder(array('Ges_Std', 'Ver_Std'), 1, -1, $mc, $lz);
			$lz++;
		}
	}
	// $zm: -3=Projekt-Personal
	else
	{
//fxDebug($Projekte,'$Projekte');
//fxDebug($Person,'$Person');
		// Aufgabenreihenfolge ermitteln
		$tsa=array();
		if(fxIsArray($zdata))
		{
			foreach($zdata as $zk => $za)
			{
				if(!isset($tsa[$za['vorgangsnummer']]))
					$tsa[$za['vorgangsnummer']]=array();
				$tsa[$za['vorgangsnummer']][]=$za;
			}
		}
//if(sizeof($tsa)) fxDebug($tsa,'$tsa');

		if(sizeof($tsa))
		{
			ksort($tsa);
			$tcnt=0;
			foreach($tsa as $vn => $tda)
			{
				// Prüfen, ob für die Person Zeitdaten auf diese Aufgabe gefunden wurden und der Benutzer Rechte darauf hat
				$pzdata=array();

				foreach($tda as $tac => $zdt)
				{
					foreach($Person as $pac => $pdata)	// Alle Mitarbeiter durchgehen
					{
						if($zdt['personen_id'] != $pdata['personen_id'])
							continue;

						// VW+GF+Eigene Person immer, PL/SP nur zu eigenen Vorgängen
						if(!$GLOBALS['fxpglobals']['persdat']['roles'][-1] && !$GLOBALS['fxpglobals']['persdat']['roles'][2] && ($GLOBALS['fxpglobals']['person'] != $pdata['personen_id']) && ($zdt['projektleiter'] != $GLOBALS['fxpglobals']['person']) && ($zdt['vertreter'] != $GLOBALS['fxpglobals']['person']) && ($kunde_id && ($zdt['zkunde'] != $kunde_id)))
							continue;

						if(!isset($pzdata[$pdata['personen_id']]))
							$pzdata[$pdata['personen_id']]=array();
						$pzdata[$pdata['personen_id']][]=$zdt;
					}
				}

				if(sizeof($pzdata))
				{
//fxDebug($pzdata,'$pzdata');
					// Aufgabe (über gesamte Zeile)
					$tname=$unbekannt;
					if($vn)
					{
						$tname=$Projekte[$vn]['name_projekt'];
						if($GLOBALS['Vorgangsnummer'])
							$tname=no_null($vn).'&nbsp;&nbsp;'.$tname;
					}
					$txt='&nbsp;'.$tname;
					fxf_listSectHeader($mc, $lz, $txt, 'tage', $lcol, $pncs);
					$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['dummy']='~|';
					$lz++;

					$ge_ges=0;
					$ge_ver=0;

					$pid=0;

					// Alle Personen durchgehen
					foreach($pzdata as $pzid => $pza)
					{
						// Neue Person?
						if($pid != $pzid)
						{
							$pid=$pzid;

							// Name (über gesamte Zeile)
							$txt='&nbsp;'.person_pic($pid, 20, $pna[$pid]['geschlecht'], $GLOBALS['fxpglobals']['client'], false).'&nbsp;&nbsp;<b>'.$pna[$pid]['name'].'</b>';
							$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['tage']='^col_d8d8d8" colspan="'.$pncs.'" height="26" nowrap><!--|~'.$txt;
							$GLOBALS['fxptdata']['lvalues'][$mc][$lz][$lcol]='~-->'.$txt;
							$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['dummy']	= '~|';
							$lz++;

							$zw_ges=0;
							$zw_ver=0;

							// Reisekosten-Stack aufbauen
							$RK_Stack=array();

							$recht81=0;		// Rechte für ZE (Tag)
							$recht34=0;		// Rechte für ZE (Summe)
							$recht150=0;	// Rechte für Reisekosten
							if($GLOBALS['fxpglobals']['persdat']['roles'][-1] || $GLOBALS['fxpglobals']['persdat']['roles'][2] || ($GLOBALS['fxpglobals']['person'] == $pid))
							{
								$recht81=tr_rechte(81);		// Rechte für ZE (Tag)
								$recht34=tr_rechte(34);		// Rechte für ZE (Summe)
								$recht150=tr_rechte(150);	// Rechte für Reisekosten
							}

							// Sondertage (Urlaubs-, Schulungs-, Krankheits- bzw. Gleittage) ermitteln
							$uskgtage=array();
							if($Tage)
								$uskgtage=get_sondertage($von, $bis, $pid, 0, false, false);
//fxDebug($uskgtage, '$uskgtage');
						}

						// Starttag aufsplitten
						$jahr=(int)substr((string)$von,0,4);
						$monat=(int)substr((string)$von,4,2);
						$tag=(int)substr((string)$von,6,2);
						$lj=fx_date('L', fx_mktime(0,0,0, 1,1,$jahr));
						if($lj)
							$mo_tage[2]=29;
						else
							$mo_tage[2]=28;

						// Startwochentag ermitteln
						$erwt=(int)$GLOBALS['fxpglobals']['settings']['ersterwt'];
						$wtg=fx_date('w', fx_mktime(0,0,0, $monat,$tag,$jahr));
						$stg=$wtg;

						// Alle Tage durchgehen
						for($tz=0; $tz<=$anztage; $tz++)
						{
							$lz_start=$lz;

							$wtag=substr('0000'.$jahr, -4).substr('00'.$monat, -2).substr('00'.$tag, -2);

							// Wochentagsnamen
							if(!$stg || ($stg > 5))	// Wochenende
								$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['tage']='~<font class="grey">'.$wochentagsnamen2[$stg].'</font>';
							else
								$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['tage']='~'.$wochentagsnamen2[$stg];

							$uskg_tag='';

							// Datum (farblich aufbereitet)
							$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['datum']='';
							$volltag=get_entry($wtag,'datum');
							// ...Feiertag
							if(fxIsArray($feiertage[$wtag]) && ($feiertage[$wtag]['typ'] > 0.00))
								$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['datum']='~<font class="red" tooltip="'.$feiertage[$wtag]['name'].'">'.$volltag.'</font>';
							// ...Sondertag
							if(fxIsArray($uskgtage[$wtag]))
							{
								$uskg_wtag=(int)$uskgtage[$wtag][-1]['typ'];
								foreach($uskgtage[$wtag] as $uskgtyp => $uskgdata)
								{
									if($uskgtyp == -1)
										continue;

									if(strlen((string)$uskg_tag))
										$uskg_tag .= ' + ';
									$uskg_tag .= $uskg[$uskgtyp];
									if($uskgdata[-1]['dauer'] == 0.5)
										$uskg_tag .= ' [½]';
									else
										$uskg_tag .= ' [1]';
								}

								if(!strlen((string)$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['datum']))
									$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['datum']='~<font class="blue" tooltip="'.$uskg_tag.'">'.$volltag.'</font>';
							}
							if(!strlen((string)$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['datum']))
							{
								// ...Wochentag
								if($stg && ($stg < 6))
									$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['datum']='~'.$volltag;
								// ...Wochenende
								else
									$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['datum']='~<font class="grey">'.$volltag.'</font>';
							}

							// Projekt-Daten
							$zvon=$wtag;
							$zbis=$wtag;
							$zeit_von='';
							$zeit_bis='';

							$ppr=array();	// Daten-Array für Zeiterfassung
							$rpr=array();	// Daten-Array für Reiseerfassung

							$sc=ln_pzdata($pza,$zvon,$zbis);
							ln_pa_ra($pid, $sc, $Projekte, $ppr,$rpr);

							ln_ppr($mc, $lz, $ppr, $pdt['mandanten_id']);
							ln_rpr($mc, $lz, $rpr);

							// Urlaub/Schulung/Krank oder Feiertag
							$tt=trim((string)$GLOBALS['fxptdata']['lvalues'][$mc][$lz_start]['taetigkeit_text']);
							if(substr((string)$tt,0,1) == '~')
								$tt=substr((string)$tt,1);
							if($uskg_tag)
							{
								$tu='<i class="lightblue">'.$uskg_tag.'</i>';
								if(strlen((string)$tt))
									$tt=$tu.' <font class="lightgrey">+</font> '.$tt;
								else
									$tt=$tu;
							}
							if(!strlen((string)$tt))
								$tt='^class_?" height="19|~';
							else
								$tt='~'.$tt;
							$GLOBALS['fxptdata']['lvalues'][$mc][$lz_start]['taetigkeit_text']=$tt;

							// Geänd. von
							if(fxIsArray($sc))
							{
//fxDebug($sc,'$sc: $lz_start='.$lz_start);
								if(!isset($pna[$sc[0]['aenderungs_id']]))
									$pna[$sc[0]['aenderungs_id']]=array('name'=>getPersonName($sc[0]['aenderungs_id']));
								$GLOBALS['fxptdata']['lvalues'][$mc][$lz_start]['aenderungs_id']='~<font class="darkgrey">'.$pna[$sc[0]['aenderungs_id']]['name'].'</font><br /><i class="grey">('.get_entry($sc[0]['zeitstempel'],'datetime').')</i>';
							}

							// Nächster Tag
							$stg++;
							if($stg > 6)
								$stg=0;

							// Einen Tag hochzählen
							$tag++;
							if($tag > $mo_tage[$monat])
							{
								$tag=1;
								$monat++;
								if($monat > 12)
								{
									$monat=1;
									$jahr++;
									$lj=fx_date('L', fx_mktime(0,0,0, 1,1,$jahr));
									if($lj)
										$mo_tage[2]=29;
									else
										$mo_tage[2]=28;
								}
							}
						}

						// Zwischensumme Person
						if($zwischensumme)
						{
							$zs_cnt++;
							$GLOBALS['Ges_Std_height_'.$lz]=20;
							fxf_maskColumnBorder($fa, 3, -1, $mc, $lz, true);
							$txt='&nbsp;<b>'.$m_zwischensumme.':</b>&nbsp;'.$tname.'&nbsp;<i>('.$pna[$pid]['name'].')</i>';
							$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['tage']		= $c_zclass.'" colspan="'.$sncs.'" nowrap style="border-top:1px solid #'.$GLOBALS['msk_nbcol'].';border-right:1px solid #'.$GLOBALS['msk_nbcol'].';"><!--|~'.$txt;
							$GLOBALS['fxptdata']['lvalues'][$mc][$lz][$slcol]		= '~-->'.$txt;
							$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['ges_std']	= $c_zclass.' bold|'.$zw_ges.'^';
							$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['ver_std']	= $c_zclass.' bold|'.$zw_ver.'^';
							$lz++;
						}

						$GLOBALS['Ges_Std_height_'.$lz]=8;
						$GLOBALS['msk_nbcol_294_'.$lz]=$GLOBALS['msk_tbcol'];
						fxf_maskColumnBorder($fa, 1, -1, $mc, $lz, true);
						$lz++;
					}

					$ap_ges += $ge_ges;
					$ap_ver += $ge_ver;

					// Gesamtsumme Aufgabe
					if($disp_sum)
					{
						$GLOBALS['Ges_Std_height_'.$lz]=20;
						$GLOBALS['msk_nbcol_294_'.$lz]=$GLOBALS['msk_tbcol'];
						$cbf=2;
						$btf='';
						if(!$zs_cnt)
						{
							$cbf=3;
							$btf='border-top:1px solid #'.$GLOBALS['msk_tbcol'].';';
						}
						fxf_maskColumnBorder($fa, $cbf, -1, $mc, $lz, true);
						$txt='&nbsp;<b>'.$m_gesamtsumme.':</b>&nbsp;'.$tname.'&nbsp;<i>('.get_entry($von,'datum').' - '.get_entry($bis,'datum').')</i>';
						$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['tage']		= $c_gclass.'" colspan="'.$sncs.'" nowrap style="'.$btf.'border-right:1px solid #'.$GLOBALS['msk_tbcol'].';"><!--|~'.$txt;
						$GLOBALS['fxptdata']['lvalues'][$mc][$lz][$slcol]		= '~-->'.$txt;
						$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['ges_std']	= $c_gclass.' bold|'.$ge_ges.'^';
						$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['ver_std']	= $c_gclass.' bold|'.$ge_ver.'^';
						$lz++;

						$GLOBALS['msk_nbcol_294_'.$lz]=$GLOBALS['msk_sbcol'];
						fxf_maskColumnBorder($fa, 1, -1, $mc, $lz, true);
						$lz++;
					}

					$tcnt++;
				}
			}

			// Gesamtsumme aller Personen
			if($disp_sum && $lz && ($tcnt > 1))
			{
				$GLOBALS['Ges_Std_height_'.$lz]=22;
				$GLOBALS['msk_nbcol_294_'.$lz]=$GLOBALS['msk_tbcol'];
				fxf_maskColumnBorder($fa, 0, -1, $mc, $lz, true);
				fxf_maskColumnBorder(array('Ges_Std', 'Ver_Std'), 2, -1, $mc, $lz);
				$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['ges_std']='^class_cr3 bold|'.$ap_ges.'^';
				$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['ver_std']='^class_cr3 bold|'.$ap_ver.'^';
				$lz++;

				$GLOBALS['msk_nbcol_294_'.$lz]=$GLOBALS['msk_sbcol'];
				fxf_maskColumnBorder($fa, 0, -1, $mc, $lz, true);
				fxf_maskColumnBorder(array('Ges_Std', 'Ver_Std'), 1, -1, $mc, $lz);
				$lz++;
			}
		}
	}

	// Spezialreport generieren
	special_timereport($zdata, 0, $Person, $Projekte);
}

if(!fxIsArray($GLOBALS['fxptdata']['lvalues'][$mc]))
	$GLOBALS['fxptdata']['lvalues'][$mc]=true;


////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/**
 * ???FUNCTION???
 *
 * @param various $pzdata - Mandatory parameter: ???PARAMETER???
 * @param various $zvon   - Mandatory parameter: ???PARAMETER???
 * @param various $zbis   - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function ln_pzdata($pzdata, $zvon, $zbis)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$ra=array();

	if(!fxIsArray($pzdata))
		return $ra;

	$zvon=substr((string)$zvon, 0, 8).'000000';
	$zbis=substr((string)$zbis, 0, 8).'240000';
//echo('Array durchsuchen mit: $zvon='.$zvon.' - $zbis='.$zbis.'<br />');

	foreach($pzdata as $pzd)
	{
		if(($pzd['ist_beginn'] <= $zbis) && ($pzd['ist_ende'] >= $zvon))
			$ra[]=$pzd;
	}

	return $ra;
}

/**
 * ???FUNCTION???
 *
 * @param various $pid       - Mandatory parameter: ???PARAMETER???
 * @param various $sc        - Mandatory parameter: ???PARAMETER???
 * @param various &$Projekte - Mandatory parameter: ???PARAMETER???
 * @param various &$ppr      - Mandatory parameter: ???PARAMETER???
 * @param various &$rpr      - Mandatory parameter: ???PARAMETER???
 */
function ln_pa_ra($pid, $sc, &$Projekte, &$ppr, &$rpr)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	global $zeros, $Projekt_ID;

	if(fxIsArray($sc))
	{
		foreach($sc as $sc_cnt => $stunden)
		{
			$GLOBALS['fxptdata']['backup']['Zeit_IDs'][]=$stunden['zeit_id'];

			$vn=$stunden['vorgangsnummer'];
			if(!$vn) // Unbekannt
				$vn='999999999';
			else if($stunden['zuordnungs_mid'] != $GLOBALS['fxpglobals']['client']) // Fremdprojekt
			{
				$vn='-'.substr((string)str_repeat('0', $zeros).$stunden['zuordnungs_mid'], -$zeros).$vn;
				$Projekte[$vn]['name_projekt']=$stunden['name_projekt'];
			}

			if(($vn == '999999999') || (!$Projekt_ID && (substr((string)$vn, 0, 1) == '-')) || ($Projekte[$vn]['projekt_id'] == $stunden['projekt_id']))
			{
				// ---> Reisekosten
				if(($stunden['zeitart'] == FXP_TE_TRAVELEXP) || (($stunden['zeitart'] == FXP_TE_TRAVELEXP_TIMEREC) && $_POST['Reisedaten']))
				{
//echo('<b>Reisekosten</b><br />');
					$rpr[$stunden['ist_beginn']]=$stunden;
					$rpr[$stunden['ist_beginn']]['vn']=$vn;
					if(($pid == $GLOBALS['fxpglobals']['person']) || ($stunden['zuordnungs_mid'] == $GLOBALS['fxpglobals']['client'])) // Eigenes Projekt
					{
						if(($stunden['rechnungs_id_rk']) || ($stunden['freigabe'] > ($GLOBALS['fxpglobals']['persdat']['role']+1)))
							$rpr[$stunden['ist_beginn']]['dummy_show']='';
						else
							$rpr[$stunden['ist_beginn']]['dummy_show']=150;
						$rpr[$stunden['ist_beginn']]['dummy']=$stunden['taetigkeit_id'];
					}
				}
				else
				{
//echo('<b>Zeiteintrag</b><br />');
					// Übergeordnete Projekte ab der Monatsübersicht
					$elter_array=array();
					if($GLOBALS['zm'] > 2)
					{
						$dpos=strpos($vn, '.');
						if($dpos > 0)
						{
							$elter		= '';
							$svn_array	= explode('.', $vn);
							foreach($svn_array as $svn)
							{
								if(strlen((string)$elter))
									$elter .= '.';
								$elter .= $svn;
								if(strlen((string)$elter) < strlen((string)$vn))
									$elter_array[$elter]=true;
								else
									break;
							}
						}
					}

					$vnb=$vn;
					$aba=FXP_INV_NOT;
					if(fxIsArray($Projekte[$vnb]))
						$aba=$Projekte[$vnb]['abrechnungsart'];

					if($GLOBALS['zm'] < 0)	// Projekte + Detailiert
					{
						if($stunden['ist_beginn'] != $stunden['ist_ende'])
							$vn=$stunden['ist_beginn'];
						else
							$vn .= '_'.substr('000'.$sc_cnt, -3);
					}
					if(fxIsArray($ppr[$vn])) // Eintrag vorhanden
					{
						$ppr[$vn]['ges_std'] += $stunden['zeitaufwand'];
						if(!$stunden['nicht_abrechenbar'] && ($aba != FXP_INV_NOT))
							$ppr[$vn]['ver_std'] += $stunden['zeitaufwand'];
					}
					else // neuer Eintrag
					{
						if($GLOBALS['zm'] < 0)	// Projekte + Detailiert
						{
							if($stunden['zeitart'] == FXP_TE_TIMEREC_SUM)
							{
								$ppr[$vn]['zeit_id']='~&sum;';
								if($stunden['zeit_id'] == $stunden['zeiteinheit'])
									$ppr[$vn]['zeit_id'] .= '&nbsp;<i class="s2 lightgrey">'.$GLOBALS['stt'][0].'</i>';
							}
							else
							{
								$zib=get_entry($stunden['ist_beginn'],'zeit');
								$zie=get_entry($stunden['ist_ende'],'zeit');
								if($zib == $zie)
									$ppr[$vn]['zeit_id']='~&nbsp;';
								else
									$ppr[$vn]['zeit_id']='~'.$zib.'-'.$zie;
							}
							$ppr[$vn]['pause']				= $stunden['pause'];
							$ppr[$vn]['taetigkeit_text']	= $stunden['taetigkeit_text'];
							$ppr[$vn]['anmerkungintern']	= $stunden['anmerkungintern'];
							$ppr[$vn]['vn']					= $vnb;
							$ppr[$vn]['la']					= -1;

							if($stunden['kostenstelle_id'] || isset($GLOBALS['laufgaben'][$stunden['projekt_id']]))
								$ppr[$vn]['la']=(int)$stunden['kostenstelle_id'];

							if(($pid == $GLOBALS['fxpglobals']['person']) || ($stunden['zuordnungs_mid'] == $GLOBALS['fxpglobals']['client'])) // Eigenes Projekt
							{
								if(($stunden['rechnungs_id']) || ($stunden['freigabe'] > ($GLOBALS['fxpglobals']['persdat']['role']+1)))
									$ppr[$vn]['dummy_show']='';
								else if($stunden['zeitart'] == FXP_TE_TIMEREC_SUM)
									$ppr[$vn]['dummy_show']=34;
								else if($stunden['zeitart'] == FXP_TE_TIMEREC)
									$ppr[$vn]['dummy_show']=81;
								$ppr[$vn]['dummy']=$stunden['taetigkeit_id'];
							}
						}

						$ppr[$vn]['ges_std']=$stunden['zeitaufwand'];
						if(!$stunden['nicht_abrechenbar'] && ($aba != FXP_INV_NOT))
							$ppr[$vn]['ver_std']=$stunden['zeitaufwand'];
						else
							$ppr[$vn]['ver_std']=0;
					}

					// Übergeordnete Projekte
					if(sizeof($elter_array))
					{
						foreach($elter_array as $elter => $dmy)
						{
							if(!isset($ppr[$elter]))
							{
								if(isset($Projekte[$elter]))
								{
//echo('<b>Eintrag nicht vorhanden:</b> $elter='.$elter.'<br />');
 									$ppr[$elter]['name_projekt']=true;
									$ppr[$elter]['ges_std']=$stunden['zeitaufwand'];
									if(!$stunden['nicht_abrechenbar'] && ($aba != FXP_INV_NOT))
										$ppr[$elter]['ver_std']=$stunden['zeitaufwand'];
									else
										$ppr[$elter]['ver_std']=0;
								}
							}
							else
							{
//echo('<b>Eintrag vorhanden:</b> $elter='.$elter.'<br />');
								$ppr[$elter]['ges_std'] += $stunden['zeitaufwand'];
								if(!$stunden['nicht_abrechenbar'] && ($aba != FXP_INV_NOT))
									$ppr[$elter]['ver_std'] += $stunden['zeitaufwand'];
							}
						}
					}
				}
			}
		}
	}

	if(fxIsArray($ppr))
		ksort($ppr);
	if(fxIsArray($rpr))
		ksort($rpr);
}

/**
 * ???FUNCTION???
 *
 * @param various $mc  - Mandatory parameter:                ???PARAMETER???
 * @param various &$lz - Mandatory parameter:                ???PARAMETER???
 * @param various $ppr - Mandatory parameter:                ???PARAMETER???
 * @param string  $mid - Optional parameter (default = ''):  ???PARAMETER???
 */
function ln_ppr($mc, &$lz, $ppr, $mid='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	global $zeros, $Projekte;

	// Zeit-Einträge
	if(fxIsArray($ppr))
	{
		if($GLOBALS['zm'] >= 0)
		{
 			if($GLOBALS['Reisedaten'])
			{
	 			$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['projekt']	= '~<span class="block">'.$GLOBALS['m_zeitblock'].'</span>';
				$lz++;
			}

			ksort($ppr);
		}

		foreach($ppr as $vn => $daten)
		{
			if($GLOBALS['zm'] < 0) // Projekte + Detailiert
				$vn=$daten['vn'];

			// "Zeit"
			if($daten['zeit_id'])
				$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['zeit_id']=$daten['zeit_id'];

			// "Pause"
			if($daten['pause'])
				$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['pause']=$daten['pause'];

			// "Projekt"
			$pst=FXP_PS_ACTIVE;
			if(fxIsArray($Projekte) && fxIsArray($Projekte[$vn]))
				$pst=$Projekte[$vn]['projekt_status'];
			$pcl='darkgrey';
			if($pst != FXP_PS_ACTIVE)
				$pcl='darkred';
			if(substr((string)$vn, 0,1) == '-') // kein eigenes Projekt
			{
				$dvn='';
				if($GLOBALS['Vorgangsnummer'])
					$dvn=no_null(substr((string)$vn, $zeros+1)).'&nbsp;';
				$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['projekt']='~'.text_wrap('<font class="grey">['.(int)substr((string)$vn, 1,$zeros).']</font>&nbsp;&nbsp;'.$dvn.'<font class="'.$pcl.'">'.$Projekte[$vn]['name_projekt'].'</font>', $GLOBALS['zuz'], $GLOBALS['ptx']);
				$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['ges_std']=$daten['ges_std'];
				$GLOBALS['zw_ges'] += $daten['ges_std'];
				$GLOBALS['ge_ges'] += $daten['ges_std'];
				if(!$daten['ver_std'])
					$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['ver_std']='~<font class="lightgrey">'.get_entry(0,'dezimal').'</font>';
				else
					$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['ver_std']=$daten['ver_std'];
				$GLOBALS['zw_ver'] += $daten['ver_std'];
				$GLOBALS['ge_ver'] += $daten['ver_std'];
			}
			else if($daten['name_projekt']) // Projekt/Teilprojekt
			{
				$dvn='';
				if($GLOBALS['Vorgangsnummer'])
					$dvn=no_null($vn).'&nbsp;';
				$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['projekt']='~'.text_wrap($dvn.'<font class="'.$pcl.'">'.$Projekte[$vn]['name_projekt'].'</font>', $GLOBALS['zuz'], $GLOBALS['ptx']);
				$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['ges_std']='~<i>('.get_entry($daten['ges_std']/3600, 'dezimal').')</i>';
				if(!$daten['ver_std'])
					$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['ver_std']='~<i class="lightgrey">('.get_entry(0,'dezimal').')</i>';
				else
					$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['ver_std']	= '~<i>('.get_entry($daten['ver_std']/3600, 'dezimal').')</i>';
			}
			else // Aufgabe/ToDo
			{
				if($vn == '999999999')		// Unbekannt
					$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['projekt']=$GLOBALS['m_unbekannt'];
				else
				{
					$dvn='';
					if($GLOBALS['Vorgangsnummer'])
						$dvn=no_null($vn).'&nbsp;';
					$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['projekt']='~'.text_wrap($dvn.'<font class="'.$pcl.'">'.$Projekte[$vn]['name_projekt'].'</font>', $GLOBALS['zuz'], $GLOBALS['ptx']);
				}
				$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['leistungsarten']='~'.$GLOBALS['larten'][(int)$daten['la']];
				$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['ges_std']=$daten['ges_std'];
				$GLOBALS['zw_ges'] += $daten['ges_std'];
				$GLOBALS['ge_ges'] += $daten['ges_std'];
				if(!$daten['ver_std'])
					$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['ver_std']='~<font class="lightgrey">'.get_entry(0,'dezimal').'</font>';
				else
					$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['ver_std']=$daten['ver_std'];
				$GLOBALS['zw_ver'] += $daten['ver_std'];
				$GLOBALS['ge_ver'] += $daten['ver_std'];
			}

			// "Tätigkeit"
			if(($GLOBALS['zm'] < 0) && $daten['taetigkeit_text'])
			{
				$txt=strtr($daten['taetigkeit_text'],$GLOBALS['ptrans']);
				$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['taetigkeit_text']='~'.text_wrap($txt, $GLOBALS['zuz'], $GLOBALS['mtx']);
			}

			// "Anmerkung"
			if(($GLOBALS['zm'] < 0) && $daten['anmerkungintern'])
			{
				$txt=strtr($daten['anmerkungintern'],$GLOBALS['ptrans']);
				$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['anmerkung']='~<i class="darkgrey">'.text_wrap($txt, $GLOBALS['zuz'], $GLOBALS['mtx']).'</i>';
			}

			// Link
			ln_link($mc,$lz,$daten,$pst);

			$lz++;
		}
	}
}

/**
 * ???FUNCTION???
 *
 * @param various $mc  - Mandatory parameter: ???PARAMETER???
 * @param various &$lz - Mandatory parameter: ???PARAMETER???
 * @param various $rpr - Mandatory parameter: ???PARAMETER???
 */
function ln_rpr($mc, &$lz, $rpr)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	global $RK_Stack, $zeros, $Projekte, $rk_art, $rk_land;

	// Reise-Einträge
	if(fxIsArray($rpr) && $GLOBALS['Reisedaten'])
	{
		$head=false;
		ksort($rpr);
		foreach($rpr as $vn => $daten)
		{
			// Prüfen, ob Eintrag noch nicht im Stack
			if(!$RK_Stack[$daten['zeit_id']])
			{
				// Spalte ermitteln, in der die RK geschrieben werden
				$sname='projekt';
				if($GLOBALS['zm'] < 0)		// Projekte + Detailiert
				{
					$sname='taetigkeit_text';
					$head=true;
					$vn=$daten['vn'];
				}
				else
					$vn='';

				// Überschrift: "Reisekosten-Block"
				if(!$head)
				{
		 			$GLOBALS['fxptdata']['lvalues'][$mc][$lz][$sname]	= '~<span class="block">'.$GLOBALS['m_reiseblock'].'</span>';
					$lz++;
					$head=true;
				}

				// Eintrag in Stack schreiben
				$RK_Stack[$daten['zeit_id']]=true;
//fxDebug($daten, 'RK-Daten');

				// "Projekt"
				$pst=FXP_PS_ACTIVE;
				if(fxIsArray($Projekte) && fxIsArray($Projekte[$vn]))
					$pst=$Projekte[$vn]['projekt_status'];
				$pcl='darkgrey';
				if($pst != FXP_PS_ACTIVE)
					$pcl='darkred';
				if($vn)
				{
					if(substr((string)$vn, 0,1) == '-') // kein eigenes Projekt
					{
						$dvn='';
						if($GLOBALS['Vorgangsnummer'])
							$dvn=no_null(substr((string)$vn, $zeros+1)).'&nbsp;';
						$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['projekt']='~'.text_wrap('<font class="grey">['.(int)substr((string)$vn, 1, $zeros).']</font>&nbsp;&nbsp;'.$dvn.'<font class="'.$pcl.'">'.$Projekte[$vn]['name_projekt'].'</font>', $GLOBALS['zuz'], $GLOBALS['ptx']);
					}
					else if($vn == '999999999')
						$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['projekt']=$GLOBALS['m_unbekannt'];
					else
					{
						$dvn='';
						if($GLOBALS['Vorgangsnummer'])
							$dvn=no_null($vn).'&nbsp;';
						$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['projekt']='~'.text_wrap($dvn.'<font class="'.$pcl.'">'.$Projekte[$vn]['name_projekt'].'</font>', $GLOBALS['zuz'], $GLOBALS['ptx']);
					}
				}

				// Reise, je nach Spalte
				$art=(int)$daten['rk_art'];
				$land=(int)$daten['land_id'];
				$fahrzeug=(int)$daten['rk_kategorie'];
				$rnr='';
				if(!is_null($daten['belegnr']) && strlen((string)$daten['belegnr']))
					$rnr=trim((string)$daten['belegnr']);
				if(strlen((string)$rnr))
					$rnr='&nbsp;['.$rnr.']';
				if(!$art)	// Alte Einträge
				{
					$art=1984;		// ...Sonstiges
					$fahrzeug=1966;	// ...Kein Fahrzeug
				}
				$GLOBALS['fxptdata']['lvalues'][$mc][$lz][$sname]='~<font class="bold">'.$rk_art[$art].$rnr.'</font>';
				if(($art == 1982) && $land)	// Auslandsreise
				{
					if(!$rk_land[$land])
						$rk_land[$land]=db_value("SELECT land_ort FROM rkpauschalen WHERE land_id=".$land." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
					$GLOBALS['fxptdata']['lvalues'][$mc][$lz][$sname] .= '&nbsp;('.$rk_land[$land].')';
				}

				// Gesamtstunden + Verrechenbare Stunden
				if($daten['zeitaufwand'] && ($daten['zeitart'] != FXP_TE_TRAVELEXP))
				{
					$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['ges_std']=$daten['zeitaufwand'];
					$GLOBALS['zw_ges'] += $daten['zeitaufwand'];
					$GLOBALS['ge_ges'] += $daten['zeitaufwand'];
					if(!$daten['nicht_abrechenbar'])
					{
						$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['ver_std']=$daten['zeitaufwand'];
						$GLOBALS['zw_ver'] += $daten['zeitaufwand'];
						$GLOBALS['ge_ver'] += $daten['zeitaufwand'];
					}
					else
						$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['ver_std']='~<font class="lightgrey">'.get_entry(0,'dezimal').'</font>';
				}

				// Link
				ln_link($mc,$lz,$daten,$pst);
				$lz++;

				// Zusatzinfos zur Reise (Datum, Fahrzeug etc.)
				if($art != 1983)
				{
					// Reisegrund + Anmerkung
					$sql="SELECT taetigkeit_text, anmerkungintern FROM taetigkeiten WHERE zeit_id=".$daten['zeit_id']." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
					$gnd=db_values($sql);
//fxDebug($gnd, $sql);
					if(fxIsArray($gnd))
					{
						// Anmerkung
						if(($GLOBALS['zm'] < 0) && !is_null($gnd['anmerkungintern']) && strlen((string)$gnd['anmerkungintern']))
						{
							$txt=trim((string)$gnd['anmerkungintern']);
							if(strlen((string)$txt))
								$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['anmerkung']='~<i class="darkgrey">'.text_wrap($txt, $GLOBALS['zuz'], $GLOBALS['mtx']).'</i>';
						}

						// Reisegrund
						if(!is_null($gnd['taetigkeit_text']) && strlen((string)$gnd['taetigkeit_text']))
						{
							$txt=trim((string)$gnd['taetigkeit_text']);
							if(strlen((string)$txt))
							{
								$GLOBALS['fxptdata']['lvalues'][$mc][$lz][$sname]='~<img src="'.$GLOBALS['gfxpath'].'dmy.png" width="16" height="16" align="center"><font class="darkgrey">['.text_wrap($txt, $GLOBALS['zuz'], $GLOBALS['mtx']).']</font>';
								$lz++;
							}
						}
					}

					$GLOBALS['fxptdata']['lvalues'][$mc][$lz][$sname]='~<img src="'.$GLOBALS['gfxpath'].'dmy.png" width="16" height="16" align="center"><font class="darkgrey">'.get_entry($daten['ist_beginn'], 'datetime').'&nbsp;-&nbsp;'.get_entry($daten['ist_ende'], 'datetime').'</font>';
					$lz++;

					$GLOBALS['fxptdata']['lvalues'][$mc][$lz][$sname]='~<img src="'.$GLOBALS['gfxpath'].'dmy.png" width="16" height="16" align="center"><font class="darkgrey">';
					if(strlen((string)$daten['rk_ort1']))
						$GLOBALS['fxptdata']['lvalues'][$mc][$lz][$sname] .= $daten['rk_ort1'].'-'.$daten['rk_ort2'].',&nbsp;';
					$GLOBALS['fxptdata']['lvalues'][$mc][$lz][$sname] .= $GLOBALS['rk_kat'][$fahrzeug];
					if(($daten['rk_km'] > 0) || ($daten['rk_mitfahrer'] > 0))
					{
						$GLOBALS['fxptdata']['lvalues'][$mc][$lz][$sname] .= '&nbsp;&nbsp;(';
						if($daten['rk_km'] > 0)
						{
							$GLOBALS['fxptdata']['lvalues'][$mc][$lz][$sname] .= $daten['rk_km'].'&nbsp;km';
							if($daten['rk_mitfahrer'] > 0)
								$GLOBALS['fxptdata']['lvalues'][$mc][$lz][$sname] .= ',&nbsp;';
						}
						if($daten['rk_mitfahrer'] > 0)
							$GLOBALS['fxptdata']['lvalues'][$mc][$lz][$sname] .= '+'.$daten['rk_mitfahrer'].substr((string)$GLOBALS['mitfahrer_str'], 0, 1);
						$GLOBALS['fxptdata']['lvalues'][$mc][$lz][$sname] .= ')';
					}
					$GLOBALS['fxptdata']['lvalues'][$mc][$lz][$sname] .= '</font>';
					$lz++;
				}

				// Tagesangaben
				$tsql="SELECT rk_kategorie FROM zeitdaten WHERE rk_kategorie=1980 AND zeiteinheit=".$daten['zeit_id']." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
				$angaben=db_values($tsql);
				if(fxIsArray($angaben))
				{
					$GLOBALS['fxptdata']['lvalues'][$mc][$lz][$sname]='~<img src="'.$GLOBALS['gfxpath'].'dmy.png" width="16" height="16" align="center">&middot;&nbsp;'.$GLOBALS['rk_kat'][1980];
					if($land)
					{
						if(!$rk_land[$land])
							$rk_land[$land]=db_value("SELECT land_ort FROM rkpauschalen WHERE land_id=".$land." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
						$GLOBALS['fxptdata']['lvalues'][$mc][$lz][$sname] .= '&nbsp;<font class="s2">('.$rk_land[$land].')</font>';
					}
					$lz++;
				}

				// Einzelbelege
				$bsql="SELECT z.belegnr, z.rk_kategorie, z.rk_ort1, t.anmerkungintern FROM zeitdaten z, taetigkeiten t WHERE z.rk_kategorie<>1980 AND z.zeiteinheit=".$daten['zeit_id']." AND z.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND t.zeit_id=z.zeit_id AND t.mandanten_id=".$GLOBALS['fxpglobals']['client']." ORDER BY z.belegnr, z.zeit_id";
				$belege=db_values($bsql, '*');
				if(fxIsArray($belege))
				{
					$GLOBALS['fxptdata']['lvalues'][$mc][$lz][$sname]='~<img src="'.$GLOBALS['gfxpath'].'dmy.png" width="16" height="16" align="center">&middot;&nbsp;'.$GLOBALS['rk_belege'].':';
					$lz++;
					foreach($belege as $bel)
					{
						$blnr=trim((string)$bel['belegnr']);
						if(strlen((string)$blnr))
							$blnr=text_wrap($blnr, $GLOBALS['zuz'], $GLOBALS['mtx']).':&nbsp;&nbsp;';
						$GLOBALS['fxptdata']['lvalues'][$mc][$lz][$sname]='~<img src="'.$GLOBALS['gfxpath'].'dmy.png" width="32" height="16" align="center"><font class="darkgrey">'.$blnr.$GLOBALS['rk_kat'][$bel['rk_kategorie']].'</font>';
						if(strlen((string)$bel['rk_ort1']))
							$GLOBALS['fxptdata']['lvalues'][$mc][$lz][$sname] .= '&nbsp;&nbsp;<font class="s2 grey"('.$bel['rk_ort1'].')</font>&nbsp;';

						// "Anmerkung"
						if(($GLOBALS['zm'] < 0) && !is_null($bel['anmerkungintern']) && strlen((string)$bel['anmerkungintern']))
						{
							$bel_notiz=trim((string)$bel['anmerkungintern']);
							$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['anmerkung']='~<i class="darkgrey">'.text_wrap($bel_notiz, $GLOBALS['zuz'], $GLOBALS['mtx']).'</i>';
						}
						$lz++;
					}
				}
			}
		}
	}
}

/**
 * ???FUNCTION???
 *
 * @param various $mc  - Mandatory parameter: ???PARAMETER???
 * @param various $lz    - Mandatory parameter: ???PARAMETER???
 * @param various $daten - Mandatory parameter: ???PARAMETER???
 * @param various $pst   - Mandatory parameter: ???PARAMETER???
 */
function ln_link($mc,$lz,$daten,$pst)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(($GLOBALS['zm'] < 0) && $daten['dummy'])
	{
		$trr=0;
		if(strlen((string)$daten['dummy_show']))
		{
			$trr=(int)$daten['dummy_show'];
			if(!isset($GLOBALS['recht'.$trr]) || !$GLOBALS['recht'.$trr])
				$trr=0;
		}
		if($trr && ($pst == FXP_PS_ACTIVE))
		{
			$tt='';
			if($trr == 150)
				$tt=$GLOBALS['stt'][3];
			else if($trr == 81)
				$tt=$GLOBALS['stt'][4];
			else if($trr == 34)
				$tt=$GLOBALS['stt'][5];
			$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['dummy']='~'.fieldLinkImage('link'.$lz, 'icch', '?tr='.$trr.'&tid='.$daten['dummy'], $tt, $trr);
		}
		else
			$GLOBALS['fxptdata']['lvalues'][$mc][$lz]['dummy']='~<img class="ainactive" src="'.$GLOBALS['gfxpath'].'icch.png">';
	}
}
?>