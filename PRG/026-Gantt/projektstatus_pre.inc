<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : projektstatus_pre.inc                                        //
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
 * PF 26: Gantt
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

$von=substr((string)time_add($von, -1, 'D'),0,8);
$bis=substr((string)time_add($bis, 1, 'D'),0,8);

// Je nach Kalenderanzeigemodus die Darstellungsart festlegen
$ke_array=array(1260=>array('A',''), 1255=>array('D','_d'), 1256=>array('W','_w'), 1257=>array('M','_m'), 1258=>array('Y','_y'));
$ke=$ke_array[$Kalenderanzeigemodus][0];
$kb=$ke_array[$Kalenderanzeigemodus][1];

// Prüfen, ob Benutzer Recht auf "Vorgang definieren" (23) oder "Projekt-Report" (118) hat
$rights_23=tr_rechte(23);
$rights_118=tr_rechte(118);

// Abhängigkeitstypes
$dtypes=getDTypes();

// Alle Projektkategorien ermitteln
$kar=fxf_getReferenceValues('kategorie');
if(fxIsArray($kar))
{
	foreach($kar as $kid => $ktxt)
		$kar[$kid]=pk_backgrounds(1,$ktxt);
}
//fxDebug($kar, '$kar');

// Abhängigkeiten und Meilensteine vorhanden?
$GLOBALS['dep_cnt']=0;
$GLOBALS['mst_cnt']=0;

$maske131_sf=true;
echo(maske(131));
$maske131_nv=true;

// Projektinfo einlesen, wenn Anzeige-Button gedrückt wurde und keine Fehler vorliegen
$pr_status=array();
if(($_POST['Button_ProAnzeigen'] || $_POST['Button_PrProAnzeigen']) && !$fehler)
{
	// Texte
	$txt_meilenstein=meldung(957, true);	// Meilenstein

	// Projekte
	$pr_sql="SELECT p.projekt_id, p.mutterprojekt_id, p.name_projekt, p.vorgangsnummer, p.aufwand_soll, p.aufwand_ist, p.projektart, p.projekt_status, p.elter, p.kategorie_id";
	if($ProjektID)	// Ausgewähltes Projekte
	{
		$pr_sql .= " FROM projekte p";
		$pr_sql .= " WHERE p.projekt_id=".$ProjektID." AND p.mandanten_id=".$GLOBALS['fxpglobals']['client'];
	}
	else // Alle Projekte
	{
		$GLOBALS['ignore_soll_beginn']=true;

		$pr_sql .= " FROM projekte p, zeitdaten z";
		$pr_sql .= " WHERE p.projekt_id=z.projekt_id AND z.zeitart=102 AND p.projektart=".FXP_PRJ_MAIN." AND p.mandanten_id=".$GLOBALS['fxpglobals']['client'];
		if($GLOBALS['fxpglobals']['person'] && ($GLOBALS['fxpglobals']['user'] > 1) && !$GLOBALS['fxpglobals']['persdat']['roles'][2])
			$pr_sql .= " AND p.projekt_id IN (".prights_get(0).")";
		$pr_sql .= " AND (z.soll_beginn<='".$bis."240000' AND z.soll_ende>='".$von."000000') AND z.mandanten_id=".$GLOBALS['fxpglobals']['client'];

		$pr_sql .= filter_projekt('and', true, false);
	}

	if($Projekt_Status)
		$statustext=meldung($Projekt_Status); // Statustext: "aktiv", ...
	else
		$statustext=meldung(1077);  // Statustext: "Alle Projekte"

	$pr_status=db_values($pr_sql." ORDER BY p.vorgangsnummer", '*');
//fxDebug($pr_status,$pr_sql);

	// 2. alle weiteren Ebenen rekursiv
	if(fxIsArray($pr_status))
	{
		$von_min=$von;
		$bis_max=$bis;
		pinfo($pr_status, true);

		// Kalender erstellen
		$xoffset=4;

		// Zeile 0
		// ... Spalte 0: Projektstatus
		$GLOBALS['fxptdata']['cal'][0][0]['text']		= '&nbsp;<b>'.$pa_string['pstatus'].'</b>:&nbsp;';
		$GLOBALS['fxptdata']['cal'][0][0]['color']		= 'ffffff';
		$GLOBALS['fxptdata']['cal'][0][0]['typ']		= 'All';
		// ... Spalte 1-3: Alle Projekte
		$GLOBALS['fxptdata']['cal'][0][1]['text']		= '&nbsp;'.$statustext.'&nbsp;'; // "<Status>" oder "Alle Projekte"
		$GLOBALS['fxptdata']['cal'][0][1]['color']		= 'f8f8f8';
		$GLOBALS['fxptdata']['cal'][0][1]['colspan']	= 3;
		$GLOBALS['fxptdata']['cal'][0][1]['typ']		= 'All';

		// Zeile 1
		// ...Spalte 0: Zeitspanne
		$GLOBALS['fxptdata']['cal'][1][0]['text']		= '&nbsp;<b>'.$pa_string['zeitspanne'].'</b>:&nbsp;';
		$GLOBALS['fxptdata']['cal'][1][0]['color']		= 'ffffff';
		$GLOBALS['fxptdata']['cal'][1][0]['typ']		= 'All';
		// ...Spalte 1-3: Datum von-bis
		$psvon=get_entry($von_min, "datum");
		$psbis=get_entry($bis_max, "datum");
		if(!$psvon && !$psbis)
			$GLOBALS['fxptdata']['cal'][1][1]['text']='&nbsp;'.$pa_string['ka'].'&nbsp;'; // (Keine Angaben)
		else
		{
			$GLOBALS['fxptdata']['cal'][1][1]['text']='&nbsp;'.$psvon.'&nbsp;'; // Von
			if($psvon != $psbis)
				$GLOBALS['fxptdata']['cal'][1][1]['text'] .= '-&nbsp;'.$psbis.'&nbsp;'; // - Bis
		}
		$GLOBALS['fxptdata']['cal'][1][1]['color']		= 'f8f8f8';
		$GLOBALS['fxptdata']['cal'][1][1]['colspan']	= 3;
		$GLOBALS['fxptdata']['cal'][1][1]['typ']		= 'All';

		// Zeile 2
		// ...Spalte 0: Überschrift "Projekt-Name"
		$GLOBALS['fxptdata']['cal'][2][0]['text']		= '&nbsp;<b>'.$pa_string['pname'].'</b>&nbsp;'; // Projekt-Name
		$GLOBALS['fxptdata']['cal'][2][0]['color']		= 'dddddd';
		$GLOBALS['fxptdata']['cal'][2][0]['typ']		= 'All';
		// ...Spalte 1: Überschrift "Projekt-Dauer"
		$GLOBALS['fxptdata']['cal'][2][1]['text']		= '&nbsp;<b>'.$pa_string['pdauer'].'</b>&nbsp;'; // Projekt-Dauer
		$GLOBALS['fxptdata']['cal'][2][1]['color']		= 'dddddd';
		$GLOBALS['fxptdata']['cal'][2][1]['typ']		= 'All';
		// ...Spalte 2-3: Überschrift "Info"
		$GLOBALS['fxptdata']['cal'][2][2]['text']		= '<b>'.$pa_string['info'].'</b>'; // Info
		$GLOBALS['fxptdata']['cal'][2][2]['color']		= 'dddddd';
		$GLOBALS['fxptdata']['cal'][2][2]['align']		= 'center';
		$GLOBALS['fxptdata']['cal'][2][2]['colspan']	= 2;
		$GLOBALS['fxptdata']['cal'][2][2]['typ']		= 'All';

		// Zeile 0-2: Loop über alle Tage des Zeitraums
		$holidays=get_feiertage($von_min, $bis_max);
		$xcolumns=kalendertage($von_min, $bis_max, $ke);

		$prj_array=array();

		// Zeile 3-?: Projekte eintragen in jede Zeile
		$aktzeile=3;
		$origin=$pr_status[0]['projekt_id'];
		projektzeile($pr_status, $aktzeile);

		// Anzahl der Meilensteine und Abhängigkeiten in Kalenderarray eintragen
		$GLOBALS['fxptdata']['cal'][0][0]['dep_cnt']=$GLOBALS['dep_cnt'];
		$GLOBALS['fxptdata']['cal'][0][0]['mst_cnt']=$GLOBALS['mst_cnt'];

		// Anzuzeigender PDF-Titel
		if($ProjektID)	// Ausgewähltes Projekte
			$GLOBALS['fxptdata']['cal'][0][0]['title']=strip_tags($pr_status[0]['name_projekt']);
		else if($GLOBALS['Projektart'] == 2)	// Nur Aufgaben
			$GLOBALS['fxptdata']['cal'][0][0]['title']=meldung(3239);	// Alle Aufgaben
		else
			$GLOBALS['fxptdata']['cal'][0][0]['title']=meldung(1077);	// Alle Projekte
	}

	// Grafische Anzeige des Projekt-Status, wenn Daten in $pr_status vorhanden
	if(fxIsArray($pr_status) && ($GLOBALS['fxptdata']['cal'][3][$xoffset]))
	{
//fxDebug($pa_string,'$pa_string');
		// Legende zeichnen
		$legende = '';
		$farbbalken = db_values("SELECT wertetabid, tabwert FROM wertetab WHERE id_feld=137 AND id_sprache=".$GLOBALS['fxpglobals']['lang']." AND mandanten_id=0 ORDER BY position", '*');
		$theight = 20;
		if(is_array($farbbalken))
		{
			$dfla=explode('|',meldung(3205));	// dynamisch|fest|dyn.|fest|Dynamisch|Fest|Dyn.|Fest|D|F|Start|Anfang|Ende|S|A|E
			foreach($farbbalken as $value)
				$farbnamen[$value['wertetabid']] = $value['tabwert'];

			$legende .= '<table cellpadding="1" cellspacing="2">'.$nl;
			// ...Projekte
			$legende .= '	<tr>'.$nl;
			$legende .= '		<td nowrap>'.meldung(43).'&nbsp;&nbsp;</td>'.$nl;
			$legende .= '		<td nowrap>'.$nl;
			$legende .= '			<div style="float:left;width:50px;height:15px;background:url('.$GLOBALS['gfxpath'].'pspr_s.png);"><img src="'.$GLOBALS['gfxpath'].'psprr.png" align="right"><img src="'.$GLOBALS['gfxpath'].'psprl.png" align="left"></div>'.$nl;
			$legende .= '		</td>'.$nl;
			$legende .= '	</tr>'.$nl;
			// ...Aufgaben (geplant, aktiv, inaktiv + abgeschl.)
			$legende .= '	<tr>'.$nl;
			$legende .= '		<td nowrap>'.meldung(326).'&nbsp;&nbsp;</td>'.$nl;
			$legende .= '		<td nowrap>'.$nl;
			foreach($farbnamen as $key => $value)
			{
				$legende .= '			<div style="float:left;width:50px;height:15px;background:url('.$GLOBALS['gfxpath'].'psau_s.png);"><img src="'.$GLOBALS['gfxpath'].'psau'.$key.'r.png" align="right"><img src="'.$GLOBALS['gfxpath'].'psau'.$key.'l.png" align="left"></div>'.$nl;
				$legende .= '			<div style="float:left;width:90px;">&nbsp;<i class="s2 grey">('.$value.')</i></div>'.$nl;
			}
			$legende .= '		</td>'.$nl;
			$legende .= '	</tr>'.$nl;
			// ...Meilensteine (dynamisch + fest)
			if($GLOBALS['mst_cnt'])
			{
				$legende .= '	<tr>'.$nl;
				$legende .= '		<td nowrap>'.meldung(375).'&nbsp;&nbsp;</td>'.$nl;
				$legende .= '		<td nowrap>'.$nl;
				$legende .= '			<div style="float:left;width:50px;"><img src="'.$GLOBALS['gfxpath'].'g_mst.png"></div>'.$nl;
				$legende .= '			<div style="float:left;width:90px;">&nbsp;<i class="s2 grey">('.$dfla[0].')</i></div>'.$nl;
				$legende .= '			<div style="float:left;width:50px;"><img src="'.$GLOBALS['gfxpath'].'g_mstf.png"></div>'.$nl;
				$legende .= '			<div style="float:left;width:90px;">&nbsp;<i class="s2 grey">('.$dfla[1].')</i></div>'.$nl;
				$legende .= '		</td>'.$nl;
				$legende .= '	</tr>'.$nl;
			}
			// ...Abhängigkeiten
			if($GLOBALS['dep_cnt'])
			{
				$legende .= '	<tr>'.$nl;
				$legende .= '		<td nowrap>'.meldung(1587).'&nbsp;&nbsp;</td>'.$nl;
				$legende .= '		<td nowrap>'.$nl;
				$legende .= '			<div style="float:left;width:50px;"><img src="'.$GLOBALS['gfxpath'].'dep_fs.png">&nbsp;<img src="'.$GLOBALS['gfxpath'].'dep_ff.png"></div>'.$nl;
				$legende .= '			<div style="float:left;width:90px;">&nbsp;<i class="s2 grey">('.$dfla[11].')</i></div>'.$nl;
				$legende .= '			<div style="float:left;width:50px;"><img src="'.$GLOBALS['gfxpath'].'depf_ts.png">&nbsp;<img src="'.$GLOBALS['gfxpath'].'depf_tf.png"></div>'.$nl;
				$legende .= '			<div style="float:left;width:90px;">&nbsp;<i class="s2 grey">('.$dfla[12].')</i></div>'.$nl;
				$legende .= '		</td>'.$nl;
				$legende .= '	</tr>'.$nl;
			}
			$legende .= '</table><br />'.$nl;
		}

		kalender(1147, $legende, '');
	}

	// Fehlermeldung, falls keine Projekte gefunden wurden
	else if(($Button_ProAnzeigen || $Button_PrProAnzeigen) && !$fehler)
		kalender(1147, '?'.meldung(1099, true), '');
}


