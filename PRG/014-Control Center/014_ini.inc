<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : tr14_ini.inc                                                 //
// Version     : 21.2                                                         //
// Begin       : 2020-11-04                                                   //
// Last Change : 2021-03-10                                                   //
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
 * PF 14: Control Center
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

$today=$GLOBALS['datetime'];
$dt_icons=getDocIcons();

// User name
if($GLOBALS['fxpglobals']['user'] > 1)
{
	$sql  = "SELECT p.pname, p.vorname FROM benutzer b, personen p";
	$sql .= " WHERE b.personen_id=p.personen_id AND b.mandanten_id=p.mandanten_id AND b.benutzer_id=".$GLOBALS['fxpglobals']['user']." AND b.mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$barray=db_values($sql);
	// Falls aus der Datenbank der Name nicht gefunden wurde wird 'Unbekannt' ausgegeben
	if(fxIsArray($barray))
	{
		$benutzername=trim($barray['pname']);
		$vorname=trim($barray['vorname']);
		if(strlen($vorname))
			$benutzername .= ', '.$vorname;
	}
	else
		$benutzername=meldung(681, true);	// Unbekannt
}
else
	$benutzername='Administrator';

// Mandantenname ermitteln
$sql="SELECT pname FROM personen WHERE personentyp=274 AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
$mandantenname=db_value($sql);

// Heutiges Datum ermitteln
$wochentag=array(691, 685, 686, 687, 688, 689, 690);
$datum=meldung($wochentag[(int) fx_date('w')]).',&nbsp;'.(string)(formatdatetime($GLOBALS['fxpglobals']['settings']['formatdatum'], fx_date("Ymd000000")));
$Head=meldung(1431, 0, '<b>"'.$benutzername.'"</b>&nbsp;<i>['.$mandantenname.']</i>&nbsp;', $datum);

// Kalenderwerte leeren
$GLOBALS['fxptdata']['cal']=array();


