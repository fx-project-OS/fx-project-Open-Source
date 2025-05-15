<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : std_rep_projekt_pre.inc                                      //
// Version     : 24.1                                                         //
// Begin       : 2020-11-18                                                   //
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
 * PF 85: Cost Report
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

$GLOBALS['fxptdata']['tmp_stundennachweis']=false;
$GLOBALS['with_pic']=false;
$GLOBALS['no_waehrung']=true;

if(fxIsArray($_POST) && $_POST['Button_Anzeigen'])
{
//fxDebug($knd_array, '$knd_array');
//fxDebug($prj_array, '$prj_array');

	// Aktuelle Rolle
	$mr=$GLOBALS['fxpglobals']['persdat']['role'];
	if($GLOBALS['fxpglobals']['persdat']['roles'][-1])
		$mr=2;

	// Maskenvariablen
	$anztage=fxp_date_diff($von, $bis);
	$maske231_nw=true;
	$mc=$GLOBALS['_maskcounter'];

	if(isset($GLOBALS['fxptdata']['lvalues']) && isset($GLOBALS['fxptdata']['lvalues'][$mc]))
		unset($GLOBALS['fxptdata']['lvalues'][$mc]);

	// Texte
	$zwischensumme		= meldung(1436, false); // Zwischensumme
	$gesamtsumme		= meldung(1555, false); // Gesamtsumme
	$unbekannt			= meldung(1449, false); // [Unbekannt]
	$rk_name			= meldung(1508, false);	// {Reisek.}
	$so					= meldung(1528, false);	// s.o.
	$tsb_abgerechnet	= meldung(2528, false);	// Zeiterfassung ist bereits freigegeben oder abgerechnet
	$trk_abgerechnet	= meldung(2596, false);	// Reisekosten sind bereits freigegeben oder abgerechnet
	$verpfl				= get_text(1888, '15');	// Verpflegung
	$uebern				= get_text(1890, '15');	// Übernachtung
	$sachbez			= get_text(1894, '20');	// abzgl. Sachbezüge
	$mitfahrer_str		= meldung(2808, false);	// Mitfahrer
	$fahrkosten			= meldung(3148, false);	// Fahrkosten
	$belege				= get_text(1887,'20');	// Belege
	$keineaufgaben		= meldung(3241, false);	// Zu keinen Aufgaben zugeordnet!
	$keineze			= meldung(3134, false);	// Keine Zeiterfassung vorhanden!
	$za_array			= array(0=>meldung(1302, false), meldung(2828, false));	// 0: Firma, 1: Privat
	$stt				= explode('|',meldung(3002,false));			// Master|Teilsumme|Hier klicken um zum Master-Eintrag zu springen.|Reisekosten-Eintrag bearbeiten|Zeiterfassungs-Eintrag bearbeiten|Zeitsummen-Eintrag bearbeiten
	$itt				= explode('|',meldung(3237,false));			// Alle Einträge noch nicht freigegeben!|Alle Einträge bereits freigegeben!|Mehre Einträge mit unterschiedlichen Freigabe-Stati!|Alle Einträge als [verrechenbar] gekennzeichnt!|Alle Einträge als [nicht verrechenbar] gekennzeichnet!|Alle Einträge [verrechenbar] wegen Buchung auf Festpreis-Aufgabe!|Alle Einträge [nicht verrechenbar] wegen Buchung auf nicht abrechenbarer Aufgabe!|Mehrere Einträge mit unterschiedlichen Verrechenbar-Stati!|Alle Einträge noch nicht abgerechnet und auch nicht abrechenbar!|Alle Einträge noch nicht abgerechnet, aber abrechenbar!|Alle Einträge bereits abgerechnet in|Mehre Einträge - einige bereits abgerechnet in
//fxDebug($itt,'$itt');

	// Einstellungen auslesen
	$vatset=get_double($GLOBALS['fxpglobals']['settings']['mwst_satz']);		// Aktueller Mehrwertsteuersatz als Double-Wert, z.B. 19.0
	$vatcal=(100.0+$vatset)/100.0;												// ...und zum Kalkulieren, z.B. 1.19
	$accuracy=$GLOBALS['fxpglobals']['settings']['nkstellen'];					// Anzahl der Nachkommastellen, z.B. 2
	$currency=$GLOBALS['waehrung'];
	if(($currency === NULL) || !strlen((string)$currency))
		$currency=$GLOBALS['fxpglobals']['settings']['waehrung'];

	// Einheiten
	$unit_c='&nbsp;'.$currency;
	$unit_h='&nbsp;h';
	$unit_ch=$unit_c.'/h';

	// Einige Variablen zwischenspeichern für PDF-Aufbereitung
	if(!fxIsArray($GLOBALS['fxptdata']['backup']))
		$GLOBALS['fxptdata']['backup']=array();
	$GLOBALS['fxptdata']['backup']['pdf']=array("sdate"=>$von, "edate"=>$bis, "pers"=>$Personen_ID, "prj"=>$Projekt_ID, "cust"=>$Kunde);

	// Anzeigemodus ermitteln -> $zm: 1=Tag, 2=Monat, 3=Jahr
	if(!$Anzeigeart) 				// Automatisch
	{
		if($anztage < 40)			// Tag(e)
			$zm=1;
		else if($anztage < 1000)	// Monat(e)
			$zm=2;
		else						// Jahr(e)
			$zm=3;
	}
	else
		$zm=$Anzeigeart-916;		// je nach Auswahl
	if($sort_travel)
	{
		$zm=3;
		$Projekt_lit=meldung(2898,false);	// Reisen
	}
//echo('Anzeigemodus $zm='.$zm.'<br />');

	// NotVisible and NoWrap fields
	$nva=array();
	$sql="SELECT m.eingabefeld, f.feldname FROM maskenfelder m, felder f WHERE m.nummer=231 AND f.id=m.eingabefeld AND f.mandanten_id=0 ORDER BY m.spalte";
	$nvt=db_values($sql,'*');
//fxDebug($nvt,$sql);
	if(fxIsArray($nvt))
	{
		foreach($nvt as $n)
		{
			$nva[$n['feldname']]=false;
			$GLOBALS[$n['feldname'].'_nw']=true;
		}
	}
	if(fxIsArray($_POST) && isset($_POST['rc_'.$GLOBALS['fxpglobals']['tr'].'_set']))
	{
		foreach($_POST as $psk => $psv)
		{
			if(($psk != 'rc_'.$GLOBALS['fxpglobals']['tr'].'_set') && (substr((string)$psk,0,7) == 'rc_231_'))
			{
				$psk=substr((string)$psk,7);
				$psv=(int)$psv;
				if($psv%2 == 0)
					$nva[$psk]=true;
			}
		}
	}

	if($sort_travel)
	{
		$nva['Tage']=true;
		$nva['Datum']=true;
		$Tage_ncs=true;
		$Datum_ncs=true;
	}

	if($sort_travel || ($GLOBALS['fxpglobals']['tr'] == 150) || ($GLOBALS['fxpglobals']['tr'] == 191) || ($Reisekosten == 2139))
	{
		$nva['SOLLStunden']=true;
		$nva['ISTStunden']=true;
		$nva['Delta']=true;
		$nva['Ver_Std']=true;
		$SOLLStunden_ncs=true;
		$ISTStunden_ncs=true;
		$Delta_ncs=true;
		$Ver_Std_ncs=true;
	}

	if($zm > 1)
	{
		$nva['Zeit_ID']=true;
		$Zeit_ID_ncs=true;
	}

	// Leistungsart
	if(isset($leistungsarten) && $leistungsarten)
	{
		$nva['leistungsarten']=false;
		$leistungsarten_fcs=true;
	}
	else
	{
		$nva['leistungsarten']=true;
		$leistungsarten_ncs=true;
	}

	// Beim Zeitkostenreport folgende Felder nicht anzeigen
	if($GLOBALS['fxpglobals']['tr'] == 190)
	{
		$nva['MwSt']=true;
		$nva['Bruttobetrag']=true;

		$MwSt_ncs=true;
		$Bruttobetrag_ncs=true;
	}

	// Bei Reisekostenerfassung oder Reisekostenreport folgende Felder nicht anzeigen
	if(($GLOBALS['fxpglobals']['tr'] == 150) || ($GLOBALS['fxpglobals']['tr'] == 191))
	{
		$nva['Zeit_ID']=true;
		$nva['SOLLStunden']=true;
		$nva['ISTStunden']=true;
		$nva['Delta']=true;
		$nva['EK']=true;
		$nva['Ver_Std']=true;
		$nva['VK']=true;
		$nva['ertrag']=true;
		$nva['Marge_verbr']=true;
		$nva['Freigabe']=true;
		$nva['abgerechnet']=true;

		$Zeit_ID_ncs=true;
		$SOLLStunden_ncs=true;
		$ISTStunden_ncs=true;
		$Delta_ncs=true;
		$EK_ncs=true;
		$Ver_Std_ncs=true;
		$VK_ncs=true;
		$ertrag_ncs=true;
		$Marge_verbr_ncs=true;
		$Freigabe_ncs=true;
		$abgerechnet_ncs=true;
	}

	// Kostenfelder anzeigen?
	$uc=darf_kosten_sehen(0);
//fxDebug($uc,'$uc');
	if(!$uc || ($uc == 2))	// 0=keine oder 2=ext
	{
		$nva['EK']=true;
		$EK_ncs=true;
	}
	if(!$uc || ($uc == 1))	// 0=keine oder 1=int
	{
		$nva['VK']=true;
		$nva['ertrag']=true;
		$VK_ncs=true;
		$ertrag_ncs=true;
	}
	if($uc != 3)	// nicht 3=alle
	{
		$nva['Marge_verbr']=true;
		$Marge_verbr_ncs=true;
	}

	// Falls Reisekosten angezeigt werden sollen immer Spalte "Projekt" anzeigen
	if(($Reisekosten == 2137) || ($Reisekosten == 2139))
	{
		$nva['Projekt']=false;
		$_POST['rc_231_Projekt']=3;
	}

	// Colspan, first and last field
	$colspan=0;
	$scolspan=0;
	$sc=true;
	$sum=0;
	$fcol='';
	$slcol='';
	$tlcol='';
	foreach($nva as $nvvar => $nvval)
	{
		if($nvvar == 'SOLLStunden')
			$sc=false;
		if($nvval)
			$GLOBALS[$nvvar.'_nv']=true;
		else
		{
			$colspan++;
			if(!strlen((string)$fcol))
				$fcol=strtolower((string)$nvvar);
			$tlcol=strtolower((string)$nvvar);

			if($sc)
			{
				$scolspan++;
				$slcol=strtolower((string)$nvvar);
			}
			else
				$sum++;

			if(isset($GLOBALS[$nvvar.'_nv']))
				unset($GLOBALS[$nvvar.'_nv']);
		}
	}
//echo('$colspan='.$colspan.', $fcol='.$fcol.', $tlcol='.$tlcol.' -- $scolspan='.$scolspan.', $slcol='.$slcol.' -- $sum='.$sum.'<hr />');

	// Personen ermitteln
	$prs_id=(int)$Personen_ID;
	$prs_anz=0;
	$prs_sql='';
	if($prs_id > 0)
	{
		$prs_sql=" AND z.personen_id=".$prs_id;
		$prs_anz=1;
	}
	else
	{
		foreach($prs_array as $id => $name)
		{
			$prs_sql .= " OR z.personen_id=".(int)$id;
			if(!$prs_id)
				$prs_anz++;
		}
		$prs_sql=" AND (".substr((string)$prs_sql, 4).")";
	}
//fxDebug($prs_sql, $prs_id.' ('.$prs_anz.')');
	if(($prs_id >=0 ) && (strlen((string)$prs_sql)))
	{
		$sql="SELECT z.personen_id, z.pname, z.vorname, z.geschlecht FROM personen z WHERE z.mandanten_id=".$GLOBALS['fxpglobals']['client'].$prs_sql." ORDER BY z.pname, z.vorname";
		$personen=db_values($sql, '*');
//fxDebug($personen, $sql);
	}
	else
		$personen=array(0=>true);

	// Vorgänge ermitteln
	$sess_pers_id=0;
	$sess_ben_id=0;
	$projekte[0]=array('elt'=>0, 'art'=>FXP_PRJ_TASK, 'vn'=>'0', 'name'=>$unbekannt, 'st'=>FXP_PS_ACTIVE, 'pabr'=>FXP_INV_HOUR, 'mabr'=>FXP_INV_MAT_HU);
	$prj_id=(int)$Projekt_ID;
	$knd_id=(int)$Kunde;

	if(($GLOBALS['fxpglobals']['tr'] == 150) && ($prs_anz == 1) && $prs_id)
	{
		$sess_pers_id=$GLOBALS['fxpglobals']['person'];
		$sess_ben_id=$GLOBALS['fxpglobals']['user'];
		$GLOBALS['fxpglobals']['person']=$prs_id;
		$GLOBALS['fxpglobals']['user']=1;
	}
	if($GLOBALS['fxpglobals']['tr'] != 150)
	{
		$prj_sql=auswahl_Projekt_ID(-1);
		if($prj_id > 0)
			$prj_sql .= " AND p.projekt_id IN (".inferior_vn($prj_id).")";
		if($knd_id > 0)
			$prj_sql .= " AND p.kunde=".$knd_id;
	}
	else
		$prj_sql=prights_get(0);
//fxDebug($prj_sql, $prj_id);
	if($sess_pers_id)
	{
		$GLOBALS['fxpglobals']['person']=$sess_pers_id;
		$GLOBALS['fxpglobals']['user']=$sess_ben_id;
	}

	$sql  = "SELECT p.projekt_id, p.elter, p.vorgangsnummer, p.name_projekt, p.projektart, p.projekt_status, b.abrechnungsart, b.abrechnungsart_mat";
	$sql .= " FROM projekte p, budget_summe b WHERE b.projekt_id=p.projekt_id AND p.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND b.mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$sql .= " AND p.projekt_id IN (".$prj_sql.")";
	$sql .= " ORDER BY p.vorgangsnummer";
	$tmp=db_values($sql, '*');
	if(fxIsArray($tmp))
	{
		foreach($tmp as $t)
		{
			$id=(int)$t['projekt_id'];
			$projekte[$id]=array('elt'=>(int)$t['elter'], 'art'=>(int)$t['projektart'], 'vn'=>no_null($t['vorgangsnummer']), 'name'=>trim((string)$t['name_projekt']), 'st'=>(int)$t['projekt_status'], 'pabr'=>(int)$t['abrechnungsart'], 'mabr'=>(int)$t['abrechnungsart_mat']);
		}
	}
//fxDebug($projekte, $sql);

	// Alle zu Aufgaben zugeordneten Kostenarten mit untergeordneten Leistungsarten ermitteln
	$laufgaben=array();
	$larten=array();
	if(!$nva['leistungsarten'])
	{
		$sql  = "SELECT z.id, z.kostenstelle_id, b.kostenstelle_id AS leistungsart";
		$sql .= " FROM kostenzuordnung z, kostenbeziehungen b";
		$sql .= " WHERE z.kostenstellenart=904 AND z.kostendefinition=902 AND z.mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$sql .= " AND z.id IN (".$prj_sql.") AND b.elter=z.kostenstelle_id AND b.mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$tmp =db_values($sql, '*');
//fxDebug($tmp, $sql);
		if(fxIsArray($tmp))
		{
			foreach($tmp as $t)
				$laufgaben[(int)$t['id']]=false;
		}
//fxDebug($laufgaben, $sql);
	}

	// Start
	if(fxIsArray($personen))
	{
		// Zeitdaten ermitteln
		// ...nur RK
		if($Reisekosten == 2139)
			$zart_sql=" AND z.zeitart IN (".FXP_TE_TRAVELEXP_TIMEREC.",".FXP_TE_TRAVELEXP.")";
		// ...nur ZE
		else if($Reisekosten == 2140)
			$zart_sql=" AND z.zeitart IN (".FXP_TE_TIMEREC.",".FXP_TE_TIMEREC_SUM.",".FXP_TE_TRAVELEXP_TIMEREC.")";
		// ...ZE + RK
		else
			$zart_sql=" AND z.zeitart IN (".FXP_TE_TIMEREC.",".FXP_TE_TIMEREC_SUM.",".FXP_TE_TRAVELEXP_TIMEREC.",".FXP_TE_TRAVELEXP.")";

		$zvon=substr((string)$von,0,8).'000000';
		$zbis=substr((string)$bis,0,8).'240000';

		$sql  = "SELECT";
		$sql .= " z.zeitart, z.zeit_id, z.personen_id, z.projekt_id, z.rechnungs_id, z.rechnungs_id_rk, z.nicht_abrechenbar, z.rk_nicht_abrechenbar,";
		$sql .= " z.ist_beginn, z.ist_ende, z.pause, z.zeitaufwand, z.freigabe,";
		$sql .= " z.mwst_satz, z.kosten_ek, z.kosten_vk, z.rk_ek, z.rk_vk, z.rk,";
		$sql .= " z.rk_art, z.land_id, z.rk_kategorie, z.rk_ort1, z.rk_ort2, z.rk_km, z.rk_anznacht AS rk_mitfahrer, z.belegnr, z.zeiteinheit, z.verz_grund,";
		$sql .= " t.taetigkeit_id, t.taetigkeit_text, t.anmerkungintern,";
		$sql .= " k.kostenstelle_id ".$nl;
		$sql .= "FROM";
		$sql .= " zeitdaten z LEFT JOIN kostenzuordnung k ON (z.zeit_id=k.id AND k.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND k.kostenstellenart=".FXP_TE_TIMEREC." AND k.kostendefinition=903), taetigkeiten t ".$nl;
		$sql .= "WHERE";
		$sql .= " z.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND t.mandanten_id=".$GLOBALS['fxpglobals']['client'].$zart_sql;
		$sql .= " AND ((z.ist_beginn<='".$zbis."' AND z.ist_ende>='".$zvon."') OR (z.ist_ende IS NULL AND z.ist_beginn>='".$zvon."' AND z.ist_beginn<='".$zbis."'))";
		$sql .= $prs_sql." AND t.zeit_id=z.zeit_id ";
		$sql .= "ORDER BY";
		$sql .= " z.ist_beginn, z.belegnr";

		$zeitarray=db_values($sql, '*');
fxDebug($zeitarray,$sql, 2);

		// Gefiltertes Zeitdaten-Array + Personen-Ist-Stunden-Array erstellen
		$zeitdaten=array();
		$ahp=array();
		if(fxIsArray($zeitarray))
		{
			foreach($zeitarray as $za)
			{
				// Zeitdaten-Array
				$add=false;
				// ...Nur spezielle Reise anzeigen (z.B. wenn Aufruf aus Reisekostenerfassung)
				if(isset($rk_detail_rzeitid) && $rk_detail_rzeitid)
				{
					if(($za['zeit_id'] == $rk_detail_rzeitid) || ($za['zeiteinheit'] == $rk_detail_rzeitid))
						$add=true;
				}
				// ...Unbekannt oder Filterprojekt
				else if(($unbekannt_anzeigen && !$za['projekt_id']) || ($za['projekt_id'] && isset($projekte[$za['projekt_id']])))
					$add=true;

				if($add)
					$zeitdaten[]=$za;

				// Ist-Stunden-Array
				$ttyp=(int)$za['zeitart'];
				if(($ttyp == FXP_TE_TIMEREC) || ($ttyp == FXP_TE_TIMEREC_SUM) || ($ttyp == FXP_TE_TRAVELEXP_TIMEREC))	// Zeiterfassungseintrag?
				{
					$ah=$za['zeitaufwand']/3600;
					if($ah > 0.0)
					{
						$tpid=(int)$za['personen_id'];
						$ttid=(int)$za['projekt_id'];
						$tdat=substr((string)$za['ist_beginn'],0,8);

						$ih=$ah;
						if(fxIsArray($projekte[$ttid]) && (($za['nicht_abrechenbar'] && ($projekte[$ttid]['pabr'] != FXP_INV_FIX)) || ($projekte[$ttid]['pabr'] == FXP_INV_NOT)))
							$ih=0.0;
						else
							$ih=$ah;

						if(!isset($ahp[$tpid]))
							$ahp[$tpid]=array();
						if(!isset($ahp[$tpid][$tdat]))
							$ahp[$tpid][$tdat]=array('ahours'=>$ah, 'ihours'=>$ih);
						else
						{
							$ahp[$tpid][$tdat]['ahours'] += $ah;
							$ahp[$tpid][$tdat]['ihours'] += $ih;
						}
					}
				}
			}
		}
//fxDebug($zeitdaten, '$zeitdaten');
fxDebug($ahp,'$ahp', 3);

		// Leistungsarten-Array
		if(fxIsArray($laufgaben))
		{
			$laa=array();
			if(fxIsArray($zeitdaten))
			{
				foreach($zeitdaten as $z)
				{
					if(isset($laufgaben[$z['projekt_id']]))
						$laufgaben[$z['projekt_id']]=true;

					if($z['kostenstelle_id'])
						$laa[$z['kostenstelle_id']]=true;
				}
			}
			foreach($laufgaben as $tid => $lstatus)
			{
				if(!$lstatus)
					unset($laufgaben[$tid]);
			}
//fxDebug($laufgaben, '$laufgaben');
			if(fxIsArray($laufgaben) || fxIsArray($laa))
			{
				if(fxIsArray($laa))
					$larten=getActivityTypes($laa);
				else
					$larten=getActivityTypes('');
			}
		}
//fxDebug($larten, '$larten');

		// Rechnungsnummern-Array
		$rnra=array();
		if(fxIsArray($zeitdaten))
		{
			foreach($zeitdaten as $za)
			{
				if($za['rechnungs_id'])
					$rnra[$za['rechnungs_id']]=true;
				if($za['rechnungs_id_rk'])
					$rnra[$za['rechnungs_id_rk']]=true;
			}
			if(sizeof($rnra))
			{
				$sql="SELECT rechnungs_id, rechnn_f, datum FROM rechnung WHERE rechnungs_id IN (".array_to_csv($rnra).") AND mandanten_id=".$GLOBALS['fxpglobals']['client']." ORDER BY rechnn_f, datum";
				$rta=db_values($sql,'*');
				$rnra=array();
				if(fxIsArray($rta))
				{
					foreach($rta as $ra)
						$rnra[$ra['rechnungs_id']]='<b>'.$ra['rechnn_f'].'</b> <i class=grey>('.get_entry($ra['datum'],'datum').')</i>';
				}
//fxDebug($rnra,$sql);
			}
		}

		// Feiertage ermitteln
		$hdays=get_feiertage($von, $bis);
//fxDebug($hdays, 'Feiertage $hdays');

		// Soll-Stunden und Zukunftsschätzung
		$festims=array();
		$tday=substr((string)$GLOBALS['datetime'],0,8);
		$aday=substr((string)$zvon,0,8);
		$eday=substr((string)$zbis,0,8);

		// Alle Personen ermitteln mit erweiterten Daten
		$GLOBALS['pil']=false;	// Keine aufgelaufenen Aufgaben
		$GLOBALS['mnt']=false;	// Keine Verwaltungsaufgaben
		$festims=getExtendedPersonInfos('', str_replace('z.', '', $prs_sql));
		$festims=getPersonSpecialDays($festims,$aday,$eday,true);

		// Alle aktiven und geplanten zugeordneten Aufgaben der Personen ermitteln
		$sql  = "SELECT m.personen_id, m.projekt_id, m.soll_beginn, m.soll_ende, m.aufwand_std, p.vorgangsnummer, p.name_projekt, p.projekt_status, b.abrechnungsart, a.ek_projekt, a.vk_projekt";
		$sql .= " FROM maplanung m, projekte p, budget_summe b, artikelzuordnung a";
//			$sql .= " WHERE p.projekt_id=m.projekt_id AND p.projekt_id IN (".$prj_sql.") AND p.projekt_status IN (".FXP_PS_ACTIVE.", ".FXP_PS_PLANNED.") AND p.mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$sql .= " WHERE p.projekt_id=m.projekt_id AND p.projekt_status IN (".FXP_PS_ACTIVE.", ".FXP_PS_PLANNED.") AND p.mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$sql .= " ".str_replace('z.', 'm.', $prs_sql)." AND m.soll_ende>='".$zvon."' AND m.soll_beginn<='".$zbis."' AND m.aufwand_std>0 AND m.mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$sql .= " AND b.projekt_id=m.projekt_id AND b.mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$sql .= " AND a.maplanung_id=m.maplanung_id AND a.artikeltyp=1094 AND a.mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$sql .= " ORDER BY p.vorgangsnummer";
		$tmp=db_values($sql,'*');
//fxDebug($tmp,$sql);
		if(fxIsArray($tmp))
		{
			foreach($tmp as $t)
			{
				$tpid=(int)$t['personen_id'];
				$ttid=(int)$t['projekt_id'];
				$festims[$tpid]['tasks'][$ttid]=array(
					'pno'=>$t['vorgangsnummer'], 'pname'=>$t['name_projekt'], 'status'=>$t['projekt_status'], 'invtype'=>$t['abrechnungsart'], 'pp'=>$t['ek_projekt'], 'sp'=>$t['vk_projekt'],
					'sdate'=>substr((string)$t['soll_beginn'],0,8), 'edate'=>substr((string)$t['soll_ende'],0,8), 'wdays'=>0, 'teffort'=>round($t['aufwand_std']/3600,$accuracy), 'deffort'=>0
				);
				$wdt=countWeekdays($festims[$tpid]['tasks'][$ttid]['sdate'],$festims[$tpid]['tasks'][$ttid]['edate'], $festims[$tpid]['su'], $festims[$tpid]['mo'], $festims[$tpid]['tu'], $festims[$tpid]['we'], $festims[$tpid]['th'], $festims[$tpid]['fr'], $festims[$tpid]['sa']);
				if($wdt > 0)
				{
					$festims[$tpid]['tasks'][$ttid]['wdays']=$wdt;
					$festims[$tpid]['tasks'][$ttid]['deffort']=round($festims[$tpid]['tasks'][$ttid]['teffort']/$wdt,$accuracy);
				}
			}
		}

		// Wochentage
		$ha=array('su','mo','tu','we','th','fr','sa');

		// Standard Tages-/Wochen-/Monats-/Jahres-Array holen und den einzelnen Personen zuordnen
		$dwmy=dwmyArray($aday,$eday);
//fxDebug($dwmy,'$dwmy');
		foreach($festims as $fpid => $farr)
		{
			// Schätzung ab heute, ausser heute wurde bereits Zeiterfassung eingetragen
			$cday=$tday;
			if(fxIsArray($ahp) && fxIsArray($ahp[$fpid]) && fxIsArray($ahp[$fpid][$cday]))
				$cday=substr((string)time_add($GLOBALS['datetime'],1,'D'),0,8);

			$festims[$fpid]['cday']=$cday;
			$festims[$fpid]['result']=array();
			foreach($dwmy['days'] as $adate => $aday)
			{
				// Im Arbeitszeitraum?
				if(($adate >= $farr['entrydate']) && ($adate <= $farr['leavedate']))
				{
					// ...Ja, dann prüfen ob Arbeitstag?
					$wh=0.0;
					if($farr[$ha[$aday]])
					{
						$wh=$farr['dailyhrs'];
						// ...evtl. Feiertag
						if(fxIsArray($hdays) && fxIsArray($hdays[$adate]))
							$wh=max(0.0, $wh-$wh*$hdays[$adate]['typ']);
						// ...evtl. Abwesenheitstag?
						if(sizeof($festims[$fpid]['sdays']) && isset($festims[$fpid]['sdays'][$adate]))
							$wh=max(0.0, $wh-$festims[$fpid]['sdays'][$adate]['dur']);
					}

					if($zm == 1)
						$rk=$adate;
					else if($zm == 2)
						$rk=substr((string)$adate,0,6);
					else
						$rk=substr((string)$adate,0,4);

					if(!fxIsArray($festims[$fpid]['result'][$rk]))
						$festims[$fpid]['result'][$rk]=array('day'=>$aday, 'eday'=>$aday, 'date'=>$adate, 'edate'=>$adate, 'tasks'=>array(), 'whours'=>0.0, 'ehours'=>0.0, 'mhours'=>0.0, 'ahours'=>0.0, 'uhours'=>0.0, 'ihours'=>0.0, 'weffort'=>0.0, 'ieffort'=>0.0, 'spp'=>0.0, 'ssp'=>0.0);
					else
					{
						$festims[$fpid]['result'][$rk]['eday']=$aday;
						$festims[$fpid]['result'][$rk]['edate']=$adate;
					}

					// ...Ja, dann Durchschnitts-EK/VK der zugeordneten Aufgaben für diesen Tag ermitteln
					if($wh)
					{
						$tph=0.0;
						$tih=0.0;
						$tas=false;
						if(($adate >= $cday) && sizeof($farr['tasks']))
						{
							foreach($farr['tasks'] as $tid => $tarr)
							{
								if(($adate >= $tarr['sdate']) && ($adate <= $tarr['edate']))	// Aufgabe liegt im Zeitraum
								{
									$tas=true;
									if(fxIsArray($projekte[$tid]))
									{
										$tinv=true;
										if($projekte[$tid]['pabr'] == FXP_INV_NOT)
											$tinv=false;

										if(!fxIsArray($festims[$fpid]['result'][$rk]['tasks'][$tid]))
										{
											$festims[$fpid]['result'][$rk]['tasks'][$tid]=array('pno'=>$tarr['pno'], 'pname'=>$tarr['pname'], 'status'=>$tarr['status'], 'phours'=>0.0, 'ihours'=>0.0, 'pp'=>$tarr['pp'], 'sp'=>$tarr['sp']);
											if(!$tinv)
												$festims[$fpid]['result'][$rk]['tasks'][$tid]['sp']=0.0;
										}
										$tph += $tarr['deffort'];
										$festims[$fpid]['result'][$rk]['tasks'][$tid]['phours'] += $tarr['deffort'];
										$festims[$fpid]['result'][$rk]['weffort'] += $tarr['deffort'];
										$festims[$fpid]['result'][$rk]['spp'] += ($tarr['deffort']*$tarr['pp']);
										if($tinv)
										{
											$tih += $tarr['deffort'];
											$festims[$fpid]['result'][$rk]['tasks'][$tid]['ihours'] += $tarr['deffort'];
											$festims[$fpid]['result'][$rk]['ieffort'] += $tarr['deffort'];
											$festims[$fpid]['result'][$rk]['ssp'] += ($tarr['deffort']*$tarr['sp']);
										}
									}
								}
							}
						}

						// Sollstunden
						$festims[$fpid]['result'][$rk]['whours'] += $wh;

						// Schätz-, Fehl- und Leerstunden
						$eh=0.0;
						$mh=0.0;
						$ah=0.0;
						$uh=0.0;
						if($adate < $cday)
						{
							if($fehlzeiten)
								$mh=$wh;
						}
						else if(!$tas)
						{
							if($leerzuordnung)
								$uh=$wh;
						}
						else if(!$tph)
							$ah=$wh;
						else
						{
							if($schaetzung)
								$eh=$wh;
						}
//echo('$adate='.$adate.' -- $wh='.$wh.', $tph='.$tph.', $tih='.$tih.' -- '.(($eh > 0.0)?'<b>':'').'$eh='.$eh.''.(($eh > 0.0)?'</b>':'').', '.(($mh > 0.0)?'<b>':'').'$mh='.$mh.''.(($mh > 0.0)?'</b>':'').', '.(($ah > 0.0)?'<b>':'').'$ah='.$ah.''.(($ah > 0.0)?'</b>':'').', '.(($uh > 0.0)?'<b>':'').'$uh='.$uh.''.(($uh > 0.0)?'</b>':'').'<br />');

						// Bereits erbrachte Zeiterfassung abziehen
						if(fxIsArray($ahp) && fxIsArray($ahp[$fpid]) && fxIsArray($ahp[$fpid][$adate]))
						{
							$eh=max(0.0, $eh-$ahp[$fpid][$adate]['ahours']);
							$mh=max(0.0, $mh-$ahp[$fpid][$adate]['ahours']);
							$ah=max(0.0, $ah-$ahp[$fpid][$adate]['ahours']);
							$uh=max(0.0, $uh-$ahp[$fpid][$adate]['ahours']);
						}
						$festims[$fpid]['result'][$rk]['ehours'] += $eh;
						$festims[$fpid]['result'][$rk]['mhours'] += $mh;
						$festims[$fpid]['result'][$rk]['ahours'] += $ah;
						$festims[$fpid]['result'][$rk]['uhours'] += $uh;

						// Geschätzte Stunden < Arbeitsstunden
						$wt=min($wh,$tph);
						if($eh && ($eh < $wt))	// && ($tph < $wh))
						{
//echo('<font class=blue>--&gt; $adate='.$adate.' -- $wt='.$wt.' (min of $wh='.$wh.' and $tph='.$tph.') &gt; $eh='.$eh.' ($uh='.$uh.') -- ');
							$dh=$wt-$eh;
							$eh -= $dh;
							$uh += $dh;
//echo('$dh='.$dh.' -- $eh='.$eh.', $uh='.$uh.'</font><br />');
							$festims[$fpid]['result'][$rk]['ehours'] -= $dh;
							$festims[$fpid]['result'][$rk]['uhours'] += $dh;
						}

						// Verrechenbare geschätzte Stunden
						if($eh && $tih)
						{
							if($_POST['MaxStdTag'])
								$tih=max($tih,$eh);
							else
								$tih=min($tih,$eh);
							$festims[$fpid]['result'][$rk]['ihours'] += $tih;
						}

						// Aufwand
						if(!$tph)
						{
							$festims[$fpid]['result'][$rk]['weffort'] += ($eh+$mh+$uh);
							$festims[$fpid]['result'][$rk]['spp'] += (($eh+$mh+$uh)*$festims[$fpid]['pp']);
						}
					}
				}
			}
		}
//fxDebug($festims,'$festims');

		// Maskentitel
		$mld_id=1423;
		if($Reisekosten == 2139)
			$mld_id=1509;
		if($prs_id > 0)
		{
			$var1=trim((string)strip_tags(str_ireplace('&nbsp;', ' ', $prs_array[$prs_id])));
			$ekp=strpos($var1, ' [');
			if($ekp !== false)
				$var1=trim(substr((string)$var1, 0, $ekp));
		}
		else if(!$prs_id)
			$var1=meldung(1410, false);
		else if($Reisekosten == 2139)
			$mld_id=1511;
		else
			$mld_id=1510;

		if(($mld_id == 1423) || ($mld_id == 1509))
			$maske231_head=meldung($mld_id, false, '&quot;'.$var1.'&quot;', get_entry($von, 'datum'), get_entry($bis, 'datum'), (1+$anztage));
		else
			$maske231_head=meldung($mld_id, false, get_entry($von, 'datum'), get_entry($bis, 'datum'), (1+$anztage));

		// Kalendernavigation?
		$ndate='';
		$nmode='';
		if($GLOBALS['fxpglobals']['tr'] == 150)	// ...in Reisekostenerfassung
		{
			$ndate=substr((string)$von,0,8);
			if($gw)
				$nmode='week';
			else if($gm)
				$nmode='month';
			else if($gj)
				$nmode='year';
		}

		// Navigation
		if(strlen((string)$ndate) && strlen((string)$nmode))
		{
			$ay=(int)substr((string)$ndate,0,4);
			$am=(int)substr((string)$ndate,4,2);
			$ad=(int)substr((string)$ndate,6,2);
			$aw=kw($ndate);

			if($nmode == 'week')
				$maske231_head=addNavigation($ay,$am,0,0,'left').$maske231_head.addNavigation($ay,$am,$ad);
			else if($nmode == 'month')
				$maske231_head=addNavigation($ay,0,0,0,'left').$maske231_head.addNavigation($ay,$am);
			else if($nmode == 'year')
				$maske231_head .= addNavigation($ay);
		}

		// Variablen
		$mc=$GLOBALS['_maskcounter'];
		$z=0;
		$mca=array('Tage', 'Datum', 'Zeit_ID', 'Projekt', 'leistungsarten', 'SOLLStunden', 'ISTStunden', 'Delta', 'EK', 'Kosten', 'MwSt', 'Bruttobetrag', 'Ver_Std', 'VK', 'ertrag', 'Marge_verbr', 'Freigabe', 'verrechenbar', 'abgerechnet', 'link');
		$mo_tage=array(1 => 31, 28, 31, 30,  31,  30,  31,  31,  30,  31,  30,  31);
		$fart=array(FXP_HL_FULL=>' [1]', FXP_HL_HALF=>' [½]', FXP_HL_DISPLAY=>' [0]');
		$rk_kategorien=rk_kategorie_array(false);
		$rk_arten=fxf_getReferenceValues(1884);
		$rk_laender=array();
		$usk=fxf_getReferenceValues(434);
		$monate=fxf_getReferenceValues('monate');
		$wochentage=fxf_getReferenceValues('wochentage');
		foreach($wochentage as $wd => $wn)
			$wochentage[$wd]=substr((string)$wn, 0, 2);

		// Evtl. selbstdefinierten Begriff statt "Tagesangaben" (ID=1980) verwenden
		if(isset($GLOBALS['fxpglobals']['settings']['txt_tangaben']) && ($GLOBALS['fxpglobals']['settings']['txt_tangaben'] !== NULL) && strlen((string)$GLOBALS['fxpglobals']['settings']['txt_tangaben']))
			$rk_kategorien[1980]=trim((string)$GLOBALS['fxpglobals']['settings']['txt_tangaben']);

		// Images
		// ...Frei. (Freigabe)
		$img_frei=array(
			0 => '<img src="'.$GLOBALS['gfxpath'].'st_red.png" tooltip="{tt}" style="margin:4px 0 0 0;cursor:help;">',
			1 => '<img src="'.$GLOBALS['gfxpath'].'st_green.png" tooltip="{tt}" style="margin:4px 0 0 0;cursor:help;">',
			2 => '<img src="'.$GLOBALS['gfxpath'].'st_mix.png" tooltip="{tt}" style="margin:4px 0 0 0;cursor:help;">'
		);
		// ...Verr. (Verrechenbar)
		$img_verr=array(
			0 => '<img src="'.$GLOBALS['gfxpath'].'st_green.png" tooltip="{tt}" style="margin:4px 0 0 0;cursor:help;">',
			1 => '<img src="'.$GLOBALS['gfxpath'].'st_red.png" tooltip="{tt}" style="margin:4px 0 0 0;cursor:help;">',
			2 => '<img src="'.$GLOBALS['gfxpath'].'st_lightgreen.png" tooltip="{tt}" style="margin:4px 0 0 0;cursor:help;">',
			3 => '<img src="'.$GLOBALS['gfxpath'].'st_lightred.png" tooltip="{tt}" style="margin:4px 0 0 0;cursor:help;">',
			4 => '<img src="'.$GLOBALS['gfxpath'].'st_mix.png" tooltip="{tt}" style="margin:4px 0 0 0;cursor:help;">'
		);
		// ...Abg. (Abgerechnet)
		$img_abger=array(
			0 => '<img src="'.$GLOBALS['gfxpath'].'st_red.png" tooltip="{tt}" style="margin:4px 0 0 0;cursor:help;">',
			1 => '<img src="'.$GLOBALS['gfxpath'].'st_lightred.png" tooltip="{tt}" style="margin:4px 0 0 0;cursor:help;">',
			2 => '<img src="'.$GLOBALS['gfxpath'].'st_green.png" tooltip="{tt}" style="margin:4px 0 0 0;cursor:help;">',
			3 => '<img src="'.$GLOBALS['gfxpath'].'st_mix.png" tooltip="{tt}" style="margin:4px 0 0 0;cursor:help;">'
		);

		// Angezeigte Personen
		$dpc=0;

		// Gesamtsumme
		$total_sstd=0;
		$total_istd=0;
		$total_vstd=0;
		$total_gek=0.0;
		$total_gvk=0.0;
		$total_mwst=0.0;
		$total_brutto=0.0;
		$total_marge=0.0;

		// Alle Personen durchgehen
		foreach($personen as $pers)
		{
			$pers_id=(int)$pers['personen_id'];
			$entries=0;
			$head=false;
			$pz=$z;

			$pzsa=array();

			// Personenmonats-, Personenjahres- und Personengesamtsumme
			$pers_mon_sstd=0;
			$pers_mon_istd=0;
			$pers_mon_vstd=0;
			$pers_mon_gek=0.0;
			$pers_mon_gvk=0.0;
			$pers_mon_mwst=0.0;
			$pers_mon_brutto=0.0;
			$pers_mon_marge=0.0;

			$pers_ges_sstd=0;
			$pers_ges_istd=0;
			$pers_ges_vstd=0;
			$pers_ges_gek=0.0;
			$pers_ges_gvk=0.0;
			$pers_ges_mwst=0.0;
			$pers_ges_brutto=0.0;
			$pers_ges_marge=0.0;

			if($pers_id)
			{
				$head=true;

				// Leerzeile?
				$leerzeile=$z;
				if($z)
				{
					$GLOBALS['msk_nbcol_231_'.$z]=$GLOBALS['msk_sbcol'];
					fxf_maskColumnBorder($mca, 1, -1, $mc, $z, true);
					$z++;
				}

				// Name (über gesamte Zeile)
				$name=trim((string)$pers['vorname']);
				if(strlen((string)$name))
					$name=', '.$name;
				$name=preg_replace(array('/<tt>(.*)<\/tt>/is', '/<img(.*) \/>/is'), array('', ''), trim((string)$pers['pname']).$name);
				$fc='black';
				if(substr((string)$name,0,5) == '<red>')
				{
					$fc='darkgrey italic';
					$name=substr((string)$name,5);
				}
				$opp=strpos($name,'(');
				if($opp)
					$name=trim(substr((string)$name,0,$opp));

				$txt='&nbsp;'.person_pic($pers_id, 20, $pers['geschlecht'], $GLOBALS['fxpglobals']['client'], false, 'border-top-left-radius:6px;').'&nbsp;<font class="s4b '.$fc.' tshadow">'.$name.'</font>';
				$lshc='';
				if($pers_id == $GLOBALS['fxpglobals']['person'])
					$lshc='b';
				fxf_listSectHeader($mc, $z, $txt, $fcol, $tlcol, $colspan, $lshc);
				$z++;
			}

			// Starttag aufsplitten
			$atag=substr((string)$von,0,8);
			$btag=$atag;
			$etag=substr((string)$bis,0,8);
			$y=(int)substr((string)$atag,0,4);
			$m=(int)substr((string)$atag,4,2);
			$d=(int)substr((string)$atag,6,2);
			$lj=fx_date('L', fx_mktime(0, 0, 0, 1, 1, $y));
			if($lj)
				$mo_tage[2]=29;

			$ac=0;
			$ae=false;

			// Startwochentag ermitteln
			$fd=$GLOBALS['fxpglobals']['settings']['ersterwt'];
			$wd=fx_date('w', fx_mktime(0, 0, 0, $m, $d, $y));
			if($wd < 0)
				$wd += 7;

			$aw=0;
			$cw=0;

			$usktage='';
			if(($zm == 1) && $pers_id && !$sort_travel)	// Tage
			{
				// Sondertage (Urlaub/Schulung/Krank) ermitteln
				$usktage=get_sondertage($von, $bis, $pers_id);
//fxDebug($usktage, 'SONDERTAGE');
			}

			$sb=array();
			$rk=array();
			$rkrem=array();

			// Alle Tage durchgehen
			while($atag <= $etag)
			{
				$dd='';
				$ec=0;
				$sh=0.0;
				$da=array();

				if($zm == 2)
				{
					$stag=max($btag, substr('0000'.$y, -4).substr('00'.$m, -2).'01');
					$ltag=min($etag, substr('0000'.$y, -4).substr('00'.$m, -2).substr('00'.$mo_tage[$m], -2));
					$fkey=substr((string)$ltag,0,6);
				}
				else if($zm == 3)
				{
					$stag=max($btag, substr('0000'.$y, -4).'0101');
					$ltag=min($etag, substr('0000'.$y, -4).'1231');
					$fkey=substr((string)$ltag,0,4);
				}
				else
				{
					$stag=$atag;
					$ltag=$atag;
					$fkey=$ltag;
				}
//echo('$atag=<b>'.$atag.'</b>, $stag='.$stag.', $etag='.$etag.', $ltag='.$ltag.' -- $y='.$y.', $m='.$m.', $d='.$d.'<hr />');

				// Alle Zeitdaten durchgehen
				if(fxIsArray($zeitdaten))
				{
					foreach($zeitdaten as $zk => $zeit)
					{
						$zid=(int)$zeit['zeit_id'];
						$zpers_id=(int)$zeit['personen_id'];
						$zproj_id=(int)$zeit['projekt_id'];
						if(($zeit['ist_beginn'] !== NULL) && strlen((string)$zeit['ist_beginn']))
							$zvon=substr(trim((string)$zeit['ist_beginn']), 0,8);
						else
							$zvon='00000000';
						if(($zeit['ist_ende'] !== NULL) && strlen((string)$zeit['ist_ende']))
							$zbis=substr(trim((string)$zeit['ist_ende']), 0,8);
						else
							$zbis='99999999';
						$zeinheit=(int)$zeit['zeiteinheit'];
						$zabr=$projekte[$zproj_id]['pabr'];

						if($zeit['zeitart'] == FXP_TE_TIMEREC_SUM)
							$zeinheit=0;

						if(!strlen((string)$zbis))
							$zbis=$zvon;

						if($pers_id && ($zpers_id != $pers_id))
							continue;
						if(!strlen((string)$zvon) || ($zvon > $atag) || ($zbis < $atag) || $zeinheit)
							continue;

						$zkp=$zproj_id;
						if(fxIsArray($laufgaben) && fxIsArray($larten))
						{
							$lid=(int)$zeit['kostenstelle_id'];
							if($lid > 0)
								$zkp .= '|'.$lid;
							else if(isset($laufgaben[$zproj_id]))
								$zkp .= '|0';
							else
								$zkp .= '|-1';
						}

						if(!strlen((string)$zeit['mwst_satz']) || !$zeit['mwst_satz'])
							$mwst=get_double($GLOBALS['fxpglobals']['settings']['mwst_satz']);
						else
							$mwst=get_double($zeit['mwst_satz']);

						$valid=true;
						if(!$sort_travel && ($zvon != $atag))
							$valid=false;

						if($sort_travel)
							$zkp .= '#'.$zk;

						// Freigabe
						$ifrei=0;
						if($zeit['freigabe'])
							$ifrei=1;
						$frei=false;
						if(($Freigabe == 2871) || (($Freigabe == 2872) && $ifrei) || (($Freigabe == 2873) && !$ifrei))
							$frei=true;

						// Zeitdaten verrechenbar
						// ..."verrechenbar", falls Buchung auf Aufgabe mit Abrechnungsart "Festpreis"
						if($zabr == FXP_INV_FIX)
							$iznverr=2;
						// ..."nicht verrechenbar", falls Buchung auf Aufgabe mit Abrechnungsart "Nicht verrechenbar"
						else if($zabr == FXP_INV_NOT)
							$iznverr=3;
						// ..."nicht verrechenbar", falls in Zeiterfassungseintrag angegeben
						else if($zeit['nicht_abrechenbar'])
							$iznverr=1;
						// ..."verrechenbar"
						else
							$iznverr=0;
						$zverr=false;
						if(($verrechenbar == 2874) || (($verrechenbar == 2875) && !($iznverr%2)) || (($verrechenbar == 2876) && ($iznverr%2)))
							$zverr=true;

						// Reisekosten verrechenbar
						$irnverr=0;
						if($zeit['rk_nicht_abrechenbar'])
							$irnverr=1;
						$rverr=false;
						if(($verrechenbar == 2874) || (($verrechenbar == 2875) && !$irnverr) || (($verrechenbar == 2876) && $irnverr))
							$rverr=true;

						// Zeitdaten abgerechnet
						// ..."abgerechnet"
						if($zeit['rechnungs_id'])
							$izabger=2;
						// ..."nicht abgerechnet", aber verrechenbar
						else if(!$iznverr || ($iznverr == 2))
							$izabger=1;
						// ..."nicht abgerechnet", und auch nicht verrechenbar
						else
							$izabger=0;
						$zabger=false;
						if(($abgerechnet == 3234) || (($abgerechnet == 3235) && ($izabger == 2)) || (($abgerechnet == 3236) && ($izabger < 2)))
							$zabger=true;

						// Reisekosten abgerechnet
						// ..."abgerechnet"
						if($zeit['rechnungs_id_rk'])
							$irabger=2;
						// ..."nicht abgerechnet", aber verrechenbar
						else if(!$irnverr || ($irnverr == 2))
							$irabger=1;
						// ..."nicht abgerechnet", und auch nicht verrechenbar
						else
							$irabger=0;
						$rabger=false;
						if(($abgerechnet == 3234) || (($abgerechnet == 3235) && ($irabger == 2)) || (($abgerechnet == 3236) && ($irabger < 2)))
							$rabger=true;

						// Stundenkosten
						if(($zeit['zeitart'] != FXP_TE_TRAVELEXP) && ($Reisekosten != 2139))
						{
							// Nur falls "Freigabe" + "Verrechenbar" + "Abgerechnet" übereinstimmt
							if($frei && $zverr && $zabger)
							{
								if(!fxIsArray($sb[$zkp]))
									$sb[$zkp]=array('art'=>$zeit['zeitart'], 'dat'=>$zeit['ist_beginn'], 'end'=>$zeit['ist_ende'], 'tid'=>(int)$zeit['taetigkeit_id'], 'rid'=>array(), 'freigabe'=>$ifrei, 'std'=>0, 'vstd'=>0, 'ek'=>array(), 'vk'=>array(), 'gek'=>0.0, 'gvk'=>0.0, 'mwst'=>0.0, 'brutto'=>0.0, 'n_verr'=>$iznverr, 'abger'=>$izabger);
								if($izabger == 2)
								{
									$rid=(int)$zeit['rechnungs_id'];
									if(!isset($sb[$zkp]['rid'][$rid]))
										$sb[$zkp]['rid'][$rid]=0;
									$sb[$zkp]['rid'][$rid]++;
								}

								if($valid)
								{
									$ek=round($zeit['kosten_ek'], $GLOBALS['fxpglobals']['settings']['nkstellen']);
									if($iznverr%2 == 1)
										$vk=0.0;
									else
										$vk=round($zeit['kosten_vk'], $GLOBALS['fxpglobals']['settings']['nkstellen']);
									$mw=round($ek*$mwst/100.0, $GLOBALS['fxpglobals']['settings']['nkstellen']);
									$bk=round($ek+$mw, $GLOBALS['fxpglobals']['settings']['nkstellen']);

									$sb[$zkp]['std']	+= $zeit['zeitaufwand'];
									$sb[$zkp]['gek']	+= $ek;
									$sb[$zkp]['gvk']	+= $vk;
									$sb[$zkp]['mwst']	+= $mw;
									$sb[$zkp]['brutto']	+= $bk;

									$aek=0.0;
									$avk=0.0;
									if($zeit['zeitaufwand'])
									{
										$aek=round($zeit['kosten_ek']*3600/$zeit['zeitaufwand'], $GLOBALS['fxpglobals']['settings']['nkstellen']);
										$avk=round($zeit['kosten_vk']*3600/$zeit['zeitaufwand'], $GLOBALS['fxpglobals']['settings']['nkstellen']);
									}
									$sb[$zkp]['ek'][$aek]=true;
									$sb[$zkp]['vk'][$avk]=true;

									if(!$iznverr || ($iznverr == 2))
										$sb[$zkp]['vstd'] += $zeit['zeitaufwand'];

									// Falls unterschiedliche Freigabe, dann 2=Mix
									if($sb[$zkp]['freigabe'] != $ifrei)
										$sb[$zkp]['freigabe']=2;
									// Falls unterschiedlich verrechenbar, dann 4=Mix
									if($sb[$zkp]['n_verr'] != $iznverr)
										$sb[$zkp]['n_verr']=4;
									// Falls unterschiedlich abgerechnet, dann 3=Mix
									if($sb[$zkp]['abger'] != $izabger)
										$sb[$zkp]['abger']=3;
								}
							}
						}
						// Reisekosten
						if(($zeit['zeitart'] != FXP_TE_TIMEREC) && ($zeit['zeitart'] != FXP_TE_TIMEREC_SUM) && ($Reisekosten != 2140))
						{
							if(!sizeof($rk) || !fxIsArray($rk[$zkp]) || !fxIsArray($rk[$zkp][$zk]))
							{
								$rnr='';
								if($zeit['belegnr'] !== NULL)
									$rnr=trim((string)$zeit['belegnr']);
								if(strlen((string)$rnr) && !$sort_travel)
									$rnr='&nbsp;&quot;'.$rnr.'&quot;';

								if($valid)
								{
									$ek=round($zeit['rk_ek'], $GLOBALS['fxpglobals']['settings']['nkstellen']);
									$vk=round($zeit['rk_vk'], $GLOBALS['fxpglobals']['settings']['nkstellen']);
									if($zeit['rk'])
									{
										$bk=round($zeit['rk'], $GLOBALS['fxpglobals']['settings']['nkstellen']);
										$mw=round($bk-$ek, $GLOBALS['fxpglobals']['settings']['nkstellen']);
									}
									else
									{
										$mw=round($ek*$mwst/100.0, $GLOBALS['fxpglobals']['settings']['nkstellen']);
										$bk=round($ek+$mw, $GLOBALS['fxpglobals']['settings']['nkstellen']);
									}
								}
								else
								{
									$ek=0.0;
									$vk=0.0;
									$mw=0.0;
									$bk=0.0;
								}

								// Nur falls "Freigabe" + "Verrechenbar" + "Abgerechnet" übereinstimmt
								if($frei && $rverr && $rabger)
								{
									if(!fxIsArray($rk[$zkp][$zk]))
										$rk[$zkp][$zk]=array('id'=>$zid, 'dat'=>$zeit['ist_beginn'], 'end'=>$zeit['ist_ende'], 'tid'=>(int)$zeit['taetigkeit_id'], 'rid'=>array(), 'freigabe'=>$ifrei, 'gek'=>0.0, 'gvk'=>0.0, 'mwst'=>0.0, 'brutto'=>0.0, 'rk_art'=>$zeit['rk_art'], 'rk_land'=>$zeit['land_id'], 'rk_fahrzeug'=>$zeit['rk_kategorie'], 'rk_start'=>$zeit['ist_beginn'], 'rk_ende'=>$zeit['ist_ende'], 'rk_ort1'=>$zeit['rk_ort1'], 'rk_ort2'=>$zeit['rk_ort2'], 'rk_mitfahrer'=>(int)$zeit['rk_mitfahrer'], 'rk_km'=>(int)$zeit['rk_km'], 'reisenr'=>$rnr, 'n_verr'=>$irnverr, 'abger'=>$irabger, 'tagesangaben'=>array(), 'einzelbelege'=>array());
									if($irabger == 2)
									{
										$rid=(int)$zeit['rechnungs_id_rk'];
										if(!isset($rk[$zkp][$zk]['rid'][$rid]))
											$rk[$zkp][$zk]['rid'][$rid]=0;
										$rk[$zkp][$zk]['rid'][$rid]++;
									}

									$rk[$zkp][$zk]['gek']		+= $ek;
									$rk[$zkp][$zk]['gvk']		+= $vk;
									$rk[$zkp][$zk]['mwst']		+= $mw;
									$rk[$zkp][$zk]['brutto']	+= $bk;

									// Falls unterschiedliche Freigabe, dann 2=Mix
									if($rk[$zkp][$zk]['freigabe'] != $ifrei)
										$rk[$zkp][$zk]['freigabe']=2;
									// Falls unterschiedlich verrechenbar, dann 4=Mix
									if($rk[$zkp][$zk]['n_verr'] != $irnverr)
										$rk[$zkp][$zk]['n_verr']=4;
									// Falls unterschiedlich abgerechnet, dann 3=Mix
									if($rk[$zkp][$zk]['abger'] != $irabger)
										$rk[$zkp][$zk]['abger']=3;

									if($Reisekosten == 2138)
									{
										if(!fxIsArray($sb[$zkp]))
											$sb[$zkp]=array('art'=>$zeit['zeitart'], 'dat'=>$zeit['ist_beginn'], 'end'=>$zeit['ist_ende'], 'tid'=>(int)$zeit['taetigkeit_id'], 'rid'=>array(), 'freigabe'=>$ifrei, 'std'=>0, 'vstd'=>0, 'ek'=>array(), 'vk'=>array(), 'gek'=>0.0, 'gvk'=>0.0, 'mwst'=>0.0, 'brutto'=>0.0, 'n_verr'=>$iznverr, 'abger'=>$izabger);
										if($izabger == 2)
										{
											$rid=(int)$zeit['rechnungs_id'];
											if(!isset($sb[$zkp]['rid'][$rid]))
												$sb[$zkp]['rid'][$rid]=0;
											$sb[$zkp]['rid'][$rid]++;
										}

										$sb[$zkp]['gek']	+= $ek;
										$sb[$zkp]['gvk']	+= $vk;
										$sb[$zkp]['mwst']	+= $mw;
										$sb[$zkp]['brutto']	+= $bk;

										$aek=0.0;
										$avk=0.0;
										if($zeit['zeitaufwand'])
										{
											$aek=round($zeit['kosten_ek']*3600/$zeit['zeitaufwand'], $GLOBALS['fxpglobals']['settings']['nkstellen']);
											$avk=round($zeit['kosten_vk']*3600/$zeit['zeitaufwand'], $GLOBALS['fxpglobals']['settings']['nkstellen']);
										}
										$sb[$zkp]['ek'][$aek]=true;
										$sb[$zkp]['vk'][$avk]=true;

										// Falls unterschiedliche Freigabe, dann 2=Mix
										if($sb[$zkp]['freigabe'] != $ifrei)
											$sb[$zkp]['freigabe']=2;
										// Falls unterschiedlich verrechenbar, dann 4=Mix
										if($sb[$zkp]['n_verr'] != $irnverr)
											$sb[$zkp]['n_verr']=4;
										// Falls unterschiedlich abgerechnet, dann 3=Mix
										if($sb[$zkp]['abger'] != $izabger)
											$sb[$zkp]['abger']=3;
									}
								}

								// ...Tagesangaben/Einzelbelege
								foreach($zeitdaten as $zi => $bzeit)
								{
									$all_done=false;
									if(($bzeit['ist_beginn'] !== NULL) && strlen((string)$bzeit['ist_beginn']))
										$bstart=substr((string)$bzeit['ist_beginn'], 0,8);
									else
										$bstart='00000000';
									if(($bzeit['ist_ende'] === NULL) || !strlen((string)$bzeit['ist_ende']))
									{
										$bende=$ltag;
										$all_done=true;
									}
									else
									{
										$bende=substr((string)$bzeit['ist_ende'],0,8);
										if($bende <= $ltag)
											$all_done=true;
									}
									// ...Tagesangabe
									if($bzeit['rk_kategorie'] == 1980)
										$bzk='TA_'.$bstart.'_'.$bende;
									// ...Einzelbeleg
									else
										$bzk='BN_'.$bzeit['belegnr'].'_'.$bstart;

									$bzid=(int)$bzeit['zeiteinheit'];
									if(($zid != $bzid) || (!$sort_travel && (($bende < $atag) || ($bstart > $ltag))))
										continue;
//fxDebug($bzeit,'$bzeit: '.$bzk);

									if(!strlen((string)$zeit['mwst_satz']) || !$zeit['mwst_satz'])
										$mwst=get_double($GLOBALS['fxpglobals']['settings']['mwst_satz']);
									else
										$mwst=get_double($bzeit['mwst_satz']);

									$ek=round($bzeit['rk_ek'], $GLOBALS['fxpglobals']['settings']['nkstellen']);
									$vk=round($bzeit['rk_vk'], $GLOBALS['fxpglobals']['settings']['nkstellen']);
									if($bzeit['rk'])
									{
										$bk=round($bzeit['rk'], $GLOBALS['fxpglobals']['settings']['nkstellen']);
										$mw=round($bk-$ek, $GLOBALS['fxpglobals']['settings']['nkstellen']);
									}
									else
									{
										$mw=round($ek*$mwst/100.0, $GLOBALS['fxpglobals']['settings']['nkstellen']);
										$bk=round($ek+$mw, $GLOBALS['fxpglobals']['settings']['nkstellen']);
									}

									// Freigabe
									$bifrei=0;
									if($bzeit['freigabe'])
										$bifrei=1;
									$bfrei=false;
									if(($Freigabe == 2871) || (($Freigabe == 2872) && $bifrei) || (($Freigabe == 2873) && !$bifrei))
										$bfrei=true;

									// Tagesangaben/Einzelbelege verrechenbar
									$birnverr=0;
									if($bzeit['rk_nicht_abrechenbar'])
										$birnverr=1;
									$brverr=false;
									if(($verrechenbar == 2874) || (($verrechenbar == 2875) && !$birnverr) || (($verrechenbar == 2876) && $birnverr))
										$brverr=true;

									// Tagesangaben/Einzelbelege abgerechnet
									// ..."abgerechnet"
									if($bzeit['rechnungs_id_rk'])
										$birabger=2;
									// ..."nicht abgerechnet", aber verrechenbar
									else if(!$birnverr || ($birnverr == 2))
										$birabger=1;
									// ..."nicht abgerechnet", und auch nicht verrechenbar
									else
										$birabger=0;
									$brabger=false;
									if(($abgerechnet == 3234) || (($abgerechnet == 3235) && ($birabger == 2)) || (($abgerechnet == 3236) && ($birabger < 2)))
										$brabger=true;

									// ...Tagesangaben
									if(($bzeit['rk_kategorie'] == 1980) && (!fxIsArray($rk[$zkp][$zk]['tagesangaben']) || !fxIsArray($rk[$zkp][$zk]['tagesangaben'][$bzk])))
									{
										// Nur falls "Freigabe" + "Verrechenbar" + "Abgerechnet" übereinstimmt, sowie Tagesangaben angezeigt werden sollen
										$rid=0;
										if($bfrei && $brverr && $brabger && ($tagesangabe == 3243))
										{
											if(!fxIsArray($rk[$zkp][$zk]))
												$rk[$zkp][$zk]=array('id'=>$zid, 'dat'=>$zeit['ist_beginn'], 'end'=>$zeit['ist_ende'], 'tid'=>(int)$zeit['taetigkeit_id'], 'rid'=>array(), 'freigabe'=>$bifrei, 'gek'=>0.0, 'gvk'=>0.0, 'mwst'=>0.0, 'brutto'=>0.0, 'rk_art'=>$zeit['rk_art'], 'rk_land'=>$zeit['land_id'], 'rk_fahrzeug'=>$zeit['rk_kategorie'], 'rk_start'=>$zeit['ist_beginn'], 'rk_ende'=>$zeit['ist_ende'], 'rk_ort1'=>$zeit['rk_ort1'], 'rk_ort2'=>$zeit['rk_ort2'], 'rk_mitfahrer'=>(int)$zeit['rk_mitfahrer'], 'rk_km'=>(int)$zeit['rk_km'], 'reisenr'=>$rnr, 'n_verr'=>$birnverr, 'abger'=>$birabger, 'tagesangaben'=>array(), 'einzelbelege'=>array());
											$ria=array();
											if($birabger == 2)
											{
												$rid=(int)$bzeit['rechnungs_id_rk'];
												$ria[$rid]=1;
											}

											// Falls unterschiedliche Freigabe, dann 2=Mix
											if(($Freigabe != 2871) && ($rk[$zkp][$zk]['freigabe'] != $bifrei))
												$rk[$zkp][$zk]['freigabe']=2;
											// Falls unterschiedlich verrechenbar, dann 4=Mix
											if(($verrechenbar != 2874) && ($rk[$zkp][$zk]['n_verr'] != $birnverr))
												$rk[$zkp][$zk]['n_verr']=4;
											// Falls unterschiedlich abgerechnet, dann 3=Mix
											if(($abgerechnet != 3234) && ($rk[$zkp][$zk]['abger'] != $birabger))
												$rk[$zkp][$zk]['abger']=3;

											// Datum
											$dat=get_entry($bzeit['ist_beginn'], 'datum');
											if(strlen((string)$bzeit['ist_ende']))
											{
												$de=get_entry($bzeit['ist_ende'], 'datum');
												if($de != $dat)
													$dat .= ' - '.$de;
											}

											$sql="SELECT datum, pverpflegung, pnacht, pfrueh, pmittag, pabend FROM rktagesangaben WHERE zeit_id=".$bzeit['zeit_id']." AND datum>='".$stag."000000' AND datum<='".$ltag."240000' AND mandanten_id=".$GLOBALS['fxpglobals']['client']." ORDER BY datum";
											$dta=db_values($sql,'*');
//fxDebug($dta, $sql);
											if(fxIsArray($dta))
											{
												foreach($dta as $det)
												{
													$pv=0;
													$pvb=0.0;
													if($det['pverpflegung'])
													{
														$pv=1;
														$pvb=$det['pverpflegung'];
													}
													$pn=0;
													$pnb=0.0;
													if($det['pnacht'])
													{
														$pn=1;
														$pnb=$det['pnacht'];
													}
													$pmb=0.0;
													if($det['pfrueh'])
														$pmb += $det['pfrueh'];
													if($det['pmittag'])
														$pmb += $det['pmittag'];
													if($det['pabend'])
														$pmb += $det['pabend'];
													if($pmb > $pvb)
														$pmb=$pvb;

													$pnetto=$pvb+$pnb-$pmb;

													$dat=get_entry($det['datum'], 'datum');
													if($sort_travel)
													{
														if(!fxIsArray($rk[$zkp][$zk]['tagesangaben'][$bzk]))
															$rk[$zkp][$zk]['tagesangaben'][$bzk]=array('gek'=>$pnetto, 'gvk'=>$pnetto, 'mwst'=>0.0, 'brutto'=>$pnetto, 'zeit_id'=>$bzeit['zeit_id'], 'pv'=>$pv, 'pvb'=>$pvb, 'pn'=>$pn, 'pnb'=>$pnb, 'pf'=>$det['pfrueh'], 'pm'=>$det['pmittag'], 'pa'=>$det['pabend'], 'pmb'=>$pmb, 'datum'=>$dat, 'anmerkung'=>$bzeit['anmerkungintern'], 'rid'=>$ria, 'freigabe'=>$bifrei, 'n_verr'=>$birnverr, 'abger'=>$birabger);
														else
														{
															$rk[$zkp][$zk]['tagesangaben'][$bzk]['gek'] += $pnetto;
															$rk[$zkp][$zk]['tagesangaben'][$bzk]['gvk'] += $pnetto;
															$rk[$zkp][$zk]['tagesangaben'][$bzk]['brutto'] += $pnetto;
															$rk[$zkp][$zk]['tagesangaben'][$bzk]['pv'] += $pv;
															$rk[$zkp][$zk]['tagesangaben'][$bzk]['pvb'] += $pvb;
															$rk[$zkp][$zk]['tagesangaben'][$bzk]['pn'] += $pn;
															$rk[$zkp][$zk]['tagesangaben'][$bzk]['pnb'] += $pnb;
															$rk[$zkp][$zk]['tagesangaben'][$bzk]['pf'] += $det['pfrueh'];
															$rk[$zkp][$zk]['tagesangaben'][$bzk]['pm'] += $det['pmittag'];
															$rk[$zkp][$zk]['tagesangaben'][$bzk]['pa'] += $det['pabend'];
															$rk[$zkp][$zk]['tagesangaben'][$bzk]['pmb'] += $pmb;

															$dds=strpos($rk[$zkp][$zk]['tagesangaben'][$bzk]['datum'],' -');
															if($dds)
																$rk[$zkp][$zk]['tagesangaben'][$bzk]['datum']=substr((string)$rk[$zkp][$zk]['tagesangaben'][$bzk]['datum'],0,$dds);
															$rk[$zkp][$zk]['tagesangaben'][$bzk]['datum'] .= ' - '.$dat;
														}
													}
													else
													{
														$bzkn=$bzk.'_'.substr((string)$det['datum'],0,8);
														$rk[$zkp][$zk]['tagesangaben'][$bzkn]=array('gek'=>$pnetto, 'gek'=>$pnetto, 'mwst'=>0.0, 'brutto'=>$pnetto, 'zeit_id'=>$bzeit['zeit_id'], 'pv'=>$pv, 'pvb'=>$pvb, 'pn'=>$pn, 'pnb'=>$pnb, 'pf'=>$det['pfrueh'], 'pm'=>$det['pmittag'], 'pa'=>$det['pabend'], 'pmb'=>$pmb, 'datum'=>$dat, 'anmerkung'=>$bzeit['anmerkungintern'], 'rid'=>$ria, 'freigabe'=>$bifrei, 'n_verr'=>$birnverr, 'abger'=>$birabger);
													}
												}
											}
//fxDebug($rk[$zkp][$zk]['tagesangaben'], 'tagesangaben: $zi='.$zi.', $bzk='.$bzk);

											if($Reisekosten == 2138)
											{
												if(!fxIsArray($sb[$zkp]))
													$sb[$zkp]=array('art'=>$zeit['zeitart'], 'dat'=>$zeit['ist_beginn'], 'end'=>$zeit['ist_ende'], 'tid'=>(int)$zeit['taetigkeit_id'], 'rid'=>array(), 'freigabe'=>$bifrei, 'std'=>0, 'vstd'=>0, 'ek'=>array(), 'vk'=>array(), 'gek'=>0.0, 'gvk'=>0.0, 'mwst'=>0.0, 'brutto'=>0.0, 'n_verr'=>$birnverr, 'abger'=>$birabger);
												if($birabger == 2)
												{
													$rid=(int)$bzeit['rechnungs_id_rk'];
													if(!isset($sb[$zkp]['rid'][$rid]))
														$sb[$zkp]['rid'][$rid]=0;
													$sb[$zkp]['rid'][$rid]++;
												}

												$sb[$zkp]['gek']	+= $ek;
												$sb[$zkp]['gvk']	+= $vk;
												$sb[$zkp]['mwst']	+= $mw;
												$sb[$zkp]['brutto']	+= $bk;

												$aek=0.0;
												$avk=0.0;
												if($zeit['zeitaufwand'])
												{
													$aek=round($zeit['kosten_ek']*3600/$zeit['zeitaufwand'], $GLOBALS['fxpglobals']['settings']['nkstellen']);
													$avk=round($zeit['kosten_vk']*3600/$zeit['zeitaufwand'], $GLOBALS['fxpglobals']['settings']['nkstellen']);
												}
												$sb[$zkp]['ek'][$aek]=true;
												$sb[$zkp]['vk'][$avk]=true;

												// Falls unterschiedliche Freigabe, dann 2=Mix
												if($sb[$zkp]['freigabe'] != $bifrei)
													$sb[$zkp]['freigabe']=2;
												// Falls unterschiedlich verrechenbar, dann 4=Mix
												if($sb[$zkp]['n_verr'] != $birnverr)
													$sb[$zkp]['n_verr']=4;
												// Falls unterschiedlich abgerechnet, dann 3=Mix
												if($sb[$zkp]['abger'] != $birabger)
													$sb[$zkp]['abger']=3;
											}
										}

										if($all_done)
											unset($zeitdaten[$zi]);
									}

									// ...Einzelbelege
									else if(($bzeit['rk_kategorie'] != 1980) && (!fxIsArray($rk[$zkp][$zk]['einzelbelege']) || !fxIsArray($rk[$zkp][$zk]['einzelbelege'][$bzk])))
									{
										// Nur falls "Freigabe" + "Verrechenbar" + "Abgerechnet" übereinstimmt, sowie diese Belegsart angezeigt werden soll
										if($bfrei && $brverr && $brabger)
										{
											$za=(int)$bzeit['verz_grund'];
											if(($zahlungsart == 2845) || (($zahlungsart == 2843) && !$za) || (($zahlungsart == 2844) && $za))
											{
												if(!fxIsArray($rk[$zkp][$zk]))
													$rk[$zkp][$zk]=array('id'=>$zid, 'dat'=>$zeit['ist_beginn'], 'end'=>$zeit['ist_ende'], 'tid'=>(int)$zeit['taetigkeit_id'], 'rid'=>array(), 'freigabe'=>$bifrei, 'gek'=>0.0, 'gvk'=>0.0, 'mwst'=>0.0, 'brutto'=>0.0, 'rk_art'=>$zeit['rk_art'], 'rk_land'=>$zeit['land_id'], 'rk_fahrzeug'=>$zeit['rk_kategorie'], 'rk_start'=>$zeit['ist_beginn'], 'rk_ende'=>$zeit['ist_ende'], 'rk_ort1'=>$zeit['rk_ort1'], 'rk_ort2'=>$zeit['rk_ort2'], 'rk_mitfahrer'=>(int)$zeit['rk_mitfahrer'], 'rk_km'=>(int)$zeit['rk_km'], 'reisenr'=>$rnr, 'n_verr'=>$birnverr, 'abger'=>$birabger, 'tagesangaben'=>array(), 'einzelbelege'=>array());
												$ria=array();
												if($birabger == 2)
												{
													$rid=(int)$bzeit['rechnungs_id_rk'];
													$ria[$rid]=1;
												}

												// Falls unterschiedliche Freigabe, dann 2=Mix
												if(($Freigabe != 2871) && ($rk[$zkp][$zk]['freigabe'] != $bifrei))
													$rk[$zkp][$zk]['freigabe']=2;
												// Falls unterschiedlich verrechenbar, dann 4=Mix
												if(($verrechenbar != 2874) && ($rk[$zkp][$zk]['n_verr'] != $birnverr))
													$rk[$zkp][$zk]['n_verr']=4;
												// Falls unterschiedlich abgerechnet, dann 3=Mix
												if(($abgerechnet != 3234) && ($rk[$zkp][$zk]['abger'] != $birabger))
													$rk[$zkp][$zk]['abger']=3;

												// Datum
												$dat=get_entry($bzeit['ist_beginn'], 'datum');

												$rk[$zkp][$zk]['einzelbelege'][$bzk]=array('gek'=>$ek, 'gvk'=>$vk, 'mwst'=>$mw, 'brutto'=>$bk, 'belegnr'=>$bzeit['belegnr'], 'rk_kategorie'=>$bzeit['rk_kategorie'], 'rk_betreff'=>$bzeit['rk_ort1'], 'zahlungsart'=>$za, 'datum'=>$dat, 'rid'=>$ria, 'freigabe'=>$bifrei, 'n_verr'=>$birnverr, 'abger'=>$birabger);
//fxDebug($rk[$zkp][$zk]['einzelbelege'][$bzk], 'einzelbelege: $zi='.$zi.', $bzk='.$bzk, 0);

												if($Reisekosten == 2138)
												{
													if(!fxIsArray($sb[$zkp]))
														$sb[$zkp]=array('art'=>$zeit['zeitart'], 'dat'=>$zeit['ist_beginn'], 'end'=>$zeit['ist_ende'], 'tid'=>(int)$zeit['taetigkeit_id'], 'rid'=>array(), 'freigabe'=>$bifrei, 'std'=>0, 'vstd'=>0, 'ek'=>array(), 'vk'=>array(), 'gek'=>0.0, 'gvk'=>0.0, 'mwst'=>0.0, 'brutto'=>0.0, 'n_verr'=>$birnverr, 'abger'=>$birabger);
													if($birabger == 2)
													{
														$rid=(int)$bzeit['rechnungs_id_rk'];
														if(!isset($sb[$zkp]['rid'][$rid]))
															$sb[$zkp]['rid'][$rid]=0;
														$sb[$zkp]['rid'][$rid]++;
													}

													$sb[$zkp]['gek']	+= $ek;
													$sb[$zkp]['gvk']	+= $vk;
													$sb[$zkp]['mwst']	+= $mw;
													$sb[$zkp]['brutto']	+= $bk;

													$aek=0.0;
													$avk=0.0;
													if($zeit['zeitaufwand'])
													{
														$aek=round($zeit['kosten_ek']*3600/$zeit['zeitaufwand'], $GLOBALS['fxpglobals']['settings']['nkstellen']);
														$avk=round($zeit['kosten_vk']*3600/$zeit['zeitaufwand'], $GLOBALS['fxpglobals']['settings']['nkstellen']);
													}
													$sb[$zkp]['ek'][$aek]=true;
													$sb[$zkp]['vk'][$avk]=true;

													// Falls unterschiedliche Freigabe, dann 2=Mix
													if($sb[$zkp]['freigabe'] != $bifrei)
														$sb[$zkp]['freigabe']=2;
													// Falls unterschiedlich verrechenbar, dann 4=Mix
													if($sb[$zkp]['n_verr'] != $birnverr)
														$sb[$zkp]['n_verr']=4;
													// Falls unterschiedlich abgerechnet, dann 3=Mix
													if($sb[$zkp]['abger'] != $birabger)
														$sb[$zkp]['abger']=3;
												}
											}

											if($all_done)
												unset($zeitdaten[$zi]);
										}
									}
								}
//fxDebug($rk[$zkp][$zk], 'reisen: $zk='.$zk);
							}
						}

						if($zbis == $atag)
							unset($zeitdaten[$zk]);
					}
				}

				$se=false;
				if($atag == $ltag)
					$se=true;

				if($se && ($leer_anzeigen || sizeof($sb) || sizeof($rk)))
				{
					// Leerzeile
					if(($entries && $ae) || ($leer_anzeigen && ($zm == 1) && ($wd == $fd) && ($atag != $btag)))
					{
						if($leerzeile != $z-1)
						{
							$leerzeile=$z;
							fxf_maskColumnBorder($mca, 1, -1, $mc, $z, true);
							$z++;
						}
						$cw=$aw;
					}
					$ae=false;

					// Reisen neu sortieren nach Startdatum
					if($sort_travel)
					{
						$rka=array();
						$rkt=array();
						foreach($rk as $rpkid => $rdata)
						{
							$rvb='';
							foreach($rdata as $rz)
							{
								if(!strlen((string)$rvb))
								{
									if($rz['rk_art'] == 1983)		// Beleg(e) ohne Reise
										$rvb='0';
									else if($rz['rk_art'] == 1984)	// Sonstige
										$rvb='1';
									else							// In- oder Ausladnsreise
										$rvb='2';
									$rvb .= '-'.$rz['rk_start'].'-'.$rz['rk_ende'];
									break;
								}
							}
							if(strlen((string)$rvb))
							{
								while(isset($rkt[$rvb]))
									$rvb .= '-9';
								$rkt[$rvb]=$rpkid;
							}
						}
						if(fxIsArray($rkt))
						{
							ksort($rkt);
							foreach($rkt as $r)
								$rka[$r]=$rk[$r];
							$rk=$rka;
						}
					}

					if($aw != $cw)
					{
						if($entries && ($leerzeile != $z-1))
						{
							$leerzeile=$z;
							fxf_maskColumnBorder($mca, 1, -1, $mc, $z, true);
							$z++;
						}
						$cw=$aw;
					}

					$dtz=$z;
					$dtv=0;
					if($GLOBALS['fxpglobals']['tr'] == 150)	// Bei Reisekostenerfassung auch "leere" Reisen anzeigen
						$dtv=1;;

					if($zm == 1)
					{
						setzeTag();
						$dd=$atag;
					}
					else
					{
						if($stag >= $tday)
							$tc='blue';
						else
							$tc='black';
						if($zm == 2)
						{
							$GLOBALS['fxptdata']['lvalues'][$mc][$z]['tage']='~<span class="'.$tc.'">'.$monate[$m].' '.$y.'</span>';
							$dd=substr('0'.$m,-2).$y;
						}
						else
						{
							$GLOBALS['fxptdata']['lvalues'][$mc][$z]['tage']='~<span class="'.$tc.'">'.$y.'</span>';
							$dd=$y;
						}
						$GLOBALS['fxptdata']['lvalues'][$mc][$z]['datum']='~<span class="'.$tc.'">'.get_entry($stag, 'datum').'-'.get_entry($ltag, 'datum').'</span>';
					}

					if(!sizeof($sb) && !sizeof($rk))
					{
						$GLOBALS['fxptdata']['lvalues'][$mc][$z]['projekt']			= '~^';
						$GLOBALS['fxptdata']['lvalues'][$mc][$z]['sollstunden']		= '~^';
						$GLOBALS['fxptdata']['lvalues'][$mc][$z]['iststunden']		= '~^';
						$GLOBALS['fxptdata']['lvalues'][$mc][$z]['delta']			= '~^';
						$GLOBALS['fxptdata']['lvalues'][$mc][$z]['ek']				= '~^';
						$GLOBALS['fxptdata']['lvalues'][$mc][$z]['kosten']			= '~^';
						$GLOBALS['fxptdata']['lvalues'][$mc][$z]['mwst']			= '~^';
						$GLOBALS['fxptdata']['lvalues'][$mc][$z]['bruttobetrag']	= '~^';
						$GLOBALS['fxptdata']['lvalues'][$mc][$z]['ver_std']			= '~^';
						$GLOBALS['fxptdata']['lvalues'][$mc][$z]['vk']				= '~^';
						$GLOBALS['fxptdata']['lvalues'][$mc][$z]['ertrag']			= '~^';
						$GLOBALS['fxptdata']['lvalues'][$mc][$z]['marge_verbr']		= '~^';
						$GLOBALS['fxptdata']['lvalues'][$mc][$z]['link']			= '~^';
						$z++;
					}
					else
					{
						$zc=$z;
						foreach($projekte as $id => $p)
						{
							$zp=$z;
							$again=true;

							$pcl='darkgrey';
							if($projekte[$id]['st'] != FXP_PS_ACTIVE)
								$pcl='darkred';

							while($again)
							{
								$again=false;

								// Stundenbasis
								$skid=-1;
								$spid=0;
								$sdat=array();
								if(sizeof($sb))
								{
//fxDebug($sb, '$sb');
									foreach($sb as $spkid => $sdata)
									{
										$spkp=strpos($spkid, '#');
										if($spkp)
											$nspkid=substr((string)$spkid,0,$spkp);
										else
											$nspkid=$spkid;
										$spkp=strpos($nspkid, '|');
										if($spkp)
										{
											$skid=(int)substr((string)$nspkid, $spkp+1);
											$spid=(int)$nspkid;
											if($spid == $id)
											{
												$sdat=$sdata;
												unset($sb[$spkid]);
												$again=true;
												break;
											}
										}
										else if($nspkid == $id)
										{
											$spid=(int)$nspkid;
											$sdat=$sdata;
											unset($sb[$spkid]);
											$again=true;
											break;
										}
									}
								}
								if(sizeof($sdat))
								{
									if($zc != $z)
									{
										$GLOBALS['fxptdata']['lvalues'][$mc][$z]['tage']='~';
										$GLOBALS['fxptdata']['lvalues'][$mc][$z]['datum']='~';
									}

									if($zp == $z)
									{
										if($spid)
										{
											if($schaetzung && fxIsArray($festims) && fxIsArray($festims[$pers_id]) && fxIsArray($festims[$pers_id]['result']) && fxIsArray($festims[$pers_id]['result'][$fkey]) && fxIsArray($festims[$pers_id]['result'][$fkey]['tasks']) && fxIsArray($festims[$pers_id]['result'][$fkey]['tasks'][$id]))
											{
												$tl  = '<table width=100% border=0 cellpadding=0 cellspacing=0>';
												$tl .= '	<tr>';
												$tl .= '		<td width="100%">'.$projekte[$id]['vn'].'&nbsp;<font class="'.$pcl.'">'.$projekte[$id]['name'].'</font></td>';
												$tl	.= '		<td nowrap align="right">&nbsp;&nbsp;<span class="s2 '.$pcl.'">'.get_entry($festims[$pers_id]['result'][$fkey]['tasks'][$id]['phours']*3600,'zeitspanne','','s2').$unit_h.'</span></td>';
												$tl .= '	</tr>';
												$tl .= '</table>';
											}
											else
												$tl=$projekte[$id]['vn'].'&nbsp;<font class="'.$pcl.'">'.$projekte[$id]['name'].'</font>';
											$GLOBALS['fxptdata']['lvalues'][$mc][$z]['projekt']='~'.$tl;

											if(!isset($da[$id]))
												$da[$id]=$sdat['std'];
											else
												$da[$id] += $sdat['std'];
										}
										else
											$GLOBALS['fxptdata']['lvalues'][$mc][$z]['projekt']='~<i class="grey">'.$unbekannt.'</i>';
									}

									if(!$nva['Zeit_ID'])
									{
										$zeit='';
										if(($zm == 1) && ($sdat['dat'] != $sdat['end']))
											$zeit=get_entry($sdat['dat'],'zeit').'-'.get_entry($sdat['end'],'zeit');
										$GLOBALS['fxptdata']['lvalues'][$mc][$z]['zeit_id']='~<font class="darkgrey">'.$zeit.'</font>';
									}

									if(!$nva['leistungsarten'] && isset($larten[$skid]))
										$GLOBALS['fxptdata']['lvalues'][$mc][$z]['leistungsarten']	= '~'.$larten[$skid];

									if($sdat['n_verr'] == 3)		// nicht verrechenbar, da Buchung auf NA
										$vc='lightgrey';
									else if($sdat['n_verr'] == 2)	// verrechenbar, da Buchung auf FP
										$vc='darkgrey';
									else if(!$sdat['vstd'])			// nicht verrechenbar
										$vc='lightergrey';
									else
										$vc='';

									$ek=0.0;
									$vk=0.0;
									if($sdat['std'])
										$ek=$sdat['gek']*3600/$sdat['std'];
									if($sdat['vstd'])
										$vk=$sdat['gvk']*3600/$sdat['vstd'];
									if(sizeof($sdat['ek']) > 1)
										$ek='~<table width="80"><tr><td>&Delta;&nbsp;</td><td width="100%" align="right">'.getCH($ek).'</td></tr></table>';
									else
										$ek='~'.getCH($ek);
									if(sizeof($sdat['vk']) > 1)
										$vk='~<table width="80"><tr><td>&Delta;&nbsp;</td><td width="100%" align="right">'.getCH($vk).'</td></tr></table>';
									else
										$vk='~'.getCH($vk);

									$ph=$sdat['std'];
									if(fxIsArray($festims) && fxIsArray($festims[$pers_id]) && fxIsArray($festims[$pers_id]['result']) && fxIsArray($festims[$pers_id]['result'][$fkey]))
									{
										$ph=min($ph, $festims[$pers_id]['result'][$fkey]['whours']*3600-$sh);
										$sh += $ph;
									}

									$GLOBALS['fxptdata']['lvalues'][$mc][$z]['sollstunden']		= '~'.getH($ph);
									$GLOBALS['fxptdata']['lvalues'][$mc][$z]['iststunden']		= '~'.getH($sdat['std']);
									$GLOBALS['fxptdata']['lvalues'][$mc][$z]['delta']			= '~'.getH($sdat['std']-$ph);
									$GLOBALS['fxptdata']['lvalues'][$mc][$z]['ek']				= $ek;
									$GLOBALS['fxptdata']['lvalues'][$mc][$z]['kosten']			= '~'.getC($sdat['gek']);
									$GLOBALS['fxptdata']['lvalues'][$mc][$z]['mwst']			= '~'.getC($sdat['mwst']);
									$GLOBALS['fxptdata']['lvalues'][$mc][$z]['bruttobetrag']	= '~'.getC($sdat['brutto']);
									$GLOBALS['fxptdata']['lvalues'][$mc][$z]['ver_std']			= '~'.getH($sdat['vstd'],$vc);
									$GLOBALS['fxptdata']['lvalues'][$mc][$z]['vk']				= $vk;
									$GLOBALS['fxptdata']['lvalues'][$mc][$z]['ertrag']			= '~'.getC($sdat['gvk']);
									$GLOBALS['fxptdata']['lvalues'][$mc][$z]['marge_verbr']		= '~'.getC($sdat['gvk']-$sdat['gek']);

									if($sdat['std'] || $sdat['gek'] || $sdat['brutto'])
										$dtv++;

									$pers_mon_sstd			+= $ph;
									$pers_mon_istd			+= $sdat['std'];
									$pers_mon_vstd			+= $sdat['vstd'];
									$pers_mon_gek			+= $sdat['gek'];
									$pers_mon_gvk			+= $sdat['gvk'];
									$pers_mon_mwst			+= $sdat['mwst'];
									$pers_mon_brutto		+= $sdat['brutto'];

									$pers_ges_sstd			+= $ph;
									$pers_ges_istd			+= $sdat['std'];
									$pers_ges_vstd			+= $sdat['vstd'];
									$pers_ges_gek			+= $sdat['gek'];
									$pers_ges_gvk			+= $sdat['gvk'];
									$pers_ges_mwst			+= $sdat['mwst'];
									$pers_ges_brutto		+= $sdat['brutto'];

									// Freigabe + Verrechenbar + Abgerechnet
									setzeFVA($sdat);

									// Link ...berechtigt für eigene Person, oder falls Geschäftsführer, Verwaltung oder Admin
									$recht_sb=0;
									$trr=0;
									$tt='';
									if(isset($sdat['tid']) && ($GLOBALS['fxpglobals']['persdat']['roles'][-1] || $GLOBALS['fxpglobals']['persdat']['roles'][2] || ($GLOBALS['fxpglobals']['user'] == 1) || ($GLOBALS['fxpglobals']['person'] == $pers_id)))
									{
										if($sdat['art'] == FXP_TE_TIMEREC_SUM)
										{
											$trr=34;
											$tt=$stt[5];
										}
										else
										{
											$trr=81;
											$tt=$stt[4];
										}
										$recht_sb=tr_rechte($trr, -$pers_id);
									}

									if($recht_sb && ($projekte[$id]['st'] == FXP_PS_ACTIVE))
									{
										$href='?aktion='.FXP_CHANGE.'&gd='.substr((string)$sdat['dat'],0,8).'&persid='.$pers_id;
										if(sizeof($sdat['rid']) || ($sdat['freigabe'] > ($GLOBALS['fxpglobals']['persdat']['role'] + 1)))
										{
											$tt='<font class=red>'.$tsb_abgerechnet.'!</font>';
											$href='';
										}
										$GLOBALS['fxptdata']['lvalues'][$mc][$z]['link']='~'.fieldLinkImage('link'.$z, 'icch', $href, $tt, $trr);
									}
									else
										$GLOBALS['fxptdata']['lvalues'][$mc][$z]['link']='~<img class="ainactive" src="'.$GLOBALS['gfxpath'].'icch.png">';

									$entries++;
									$ec++;
									$ac++;
									$z++;
								}

								// Reisekosten
								$rkid=-1;
								$rpid=0;
								$rdat=array();
								if(($Reisekosten != 2138) && sizeof($rk))
								{
//fxDebug($rk, '$rk');
									foreach($rk as $rpkid => $rdata)
									{
										$rpkp=strpos($rpkid, '#');
										if($rpkp)
											$nrpkid=substr((string)$rpkid,0,$rpkp);
										else
											$nrpkid=$rpkid;
										$rpkp=strpos($nrpkid, '|');
//echo('$rpkid='.$rpkid.', $nrpkid='.$nrpkid.', $rpkp='.$rpkp.'<br />');
										if($rpkp)
										{
											$rkid=(int)substr((string)$nrpkid, $rpkp+1);
											$rpid=(int)$nrpkid;
											if($sort_travel)
											{
												$rdat=$rdata;
												$id=$rpid;
												unset($rk[$rpkid]);
												break;
											}
											else if($rpid == $id)
											{
												$rdat=$rdata;
												unset($rk[$rpkid]);
												$again=true;
												break;
											}
										}
										else if($sort_travel)
										{
											$rdat=$rdata;
											$id=$nrpkid;
											unset($rk[$rpkid]);
											break;
										}
										else if($nrpkid == $id)
										{
											$rpid=(int)$nrpkid;
											$rdat=$rdata;
											unset($rk[$rpkid]);
											$again=true;
											break;
										}
									}
								}
								if(sizeof($rdat))
								{
//fxDebug($rdat,'$rdat');
									foreach($rdat as $rz)
									{
										$rk_nettobetrag		= 0.0;
										$rk_mwst			= 0.0;
										$rk_bruttobetrag	= 0.0;

										$rkvalid=true;
										if(isset($rkrem[$rz['rk_start']]))
											$rkvalid=false;

										$rkcol1='black';
										$rkcol2='darkgrey';
										if(!$rkvalid)
										{
											$rkcol1='grey';
											$rkcol2='lightgrey';
										}

										$art		= (int)$rz['rk_art'];
										$land		= (int)$rz['rk_land'];
										$fahrzeug	= (int)$rz['rk_fahrzeug'];
										if(!$art)				// Alte Einträge
										{
											$art		= 1984;	// ...Sonstiges
											$fahrzeug	= 1966;	// ...Kein Fahrzeug
										}

										// Datum
										$dat1=substr((string)$rz['rk_start'],0,8);
										$dat2=substr((string)$rz['rk_ende'],0,8);
										$uhr1=substr((string)$rz['rk_start'],8,6);
										$uhr2=substr((string)$rz['rk_ende'],8,6);
										if(($uhr1 != '000000') || ($uhr2 != '000000'))
										{
											$dat=get_entry($rz['rk_start'], 'datetime').' - ';
											if($dat2 != $dat1)
												$dat .= get_entry($rz['rk_ende'], 'datetime');
											else
												$dat .= get_entry($rz['rk_ende'], 'zeit');
										}
										else if($dat2 != $dat1)
											$dat=get_entry($dat1, 'datum').' - '.get_entry($dat2, 'datum');
										else
											$dat=get_entry($dat1, 'datum');

										if($sort_travel)
										{
											// Leerzeile
											if($entries && ($leerzeile != $z-1))
											{
												$leerzeile=$z;
												fxf_maskColumnBorder($mca, 1, -1, $mc, $z, true);
												$z++;
											}

											// Reiseart: Startzeit-Endzeit
											$GLOBALS['fxptdata']['lvalues'][$mc][$z]['projekt']='~<font class="bold '.$rkcol1.'">'.$rk_arten[$art].':</font>&nbsp;<font class="'.$rkcol1.'">'.$dat.'</font>';
											// Reisenummer
											if(strlen((string)$rz['reisenr']))
												$GLOBALS['fxptdata']['lvalues'][$mc][$z]['projekt'] .= '<br /><img src="'.$GLOBALS['gfxpath'].'dmy.png" border="0" width="16" height="16" align="center"><font class="'.$rkcol2.'">'.$rz['reisenr'].'</font>';
											$rnz=$rz['reisenr'];
											if(strlen((string)$rnz))
												$rnz='&nbsp;&quot;'.$rnz.'&quot;';
											if($art != 1983)
											{
												// Startort-Zielort (Land), Fahrzeugart (km, Mitfahrer)
												$GLOBALS['fxptdata']['lvalues'][$mc][$z]['projekt'] .= '<br /><img src="'.$GLOBALS['gfxpath'].'dmy.png" border="0" width="16" height="16" align="center"><font class="'.$rkcol2.'">';
												if(strlen((string)$rz['rk_ort1']))
													$GLOBALS['fxptdata']['lvalues'][$mc][$z]['projekt'] .= $rz['rk_ort1'].'-'.$rz['rk_ort2'];
												if($land)
												{
													if(!$rk_laender[$land])
														$rk_laender[$land]=db_value("SELECT land_ort FROM rkpauschalen WHERE land_id=".$land." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
													$GLOBALS['fxptdata']['lvalues'][$mc][$z]['projekt'] .= '&nbsp;('.$rk_laender[$land].')';
												}
												if($fahrzeug != 1966)
												{
													if(strlen((string)$rz['rk_ort1']) || $land)
														$GLOBALS['fxptdata']['lvalues'][$mc][$z]['projekt'] .= ',&nbsp;';
													$GLOBALS['fxptdata']['lvalues'][$mc][$z]['projekt'] .= $rk_kategorien[$fahrzeug];
												}
												if(($rz['rk_km'] > 0) || ($rz['rk_mitfahrer'] > 0))
												{
													if(strlen((string)$rz['rk_ort1']) || ($fahrzeug != 1966))
														$GLOBALS['fxptdata']['lvalues'][$mc][$z]['projekt'] .= '&nbsp;&nbsp;';
													$GLOBALS['fxptdata']['lvalues'][$mc][$z]['projekt'] .= '(';
													if($rz['rk_km'] > 0)
													{
														$GLOBALS['fxptdata']['lvalues'][$mc][$z]['projekt'] .= $rz['rk_km'].'&nbsp;km';
														if($rz['rk_mitfahrer'] > 0)
															$GLOBALS['fxptdata']['lvalues'][$mc][$z]['projekt'] .= ',&nbsp;';
													}
													if($rz['rk_mitfahrer'] > 0)
														$GLOBALS['fxptdata']['lvalues'][$mc][$z]['projekt'] .= '+'.$rz['rk_mitfahrer'].substr((string)$mitfahrer_str, 0, 1);
													$GLOBALS['fxptdata']['lvalues'][$mc][$z]['projekt'] .= ')';
												}
												$GLOBALS['fxptdata']['lvalues'][$mc][$z]['projekt'] .= '</font>';
											}
											// Projekt
											$GLOBALS['fxptdata']['lvalues'][$mc][$z]['projekt'] .= '<br /><img src="'.$GLOBALS['gfxpath'].'dmy.png" border="0" width="16" height="16" align="center"><font class="'.$rkcol2.'">';
											if($id)
												$GLOBALS['fxptdata']['lvalues'][$mc][$z]['projekt'] .= '['.$projekte[$id]['vn'].'&nbsp;'.$projekte[$id]['name'].']';
											else
												$GLOBALS['fxptdata']['lvalues'][$mc][$z]['projekt'] .= '<i>'.$unbekannt.'</i>';
											$GLOBALS['fxptdata']['lvalues'][$mc][$z]['projekt'] .= '</font>';
											// Tätigkeit/Reisegrund
											$sql="SELECT taetigkeit_text, anmerkungintern FROM taetigkeiten WHERE zeit_id=".$rz['id']." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
											$gnd=db_values($sql);
//fxDebug($gnd, $sql);
											$rk_grund='';
											$rk_notiz='';
											if(fxIsArray($gnd))
											{
												if(strlen((string)$gnd['taetigkeit_text']))
													$rk_grund=trim((string)$gnd['taetigkeit_text']);
												if(strlen((string)$gnd['anmerkungintern']))
													$rk_notiz=trim((string)$gnd['anmerkungintern']);
											}
											if(strlen((string)$rk_grund))
												$GLOBALS['fxptdata']['lvalues'][$mc][$z]['projekt'] .= '<br /><img src="'.$GLOBALS['gfxpath'].'dmy.png" border="0" width="16" height="16" align="center"><font class="'.$rkcol2.'">&quot;'.$rk_grund.'&quot;</font>';

											$GLOBALS['fxptdata']['lvalues'][$mc][$z]['ek']				= '~^';
											$GLOBALS['fxptdata']['lvalues'][$mc][$z]['kosten']			= '~^';
											$GLOBALS['fxptdata']['lvalues'][$mc][$z]['mwst']			= '~^';
											$GLOBALS['fxptdata']['lvalues'][$mc][$z]['bruttobetrag']	= '~^';
											$GLOBALS['fxptdata']['lvalues'][$mc][$z]['vk']				= '~^';
											$GLOBALS['fxptdata']['lvalues'][$mc][$z]['ertrag']			= '~^';
											$GLOBALS['fxptdata']['lvalues'][$mc][$z]['marge_verbr']		= '~^';
										}
										else
										{
											if($zc != $z)
											{
												$GLOBALS['fxptdata']['lvalues'][$mc][$z]['tage']='~';
												$GLOBALS['fxptdata']['lvalues'][$mc][$z]['datum']='~';
											}

											if($zp == $z)
											{
												if($id)
													$GLOBALS['fxptdata']['lvalues'][$mc][$z]['projekt']	= '~'.$projekte[$id]['vn'].'&nbsp;<font class="'.$pcl.'">'.$projekte[$id]['name'].'</font>';
												else
													$GLOBALS['fxptdata']['lvalues'][$mc][$z]['projekt']	= '~<i class="grey">'.$unbekannt.'</i>';
												$GLOBALS['fxptdata']['lvalues'][$mc][$z]['ek']				= '~^';
												$GLOBALS['fxptdata']['lvalues'][$mc][$z]['kosten']			= '~^';
												$GLOBALS['fxptdata']['lvalues'][$mc][$z]['mwst']			= '~^';
												$GLOBALS['fxptdata']['lvalues'][$mc][$z]['bruttobetrag']	= '~^';
												$GLOBALS['fxptdata']['lvalues'][$mc][$z]['vk']				= '~^';
												$GLOBALS['fxptdata']['lvalues'][$mc][$z]['ertrag']			= '~^';
												$GLOBALS['fxptdata']['lvalues'][$mc][$z]['marge_verbr']		= '~^';
												$z++;

												$GLOBALS['fxptdata']['lvalues'][$mc][$z]['tage']='~';
												$GLOBALS['fxptdata']['lvalues'][$mc][$z]['datum']='~';
											}

											if(isset($larten[$rkid]))
												$GLOBALS['fxptdata']['lvalues'][$mc][$z]['leistungsarten']	= '~'.$larten[$rkid];

											$GLOBALS['fxptdata']['lvalues'][$mc][$z]['projekt']='~<font class="bold '.$rkcol1.'">'.$rk_arten[$art].'</font>';
											if(strlen((string)$rz['reisenr']))
												$GLOBALS['fxptdata']['lvalues'][$mc][$z]['projekt'] .= '<font class="'.$rkcol1.'">'.$rz['reisenr'].'</font>';
											if(($art == 1982) && $land)
											{
												if(!$rk_laender[$land])
													$rk_laender[$land]=db_value("SELECT land_ort FROM rkpauschalen WHERE land_id=".$land." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
												$GLOBALS['fxptdata']['lvalues'][$mc][$z]['projekt'] .= '&nbsp;<font class="'.$rkcol1.'">('.$rk_laender[$land].')</font>';
											}
											if(($art != 1983) && $rkvalid)
											{
												// Tätigkeit/Reisegrund
												$sql="SELECT taetigkeit_text, anmerkungintern FROM taetigkeiten WHERE zeit_id=".$rz['id']." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
												$gnd=db_values($sql);
//fxDebug($gnd, $sql);
												$rk_grund='';
												$rk_notiz='';
												if(fxIsArray($gnd))
												{
													if(($gnd['taetigkeit_text'] !== NULL) && strlen((string)$gnd['taetigkeit_text']))
														$rk_grund=trim((string)$gnd['taetigkeit_text']);
													if(($gnd['anmerkungintern'] !== NULL) && strlen((string)$gnd['anmerkungintern']))
														$rk_notiz=trim((string)$gnd['anmerkungintern']);
												}
												if(strlen((string)$rk_grund))
													$GLOBALS['fxptdata']['lvalues'][$mc][$z]['projekt'] .= '<br /><img src="'.$GLOBALS['gfxpath'].'dmy.png" border="0" width="16" height="16" align="center"><font class="'.$rkcol2.'">['.$rk_grund.']</font>';
											}
											// Startzeit-Endzeit
											$GLOBALS['fxptdata']['lvalues'][$mc][$z]['projekt'] .= '<br /><img src="'.$GLOBALS['gfxpath'].'dmy.png" border="0" width="16" height="16" align="center"><font class="'.$rkcol2.'">'.$dat.'</font>';
											if(($art != 1983) && $rkvalid)
											{
												// Startort-Zielort, Fahrzeugart (km, Mitfahrer)
												$GLOBALS['fxptdata']['lvalues'][$mc][$z]['projekt'] .= '<br /><img src="'.$GLOBALS['gfxpath'].'dmy.png" border="0" width="16" height="16" align="center"><font class="'.$rkcol2.'">';
												if(strlen((string)$rz['rk_ort1']))
													$GLOBALS['fxptdata']['lvalues'][$mc][$z]['projekt'] .= $rz['rk_ort1'].'-'.$rz['rk_ort2'];
												if($fahrzeug != 1966)
												{
													if(strlen((string)$rz['rk_ort1']) || $land)
														$GLOBALS['fxptdata']['lvalues'][$mc][$z]['projekt'] .= ',&nbsp;';
													$GLOBALS['fxptdata']['lvalues'][$mc][$z]['projekt'] .= $rk_kategorien[$fahrzeug];
												}
												if(($rz['rk_km'] > 0) || ($rz['rk_mitfahrer'] > 0))
												{
													if(strlen((string)$rz['rk_ort1']) || ($fahrzeug != 1966))
														$GLOBALS['fxptdata']['lvalues'][$mc][$z]['projekt'] .= '&nbsp;&nbsp;';
													$GLOBALS['fxptdata']['lvalues'][$mc][$z]['projekt'] .= '(';
													if($rz['rk_km'] > 0)
													{
														$GLOBALS['fxptdata']['lvalues'][$mc][$z]['projekt'] .= $rz['rk_km'].'&nbsp;km';
														if($rz['rk_mitfahrer'] > 0)
															$GLOBALS['fxptdata']['lvalues'][$mc][$z]['projekt'] .= ',&nbsp;';
													}
													if($rz['rk_mitfahrer'] > 0)
														$GLOBALS['fxptdata']['lvalues'][$mc][$z]['projekt'] .= '+'.$rz['rk_mitfahrer'].substr((string)$mitfahrer_str, 0, 1);
													$GLOBALS['fxptdata']['lvalues'][$mc][$z]['projekt'] .= ')';
												}
												$GLOBALS['fxptdata']['lvalues'][$mc][$z]['projekt'] .= '</font>';
											}

											$GLOBALS['fxptdata']['lvalues'][$mc][$z]['ek']='~^';
											$GLOBALS['fxptdata']['lvalues'][$mc][$z]['vk']='~^';
											if($rkvalid)
											{
												if(!$rz['gek'])
												{
													$GLOBALS['fxptdata']['lvalues'][$mc][$z]['kosten']			= '^class_? grey|~'.get_entry($rz['gek'],'dezimal').$unit_c;
													$GLOBALS['fxptdata']['lvalues'][$mc][$z]['mwst']			= '^class_? grey|~'.get_entry($rz['mwst'],'dezimal').$unit_c;
													$GLOBALS['fxptdata']['lvalues'][$mc][$z]['bruttobetrag']	= '^class_? grey|~'.get_entry($rz['brutto'],'dezimal').$unit_c;
													$GLOBALS['fxptdata']['lvalues'][$mc][$z]['ertrag']			= '^class_? grey|~'.get_entry($rz['brutto'],'dezimal').$unit_c;
													$GLOBALS['fxptdata']['lvalues'][$mc][$z]['marge_verbr']		= '^class_? grey|~'.get_entry($rz['brutto'],'dezimal').$unit_c;
												}
												else
												{
													$GLOBALS['fxptdata']['lvalues'][$mc][$z]['kosten']			= '~'.get_entry($rz['gek'],'dezimal').$unit_c;
													$GLOBALS['fxptdata']['lvalues'][$mc][$z]['mwst']			= '~'.get_entry($rz['mwst'],'dezimal').$unit_c;
													$GLOBALS['fxptdata']['lvalues'][$mc][$z]['bruttobetrag']	= '~'.get_entry($rz['brutto'],'dezimal').$unit_c;
													$GLOBALS['fxptdata']['lvalues'][$mc][$z]['ertrag']			= '~'.get_entry($rz['gvk'],'dezimal').$unit_c;
													$GLOBALS['fxptdata']['lvalues'][$mc][$z]['marge_verbr']		= '~'.get_entry($rz['gvk']-$rz['gek'],'dezimal').$unit_c;;
												}

												if($rz['gek'])
													$dtv++;

												$rk_nettobetrag			+= $rz['gek'];
												$rk_mwst				+= $rz['mwst'];
												$rk_bruttobetrag		+= $rz['brutto'];

												$pers_mon_gek			+= $rz['gek'];
												$pers_mon_gvk			+= $rz['gvk'];
												$pers_mon_mwst			+= $rz['mwst'];
												$pers_mon_brutto		+= $rz['brutto'];

												$pers_ges_gek			+= $rz['gek'];
												$pers_ges_gvk			+= $rz['gvk'];
												$pers_ges_mwst			+= $rz['mwst'];
												$pers_ges_brutto		+= $rz['brutto'];
											}
											else
											{
												$GLOBALS['fxptdata']['lvalues'][$mc][$z]['kosten']			= '~^';
												$GLOBALS['fxptdata']['lvalues'][$mc][$z]['mwst']			= '~^';
												$GLOBALS['fxptdata']['lvalues'][$mc][$z]['bruttobetrag']	= '~^';
												$GLOBALS['fxptdata']['lvalues'][$mc][$z]['ertrag']			= '~^';
												$GLOBALS['fxptdata']['lvalues'][$mc][$z]['marge_verbr']		= '~^';
											}

											// Freigabe + Verrechenbar + Abgerechnet
											setzeFVA($rz);
										}
										$entries++;
										$ec++;
										$ac++;

										// Link ...berechtigt für eigene Person, oder falls Geschäftsführer, Verwaltung oder Admin
										if($rkvalid)
										{
											$recht_rk=0;
											$trr=0;
											$tt='';
											if(isset($rz['tid']) && ($GLOBALS['fxpglobals']['persdat']['roles'][-1] || $GLOBALS['fxpglobals']['persdat']['roles'][2] || ($GLOBALS['fxpglobals']['user'] == 1) || ($GLOBALS['fxpglobals']['person'] == $pers_id)))
											{
												$trr=150;
												$tt=$stt[3];
												$recht_rk=tr_rechte($trr, -$pers_id);
											}

											if($recht_rk && ($projekte[$id]['st'] == FXP_PS_ACTIVE))
											{
												$href='?aktion='.FXP_CHANGE.'&gd='.substr((string)$rz['dat'],0,8).'&persid='.$pers_id;
												if(sizeof($rz['rid']) || ($rz['freigabe'] > ($GLOBALS['fxpglobals']['persdat']['role'] + 1)))
													$tt='<font class=red>'.$trk_abgerechnet.'!</font>';
												$GLOBALS['fxptdata']['lvalues'][$mc][$z]['link']='~'.fieldLinkImage('link'.$z, 'icch', $href, $tt, $trr);
											}
											else
												$GLOBALS['fxptdata']['lvalues'][$mc][$z]['link']='~<img class="ainactive" src="'.$GLOBALS['gfxpath'].'icch.png">';
										}

										$z++;

										$rkcol1='black';
										$rkcol2='darkgrey';

										// ...Tagesangaben
										if(fxIsArray($rz['tagesangaben']))
										{
											$zb=0;
											foreach($rz['tagesangaben'] as $ta)
											{
												if($zp == $z)
												{
													$GLOBALS['fxptdata']['lvalues'][$mc][$z]['projekt']			= '~'.$projekte[$id]['vn'].'&nbsp;<font class="'.$pcl.'">'.$projekte[$id]['name'].'</font>';
													$GLOBALS['fxptdata']['lvalues'][$mc][$z]['ek']				= '~^';
													$GLOBALS['fxptdata']['lvalues'][$mc][$z]['kosten']			= '~^';
													$GLOBALS['fxptdata']['lvalues'][$mc][$z]['mwst']			= '~^';
													$GLOBALS['fxptdata']['lvalues'][$mc][$z]['bruttobetrag']	= '~^';
													$GLOBALS['fxptdata']['lvalues'][$mc][$z]['vk']				= '~^';
													$GLOBALS['fxptdata']['lvalues'][$mc][$z]['ertrag']			= '~^';
													$GLOBALS['fxptdata']['lvalues'][$mc][$z]['marge_verbr']		= '~^';
													$z++;
												}
												if($zc != $z)
												{
													$GLOBALS['fxptdata']['lvalues'][$mc][$z]['tage']='~';
													$GLOBALS['fxptdata']['lvalues'][$mc][$z]['datum']='~';
												}
												if($ta['gek'])
												{
													$dtv++;

													if(!$zb)
													{
														$zb=$z;
														$GLOBALS['fxptdata']['lvalues'][$mc][$z]['projekt']			= '~<img src="'.$GLOBALS['gfxpath'].'dmy.png" border="0" width="16" height="16" align="center">&middot;&nbsp;<font class="'.$rkcol1.'">'.$rk_kategorien[1980].':</font>';
														$GLOBALS['fxptdata']['lvalues'][$mc][$z]['ek']				= '~^';
														$GLOBALS['fxptdata']['lvalues'][$mc][$z]['kosten']			= '~^';
														$GLOBALS['fxptdata']['lvalues'][$mc][$z]['mwst']			= '~^';
														$GLOBALS['fxptdata']['lvalues'][$mc][$z]['bruttobetrag']	= '~^';
														$GLOBALS['fxptdata']['lvalues'][$mc][$z]['vk']				= '~^';
														$GLOBALS['fxptdata']['lvalues'][$mc][$z]['ertrag']			= '~^';
														$GLOBALS['fxptdata']['lvalues'][$mc][$z]['marge_verbr']		= '~^';
														$z++;
													}

													$GLOBALS['fxptdata']['lvalues'][$mc][$z]['tage']='~';
													$GLOBALS['fxptdata']['lvalues'][$mc][$z]['datum']='~';

													$GLOBALS['fxptdata']['lvalues'][$mc][$z]['projekt']  = '~<img src="'.$GLOBALS['gfxpath'].'dmy.png" border="0" width="32" height="16" align="center"><font class="'.$rkcol2.'">'.$ta['datum'].'</font>';
													$GLOBALS['fxptdata']['lvalues'][$mc][$z]['projekt'] .= '&nbsp;<font class="s2 '.$rkcol2.'">(';
													if($ta['pv'])
													{
														$GLOBALS['fxptdata']['lvalues'][$mc][$z]['projekt'] .= '<span tooltip="'.get_entry($ta['pvb'],'dezimal').$unit_c.'" style="cursor:help;">'.$ta['pv'].'x&nbsp;'.$verpfl.'</span>';
														if($ta['pn'])
															$GLOBALS['fxptdata']['lvalues'][$mc][$z]['projekt'] .= '&nbsp;+&nbsp;';
													}
													if($ta['pn'])
														$GLOBALS['fxptdata']['lvalues'][$mc][$z]['projekt'] .= '<span tooltip="'.get_entry($ta['pnb'],'dezimal').' '.$unit_c.'" style="cursor:help;">'.$ta['pn'].'x&nbsp;'.$uebern.'</span>';
													if($ta['pmb'])
														$GLOBALS['fxptdata']['lvalues'][$mc][$z]['projekt'] .= '&nbsp;<span tooltip="'.get_entry($ta['pmb'],'dezimal').$unit_c.'" style="cursor:help;">'.$sachbez.'</span>';
													$GLOBALS['fxptdata']['lvalues'][$mc][$z]['projekt'] .= ')</font>';
													if(($ta['anmerkung'] !== NULL) && strlen((string)$ta['anmerkung']))
														$GLOBALS['fxptdata']['lvalues'][$mc][$z]['projekt'] .= '<br /><img src="'.$GLOBALS['gfxpath'].'dmy.png" border="0" width="52" height="16" align="center"><font class="'.$rkcol2.'">['.$ta['anmerkung'].']</font>';

													$GLOBALS['fxptdata']['lvalues'][$mc][$z]['ek']				= '~^';
													$GLOBALS['fxptdata']['lvalues'][$mc][$z]['kosten']			= '~'.get_entry($ta['gek'],'dezimal').$unit_c;
													$GLOBALS['fxptdata']['lvalues'][$mc][$z]['mwst']			= '~'.get_entry($ta['mwst'],'dezimal').$unit_c;
													$GLOBALS['fxptdata']['lvalues'][$mc][$z]['bruttobetrag']	= '~'.get_entry($ta['brutto'],'dezimal').$unit_c;
													$GLOBALS['fxptdata']['lvalues'][$mc][$z]['vk']				= '~^';
													$GLOBALS['fxptdata']['lvalues'][$mc][$z]['ertrag']			= '~'.get_entry($ta['gvk'],'dezimal').$unit_c;
													$GLOBALS['fxptdata']['lvalues'][$mc][$z]['marge_verbr']		= '~'.get_entry($ta['gvk']-$ta['gek'],'dezimal').$unit_c;

													$rk_nettobetrag			+= $ta['gek'];
													$rk_mwst				+= $ta['mwst'];
													$rk_bruttobetrag		+= $ta['brutto'];

													$pers_mon_gek			+= $ta['gek'];
													$pers_mon_gvk			+= $ta['gvk'];
													$pers_mon_mwst			+= $ta['mwst'];
													$pers_mon_brutto		+= $ta['brutto'];

													$pers_ges_gek			+= $ta['gek'];
													$pers_ges_gvk			+= $ta['gvk'];
													$pers_ges_mwst			+= $ta['mwst'];
													$pers_ges_brutto		+= $ta['brutto'];

													// Freigabe + Verrechenbar + Abgerechnet
													setzeFVA($ta);

													$z++;
												}
											}
										}

										// ...Fahrkosten
										if($sort_travel && ($art != 1983) && ($rz['brutto'] > 0.0))
										{
											$GLOBALS['fxptdata']['lvalues'][$mc][$z]['projekt']			= '~<img src="'.$GLOBALS['gfxpath'].'dmy.png" border="0" width="16" height="16" align="center">&middot;&nbsp;<font class="'.$rkcol1.'">'.$fahrkosten.'</font>';

											$GLOBALS['fxptdata']['lvalues'][$mc][$z]['ek']				= '~^';
											$GLOBALS['fxptdata']['lvalues'][$mc][$z]['kosten']			= '~'.get_entry($rz['gek'],'dezimal').$unit_c;
											$GLOBALS['fxptdata']['lvalues'][$mc][$z]['mwst']			= '~'.get_entry($rz['mwst'],'dezimal').$unit_c;
											$GLOBALS['fxptdata']['lvalues'][$mc][$z]['bruttobetrag']	= '~'.get_entry($rz['brutto'],'dezimal').$unit_c;
											$GLOBALS['fxptdata']['lvalues'][$mc][$z]['vk']				= '~^';
											$GLOBALS['fxptdata']['lvalues'][$mc][$z]['ertrag']			= '~'.get_entry($rz['gvk'],'dezimal').$unit_c;
											$GLOBALS['fxptdata']['lvalues'][$mc][$z]['marge_verbr']		= '~'.get_entry($rz['gvk']-$rz['gek'],'dezimal').$unit_c;

											if($rz['gek'])
												$dtv++;

											$rk_nettobetrag			+= $rz['gek'];
											$rk_mwst				+= $rz['mwst'];
											$rk_bruttobetrag		+= $rz['brutto'];

											$pers_mon_gek			+= $rz['gek'];
											$pers_mon_gvk			+= $rz['gvk'];
											$pers_mon_mwst			+= $rz['mwst'];
											$pers_mon_brutto		+= $rz['brutto'];

											$pers_ges_gek			+= $rz['gek'];
											$pers_ges_gvk			+= $rz['gvk'];
											$pers_ges_mwst			+= $rz['mwst'];
											$pers_ges_brutto		+= $rz['brutto'];

											// Freigabe + Verrechenbar + Abgerechnet
											setzeFVA($rz);

											$z++;
										}

										// ...Einzelbelege
										if(fxIsArray($rz['einzelbelege']))
										{
											ksort($rz['einzelbelege']);
											$zb=0;
											foreach($rz['einzelbelege'] as $bz)
											{
//fxDebug($bz, '$bz');
												if($zp == $z)
												{
													$GLOBALS['fxptdata']['lvalues'][$mc][$z]['projekt']			= '~'.$projekte[$id]['vn'].'&nbsp;<font class="'.$pcl.'">'.$projekte[$id]['name'].'</font>';
													$GLOBALS['fxptdata']['lvalues'][$mc][$z]['ek']				= '~^';
													$GLOBALS['fxptdata']['lvalues'][$mc][$z]['kosten']			= '~^';
													$GLOBALS['fxptdata']['lvalues'][$mc][$z]['mwst']			= '~^';
													$GLOBALS['fxptdata']['lvalues'][$mc][$z]['bruttobetrag']	= '~^';
													$GLOBALS['fxptdata']['lvalues'][$mc][$z]['vk']				= '~^';
													$GLOBALS['fxptdata']['lvalues'][$mc][$z]['ertrag']			= '~^';
													$GLOBALS['fxptdata']['lvalues'][$mc][$z]['marge_verbr']		= '~^';
													$z++;
												}
												if($zc != $z)
												{
													$GLOBALS['fxptdata']['lvalues'][$mc][$z]['tage']='~';
													$GLOBALS['fxptdata']['lvalues'][$mc][$z]['datum']='~';
												}
												if($bz['gek'])
												{
													$dtv++;

													if(!$zb)
													{
														$zb=$z;
														$GLOBALS['fxptdata']['lvalues'][$mc][$z]['projekt']			= '~<img src="'.$GLOBALS['gfxpath'].'dmy.png" border="0" width="16" height="16" align="center">&middot;&nbsp;<font class="'.$rkcol1.'">'.$belege.':</font>';
														$GLOBALS['fxptdata']['lvalues'][$mc][$z]['ek']				= '~^';
														$GLOBALS['fxptdata']['lvalues'][$mc][$z]['kosten']			= '~^';
														$GLOBALS['fxptdata']['lvalues'][$mc][$z]['mwst']			= '~^';
														$GLOBALS['fxptdata']['lvalues'][$mc][$z]['bruttobetrag']	= '~^';
														$GLOBALS['fxptdata']['lvalues'][$mc][$z]['vk']				= '~^';
														$GLOBALS['fxptdata']['lvalues'][$mc][$z]['ertrag']			= '~^';
														$GLOBALS['fxptdata']['lvalues'][$mc][$z]['marge_verbr']		= '~^';
														$z++;
													}

													$GLOBALS['fxptdata']['lvalues'][$mc][$z]['tage']='~';
													$GLOBALS['fxptdata']['lvalues'][$mc][$z]['datum']='~';

													$blnr=trim((string)$bz['belegnr']);
													if(strlen((string)$blnr))
														$blnr='<font class="'.$rkcol2.'">'.$blnr.':</font>&nbsp;&nbsp;';
													$GLOBALS['fxptdata']['lvalues'][$mc][$z]['projekt']='~<img src="'.$GLOBALS['gfxpath'].'dmy.png" border="0" width="32" height="16" align="center">'.$blnr.'<font class="'.$rkcol2.'">'.$rk_kategorien[$bz['rk_kategorie']].'</font>&nbsp;&nbsp;<font class="'.$rkcol2.'">'.$bz['datum'].'</font>&nbsp;&nbsp;<i class="s2 '.$rkcol2.'">['.$za_array[$bz['zahlungsart']].']</i>';
													if(strlen((string)$bz['rk_betreff']))
														$GLOBALS['fxptdata']['lvalues'][$mc][$z]['projekt'] .= '<br /><img src="'.$GLOBALS['gfxpath'].'dmy.png" border="0" width="52" height="16" align="center"><font class=" '.$rkcol2.'">&quot;'.$bz['rk_betreff'].'&quot;</font>';

													$GLOBALS['fxptdata']['lvalues'][$mc][$z]['ek']				= '~^';
													$GLOBALS['fxptdata']['lvalues'][$mc][$z]['kosten']			= '~'.get_entry($bz['gek'],'dezimal').$unit_c;
													$GLOBALS['fxptdata']['lvalues'][$mc][$z]['mwst']			= '~'.get_entry($bz['mwst'],'dezimal').$unit_c;
													$GLOBALS['fxptdata']['lvalues'][$mc][$z]['bruttobetrag']	= '~'.get_entry($bz['brutto'],'dezimal').$unit_c;
													$GLOBALS['fxptdata']['lvalues'][$mc][$z]['vk']				= '~^';
													$GLOBALS['fxptdata']['lvalues'][$mc][$z]['ertrag']			= '~'.get_entry($bz['gvk'],'dezimal').$unit_c;
													$GLOBALS['fxptdata']['lvalues'][$mc][$z]['marge_verbr']		= '~'.get_entry($bz['gvk']-$bz['gek'],'dezimal').$unit_c;

													$rk_nettobetrag			+= $bz['gek'];
													$rk_mwst				+= $bz['mwst'];
													$rk_bruttobetrag		+= $bz['brutto'];

													$pers_mon_gek			+= $bz['gek'];
													$pers_mon_gvk			+= $bz['gvk'];
													$pers_mon_mwst			+= $bz['mwst'];
													$pers_mon_brutto		+= $bz['brutto'];

													$pers_ges_gek			+= $bz['gek'];
													$pers_ges_gvk			+= $bz['gvk'];
													$pers_ges_mwst			+= $bz['mwst'];
													$pers_ges_brutto		+= $bz['brutto'];

													// Freigabe + Verrechenbar + Abgerechnet
													setzeFVA($bz);

													$z++;
												}
											}
										}

										$rkrem[$rz['rk_start']]=true;

										// Zwischensumme Reise
										if($sort_travel)
										{
											$pzsa[]=$z;

											$sline=true;

											$GLOBALS['maske231_as'.$z]='border-top:1px solid #aaaaaa;padding-top:3px;padding-bottom:3px;';
											$cls1='^col_e2e2e2" style="'.$GLOBALS['maske231_as'.$z];
											$cls2='^col_e2e2e2';
											$cls3='^col_ececec';

											$zs='&nbsp;<b>'.$zwischensumme.':</b> '.$rk_arten[$art].$rnz;

											if($scolspan > 1)
											{
												$GLOBALS['fxptdata']['lvalues'][$mc][$z][$fcol]			= $cls1.'" nowrap colspan="'.$scolspan.'"><!--|~'.$zs;
												$GLOBALS['fxptdata']['lvalues'][$mc][$z][$slcol]		= '~-->'.$zs;
											}
											else
												$GLOBALS['fxptdata']['lvalues'][$mc][$z][$fcol]			= $cls1.'|~'.$zs;

											$GLOBALS['fxptdata']['lvalues'][$mc][$z]['sollstunden']		= $cls2.'|~';
											$GLOBALS['fxptdata']['lvalues'][$mc][$z]['iststunden']		= $cls2.'|~';
											$GLOBALS['fxptdata']['lvalues'][$mc][$z]['delta']			= $cls2.'|~';
											$GLOBALS['fxptdata']['lvalues'][$mc][$z]['ek']				= $cls2.'|~';
											$GLOBALS['fxptdata']['lvalues'][$mc][$z]['kosten']			= $cls2.'|~'.get_entry($rk_nettobetrag,'dezimal').$unit_c;
											$GLOBALS['fxptdata']['lvalues'][$mc][$z]['mwst']			= $cls2.'|~'.get_entry($rk_mwst,'dezimal').$unit_c;
											$GLOBALS['fxptdata']['lvalues'][$mc][$z]['bruttobetrag']	= $cls2.'|~'.get_entry($rk_bruttobetrag,'dezimal').$unit_c;
											$GLOBALS['fxptdata']['lvalues'][$mc][$z]['ver_std']			= $cls2.'|~';
											$GLOBALS['fxptdata']['lvalues'][$mc][$z]['vk']				= $cls2.'|~';
											$GLOBALS['fxptdata']['lvalues'][$mc][$z]['ertrag']			= $cls2.'|~'.get_entry($rk_nettobetrag,'dezimal').$unit_c;
											$GLOBALS['fxptdata']['lvalues'][$mc][$z]['marge_verbr']		= $cls2.'|~'.get_entry(0.0,'dezimal').$unit_c;
											$GLOBALS['fxptdata']['lvalues'][$mc][$z]['freigabe']		= $cls3.'|';
											$GLOBALS['fxptdata']['lvalues'][$mc][$z]['verrechenbar']	= $cls3.'|';
											$GLOBALS['fxptdata']['lvalues'][$mc][$z]['abgerechnet']		= $cls3.'|';
											$GLOBALS['fxptdata']['lvalues'][$mc][$z]['link']			= $cls3.'|';
											$z++;

											$sline=false;
										}
									}
								}
							}
						}
					}

					$sb=array();
					$rk=array();

					if(!$leer_anzeigen && !$dtv)
					{
//echo('<b>DEL</b> $dtv: '.$dtz.'-'.$z.'<hr />');
						while($z >= $dtz)
						{
							unset($GLOBALS['fxptdata']['lvalues'][$mc][$z]);
							$z--;
						}
						$z=$dtz;
						$dd='';
					}
				}

				// Zukunftsschätzung?
				if(fxIsArray($festims) && fxIsArray($festims[$pers_id]) && fxIsArray($festims[$pers_id]['result']) && fxIsArray($festims[$pers_id]['result'][$fkey]) && ($festims[$pers_id]['result'][$fkey]['edate'] == $atag) && $festims[$pers_id]['result'][$fkey]['whours'])
				{
					$fra=$festims[$pers_id]['result'][$fkey];
					$cday=$festims[$pers_id]['cday'];

					// ...Schätzungs-, Fehl- und/oder Leerstunden
					$emuha=array($fra['ehours'], $fra['mhours'], $fra['uhours']);
					foreach($emuha as $hm => $h)
					{
						if($h)
						{
							$eh=round($h*3600,$accuracy);
							$ch=0.0;
							if($hm != 1)
								$ch=$eh;
							$ih=0.0;
							if(!$hm)
								$ih=round($fra['ihours']*3600,$accuracy);
							$net=0.0;
							if($fra['weffort'])
								$net=round(($fra['spp']*$h)/$fra['weffort'],$accuracy);
							$inv=0.0;
							if(!$hm && $fra['ieffort'])
								$inv=round(($fra['ssp']*$fra['ihours'])/$fra['ieffort'],$accuracy);
							$ek=0.0;
							if($eh)
								$ek=round($net*3600/$eh, $accuracy);
							$vk=0.0;
							if($ih)
								$vk=round($inv*3600/$ih, $accuracy);
							$grs=round($net*$vatcal, $accuracy);
							$vat=$grs-$net;
//echo('$pers_id='.$pers_id.', $hm='.$hm.': z='.$z.', $h='.$h.' -- $fkey='.$fkey.' -- $eh='.$eh.', $ih='.$ih.', $net='.$net.', $inv='.$inv.', $ek='.$ek.', $vk='.$vk.'<br />');

							// Leerzeile
							if($ae && $entries)
							{
								if($leerzeile != $z-1)
								{
									$leerzeile=$z;
									fxf_maskColumnBorder($mca, 1, -1, $mc, $z, true);
									$z++;
								}
								$cw=$aw;
							}
							$ae=false;

							if(!$hm)
								$pc='lightblue';
							else if($hm == 1)
								$pc='lightred';
							else
								$pc='lightgrey';

							if(!$hm)
							{
								if($vk)
								{
									$fc='blue';
									$lc='lightblue';
								}
								else
								{
									$fc='grey';
									$lc='lightergrey';
								}
							}
							else
							{
								$fc='red';
								$lc='lightred';
							}

							// Tag, Datum + Zeit
							if(strlen((string)$dd) && $ec)
							{
								$GLOBALS['fxptdata']['lvalues'][$mc][$z]['tage']='~';
								$GLOBALS['fxptdata']['lvalues'][$mc][$z]['datum']='~';
							}
							else
							{
								if(strlen((string)$dd) && !$ec)
									$z--;

								if($aw != $cw)
								{
									if($entries && ($leerzeile != $z-1))
									{
										$leerzeile=$z;
										fxf_maskColumnBorder($mca, 1, -1, $mc, $z, true);
										$z++;
									}
									$cw=$aw;
								}

								if($zm == 1)
								{
									setzeTag();
									$dd=$atag;
								}
								else
								{
									if($stag >= $cday)
										$tc='blue';
									else
										$tc='black';
									if($zm == 2)
									{
										$GLOBALS['fxptdata']['lvalues'][$mc][$z]['tage']='~<span class="'.$tc.'">'.$monate[$m].' '.$y.'</span>';
										$dd=substr('0'.$m,-2).$y;
									}
									else
									{
										$GLOBALS['fxptdata']['lvalues'][$mc][$z]['tage']='~<span class="'.$tc.'">'.$y.'</span>';
										$dd=$y;
									}
									$GLOBALS['fxptdata']['lvalues'][$mc][$z]['datum']='~<span class="'.$tc.'">'.get_entry($stag, 'datum').'-'.get_entry($ltag, 'datum').'</span>';
								}
							}
							$GLOBALS['fxptdata']['lvalues'][$mc][$z]['zeit_id']='~';

							// Projekt
							$tla=array();
							if(!$hm && fxIsArray($fra['tasks']))
							{
								$tlc=sizeof($tla);
								$tad='';
								if(sizeof($da))
									$tad='+&nbsp;';
								foreach($fra['tasks'] as $frpid => $frpa)
								{
									if(!isset($da[$frpid]))
									{
										$tla[$tlc]  = '<table width=100% border=0 cellpadding=0 cellspacing=0>';
										$tla[$tlc] .= '	<tr>';
										$tla[$tlc] .= '		<td width="100%"><span class="'.$pc.'">'.$tad.no_null($frpa['pno']).'&nbsp;'.$frpa['pname'].'</span></td>';
										$tla[$tlc] .= '		<td nowrap align="right">&nbsp;&nbsp;'.getH($frpa['phours']*3600, 's2 '.$pc, 's2').'</td>';
										$tla[$tlc] .= '	</tr>';
										$tla[$tlc] .= '</table>';

										$tlc++;
									}
								}
								if(isset($tla[0]))
									$tl=$tla[0];
								else
									$tl='+';
							}
							else if($hm == 1)
								$tl='<i class="'.$pc.'">'.$keineze.'</i>';
							else if($hm > 1)
								$tl='<i class="'.$pc.'">'.$keineaufgaben.'</i>';
							$GLOBALS['fxptdata']['lvalues'][$mc][$z]['projekt']			= '~'.$tl;

							// Stunden, EK, VK, etc.
							$GLOBALS['fxptdata']['lvalues'][$mc][$z]['sollstunden']		= '~'.getH($eh,$fc);
							$GLOBALS['fxptdata']['lvalues'][$mc][$z]['iststunden']		= '~'.getH($ch,$fc);
							$GLOBALS['fxptdata']['lvalues'][$mc][$z]['delta']			= '~'.getH($ch-$eh,$fc);
							$GLOBALS['fxptdata']['lvalues'][$mc][$z]['ek']				= '~'.getCH($ek,$fc);
							$GLOBALS['fxptdata']['lvalues'][$mc][$z]['kosten']			= '~'.getC($net,$lc);
							$GLOBALS['fxptdata']['lvalues'][$mc][$z]['mwst']			= '~'.getC($vat,$lc);
							$GLOBALS['fxptdata']['lvalues'][$mc][$z]['bruttobetrag']	= '~'.getC($grs,$lc);
							$GLOBALS['fxptdata']['lvalues'][$mc][$z]['ver_std']			= '~'.getH($ih,$lc);
							$GLOBALS['fxptdata']['lvalues'][$mc][$z]['vk']				= '~'.getCH($vk,$fc);
							$GLOBALS['fxptdata']['lvalues'][$mc][$z]['ertrag']			= '~'.getC($inv,$lc);
							$GLOBALS['fxptdata']['lvalues'][$mc][$z]['marge_verbr']		= '~'.getC($inv-$net,$lc);
							$GLOBALS['fxptdata']['lvalues'][$mc][$z]['freigabe']		= '~';
							$GLOBALS['fxptdata']['lvalues'][$mc][$z]['verrechnebar']	= '~';
							$GLOBALS['fxptdata']['lvalues'][$mc][$z]['abgerechnet']		= '~';
							$GLOBALS['fxptdata']['lvalues'][$mc][$z]['link']			= '~';

							$pers_mon_sstd		+= $eh;
							$pers_mon_istd		+= $ch;
							$pers_mon_vstd		+= $ih;
							$pers_mon_gek		+= $net;
							$pers_mon_gvk		+= $inv;
							$pers_mon_mwst		+= $vat;
							$pers_mon_brutto	+= $grs;

							$pers_ges_sstd		+= $eh;
							$pers_ges_istd		+= $ch;
							$pers_ges_vstd		+= $ih;
							$pers_ges_gek		+= $net;
							$pers_ges_gvk		+= $inv;
							$pers_ges_mwst		+= $vat;
							$pers_ges_brutto	+= $grs;

							$entries++;
							$ec++;
							$ac++;
							$z++;

							if(sizeof($tla) > 1)
							{
								foreach($tla as $tlc => $tl)
								{
									if($tlc)
									{
										$GLOBALS['fxptdata']['lvalues'][$mc][$z]['tage']			= '~';
										$GLOBALS['fxptdata']['lvalues'][$mc][$z]['datum']			= '~';
										$GLOBALS['fxptdata']['lvalues'][$mc][$z]['zeit_id']			= '~';
										$GLOBALS['fxptdata']['lvalues'][$mc][$z]['projekt']			= '~'.$tl;
										$GLOBALS['fxptdata']['lvalues'][$mc][$z]['sollstunden']		= '~';
										$GLOBALS['fxptdata']['lvalues'][$mc][$z]['iststunden']		= '~';
										$GLOBALS['fxptdata']['lvalues'][$mc][$z]['delta']			= '~';
										$GLOBALS['fxptdata']['lvalues'][$mc][$z]['ek']				= '~';
										$GLOBALS['fxptdata']['lvalues'][$mc][$z]['kosten']			= '~';
										$GLOBALS['fxptdata']['lvalues'][$mc][$z]['mwst']			= '~';
										$GLOBALS['fxptdata']['lvalues'][$mc][$z]['bruttobetrag']	= '~';
										$GLOBALS['fxptdata']['lvalues'][$mc][$z]['ver_std']			= '~';
										$GLOBALS['fxptdata']['lvalues'][$mc][$z]['vk']				= '~';
										$GLOBALS['fxptdata']['lvalues'][$mc][$z]['ertrag']			= '~';
										$GLOBALS['fxptdata']['lvalues'][$mc][$z]['marge_verbr']		= '~';
										$GLOBALS['fxptdata']['lvalues'][$mc][$z]['freigabe']		= '~';
										$GLOBALS['fxptdata']['lvalues'][$mc][$z]['verrechnebar']	= '~';
										$GLOBALS['fxptdata']['lvalues'][$mc][$z]['abgerechnet']		= '~';
										$GLOBALS['fxptdata']['lvalues'][$mc][$z]['link']			= '~';

										$z++;
									}
								}
							}
						}
					}
				}

				// Einen Tag hochzählen
				$ny=$y;
				$nm=$m;
				$nd=$d+1;
				if($nd > $mo_tage[$nm])
				{
					if($zm == 1)
						$ae=true;
					$nd=1;
					$nm++;
					if($nm > 12)
					{
						if($zm == 2)
							$ae=true;
						$nm=1;
						$ny++;
						$lj=fx_date('L', fx_mktime(0, 0, 0, 1, 1, $ny));
						if($lj)
							$mo_tage[2]=29;
						else
							$mo_tage[2]=28;
					}
				}
				if(($zm < 3) && ($atag == $etag))
					$ae=true;
//echo('-&gt; $ae='.$ae.', $ac='.$ac.'<br />');

				if($ae && $ac)
				{
					$ac=0;

					// Abschnittszwischensumme
					if(!$sort_travel)
					{
						$pzsa[]=$z;

						$sline=true;

						fxf_maskColumnBorder($mca, 3, -1, $mc, $z, true);
						$GLOBALS['maske231_as'.$z]='padding-top:3px;padding-bottom:3px;font-weight:bold;';
						$cls1='^col_e2e2e2';
						$cls2='^col_ececec';

						$name=trim((string)$pers['vorname']);
						if(strlen((string)$name))
							$name=',&nbsp;'.$name;
						$name=trim((string)$pers['pname']).$name;

						$zs='&nbsp;<b>'.$zwischensumme.':</b> '.$name.' - ';
						if($zm == 1)
							$zs .= $monate[$m].' ';
						$zs .= $y;

						$ek=0.0;
						$vk=0.0;
						if($pers_mon_istd)
							$ek=$pers_mon_gek*3600/$pers_mon_istd;
						if($pers_mon_vstd)
							$vk=$pers_mon_gvk*3600/$pers_mon_vstd;

						if($scolspan > 1)
						{
							$GLOBALS['fxptdata']['lvalues'][$mc][$z][$fcol]			= $cls1.'" nowrap colspan="'.$scolspan.'" style="border-top:1px solid #'.$GLOBALS['msk_nbcol'].';border-right:1px solid #'.$GLOBALS['msk_nbcol'].';"><!--|~'.$zs;
							$GLOBALS['fxptdata']['lvalues'][$mc][$z][$slcol]		= '~-->'.$zs;
						}
						else
							$GLOBALS['fxptdata']['lvalues'][$mc][$z][$fcol]			= $cls1.'|~'.$zs;

						$GLOBALS['fxptdata']['lvalues'][$mc][$z]['sollstunden']		= $cls1.'|~'.getH($pers_mon_sstd);
						$GLOBALS['fxptdata']['lvalues'][$mc][$z]['iststunden']		= $cls1.'|~'.getH($pers_mon_istd);
						$GLOBALS['fxptdata']['lvalues'][$mc][$z]['delta']			= $cls1.'|~'.getH($pers_mon_istd-$pers_mon_sstd);
						$GLOBALS['fxptdata']['lvalues'][$mc][$z]['ek']				= $cls1.'|~'.getCH($ek);
						$GLOBALS['fxptdata']['lvalues'][$mc][$z]['kosten']			= $cls1.'|~'.getC($pers_mon_gek);
						$GLOBALS['fxptdata']['lvalues'][$mc][$z]['mwst']			= $cls1.'|~'.getC($pers_mon_mwst);
						$GLOBALS['fxptdata']['lvalues'][$mc][$z]['bruttobetrag']	= $cls1.'|~'.getC($pers_mon_brutto);
						$GLOBALS['fxptdata']['lvalues'][$mc][$z]['ver_std']			= $cls1.'|~'.getH($pers_mon_vstd);
						$GLOBALS['fxptdata']['lvalues'][$mc][$z]['vk']				= $cls1.'|~'.getCH($vk);
						$GLOBALS['fxptdata']['lvalues'][$mc][$z]['ertrag']			= $cls1.'|~'.getC($pers_mon_gvk);
						$GLOBALS['fxptdata']['lvalues'][$mc][$z]['marge_verbr']		= $cls1.'|~'.getC($pers_mon_gvk-$pers_mon_gek);
						$GLOBALS['fxptdata']['lvalues'][$mc][$z]['freigabe']		= $cls2.'|';
						$GLOBALS['fxptdata']['lvalues'][$mc][$z]['verrechenbar']	= $cls2.'|';
						$GLOBALS['fxptdata']['lvalues'][$mc][$z]['abgerechnet']		= $cls2.'|';
						$GLOBALS['fxptdata']['lvalues'][$mc][$z]['link']			= $cls2.'|';
						$z++;

						$sline=false;

						$pers_mon_sstd=0;
						$pers_mon_istd=0;
						$pers_mon_vstd=0;
						$pers_mon_gek=0.0;
						$pers_mon_gvk=0.0;
						$pers_mon_mwst=0.0;
						$pers_mon_brutto=0.0;
					}
				}

				$wd++;
				if($wd > 6)
					$wd=0;

				if(($zm == 1) && ($wd == $fd))
					$aw++;

				$y=$ny;
				$m=$nm;
				$d=$nd;
				$atag=substr('0000'.$y, -4).substr('00'.$m, -2).substr('00'.$d, -2);
//echo('-&gt; $atag='.$atag.' -- $d='.$d.', $m='.$m.', $y='.$y.' -- $ae='.$ae.', $ac='.$ac.'<br />');
			}

			if($entries || $leer_anzeigen)
			{
				if(sizeof($pzsa) == 1)
				{
//echo('<b>DEL</b> $pzsa: '.$pzsa[0].'-'.$z.'<hr />');
					while($z >= $pzsa[0])
					{
						unset($GLOBALS['fxptdata']['lvalues'][$mc][$z]);
						$z--;
					}
					$z=$pzsa[0];
				}

				// Gesamtsumme je Person
				$sline=true;

				$GLOBALS['msk_nbcol_231_'.$z]=$GLOBALS['msk_tbcol'];
				fxf_maskColumnBorder($mca, 3, -1, $mc, $z, true);
				$GLOBALS['maske231_as'.$z]='padding-top:6px;padding-bottom:6px;font-weight:bold;';
				$cls1='^col_d3e0e6';
				$cls2='^col_e2f0f4';

				$name=trim((string)$pers['vorname']);
				if(strlen((string)$name))
					$name=',&nbsp;'.$name;
				$name=trim((string)$pers['pname']).$name;

				$zs='&nbsp;<b>'.$gesamtsumme.':</b> '.$name;

				$ek=0.0;
				$vk=0.0;
				if($pers_ges_istd)
					$ek=$pers_ges_gek*3600/$pers_ges_istd;
				if($pers_ges_vstd)
					$vk=$pers_ges_gvk*3600/$pers_ges_vstd;

				if($scolspan > 1)
				{
					$GLOBALS['fxptdata']['lvalues'][$mc][$z][$fcol]			= $cls1.'" nowrap colspan="'.$scolspan.'" style="border-top:1px solid #'.$GLOBALS['msk_tbcol'].';border-right:1px solid #'.$GLOBALS['msk_tbcol'].';"><!--|~'.$zs;
					$GLOBALS['fxptdata']['lvalues'][$mc][$z][$slcol]		= '~-->'.$zs;
				}
				else
					$GLOBALS['fxptdata']['lvalues'][$mc][$z][$fcol]			= $cls1.'|~'.$zs;

				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['sollstunden']		= $cls1.'|~'.getH($pers_ges_sstd);
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['iststunden']		= $cls1.'|~'.getH($pers_ges_istd);
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['delta']			= $cls1.'|~'.getH($pers_ges_istd-$pers_ges_sstd);
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['ek']				= $cls1.'|~'.getCH($ek);
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['kosten']			= $cls1.'|~'.getC($pers_ges_gek);
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['mwst']			= $cls1.'|~'.getC($pers_ges_mwst);
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['bruttobetrag']	= $cls1.'|~'.getC($pers_ges_brutto);
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['ver_std']			= $cls1.'|~'.getH($pers_ges_vstd);
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['vk']				= $cls1.'|~'.getCH($vk);
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['ertrag']			= $cls1.'|~'.getC($pers_ges_gvk);
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['marge_verbr']		= $cls1.'|~'.getC($pers_ges_gvk-$pers_ges_gek);
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['freigabe']		= $cls2.'|';
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['verrechenbar']	= $cls2.'|';
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['abgerechnet']		= $cls2.'|';
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['link']			= $cls2.'|';
				$z++;

				$sline=false;

				// Gesamtsumme=Summe aller Personensummen
				$total_sstd			+= $pers_ges_sstd;
				$total_istd			+= $pers_ges_istd;
				$total_vstd			+= $pers_ges_vstd;
				$total_gek			+= $pers_ges_gek;
				$total_gvk			+= $pers_ges_gvk;
				$total_mwst			+= $pers_ges_mwst;
				$total_brutto		+= $pers_ges_brutto;

				$dpc++;
			}
			else if($z > $pz)
			{
				while($z >= $pz)
				{
					if(fxIsArray($GLOBALS['fxptdata']['lvalues'][$mc][$z]))
						unset($GLOBALS['fxptdata']['lvalues'][$mc][$z]);
					$z--;
				}
				$z=$pz;
			}
		}

		// Gesamtsumme bei mehreren Personen
		if($dpc > 1)
		{
			// Leerzeile?
			if($leerzeile != $z-1)
			{
				$leerzeile=$z;
				$GLOBALS['msk_nbcol_231_'.$z]=$GLOBALS['msk_sbcol'];
				fxf_maskColumnBorder($mca, 1, -1, $mc, $z, true);
				$z++;
			}

			// Alle Mitarbeiter (über gesamte Zeile)
			$txt='&nbsp;'.meldung(2141,true);
			fxf_listSectHeader($mc, $z, $txt, $fcol, $tlcol, $colspan, 'g');
			$z++;

			$sline=true;

			$GLOBALS['msk_nbcol_231_'.$z]=$GLOBALS['msk_tbcol'];
			fxf_maskColumnBorder($mca, 2, -1, $mc, $z, true);
			$GLOBALS['maske231_as'.$z]='padding-top:9px;padding-bottom:9px;font-weight:bold;';
			$cls1='^col_d3e6e0';
			$cls2='^col_e2f4f0';

			$zs='&nbsp;<b>'.$gesamtsumme.':</b> '.get_entry($von,'datum').'-'.get_entry($bis,'datum');

			$ek=0.0;
			$vk=0.0;
			if($total_istd)
				$ek=$total_gek*3600/$total_istd;
			if($total_vstd)
				$vk=$total_gvk*3600/$total_vstd;

			if($scolspan > 1)
			{
				$GLOBALS['fxptdata']['lvalues'][$mc][$z][$fcol]			= $cls1.'" nowrap colspan="'.$scolspan.'" style="border-right:1px solid #'.$GLOBALS['msk_tbcol'].';"><!--|~'.$zs;
				$GLOBALS['fxptdata']['lvalues'][$mc][$z][$slcol]		= '~-->'.$zs;
			}
			else
				$GLOBALS['fxptdata']['lvalues'][$mc][$z][$fcol]			= $cls1.'|~'.$zs;

			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['sollstunden']		= $cls1.'|~'.getH($total_sstd);
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['iststunden']		= $cls1.'|~'.getH($total_istd);
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['delta']			= $cls1.'|~'.getH($total_istd-$total_sstd);
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['ek']				= $cls1.'|~'.getCH($ek);
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['kosten']			= $cls1.'|~'.getC($total_gek);
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['mwst']			= $cls1.'|~'.getC($total_mwst);
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['bruttobetrag']	= $cls1.'|~'.getC($total_brutto);
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['ver_std']			= $cls1.'|~'.getH($total_vstd);
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['vk']				= $cls1.'|~'.getCH($vk);
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['ertrag']			= $cls1.'|~'.getC($total_gvk);
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['marge_verbr']		= $cls1.'|~'.getC($total_gvk-$total_gek);
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['freigabe']		= $cls2.'|';
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['verrechenbar']	= $cls2.'|';
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['abgerechnet']		= $cls2.'|';
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['link']			= $cls2.'|';
			$z++;

			$sline=false;
		}

		if($z)
		{
			$GLOBALS['msk_nbcol_231_'.$z]=$GLOBALS['msk_sbcol'];
			fxf_maskColumnBorder($mca, 1, -1, $mc, $z, true);
			$z++;
		}
	}
}


////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/**
 * Set columns "Rel.", "Clear" + "Inv."
 *
 * @param array $a - Mandatory parameter:  Value array
 */
function setzeFVA($a)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Frei. (Freigabe)
	$GLOBALS['fxptdata']['lvalues'][$GLOBALS['mc']][$GLOBALS['z']]['freigabe']='~'.str_replace('{tt}', $GLOBALS['itt'][$a['freigabe']], $GLOBALS['img_frei'][$a['freigabe']]);
	// Verr. (Verrechenbar)
	$GLOBALS['fxptdata']['lvalues'][$GLOBALS['mc']][$GLOBALS['z']]['verrechenbar']='~'.str_replace('{tt}', $GLOBALS['itt'][3+$a['n_verr']], $GLOBALS['img_verr'][$a['n_verr']]);
	// Abg. (Abgerechnet)
	$ai=8+$a['abger'];
	$tt='';
	if(sizeof($a['rid']))
	{
		$tt .= '<ul>';
		foreach($a['rid'] as $rid => $rc)
		{
			if(isset($GLOBALS['rnra'][$rid]))
				$tt .= '<li>'.$GLOBALS['rnra'][$rid].'</li>';
			else
				$tt .= '<li><i class=grey>('.$rid.')</li>';
		}
		$tt .= '</ul>';
	}
	else if($ai != 11)
	{
		if(!$a['n_verr'] || ($a['n_verr'] == 2))
			$ai=9;
		else
			$ai=8;
	}
	$tt=$GLOBALS['itt'][$ai].$tt;
	$GLOBALS['fxptdata']['lvalues'][$GLOBALS['mc']][$GLOBALS['z']]['abgerechnet']='~'.str_replace('{tt}', $tt, $GLOBALS['img_abger'][$ai-8]);
}

/**
 * Set columns "Day" + "Date"
 */
function setzeTag()
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Farbe
	$cls='';
	$tgb='';
	if($GLOBALS['usktage'][$GLOBALS['atag']] || $GLOBALS['hdays'][$GLOBALS['atag']])		// Sondertag oder Feiertag
	{
		if($GLOBALS['hdays'][$GLOBALS['atag']])
		{
			if($GLOBALS['hdays'][$GLOBALS['atag']]['art'] != FXP_HL_DISPLAY)
				$cls='red';
			$tgb .= $GLOBALS['hdays'][$GLOBALS['atag']]['name'].$GLOBALS['fart'][$GLOBALS['hdays'][$GLOBALS['atag']]['art']];
		}
		if($GLOBALS['usktage'][$GLOBALS['atag']])
		{
			$usk_wtag=(int)$GLOBALS['usktage'][$GLOBALS['atag']][-1]['typ'];
			if(!strlen((string)$cls))
				$cls='green';
			foreach($GLOBALS['usktage'][$GLOBALS['atag']] as $usktyp => $uskdata)
			{
				if($usktyp == -1)
					continue;

				if(strlen((string)$tgb))
					$tgb .= '<br />';
				$tgb .= $GLOBALS['usk'][$usktyp];
				if($uskdata[-1]['dauer'] == 0.5)
					$tgb .= ' [½]';
				else
					$tgb .= ' [1]';
			}
		}
	}
	if(!strlen((string)$cls))
	{
		if(!$GLOBALS['wd'] || ($GLOBALS['wd'] > 5))					// Wochenende
		{
			if($GLOBALS['atag'] >= $GLOBALS['tday'])
				$cls='lightblue';
			else
				$cls='grey';
		}
		else if($GLOBALS['atag'] >= $GLOBALS['tday'])
			$cls='blue';
		else
			$cls='black';
	}

	$tti='';
	if(strlen((string)$tgb))
	{
		$tti='&nbsp;<img src="'.$GLOBALS['gfxpath'].'tts.png" align="top">';
		$cls .= '" style="cursor:help;';
	}

	$GLOBALS['fxptdata']['lvalues'][$GLOBALS['mc']][$GLOBALS['z']]['tage']='~<span class="'.$cls.'">'.$GLOBALS['wochentage'][$GLOBALS['wd']].'</span>';
	$GLOBALS['fxptdata']['lvalues'][$GLOBALS['mc']][$GLOBALS['z']]['datum']='~<span class="'.$cls.'" tooltip="'.$tgb.'">'.get_entry($GLOBALS['atag'], 'datum').$tti.'</span>';
}