////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/**
 * -----------------------------------------------------------------------------
 * ----------------------------------------------------------------- PROJEKTINFO
 * -----------------------------------------------------------------------------
 *
 * @param various &$pr_status - Mandatory parameter: ???PARAMETER???
 * @param various $ebene      - Mandatory parameter: ???PARAMETER???
 */
function pinfo(&$pr_status, $ebene)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	global $Projekt_Status;

	// Für jedes Projekt die entspr. Infos suchen
	for($i=0; $i<sizeof($pr_status); $i++)
	{
		$pr_sql  = "SELECT projekt_id, mutterprojekt_id, name_projekt, vorgangsnummer, aufwand_soll, aufwand_ist, projektart, projekt_status, elter, kategorie_id FROM projekte";
		$pr_sql .= " WHERE elter=".$pr_status[$i]['projekt_id']." AND projektart NOT IN (78,79) AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		if($GLOBALS['lvl'] < 666)
			$pr_sql .= " AND ".$GLOBALS['slen']."(vorgangsnummer)<".(($GLOBALS['lvl']+1)*5);
		$pr_status[$i]['sublevel']=db_values($pr_sql." ORDER BY vorgangsnummer", '*');

		// Projekt gefunden mit Unterprojekten/-aufgaben
		if(fxIsArray($pr_status[$i]['sublevel']))
		{
			$pr_status[$i]['darstellung']='pr';
			if($ebene)
				pinfo($pr_status[$i]['sublevel'], true);
			else
				$pr_status[$i]['sublevel']=false;
		}
		// Aufgabe oder Projekt ohne Unterprojekte/-aufgaben gefunden
		else
		{
			if($pr_status[$i]['projektart'] == FXP_PRJ_TASK)
				$pr_status[$i]['darstellung']='au';
			else
				$pr_status[$i]['darstellung']='pr';
		}

		// Plandaten ermitteln für Projekt/Aufgabe
		$sql  = "SELECT soll_beginn, soll_ende, NULl AS ist_beginn, NULL AS ist_ende FROM zeitdaten";
		$sql .= " WHERE projekt_id=".$pr_status[$i]['projekt_id']." AND mandanten_id=".$GLOBALS['fxpglobals']['client']." AND zeitart=102";
		$zeitdaten=db_values($sql);
		$zeitdaten['soll_beginn']=substr((string)$zeitdaten['soll_beginn'], 0, 8);
		$zeitdaten['soll_ende']=substr((string)$zeitdaten['soll_ende'],0,8);
		// Istdaten ermitteln
		if(!$GLOBALS['jplan'])
		{
			$sql  = "SELECT MIN(ist_beginn) AS ib, MAX(ist_ende) AS ie FROM zeitdaten";
			$sql .= " WHERE projekt_id IN (SELECT projekt_id FROM projekte WHERE vorgangsnummer LIKE '".$pr_status[$i]['vorgangsnummer']."%' AND projektart=".FXP_PRJ_TASK." AND mandanten_id=".$GLOBALS['fxpglobals']['client'].")";
			$sql .= " AND zeitart IN (".FXP_TE_TIMEREC.",".FXP_TE_TIMEREC_SUM.",".FXP_TE_TRAVELEXP_TIMEREC.",".FXP_TE_TRAVELEXP.") AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$ist=db_values($sql);
			if(fxIsArray($ist))
			{
				$zeitdaten['ist_beginn']='';
				if(!is_null($ist['ib']) && strlen((string)$ist['ib']))
					$zeitdaten['ist_beginn']=substr((string)$ist['ib'], 0,8);
				$zeitdaten['ist_ende']='';
				if(!is_null($ist['ie']) && strlen((string)$ist['ie']))
					$zeitdaten['ist_ende']=substr((string)$ist['ie'], 0,8);
			}
		}
//fxDebug($zeitdaten, $pr_status[$i]['projekt_id']);

		if($zeitdaten['ist_beginn'])
			$z_von=min($zeitdaten['soll_beginn'], $zeitdaten['ist_beginn']);
		else
			$z_von=$zeitdaten['soll_beginn'];

		if($zeitdaten['ist_ende'])
			$z_bis=max($zeitdaten['soll_ende'], $zeitdaten['ist_ende']);
		else
			$z_bis=$zeitdaten['soll_ende'];

		$pr_status[$i]['zeitdaten']=$zeitdaten;
		$pr_status[$i]['zeitdaten']['von']=$z_von;
		$pr_status[$i]['zeitdaten']['bis']=$z_bis;

		// Prüfen, ob Projekt angezeigt werden soll oder nicht
		// Falscher Projektstatus
		if($Projekt_Status && ($pr_status[$i]['projekt_status'] != $Projekt_Status))
			$pr_status[$i]['darstellung']=false;
		// Alle Projekte, dann Projektstatus anhand Filtereinstellungen
		if(!$Projekt_Status && is_array($GLOBALS['fxpglobals']['filter']))
		{
			$ausblenden=true;
			$cnt=0;
			foreach($GLOBALS['fxpglobals']['filter'] as $feld => $wert)
			{
				if((substr((string)$feld, 0, 14) == 'projektstatus_') && (!$GLOBALS['fxpglobals']['setfilter'] || $GLOBALS['fxpglobals']['filter']['fprojekt_inaktiv'] || $wert))
				{
					$cnt++;
					$pstid=(int)substr((string)$feld, 14);
					if($pr_status[$i]['projekt_status'] == $pstid)
					{
						$ausblenden=false;
						break;
					}
				}
			}
			if(!$cnt)
				$ausblenden=false;
			if($ausblenden)
				$pr_status[$i]['darstellung']=false;
		}
		// Projekt liegt ausserhalb des gewünschten Zeitraums
		if(($z_bis < $GLOBALS['von_min']) || ($z_von > $GLOBALS['bis_max']))
			$pr_status[$i]['darstellung']=false;
	}
}