//////////////////////////////////////////////////////////////////////////
// NEWSTICKER															//
//////////////////////////////////////////////////////////////////////////
$ntwidth=982;
$sql  = "SELECT termin_id, personen_id, projekt_id, startzeit, endzeit, terminart, name_termin, beschreibung, treffpunkt, person_angelegt";
$sql .= " FROM termine";
$sql .= " WHERE mandanten_id=".$GLOBALS['fxpglobals']['client']." AND (erledigt=0 OR erledigt IS NULL) AND (archiv=0 OR archiv IS NULL)";
$sql .= " AND terminart=".FXP_AT_MESSAGE;
$sql .= " AND startzeit<='".substr($today,0,8)."240000' AND endzeit>='".substr($today,0,8)."000000'";
$sql .= " ORDER BY startzeit";
$ntermine=db_values($sql,'*');
if(fxIsArray($ntermine))
{
//fxDebug($ntermine,$sql);
	$ntpa=array();
	$ntna=array();
	$ncnt=0;
	$ntext='<marquee id="marquee" class="fld" behavior="scroll" scrollamount="3" scrolldelay="0" style="width:'.$ntwidth.'px;height:20px;padding-top:6px;">';
	foreach($ntermine as $nt)
	{
		// Check rights
		$ntrights=false;
		// ...Own
		if(!$ntrights && ($nt['personen_id'] == $GLOBALS['fxpglobals']['person']))
			$ntrights=true;
		// ...GM or Admin
		if(!$ntrights && ($GLOBALS['fxpglobals']['persdat']['role'] >= 2))
			$ntrights=true;
		// Project rights: Project manager or project leaders
		if(!$ntrights && ($nt['projekt_id'] > 0))
		{
			$sql="SELECT projektmanager, projektleiter, vertreter FROM projekte WHERE projekt_id=".$nt['projekt_id']." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$tmp=db_values($sql);
//fxDebug($tmp,$sql);
			if(fxIsArray($tmp) && (($tmp['projektmanager'] == $GLOBALS['fxpglobals']['person']) || ($tmp['projektleiter'] == $GLOBALS['fxpglobals']['person']) || ($tmp['vertreter'] == $GLOBALS['fxpglobals']['person'])))
				$ntrights=true;
		}
		// Invitations
		if(!$ntrights)
		{
			$sql="SELECT personen_id FROM termine WHERE mastertermin_id=".$nt['termin_id']." AND termin_id<>".$nt['termin_id']." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$tmp=db_values($sql,'*');
//fxDebug($tmp,$sql);
			if(fxIsArray($tmp))
			{
				foreach($tmp as $t)
				{
					if($t['personen_id'] == $GLOBALS['fxpglobals']['person'])
					{
						$ntrights=true;
						break;
					}
				}
			}
		}

		if($ntrights)
		{
			if($ncnt)
				$ntext .= '&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font class="lightergrey">|</font>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;';
			$ntext .= '<span id="msg'.$nt['termin_id'].'" onmouseover="$(\'marquee\').scrollAmount=0;" onmouseout="$(\'marquee\').scrollAmount=3;" style="cursor:default;"><font class="grey">'.get_entry($nt['startzeit'], 'datetime');
			if($nt['endzeit'] > $nt['startzeit'])
				$ntext .= '-'.get_entry($nt['endzeit'], 'zeit');
			$ntext .= '</font>&nbsp;&nbsp;';
			if($nt['personen_id'] != $GLOBALS['fxpglobals']['person'])
			{
				if(!isset($ntna[$nt['personen_id']]))
				{
					$sql="SELECT vorname, pname FROM personen WHERE personen_id=".$nt['personen_id']." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
					$tmp=db_values($sql);
//fxDebug($tmp,$sql);
					if(fxIsArray($tmp))
					{
						$name=$tmp['pname'];
						$vnam=$tmp['vorname'];
						if(strlen($vnam))
							$name=$vnam.'&nbsp;'.$name;
						$ntna[$nt['personen_id']]='<font class="darkgrey">'.$name.'</font>&nbsp;<font class="s4 lightgrey">&rarr;</font>&nbsp;';
					}
				}
				if(isset($ntna[$nt['personen_id']]))
					$ntext .= $ntna[$nt['personen_id']];
			}
			if($nt['projekt_id'] > 0)
			{
				if(!isset($ntpa[$nt['projekt_id']]))
				{
					$sql="SELECT vorgangsnummer, name_projekt FROM projekte WHERE projekt_id=".$nt['projekt_id']." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
					$tmp=db_values($sql);
//fxDebug($tmp,$sql);
					if(fxIsArray($tmp))
						$ntpa[$nt['projekt_id']]='<font class="blue">['.no_null($tmp['vorgangsnummer']).'&nbsp;'.$tmp['name_projekt'].']</font>&nbsp;';
				}
				if(isset($ntpa[$nt['projekt_id']]))
					$ntext .= $ntpa[$nt['projekt_id']];
			}
			$ntext .= $nt['name_termin'];
			if(strlen(trim($nt['beschreibung'])))
				$ntext .= ':&nbsp;<font class="lightgrey">'.trim($nt['beschreibung']).'</font>';
			$ntext .= '</span>';
			$ncnt++;
		}
	}
	$ntext .= '</marquee>';
	if($ncnt)
		echo(std_mask('News Ticker', $ntext, $ntwidth));
}

//////////////////////////////////////////////////////////////////////////
// AKTUELLE PROJEKTE													//
//////////////////////////////////////////////////////////////////////////
$fehler					= false;			// Fehlervariable, falls Fehler auftritt
$_POST['ZProjekte']		= 1;				// Projekte anzeigen
$_POST['nur_plandaten']	= '';				// Ist-Daten integrieren
$_POST['ZAuslastung']	= 1;				// Auslastung auch
$_POST['atyp_akt']		= 1;				// Aktive Aufgabe anzeigen