/**
 * Format a currency value
 *
 * @param float  $val - Mandatory parameter:                Float value
 * @param string $cls - Optional parameter (default = ''):  Display class
 *
 * @return Formatted value
 */
function getC($val, $cls='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$sum=false;
	if(isset($GLOBALS['sline']) && $GLOBALS['sline'])
		$sum=true;

	if(!strlen((string)$cls))
	{
		if($val < 0.0)
			$cls='red';
		else if(($val > 0.0) || $sum)
			$cls='black';
		else
			$cls='grey';
	}
	else if(!$sum && !$val && (substr((string)$cls,0,5) != 'light'))
		$cls='light'.$cls;

	return '<span class="'.$cls.'">'.get_entry($val,'dezimal').$GLOBALS['unit_c'].'</span>';
}

/**
 * Format a hour value
 *
 * @param float  $val  - Mandatory parameter:                Float value
 * @param string $cls  - Optional parameter (default = ''):  Display class
 * @param string $ecls - Optional parameter (default = ''):  Float format
 *
 * @return Formatted value
 */
function getH($val, $cls='', $ecls='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$sum=false;
	if(isset($GLOBALS['sline']) && $GLOBALS['sline'])
		$sum=true;

	if(!strlen((string)$cls))
	{
		if(($val > 0.0) || $sum)
			$cls='black';
		else
			$cls='grey';
	}
	else if(!$sum && ($val <= 0.0) && (substr((string)$cls,0,5) != 'light'))
		$cls='light'.$cls;

	return '<span class="'.$cls.'">'.get_entry($val,'zeitspanne','',$ecls).$GLOBALS['unit_h'].'</span>';
}

/**
 * Format a currency/hour value
 *
 * @param float  $val - Mandatory parameter:                Float value
 * @param string $cls - Optional parameter (default = ''):  Display class
 *
 * @return Formatted value
 */
function getCH($val, $cls='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$sum=false;
	if(isset($GLOBALS['sline']) && $GLOBALS['sline'])
		$sum=true;

	if(!strlen((string)$cls))
	{
		if(($val > 0.0) || $sum)
			$cls='black';
		else
			$cls='grey';
	}
	else if(!$sum && ($val <= 0.0) && (substr((string)$cls,0,5) != 'light'))
		$cls='light'.$cls;

	return '<span class="'.$cls.'">'.get_entry($val,'dezimal').$GLOBALS['unit_ch'].'</span>';
}
?>