/**
 * -----------------------------------------------------------------------------
 * --------------------------------------------------------------- PROKJEKTZEILE
 * -----------------------------------------------------------------------------
 *
 * @param various $pr_status - Mandatory parameter:                ???PARAMETER???
 * @param various &$aktzeile - Mandatory parameter:                ???PARAMETER???
 * @param string  $sublevel  - Optional parameter (default = ''):  ???PARAMETER???
 */
function projektzeile($pr_status, &$aktzeile, $sublevel='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	global $pa_string, $zeitanzeige, $origin, $ke, $Button_PrProAnzeigen, $rights_23, $rights_118, $kar, $nl;

	$sublevel_next=$sublevel;
	if($GLOBALS['Projektart'] != 2)
		$sublevel_next .= '&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;';

	$col_bgl0='f8f8f8';
	$col_bgl1='f4f4f4';
	$col_bgr='ffffff';

	$lh=34;
	$dh=24;
	$lph=$dh-10;
	$lth=$dh-8;

	// Alle Projekte/Aufgabe der aktuellen Ebene anzeigen
	foreach($pr_status as $prid => $zeile)
	{
//fxDebug($zeile, $prid);
		// Nur Projekte bzw. Aufgaben auch wirklich darstellen
		if($zeile['darstellung'])
		{
			$GLOBALS['prj_array'][$zeile['projekt_id']]=$aktzeile;

			// Prüfen, ob Problemprojekt
			$problem=ist_problem($zeile);
			if($Button_PrProAnzeigen && ($problem['id'] < 1))
				continue;

			// Prüfen, ob Abhängigkeiten vorhanden sind
			$abh_sql  = "SELECT a.projekt_a, pa.vorgangsnummer AS nr_a, pa.name_projekt AS name_a, a.projekt_b, pb.vorgangsnummer AS nr_b, pb.name_projekt AS name_b, a.abh_typ, a.tagesanzahl";
			$abh_sql .= " FROM abhaengigkeiten a, projekte pa, projekte pb";
			$abh_sql .= " WHERE (a.projekt_a=".$zeile['projekt_id']." OR a.projekt_b=".$zeile['projekt_id'].") AND a.mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$abh_sql .= " AND pa.projekt_id=a.projekt_a AND pa.mandanten_id=a.mandanten_id AND pb.projekt_id=a.projekt_b AND pb.mandanten_id=a.mandanten_id";
			$abh_sub=db_values($abh_sql, '*');
//fxDebug($abh_sub,$abh_sql);

			// Prüfen, ob Meilensteine vorhanden
			$vm=$GLOBALS['von_min'].'000000';
			$bm=$GLOBALS['bis_max'].'240000';
			$sql  = "SELECT meilenstein_id, name_meilenstein, soll_ende, statusfeld, tagesanzahl FROM meilensteine";
			$sql .= " WHERE projekt_id=".$zeile['projekt_id']." AND mandanten_id=".$GLOBALS['fxpglobals']['client']." AND (soll_ende>='".$vm."' AND soll_ende<='".$bm."')";
			$sql .= " ORDER BY soll_ende";
			$miles=db_values($sql, '*');
//fxDebug($miles,$sql);

			$ci=$zeile['kategorie_id'];

			// Zeile x (Spalte 0: "Projekt-Name")
			$has_pr=0;
			$has_ts=0;
			$defimg='';
			if(fxIsArray($zeile['sublevel']) || fxIsArray($miles))
			{
				if(fxIsArray($zeile['sublevel']))
				{
					foreach($zeile['sublevel'] as $zs)
					{
						if($zs['darstellung'])
						{
							if($zs['projektart'] == FXP_PRJ_TASK)
								$has_ts++;
							else if($zs['projektart'] == FXP_PRJ_SUB)
								$has_pr++;
						}
					}
				}
//echo('<b>'.$zeile['projekt_id'].':</b> $has_pr='.$has_pr.', $has_ts='.$has_ts.'<br>');

				if($has_pr || $has_ts || fxIsArray($miles))
				{
					// Aufgaben ausblenden
					if($GLOBALS['Projektart'] == 1)	// Nur Projekte
					{
						if($has_ts || fxIsArray($miles))
							$defimg='op';
						else
							$defimg='cl';
					}
					// Aufgaben einblenden
					else
						$defimg='cl';
				}
			}
			$actimg=$defimg;

			$disp='';
			if($zeile['projektart'] == FXP_PRJ_TASK)
			{
				if($GLOBALS['Projektart'] == 1)		// Nur Projekte
					$disp='none';
			}
			else if($GLOBALS['Projektart'] == 2)	// Nur Aufgaben
				$disp='none';

			if(strlen((string)$actimg))
				$seimg='<img id="its'.$aktzeile.'" src="'.$GLOBALS['gfxpath'].'fl_'.$actimg.'.png" hspace="2" style="cursor:pointer;" '.fxf_jsFunction('toggleSubs', $aktzeile, $zeile['vorgangsnummer']).'>';
			else
				$seimg='<img src="'.$GLOBALS['gfxpath'].'dmy.png" width="13" height="1">';
			if(substr((string)$zeile['darstellung'], 0, 1) == 'p')
			{
				$cl1='darkgrey';
				$cl2='grey bold';
			}
			else
			{
				$cl1='black';
				$cl2='darkgrey';
			}
			$GLOBALS['fxptdata']['cal'][$aktzeile][0]['text']  = $seimg.$sublevel.'&nbsp;';
			$GLOBALS['fxptdata']['cal'][$aktzeile][0]['text'] .= '<font class="'.$cl1.'">'.no_null($zeile['vorgangsnummer']).'</font>&nbsp;';
			$GLOBALS['fxptdata']['cal'][$aktzeile][0]['text'] .= '<font class="'.$cl2.'">'.$zeile['name_projekt'].'</font>&nbsp;';
			$GLOBALS['fxptdata']['cal'][$aktzeile][0]['text'] .= fieldHidden('disp'.$zeile['projekt_id'],$disp.'_'.$actimg).fieldHidden('pr['.$aktzeile.']',$zeile['projekt_id']).fieldHidden('vn['.$aktzeile.']',$zeile['vorgangsnummer']);
			$GLOBALS['fxptdata']['cal'][$aktzeile][0]['color']=${'col_bgl'.($aktzeile%2)};
			$GLOBALS['fxptdata']['cal'][$aktzeile][0]['disp']=$disp;
			$GLOBALS['fxptdata']['cal'][$aktzeile][0]['typ']='All';
			$GLOBALS['fxptdata']['cal'][$aktzeile][0]['pid']=$zeile['projekt_id'];

			// Zeile x (Spalte 1: "Projekt-Dauer")
			$sb=get_entry($zeile['zeitdaten']['soll_beginn'], 'datum');
			$se=get_entry($zeile['zeitdaten']['soll_ende'], 'datum');
			$GLOBALS['fxptdata']['cal'][$aktzeile][1]['text']  = '<a href="#pj'.($aktzeile-2).'">';
			$GLOBALS['fxptdata']['cal'][$aktzeile][1]['text'] .= '&nbsp;<i class="grey">('.$sb;		// Soll-Beginn
			if($sb != $se)
				$GLOBALS['fxptdata']['cal'][$aktzeile][1]['text'] .= '&nbsp;-&nbsp;'.$se;			// Soll-Ende
			$GLOBALS['fxptdata']['cal'][$aktzeile][1]['text'] .= ')</i>&nbsp;<br>';
			if($zeile['zeitdaten']['ist_beginn'])
			{
				$ib=get_entry($zeile['zeitdaten']['ist_beginn'], 'datum');
				$ie=get_entry($zeile['zeitdaten']['ist_ende'], 'datum');
				$GLOBALS['fxptdata']['cal'][$aktzeile][1]['text'] .= '&nbsp;'.$ib;					// Ist-Beginn
				if($ib != $ie)
					$GLOBALS['fxptdata']['cal'][$aktzeile][1]['text'] .= '&nbsp;-&nbsp;'.$ie;		// Ist-Ende
				$GLOBALS['fxptdata']['cal'][$aktzeile][1]['text'] .= '&nbsp;';
			}
			$GLOBALS['fxptdata']['cal'][$aktzeile][1]['text'] .= '</a>';
			$GLOBALS['fxptdata']['cal'][$aktzeile][1]['color']=${'col_bgl'.($aktzeile%2)};
			$GLOBALS['fxptdata']['cal'][$aktzeile][1]['align']='center';
			$GLOBALS['fxptdata']['cal'][$aktzeile][1]['typ']='All';

			// Zeile x (Spalte 2: "Problem")
			if($problem['id'] < 0)
				$GLOBALS['fxptdata']['cal'][$aktzeile][2]['text']='~';
			else
			{
				$tt_js=str_replace($nl, '<br />', $problem['text']);

				$GLOBALS['fxptdata']['cal'][$aktzeile][2]['align']='center';
				$GLOBALS['fxptdata']['cal'][$aktzeile][2]['tooltip_js']=$tt_js;
 				$GLOBALS['fxptdata']['cal'][$aktzeile][2]['text']='<img src="'.$problem['gfx'].'" style="margin:2px;margin-top:2px;">';
				$GLOBALS['fxptdata']['cal'][$aktzeile][2]['typ']='All';
			}
			$GLOBALS['fxptdata']['cal'][$aktzeile][2]['color']=${'col_bgl'.($aktzeile%2)};

			// Zeile x (Spalte 3: "Info")
			$GLOBALS['fxptdata']['cal'][$aktzeile][3]['text']='<span style="padding:0 2px;">'.fieldLinkImage('info'.$aktzeile, 'icsh', 'loader.php?url=prj_info.inc&pid='.$zeile['projekt_id'], 'Info').'</span>';
			$GLOBALS['fxptdata']['cal'][$aktzeile][3]['align']='right';
			$GLOBALS['fxptdata']['cal'][$aktzeile][3]['color']=${'col_bgl'.($aktzeile%2)};
			$GLOBALS['fxptdata']['cal'][$aktzeile][3]['typ']='All';

			// Tooltip: "Projekt-Name"
			$alt_js='<table border=0 cellpadding=1 cellspacing=0>';
			// Tooltip: "Soll-Zeitraum"
			$sb=get_entry($zeile['zeitdaten']['soll_beginn'], 'datum');
			$se=get_entry($zeile['zeitdaten']['soll_ende'], 'datum');
			$alt_js	.= '<tr><td nowrap valign=top><b>'.$pa_string['szeit'].':</b></td><td nowrap>'.$sb;
			if($se != $sb)
				$alt_js .= '&nbsp;-&nbsp;'.$se;
			$alt_js .= '</td></tr>';
			// Tooltip: "Ist-Zeitraum"
			if($zeile['zeitdaten']['ist_beginn'])
			{
				$ib=get_entry($zeile['zeitdaten']['ist_beginn'], 'datum');
				$ie=get_entry($zeile['zeitdaten']['ist_ende'], 'datum');
				$alt_js	.= '<tr><td nowrap valign=top><b>'.$pa_string['izeit'].':</b></td><td nowrap>'.$ib;
				if($ie != $ib)
					$alt_js .= '&nbsp;-&nbsp;'.$ie;
				$alt_js .= '</td></tr>';
			}
			// Tooltip: "Abhängigkeit"
			if(fxIsArray($abh_sub))
				$alt_js	.= tooltip_dep_sub($zeile['projekt_id'], $abh_sub, true, true);
			// Tooltip: "Meilenstein"
			if(fxIsArray($miles))
			{
				$alt_js	.= '<tr><td nowrap valign=top><b>'.$GLOBALS['txt_meilenstein'].':</b></td><td nowrap>';
				foreach($miles as $mc => $md)
					$alt_js .= getMstName($md).'<br>';
				$alt_js .= '</td></tr>';
			}
			// Tooltip: "Projekt-Dauer"
			if($psb || $pse)
			{
				$tdiff=get_entry(1+fxp_date_diff($pdv, $pdb, 'D'), 'dezimal');
				if($zeitanzeige == STUNDEN)
					$tdiff=get_entry($tdiff*24, 'dezimal');
				else if($zeitanzeige == MONATE)
					$tdiff=get_entry($tdiff/30, 'dezimal');
				else if($zeitanzeige == JAHRE)
					$tdiff=get_entry($tdiff/365, 'dezimal');
				$alt_js	.= '<tr><td nowrap valign=top><b>'.$pa_string['pdauer'].':</b></td><td nowrap>'.$tdiff.'&nbsp;'.$pa_string[$zeitanzeige].'</td></tr>';
			}
			// Tooltip: "Projekt-Status"
			$alt_js	.= '<tr><td nowrap valign=top><b>'.$pa_string['pstatus'].':</b></td><td nowrap>'.$pa_string[$zeile['projekt_status']].'</td></tr>';
			// Tooltip: "Projektkategorie"
			if(isset($kar[$ci]))
			{
				$ct='';
				if(strlen((string)$kar[$ci]['col']))
					$ct='<span style=border-radius:4px;background:'.$kar[$ci]['bg'].';>&nbsp;&nbsp;</span>&nbsp;';
				$ct .= $kar[$ci]['txt'];
				$alt_js	.= '<tr><td nowrap><b>'.$pa_string['pkategorie'].':</b></td><td nowrap>'.$ct.'</td></tr>';
			}
			$alt_js	.= '</table>';

			// JavaScript
			$alt_js="<div class=rhd>".no_null($zeile['vorgangsnummer']).'&nbsp;-&nbsp;'.strtr(str_replace(' ', '&nbsp;', $zeile['name_projekt']), $GLOBALS['caption_strtr']).'</div>'.$alt_js;
//echo('<PRE>'.fxHtmlEncode($alt_js).'</PRE>');

			// Zeile x (Gantt-Balken)
			// ...Soll-Beginn
			$zc=$zeile['zeitdaten']['soll_beginn'];
			if(isset($GLOBALS['oarr'][$zc]))
			{
				$ppl=true;
				$isb=$GLOBALS['xoffset'] + $GLOBALS['oarr'][$zc];
			}
			else
			{
				$ppl=false;
				if($zc < $GLOBALS['von_min'])
					$isb=$GLOBALS['xoffset'];
				else
					$isb=0;
			}

			// ...Soll-Ende
			$zc=$zeile['zeitdaten']['soll_ende'];
			if(isset($GLOBALS['oarr'][$zc]))
			{
				$ise=$GLOBALS['xoffset'] + $GLOBALS['oarr'][$zc];
				$ppr=true;
			}
			else
			{
				$ppr=false;
				if($zc > $GLOBALS['bis_max'])
					$ise=$GLOBALS['xcolumns']-1;
				else
					$ise=0;
			}

			// ...Ist-Beginn
			$zc=$zeile['zeitdaten']['ist_beginn'];
			$iib=0;
			$pil=false;
			if($zc)
			{
				if(isset($GLOBALS['oarr'][$zc]))
				{
					$pil=true;
					$iib=$GLOBALS['xoffset'] + $GLOBALS['oarr'][$zc];
				}
				else if($zc < $GLOBALS['von_min'])
					$iib=$GLOBALS['xoffset'];
			}

			// ...Ist-Ende
			$zc=$zeile['zeitdaten']['ist_ende'];
			$iie=0;
			$pir=false;
			if($zc)
			{
				if(isset($GLOBALS['oarr'][$zc]))
				{
					$iie=$GLOBALS['xoffset'] + $GLOBALS['oarr'][$zc];
					$pir=true;
				}
				else if($zc > $GLOBALS['bis_max'])
					$iie=$GLOBALS['xcolumns']-1;
			}
//echo(__LINE__.': <b>$isb='.$isb.'</b> ($ppl='.($ppl?'true':'false').'), <b>$ise='.$ise.'</b> ($ppr='.($ppr?'true':'false').'), <b>$iib='.$iib.'</b> ($pil='.($pil?'true':'false').'), <b>$iie='.$iie.'</b> ($pir='.($pir?'true':'false').')<hr>');

			// Zeitstrahl
			$zs=array();
			// ...leer vorbereiten
			for($zsi=$GLOBALS['xoffset']; $zsi<$GLOBALS['xcolumns']; $zsi++)
				$zs[$zsi]=0;
			// ...Plandaten hinzufügen
			for($zsi=$isb; $zsi<=$ise; $zsi++)
			{
				if(isset($zs[$zsi]))
					$zs[$zsi]=1;
			}
			// ...Istdaten hinzufügen
			if($iib)
			{
				for($zsi=$iib; $zsi<=$iie; $zsi++)
				{
					if(isset($zs[$zsi]))
					{
						if($zs[$zsi])
							$zs[$zsi]=3;
						else
							$zs[$zsi]=2;
					}
				}
			}
//fxDebug($zs, '$zs');

			$zeit=array();
			$md=-1;
			$zc=-1;
			foreach($zs as $zo => $zm)
			{
				if($zm != $md)
				{
					$zc++;
					$zeit[$zc]=array('von'=>$zo, 'bis'=>$zo, 'md'=>$zm);
					$md=$zm;
				}
				else
					$zeit[$zc]['bis']=$zo;
			}
//fxDebug($zeit, '$zeit');

			// Balken zeichnen
			$so_zeit=sizeof($zeit) - 1;
			$mode_prev=0;
			$name_set=false;
			$panchor=false;
			foreach($zeit as $zc => $zeitabschnitt)
			{
				$aktspalte=$zeitabschnitt['von'];
				$mode=$zeitabschnitt['md'];
				$colspan=1+$zeitabschnitt['bis']-$zeitabschnitt['von'];

				if($zc == $so_zeit)
					$mode_next=0;
				else
					$mode_next=$zeit[$zc+1]['md'];
//echo('$mode_prev='.$mode_prev.', $mode='.$mode.', $mode_next='.$mode_next.'<br>');

				// Colspan
				$GLOBALS['fxptdata']['cal'][$aktzeile][$aktspalte]['colspan']=$colspan;

				if($mode > 0)
				{
					$GLOBALS['fxptdata']['cal'][$aktzeile][$aktspalte]['bgp']='gfxpath';
					$GLOBALS['fxptdata']['cal'][$aktzeile][$aktspalte]['background']='g_bg'.$GLOBALS['kb'];

					// Sollbalken: Projekt oder Aufgabe (evtl. in der Projektkategoriefarbe)
					$pt='project';
					$st='0';
					$ph=$lph;
					$nh=$lph+3;
					$nw=3;
					if($zeile['darstellung'] == 'au')
					{
						$pt='task';
						$st=$zeile['projekt_status'];
						$ph=$lth;
						$nh=$lth;
						$nw=7;
					}
					$cc='';
					$cb='';
					if(isset($kar[$ci]) && strlen((string)$kar[$ci]['col']))
					{
						$cc=$kar[$ci]['col'];
						$cb=$kar[$ci]['bg'];
					}
//fxDebug($cc,'$ci='.$ci);
					$pkl=false;
					$pkr=false;
					$ikl=false;
					$ikr=false;

					if(!isset($GLOBALS['fxptdata']['cal'][$aktzeile][0]['pt']))
					{
						$GLOBALS['fxptdata']['cal'][$aktzeile][0]['pt']=$pt;
						$GLOBALS['fxptdata']['cal'][$aktzeile][0]['cc']=$cc;
						$GLOBALS['fxptdata']['cal'][$aktzeile][0]['cb']=$cb;
					}

					$GLOBALS['fxptdata']['cal'][$aktzeile][$aktspalte]['text']='~<div style="position:relative;left:0;top:1px;width:100%;height:28px;">';
					if($mode != 2)	// Nur Sollbalken?
					{
						if($ppl && (!$mode_prev || ($mode_prev == 2)))
							$pkl=true;
						if($ppr && (!$mode_next || ($mode_next == 2)))
							$pkr=true;

						$brd='';
						if(!$pkl || !$pkr)
						{
							if(!$pkl)
								$brd .= 'border-top-left-radius:0;border-bottom-left-radius:0;';
							if(!$pkr)
								$brd .= 'border-top-right-radius:0;border-bottom-right-radius:0;';
						}

						$GLOBALS['fxptdata']['cal'][$aktzeile][$aktspalte]['text'] .= '<div class="b'.$pt.$cc.'" style="position:absolute;left:0;top:0;width:100%;height:'.$ph.'px;'.$brd.'"></div>';

						// Name
						if(!$name_set)
						{
							$GLOBALS['fxptdata']['cal'][$aktzeile][$aktspalte]['text'] .= '<div style="position:absolute;left:8px;top:1px;width:100%;z-index:2;">';
							if($pt == 'project')
								$GLOBALS['fxptdata']['cal'][$aktzeile][$aktspalte]['text'] .= '<b>';
							$GLOBALS['fxptdata']['cal'][$aktzeile][$aktspalte]['text'] .= $zeile['name_projekt'];
							if($pt == 'project')
								$GLOBALS['fxptdata']['cal'][$aktzeile][$aktspalte]['text'] .= '</b>';
							$GLOBALS['fxptdata']['cal'][$aktzeile][$aktspalte]['text'] .= '</div>';
							$name_set=true;
						}

						// ...Planknoten links
						if($pkl)
						{
							$pkl=true;
							$GLOBALS['fxptdata']['cal'][$aktzeile][$aktspalte]['text'] .= '<div class="st'.$st.'" style="position:absolute;left:0;top:0;width:'.$nw.'px;height:'.$nh.'px;border-top-right-radius:0;border-bottom-right-radius:0;z-index:3;"></div>';
							$GLOBALS['fxptdata']['cal'][$aktzeile][0]['sk']=$aktspalte;
						}

						// ...Planknoten rechts
						if($pkr)
						{
							$GLOBALS['fxptdata']['cal'][$aktzeile][$aktspalte]['text'] .= '<div class="st'.$st.'" style="position:absolute;right:0;top:0;width:'.$nw.'px;height:'.$nh.'px;border-top-left-radius:0;border-bottom-left-radius:0;z-index:3;"></div>';
							$GLOBALS['fxptdata']['cal'][$aktzeile][0]['ek']=$aktspalte+$colspan-1;
						}

						// Abhängigkeiten links (beim Sollstartknoten)
						if($pkl && fxIsArray($abh_sub) && !$abh)
						{
							// Abhängigkeiten vom Typ AA oder AE
							foreach($abh_sub as $a)
							{
								if(($a['projekt_a'] == $zeile['projekt_id']) && (substr((string)$a['abh_typ'],0,1) == 'S'))
								{
									$GLOBALS['fxptdata']['cal'][$aktzeile][$aktspalte]['text'] .= '<img src="'.$GLOBALS['gfxpath'].'dep_fs.png" tooltip="'.str_replace('"', '&quot;', no_null($a['nr_a']).': '.$a['name_a'].' -> '.$GLOBALS['dtypes'][$GLOBALS['fxpglobals']['lang']][$a['abh_typ']].' -> '.no_null($a['nr_b']).': '.$a['name_b']).'" style="position:absolute;left:-16px;top:3px;">';
									$GLOBALS['dep_cnt']++;

									// Informationen für Druckaufbereitung
									if(!fxIsArray($GLOBALS['fxptdata']['cal'][$aktzeile][0]['dep']))
										$GLOBALS['fxptdata']['cal'][$aktzeile][0]['dep']=array();
									$ac=sizeof($GLOBALS['fxptdata']['cal'][$aktzeile][0]['dep']);
									$GLOBALS['fxptdata']['cal'][$aktzeile][0]['dep'][$ac]=$a;
									$GLOBALS['fxptdata']['cal'][$aktzeile][0]['dep'][$ac]['col']=$aktspalte;
//fxDebug($GLOBALS['fxptdata']['cal'][$aktzeile][0]['dep'], 'dep');
								}

								if(($a['projekt_b'] == $zeile['projekt_id']) && (substr((string)$a['abh_typ'],1,1) == 'S'))
								{
									$GLOBALS['fxptdata']['cal'][$aktzeile][$aktspalte]['text'] .= '<img src="'.$GLOBALS['gfxpath'].'depf_ts.png" tooltip="'.str_replace('"', '&quot;', no_null($a['nr_a']).': '.$a['name_a'].' -> '.$GLOBALS['dtypes'][$GLOBALS['fxpglobals']['lang']][$a['abh_typ']].' -> '.no_null($a['nr_b']).': '.$a['name_b']).'" style="position:absolute;left:-16px;top:3px;">';
									$GLOBALS['dep_cnt']++;
								}
							}
						}

						// Abhängigkeiten rechts (beim Sollendknoten)
						if($pkr && fxIsArray($abh_sub))
						{
							// Abhängigkeiten vom Typ EE oder EA
							foreach($abh_sub as $a)
							{
								if(($a['projekt_a'] == $zeile['projekt_id']) && (substr((string)$a['abh_typ'],0,1) == 'F'))
								{
									$GLOBALS['fxptdata']['cal'][$aktzeile][$aktspalte]['text'] .= '<img src="'.$GLOBALS['gfxpath'].'dep_ff.png" tooltip="'.str_replace('"', '&quot;', no_null($a['nr_a']).': '.$a['name_a'].' -> '.$GLOBALS['dtypes'][$GLOBALS['fxpglobals']['lang']][$a['abh_typ']].' -> '.no_null($a['nr_b']).': '.$a['name_b']).'" style="position:absolute;right:-16px;top:3px;">';
									$GLOBALS['dep_cnt']++;

									// Informationen für Druckaufbereitung
									if(!fxIsArray($GLOBALS['fxptdata']['cal'][$aktzeile][0]['dep']))
										$GLOBALS['fxptdata']['cal'][$aktzeile][0]['dep']=array();
									$ac=sizeof($GLOBALS['fxptdata']['cal'][$aktzeile][0]['dep']);
									$GLOBALS['fxptdata']['cal'][$aktzeile][0]['dep'][$ac]=$a;
									$GLOBALS['fxptdata']['cal'][$aktzeile][0]['dep'][$ac]['col']=$aktspalte;
//fxDebug($GLOBALS['fxptdata']['cal'][$aktzeile][0]['dep'], 'dep');
								}

								if(($a['projekt_b'] == $zeile['projekt_id']) && (substr((string)$a['abh_typ'],1,1) == 'F'))
								{
									$GLOBALS['fxptdata']['cal'][$aktzeile][$aktspalte]['text'] .= '<img src="'.$GLOBALS['gfxpath'].'depf_tf.png" tooltip="'.str_replace('"', '&quot;', no_null($a['nr_a']).': '.$a['name_a'].' -> '.$GLOBALS['dtypes'][$GLOBALS['fxpglobals']['lang']][$a['abh_typ']].' -> '.no_null($a['nr_b']).': '.$a['name_b']).'" style="position:absolute;right:-16px;top:3px;">';
									$GLOBALS['dep_cnt']++;
								}
							}
						}
					}

					// Istbalken
					if($mode > 1)
					{
						$ibt=0;
						$GLOBALS['fxptdata']['cal'][$aktzeile][$aktspalte]['text'] .= '<div style="position:absolute;left:0;top:'.$ibt.'px;background:url('.$GLOBALS['gfxpath'].'g_ad.png);width:100%;height:3px;"></div>';

						// ...Istknoten links
						if($pil && ($mode_prev < 2))
						{
							$ikl=true;
							$GLOBALS['fxptdata']['cal'][$aktzeile][$aktspalte]['text'] .= '<img src="'.$GLOBALS['gfxpath'].'g_ad_node.png" style="position:absolute;left:0;top:'.($ibt-2).'px;">';
						}

						// ...Istknoten rechts
						if($pir && ($mode_next < 2))
						{
							$ikr=true;
							$GLOBALS['fxptdata']['cal'][$aktzeile][$aktspalte]['text'] .= '<img src="'.$GLOBALS['gfxpath'].'g_ad_node.png" style="position:absolute;right:0;top:'.($ibt-2).'px;">';
						}
					}

					$GLOBALS['fxptdata']['cal'][$aktzeile][$aktspalte]['text'] .= '</div>';

					$GLOBALS['fxptdata']['cal'][$aktzeile][$aktspalte]['pkl']=$pkl;
					$GLOBALS['fxptdata']['cal'][$aktzeile][$aktspalte]['pkr']=$pkr;
					$GLOBALS['fxptdata']['cal'][$aktzeile][$aktspalte]['ikl']=$ikl;
					$GLOBALS['fxptdata']['cal'][$aktzeile][$aktspalte]['ikr']=$ikr;

					// Align
					$GLOBALS['fxptdata']['cal'][$aktzeile][$aktspalte]['align']='left" valign="top';

					// Tooltip nur, wenn "echtes" Recht auf Projekt
					$GLOBALS['fxptdata']['cal'][$aktzeile][$aktspalte]['tooltip_js']=$alt_js;

					// Link
					if($rights_23)
						$GLOBALS['fxptdata']['cal'][$aktzeile][$aktspalte]['link']='?tr=23&aktion=1&gproject='.$zeile['projekt_id'];

					// Anker
					if(!$panchor)
					{
						$GLOBALS['fxptdata']['cal'][$aktzeile][$aktspalte]['anchor']=true;
						$panchor=true;
					}

					// Status
					$GLOBALS['fxptdata']['cal'][$aktzeile][$aktspalte]['status']=$zeile['projekt_status'];

					// Mode
					$GLOBALS['fxptdata']['cal'][$aktzeile][$aktspalte]['mode']=$mode_prev.$mode.$mode_next;
				}
				else
					$GLOBALS['fxptdata']['cal'][$aktzeile][$aktspalte]['color']=$col_bgr;

				$mode_prev=$mode;
			}
			$aktzeile++;

			// Meilensteine
			if(fxIsArray($miles))
			{
//fxDebug($miles,'$miles');
				$disp='';
				if($GLOBALS['Projektart'] == 1)
					$disp='none';

				foreach($miles as $mc => $md)
				{
					$mdt=substr((string)$md['soll_ende'],0,8);
					if(!isset($GLOBALS['oarr'][$mdt]))
						continue;

					$GLOBALS['mst_cnt']++;

					// Zeile x (Spalte 0: "Projekt-Name")
					$mi='mst';
					if(!$md['statusfeld'])
						$mi .= 'f';
					$mn0=getMstName($md,false);
					$mn1=getMstName($md,true);
					$GLOBALS['fxptdata']['cal'][$aktzeile][0]['text']  = '<img src="'.$GLOBALS['gfxpath'].'dmy.png" width="13" height="1">'.$sublevel_next.'&middot;&nbsp;<font color="#987300">'.$mn0.'</font>&nbsp;';
					$GLOBALS['fxptdata']['cal'][$aktzeile][0]['text'] .= fieldHidden('disp-'.$md['meilenstein_id'],$disp.'_').fieldHidden('pr['.$aktzeile.']',-$md['meilenstein_id']).fieldHidden('vn['.$aktzeile.']',$zeile['vorgangsnummer'].'.'.substr('0000'.$mc,-4));
					$GLOBALS['fxptdata']['cal'][$aktzeile][0]['color']=${'col_bgl'.($aktzeile%2)};
					$GLOBALS['fxptdata']['cal'][$aktzeile][0]['disp']=$disp;
					$GLOBALS['fxptdata']['cal'][$aktzeile][0]['typ']='All';
					$GLOBALS['fxptdata']['cal'][$aktzeile][0]['pid']=-$md['meilenstein_id'];
					// Zeile x (Spalte 1: "Projekt-Dauer")
					$GLOBALS['fxptdata']['cal'][$aktzeile][1]['text']='<i style="color:#d09300;">'.get_entry($md['soll_ende'],'datum').'</i>';
					$GLOBALS['fxptdata']['cal'][$aktzeile][1]['color']=${'col_bgl'.($aktzeile%2)};
					$GLOBALS['fxptdata']['cal'][$aktzeile][1]['align']='center';
					$GLOBALS['fxptdata']['cal'][$aktzeile][1]['typ']='All';
					// Zeile x (Spalte 2: "Problem")
					$GLOBALS['fxptdata']['cal'][$aktzeile][2]['text']='&nbsp;';
					$GLOBALS['fxptdata']['cal'][$aktzeile][2]['color']=${'col_bgl'.($aktzeile%2)};
					$GLOBALS['fxptdata']['cal'][$aktzeile][2]['typ']='All';
					// Zeile x (Spalte 3: "Info")
					$GLOBALS['fxptdata']['cal'][$aktzeile][3]['text']='&nbsp;';
					$GLOBALS['fxptdata']['cal'][$aktzeile][3]['color']=${'col_bgl'.($aktzeile%2)};
					$GLOBALS['fxptdata']['cal'][$aktzeile][3]['typ']='All';

					// Zeile x (Gantt-Balken)
					$xc=$GLOBALS['xoffset'];
					$xo=$GLOBALS['xoffset']+$GLOBALS['oarr'][$mdt];
					// ...Leerraum links
					if($xo > $GLOBALS['xoffset'])
					{
						$GLOBALS['fxptdata']['cal'][$aktzeile][$xc]['colspan']=$xo-$GLOBALS['xoffset'];
						$GLOBALS['fxptdata']['cal'][$aktzeile][$xc]['color']=$col_bgr;
						$xc += $xo-$GLOBALS['xoffset'];
					}
					// ...Meilenstein
					$mp='';
					if($md['statusfeld'])
					{
						if($md['statusfeld'] < 0)
							$px=$isb;
						else
							$px=$ise;
						$pw=$GLOBALS['fxptdata']['cal'][2][$px]['width'];
						if(!$pw)
							$pw=23;
						else
							$pw++;
						$pw2=floor($pw/2);
//echo(__LINE__.': <b>'.$mn0.'</b> -- $aktzeile='.$aktzeile.' ($pw='.$pw.', $pw2='.$pw2.'), $xc='.$xc.' -- $isb='.$isb.', $ise='.$ise.' -- $ppl='.$ppl.', $ppr='.$ppr.' -- $px='.$px.'<br />');
						$mpb=6;
						if((($md['statusfeld'] < 0) && $ppl) || (($md['statusfeld'] > 0) && $ppr))
							$mpt=-$lh*$mc-18;
						else
							$mpt=$mpb;
						if($px == $xc)
						{
							$mpl=$pw2;
							$mpr=$pw2;
						}
						else
						{
							$brd='border-bottom:1px dashed #d09300;';
							if($px < $xc)
							{
								$mpl=$pw2-($xc-$px)*$pw;
								$mpr=$pw2;
							}
							else
							{
								$mpl=$pw2;
								$mpr=$pw2+($px-$xc)*$pw;
							}
						}
						if($mpt < $mpb)
						{
							if($px < $xc)
								$brd .= 'border-left:1px dashed #d09300;';
							else
								$brd .= 'border-right:1px dashed #d09300;';
						}
						$mp='<div style="position:absolute;left:'.$mpl.'px;top:'.$mpt.'px;width:'.($mpr-$mpl).'px;height:'.($mpb-$mpt).'px;'.$brd.'z-index:1;"></div>';
					}
					$tt=' tooltip="'.$mn1.'"';
					$GLOBALS['fxptdata']['cal'][$aktzeile][$xc]['text']  = '<div style="position:relative;height:'.($lh-2).'px;cursor:help;">';
					$GLOBALS['fxptdata']['cal'][$aktzeile][$xc]['text'] .= '<img src="'.$GLOBALS['gfxpath'].$mi.'.png" width="22" height="22"'.$tt.' style="position:absolute;left:'.($pw2-11).'px;top:4px;z-index:2;">';
					$GLOBALS['fxptdata']['cal'][$aktzeile][$xc]['text'] .= '<div class="s2"'.$tt.' style="position:absolute;left:'.($pw2+11).'px;top:7px;color:#987300;z-index:2;">'.$mn0.'</div>';
					$GLOBALS['fxptdata']['cal'][$aktzeile][$xc]['text'] .= $mp;
					$GLOBALS['fxptdata']['cal'][$aktzeile][$xc]['text'] .= '</div>';
					$GLOBALS['fxptdata']['cal'][$aktzeile][$xc]['color']=$col_bgr;
					$GLOBALS['fxptdata']['cal'][$aktzeile][$xc]['typ']='All';
					// ...Informationen für Druckaufbereitung
					$GLOBALS['fxptdata']['cal'][$aktzeile][0]['mst'][$mc]=array('name'=>$mn1, 'col'=>$xc, 'type'=>$md['statusfeld'], 'days'=>$md['tagesanzahl'], 'sb'=>$isb, 'se'=>$ise, 'ppl'=>$ppl, 'ppr'=>$ppr, 'px'=>$px);
					$xc++;
					// ...Leerraum rechts
					if($xc < $GLOBALS['xcolumns'])
					{
						$GLOBALS['fxptdata']['cal'][$aktzeile][$xc]['colspan']=$GLOBALS['xcolumns']-$xc;
						$GLOBALS['fxptdata']['cal'][$aktzeile][$xc]['color']=$col_bgr;
					}

					$aktzeile++;
				}
			}
		}

		// Unterprojekte rekursiv wieder aufrufen
		if($zeile['sublevel'])
			projektzeile($zeile['sublevel'], $aktzeile, $sublevel_next);
	}
}