// Farblegende-Infos holen (Texte, Farbnamen, etc.)
if(!is_array($GLOBALS['fxptdata']['backup']['legend'.$GLOBALS['fxpglobals']['lang']]))
	farblegende('vars');
extract($GLOBALS['fxptdata']['backup']['legend'.$GLOBALS['fxpglobals']['lang']]);

// Personeninfos ermitteln
$personenid	= $GLOBALS['fxpglobals']['person'];
$personentyparray = array(275=>get_text(1225, '5'), 840=>get_text(1229, '5'));	// Int. / Ext.

// Startdatum = Montag der aktuellen Woche
$von = $today;

$wd = getWeekday($von);
if($wd != 1)
{
	if(!$wd)
		$von = fx_date('Ymd', fx_mktime(12, 0, 0, (int)substr($von, 4, 2), (int)substr($von, 6, 2)-6, (int)substr($von, 0, 4))).'000000';
	else
		$von = fx_date('Ymd', fx_mktime(12, 0, 0, (int)substr($von, 4, 2), (int)substr($von, 6, 2)-$wd+1, (int)substr($von, 0, 4))).'000000';
}
else
	$von = substr($von, 0, 8).'000000';

// Enddatum = Sonntag in x Wochen (laut Default: 4)
$bis = fx_date('Ymd240000', fx_mktime(12, 0, 0, (int)substr($von, 4, 2), (int)substr($von, 6, 2)+$GLOBALS['fxpglobals']['settings']['stdzeitspanne']-1, (int)substr($von, 0, 4)));

$personen = array();
$ilen = 0;
$zeile = 2;  // 0: Monat, 1: KW, 2: Tag

// 1. Schritt:	Personendaten der Person auslesen (Name etc.)
$sql  = "SELECT";
$sql .= " mandanten_id, pname, vorname, personentyp, geschlecht ";
$sql .= "FROM";
$sql .= " personen ";
$sql .= "WHERE";
$sql .= "  personen_id=".$personenid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
$tmp  = db_values($sql);
//fxDebug($tmp, $sql);
if(fxIsArray($tmp))
{
	$i = $personenid;
	$ma_zeile[$i] = ++$zeile;

	$personen[$i]['id'] = $i;
	$personen[$i]['fullname'] = $tmp['pname'];
	$vn = trim($tmp['vorname']);
	if(strlen($vn))
		$personen[$i]['fullname'] .= ',&nbsp;'.$vn;
	$personen[$i]['aktiv']	= true;
	$personen[$i]['paktiv']	= 0;
	$personen[$i]['ptyp']	= $tmp['personentyp'];
	$personen[$i]['sex']	= $tmp['geschlecht'];
	$personen[$i]['tage']	= 0;

	// 2. Schritt:	Zusatzdaten der Person auslesen (Arbeitstage etc.)
	$sql  = "SELECT";
	$sql .= " mandanten_id, personen_id, arbeitstage, wochenarbeitstd, maeintrittsdatum, maaustrittsdatum, soll_beginn AS startzeit, pause,";
	$sql .= " sonntag AS a, montag AS b, dienstag AS c, mittwoch AS d, donnerstag AS e, freitag AS f, samstag AS g ";
	$sql .= "FROM";
	$sql .= " madaten ";
	$sql .= "WHERE";
	$sql .= " personen_id=".$personenid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$tmp  = db_values($sql);
//fxDebug($tmp, $sql);
	if(is_array($tmp))
	{
		$ilen = 1;

		for($wtc=0; $wtc<7; $wtc++)
		{
			if($tmp[chr(0x61 + $wtc)])
				$personen[$i][$wtc] = 1;
			else
				$personen[$i][$wtc] = 0;
		}

		// Mandanten-ID hinzufügen
		$personen[$i]['mandanten_id']		= $tmp['mandanten_id'];
		// Arbeitstage hinzufügen
		$personen[$i]['arbeitstage']		= 0;
		$personen[$i]['wochenarbeitstd']	= 0.00;
		$personen[$i]['startzeit']			= '';
		$personen[$i]['pause']				= 0;
		$personen[$i]['eintritt']			= '00000000000000';
		$personen[$i]['austritt']			= '';

		if($tmp['arbeitstage'])
			$personen[$i]['arbeitstage'] = $tmp['arbeitstage'];
		if($tmp['wochenarbeitstd'])
			$personen[$i]['wochenarbeitstd'] = $tmp['wochenarbeitstd'];
		$personen[$i]['startzeit'] = $tmp['startzeit'];
		$personen[$i]['pause'] = $tmp['pause'];
		if(strlen($tmp['maeintrittsdatum']))
			$personen[$i]['eintritt'] = $tmp['maeintrittsdatum'];
		if(strlen($tmp['maaustrittsdatum']))
			$personen[$i]['austritt'] = $tmp['maaustrittsdatum'];

		$personen[$i]['aufwand_sek'] = 0.00;
		$personen[$i]['prozent'] = 0.00;
		$personen[$i]['d_aufw'] = false;

		$personen[$i]['maus']['sstd']		= 0.00;
		$personen[$i]['maus']['gstd']		= 0.00;
		$personen[$i]['maus']['astd_ist']	= 0.00;
		$personen[$i]['maus']['astd_ges']	= 0.00;
		$personen[$i]['maus']['astd_term']	= 0.00;
		$personen[$i]['maus']['astd_akt']	= 0.00;
		$personen[$i]['maus']['astd_gen']	= 0.00;
		$personen[$i]['maus']['astd_gepl']	= 0.00;

		if($personen[$i]['arbeitstage'] > 0)
			$personen[$i]['maus']['sstd'] = $personen[$i]['wochenarbeitstd']/$personen[$i]['arbeitstage'];

		// 3. Schritt:	Aktive Aufgaben der Person (ohne Wartungsaufgaben) auslesen
		$sql  = "SELECT";
		$sql .= " m.personen_id, m.projekt_id, m.auslastung, m.aufwand_std AS aufwand_sek, m.prozent, m.soll_beginn, m.soll_ende,";
		$sql .= " p.mutterprojekt_id, p.vorgangsnummer, p.name_projekt, p. prioritaet, p.projekt_status, p.beschreibung,";
		$sql .= " 0 AS ist_sek, 0 AS fgrad, 0 AS atyp, 1 AS disp, b.budget_pers_int_gepl, budget_pers_int_gen ";
		$sql .= "FROM";
		$sql .= " maplanung m, projekte p, budget_summe b ";
		$sql .= "WHERE";
		$sql .= " m.personen_id=".$personenid." AND m.mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$sql .= " AND p.projekt_id=m.projekt_id AND p.mandanten_id=m.mandanten_id AND p.projektart=".FXP_PRJ_TASK;
		$sql .= " AND b.projekt_id=m.projekt_id AND b.mandanten_id=m.mandanten_id";
		$sql .= " AND ((m.soll_beginn<='".$bis."' AND m.soll_ende>='".$von."' AND p.projekt_status=".FXP_PS_ACTIVE.")";
		$sql .= " OR (m.soll_beginn<='".$bis."' AND p.projekt_status=".FXP_PS_ACTIVE." AND m.aufwand_std>0)) ";
		$sql .= "ORDER BY";
		$sql .= " m.soll_beginn, p.prioritaet DESC, p.vorgangsnummer";

		$pers_prj = db_values($sql, '*');
//fxDebug($pers_prj, $sql);

		// Aufgabentyp festlegen: 0=aktiv (Standard), 1=geplant ohne Budgetgenehmigung, 2=geplant mit Budgetgenehmigung
		if(fxIsArray($pers_prj))
		{
			foreach($pers_prj as $pj)
			{
				$pjc = (int)$pj['projekt_id'];

				// Kalkulationsenddatum evtl. anpassen
				$pj['ber_ende'] = $pj['soll_ende'];
				if(($pj['projekt_status'] == FXP_PS_ACTIVE) && ($pj['ber_ende'] < $today))
					$pj['ber_ende'] = substr($today, 0, 8).'240000';

				$personen[$i]['projekte'][$pjc] = $pj;
			}
		}
//fxDebug($personen, '$personen');
	}
}