/**
 * tooltip_dep_sub()
 *
 * @param various $pid     - Mandatory parameter:                   ???PARAMETER???
 * @param various $abh_sub - Mandatory parameter:                   ???PARAMETER???
 * @param boolean $tip     - Optional parameter (default = false):  ???PARAMETER???
 * @param boolean $js      - Optional parameter (default = false):  ???PARAMETER???
 */
function tooltip_dep_sub($pid, $abh_sub, $tip=false, $js=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	global $pa_string, $nl;

	if($js)
		$tt='<tr>';
	else
		$tt='';
	if($tip)
	{
		if($js)
			$tt .= '<td nowrap><b>'.$pa_string['abh'].':</b></td>';
		else
			$tt .= $pa_string['abh'].':	';
	}
	else if($js)
		$tt .= '<td nowrap><b>'.$pa_string['abh_sub'].':</b></td></tr>';
	else
		$tt .= $pa_string['abh_sub'].':'.$nl;

	$apnl=false;
	foreach($abh_sub as $ap)
	{
		if($apnl)
		{
			if($tip)
			{
				if($js)
					$tt .= '<tr><td></td>';
				else
					$tt .= $nl.'		';
			}
			else if($js)
				$tt .= '<tr>';
			else
				$tt .= $nl;
		}
		else
			$apnl=true;
		if($js)
		{
			if(!$tip)
				$tt .= '<tr>';
			$tt .= '<td nowrap>';
		}
		if($ap['projekt_a'] == $pid)
			$tt .= '*';
		else
			$tt .= no_null($ap['nr_a']).':&nbsp;'.$ap['name_a'];
		$tt .= '&nbsp;&nbsp;->&nbsp;'.$GLOBALS['dtypes'][$GLOBALS['fxpglobals']['lang']][$ap['abh_typ']].'&nbsp;->&nbsp;&nbsp;';
		if($ap['projekt_b'] == $pid)
			$tt .= '*';
		else
			$tt .= no_null($ap['nr_b']).':&nbsp;'.$ap['name_b'];
		if($js)
			$tt .= '</td></tr>';
	}

	return($tt);
}