// Daten für Anzeige ermitteln nur, wenn Personen inkl. Aufgaben gefunden
$GLOBALS['fxptdata']['cal'] = array();
if(is_array($personen) && is_array($personen[$personenid]['projekte']))
{
	// Kalender erstellen
	// ------------------
	$xoffset = 2;	// ab Spalte 3 beginnt der Kalender

	// Zeile 0+1 (0): Aktuelle Projekte
	$GLOBALS['fxptdata']['cal'][0][0]['text']		= '&nbsp;<b>'.$pb_string['aktuell'].'</b><br />&nbsp;';
	$GLOBALS['fxptdata']['cal'][0][0]['color']		= 'ffffff';
	$GLOBALS['fxptdata']['cal'][0][0]['rowspan']	= 2;
	$GLOBALS['fxptdata']['cal'][0][0]['typ']		= 'All';

	// Zeile 0+1 (1): Von - Bis
	$psvon	= formatdatetime($GLOBALS['fxpglobals']['settings']['formatdatum'], $von);
	$psbis	= formatdatetime($GLOBALS['fxpglobals']['settings']['formatdatum'], $bis);
	$zs		= $psvon.'-'.$psbis;
	$GLOBALS['fxptdata']['cal'][0][1]['text']		= '&nbsp;'.$zs.'&nbsp;<br />&nbsp;';
	$GLOBALS['fxptdata']['cal'][0][1]['color']		= 'ffffff';
	$GLOBALS['fxptdata']['cal'][0][1]['rowspan']	= 2;
	$GLOBALS['fxptdata']['cal'][0][1]['typ']		= 'All';

	// Zeile 2 (0-2): Überschriften "<Personentyp>", "Info"
	$GLOBALS['fxptdata']['cal'][2][0]['text']		= '&nbsp;<b>'.meldung($personen[$personenid]['ptyp'], true).'</b>&nbsp;'; // Personentyp
	$GLOBALS['fxptdata']['cal'][2][0]['color']		= 'dddddd';
	$GLOBALS['fxptdata']['cal'][2][0]['typ']		= 'All';

	$GLOBALS['fxptdata']['cal'][2][1]['text']		= '<b>'.$pb_string['ausl'].'</b>'; // Auslastung
	$GLOBALS['fxptdata']['cal'][2][1]['color']		= 'dddddd';
	$GLOBALS['fxptdata']['cal'][2][1]['align']		= 'center';
	$GLOBALS['fxptdata']['cal'][2][1]['typ']		= 'All';

	// Zeile 0-2: Loop über alle Tage des Zeitraums
	$holidays = get_feiertage($von, $bis, $GLOBALS['fxpglobals']['client']);
	$xcolumns = kalendertage($von, $bis, 'D');

	// Zeile 3-?: Personen + Projekte eintragen in jede Zeile
	$aktzeile = 3;
	kalenderzeile($personenid, $aktzeile, 0, $pb_string);

	// Kalender darstellen
	if(fxIsArray($GLOBALS['fxptdata']['cal'][3][$xoffset]))
	{
		$legende = farblegende('get');
		kalender(1143, $legende, '');
	}
}

//////////////////////////////////////////////////////////////////////////
// FORTSCHRITTS-SCHÄTZUNG												//
//////////////////////////////////////////////////////////////////////////
$tr254=tr_rechte(254);
if($tr254)
{
	// Alle Ressourcen-Aufgaben ermitteln
	if((strtolower($GLOBALS['fxpglobals']['settings']['zeit_zukunft']) != 'ja') && (strtolower($GLOBALS['fxpglobals']['settings']['ze_alle_aktiv']) != 'ja'))	// Einträge in der Zukunft und nicht auf alle aktiven Aufgaben verboten, dann Daten nur bis Heute
		$pdatum = substr($today, 0, 8).'240000';
	else																																		// ansonsten alle
		$pdatum = '99999999999999';

	$sqlz  = " FROM projekte p, maplanung m, zeitdaten z";
	$sqlz .= " WHERE p.projekt_id=m.projekt_id AND p.mandanten_id=m.mandanten_id AND p.projekt_id=z.projekt_id AND p.mandanten_id=z.mandanten_id";
	$sqlz .= " AND p.projektart=".FXP_PRJ_TASK." AND p.projekt_status=".FXP_PS_ACTIVE;
	$sqlz .= " AND m.personen_id=".$GLOBALS['fxpglobals']['person']." AND m.mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$sqlz .= " AND z.zeitart=102 AND z.soll_beginn<='$pdatum'";
	$sqlz .= filter_projekt('and', true, false, 'Aufgaben');

	$sql="SELECT p.projekt_id, p.name_projekt, p.vorgangsnummer, p.prioritaet, m.personen_id, z.soll_beginn, z.soll_ende".$sqlz." ORDER BY p.vorgangsnummer";
	$sql_30="SELECT p.projekt_id".$sqlz;

	$ar=array();
	$tmp=db_values($sql, '*');
	if(fxIsArray($tmp))
	{
		$ar[-30] = true;
		foreach($tmp as $t)
			$ar[$t['personen_id'].'_'.$t['projekt_id']] = $t;
	}

	// Programm zum füllen der Anzeige einbinden
	if(isset($_POST['nv_check2']))
		unset($_POST['nv_check2']);
	$_POST['Aufgaben']=0;
	$_POST['nv_check1']=1;
	$_POST['nv_check2']='';
	$_POST['nv_check3']=1;
	$_POST['nv_check4']=1;

	// Felder ausblenden, falls im Kontrollzentrum aufgerufen
	$allg_zahlenfeld_nv	= true;
	$nv_text1_nv		= true;
	$Anmerkung_nv		= true;
	$Zeitraum_nv		= true;
	$Button_Anpassen_nv	= true;
	$Button_Speichern_nv= true;

	$req_fn=fxf_fn_reqFilename('pfortschritt_eingabe_pre', 254);
	if(strlen($req_fn))
	{
		require($req_fn);

		// Maske anzeigen
		echo(maske(501));
	}

	// Felder wieder freigeben für nächste Masken
	unset($allg_zahlenfeld_nv);
	unset($nv_text1_nv);
	unset($Anmerkung_nv);
	unset($Zeitraum_nv);
	unset($Button_Anpassen_nv);
}

//////////////////////////////////////////////////////////////////////////
// DOKUMENTE															//
//////////////////////////////////////////////////////////////////////////
$tr107=tr_rechte(107);	// ...Dokumente suchen
$tr110=tr_rechte(110);	// ...Dokumentenmanager
if($tr107 || $tr110)
{
	// Enddatum = Sonntag der aktuellen Woche
	$Zeitspanne_bis=$today;
	$wd=getWeekday($Zeitspanne_bis);
	if($wd)
		$Zeitspanne_bis=fx_date('Ymd240000', fx_mktime(12, 0, 0, (int)substr($Zeitspanne_bis,4,2), (int)substr($Zeitspanne_bis,6,2)+(7-$wd), (int)substr($Zeitspanne_bis,0,4)));
	else
		$Zeitspanne_bis=substr($Zeitspanne_bis,0,8).'240000';

	// Startdatum = Montag vor x Wochen (laut Default: 4)
	$Zeitspanne_von=fx_date('Ymd000000', fx_mktime(12, 0, 0, (int)substr($Zeitspanne_bis,4,2), (int)substr($Zeitspanne_bis,6,2)-$GLOBALS['fxpglobals']['settings']['stdzeitspanne']+1, (int)substr($Zeitspanne_bis,0,4)));

	$elter_art=0;
	$Kategorie=array(0=>0);
	$allg_textfeld='';
	$groesse_von='';
	$groesse_bis='';
	$E_Mail='1';

	$Button_suchen=true;

	$req_fn=fxf_fn_reqFilename('d_dok_liste_ini', 107);
	if(strlen($req_fn))
	{
		require($req_fn);
		$req_fn=fxf_fn_reqFilename('d_dok_liste_pre', 107);
		if(strlen($req_fn))
		{
			require($req_fn);

			// Maske anzeigen
			$maske170_head=meldung(324,false).' <font class="s2 lightgrey">('.get_entry($Zeitspanne_von,'datum').' - '.get_entry($Zeitspanne_bis,'datum').')</font>';
			echo(maske(170));
		}
	}

	// Felder wieder freigeben für nächste Masken
	unset($Zeitspanne_von);
	unset($Zeitspanne_bis);
	unset($elter_art);
	unset($Kategorie);
	unset($allg_textfeld);
	unset($groesse_von);
	unset($groesse_bis);
	unset($E_Mail);
	unset($Button_suchen);
}