/**
 * ist_problem()
 *
 * @param various $zeile - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function ist_problem($zeile)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	global $problemtexte, $nl;

	$ptext=array('id'=>-1, 'gfx'=>$GLOBALS['gfxpath'].'1.png', 'text'=>'');

	// abgeschl. (301) Projekte werden bei der Problemsuche nicht berücksichtigt
	if($zeile['projekt_status'] == 301)
		return $ptext;

	$ptext['gfx']='';

	// Aufwand
	if((double)$zeile['aufwand_ist'] > (double)$zeile['aufwand_soll'])
	{
		$trans_table=array('$var1' => get_entry($zeile['aufwand_ist']/3600, 'dezimal').'&nbsp;h', '$var2' => get_entry($zeile['aufwand_soll']/3600, 'dezimal').'&nbsp;h');
		if(strlen((string)$ptext['gfx']))
			$ptext['text'] .= $nl.$nl;
		else
			$ptext['gfx']=$GLOBALS['gfxpath'].'c_abs.png';
		$ptext['text'] .= strtr($problemtexte[1289], $trans_table); // Der tatsächliche Aufwand ($var1) ist grösser als der geplante Aufwand ($var2)!
	}

	// Ist-Beginn
	if($zeile['zeitdaten']['ist_beginn'])
	{
		if($zeile['zeitdaten']['ist_beginn'] > $zeile['zeitdaten']['soll_ende'])
		{
			if(strlen((string)$ptext['gfx']))
				$ptext['text'] .= $nl.$nl;
			else
				$ptext['gfx']=$GLOBALS['gfxpath'].'c_time.png';
			$ptext['text'] .= $problemtexte[1290]; // Der tatsächliche Projektbeginn liegt über dem geplanten Projektende!
		}
		$diff=fxp_date_diff($zeile['zeitdaten']['soll_beginn'], $zeile['zeitdaten']['ist_beginn'], 'D');
		if($diff > $GLOBALS['fxpglobals']['settings']['p_toleranz'])
		{
			$trans_table=array('$var1' => $diff);
			if(strlen((string)$ptext['gfx']))
				$ptext['text'] .= $nl.$nl;
			else
				$ptext['gfx']=$GLOBALS['gfxpath'].'c_time.png';
			$ptext['text'] .= strtr($problemtexte[1291], $trans_table); // Der tatsächliche Projektbeginn liegt $var1 Tag(e) über dem geplanten Projekstart!
		}
	}

	// akt. Datum
	if($GLOBALS['today'] > $zeile['zeitdaten']['soll_ende'])
	{
		if(strlen((string)$ptext['gfx']))
			$ptext['text'] .= $nl.$nl;
		else
			$ptext['gfx']=$GLOBALS['gfxpath'].'c_msg.png';
 		if($zeile['zeitdaten']['ist_beginn'])
			$ptext['text'] .= $problemtexte[1292]; // Das geplante Projektende wurde bereits überschritten. Die Aufgabe ist jedoch noch nicht fertiggestellt!
		else
			$ptext['text'] .= $problemtexte[1293]; // Das geplante Projektende wurde bereits überschritten. Die Aufgabe wurde jedoch noch nicht begonnen!
	}
	else
	{
		$diff=fxp_date_diff($zeile['zeitdaten']['soll_beginn'], $GLOBALS['today'], 'D');
		if($diff > $GLOBALS['fxpglobals']['settings']['p_toleranz'])
		{
	 		$trans_table=array('$var1' => $diff);
			if($zeile['zeitdaten']['ist_beginn'])
			{
				if($zeile['zeitdaten']['ist_beginn'] >= $zeile['zeitdaten']['soll_ende'])
				{
					if(strlen((string)$ptext['gfx']))
						$ptext['text'] .= $nl.$nl;
					else
						$ptext['gfx']=$GLOBALS['gfxpath'].'c_time.png';
					$ptext['text'] .= strtr($problemtexte[1294], $trans_table); // Es liegen bereits $var1 Tag(e) über dem geplanten Projektstart!
				}
			}
			else
			{
				if(strlen((string)$ptext['gfx']))
					$ptext['text'] .= $nl.$nl;
				else
					$ptext['gfx']=$GLOBALS['gfxpath'].'c_time.png';
				$ptext['text'] .= strtr($problemtexte[1295], $trans_table); // Es liegen bereits $var1 Tag(e) über dem geplanten Projektstart. Die Aufgabe wurde jedoch noch nicht begonnen!
			}
		}
	}

	// Keine Probleme
	if(!strlen((string)$ptext['gfx']))
	{
		$ptext['id']=0;
		$ptext['gfx']=$GLOBALS['gfxpath'].'1.png';
		$ptext['text']='OK.';
	}
	else
		$ptext['id']=1;

	return($ptext);
}

/**
 * ???FUNCTION???
 *
 * @param various $md        - Mandatory parameter:                  ???PARAMETER???
 * @param boolean $with_date - Optional parameter (default = true):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function getMstName($md, $with_date=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$mname='';
	if($with_date)
		$mname .= get_entry($md['soll_ende'], 'datum').': ';
	$mname .= str_replace('"', '&quot;', $md['name_meilenstein']).' (';
	if(!$md['statusfeld'])
		$mname .= 'F';
	else
	{
		if($md['statusfeld'] < 0)
			$mname .= 'S';
		else
			$mname .= 'E';
		if($md['tagesanzahl'])
		{
			if($md['tagesanzahl'] > 0)
				$mname .= '+';
			$mname .= $md['tagesanzahl'];
		}
	}
	$mname .= ')';

	return $mname;
}
?>