//////////////////////////////////////////////////////////////////////////
// MEILENSTEINE															//
//////////////////////////////////////////////////////////////////////////
$cmc=2;
$tr44=tr_rechte(44);	// ...Meilensteine
$with_planned_milestones=false;
if($tr44)
	$with_planned_milestones=true;
$GLOBALS['_maskcounter']=$cmc;
if(fxIsArray($GLOBALS['fxptdata']['lvalues'][$GLOBALS['_maskcounter']]))
	$GLOBALS['fxptdata']['lvalues'][$GLOBALS['_maskcounter']]=array();

$req_fn=fxf_fn_reqFilename('mst_liste_pre', 44);
if(strlen($req_fn))
{
	require($req_fn);
	echo(maske(543));
}

//////////////////////////////////////////////////////////////////////////
// TERMINE																//
//////////////////////////////////////////////////////////////////////////
$cmc++;
$tr27=tr_rechte(27);	// ...Terminplaner
$tr222=tr_rechte(222);	// ...Abwesenheit
if($tr27 || $tr222)
{
	$GLOBALS['_maskcounter']=$cmc;
	if(fxIsArray($GLOBALS['fxptdata']['lvalues'][$GLOBALS['_maskcounter']]))
		$GLOBALS['fxptdata']['lvalues'][$GLOBALS['_maskcounter']]=array();

	// Öffnen einer Tabelle, damit sich Termintabelle und Kalender nicht überlagern
	echo('<table border="0" cellpadding="0" cellspacing="0">'.$nl);
	echo('<td nowrap valign="top" style="padding-bottom:12px; border-bottom:1px dotted #cccccc;">'.$nl);

	echo('<div id="ccapp">'.$nl);
	$req_fn=fxf_fn_reqFilename('appointments', 27);
	if(strlen($req_fn))
		require($req_fn);
	$msk_html=maske(185);
	echo($msk_html);
	echo('</div>'.$nl);

	$maske185_nv=true;

	// Variablen für den Navigationskalender
	$datum = $Heute_a;

	// 1. Aufruf => heutiges Datum
	if($today_is)
		$datum=$today_is;

	$wochentagsnamen=db_values("SELECT tabwert FROM wertetab WHERE id_feld=289 AND id_sprache=".$GLOBALS['fxpglobals']['lang']." AND mandanten_id=0 ORDER BY position", '*');
	$monatsnamen=db_values("SELECT tabwert FROM wertetab WHERE id_feld=62 AND id_sprache=".$GLOBALS['fxpglobals']['lang']." AND mandanten_id=0 ORDER BY position", '*');

	// Navigationskalender aufrufen (Nur im "Kontrollzentrum", nicht aber in "Termine heute")
	echo('</td>'.$nl);
	echo('<td nowrap style="border-right:1px dotted #cccccc;border-bottom:1px dotted #cccccc;">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td>'.$nl);
	echo('<td nowrap valign="top" style="width:288px;padding:12px;border-top:1px dotted #cccccc;">'.$nl);

	$fdate=substr($datum, 0,8);
	$mark=$fdate;

	$req_fn=fxf_fn_reqFilename('popup_cal');
	if(strlen($req_fn))
		require($req_fn);

	// Schließen der Spalte und der Tabelle
	echo('</td>'.$nl);
	echo('</table>'.$nl);
}
$brset=false;

//////////////////////////////////////////////////////////////////////////
// OUTGOING INVOICES													//
//////////////////////////////////////////////////////////////////////////
$cmc++;
$tr105=tr_rechte(105);	// ...Outgoing Invoices
if($tr105)
{
	if(!$brset)
	{
		$brset=true;
		echo('<br />'.$nl);
	}

	$GLOBALS['_maskcounter']=$cmc;
	if(fxIsArray($GLOBALS['fxptdata']['lvalues'][$GLOBALS['_maskcounter']]))
		$GLOBALS['fxptdata']['lvalues'][$GLOBALS['_maskcounter']]=array();

	$req_fn=fxf_fn_reqFilename('oinvoices', 105);
	if(strlen($req_fn))
	{
		echo('<div id="ccinv">'.$nl);
		require($req_fn);
		echo('</div>'.$nl);
	}
}

//////////////////////////////////////////////////////////////////////////
// INCOMING INVOICES													//
//////////////////////////////////////////////////////////////////////////
$cmc++;
$tr117=tr_rechte(117);	// ...Incoming Invoices
if($tr117)
{
	if(!$brset)
	{
		$brset=true;
		echo('<br />'.$nl);
	}

	$GLOBALS['_maskcounter']=$cmc;
	if(fxIsArray($GLOBALS['fxptdata']['lvalues'][$GLOBALS['_maskcounter']]))
		$GLOBALS['fxptdata']['lvalues'][$GLOBALS['_maskcounter']]=array();

	$show_list=true;

	$arr_prj=auswahl_Projekt_ID(1,array(),'Projekt_IDopen');
	$arr_pers=array();

	$itca=array(3114=>2091, 276, 1372, 275, 840);
	foreach($itca as $it => $inv_pers_type)
	{
		$Personen_ID_arg=array('ptyp'=>$inv_pers_type);
		$arr_pers[$it]=auswahl_Personen_ID(1,array(),'Personen_ID');
		if(!is_array($arr_pers[$it]))
			$arr_pers[$it]=array();
	}

	$_POST['Zeitspanne_von']='';
	$_POST['Zeitspanne_bis']='';

	$_POST['typ']=0;
	$_POST['bezahlstatus']=3090;	// Open

	$_POST['apname']='';
	foreach($itca as $it => $inv_pers_type)
		$_POST['apers_'.$it]=0;
	$_POST['Projekt_IDopen']=0;

	$req_fn=fxf_fn_reqFilename('rechnung_e_list_pre', 117);
	if(strlen($req_fn))
	{
		require($req_fn);

		// Display mask
		$maske55_head  = '<table width=100% border=0 cellpadding=0 cellspacing=0><tr>';

		$htitle=meldung(3104,false);	// Listing of all "Open" Incoming Invoices
		$maske55_head .= '<td nowrap><font class=s4>'.$htitle.'</font></td>';

		$ltt=meldung(3140).': <b class=grey>'.trPath(117).'</b>';
		$icon=fieldSubmitImage('icon_ch55','icln','',$ltt,false,false,117,'str='.$GLOBALS['fxpglobals']['tr'].'&aktion=3&suche_von='.$suche_von.'&keep_get=1', ' style="margin-right:10px;margin-top:3px;"');	// Edit list in the program function
		$maske55_head .= '<td align=right nowrap>'.$icon.'</td>';

		$maske55_head .= '</tr></table>';
		echo(maske(55));
	}
}
?>